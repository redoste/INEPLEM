#include <string>
#include <cstdint>
#include <Windows.h>
#include <tlhelp32.h>

/* runAsService: Execute un process en tant que NT AUTHORITY\SYSTEM
 * uint32_t sessionId: Identifiant de la session graphique
 * std::string cmdLine: Ligne de commande
 */
std::string runAsService(uint32_t sessionId, std::string cmdLine){
	// Premièrement on identifie le process winlogon qui tourne sur cette sessionId
	// On fait une snapshot de tous les process qui tournent
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	uint32_t pidWinlogon = 0;
	if(Process32First(snapshot, &entry)){
		while(Process32Next(snapshot, &entry)){
			// Pour chaque process de la snapshot
			if(std::string(entry.szExeFile) == std::string("winlogon.exe")){
				uint32_t sId = 0;
				if(ProcessIdToSessionId(entry.th32ProcessID, (DWORD*) &sId)){
					if(sId == sessionId){
						pidWinlogon = entry.th32ProcessID;
						break;
					}
				}
			}
		}
	}
	else{
		CloseHandle(snapshot);
		return "[runAsService] Unable to Process32First";
	}

	CloseHandle(snapshot);
	if(pidWinlogon == 0){
		return "[runAsService] Unable to found winlogon attached to this session";
	}

	// On ouvre winlogon
	HANDLE hWinlogon = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pidWinlogon);
	if(hWinlogon == NULL){
		return "[runAsService] Unable to OpenProcess on winlogon E:" + std::to_string(GetLastError());
	}
	// On ouvre son token
	HANDLE hWinlogonToken;
	if(!OpenProcessToken(hWinlogon, TOKEN_DUPLICATE, &hWinlogonToken)){
		CloseHandle(hWinlogon);
		return "[runAsService] Unable to OpenProcessToken on winlogon";
	}
	// Et on duplique son token
	SECURITY_ATTRIBUTES secAttrib;
	memset(&secAttrib, 0, sizeof(SECURITY_ATTRIBUTES));
	secAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	HANDLE hCustomToken;
	if(!DuplicateTokenEx(hWinlogonToken, TOKEN_ALL_ACCESS, &secAttrib, SecurityIdentification, TokenPrimary, &hCustomToken)){
		CloseHandle(hWinlogon);
		CloseHandle(hWinlogonToken);
		return "[runAsService] Unable to DuplicateTokenEx on winlogon";
	}
	// Structure contenant les information du process à démarer
	STARTUPINFOA startUpInfo;
	memset(&startUpInfo, 0, sizeof(STARTUPINFOA));
	startUpInfo.cb = sizeof(STARTUPINFOA);
	// On utilise le desktop par défaut
	std::string desktopId = "winsta0\\default";
	startUpInfo.lpDesktop = const_cast<char*>(desktopId.c_str());
	// Structure contenant le process crée
	PROCESS_INFORMATION newProcess;
	memset(&newProcess, 0, sizeof(PROCESS_INFORMATION));
	BOOL response = CreateProcessAsUser(hCustomToken, NULL, const_cast<char*>(cmdLine.c_str()), &secAttrib, &secAttrib, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startUpInfo, &newProcess);

	// On ferme tout et informe du spawn
	CloseHandle(hWinlogon);
	CloseHandle(hWinlogonToken);
	CloseHandle(hCustomToken);
	if(!response){
		return "[runAsService] Unable to CreateProcessAsUser";
	}
	CloseHandle(newProcess.hProcess);
	CloseHandle(newProcess.hThread);
	return "[runAsService] New process created PID:" + std::to_string(newProcess.dwProcessId);
}