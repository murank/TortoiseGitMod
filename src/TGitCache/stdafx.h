// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "stdafx_common.h"

#include <shellapi.h>
#include <shlobj.h>

typedef CComCritSecLock<CComAutoCriticalSection> AutoLocker;

// Temporary fix for people not using the latest SDK
#ifndef PROCESS_MODE_BACKGROUND_BEGIN
#define PROCESS_MODE_BACKGROUND_BEGIN 0x00100000
#endif
#ifndef THREAD_MODE_BACKGROUND_BEGIN
#define THREAD_MODE_BACKGROUND_BEGIN 0x00010000
#endif
#ifndef THREAD_MODE_BACKGROUND_END
#define THREAD_MODE_BACKGROUND_END 0x00020000
#endif

#ifdef _WIN64
#	define APP_X64_STRING	"x64"
#else
#	define APP_X64_STRING ""
#endif
