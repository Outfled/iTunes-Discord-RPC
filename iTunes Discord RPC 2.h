
// iTunes Discord RPC 2.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CiTunesDiscordRPC2App:
// See iTunes Discord RPC 2.cpp for the implementation of this class
//

class CiTunesDiscordRPC2App : public CWinApp
{
public:
	CiTunesDiscordRPC2App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CiTunesDiscordRPC2App theApp;
