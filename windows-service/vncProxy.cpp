#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "vncProxy.h"

/* vncProxyAcceptingThreadConnector: Est démarée par un thread et le redirige vers VncProxy::acceptingThread
 * LPVOID lpParameter: VncProxy utilisé
 * Retourne un uint32_t: retour de VncProxy::acceptingThread
 */
uint32_t WINAPI vncProxyAcceptingThreadConnector(LPVOID lpParameter){
	VncProxy *proxy = (VncProxy*) lpParameter;
	return proxy->acceptingThread();
}

/* VncProxy::VncProxy: Constructeur de VncProxy
 * SOCKET listeningSocket: Socket utilisé pour l'écoute
 */
VncProxy::VncProxy(SOCKET listeningSocket){
	this->m_listeningSocket = listeningSocket;
	this->m_acceptingThread = NULL;
	this->m_connections = std::vector<VncProxyConnection>();
}

/* VncProxy::startAcceptingThread: démare le thread d'accept via vncProxyAcceptingThreadConnector
 * Aucun paramètre ni retour
 */
void VncProxy::startAcceptingThread(){
	this->m_acceptingThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) vncProxyAcceptingThreadConnector, (LPVOID) this, 0, NULL);
}

/* VncProxy::stopAcceptingThread: arrête le thread crée par VncProxy::startAcceptingThread
 * Aucun paramètre ni retour
 */
void VncProxy::stopAcceptingThread(){
	CloseHandle(this->m_acceptingThread);
}

/* VncProxy::acceptingThread: Thread qui accepte les connection est crée les paire de connection adaptée
 * Retourne un uint32_t: 0 en cas de réuisste, 1 en cas d'erreur
 */
uint32_t VncProxy::acceptingThread(){
	std::cout << "[VncProxy] acceptingThread started" << std::endl;
	while(1){
		sockaddr italcAddr = {};
		int16_t italcAddrLen = sizeof(italcAddr);
		SOCKET italcSocket = accept(this->m_listeningSocket, &italcAddr, (int*) &italcAddrLen);
		if(italcSocket == INVALID_SOCKET){
			// Le socket a dut être fermée => on arrête le thread
			return 1;
		}

		// Conversion de l'addresse en ASCII
		char italcIpAddrAscii[32] = {};
		uint32_t italcIpAddrAsciiLen = 32;
		WSAAddressToStringA(&italcAddr, italcAddrLen, NULL, italcIpAddrAscii, (LPDWORD) &italcIpAddrAsciiLen);
		std::string italcAddrStr(italcIpAddrAscii);
		std::cout << "[VncProxy] New connection from " << italcAddrStr << std::endl;
	}
	return 0;
}