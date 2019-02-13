#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "net.h"
#include "vncProxy.h"
#include "pipe.h"

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
		// On accepte une nouvelle connection
		VncProxyConnection newConnection = {};
		newConnection.italcMasterAddr = {};
		int16_t italcAddrLen = sizeof(sockaddr);
		std::cout << "[VncProxy] Waiting for incomming connection..." << std::endl;
		newConnection.italcMasterSocket = accept(this->m_listeningSocket, &newConnection.italcMasterAddr, NULL);
		if(newConnection.italcMasterSocket == INVALID_SOCKET){
			// Le socket a dut être fermée => on arrête le thread
			std::cout << "[VncProxy] acceptingThread stoped. WSError:" << WSAGetLastError() << std::endl;
			return 1;
		}

		// Conversion de l'addresse en ASCII
		char italcIpAddrAscii[32] = {};
		uint32_t italcIpAddrAsciiLen = 32;
		WSAAddressToStringA(&newConnection.italcMasterAddr, italcAddrLen, NULL, italcIpAddrAscii, (LPDWORD) &italcIpAddrAsciiLen);
		newConnection.italcMasterAddrStr = std::string(italcIpAddrAscii);
		std::cout << "[VncProxy] New connection from " << newConnection.italcMasterAddrStr << " Socket:" << newConnection.italcMasterSocket << std::endl;

		// On se connecte à libvnc
		newConnection.libvncSocket = createClientConnection("127.0.0.1", 11142);
		std::cout << "[VncProxy] New local connection Socket:" << newConnection.libvncSocket << std::endl;

		// On créer 2 pipes
		uint32_t pairId = this->m_connections.size();
		newConnection.italcToLibvncPipe = createPipe(newConnection.italcMasterSocket, newConnection.libvncSocket, pairId, this, PIPE_WAY_ITALC_TO_LIBVNC);
		newConnection.libvncToItalcPipe = createPipe(newConnection.libvncSocket, newConnection.italcMasterSocket, pairId, this, PIPE_WAY_LIBVNC_TO_ITALC);

		// Et on push le tout dans le vector
		newConnection.alive = 1;
		this->m_connections.push_back(newConnection);
		std::cout << "[VncProxy] Two new pipe created pairId:" << pairId << std::endl;
	}
	return 0;
}

/* VncProxy::stopPair: Arrête une paire de pipe et ferme les socket correspondant
 * uint32_t pairId: Identifiant de la paire
 * Aucun retour
 */
void VncProxy::stopPair(uint32_t pairId){
	VncProxyConnection toClose = this->m_connections[pairId];
	if(toClose.alive){
		stopPipe(toClose.italcToLibvncPipe);
		stopPipe(toClose.libvncToItalcPipe);
		closesocket(toClose.italcMasterSocket);
		closesocket(toClose.libvncSocket);
		std::cout << "[VncProxy] Closed pipe pair pairId:" << pairId << std::endl;
		this->m_connections[pairId].alive = 0;
	}
}

/* VncProxy::stopAllPairs(): Arrête toutes les paires en vie
 * Aucun paramètre ni retour
 */
void VncProxy::stopAllPairs(){
	for(std::vector<VncProxyConnection>::iterator pairIt = this->m_connections.begin(); pairIt != this->m_connections.end(); pairIt++){
		if(pairIt->alive){
			this->stopPair(pairIt - this->m_connections.begin());
		}
	}
}