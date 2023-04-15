#include "pch.h"
#include "framework.h"
#include "include/iTunesEvents.h"
#include "include/DiscordStatus.h"
#include "include/iTunesThread.h"
#include "include/iTunes/Interface_i.c"
#include <PathCch.h>
#include <time.h>


#pragma comment( lib, "Pathcch.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _DEBUG


const IID IID_IiTunesEvents = {
	0x429dd3c8, 0x703e, 0x4188, { 0x96, 0xe, 0xa9, 0x82, 0x1f, 0x14, 0xb0, 0x4c }
};

#define WINDOWS_TICK		10000000
#define EPOCH_DIFFERENCE	11644473600LL

#define CopyString(Source, Dest)		( CopyMemory(Dest, Source, (wcslen(Source) + 1) * sizeof(wchar_t)))
#define SAFE_STR_FREE(str)				if (str != NULL) SysFreeString(str); (str) = NULL;

// Discord RPC Assests Do not Allow Certain Characters -- The characters must be converted to underscores
#define AssetCharacterAllowed(c)		( (c == L'_' || c == L'-') || (c >= 48 && ((c >= 97 && c <= 122 ) || (c <= 57 ) || (c >= 65 && c <= 90))) )


// Get Event Update & Prepare Data for RPC Status 
static VOID PrepareEventUpdate( IITTrack *Track, BOOL Paused = FALSE )
{
	DISCORDSTATUS	Status = { 0 };
	HRESULT			hResult;
	LONG			lStartTime;
	LONG			lDuration;
	BSTR			strValue;
	
	/* App is disabled or paused -- Send empty status */
	if (WAIT_OBJECT_0 == WaitForSingleObject( g_hAppDisabledEvent, 0 ) || Paused) 
	{
		UpdateDiscordStatus( NULL );
		Track->Release();

		return;
	}


	//-------------------------
	// Song name
	if (SUCCEEDED( Track->get_Name( &strValue ) ))
	{
		Status.SongName = SysAllocString( strValue );
		SysFreeString( strValue );
	}

	//-------------------------
	// Artist
	if (SUCCEEDED( Track->get_Artist( &strValue ) ))
	{
		Status.Artist = SysAllocString( strValue );
		SysFreeString( strValue );
	}

	//-------------------------
	// Album name
	if (SUCCEEDED( Track->get_Album( &strValue ) ))
	{
		WCHAR szBuffer[1024]{};

		Status.AlbumName = SysAllocString( strValue );

		/* Translate album name */
		for (SIZE_T cOriginal = 0, cBuffer = 0; cOriginal < wcslen( strValue ) && cBuffer < ARRAYSIZE(szBuffer); ++cOriginal, ++cBuffer)
		{
			WCHAR Character = Status.AlbumName[cOriginal];

			/* Character is not allowed */
			if (FALSE == AssetCharacterAllowed( Character ))
			{
				/* Replace character with underscore */
				szBuffer[cBuffer] = L'_';
				if (Character == L':' && Status.AlbumName[cOriginal + 1] == L' ')
				{
					/* Add extra underscore if next character is space */
					szBuffer[++cBuffer] = L'_';
				}
				else if (Character == L' ' && Status.AlbumName[cOriginal + 1] == L' ')
				{
					continue;
				}

				/* Search until valid character is found */
				++cOriginal;
				while (FALSE == AssetCharacterAllowed( Status.AlbumName[cOriginal] ))
				{
					++cOriginal;
				}

				--cOriginal;
				continue;
			}

			/* Discord automatically sets all asset-name characters to lowercase */
			szBuffer[cBuffer] = towlower( Character );
		}

		SysFreeString( strValue );
		Status.AlbumPath = SysAllocString( szBuffer );
		
	}

	//-------------------------
	// Song duration
	if (SUCCEEDED( Track->get_Duration( &lDuration ) ))
	{
		FILETIME		FileTime;
		LARGE_INTEGER	lTime;
		SYSTEMTIME		sysTime;
		LONG			nPosition;

		GetSystemTimeAsFileTime( &FileTime );

		/* Get song position time */
		if (g_ItunesConnection && SUCCEEDED( g_ItunesConnection->get_PlayerPosition(&nPosition ) ))
		{
			/* Set status time as Unix Epoch */
			lTime.LowPart	= FileTime.dwLowDateTime;
			lTime.HighPart	= FileTime.dwHighDateTime;

			Status.Time = (lTime.QuadPart - EPOCH_DIFFERENCE) / WINDOWS_TICK;

			time_t tNow = time( 0 );
			tNow -= nPosition;

			Status.Time = tNow;
		}
	}

	Track->Release();

	/* Send discord status */
	UpdateDiscordStatus( &Status );

	/* Cleanup */
	SAFE_STR_FREE( Status.SongName );
	SAFE_STR_FREE( Status.Artist );
	SAFE_STR_FREE( Status.AlbumName );
	SAFE_STR_FREE( Status.AlbumPath );
}


// CiTunesEvents Constructor
CiTunesEvents::CiTunesEvents()
{
	EnableAutomation();
}


CiTunesEvents::~CiTunesEvents()
{

}

LPUNKNOWN
CiTunesEvents::GetInterfaceHook( LPCVOID ppIID )
{
	REFIID IId = *(IID *)ppIID;
	if (IId == DIID__IiTunesEvents)
	{
		return GetIDispatch( FALSE );
	}

	return NULL;
}


