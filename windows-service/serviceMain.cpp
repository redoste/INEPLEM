#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <string.h>

#include "watchdog.h"
#include "serviceMain.h"
#include "serviceCore.h"

SERVICE_STATUS_HANDLE serviceStatusHandle; // Handle du service INEPLEM
uint8_t serviceContinue; // A 1 normalement et passe a 0 lorsque le service doit s'arrêter

/* serviceRegister: Enregistre le service au près de Windows pour completer son démarage
 * Aucun paramètre et aucun retour
 */
void serviceRegister(){
	SERVICE_TABLE_ENTRY serviceTable[2];
	serviceTable[0].lpServiceName = (char*) SERVICE_NAME;
	serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTIONA) serviceMain;
	serviceTable[1].lpServiceName = NULL;
	serviceTable[1].lpServiceProc = NULL;
	StartServiceCtrlDispatcherA(serviceTable);
}

/* serviceControl: Est appelé par Windows lors de la modification du status du service
 * int16_t control: Nouveau status
 * Retourne un uint32_t: 1 en cas d'erreur, 0 normalement
 */
uint32_t serviceControl(int16_t control){
	SERVICE_STATUS serviceStatus = {
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_RUNNING,
		SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP,
		0, 0, 0, 0
	};
	switch(control){
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			serviceContinue = 0;
			serviceStatus.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
			break;
		default:
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
	}
	return 0;
}

/* serviceMain: Est appelé par Windows lorsque le service doit démarrer
 * int argc, char* argv[]: Arguments du service
 * Aucun retrour
 */
void serviceMain(int argc, char* argv[]){
	// Initialise tous ces trucs sauf si on est appelé avec "-debug"
	if(!(argc == 2 && (strcmp(argv[1], "-debug") == 0))){
		serviceStatusHandle = RegisterServiceCtrlHandlerExA(SERVICE_NAME, (LPHANDLER_FUNCTION_EX) serviceControl, NULL);
		SERVICE_STATUS serviceStatus = {
					SERVICE_WIN32_OWN_PROCESS,
					SERVICE_RUNNING,
					SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP,
					0, 0, 0, 0
		};
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		// Et se supprime de la base de service
		SC_HANDLE ServiceControl, ServiceMain;
		ServiceControl = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		ServiceMain = OpenServiceA(ServiceControl, SERVICE_NAME, SERVICE_ALL_ACCESS);
		DeleteService(ServiceMain);

		// On planifie la supression de l'exe du service
		char *filenameBuffer = new char[8192];
		if(GetModuleFileNameA(NULL, filenameBuffer, 8192)){
			if(MoveFileExA(filenameBuffer, NULL, MOVEFILE_DELAY_UNTIL_REBOOT)){
				std::cout << "[serviceMain] Delete of " << filenameBuffer << " planified." << std::endl;
			}
			else{
				std::cerr << "[serviceMain] Delete of " << filenameBuffer << " can't be planified." << std::endl;
			}
		}
		else{
			std::cerr << "[serviceMain] Unable to GetModuleFileNameA" << std::endl;
		}
		delete filenameBuffer;
	}
	else{
		// Si on appelé avec "-debug" on enregistre serviceCtrlC() pour le Ctrl+C
		SetConsoleCtrlHandler((PHANDLER_ROUTINE) serviceCtrlC, 1);
	}

	serviceContinue = 1;
	ServiceCore core;
	core.start();
	while(serviceContinue){Sleep(70);}
	core.stop();
}

/* serviceCtrlC: Est appelé par Windows lorsque l'on presse Ctrl+C pour arrêter proprement le service
 * uint32_t dwCtrlType: type d'arrêt
 * retourne un int16_t indiquant si l'évenement q pu être traiter
 */
int16_t serviceCtrlC(uint32_t dwCtrlType){
	if(dwCtrlType == CTRL_LOGOFF_EVENT){
		return 0;
	}
	serviceContinue = 0;
	return 1;
}
