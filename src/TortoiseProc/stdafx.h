// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include "stdafx_common.h"

#include <afxwinappex.h>
#include <afxdlgs.h>
#include <afxmenubutton.h>
#include <afxdtctl.h>
#include <afxfontcombobox.h>
#include <afxcolorbutton.h>
#include <afxctl.h>
#include <afxtabctrl.h>
#include <afxtoolbarfontcombobox.h>
#include <afxvisualmanagerwindows.h >
#include <comdef.h>

#include <shlobj.h>
#include <WinInet.h>

#include <gdiplus.h>
#define USE_GDI_GRADIENT
#define HISTORYCOMBO_WITH_SYSIMAGELIST

#ifndef LVS_EX_DOUBLEBUFFER
#define LVS_EX_DOUBLEBUFFER     0x00010000
#endif

#ifdef _WIN64
#	define APP_X64_STRING	"x64"
#else
#	define APP_X64_STRING ""
#endif

#define XMESSAGEBOX_APPREGPATH "Software\\TortoiseGit\\"

#include <PropKey.h>

#undef min
#undef max
