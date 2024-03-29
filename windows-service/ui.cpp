// Copyright: 2019 redoste
/*
*	This file is part of INEPLEM.
*
*	INEPLEM is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	INEPLEM is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with INEPLEM.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <string.h>
#include <iostream>
#include <map>
#include <windows.h>

#include "ui.h"
#include "uiToService.h"
#include "serviceCore.h"
#include "resource.h"

// ineplemUiPtr: Pointeur vers l'Ui utilisé par uiWindowCallback pour appeler processWindowMessage
Ui* ineplemUiPtr;

/* uiWindowCallback: Fonction appelée lors d'un nouveau message sur la fenètre, transfert vers Ui::processWindowMessage
 * HWND window: Fenetre
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne une valeur spécifique selon le message
 */
LRESULT CALLBACK uiWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
	return ineplemUiPtr->processWindowMessage(window, message, wParam, lParam);
}

/* uiUsernameDialogCallback: Fonction appelée lors d'un nouveau message sur la dialog de changement de username, transfert vers Ui::processUsernameDialogMessage
 * HWND window: Dialog
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne 1 si le message a été traité, 0 sinon
 */
INT_PTR CALLBACK uiUsernameDialogCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
	return ineplemUiPtr->processUsernameDialogMessage(window, message, wParam, lParam);
}

/* uiAddressDialogCallback: Fonction appelée lors d'un nouveau message sur la dialog d'addresse, transfert vers Ui::processAddressDialogMessage
 * HWND window: Dialog
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne 1 si le message a été traité, 0 sinon
 */
INT_PTR CALLBACK uiAddressDialogCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
	return ineplemUiPtr->processAddressDialogMessage(window, message, wParam, lParam);
}

/* uiCredsDialogCallback: Fonction appelée lors d'un nouveau message sur la dialog de mot de passe pour les creds, transfert vers Ui::processCredsDialogMessage
 * HWND window: Dialog
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne 1 si le message a été traité, 0 sinon
 */
INT_PTR CALLBACK uiCredsDialogCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
	return ineplemUiPtr->processCredsDialogMessage(window, message, wParam, lParam);
}

/* uiRunasDialogCallback: Fonction appelée lors d'un nouveau message sur la dialog de runas, transfert vers Ui::processRunasDialogMessage
 * HWND window: Dialog
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne 1 si le message a été traité, 0 sinon
 */
INT_PTR CALLBACK uiRunasDialogCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
	return ineplemUiPtr->processRunasDialogMessage(window, message, wParam, lParam);
}

/* checkPassword: Vérifie le mot de passe via une fonction de checksum TRES PEU SECURE
 * char *password: Pointeur vers les carractère de la chaine
 * uint16_t passwordLen: Taille du mot de passe
 * retourne un uint8_t: 0 si le mot de passe est faux, 1 sinon
 */
uint8_t checkPassword(char *password, uint16_t passwordLen){
	// Solution EXTREMEMENT secure pour stocker un mot de passe -_-
	// NE FAITES JAMAIS CA
	uint16_t sum = 0;
	for(uint16_t i = 0; i < passwordLen; i++){
		sum += password[i];
	}
	return sum == UI_PASSWORD;
}

/* Ui::Ui: Constructeur de Ui
 * uint16_t port: Port de la connection TCP avec le service
 */
Ui::Ui(uint16_t port){
	std::cout << "[UI] Ui started on port: " << port << std::endl;
	ineplemUiPtr = this;

	// On initialise les différents composant
	this->m_moduleHandle = GetModuleHandle(NULL);
	this->m_nidMessage = RegisterWindowMessageA(UI_NID_MSG);

	this->registerClass();
	this->createWindow();

	this->createMenu();

	this->createTray();
	this->enableTray();

	this->m_uiToService = new UiToService(port, this);
	this->m_uiToService->startThread();
}

/* Ui::registerClass: Enregistre la classe de fenetre aupres du systeme
 * Aucun paramètre ni retour
 */
