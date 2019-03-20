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
#ifndef ITALCEXTENSION_H
#define ITALCEXTENSION_H

#include <rfb/rfb.h>

#include "serviceCore.h"

rfbBool italcExtensionHandleClient(rfbClientRec *client, void **);
rfbBool italcExtensionHandleMessage(rfbClientRec *client, void *data, const rfbClientToServerMsg *message);
void italcExtensionRegister(ServiceCore *service);
void italcResponseUserInformation(rfbClientRec *client);
void italcResponseUserInformationWithService(rfbClientRec *client, ServiceCore *service);

#endif