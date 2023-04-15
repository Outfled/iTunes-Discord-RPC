#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "include/AppTray.h"
#include "include/AppReg.h"
#include "include/iTunesObject.h"
#include "include/DiscordStatus.h"
#include <shellapi.h>

#pragma comment ( lib, "Shell32.lib" )


#define TRAY_WND_NAME		L"iTunes Discord RPC"
#define TRAY_HOVER_MSG		TRAY_WND_NAME
#define TRAY_ICON_CALLBACK	WM_APP + 1
#define MENU_ID_OPENAPP		101
#define MENU_ID_EXITAPP		102


//--------------------
// Global Variables
HWND					g_hTrayWnd;
NOTIFYICONDATA			g_IconData;
static WNDCLASSEX		g_WndClass;
static HMODULE			g_Instance;
static CWnd				*g_AppWnd;

static LRESULT TrayIconProcedure( HWND hWnd, UINT uMsg, WPARAM wParm, LPARAM lParam )
{
	if (uMsg == WM_CREATE)
	{
		ZeroMemory( &g_IconData, sizeof( g_IconData ) );

		/* Initialize tray icon */
		g_IconData.cbSize = sizeof( NOTIFYICONDATA );
		g_IconData.uID = 0;
		g_IconData.hWnd = hWnd;
		g_IconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP | NIF_INFO;
		g_IconData.uVersion = NOTIFYICON_VERSION_4;
		g_IconData.uCallbackMessage = TRAY_ICON_CALLBACK;
		g_IconData.dwInfoFlags = NIIF_INFO;
		g_IconData.hIcon = g_AppWnd->GetIcon( TRUE );
		StringCbCopy( g_IconData.szTip,
			ARRAYSIZE( g_IconData.szTip ),
			TRAY_HOVER_MSG
		);

		/* Delete Old Tray-Icon, if Exists */
		Shell_NotifyIcon( NIM_DELETE, &g_IconData );

		/* Create Tray-Icon */
		Shell_NotifyIcon( NIM_ADD, &g_IconData );
		Shell_NotifyIcon( NIM_SETVERSION, &g_IconData );

		return 1;
	}

	if (uMsg == TRAY_ICON_CALLBACK)
	{
		DWORD dwLow		= LOWORD( lParam );
		DWORD dwHigh	= HIWORD( lParam );

		if (dwLow == WM_CONTEXTMENU)
		{
			POINT			Cursor;
			HMENU			PopupMenu;
			MENUITEMINFO	ItemInfo;
			UINT			MenuResult;

			PopupMenu = CreatePopupMenu();
			ItemInfo.cbSize = sizeof( ItemInfo );
			ItemInfo.fMask = MIIM_FTYPE;
			ItemInfo.fType = MFT_SEPARATOR;

			/* Create menu options */
			InsertMenu( PopupMenu,
				0,
				g_AppWnd->IsWindowVisible() ? MF_GRAYED : 0 | MF_STRING | MF_BYCOMMAND,
				MENU_ID_OPENAPP,
				L"Open Window" );
			InsertMenu( PopupMenu,
				2,
				MF_BYPOSITION,
				MENU_ID_EXITAPP,
				L"Exit" );
			InsertMenuItem( PopupMenu, 1, TRUE, &ItemInfo );

			SetForegroundWindow( hWnd );
			GetCursorPos( &Cursor );

			MenuResult = TrackPopupMenu( PopupMenu,
				0x0004L | 0x000L | 0x0002L | 0x2000L | 0x0100L,
				Cursor.x,
				Cursor.y,
				0,
				hWnd,
				NULL 
			);
			switch (MenuResult)
			{
			case MENU_ID_OPENAPP:
				g_AppWnd->ShowWindow( SW_SHOW );
				break;

			case MENU_ID_EXITAPP:
				Shell_NotifyIcon( NIM_DELETE, &g_IconData );
				PostQuitMessage( 0 );

				return 0;
			default:
				break;
			}
		}

		return 0;
	}

	if (uMsg == WM_DESTROY)
	{
		Shell_NotifyIcon( NIM_DELETE, &g_IconData );
		PostQuitMessage( 0 );

		return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParm, lParam );
}

static BOOL CreateTrayWndClass()
{
	ZeroMemory( &g_WndClass, sizeof( g_WndClass ) );

	g_WndClass.cbSize			= sizeof( WNDCLASSEX );
	g_WndClass.style			= CS_HREDRAW | CS_VREDRAW;
	g_WndClass.lpfnWndProc		= TrayIconProcedure;
	g_WndClass.hInstance		= g_Instance;
	g_WndClass.lpszClassName	= TRAY_WNDCLASS_NAME;

	return RegisterClassEx( &g_WndClass );
}


//---------------------------------------
// Tray Icon Handler Thread
DWORD WINAPI ApplicationTrayHandler( LPVOID lpvParam )
{
	BOOL	bResult;
	MSG		MsgWnd;
	BOOL	bPreviousValue;

	g_AppWnd	= (CWnd *)lpvParam;
	g_Instance	= GetModuleHandle( NULL );

	/* Wait for dlg window to be hidden */
	while (g_AppWnd->IsWindowVisible() == TRUE)
	{
		Sleep( 250 );
	}

	while (1)
	{
		g_hTrayWnd = NULL;

		/* Register hidden window class */
		bResult = CreateTrayWndClass();
		if (bResult == FALSE)
		{
			SafeRelease( &g_ItunesConnection );
			TerminateApplication( g_AppWnd, L"An unknown error occurred", GetLastError() );
		}

		/* Create tray icon */
		g_hTrayWnd = CreateWindow(
			TRAY_WNDCLASS_NAME,
			TRAY_WND_NAME,
			WS_OVERLAPPED,
			CW_USEDEFAULT,
			0,
			CW_USEDEFAULT,
			0,
			NULL,
			NULL,
			g_Instance,
			NULL
		);
		if (g_hTrayWnd == NULL)
		{
			UnregisterClass( TRAY_WNDCLASS_NAME, g_Instance );
			TerminateApplication( g_AppWnd, L"An unknown error occurred", GetLastError() );

			return 0;
		}

		/* Get tray messages */
		while (GetMessage( &MsgWnd, NULL, 0, 0 ))
		{
			TranslateMessage( &MsgWnd );
			DispatchMessageW( &MsgWnd );
		}

		/* Cleanup */
		Shell_NotifyIcon( NIM_DELETE, &g_IconData );
		DestroyWindow( g_hTrayWnd );
		UnregisterClassW( TRAY_WNDCLASS_NAME, g_Instance );

		break;
	}

	/* Minimize to tray is disabled -- exit the app */
	UpdateDiscordStatus( NULL ); // Clear Discord status

	/* Delete tray icon */
	Shell_NotifyIcon( NIM_DELETE, &g_IconData );
	::DestroyWindow( g_hTrayWnd );
	UnregisterClassW( TRAY_WNDCLASS_NAME, GetModuleHandle( NULL ) );

	/* Cleanup iTunes connection */
	SafeRelease( &g_ItunesConnection );
	Sleep( 1000 );
	ExitProcess( 0 );

	return 1;
}
