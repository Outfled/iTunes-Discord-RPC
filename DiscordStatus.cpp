#include "pch.h"
#include "DiscordStatus.h"
#include "Discord/RPC.h"
#include <strsafe.h>

#pragma comment ( lib, "discord-rpc.lib" )

#define SAFE_STR_DELETE(str)				if (str != NULL) delete[] str; (str) = NULL;

HANDLE g_hAppDisabledEvent;
static DiscordRichPresence g_DiscordRichPresence;


static VOID UnicodeToMultiByte( LPCWSTR lpszUnicode, LPSTR *lppszMultibyte )
{
	if (lpszUnicode == NULL || wcslen( lpszUnicode ) == 0) {
		*lppszMultibyte = nullptr;
		return;
	}
	
	size_t cch = wcslen( lpszUnicode );
	*lppszMultibyte = new char[cch + 1];

	wcstombs_s( &cch, *lppszMultibyte, cch + 1, lpszUnicode, (cch + 1) * sizeof( wchar_t ) );
}

VOID UpdateDiscordStatus( LPDISCORDSTATUS lpStatus )
{
	CHAR *Song				= NULL;
	CHAR *Artist			= NULL;
	CHAR *Album				= NULL;
	CHAR *AssetName			= NULL;
	static BOOL Initialized = FALSE;

	ZeroMemory( &g_DiscordRichPresence, sizeof( g_DiscordRichPresence ) );

	/* NULL lpStatus means to clear the RPC status */
	if (lpStatus == NULL)
	{
		Discord_ClearPresence();
		return;
	}

	/* App disabled -- don't send any updates to discord */
	if (WAIT_OBJECT_0 == WaitForSingleObject( g_hAppDisabledEvent, 0 ))
	{
		Discord_ClearPresence();
		return;
	}

	UnicodeToMultiByte( lpStatus->SongName, &Song );
	UnicodeToMultiByte( lpStatus->AlbumName, &Album );
	UnicodeToMultiByte( lpStatus->AlbumPath, &AssetName );
	UnicodeToMultiByte( lpStatus->Artist, &Artist );

	if (Song && Album && AssetName)
	{
		CHAR szState[1000]{};

		/* Add album & artist into single string */
		StringCbPrintfA( szState, sizeof( szState ), "by %s on '%s'", Artist, Album );

		/* Set DiscordRPC info */
		g_DiscordRichPresence.startTimestamp	= lpStatus->Time;
		g_DiscordRichPresence.details			= Song;
		g_DiscordRichPresence.state				= szState;
		g_DiscordRichPresence.smallImageKey		= SMALL_IMGKEY;
		g_DiscordRichPresence.largeImageKey		= AssetName;
		g_DiscordRichPresence.largeImageText	= Album;

		/* Initialize DiscordRPC */
		if (Initialized == FALSE) {
			Discord_Initialize( RPC_APPLICATION_ID, NULL, 1, NULL );
			Initialized = TRUE;
		}

		/* Send status */
		Discord_UpdatePresence( &g_DiscordRichPresence );
		Discord_RunCallbacks();
	}

	/* Cleanup */
	SAFE_STR_DELETE( Artist );
	SAFE_STR_DELETE( AssetName );
	SAFE_STR_DELETE( Album );
	SAFE_STR_DELETE( Song );
}