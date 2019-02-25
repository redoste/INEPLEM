#include <rfb/rfb.h>
#include <cstdint>
#include <iostream>

#include "serviceCore.h"
#include "noneAuth.h"

/* noneAuthHandler: Handler pour l'authentidication NoneAuth
 * rfbClientRec* client: Pointeur vers les informations du client
 */
void noneAuthHandler(rfbClientRec *client){
	uint32_t authResult = 0;
	rfbWriteExact(client, (char*) &authResult, 4);
	client->state = rfbClientRec::RFB_INITIALISATION;
	std::cout << "[noneAuthHandler] New client choose NoneAuth" << std::endl;
}

/* noneAuthRegisterSecurity: Enregistre le security handler pour l'authentification NoneAuth
 * retourne un rfbSecurityHandler*: Un pointeur vers le security handler enregistré
 */
rfbSecurityHandler* noneAuthRegisterSecurity(){
	// On le new car sinon il est "déalloué" à la fin de la fonction => SegFault
	rfbSecurityHandler *noneAuthSecurityHandler = new rfbSecurityHandler;
	memset(noneAuthSecurityHandler, 0, sizeof(rfbSecurityHandler));
	noneAuthSecurityHandler->type = VNC_NONEAUTH;
	noneAuthSecurityHandler->handler = &noneAuthHandler;

	rfbRegisterSecurityHandler(noneAuthSecurityHandler);
	return noneAuthSecurityHandler;
}