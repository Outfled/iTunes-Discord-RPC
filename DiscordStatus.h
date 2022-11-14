#pragma once

#include "pch.h"

#define RPC_APPLICATION_ID	"940451201659592734"
#define SMALL_IMGKEY		"small-icon"

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