#include <Windows.h>
#include <cstdint>
#include <string.h>

#include "watchdog.h"
#include "serviceMain.h"

SERVICE_STATUS_HANDLE serviceStatusHandle;
uint8_t serviceContinue;

void serviceRegister(){
	SERVICE_TABLE_ENTRY serviceTable[2];
	serviceTable[0].lpServiceName = (char*) SERVICE_NAME;
	serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTIONA) serviceMain;
	serviceTable[1].lpServiceName = NULL;
	serviceTable[1].lpServiceProc = NULL;
	StartServiceCtrlDispatcherA(serviceTable);
}

uint32_t serviceControl(int16_t control){
	// Appellée lors de la modification du status du service
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

void serviceMain(int argc, char* argv[]){
	// Appellée au main du service
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
	}

	serviceContinue = 1;
	HANDLE threadWatchdog = italcWatchdogThread();
	while(serviceContinue){Sleep(70);}
	CloseHandle(threadWatchdog);
}
