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
#include <cstdint>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "serviceCore.h"

/* initWScoket: Initialise l'API de Socket de Windows
 * Auncun paramètre
 * Retourne un uint16_t: code d'erreur de WSAStartup, s'il y a une erreur
 */
uint16_t initWSocket(){
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2,2), &wsaData);
}

/* createClientConnection: Se connecte via une connection TCP
 * std::string ip: ip de destination
 * uint16_t port: port de destination
 * Retourne un SOCKET: Socket de la connection ou INVALID_SOCKET en cas d'erreur
 */
SOCKET createClientConnection(std::string ip, uint16_t port){
	SOCKADDR_IN clientAddress;
	SOCKET clientSocket = INVALID_SOCKET;

	clientAddress.sin_addr.s_addr = inet_addr(ip.c_str());
	clientAddress.sin_family = AF_INET;
	clientAddress.sin_port = htons(port);

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket == INVALID_SOCKET){
		return INVALID_SOCKET;
	}

	int16_t errorReturn = connect(clientSocket, (SOCKADDR *)&clientAddress, sizeof(clientAddress));
	if(errorReturn == SOCKET_ERROR){
		return INVALID_SOCKET;
	}

	return clientSocket;
}

/* createServerConnection: Initialise une écoute TCP
 * std::string ip: addresse d'écoute
 * uint16_t port: port d'écoute
 * Retourne un SOCKET: Socket du serveur ou INVALID_SOCKET en cas d'erreur
 */
SOCKET createServerConnection(std::string ip, uint16_t port){
	SOCKET serverSocket = INVALID_SOCKET;
	SOCKADDR_IN listenAddress;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	listenAddress.sin_addr.s_addr = inet_addr(ip.c_str());
	listenAddress.sin_family = AF_INET;
	listenAddress.sin_port = htons(port);

	if(serverSocket == INVALID_SOCKET){
		return INVALID_SOCKET;
	}

	int16_t errorReturn = bind(serverSocket, (SOCKADDR *)&listenAddress, sizeof(listenAddress));
	if(errorReturn == SOCKET_ERROR){
		return INVALID_SOCKET;
	}
	errorReturn = listen(serverSocket, 0);
	if(errorReturn == SOCKET_ERROR){
		return INVALID_SOCKET;
	}

	return serverSocket;
}

/* whitelistFirewall: Ajoute INEPLEM a la whitelist du firewall Windows
 * Aucun parramètre ni retour
 */
void whitelistFirewall(){
	//std::string disableCommand("netsh advfirewall set allprofile firewallpolicy allowinbound,allowoutbound");
	std::string allowCommand("netsh advfirewall firewall add rule name=\"iTALC Client Application\" dir=in action=allow enable=yes protocol=TCP localport=");
	allowCommand += std::to_string(ITALC_PORT);
	system(allowCommand.c_str());
}