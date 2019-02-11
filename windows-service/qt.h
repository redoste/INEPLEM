#ifndef QT_H
#define QT_H

#define QVARIANT_INT 2
#define QVARIANT_STRING 10

uint32_t recvQVariantHeader(SOCKET sockIn);
void sendQVariantHeader(SOCKET sockOut, uint32_t qtype);
uint32_t recvQInt(SOCKET sockIn);
void sendQInt(SOCKET sockOut, uint32_t qint);
std::string recvQString(SOCKET sockIn);
void sendQString(SOCKET sockOut, std::string stringIn);
#endif