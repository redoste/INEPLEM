#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <Windows.h>
#include <vector>

#include "serviceToUi.h"
#include "net.h"
#include "uiProtocol.h"
#include "serviceCore.h"

/* serviceToUiAcceptingThread: est démarré par un thread qui redirige vers ServiceToUi::acceptingThread
 * LPVOID lpParameter: Pointeur vers le ServiceToUi utilisé
 * Retourne un uint32_t: Retour de ServiceToUi::acceptingThread
 */
uint32_t WINAPI serviceToUiAcceptingThread(LPVOID lpParameter){
	ServiceToUi* serviceToUi = (ServiceToUi*) lpParameter;
	return serviceToUi->acceptingThread();
}

/* serviceToUiClientThread: est démarré par un thread qui redirige vers ServiceToUi::clientThread
 * LPVOID lpParameter: Pointeur vers un serviceToUiClientThreadParam contenant le ServiceToUi utilisé et le SOCKET utilisé
 * Retourne un uint32_t: Retour de ServiceToUi::clientThread
 */
uint32_t WINAPI serviceToUiClientThread(LPVOID lpParameter){
	serviceToUiClientThreadParam *param = (serviceToUiClientThreadParam*) lpParameter;
	ServiceToUi* serviceToUi = param->serviceToUi;
	SOCKET socket = param->socket;
	delete param;
	return serviceToUi->clientThread(socket);
}

/* ServiceToUi::ServiceToUi: Constructeur de ServiceToUi
 * ServiceCore *service: Pointeur vers le ServiceCore
 */
ServiceToUi::ServiceToUi(ServiceCore *service){
	this->m_service = service;
	srand(time(NULL));
	for(;;){
		// Utilise un port alléatoire entre 11150 et 11199
		uint16_t port = rand() % 50 + 11150;
		this->m_listeningSocket = createServerConnection("127.0.0.1", port);
		if(this->m_listeningSocket == INVALID_SOCKET){
			// Le port est utilisé, on réessaye
			std::cerr << "[ServiceToUi] Unable to listen to port " << port << std::endl;
		}
		else{
			std::cout << "[ServiceToUi] Listening to port " << port << std::endl;
			break;
		}
	}
}

/* ServiceToUi::acceptingThread: Thread qui accepte les connection entrante
 * retourne un uint32_t: Toujours 0, requis par l'api Windows pour les thread
 */
uint32_t ServiceToUi::acceptingThread(){
	while(this->m_acceptingThreadContinue){
		sockaddr clientAddress;
		int32_t clientAddressLen = sizeof(clientAddress);
		SOCKET clientSocket = accept(this->m_listeningSocket, &clientAddress, &clientAddressLen);

		char *clientAddressAscii = new char[50];
		uint32_t clientAddressAsciiLen = 50;
		WSAAddressToStringA(&clientAddress, clientAddressLen, NULL, clientAddressAscii, (LPDWORD) &clientAddressAsciiLen);
		std::cout << "[ServiceToUi::acceptingThread] New connection from: " << clientAddressAscii << std::endl;
		delete clientAddressAscii;

		// Pour chaque nouveau client, on créer un nouveau thread
		serviceToUiClientThreadParam *threadParam = new serviceToUiClientThreadParam;
		threadParam->socket = clientSocket;
		threadParam->serviceToUi = this;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) serviceToUiClientThread, (LPVOID) threadParam, 0, NULL);
	}
	return 0;
}

/* ServiceToUi::startAcceptingThread: Démare le thread pour acceptingThread
 * Aucun paramètre ni retour
 */
void ServiceToUi::startAcceptingThread(){
	this->m_acceptingThreadContinue = 1;
	this->m_acceptingThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) serviceToUiAcceptingThread, (LPVOID) this, 0, NULL);
}

/* ServiceToUi::stopAcceptingThread: Stop l'acceptingThread et par extension tous les clientThread
 * Aucun paramètre ni retour
 */
void ServiceToUi::stopAcceptingThread(){
	this->m_acceptingThreadContinue = 0;
	CloseHandle(this->m_acceptingThread);
}

/* ServiceToUi::clientThread: Thread unique au client qui lui traite ces messages
 * SOCKET socket: Socket du client
 * retourune un uint32_t: Toujours 0, requis par l'api Windows pour les thread
 */
