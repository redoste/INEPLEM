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
#ifndef VNCSERVER_H
#define VNCSERVER_H

#include <rfb/rfb.h>
#include <Windows.h>
class ServiceCore;

uint32_t WINAPI vncServerThread(LPVOID lpParameter);

/* Classe pour la gestion du serveur libvnc
 */
class VncServer{
	public:
		VncServer(uint16_t port, ServiceCore *service);
		uint32_t serverThread();
		void startThread();
		void stopThread();
		~VncServer();
		void updateSecurityTypes();
		void tryToUnregister(rfbSecurityHandler **toUnregister);
		void updateUsername();
		std::string killClients(std::string address);

		uint16_t getFrameBufferX(){ return this->m_frameBufferX; };
		uint16_t getFrameBufferY(){ return this->m_frameBufferY; };
	private:
		rfbScreenInfoPtr m_screen;
		ServiceCore *m_service;
		char *m_frameBuffer;
		uint16_t m_frameBufferX;
		uint16_t m_frameBufferY;

		uint8_t m_threadContinue;
		HANDLE m_thread;

		rfbSecurityHandler *m_msAuthII;
		rfbSecurityHandler *m_noneAuth;
};

#endif