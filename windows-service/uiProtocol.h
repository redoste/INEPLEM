#ifndef UIPROTOCOL_H
#define UIPROTOCOL_H

#define U2S_STATUS 0x01
#define U2S_AUTHMETHOD 0x02
#define U2S_AUTHRESPONSE 0x03
#define U2S_USERNAME 0x04
//#define U2S_LOG 0x05
#define U2S_CREDS 0x06
#define U2S_FRAMES 0x07

#define S2U_STATUS 0xF1
#define S2U_AUTHMETHOD 0xF2
#define S2U_AUTHRESPONSE 0xF3
#define S2U_USERNAME 0xF4
//#define S2U_LOG 0xF5
#define S2U_CREDS 0xF6
#define S2U_NOTIFICATION 0xFE

#endif