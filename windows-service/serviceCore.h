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

/* Classe ServiceCore: Gère et coordonne les différent composant du service
 */
class ServiceCore{
	public:
		ServiceCore();
		void start();
		void stop();
	private:
		HANDLE m_watchdogThread;
		std::string m_italcUsername;
		uint8_t m_italcUsernameNull;
		uint8_t m_italcAuthtype;
		uint8_t m_italcAuthresponse;
};

#endif