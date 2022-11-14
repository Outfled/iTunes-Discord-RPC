// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define UNICODE

// add headers that you want to pre-compile here
#include "framework.h"
#include <strsafe.h>

template<class T>
VOID SafeRelease( T **ppObj )
{
	if (ppObj && *ppObj)
	{
		(*ppObj)->Release();
		*ppObj = nullptr;
	}
}

inline VOID TerminateApplication( CWnd *pWnd, LPCWSTR lpszMessage, DWORD dwError )
{
	WCHAR ErrorBuf[1024];

	StringCbPrintf( ErrorBuf, sizeof( ErrorBuf ), L"%s %lu", lpszMessage, dwError );
	pWnd->MessageBox( ErrorBuf, L"Error Occurred", MB_OK );

	ExitProcess( 0 );
}

#endif //PCH_H
