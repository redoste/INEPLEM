#include <iostream>
#include <Windows.h>
#include <string>

#include "watchdog.h"

HANDLE italcWatchdogThread(){
	// Créer un thread pour italcWatchdog();
	HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) italcWatchdog, NULL, 0, NULL);
	return thread;
}

uint32_t WINAPI italcWatchdog(LPVOID lpParameter){
	SC_HANDLE ServiceControl, ServiceItalc;
	SERVICE_STATUS ItalcStatus = {};
	int16_t error;
	std::string error_name;

	// Service Manager pour lire des infos sur les services
	ServiceControl = OpenSCManagerA(NULL, NULL, GENERIC_READ);
	if(ServiceControl == NULL){
		error = GetLastError();
		if(error == ERROR_ACCESS_DENIED) error_name = std::string("ERROR_ACCESS_DENIED");
		else error_name = std::string("UNKNOWN ERROR");
		std::cerr << "[WATCHDOG] Unable to open ServiceControl: " << error_name << std::endl;
		return 1;
	}
	std::cout << "[WATCHDOG] ServiceControl opened." << std::endl;

	// Ouverture du service icas Italc Client Service
	ServiceItalc = OpenServiceA(ServiceControl, "icas", SERVICE_STOP | SERVICE_QUERY_STATUS);
	if(ServiceItalc == NULL){
		error = GetLastError();
		if(error == ERROR_ACCESS_DENIED) error_name = std::string("ERROR_ACCESS_DENIED");
		else if(error == ERROR_INVALID_NAME) error_name = std::string("ERROR_INVALID_NAME");
		else if(error == ERROR_SERVICE_DOES_NOT_EXIST) error_name = std::string("ERROR_SERVICE_DOES_NOT_EXIST");
		else error_name = std::string("UNKNOWN ERROR");
		std::cerr << "[WATCHDOG] Unable to open ServiceItalc: " << error_name << std::endl;
		return 1;
	}
	std::cout << "[WATCHDOG] ServiceItalc opened." << std::endl;

	while(1){
		// On met a jour le status d'icas
		error = QueryServiceStatus(ServiceItalc, &ItalcStatus);
		if(!error){
			error = GetLastError();
			std::cerr << "[WATCHDOG] Unable to get italc status: UNKNOWN ERROR ("<< error << ")" << std::endl;
			return 1;
		}

		switch(ItalcStatus.dwCurrentState){
			// Il est arrêté, on Sleep 1 seconde
			case SERVICE_STOPPED:
			case SERVICE_STOP_PENDING:
				Sleep(1000);
				break;
			// Il tourne ou s'aprète a démarrer
			case SERVICE_RUNNING:
			case SERVICE_START_PENDING:
			case SERVICE_CONTINUE_PENDING:
			case SERVICE_PAUSE_PENDING:
			case SERVICE_PAUSED:
				// On le stop
				if(!ControlService(ServiceItalc, SERVICE_CONTROL_STOP, &ItalcStatus)){
					error = GetLastError();
					if(error == ERROR_DEPENDENT_SERVICES_RUNNING) error_name = std::string("ERROR_DEPENDENT_SERVICES_RUNNING");
					else error_name = std::string("UNKNOWN ERROR");
					std::cerr << "[WATCHDOG] Unable to stop italc: " << error_name << " (" << error << ")" << std::endl;
				}
				else{
					std::cout << "[WATCHDOG] Italc is stopping..." << std::endl;
				}
				// Et on Sleep 250ms
				Sleep(250);
				break;
		}
	}
	return 0;
}
