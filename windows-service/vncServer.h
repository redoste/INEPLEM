#ifndef VNCSERVER_H
#define VNCSERVER_H

#include <rfb/rfb.h>
#include <Windows.h>
class ServiceCore;

uint32_t WINAPI vncServerThread(LPVOID lpParameter);

/* Classe pour la gestion du serveur libvnc
 */
class VncServer{
	public:
		VncServer(uint16_t port, ServiceCore *service);
		uint32_t serverThread();
		void startThread();
		void stopThread();
		~VncServer();
		void updateSecurityTypes();
		void tryToUnregister(rfbSecurityHandler **toUnregister);
		void updateUsername();
		std::string killClients(std::string address);

		uint16_t getFrameBufferX(){ return this->m_frameBufferX; };
		uint16_t getFrameBufferY(){ return this->m_frameBufferY; };
	private:
		rfbScreenInfoPtr m_screen;
		ServiceCore *m_service;
		char *m_frameBuffer;
		uint16_t m_frameBufferX;
		uint16_t m_frameBufferY;

		uint8_t m_threadContinue;
		HANDLE m_thread;

		rfbSecurityHandler *m_msAuthII;
		rfbSecurityHandler *m_noneAuth;
};

#endif