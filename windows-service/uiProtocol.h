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
#ifndef UIPROTOCOL_H
#define UIPROTOCOL_H

#define U2S_STATUS 0x01
#define U2S_AUTHMETHOD 0x02
#define U2S_AUTHRESPONSE 0x03
#define U2S_USERNAME 0x04
//#define U2S_LOG 0x05
#define U2S_CREDS 0x06
#define U2S_FRAMES 0x07
#define U2S_FORCE_DISCONNECT 0x08
#define U2S_LAUNCH_PROCESS 0x09

#define S2U_STATUS 0xF1
#define S2U_AUTHMETHOD 0xF2
#define S2U_AUTHRESPONSE 0xF3
#define S2U_USERNAME 0xF4
//#define S2U_LOG 0xF5
#define S2U_CREDS 0xF6
#define S2U_NOTIFICATION 0xFE

#endif