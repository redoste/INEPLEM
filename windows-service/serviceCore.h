#ifndef SERVICECORE_H
#define SERVICECORE_H

#include <Windows.h>
#include <string>
#include <cstdint>
#include <rfb/rfbproto.h>

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

		std::string getUsername(){ return this->m_italcUsername; };
		uint8_t getUsernameNull(){ return this->m_italcUsernameNull; };
		uint8_t getAuthtype(){ return this->m_italcAuthtype; };
		uint32_t getAuthresponse() { return this->m_italcAuthresponse; };
		ServiceToUi* getServiceToUi(){ return this->m_serviceToUi; };
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
		}
		void setAuthresponse(uint32_t authresponse){
			this->m_italcAuthresponse = authresponse;
			this->m_serviceToUi->broadcastEvent(S2U_AUTHRESPONSE);
		}
	private:
		HANDLE m_watchdogThread;
		std::string m_italcUsername;
		uint8_t m_italcUsernameNull;
		uint8_t m_italcAuthtype;
		uint32_t m_italcAuthresponse;
		uint8_t m_italcSleep;

		VncServer* m_vncServer;
		ServiceToUi* m_serviceToUi;
};

#endif