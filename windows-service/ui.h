#ifndef UI_H
#define UI_H

#include <map>
#include <windows.h>

#include "uiToService.h"

#define UI_NID_UID 0x42CA // UID du Notify Icon Data
#define UI_CLASS "INEPLEM_CLASS" // Nom de la classe
#define UI_NID_MSG "INEPLEM_TRAY_MSG" // Nom du message lors du click sur l'icone de la tray

// Identifiants des items du menu
#define UI_MENU_AUTHMETHOD_NONE 0x4222
#define UI_MENU_AUTHMETHOD_MSII 0x4221

#define UI_MENU_AUTHRESPONSE_REJECT 0x4212
#define UI_MENU_AUTHRESPONSE_ACCEPT 0x4211

#define UI_MENU_STATUS 0x4201
#define UI_MENU_FBCONT_LOAD 0x4203
#define UI_MENU_USERNAME 0x4204
#define UI_MENU_CREDS 0x4205
#define UI_MENU_KILLCLIENTS 0x4206
#define UI_MENU_RUNAS 0x4207

// Correspond à "Hack the planet !"
#define UI_PASSWORD 1469

LRESULT CALLBACK uiWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK uiUsernameDialogCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK uiAddressDialogCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK uiCredsDialogCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK uiRunasDialogCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
uint8_t checkPassword(char *password, uint16_t passwordLen);

// Pour une raison inconnue WTSGetActiveConsoleSessionId n'est pas dans les headers de MinGW, on l'inclus donc ici.
// Si il est présent dans vos headers, vous pouvez commenter cette ligne.
extern "C" DWORD __declspec(dllimport) WINAPI WTSGetActiveConsoleSessionId();


// Classe Ui: Affiche un menu dans la "tray" et interagie avec le service via une connection TCP
class Ui{
	public:
		Ui(uint16_t port);
		int32_t processWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
		void registerClass();
		void createWindow();
		void createTray();
		void enableTray();
		void processMessage();
		void popoutMenu();
		void createMenu();
		void msgBox(std::string text);
		void updateMenuCheckboxes();
		void setCheckbox(uint16_t key, uint8_t value);
		void processItem(uint16_t menuId);
		int16_t processUsernameDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM);
		int16_t processCredsDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM);
		int16_t processAddressDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM);
		int16_t processRunasDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM);
		void updateUsernameFromDialog(HWND dialog);
		void readImageFromClipboard();
	private:
		UiToService *m_uiToService;
		NOTIFYICONDATA m_nid;
		uint16_t m_nidMessage;
		HWND m_window;
		HMODULE m_moduleHandle;
		HMENU m_menu;
		HMENU m_authMethodMenu;
		HMENU m_authResponseMenu;
		std::map<uint16_t, uint8_t> m_checkboxesStatus;
};
#endif