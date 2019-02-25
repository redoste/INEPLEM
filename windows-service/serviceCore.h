#ifndef SERVICECORE_H
#define SERVICECORE_H

#include <Windows.h>
#include <string>
#include <cstdint>

#define VNC_MSAUTH 0x71
#define VNC_NONEAUTH 0x01

#define VNC_DROP 0x00
#define VNC_REJECT 0x01
#define VNC_ACCEPT 0x02

#define ITALC_PORT 11100

#include "vncServer.h"

/* Classe ServiceCore: Gère et coordonne les différent composant du service
 */
class ServiceCore{
	public:
		ServiceCore();
		void start();
		void stop();
		void italcSleep();
		void kill();
		std::string getUsername(){ return this->m_italcUsername; };
		uint8_t getUsernameNull(){ return this->m_italcUsernameNull; };
	private:
		HANDLE m_watchdogThread;
		std::string m_italcUsername;
		uint8_t m_italcUsernameNull;
		uint8_t m_italcAuthtype;
		uint8_t m_italcAuthresponse;
		uint8_t m_italcSleep;

		VncServer* m_vncServer;
};

#endif