#ifndef ITALCEXTENSION_H
#define ITALCEXTENSION_H

#include <rfb/rfb.h>

rfbBool italcExtensionHandleClient(rfbClientRec *client, void **data);
rfbBool italcExtensionHandleMessage(rfbClientRec *client, void *data, const rfbClientToServerMsg *message);
void italcExtensionRegister();

#endif