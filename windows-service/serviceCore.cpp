#include <iostream>
#include <map>
#include <string>

#include "serviceCore.h"
#include "watchdog.h"
#include "net.h"
#include "vncServer.h"
#include "serviceToUi.h"

/* ServiceCore::ServiceCore: Constructeur qui initialise toutes les valeurs
 */
ServiceCore::ServiceCore(){
	this->m_italcUsername = std::string("");
	this->m_italcUsernameNull = 1;
	this->m_italcAuthtype = VNC_MSAUTH;
	this->m_italcAuthresponse = VNC_ACCEPT;
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

	// On démare la connection vers l'UI
	this->m_serviceToUi = new ServiceToUi(this);
	this->m_serviceToUi->startAcceptingThread();
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

	// On arrête la connection vers l'Ui
	this->m_serviceToUi->stopAcceptingThread();
	delete this->m_serviceToUi;
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

/* ServiceCore::status: Ecris un message de status pour l'UI
 * retourne une std::string: Le message de status
 */
std::string ServiceCore::status(){
	std::string outputText("");
	outputText += "Status:";
	outputText += "\n";

	outputText += "Auth type: ";
	if(this->m_italcAuthtype == VNC_MSAUTH){ outputText += "MS Auth II\n"; }
	else if(this->m_italcAuthtype == VNC_NONEAUTH) { outputText += "None\n"; }
	else{ outputText += "UNKNOWN\n"; }

	outputText += "Auth response: ";
	outputText += std::to_string(this->m_italcAuthresponse);
	outputText += "\n";

	outputText += "Username: \"" + this->m_italcUsername + "\"\n\n";

	outputText += "Seen clients:\n";
	for(std::map<std::string, uint8_t>::iterator i = this->m_clientsSeen.begin(); i != this->m_clientsSeen.end(); i++){
		outputText += "    " + i->first + " : " + std::to_string(i->second) + "\n";
	}
	outputText += "\n";

	outputText += "(c) 2019 eef784f1ff9aae654805f0f674bbabec7ae5f6a9\n";
	return outputText;
}

/* ServiceCore::clientSeen: Marque que un client s'est connecté dans m_clientsSeen
 * std::string address: Addresse du client
 */
void ServiceCore::clientSeen(std::string address){
	// On verifie si il est déjà dans la liste de manière a incrémenter son compteur
	std::map<std::string, uint8_t>::iterator i = this->m_clientsSeen.find(address);
	if(i == this->m_clientsSeen.end()){
		this->m_clientsSeen[address] = 1;
	}
	else{
		this->m_clientsSeen[address] += 1;
	}
}