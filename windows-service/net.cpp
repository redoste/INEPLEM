#include <cstdint>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

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
