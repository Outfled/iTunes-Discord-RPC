#pragma once

#include "pch.h"

#define TRAY_WNDCLASS_NAME	L"__hidden__"

extern NOTIFYICONDATA	g_IconData;
extern HWND				g_hTrayWnd;

DWORD WINAPI ApplicationTrayHandler( LPVOID );