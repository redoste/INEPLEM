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
		void broadcastNotification(std::string text);
		void processNewFrames(SOCKET socket);
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