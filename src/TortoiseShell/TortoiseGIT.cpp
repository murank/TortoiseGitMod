// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2003-2008 - TortoiseSVN
// Copyright (C) 2008-2011 - TortoiseGit

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "stdafx.h"
#include "ShellExt.h"
#include "Guids.h"
#include "ShellExtClassFactory.h"
//#include "git_dso.h"

#include "InterprocessClient.h"
#include "IPPipeFunctions.h"

UINT				g_cRefThisDll = 0;				///< reference count of this DLL.
HINSTANCE			g_hmodThisDll = NULL;			///< handle to this DLL itself.
int					g_cAprInit = 0;
ShellCache			g_ShellCache;					///< caching of registry entries, ...
DWORD				g_langid;
DWORD				g_langTimeout = 0;
HINSTANCE			g_hResInst = NULL;
stdstring			g_filepath;
git_wc_status_kind	g_filestatus = git_wc_status_none;	///< holds the corresponding status to the file/dir above
bool				g_readonlyoverlay = false;
bool				g_lockedoverlay = false;

bool				g_normalovlloaded = false;
bool				g_modifiedovlloaded = false;
bool				g_conflictedovlloaded = false;
bool				g_readonlyovlloaded = false;
bool				g_deletedovlloaded = false;
bool				g_lockedovlloaded = false;
bool				g_addedovlloaded = false;
bool				g_ignoredovlloaded = false;
bool				g_unversionedovlloaded = false;
CComCriticalSection	g_csGlobalCOMGuard;

LPCTSTR				g_MenuIDString = _T("TortoiseGit");
extern std::set<CShellExt *> g_exts;

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

static void InitializeInterprocessClient()
{
	CString pipeName = GetInterprocessPipeName();
	shared_ptr<InterprocessClient> client(new InterprocessClient(pipeName));

	SetGlobalInterprocessClient(client);
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpReserved */)
{
#ifdef _DEBUG
	// if no debugger is present, then don't load the dll.
	// this prevents other apps from loading the dll and locking
	// it.

	bool bInShellTest = false;
	TCHAR buf[_MAX_PATH + 1];		// MAX_PATH ok, the test really is for debugging anyway.
	DWORD pathLength = GetModuleFileName(NULL, buf, _MAX_PATH);
	if(pathLength >= 14)
	{
		if ((_tcsicmp(&buf[pathLength-14], _T("\\ShellTest.exe"))) == 0)
		{
			bInShellTest = true;
		}
		if ((_tcsicmp(&buf[pathLength-13], _T("\\verclsid.exe"))) == 0)
		{
			bInShellTest = true;
		}
	}

	if (!::IsDebuggerPresent() && !bInShellTest)
	{
		ATLTRACE("In debug load preventer\n");
		return FALSE;
	}
#endif

	// NOTE: Do *NOT* call apr_initialize() or apr_terminate() here in DllMain(),
	// because those functions call LoadLibrary() indirectly through malloc().
	// And LoadLibrary() inside DllMain() is not allowed and can lead to unexpected
	// behavior and even may create dependency loops in the dll load order.
    if (dwReason == DLL_PROCESS_ATTACH)
    {
		if (g_hmodThisDll == NULL)
		{
			g_csGlobalCOMGuard.Init();
		}

        // Extension DLL one-time initialization
        g_hmodThisDll = hInstance;

		InitializeInterprocessClient();
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
		// sometimes an application doesn't release all COM objects
		// but still unloads the dll.
		// in that case, we do it ourselves
		if (g_cRefThisDll > 0)
		{
			std::set<CShellExt *>::iterator it = g_exts.begin();
			while (it != g_exts.end())
			{
				delete *it;
				it = g_exts.begin();
			}
			while (g_cAprInit--)
			{
				g_GitAdminDir.Close();
//				apr_terminate();
			}
		}
		g_csGlobalCOMGuard.Term();
    }
    return 1;   // ok
}

STDAPI DllCanUnloadNow(void)
{
	return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = NULL;

    FileState state = FileStateInvalid;
    if (IsEqualIID(rclsid, CLSID_Tortoisegit_UPTODATE))
        state = FileStateVersioned;
    else if (IsEqualIID(rclsid, CLSID_Tortoisegit_MODIFIED))
        state = FileStateModified;
    else if (IsEqualIID(rclsid, CLSID_Tortoisegit_CONFLICTING))
        state = FileStateConflict;
    else if (IsEqualIID(rclsid, CLSID_Tortoisegit_UNCONTROLLED))
        state = FileStateUncontrolled;
	else if (IsEqualIID(rclsid, CLSID_Tortoisegit_DROPHANDLER))
		state = FileStateDropHandler;
	else if (IsEqualIID(rclsid, CLSID_Tortoisegit_DELETED))
		state = FileStateDeleted;
	else if (IsEqualIID(rclsid, CLSID_Tortoisegit_READONLY))
		state = FileStateReadOnly;
	else if (IsEqualIID(rclsid, CLSID_Tortoisegit_LOCKED))
		state = FileStateLockedOverlay;
	else if (IsEqualIID(rclsid, CLSID_Tortoisegit_ADDED))
		state = FileStateAddedOverlay;
	else if (IsEqualIID(rclsid, CLSID_Tortoisegit_IGNORED))
		state = FileStateIgnoredOverlay;
	else if (IsEqualIID(rclsid, CLSID_Tortoisegit_UNVERSIONED))
		state = FileStateUnversionedOverlay;

    if (state != FileStateInvalid)
    {
//		apr_initialize();
//		git_dso_initialize2();
		g_GitAdminDir.Init();
		g_cAprInit++;

		CShellExtClassFactory *pcf = new CShellExtClassFactory(state);
		return pcf->QueryInterface(riid, ppvOut);
    }

    return CLASS_E_CLASSNOTAVAILABLE;

}


