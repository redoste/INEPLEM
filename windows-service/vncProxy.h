#ifndef VNCPROXY_H
#define VNCPROXY_H

#include <Windows.h>
#include <vector>
#include <string>
#include <cstdint>

/* Structure VncProxyConnection: représente une paire de connections gérée par VncProxy
 */
typedef struct VncProxyConnection{
	SOCKET italcMasterSocket;
	sockaddr italcMasterAddr;
	std::string italcMasterAddrStr;
	SOCKET libvncSocket;
	uint8_t alive;
} VncProxyConnection;

uint32_t WINAPI vncProxyAcceptingThreadConnector(LPVOID lpParameter);

/* Classe VncProxy: Coordonne les connection entre les connection d'italc master et de libvncserver
 */
class VncProxy{
	public:
		VncProxy(SOCKET listeningSocket);
		uint32_t acceptingThread();
		void startAcceptingThread();
		void stopAcceptingThread();
	private:
		std::vector<VncProxyConnection> m_connections;
		SOCKET m_listeningSocket;
		HANDLE m_acceptingThread;
};

#endif