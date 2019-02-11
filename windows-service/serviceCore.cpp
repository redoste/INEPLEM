#include <iostream>
#include <string>

#include "serviceCore.h"
#include "watchdog.h"
#include "net.h"
#include "vncProxy.h"

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

	// On écoute sur le socket
	// On attend avant que Italc soit arrêté
	while(this->m_italcSleep == 0) Sleep(50);
	this->m_italcListeningSocket = createServerConnection("0.0.0.0", ITALC_PORT);
	if(this->m_italcListeningSocket == INVALID_SOCKET){
		std::cerr << "[ServiceCore] Error while listening to " << ITALC_PORT << " TCP Port. E:" << WSAGetLastError() << std::endl;
		this->kill();
	}
	std::cout << "[ServiceCore] Listening Socket on TCP:" << ITALC_PORT << " established." << std::endl;

	// On démare le proxy
	this->m_vncProxy = new VncProxy(this->m_italcListeningSocket);
	this->m_vncProxy->startAcceptingThread();
}

/* ServiceCore::stop: Arrête les différents composant du service
 * Aucun parramètre et aucun retour
 */
void ServiceCore::stop(){
	std::cout << "[ServiceCore] Stopping..." << std::endl;
	// On arrète le watchdog Italc
	CloseHandle(this->m_watchdogThread);
	// On arrête le proxy
	this->m_vncProxy->stopAcceptingThread();
	delete this->m_vncProxy;
	// On ferme le socket d'écoute
	closesocket(this->m_italcListeningSocket);
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