#ifndef PIPE_H
#define PIPE_H

#include <Windows.h>
#include <cstdint>

#define PIPE_WAY_ITALC_TO_LIBVNC 0x00
#define PIPE_WAY_LIBVNC_TO_ITALC 0x01

// On n'inclus pas directement "vncProxy.h" pour évitez une "boucle" de strcture (co-dépandance)
class VncProxy;

/* Structure PipeConfiguration: Représante la configuration d'un pipe
 */
typedef struct PipeConfiguration{
	SOCKET src;
	SOCKET dest;
	uint32_t pairId;
	VncProxy *proxy;
	uint8_t (*checker)(char*, uint16_t, PipeConfiguration*);
	uint8_t threadContinue;
	HANDLE pipeThread;
} PipeConfiguration;

uint32_t WINAPI pipeThread(LPVOID lpParameter);
uint8_t pipeCheckerNone(char *data, uint16_t dataSize, PipeConfiguration *config);
PipeConfiguration* createPipe(SOCKET src, SOCKET dest, uint32_t pairId, VncProxy *proxy, uint8_t way);
void stopPipe(PipeConfiguration *config);

#endif