void Ui::registerClass(){
	WNDCLASSEX windowClass;
	memset(&windowClass, 0, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	// On y définis le callback pour les messages
	windowClass.lpfnWndProc = &uiWindowCallback;
	windowClass.hInstance = this->m_moduleHandle;
	windowClass.lpszClassName = UI_CLASS;
	if(!RegisterClassExA(&windowClass)){
		std::cerr << "[UI] Unable to Register WNDCLASSEX E:" << GetLastError() << std::endl;
	}
}

/* Ui::createWindow: Crée une fenetre invisible sur la quelle repose notre menu et notre icone dans la tray
 * Aucun paramètre ni retour
 */
void Ui::createWindow(){
	this->m_window = CreateWindowEx(0, UI_CLASS, UI_CLASS, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
}

/* Ui::createTray: Crée l'icone dans la tray
 * Aucun paramètre ni retour
 */
void Ui::createTray(){
	memset(&this->m_nid, 0, sizeof(NOTIFYICONDATA));
	this->m_nid.cbSize = sizeof(NOTIFYICONDATA);
	this->m_nid.hWnd = this->m_window;
	this->m_nid.uID = UI_NID_UID;
	this->m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	// On y définis le message en cas de clic sur la tray
	this->m_nid.uCallbackMessage = this->m_nidMessage;
	this->m_nid.hIcon = (HICON) LoadImage(this->m_moduleHandle, "id", IMAGE_ICON, 128, 128, LR_DEFAULTCOLOR);
	strncpy(this->m_nid.szTip, "INEPLEM\0", 64);
}

/* Ui::enableTray: Fait apparêtre l'icone dans la tray
 * Aucun paramètre ni retour
 */
void Ui::enableTray(){
	Shell_NotifyIcon(NIM_ADD, &this->m_nid);
}

/* Ui::processWindowMessage: Fonction appelée lors d'un nouveau message sur la fenètre, appelée via uiWindowCallback
 * HWND window: Fenetre
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne une valeur spécifique selon le message
 */
int32_t Ui::processWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
	if(message == WM_ACTIVATE){
		// Quand la fenetre est initialisé, on vérifie que la tray est affiché
		this->enableTray();
	}
	else if(message == this->m_nidMessage && lParam == WM_LBUTTONUP){
		// Quand on clique avec le boutton gauche sur l'icone, on fait apparaitre le menu
		this->popoutMenu();
	}
	else if(message == WM_COMMAND){
		// Lorsque une commande est activée, on la traite
		this->processItem(LOWORD(wParam));
	}
	// Gère les message qui n'ont pas été traité
	return DefWindowProc(window, message, wParam, lParam);
}

/* Ui::processMessage: Met a jour la liste de message et les traites
 * Aucun paramètre ni retour
 */
void Ui::processMessage(){
	MSG message;
	GetMessage(&message, NULL, 0, 0);
	TranslateMessage(&message);
	DispatchMessage(&message);
}

/* Ui::popoutMenu: Fait apparaite le menu au niveau du curseur de la souris
 * Aucun paramètre ni retour
 */
void Ui::popoutMenu(){
	POINT iconPos;
	memset(&iconPos, 0, sizeof(POINT));
	GetCursorPos(&iconPos);

	// Les fonction SetForegroundWindow et PostMessage sont appelée car un bug de Windows peut faire glitcher le menu sinon
	SetForegroundWindow(this->m_window);
	TrackPopupMenu(this->m_menu, 0, iconPos.x, iconPos.y, 0, this->m_window, NULL);
	PostMessage(this->m_window, WM_NULL, 0, 0);
}

/* Ui::createMenu: Créer le menu de l'icone
 * Aucun paramètre ni retour
 */
void Ui::createMenu(){
	// Sous menu "Auth Method"
	this->m_authMethodMenu = CreatePopupMenu();
	AppendMenuA(this->m_authMethodMenu, MF_STRING, UI_MENU_AUTHMETHOD_NONE, "No Auth");
	AppendMenuA(this->m_authMethodMenu, MF_STRING, UI_MENU_AUTHMETHOD_MSII, "MS Auth II");

	// Sous menu "Auth Response"
	this->m_authResponseMenu = CreatePopupMenu();
	AppendMenuA(this->m_authResponseMenu, MF_STRING, UI_MENU_AUTHRESPONSE_REJECT, "REJECT");
	AppendMenuA(this->m_authResponseMenu, MF_STRING, UI_MENU_AUTHRESPONSE_ACCEPT, "ACCEPT");

	// Menu principal
	this->m_menu = CreatePopupMenu();
	AppendMenuA(this->m_menu, MF_STRING, UI_MENU_FBCONT_LOAD, "FrameBuffer Content Load");
	AppendMenuA(this->m_menu, MF_SEPARATOR, 0, NULL);
	AppendMenuA(this->m_menu, MF_POPUP, (UINT) this->m_authResponseMenu, "Auth Response");
	AppendMenuA(this->m_menu, MF_POPUP, (UINT) this->m_authMethodMenu, "Auth Method");
	AppendMenuA(this->m_menu, MF_STRING, UI_MENU_KILLCLIENTS, "Kill client(s)");
	AppendMenuA(this->m_menu, MF_SEPARATOR, 0, NULL);
	AppendMenuA(this->m_menu, MF_STRING, UI_MENU_CREDS, "Creds");
	AppendMenuA(this->m_menu, MF_STRING, UI_MENU_RUNAS, "Run As Service");
	AppendMenuA(this->m_menu, MF_STRING, UI_MENU_USERNAME, "Username");
	AppendMenuA(this->m_menu, MF_STRING, UI_MENU_STATUS, "Status");
}

/* Ui::msgBox: Affiche une boite de message
 * std::string text: Texte a afficher
 */
void Ui::msgBox(std::string text){
	MessageBox(this->m_window, text.c_str(), "INEPLEM", MB_OK);
}

/* Ui:updateMenuCheckboxes: Met a jour les checkbox du menu en fonction de m_checkboxesStatus
 * Aucun paramètre ni retour
 */
void Ui::updateMenuCheckboxes(){
	for(std::map<uint16_t, uint8_t>::iterator it = this->m_checkboxesStatus.begin(); it != this->m_checkboxesStatus.end(); it++){
		MENUITEMINFOA updatedInfo;
		memset(&updatedInfo, 0, sizeof(MENUITEMINFOA));
		updatedInfo.cbSize = sizeof(MENUITEMINFOA);
		updatedInfo.fMask = MIIM_STATE;
		updatedInfo.fState = it->second ? MFS_CHECKED : MFS_UNCHECKED;
		SetMenuItemInfoA(this->m_menu, it->first, FALSE, &updatedInfo);
	}
}

/* Ui::setCheckbox: Définis la valeur d'une checkbox
 * uint16_t key: Id de la checkbox
 * uint8_t value: Valeur de la checkbox
 */
void Ui::setCheckbox(uint16_t key, uint8_t value){
	this->m_checkboxesStatus[key] = value;
}

/* Ui:processItem: Traite un item sur lequel on a cliqué
 * uint16_t menuId: Identifiant de l'item
 */
void Ui::processItem(uint16_t menuId){
	switch(menuId){
		case UI_MENU_STATUS:
			this->m_uiToService->askStatus();
			break;
		case UI_MENU_AUTHRESPONSE_ACCEPT:
			this->m_uiToService->sendAuthresponse(VNC_ACCEPT);
			break;
		case UI_MENU_AUTHRESPONSE_REJECT:
			this->m_uiToService->sendAuthresponse(VNC_REJECT);
			break;
		case UI_MENU_AUTHMETHOD_MSII:
			this->m_uiToService->sendAuthmethod(VNC_MSAUTH);
			break;
		case UI_MENU_AUTHMETHOD_NONE:
			this->m_uiToService->sendAuthmethod(VNC_NONEAUTH);
			break;
		case UI_MENU_USERNAME:
			DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG1), this->m_window, &uiUsernameDialogCallback, 0);
			break;
		case UI_MENU_CREDS:
			DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG2), this->m_window, &uiCredsDialogCallback, 0);
			break;
		case UI_MENU_FBCONT_LOAD:
			this->readImageFromClipboard();
			break;
		case UI_MENU_KILLCLIENTS:
			DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG3), this->m_window, &uiAddressDialogCallback, 0);
			break;
		case UI_MENU_RUNAS:
			DialogBoxParamA(NULL, MAKEINTRESOURCEA(IDD_DIALOG4), this->m_window, &uiRunasDialogCallback, 0);
			break;
	}
}

