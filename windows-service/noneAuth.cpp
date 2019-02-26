#include <rfb/rfb.h>
#include <cstdint>
#include <iostream>

#include "serviceCore.h"
#include "noneAuth.h"

// noneAuthServiceCorePtr: Pointeur vers le ServiceCore utilisé par noneAuthHandler pour connaitre la AuthResponse
ServiceCore *noneAuthServiceCorePtr = NULL;

/* noneAuthHandler: Handler pour l'authentidication NoneAuth
 * rfbClientRec* client: Pointeur vers les informations du client
 */
void noneAuthHandler(rfbClientRec *client){
	uint32_t authResult = noneAuthServiceCorePtr->getAuthresponse();
	uint32_t authResultToSend = Swap32IfLE(authResult);
	rfbWriteExact(client, (char*) &authResultToSend, 4);
	if(authResult == VNC_ACCEPT){
		client->state = rfbClientRec::RFB_INITIALISATION;
		std::cout << "[noneAuthHandler] New client choose NoneAuth : ACCEPTED" << std::endl;
	}
	else{
		//rfbClientSendString(client, (char*) "Authentification failed");
		std::cout << "[noneAuthHandler] New client choose NoneAuth : REJECTED" << std::endl;
		rfbCloseClient(client);
	}
}

/* noneAuthRegisterSecurity: Enregistre le security handler pour l'authentification NoneAuth
 * ServiceCore* service: Pointeur pour définir noneAuthServiceCorePtr
 * retourne un rfbSecurityHandler*: Un pointeur vers le security handler enregistré
 */
rfbSecurityHandler* noneAuthRegisterSecurity(ServiceCore* service){
	// On le new car sinon il est "déalloué" à la fin de la fonction => SegFault
	rfbSecurityHandler *noneAuthSecurityHandler = new rfbSecurityHandler;
	memset(noneAuthSecurityHandler, 0, sizeof(rfbSecurityHandler));
	noneAuthSecurityHandler->type = VNC_NONEAUTH;
	noneAuthSecurityHandler->handler = &noneAuthHandler;
	rfbRegisterSecurityHandler(noneAuthSecurityHandler);

	noneAuthServiceCorePtr = service;

	return noneAuthSecurityHandler;
}