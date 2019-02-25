#ifndef MSAUTH_H
#define MSAUTH_H

#include <rfb/rfb.h>
#include <cstdint>

#include "serviceCore.h"

void vncDecryptBytes(char *output, uint16_t length, unsigned char *key);
void msAuthIIHandler(rfbClientRec *client);
rfbSecurityHandler* msAuthIIRegisterSecurity(ServiceCore* service);
#endif