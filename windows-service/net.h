#ifndef NET_H
#define NET_H

#include <cstdint>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

uint16_t initWSocket();
SOCKET createClientConnection(std::string ip, uint16_t port);
SOCKET createServerConnection(std::string ip, uint16_t port);
void whitelistFirewall();
#endif