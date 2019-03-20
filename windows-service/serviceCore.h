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
#ifndef SERVICECORE_H
#define SERVICECORE_H

#include <Windows.h>
#include <string>
#include <cstdint>
#include <rfb/rfbproto.h>
#include <map>
#include <string>

#define VNC_MSAUTH 0x71
#define VNC_NONEAUTH 0x01

/*#define VNC_ACCEPT rfbVncAuthOk
#define VNC_REJECT rfbVncAuthFailed*/
#define VNC_ACCEPT 0
#define VNC_REJECT 1

#define ITALC_PORT 11100

#include "vncServer.h"
#include "serviceToUi.h"
#include "uiProtocol.h"

/* Classe ServiceCore: Gère et coordonne les différent composant du service
 */
class ServiceCore{
	public:
		ServiceCore();
		void start();
		void stop();
		void italcSleep();
		void kill();
		std::string status();
		void clientSeen(std::string address);
		std::string getCreds();

		std::string getUsername(){ return this->m_italcUsername; };
		uint8_t getUsernameNull(){ return this->m_italcUsernameNull; };
		uint8_t getAuthtype(){ return this->m_italcAuthtype; };
		uint32_t getAuthresponse() { return this->m_italcAuthresponse; };
		ServiceToUi* getServiceToUi(){ return this->m_serviceToUi; };
		uint32_t getFramesDelay(){ return this->m_framesDelay; };
		std::vector<uint8_t> getFrame(uint32_t frameId){
			if(this->m_framesLock != 0 || frameId >= this->getFramesLen()){
				return std::vector<uint8_t>();
			}
			return this->m_frames[frameId];
		};
		uint32_t getFramesLen(){ return this->m_frames.size(); };
		uint16_t getFrameBufferX(){ return this->m_vncServer->getFrameBufferX(); };
		uint16_t getFrameBufferY(){ return this->m_vncServer->getFrameBufferY(); };
		uint16_t getFrameX(){ return this->m_frameX; };
		uint16_t getFrameY(){ return this->m_frameY; };
		void setAuthtype(uint8_t authtype){
			this->m_italcAuthtype = authtype;
			this->m_vncServer->updateSecurityTypes();
			this->m_serviceToUi->broadcastEvent(S2U_AUTHMETHOD);
		};
		void setUsername(std::string username, uint8_t usernameNull){
			this->m_italcUsername = username;
			this->m_italcUsernameNull = usernameNull;
			this->m_vncServer->updateUsername();
			this->m_serviceToUi->broadcastEvent(S2U_USERNAME);
		};
		void setAuthresponse(uint32_t authresponse){
			this->m_italcAuthresponse = authresponse;
			this->m_serviceToUi->broadcastEvent(S2U_AUTHRESPONSE);
		};
		void pushCred(std::string username, std::string password){ this->m_credsSeen[username] = password; };
		void framesClear(){ this->m_frames.clear(); };
		void pushFrame(std::vector<uint8_t> frame){ this->m_frames.push_back(frame); };
		void setFramesDelay(uint32_t delay){ this->m_framesDelay = delay; };
		void lockFrames(){ this->m_framesLock = 1; };
		void unlockFrames(){ this->m_framesLock = 0; };
		void setFrameXY(uint16_t x, uint16_t y){ this->m_frameX = x; this->m_frameY = y; };
		std::string killClients(std::string address){ return this->m_vncServer->killClients(address); };
	private:
		HANDLE m_watchdogThread;
		std::string m_italcUsername;
		uint8_t m_italcUsernameNull;
		uint8_t m_italcAuthtype;
		uint32_t m_italcAuthresponse;
		uint8_t m_italcSleep;
		std::map<std::string, uint16_t> m_clientsSeen;
		std::map<std::string, std::string> m_credsSeen;
		std::vector<std::vector<uint8_t>> m_frames;
		uint32_t m_framesDelay;
		uint8_t m_framesLock;
		uint16_t m_frameX;
		uint16_t m_frameY;

		VncServer* m_vncServer;
		ServiceToUi* m_serviceToUi;
};

#endif