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