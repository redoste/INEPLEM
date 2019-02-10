#include <iostream>

#include "serviceCore.h"
#include "watchdog.h"

/* ServiceCore::start: Démare les différents composant du service
 * Aucun parramètre et aucun retour
 */
void ServiceCore::start(){
	std::cout << "[ServiceCore] Starting..." << std::endl;
	// On démare le watchdog Italc
	this->m_watchdogThread = italcWatchdogThread();
}

/* ServiceCore::stop: Arrête les différents composant du service
 * Aucun parramètre et aucun retour
 */
void ServiceCore::stop(){
	std::cout << "[ServiceCore] Stopping..." << std::endl;
	// On arrète le watchdog Italc
	CloseHandle(this->m_watchdogThread);
}