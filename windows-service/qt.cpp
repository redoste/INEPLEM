#include <iostream>
#include <cstdint>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <rfb/rfb.h>

#include "qt.h"

// ATTENTION: Dans certain cas, le terme Socket est utilisé dans ce fichier pour désigner des rfbClientPtr

/* recvQVariantHeader: Reçois le type d'un QVariant via un socket
 * rfbClientPtr sockIn: Socket pour la reception
 * Retourne un uint32_t: le type du QVariant
 */
uint32_t recvQVariantHeader(rfbClientPtr sockIn){
	char unkVal;
	uint32_t QVariantType = recvQInt(sockIn);
	rfbReadExact(sockIn, &unkVal, 1);
	return QVariantType;
}

/* sendQVariantHeader: Envois le type d'un QVariant via un socket
 * SOCKET sockOut: Socket pour l'envois
 * uint32_t qtype: type du QVarriant
 */
void sendQVariantHeader(SOCKET sockOut, uint32_t qtype){
	char unkVal = 0;
	sendQInt(sockOut, qtype);
	send(sockOut, &unkVal, 1, 0);
}

/* recvQInt: Reçois un QInt via un socket
 * rfbClientPtr sockIn: Socket pour la reception
 * Retourne un uint32_t: le QInt
 */
uint32_t recvQInt(rfbClientPtr sockIn){
	char qintBuffer[4];
	rfbReadExact(sockIn, qintBuffer, 4);
	// Ce "roullement" d'octet est requis car les QInt sont en Big-Endian contrairement au CPU x86 qui sont en Little-endian.
	return qintBuffer[3] + (qintBuffer[2] << 8) + (qintBuffer[1] << 16) + (qintBuffer[0] << 24);
}

/* sendQInt: Envois un QInt via un socket
 * SOCKET sockOut: Socket pour l'envois
 * uint42_t qint: QInt à envoyer
 */
void sendQInt(SOCKET sockOut, uint32_t qint){
	char qintBEBuffer[4];
	char *qintLEBuffer = (char*) &qint;
	// Ce "roullement" d'octet est requis car les QInt sont en Big-Endian contrairement au CPU x86 qui sont en Little-endian.
	qintBEBuffer[0] = qintLEBuffer[3];
	qintBEBuffer[1] = qintLEBuffer[2];
	qintBEBuffer[2] = qintLEBuffer[1];
	qintBEBuffer[3] = qintLEBuffer[0];
	send(sockOut, qintBEBuffer, 4, 0);
}

/* recvQString: Reçois une QString via un socket
 * rfbClientPtr sockIn: Socket pour la reception
 * Retourne une std::string: la QString
 */
std::string recvQString(rfbClientPtr sockIn){
	int32_t stringSize = (int32_t) recvQInt(sockIn);
	if(stringSize <= 0){
		std::cerr << "[recvQString] Empty string len:" << stringSize << std::endl;
		return std::string("");
	}
	char* buffer = new char[stringSize];
	rfbReadExact(sockIn, buffer, stringSize);

	// On retire les 0x00 inutiles pour faire une "conversion" (très salle) de l'UTF-16 (Big-Endian) en UTF-8
	char outString[stringSize / 2 + 1];
	for(int32_t i = 1; i <= stringSize / 2; i++){
		outString[i - 1] = buffer[i * 2 - 1];
	}
	outString[stringSize / 2] = '\0';
	delete buffer;
	return std::string(outString);
}

/* sendQString: Envois une QString via un socket
 * SOCKET sockOut: Socket pour l'envois
 * std::string stringIn: QString à envoyer
 */
void sendQString(SOCKET sockOut, std::string stringIn){
	char buffer[8192];
	uint32_t finalSize = stringIn.length() * 2;
	if(finalSize > 8192){
		// [TODO] : Buffer Dynamique
		std::cerr << "[sendQString] String too big" << std::endl;
	}
	else{
		// On ajoute des 0x00 de manière à faire une "conversion" (très salle) de l'UTF-8 à l'UTF-16 (Big-Endian)
		for(uint32_t i = 0; i < stringIn.length(); i++){
			buffer[i * 2] = '\0';
			buffer[i * 2 + 1] = stringIn[i];
		}

		sendQInt(sockOut, finalSize);
		send(sockOut, buffer, finalSize, 0);
	}
}