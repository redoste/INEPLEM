#include <rfb/rfb.h>
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <string>

#include "vncServer.h"
#include "serviceCore.h"
#include "italcExtension.h"

/* vncServerThread: est démarré par un thread qui redirige vers VncServer::serverThread
 * LPVOID lpParameter: Pointeur vers le VncServer utilisé
 * Retourne un uint32_t: Retour de VncServer::serverThread
 */
uint32_t WINAPI vncServerThread(LPVOID lpParameter){
	VncServer* server = (VncServer*) lpParameter;
	return server->serverThread();
}

/* VncServer::VncServer: Constructeur du VncServer
 * uint16_t port: Port d'écoute du serveur VNC
 * ServiceCore *service: Pointeur vers le ServiceCore
 */
VncServer::VncServer(uint16_t port, ServiceCore *service){
	this->m_service = service;

	// Lecture de la taille de l'écran
	this->m_frameBufferX = GetSystemMetrics(SM_CXSCREEN);
	this->m_frameBufferY = GetSystemMetrics(SM_CYSCREEN);
	// Gestion des erreurs
	this->m_frameBufferX = this->m_frameBufferX == 0 ? 800 : this->m_frameBufferX;
	this->m_frameBufferY = this->m_frameBufferY == 0 ? 600 : this->m_frameBufferY;

	this->m_frameBuffer = new char[this->m_frameBufferX*this->m_frameBufferY*3];
	memset(this->m_frameBuffer, 255, this->m_frameBufferX*this->m_frameBufferY*3);

	this->m_screen = rfbGetScreen(0, NULL, this->m_frameBufferX, this->m_frameBufferY, 8, 3, 3);
	this->m_screen->port = port;
	this->m_screen->ipv6port = port;
	this->m_screen->frameBuffer = this->m_frameBuffer;
	italcExtensionRegister();

	rfbInitServer(this->m_screen);
}

/* VncServer::serverThread: Thread principal du VncServer
 * Retourne un uint32_t: 0 en cas de réussite, 1 en cas d'erreur
 */
uint32_t VncServer::serverThread(){
	std::cout << "[VncServer] Start serverThread..." << std::endl;
	while(this->m_threadContinue){
		rfbProcessEvents(this->m_screen, this->m_screen->deferUpdateTime * 1000);
	}
	std::cout << "[VncServer] Stop serverThread..." << std::endl;
	return 0;
}

/* VncServer::startThread: Démare un thread pour VncServer::serverThread via vncServerThread
 * Aucun paramètre ni retour
 */
void VncServer::startThread(){
	this->m_threadContinue = 1;
	this->m_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) vncServerThread, (LPVOID) this, 0, NULL);
}

/* VncServer::stopThread: Arrête le thread principal du VncServer
 * Aucun paramètre ni retour
 */
void VncServer::stopThread(){
	this->m_threadContinue = 0;
	CloseHandle(this->m_thread);
}

/* VncServer::~VncServer: Déstructeur du VncServer
 */
VncServer::~VncServer(){
	rfbScreenCleanup(this->m_screen);
	delete this->m_frameBuffer;
}