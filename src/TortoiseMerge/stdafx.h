// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include "stdafx_common.h"

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxwinappex.h>
#include <afxctl.h>
#include <afxpanedialog.h>
#include <afxframewndex.h>
#include <afxstatusbar.h>
#include <afxvisualmanagerwindows.h>
#include <afxcolorbutton.h>
#include <afxfontcombobox.h>
#include <afxvisualmanageroffice2003.h>
#include <afxvisualmanagervs2005.h>
#include <afxvisualmanageroffice2007.h>
#include <afxtoolbarfontcombobox.h>

#define XMESSAGEBOX_APPREGPATH "Software\\TortoiseMerge\\"

#ifdef _WIN64
#	define APP_X64_STRING	"x64"
#else
#	define APP_X64_STRING ""
#endif

#ifndef LVS_EX_DOUBLEBUFFER
#define LVS_EX_DOUBLEBUFFER     0x00010000
#endif

#define USE_GDI_GRADIENT
