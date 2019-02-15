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
	private:
		rfbScreenInfoPtr m_screen;
		ServiceCore *m_service;
		char *m_frameBuffer;
		uint16_t m_frameBufferX;
		uint16_t m_frameBufferY;

		uint8_t m_threadContinue;
		HANDLE m_thread;
};

#endif