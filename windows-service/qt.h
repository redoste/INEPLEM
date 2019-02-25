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