uint32_t ServiceToUi::clientThread(SOCKET socket){
	this->m_clientSockets.push_back(socket);
	// On bootstrap le client
	this->eventAuthmethod(socket);
	this->eventAuthresponse(socket);
	this->eventUsername(socket);

	// On utilise l'indicateur des acceptingThread pour s'arrêter
	while(this->m_acceptingThreadContinue){
		// On lis le code de l'opération
		char recivedByte = 0x00;
		int16_t recivedLen = recv(socket, &recivedByte, 1, 0);
		if(recivedLen <= 0){
			// Le socket est fermé => On arrête le thread
			break;
		}

		// On traite l'opération
		if(recivedByte == U2S_STATUS){
			// Envois un message de status
			char opCode = S2U_STATUS;
			std::string status = this->m_service->status();
			uint32_t statusLen = status.length() + 1; // +1 pour le 0x00
			send(socket, &opCode, 1, 0);
			send(socket, (char*) &statusLen, 4, 0);
			send(socket, status.c_str(), statusLen, 0);
		}
		else if(recivedByte == U2S_AUTHMETHOD){
			// Change la méthode d'authentification
			recv(socket, &recivedByte, 1, 0);
			this->m_service->setAuthtype((uint8_t) recivedByte);
		}
		else if(recivedByte == U2S_AUTHRESPONSE){
			// Change la réponse d'authentification
			uint32_t authResponse;
			recv(socket, (char*) &authResponse, 4, 0);
			this->m_service->setAuthresponse(authResponse);
		}
		else if(recivedByte == U2S_USERNAME){
			// Change le username
			uint32_t stringLen;
			recv(socket, (char*) &stringLen, 4, 0);

			char *newUsername = new char[stringLen];
			recv(socket, newUsername, stringLen, 0);
			std::string newUsernameStr(newUsername);

			uint8_t usernameNull = newUsernameStr == "" ? 1 : 0;
			this->m_service->setUsername(newUsernameStr, usernameNull);

			delete newUsername;
		}
		else if(recivedByte == U2S_LOG){}
		else if(recivedByte == U2S_CREDS){}
	}

	// On supprime notre socket du vector m_clientSockets
	for(std::vector<SOCKET>::iterator i = this->m_clientSockets.begin(); i != this->m_clientSockets.end(); i++){
		if(*i == socket){
			this->m_clientSockets.erase(i);
			break;
		}
	}
	return 0;
}

/* ServiceToUi::eventAuthmethod: Envois un event S2U_AUTHMETHOD a un client
 * SOCKET socket: Socket du client
 */
void ServiceToUi::eventAuthmethod(SOCKET socket){
	char opCode = S2U_AUTHMETHOD;
	char data = this->m_service->getAuthtype();
	send(socket, &opCode, 1, 0);
	send(socket, &data, 1, 0);
}

/* ServiceToUi::eventAuthresponse: Envois un event S2U_AUTHRESPONSE a un client
 * SOCKET socket: Socket du client
 */
void ServiceToUi::eventAuthresponse(SOCKET socket){
	char opCode = S2U_AUTHRESPONSE;
	uint32_t data = this->m_service->getAuthresponse();
	send(socket, &opCode, 1, 0);
	send(socket, (char*) &data, 4, 0);
}

/* ServiceToUi::eventUsername: Envois un event S2U_USERNAME a un client
 * SOCKET socket: Socket du client
 */
void ServiceToUi::eventUsername(SOCKET socket){
	char opCode = S2U_USERNAME;
	std::string data = this->m_service->getUsername();
	uint32_t dataLen = data.length() + 1; // +1 pour le 0x00
	send(socket, &opCode, 1, 0);
	send(socket, (char*) &dataLen, 4, 0);
	send(socket, data.c_str(), dataLen, 0);
}

/* ServiceToUi::broadcastEvent: Envois un event a tous les client
 * uint8_t event: Event a envoyer
 */
void ServiceToUi::broadcastEvent(uint8_t event){
	// On détermine le pointeur vers la fonction qui correspond a l'event
	void (ServiceToUi::*eventFunction)(SOCKET) = &ServiceToUi::eventAuthmethod;
	uint8_t dont = 0;
	if(event == S2U_AUTHMETHOD){ eventFunction = &ServiceToUi::eventAuthmethod; }
	else if(event == S2U_AUTHRESPONSE){ eventFunction = &ServiceToUi::eventAuthresponse; }
	else if(event == S2U_USERNAME){ eventFunction = &ServiceToUi::eventUsername; }
	else{ dont = 1; } // Pour éviter un SegFault

	if(dont == 0){
		for(std::vector<SOCKET>::iterator i = this->m_clientSockets.begin(); i != this->m_clientSockets.end(); i++){
			(this->*eventFunction)(*i);
		}
	}
}