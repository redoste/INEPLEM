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
#include <rfb/rfb.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <map>

#include "qt.h"
#include "italcExtension.h"
#include "serviceCore.h"

// italcExtensionServiceCorePtr: Pointeur vers le serviceCore, utilisé par italcResponseUserInformation pour connaitre le nom d'utilisateur
ServiceCore *italcExtensionServiceCorePtr = NULL;

/* italcExtensionHandleClient: Fonction indiquant si l'extension italc doit être chargé pour se client
 * Retourne toujours le rfbBool TRUE
 */
rfbBool italcExtensionHandleClient(rfbClientRec *client, void **){
	std::string consoleOut("[italcExtensionHandleClient] New client: ");
	std::string address(client->host);
	consoleOut += address;
	std::cout << consoleOut << std::endl;
	italcExtensionServiceCorePtr->clientSeen(address);

	return TRUE;
}

/* italcExtensionHandleMessage: Appelé lorsqu'un nouveau message est reçu par la libvnc
 * rfbClientRec *client: information du client ayant envoyé ce message
 * const rfbClientToServerMsg *message: information du message
 * retourne un rfbBool: FALSE si le message ,'a pas été traité, TRUE sinon
 */
rfbBool italcExtensionHandleMessage(rfbClientRec *client, void *, const rfbClientToServerMsg *message){
	// On ne gère que les message Italc (0x28 ou 40)
	if(message->type != 0x28) return FALSE;

	std::cout << "[italcExtensionHandleMessage] New italc command" << std::endl;
	std::string command = recvQString(client);
	uint32_t argsLength = recvQInt(client);
	std::map<std::string, std::string> args;

	// Sécurité si trop d'argument sont passé, le maximum théorique est 4
	if(argsLength > 15){
		std::cerr << "[italcExtensionHandleMessage] Too much args" << std::endl;
		args["TooMuchArgs"] = std::to_string(argsLength);
	}
	else if(argsLength > 0){
		for(uint32_t i = 0; i < argsLength; i++){
			std::string key = recvQString(client);
			uint32_t argType = recvQVariantHeader(client);
			// Normalement tous les arguments sont des QSTRING
			if(argType == QVARIANT_STRING){
				std::string arg = recvQString(client);
				args[key] = arg;
			}
			else{
				std::cerr << "[italcExtensionHandleMessage] Unknown Italc args type: " << argType << std::endl;
			}
		}
	}

	// Information via les logs
	std::string consoleOut("[italcExtensionHandleMessage] Recived Italc Message:\n");
	consoleOut += "[italcExtensionHandleMessage]     cmd: " + command + "\n";
	if(argsLength > 0){
		for(std::map<std::string, std::string>::iterator it = args.begin(); it != args.end(); it++){
			consoleOut += "[italcExtensionHandleMessage]     " + it->first + " => " + it->second + "\n";
		}
	}
	std::cout << consoleOut;

	if(command == "GetUserInformation"){
		italcResponseUserInformation(client);
	}
	else{
		italcExtensionServiceCorePtr->getServiceToUi()->broadcastNotification(consoleOut);
	}
	return TRUE;
}

/* italcExtensionRegister: Enregistre l'extension italc au près de la libvnc
 * Ne prend aucun paramètre et n'a aucun retour
 */
void italcExtensionRegister(ServiceCore *service){
	// On le new car sinon il est "déalloué" à la fin de la fonction => SegFault
	rfbProtocolExtension *italcExtension = new rfbProtocolExtension;
	memset(italcExtension, 0, sizeof(rfbProtocolExtension));
	italcExtension->handleMessage = &italcExtensionHandleMessage;
	italcExtension->newClient = &italcExtensionHandleClient;

	rfbRegisterProtocolExtension(italcExtension);

	italcExtensionServiceCorePtr = service;
}

/* italcResponseUserInformation: Envois un message UserInformation si nécéssaire
 * rfbClientRec *client: Pointeur vers les informations du client VNC
 */
void italcResponseUserInformation(rfbClientRec *client){
	std::string username;
	if(italcExtensionServiceCorePtr->getUsernameNull() == 0){
		username = italcExtensionServiceCorePtr->getUsername();
	}
	else{
		username = "";
	}

	// Information que c'est un message Italc
	char italcMessage[] = {0x28};
	rfbWriteExact(client, italcMessage, 1);

	sendQString(client, "UserInformation"); // Message "UserInformation"
	sendQInt(client, 2); // Avec 2 arguments

	sendQString(client, "username"); // Arguments "username"
	sendQVariantHeader(client, QVARIANT_STRING);
	sendQString(client, username);

	sendQString(client, "homedir"); // Arguments "homedir"
	sendQVariantHeader(client, QVARIANT_STRING);
	sendQString(client, "/dev/null"); // Il n'est pas utilisé par iTalc, on lui envois donc une valeur innutile

	std::cout << "[italcResponseUserInformation] Sent user information: " << username << std::endl;
}

/* italcResponseUserInformationWithService: Appelle italcResponseUserInformation et définis italcExtensionServiceCorePtr pour éviter tous segfault
 * rfbClientRec *client: client a passer à italcResponseUserInformation
 * ServiceCore *service: valeur de italcExtensionServiceCorePtr
 */
void italcResponseUserInformationWithService(rfbClientRec *client, ServiceCore *service){
	italcExtensionServiceCorePtr = service;
	italcResponseUserInformation(client);
}