#include <cstdint>
#include <Windows.h>
#include <iostream>

#include "uiToService.h"
#include "net.h"
#include "ui.h"
#include "uiProtocol.h"
#include "serviceCore.h"

/* uiToServiceThread: Créer par un thread, redirige vers UiToService::mainThread
 * LPVOID lpParameter: Pointeur vers le UiToService a utiliser
 * retourne un uint32_t: retour de UiToService::mainThread
 */
uint32_t WINAPI uiToServiceThread(LPVOID lpParameter){
	UiToService *uitoservice = (UiToService*) lpParameter;
	return uitoservice->mainThread();
}

/* UiToService::UiToService: Constructeur de UiToService
 * uint16_t port: Port de connection au service
 * Ui* ui: Pointeur vers l'UI
 */
UiToService::UiToService(uint16_t port, Ui* ui){
	this->m_ui = ui;
	initWSocket();
	this->m_socket = createClientConnection("127.0.0.1", port);
	if(this->m_socket == INVALID_SOCKET){
		std::cerr << "[UiToService] Unable to connect to service E:" << WSAGetLastError() << std::endl;
	}
	else{
		std::cout << "[UiToService] Connected to service" << std::endl;
	}
}

/* UiToService::startThread: Créer un thread vers uiToServiceThread
 * Aucun paramètre ni retour
 */
void UiToService::startThread(){
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) uiToServiceThread, (LPVOID) this, 0, NULL);
}

/* UiToService::mainThread: Thread principal de la connection avec le service
 * retorune un uint32_t: Toujours 0, obligé pour les thread par l'API Windows
 */
uint32_t UiToService::mainThread(){
	for(;;){
		uint8_t opCode = 0x00;
		int16_t recivedLen = recv(this->m_socket, (char*) &opCode, 1, 0);
		if(recivedLen <= 0){
			// La connection c'est fermée => On arrête le thread
			break;
		}

		if(opCode == S2U_STATUS || opCode == S2U_NOTIFICATION || opCode == S2U_CREDS){
			// Status, Notification ou Creds -> Simple MsgBox
			this->m_ui->msgBox(this->recvString());
		}
		// Pour Authmethod et AuthResponse on met a jour les checkbox
		else if(opCode == S2U_AUTHMETHOD){
			uint8_t authMethod;
			recv(this->m_socket, (char*) &authMethod, 1, 0);
			if(authMethod == VNC_MSAUTH){
				this->m_ui->setCheckbox(UI_MENU_AUTHMETHOD_NONE, 0);
				this->m_ui->setCheckbox(UI_MENU_AUTHMETHOD_MSII, 1);
			}
			else if(authMethod == VNC_NONEAUTH){
				this->m_ui->setCheckbox(UI_MENU_AUTHMETHOD_NONE, 1);
				this->m_ui->setCheckbox(UI_MENU_AUTHMETHOD_MSII, 0);
			}
			else{
				this->m_ui->setCheckbox(UI_MENU_AUTHMETHOD_NONE, 0);
				this->m_ui->setCheckbox(UI_MENU_AUTHMETHOD_MSII, 0);
			}
			this->m_ui->updateMenuCheckboxes();
		}
		else if(opCode == S2U_AUTHRESPONSE){
			uint32_t authResponse;
			recv(this->m_socket, (char*) &authResponse, 4, 0);
			if(authResponse == VNC_ACCEPT){
				this->m_ui->setCheckbox(UI_MENU_AUTHRESPONSE_REJECT, 0);
				this->m_ui->setCheckbox(UI_MENU_AUTHRESPONSE_ACCEPT, 1);
			}
			else if(authResponse == VNC_REJECT){
				this->m_ui->setCheckbox(UI_MENU_AUTHRESPONSE_REJECT, 1);
				this->m_ui->setCheckbox(UI_MENU_AUTHRESPONSE_ACCEPT, 0);
			}
			else{
				this->m_ui->setCheckbox(UI_MENU_AUTHRESPONSE_REJECT, 0);
				this->m_ui->setCheckbox(UI_MENU_AUTHRESPONSE_ACCEPT, 0);
			}
			this->m_ui->updateMenuCheckboxes();
		}
		else if(opCode == S2U_USERNAME){
			// On lit le username pour éviter un décalage
			std::cout << "[UiToService] Username: " << this->recvString() << std::endl;
		}
	}
	return 0;
}

/* UiToService::recvString: Recois une string dans un format standard depuis le service
 * retourne une std::string: La string lue
 */
std::string UiToService::recvString(){
	uint32_t strLen;
	recv(this->m_socket, (char*) &strLen, 4, 0);
	char *str = new char[strLen];
	recv(this->m_socket, str, strLen, 0);
	std::string strString(str);
	delete str;
	return strString;
}

/* UiToService::askStatus: Demande le status au service
 * Aucun paramètre ni retour
 */
void UiToService::askStatus(){
	char opCode = U2S_STATUS;
	send(this->m_socket, &opCode, 1, 0);
}

/* UiToService::sendAuthmethod: Envois un message U2S_AUTHMETHOD au service
 * uint8_t authMethod: authMethod a envoyer au service
 */
void UiToService::sendAuthmethod(uint8_t authMethod){
	char opCode = U2S_AUTHMETHOD;
	send(this->m_socket, &opCode, 1, 0);
	send(this->m_socket, (char*) &authMethod, 1, 0);
}

/* UiToService::sendAuthresponse: Envois un message U2S_AUTHRESPONSE au service
 * uint32_t authResponse: authResponse a envoyer au service
 */
void UiToService::sendAuthresponse(uint32_t authResponse){
	char opCode = U2S_AUTHRESPONSE;
	send(this->m_socket, &opCode, 1, 0);
	send(this->m_socket, (char*) &authResponse, 4, 0);
}

/* UiToService::sendUsername: Envois un message U2S_USERNAME au service
 * std::string username: username à envoyer au service
 */
void UiToService::sendUsername(std::string username){
	uint32_t usernameLen = username.length() + 1; //+1 pour le 0x00
	char opCode = U2S_USERNAME;
	send(this->m_socket, &opCode, 1, 0);
	send(this->m_socket, (char*) &usernameLen, 4, 0);
	send(this->m_socket, username.c_str(), usernameLen, 0);
}

/* UiToService::askCreds: Envois un message U2S_CREDS au service pour demander les creds
 * Aucun paramètre ni retour
 */
void UiToService::askCreds(){
	char opCode = U2S_CREDS;
	send(this->m_socket, &opCode, 1, 0);
}