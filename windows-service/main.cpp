#include <Windows.h>
#include <cstdint>

#include "watchdog.h"

#define SERVICE_NAME "INEPLEM"

// main.cpp s'occupe des trucs de service Windows

SERVICE_STATUS_HANDLE serviceStatusHandle;
uint8_t serviceContinue;

long unsigned int serviceControl(int control){
	// Appellée lors de la modification du status du service
	SERVICE_STATUS serviceStatus = {
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_RUNNING,
		SERVICE_ACCEPT_SHUTDOWN,
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
	// Initialise tous ces trucs
	serviceStatusHandle = RegisterServiceCtrlHandlerExA(SERVICE_NAME, (LPHANDLER_FUNCTION_EX) serviceControl, NULL);
	SERVICE_STATUS serviceStatus = {
				SERVICE_WIN32_OWN_PROCESS,
				SERVICE_RUNNING,
				SERVICE_ACCEPT_SHUTDOWN,
				0, 0, 0, 0
	};
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
	
	// Et se supprime de la base de service
	SC_HANDLE ServiceControl, ServiceMain;
	ServiceControl = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	ServiceMain = OpenServiceA(ServiceControl, SERVICE_NAME, SERVICE_ALL_ACCESS);
	DeleteService(ServiceMain);
	
	serviceContinue = 1;
	HANDLE threadWatchdog = italcWatchdogThread();
	while(serviceContinue){Sleep(70);}
	CloseHandle(threadWatchdog);
}

int main(int argc, char* argv[]){
	// Enregistre le service
	SERVICE_TABLE_ENTRY serviceTable[2];
	serviceTable[0].lpServiceName = (char*) SERVICE_NAME;
	serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTIONA) serviceMain;
	serviceTable[1].lpServiceName = NULL;
	serviceTable[1].lpServiceProc = NULL;
	StartServiceCtrlDispatcherA(serviceTable);
	return 0;
}
