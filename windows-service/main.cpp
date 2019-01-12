#include <iostream>
#include <string>
#include <Windows.h>

SERVICE_STATUS_HANDLE serviceStatusHandle;

int italcWatchdog(){
	SC_HANDLE ServiceControl, ServiceItalc;
	SERVICE_STATUS ItalcStatus = {};
	int error;
	std::string error_name;
	
	
	ServiceControl = OpenSCManagerA(NULL, NULL, GENERIC_READ);
	if(ServiceControl == NULL){
		error = GetLastError();
		if(error == ERROR_ACCESS_DENIED) error_name = std::string("ERROR_ACCESS_DENIED");
		else error_name = std::string("UNKNOWN ERROR");
		std::cout << "[WATCHDOG] Unable to open ServiceControl: " << error_name << std::endl;
		return 1;
	}
	std::cout << "[WATCHDOG] ServiceControl opened." << std::endl;
	
	ServiceItalc = OpenServiceA(ServiceControl, "icas", SERVICE_STOP | SERVICE_QUERY_STATUS);
	if(ServiceItalc == NULL){
		error = GetLastError();
		if(error == ERROR_ACCESS_DENIED) error_name = std::string("ERROR_ACCESS_DENIED");
		else if(error == ERROR_INVALID_NAME) error_name = std::string("ERROR_INVALID_NAME");
		else if(error == ERROR_SERVICE_DOES_NOT_EXIST) error_name = std::string("ERROR_SERVICE_DOES_NOT_EXIST");
		else error_name = std::string("UNKNOWN ERROR");
		std::cout << "[WATCHDOG] Unable to open ServiceItalc: " << error_name << std::endl;
		return 1;
	}
	std::cout << "[WATCHDOG] ServiceItalc opened." << std::endl;
	
	while(1){
		error = QueryServiceStatus(ServiceItalc, &ItalcStatus);
		if(!error){
			error = GetLastError();
			std::cout << "[WATCHDOG] Unable to get italc status: UNKNOWN ERROR ("<< error << ")" << std::endl;
			return 1;
		}
		
		switch(ItalcStatus.dwCurrentState){
			case SERVICE_STOPPED:
			case SERVICE_STOP_PENDING:
				Sleep(1000);
				break;
			case SERVICE_RUNNING:
			case SERVICE_START_PENDING:
			case SERVICE_CONTINUE_PENDING:
			case SERVICE_PAUSE_PENDING:
			case SERVICE_PAUSED:
				if(!ControlService(ServiceItalc, SERVICE_CONTROL_STOP, &ItalcStatus)){
					error = GetLastError();
					if(error == ERROR_DEPENDENT_SERVICES_RUNNING) error_name = std::string("ERROR_DEPENDENT_SERVICES_RUNNING");
					else error_name = std::string("UNKNOWN ERROR");
					std::cout << "[WATCHDOG] Unable to stop italc: " << error_name << " (" << error << ")" << std::endl;
				}
				else{
					std::cout << "[WATCHDOG] Italc is stopping..." << std::endl;
				}
				Sleep(250);
				break;
		}
	}
	return 0;
}

long unsigned int serviceControl(int control){
	SERVICE_STATUS serviceStatus = {
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_RUNNING,
		SERVICE_ACCEPT_SHUTDOWN,
		0, 0, 0, 0
	};
	switch(control){
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			serviceStatus.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
			break;
		default:
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
	}
	return 0;
}

void serviceMain(int argc, char* argv[]){
	// Windows Services stuff...
	serviceStatusHandle = RegisterServiceCtrlHandlerExA("INEPLEM", (LPHANDLER_FUNCTION_EX) serviceControl, NULL);
	SERVICE_STATUS serviceStatus = {
				SERVICE_WIN32_OWN_PROCESS,
				SERVICE_RUNNING,
				SERVICE_ACCEPT_SHUTDOWN,
				0, 0, 0, 0
	};
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
	
	SC_HANDLE ServiceControl, ServiceMain;
	ServiceControl = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	ServiceMain = OpenServiceA(ServiceControl, "INEPLEM", SERVICE_ALL_ACCESS);
	DeleteService(ServiceMain);
	
	italcWatchdog();
}

int main(int argc, char* argv[]){	
	SERVICE_TABLE_ENTRY serviceTable[2];
	serviceTable[0].lpServiceName = (char*) "INEPLEM";
	serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTIONA) serviceMain;
	serviceTable[1].lpServiceName = NULL;
	serviceTable[1].lpServiceProc = NULL;
	StartServiceCtrlDispatcherA(serviceTable);
	return 0;
}