#ifndef UITOSERVICE_H
#define UITOSERVICE_H

#include <cstdint>
#include <Windows.h>

class Ui;

uint32_t WINAPI uiToServiceThread(LPVOID lpParameter);

/* UiToService: gère la connection de l'UI au service
 */
class UiToService{
	public:
		UiToService(uint16_t port, Ui* ui);
		void startThread();
		uint32_t mainThread();
		std::string recvString();
		void askStatus();
		void sendAuthmethod(uint8_t authMethod);
		void sendAuthresponse(uint32_t authResponse);
		void sendUsername(std::string username);
	private:
		SOCKET m_socket;
		Ui* m_ui;
};

#endif