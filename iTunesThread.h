#pragma once

#include "pch.h"

extern HANDLE	g_hITunesClosingEvent;

DWORD WINAPI iTunesHandlerThread( LPVOID );