/* Ui::processUsernameDialogMessage: Fonction appelée lors d'un nouveau message sur la dialog de changement de username, appelée via uiUsernameDialogCallback
 * HWND window: Dialog
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne 1 si le message a été traité, 0 sinon
 */
int16_t Ui::processUsernameDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM){
	if(message == WM_COMMAND){
		switch(LOWORD(wParam)){
			case IDOK:
				this->updateUsernameFromDialog(window);
			case IDCANCEL:
				EndDialog(window, 0);
				break;
		}
		return 1;
	}
	return 0;
}

/* Ui::updateUsernameFromDialog: Extrait le nom d'utilisateur de la dialog et l'envois au UiToSerivce
 * HWND dialog: Dialog
 */
void Ui::updateUsernameFromDialog(HWND dialog){
	char *buffer = new char[8192]; // Peut être rendue dynamique ?
	GetDlgItemTextA(dialog, IDUSERNAME, buffer, 8192);
	std::string username(buffer);
	delete buffer;
	this->m_uiToService->sendUsername(username);
}

/* Ui::processCredsDialogMessage: Fonction appelée lors d'un nouveau message sur la dialog de mot de passe pour les creds, appelée via uiCredsDialogCallback
 * HWND window: Dialog
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne 1 si le message a été traité, 0 sinon
 */
