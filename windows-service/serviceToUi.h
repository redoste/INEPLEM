#ifndef SERVICETOUI_H
#define SERVICETOUI_H

#include <cstdint>
#include <Windows.h>
#include <vector>

class ServiceCore;

uint32_t WINAPI serviceToUiAcceptingThread(LPVOID lpParameter);
uint32_t WINAPI serviceToUiClientThread(LPVOID lpParameter);

/* ServiceToUi: gère la connection du service vers l'UI
 */
class ServiceToUi{
	public:
		ServiceToUi(ServiceCore *service);
		void startAcceptingThread();
		void stopAcceptingThread();
		uint32_t acceptingThread();
		uint32_t clientThread(SOCKET socket);
		void eventAuthmethod(SOCKET socket);
		void eventAuthresponse(SOCKET socket);
		void eventUsername(SOCKET socket);
		void broadcastEvent(uint8_t event);
	private:
		ServiceCore *m_service;
		HANDLE m_acceptingThread;
		uint8_t m_acceptingThreadContinue;
		SOCKET m_listeningSocket;
		std::vector<SOCKET> m_clientSockets;
};

/* serviceToUiClientThreadParam: Structure pour les paramètres de serviceToUiClientThread
 */
typedef struct serviceToUiClientThreadParam{
	SOCKET socket;
	ServiceToUi *serviceToUi;
} serviceToUiClientThreadParam;

#endif