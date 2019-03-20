// Copyright: 2019 redoste
/*
*	This file is part of INEPLEM.
*
*	INEPLEM is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	INEPLEM is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with INEPLEM.  If not, see <https://www.gnu.org/licenses/>.
*/
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