int16_t Ui::processCredsDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM){
	if(message == WM_COMMAND){
		if(LPARAM(wParam) == IDCANCEL){
			EndDialog(window, 0);
		}
		else if(LPARAM(wParam) == IDOK){
			char *buffer = new char[8192]; // Peur être rendue dynamique ?
			uint16_t passwordLen = GetDlgItemTextA(window, IDPASSWORD, buffer, 8192);
			EndDialog(window, 0);
			if(checkPassword(buffer, passwordLen)){
				this->m_uiToService->askCreds();
			}
			else{
				this->msgBox("Wrong password");
			}
			delete buffer;
		}
		return 1;
	}
	return 0;
}

/* Ui::processAddressDialogMessage: Fonction appelée lors d'un nouveau message sur la dialog d'addresse, appelée via uiAddressDialogCallback
 * HWND window: Dialog
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne 1 si le message a été traité, 0 sinon
 */
int16_t Ui::processAddressDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM){
	if(message == WM_COMMAND){
		if(LPARAM(wParam) == IDCANCEL){
			EndDialog(window, 0);
		}
		else if(LPARAM(wParam) == IDOK){
			char *buffer = new char[8192]; // Peur être rendue dynamique ?
			GetDlgItemTextA(window, IDADDRESS, buffer, 8192);
			EndDialog(window, 0);
			std::string address(buffer);
			delete buffer;

			this->m_uiToService->killClients(address);
		}
		return 1;
	}
	return 0;
}

/* Ui::processRunasDialogMessage: Fonction appelée lors d'un nouveau message sur la dialog de runas, appelée via uiRunasDialogCallback
 * HWND window: Dialog
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne 1 si le message a été traité, 0 sinon
 */
int16_t Ui::processRunasDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM){
	if(message == WM_COMMAND){
		if(LPARAM(wParam) == IDCANCEL){
			EndDialog(window, 0);
		}
		else if(LPARAM(wParam) == IDOK){
			char *passwordBuffer = new char[8192];
			char *cmdLineBuffer = new char[8192];
			uint16_t passwordLen = GetDlgItemTextA(window, IDPASSWORD_CMDLINE, passwordBuffer, 8192);
			GetDlgItemTextA(window, IDCMDLINE, cmdLineBuffer, 8192);
			EndDialog(window, 0);
			if(checkPassword(passwordBuffer, passwordLen)){
				this->m_uiToService->sendRunas(std::string(cmdLineBuffer));
			}
			else{
				this->msgBox("Wrong password");
			}
			delete passwordBuffer;
			delete cmdLineBuffer;
		}
		return 1;
	}
	return 0;
}

/* Ui::readImageFromClipboard: Lis une image depuis le presse-papier et l'envois au UiToService
 * Aucun paramètre ni retour
 */
void Ui::readImageFromClipboard(){
	if(!OpenClipboard(NULL)){
		this->msgBox("Unable to open clipboard.\nAn other process can be using it");
	}
	else{
		HDROP file = (HDROP) GetClipboardData(CF_HDROP);
		if(file == NULL){
			this->msgBox("Unable to found a file attached to this clipboard data\nTry to save your image in a file and copy the file from the explorer");
		}
		else{
			char *buffer = new char[8192];
			DragQueryFileA(file, 0, buffer, 8192);
			std::string filename(buffer);
			delete buffer;

			this->m_uiToService->sendImage(filename);
		}
		CloseClipboard();
	}
}