/* Method is called when the last referance for an
automation object is released. The base class automatically deletes the object */
VOID CiTunesEvents::OnFinalRelease()
{
	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CiTunesEvents, CCmdTarget)
END_MESSAGE_MAP()


/* Expose the CiTunesEvents Methods */
BEGIN_DISPATCH_MAP( CiTunesEvents, CCmdTarget )
	//{{AFX_DISPATCH_MAP(CiTunesEventHandler)
	DISP_FUNCTION( CiTunesEvents, "OnDatabaseChangedEvent",
		OnDatabaseChangedEvent, VT_EMPTY, VTS_VARIANT VTS_VARIANT )
	DISP_FUNCTION( CiTunesEvents, "OnPlayerPlayEvent",
		OnPlayerPlayEvent, VT_EMPTY, VTS_VARIANT )
	DISP_FUNCTION( CiTunesEvents, "OnPlayerStopEvent",
		OnPlayerStopEvent, VT_EMPTY, VTS_VARIANT )
	DISP_FUNCTION( CiTunesEvents, "OnPlayerPlayingTrackChangedEvent",
		OnPlayerPlayingTrackChangedEvent, VT_EMPTY, VTS_VARIANT )
	DISP_FUNCTION( CiTunesEvents, "OnUserInterfaceEnabledEvent",
		OnUserInterfaceEnabledEvent, VT_EMPTY, VTS_NONE )
	DISP_FUNCTION( CiTunesEvents, "OnCOMCallsDisabledEvent",
		OnCOMCallsDisabledEvent, VT_EMPTY, VTS_I2 )
	DISP_FUNCTION( CiTunesEvents, "OnCOMCallsEnabledEvent",
		OnCOMCallsEnabled, VT_EMPTY, VTS_NONE )
	DISP_FUNCTION( CiTunesEvents, "OnQuittingEvent", OnQuittingEvent,
		VT_EMPTY, VTS_NONE )
	DISP_FUNCTION( CiTunesEvents, "OnAboutToPromptUserToQuitEvent",
		OnAboutToPromptUserToQuitEvent, VT_EMPTY, VTS_NONE )
	DISP_FUNCTION( CiTunesEvents, "OnSoundVolumeChangedEvent",
		OnSoundVolumeChangedEvent, VT_EMPTY, VTS_I4 )
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


BEGIN_INTERFACE_MAP(CiTunesEvents, CCmdTarget)
	INTERFACE_PART(CiTunesEvents, IID_IiTunesEvents, Dispatch)
END_INTERFACE_MAP()



// CiTunesEvents Methods
//
VOID
CiTunesEvents::OnDatabaseChangedEvent(
	CONST VARIANT &DeletedObjectIDs,
	CONST VARIANT &ChangedObjectIDs
)
{
	IITTrack	*Track;
	SHORT		State;

	if (g_ItunesConnection)
	{
		g_ItunesConnection->get_PlayerState( (ITPlayerState *)&State );
		g_ItunesConnection->get_CurrentTrack( &Track );

		if (Track) {
			PrepareEventUpdate( Track, (State == ITPlayerStateStopped) );
		}
	}
}

VOID
CiTunesEvents::OnPlayerPlayEvent(
	CONST VARIANT &iTrack
)
{
	IITTrack	*Track;
	IDispatch	*Source;

	Source = iTrack.pdispVal;
	if (Source)
	{
		Source->QueryInterface( &Track );
		if (Track)
		{
			PrepareEventUpdate( Track );
		}

		Source->Release();
	}
}

VOID
CiTunesEvents::OnPlayerStopEvent(
	CONST VARIANT &iTrack
)
{
	IITTrack	*Track;
	IDispatch	*Source;

	Source = iTrack.pdispVal;
	if (Source)
	{
		Source->QueryInterface( &Track );
		if (Track)
		{
			PrepareEventUpdate( Track, TRUE );
		}

		Source->Release();
	}
}

VOID
CiTunesEvents::OnPlayerPlayingTrackChangedEvent(
	CONST VARIANT &iTrack
)
{
	IITTrack	*Track;
	IDispatch	*Source;

	Source = iTrack.pdispVal;
	if (Source)
	{
		Source->QueryInterface( &Track );
		if (Track)
		{
			PrepareEventUpdate( Track );
		}

		Source->Release();
	}
}

VOID
CiTunesEvents::OnUserInterfaceEnabledEvent()
{

}

VOID
CiTunesEvents::OnCOMCallsDisabledEvent( SHORT sReason )
{

}

VOID
CiTunesEvents::OnCOMCallsEnabled()
{

}

VOID
CiTunesEvents::OnQuittingEvent()
{
	
}

VOID
CiTunesEvents::OnAboutToPromptUserToQuitEvent()
{
	SetEvent( g_hITunesClosingEvent );
}

VOID
CiTunesEvents::OnSoundVolumeChangedEvent( LONG NewVolume )
{
	IITTrack	*Track;
	SHORT		State;

	if (g_ItunesConnection)
	{
		g_ItunesConnection->get_PlayerState( (ITPlayerState *)&State );
		g_ItunesConnection->get_CurrentTrack( &Track );

		if (Track)
		{
			PrepareEventUpdate( Track, (State == ITPlayerStateStopped) ? TRUE : FALSE );
		}
	}
}