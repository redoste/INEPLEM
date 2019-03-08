#include <rfb/rfb.h>
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <chrono>

#include "vncServer.h"
#include "serviceCore.h"
#include "italcExtension.h"
#include "msAuth.h"
#include "noneAuth.h"

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
	memset(this->m_frameBuffer, 0, this->m_frameBufferX*this->m_frameBufferY*3);

	// On met une frame vide
	this->m_service->framesClear();
	this->m_service->setFramesDelay(1000);
	this->m_service->pushFrame(std::vector<uint8_t>((this->m_frameBufferX*this->m_frameBufferY*3)));
	this->m_service->setFrameXY(this->m_frameBufferX, this->m_frameBufferY);

	this->m_screen = rfbGetScreen(0, NULL, this->m_frameBufferX, this->m_frameBufferY, 8, 3, 3);
	this->m_screen->port = port;
	this->m_screen->ipv6port = port;
	this->m_screen->frameBuffer = this->m_frameBuffer;

	italcExtensionRegister(service);

	this->m_noneAuth = NULL;
	this->m_msAuthII = NULL;
	this->updateSecurityTypes();

	rfbLogEnable(0);
	rfbInitServer(this->m_screen);
}

/* VncServer::serverThread: Thread principal du VncServer
 * Retourne un uint32_t: 0 en cas de réussite, 1 en cas d'erreur
 */
uint32_t VncServer::serverThread(){
	std::cout << "[VncServer] Start serverThread..." << std::endl;
	uint32_t actualFrame = 0;
	uint16_t oldFrameX = 0;
	uint16_t oldFrameY = 0;
	std::chrono::system_clock::time_point lastFrameTime = std::chrono::system_clock::now();
	while(this->m_threadContinue){
		rfbProcessEvents(this->m_screen, this->m_screen->deferUpdateTime * 1000);

		std::chrono::system_clock::time_point actualTime = std::chrono::system_clock::now();
		std::chrono::milliseconds differenceTime = std::chrono::duration_cast<std::chrono::milliseconds>(actualTime - lastFrameTime);
		if(differenceTime.count() >= this->m_service->getFramesDelay()){
			lastFrameTime = actualTime;
			actualFrame += 1;
			if(actualFrame >= this->m_service->getFramesLen()){
				actualFrame = 0;
			}
			std::vector<uint8_t> frame = this->m_service->getFrame(actualFrame);
			if(frame.size() == (uint32_t) this->m_frameBufferX*this->m_frameBufferY*3){
				memcpy(this->m_frameBuffer, frame.data(), this->m_frameBufferX*this->m_frameBufferY*3);
				// Si les dimensions de la frame change, i renvois tous l'écran pour éviter les résidus
				uint16_t newFrameX = this->m_service->getFrameX();
				uint16_t newFrameY = this->m_service->getFrameY();
				if(newFrameX != oldFrameX || newFrameY != oldFrameY){
					oldFrameX = newFrameX;
					oldFrameY = newFrameY;
					rfbMarkRectAsModified(this->m_screen, 0, 0, this->m_frameBufferX, this->m_frameBufferY);
				}
				else{
					rfbMarkRectAsModified(this->m_screen, 0, 0, oldFrameX, oldFrameY);
				}
			}
		}
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

/* VncServer::updateSecurityTypes: Met à jour les security types enregistré au près de libvnc
 * Aucun paramètre ni retour
 */
void VncServer::updateSecurityTypes(){
	switch(this->m_service->getAuthtype()){
		case VNC_MSAUTH:
			// VNC_MSAUTH => On essaye de unregister noneAuth et de register msAuthII
			this->tryToUnregister(&this->m_noneAuth);
			if(this->m_msAuthII == NULL){
				this->m_msAuthII = msAuthIIRegisterSecurity(this->m_service);
			}
			break;
		case VNC_NONEAUTH:
			// VNC_NONEAUTH => On essaye de unregister msAuthII et de register noneAuth
			this->tryToUnregister(&this->m_msAuthII);
			if(this->m_noneAuth == NULL){
				this->m_noneAuth = noneAuthRegisterSecurity(this->m_service);
			}
			break;
		default:
			// Aucun des deux => On les uregister tous les deux => Refuse forcement le client
			this->tryToUnregister(&this->m_noneAuth);
			this->tryToUnregister(&this->m_msAuthII);
			break;
	}
}

/* VncServer::tryToUnregister: Essaye de unregister un securityHandler
 * rfbSecurityHandler **toUnregister: Pointeur vers le pointeur du security handler
 */
void VncServer::tryToUnregister(rfbSecurityHandler **toUnregister){
	if(*toUnregister != NULL){
		std::cout << "[VncServer::tryToUnregister] Unregister security handler: " << *toUnregister << std::endl;
		rfbUnregisterSecurityHandler(*toUnregister);
		delete *toUnregister;
		*toUnregister = NULL;
	}
}

/* VncServer::updateUsername: Envois un message UserInformation a tous les client pour les informer du nouveau nom d'utilisateur
 * Aucun paramètre ni retour
 */
void VncServer::updateUsername(){
	rfbClientRec *clientIt = this->m_screen->clientHead;
	while(clientIt != NULL){
		italcResponseUserInformationWithService(clientIt, this->m_service);
		clientIt = clientIt->next;
	}
}