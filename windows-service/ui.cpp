#include <string.h>
#include <iostream>
#include <map>
#include <windows.h>

#include "ui.h"

// ineplemUiPtr: Pointeur vers l'Ui utilisé par uiWindowCallback pour appeler processWindowMessage
Ui* ineplemUiPtr;

/* uiWindowCallback: Fonction appelé lors d'un nouveau message sur la fenètre, transfert vers Ui::processWindowMessage
 * HWND window: Fenetre
 * UINT message: Identifiant du message
 * WPARAM wParam et LPARAM lParam: Paramètres du message
 * Retourne une valeur spécifique selon le message
 */
LRESULT CALLBACK uiWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
	return ineplemUiPtr->processWindowMessage(window, message, wParam, lParam);
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

/* Ui::processWindowMessage: Fonction appelé lors d'un nouveau message sur la fenètre, appelé via uiWindowCallback
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

	// Les fonction SetForegroundWindow et PostMessage sont appelé car un bug de Windows peut faire glitcher le menu sinon
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
	AppendMenuA(this->m_menu, MF_STRING, UI_MENU_FBSTREAM, "FrameBuffer Stream");
	AppendMenuA(this->m_menu, MF_SEPARATOR, 0, NULL);
	AppendMenuA(this->m_menu, MF_POPUP, (UINT) this->m_authResponseMenu, "Auth Response");
	AppendMenuA(this->m_menu, MF_POPUP, (UINT) this->m_authMethodMenu, "Auth Method");
	AppendMenuA(this->m_menu, MF_SEPARATOR, 0, NULL);
	AppendMenuA(this->m_menu, MF_STRING, UI_MENU_USERNAME, "Username");
	AppendMenuA(this->m_menu, MF_STRING, UI_MENU_STATUS, "Status");

	// On met a jour les checkbox
	this->m_checkboxesStatus[UI_MENU_AUTHMETHOD_MSII] = 1;
	this->m_checkboxesStatus[UI_MENU_AUTHRESPONSE_REJECT] = 1;
	this->updateMenuCheckboxes();
}

/* Ui::statusMsgbox: Affiche la boite de dialogue a propos du status
 * Aucun paramètre ni retour
 */
void Ui::statusMsgbox(){
	std::string outputText("");
	outputText += "Status:";
	outputText += "\n";
	outputText += "(c) 2019 eef784f1ff9aae654805f0f674bbabec7ae5f6a9\n";
	MessageBox(this->m_window, outputText.c_str(), "INEPLEM Status", MB_OK);
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

/* Ui:processItem: Traite un item sur lequel on a cliqué
 * uint16_t menuId: Identifiant de l'item
 */
void Ui::processItem(uint16_t menuId){
	switch(menuId){
		case UI_MENU_STATUS:
			this->statusMsgbox();
			break;
	}
}