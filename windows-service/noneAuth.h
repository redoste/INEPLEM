#ifndef NONEAUTH_H
#define NONEAUTH_H

#include <rfb/rfb.h>

void noneAuthHandler(rfbClientRec *client);
rfbSecurityHandler* noneAuthRegisterSecurity();

#endif