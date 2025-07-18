#ifndef LOGIN_SERVER_H_
#define LOGIN_SERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "../RanLogicServer/Server/LoginServer.h"

class SUBPATH;

// value
extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern HWND g_hEditBox;
extern HWND g_hWindow;
extern bool g_bConnect;

extern CLoginServer* g_pServer;
extern SUBPATH* g_pPath;

// function
BOOL CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);

void setFirstFocus(HWND hDlg);
void cmdStartOrStop(HWND hDlg);
void executeCommand(HWND hDlg);
void cmdExit(HWND hDlg);
void initDialog(HWND hDlg);
void setFirstFocus(HWND hDlg);
void cmdShowWindow(HWND hDlg);
void cmdHideWindow(HWND hDlg);
void cmdPopupMenu(HWND hDlg);

#define WM_TRAY_NOTIFICATION			WM_APP+3

#endif // LOGIN_SERVER_H_
