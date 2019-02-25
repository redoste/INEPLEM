#include <iostream>
#include <cstdint>
#include <rfb/rfb.h>

#include "dh.h"
extern "C"{
	#include "d3des.h"
}
#include "msAuth.h"
#include "serviceCore.h"

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

	// On accepte la connection
	uint32_t result = 0;
	rfbWriteExact(client, (char*) &result, 4);
	client->state = rfbClientRec::RFB_INITIALISATION;
}

/* msAuthIIRegisterSecurity: Enregistre le securityHandler pour msAuthII au près de la libvncserver
 * retourne un rfbSecurityHandler*: le pointeur vers le securityHandler enregistré
 */
rfbSecurityHandler* msAuthIIRegisterSecurity(){
	// On le new car sinon il est "déalloué" à la fin de la fonction => SegFault
	rfbSecurityHandler *msAuthIISecurityHandler = new rfbSecurityHandler;
	memset(msAuthIISecurityHandler, 0, sizeof(rfbSecurityHandler));
	msAuthIISecurityHandler->type = VNC_MSAUTH;
	msAuthIISecurityHandler->handler = &msAuthIIHandler;

	rfbRegisterSecurityHandler(msAuthIISecurityHandler);
	return msAuthIISecurityHandler;
}