#ifndef ITALCEXTENSION_H
#define ITALCEXTENSION_H

#include <rfb/rfb.h>

#include "serviceCore.h"

rfbBool italcExtensionHandleClient(rfbClientRec *client, void **data);
rfbBool italcExtensionHandleMessage(rfbClientRec *client, void *data, const rfbClientToServerMsg *message);
void italcExtensionRegister(ServiceCore *service);
void italcResponseUserInformation(rfbClientRec *client);

#endif