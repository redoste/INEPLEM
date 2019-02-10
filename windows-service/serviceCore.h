#ifndef SERVICECORE_H
#define SERVICECORE_H

#include <Windows.h>

/* Classe ServiceCore: Gère et coordonne les différent composant du service
 */
class ServiceCore{
	public:
		void start();
		void stop();
	private:
		HANDLE m_watchdogThread;
};

#endif