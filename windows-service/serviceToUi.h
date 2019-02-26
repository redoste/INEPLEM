#ifndef SERVICETOUI_H
#define SERVICETOUI_H

#include <cstdint>
#include <Windows.h>

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
	private:
		ServiceCore *m_service;
		HANDLE m_acceptingThread;
		uint8_t m_acceptingThreadContinue;
		SOCKET m_listeningSocket;
};

/* serviceToUiClientThreadParam: Structure pour les paramètres de serviceToUiClientThread
 */
typedef struct serviceToUiClientThreadParam{
	SOCKET socket;
	ServiceToUi *serviceToUi;
} serviceToUiClientThreadParam;

#endif