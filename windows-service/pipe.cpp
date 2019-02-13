#include <Windows.h>
#include <cstdint>
#include <iostream>

#include "pipe.h"
#include "vncProxy.h"

/* pipeCheckerNone: un checker pour le pipe innutile.
 * char *data: pointeur vers le buffer
 * uint16_t dataSize: taille des données lues
 * PipeConfiguration *config: pointeur vers le configuration du pipe
 * Retourne un uint8_t: Toujours 0
 */
#pragma GCC diagnostic ignored "-Wunused-parameter" // Pas de warning unused-parameter car c'est le "but" de la fonction
uint8_t pipeCheckerNone(char *data, uint16_t dataSize, PipeConfiguration *config){
	return 0;
}
#pragma GCC diagnostic pop

/* createPipe: Créer un pipe ansi que son thread
 * SOCKET src: Socket où les données vont être lues
 * SOCKET dst: Socket où les données vont être écrites
 * uint32_t pairId: Identifiant de la paire de pipe
 * VncProxy *proxy: Pointeur vers le VncProxy
 * uint8_t way: Sens du pipe (PIPE_WAY_ITALC_TO_LIBVNC ou PIPE_WAY_LIBVNC_TO_ITALC)
 * Retourne un PipeConfiguration*: la configuration du pipe construit
 */
PipeConfiguration* createPipe(SOCKET src, SOCKET dest, uint32_t pairId, VncProxy *proxy, uint8_t way){
	PipeConfiguration* config = new PipeConfiguration;
	config->src = src;
	config->dest = dest;
	config->pairId = pairId;
	config->proxy = proxy;
	if(way == PIPE_WAY_ITALC_TO_LIBVNC){
		config->checker = &pipeCheckerNone;
	}
	else if(way == PIPE_WAY_LIBVNC_TO_ITALC){
		config->checker = &pipeCheckerNone;
	}

	config->threadContinue = 1;
	config->pipeThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) pipeThread, (LPVOID) config, 0, NULL);
	return config;
}

/* stopPipe: arrête un pipe
 * PipeConfiguration *config: Configuration du pipe à arreter
 * Ne retourne rien
 */
void stopPipe(PipeConfiguration *config){
	config->threadContinue = 0;
	CloseHandle(config->pipeThread);
	// Le PipeConfiguration est free par pipeThread lui-meme
}

/* pipeThread: thread d'un pipe
 * LPVOID lpParameter: Paramètre du thread, interprété en tant que PipeConfiguration* pour la configuration du pipe
 * Retourne un uint32_t: 0 en cas de réussite, 1 en cas d'erreur
 */
uint32_t WINAPI pipeThread(LPVOID lpParameter){
	PipeConfiguration* config = (PipeConfiguration*) lpParameter;
	char buffer[8192];
	std::cout << "[Pipe] New pipe pairId:" << config->pairId << " src:" << config->src << " dest:" << config->dest << std::endl;
	while(config->threadContinue){
		int16_t bytesRead = recv(config->src, buffer, 8192, 0);
		if(bytesRead >= 1){
			// Appel du checker => Il retourne 0 si le packet doit être retransmi
			uint8_t pipeIt = (*config->checker)(buffer, bytesRead, config);
			if(pipeIt == 0){
				send(config->dest, buffer, bytesRead, 0);
			}
		}
		else if(bytesRead < 0){
			// recv retourne -1 si le socket est déconnecté => on en informe le VncProxy
			((VncProxy*) (config->proxy))->stopPair(config->pairId);
		}
	}
	// Atteinds lorsque stopPipe met config->threadContinue à 0 => On retire de la heap la config
	std::cout << "[Pipe] Pipe closed pairId:" << config->pairId << " src:" << config->src << " dest:" << config->dest << std::endl;
	delete config;
	return 0;
}