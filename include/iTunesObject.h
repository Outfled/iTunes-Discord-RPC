#pragma once

#include "iTunesEvents.h"

class CiTunesObject
{
public:
	CiTunesObject()
	{
		hResult				= S_OK;
		pInterface			= NULL;
		pPointContainer		= NULL;
		pConnectionPoint	= NULL;
		dwCookie			= 0;
	}

	~CiTunesObject()
	{
		if (pConnectionPoint)
		{
			pConnectionPoint->Unadvise( dwCookie );
			SafeRelease( &pConnectionPoint );
		}
		if (pPointContainer)
		{
			SafeRelease( &pPointContainer );
		}
	}

	IiTunes *GetConnection()
	{
		if (pInterface == nullptr)
		{
			hResult = S_OK;

			/* Get IiTunes Object */
			hResult = ::CoCreateInstance( CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS( &pInterface ));
			if (SUCCEEDED( hResult ))
			{
				/* Create connection point */
				hResult = pInterface->QueryInterface( &pPointContainer );
				if (SUCCEEDED( hResult ))
				{
					hResult = pPointContainer->FindConnectionPoint( DIID__IiTunesEvents, &pConnectionPoint );
					if (SUCCEEDED( hResult ))
					{
						hResult = pConnectionPoint->Advise( Events.GetIDispatch( FALSE ), &dwCookie );
					}
				}
			}

			if (hResult != S_OK)
			{
				SafeRelease( &pConnectionPoint );
				SafeRelease( &pPointContainer );
				SafeRelease( &pInterface );
			}
		}

		return pInterface;
	}

public:
	HRESULT hResult;

private:
	IiTunes						*pInterface;
	IConnectionPointContainer	*pPointContainer;
	IConnectionPoint			*pConnectionPoint;
	CiTunesEvents				Events;
	DWORD						dwCookie;
};