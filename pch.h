// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <strsafe.h>

inline void TerminateApplication( CWnd *pWnd, LPCWSTR lpszMessage, DWORD dwError )
{
	WCHAR szErrorBuf[1024];

	StringCbPrintf(szErrorBuf, sizeof(szErrorBuf), L"%s %lu", lpszMessage, dwError );
	pWnd->MessageBox( szErrorBuf, L"Error Occurred", MB_OK );

	ExitProcess( 0 );
}

#endif //PCH_H
