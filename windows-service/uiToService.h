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
		void askCreds();
		void sendImage(std::string filename);
		void killClients(std::string address);
		void sendRunas(std::string cmdLine);
	private:
		SOCKET m_socket;
		Ui* m_ui;
};

#endif