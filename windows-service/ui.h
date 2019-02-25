#ifndef UI_H
#define UI_H

#include <map>
#include <windows.h>

#define UI_NID_UID 0x42CA // UID du Notify Icon Data
#define UI_CLASS "INEPLEM_CLASS" // Nom de la classe
#define UI_NID_MSG "INEPLEM_TRAY_MSG" // Nom du message lors du click sur l'icone de la tray

// Identifiants des items du menu
#define UI_MENU_AUTHMETHOD_NONE 0x4222
#define UI_MENU_AUTHMETHOD_MSII 0x4221

#define UI_MENU_AUTHRESPONSE_REJECT 0x4212
#define UI_MENU_AUTHRESPONSE_ACCEPT 0x4211

#define UI_MENU_STATUS 0x4201
#define UI_MENU_FBSTREAM 0x4202
#define UI_MENU_FBCONT_LOAD 0x4203

LRESULT CALLBACK uiWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

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
		void statusMsgbox();
		void updateMenuCheckboxes();
		void processItem(uint16_t menuId);
	private:
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