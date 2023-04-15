#pragma once

#pragma comment ( lib, "discord-rpc.lib" )

#define SMALL_IMGKEY "small-icon"
#define SAFE_STR_DELETE(str)				if (str != NULL) delete[] str; (str) = NULL;

typedef struct _DISCORDSTATUS
{
	LPWSTR	SongName;
	LPWSTR	Artist;
	LPWSTR	AlbumPath;
	LPWSTR	AlbumName;
	SIZE_T	Time;
}DISCORDSTATUS, *LPDISCORDSTATUS;

extern HANDLE g_hAppDisabledEvent;

VOID UpdateDiscordStatus( LPDISCORDSTATUS lpStatus );