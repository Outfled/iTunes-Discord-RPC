#include "pch.h"
#include "include/DiscordStatus.h"
#include "include/Discord/RPC.h"
#include "include/AlbumAssets.h"


HANDLE						g_hAppDisabledEvent;
static DiscordRichPresence	g_DiscordRichPresence;

static VOID UnicodeToMultiByte(LPCWSTR lpszUnicode, LPSTR *lppszMultibyte)
{
	DWORD cbNeeded = WideCharToMultiByte( CP_UTF8, 0, lpszUnicode, -1, NULL, 0, NULL, NULL );
	if (cbNeeded == 0)
	{
		*lppszMultibyte = new CHAR[2];
		(*lppszMultibyte)[0] = ' ';
		(*lppszMultibyte)[1] = ' ';
		return;
	}
	*lppszMultibyte = new CHAR[cbNeeded];
	WideCharToMultiByte(CP_UTF8, 0, lpszUnicode, -1, *lppszMultibyte, cbNeeded, NULL, NULL);
}

VOID UpdateDiscordStatus( LPDISCORDSTATUS lpStatus )
{
	static BOOL bInitialized = FALSE;

	LPSTR Song		= NULL;
	LPSTR Artist	= NULL;
	LPSTR Album		= NULL;
	LPSTR AssetName = NULL;

	ZeroMemory(&g_DiscordRichPresence, sizeof(g_DiscordRichPresence));

	/* NULL lpStatus means to clear the RPC status */
	if (lpStatus == NULL) {
		Discord_ClearPresence();
		return;
	}

	UnicodeToMultiByte( lpStatus->SongName, &Song );
	UnicodeToMultiByte( lpStatus->AlbumName, &Album );
	UnicodeToMultiByte( lpStatus->AlbumPath, &AssetName );
	UnicodeToMultiByte( lpStatus->Artist, &Artist );

	if (Song && Album && AssetName)
	{
		CHAR szState[100]{};

		/* Add album & artist into single string */
		StringCbPrintfA(szState, sizeof(szState), "by %s on '%s'", Artist, Album );

		/* Set the Discord RPC status info */
		g_DiscordRichPresence.startTimestamp	= lpStatus->Time;
		g_DiscordRichPresence.details			= Song;
		g_DiscordRichPresence.state				= szState;
		g_DiscordRichPresence.largeImageKey		= AssetName;
		g_DiscordRichPresence.largeImageText	= Album;

		// Comment this line out to hide the tiny iTunes logo at the bottom right of the album cover picture on the RPC status
		g_DiscordRichPresence.smallImageKey = SMALL_IMGKEY;

		/* Initialize Discord RPC for the application */
		if ( bInitialized == FALSE )
		{
			Discord_Initialize( RPC_ASSET_APPLICATION_ID, NULL, 1, NULL );
			bInitialized = TRUE;
		}

		/* Send the status */
		Discord_UpdatePresence(&g_DiscordRichPresence);
		Discord_RunCallbacks();
	}

	/* Cleanup */
	SAFE_STR_DELETE( Artist );
	SAFE_STR_DELETE( AssetName );
	SAFE_STR_DELETE( Album );
	SAFE_STR_DELETE( Song );
}