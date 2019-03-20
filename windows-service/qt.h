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
#ifndef QT_H
#define QT_H

#define QVARIANT_INT 2
#define QVARIANT_STRING 10

uint32_t recvQVariantHeader(rfbClientPtr sockIn);
void sendQVariantHeader(rfbClientPtr sockOut, uint32_t qtype);
uint32_t recvQInt(rfbClientPtr sockIn);
void sendQInt(rfbClientPtr sockOut, uint32_t qint);
std::string recvQString(rfbClientPtr sockIn);
void sendQString(rfbClientPtr sockOut, std::string stringIn);
#endif