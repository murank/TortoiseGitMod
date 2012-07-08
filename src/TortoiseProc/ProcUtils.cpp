// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2012 - TortoiseGit

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

#include "ProcUtils.h"

#include "Resource.h"

static CString GetAppName()
{
	return MAKEINTRESOURCE(IDS_APPNAME);
}

namespace internal {

CString GetTweakedWindowTitle(const CString& oldWindowTitle, const CString& dir, const CString& appName)
{
	const int MAX_WINDOW_TITLE_LEN = 70;
	assert(oldWindowTitle.GetLength() < MAX_WINDOW_TITLE_LEN);
	assert(dir.GetLength() < MAX_PATH);

	TCHAR pathbuf[MAX_PATH] = {0};
	PathCompactPathEx(pathbuf, dir, MAX_WINDOW_TITLE_LEN - oldWindowTitle.GetLength(), 0);

	CString newWindowTitle;
	newWindowTitle.Format(_T("%s - %s - %s"), pathbuf, oldWindowTitle, appName);
	return newWindowTitle;
}

} // namesapce internal

void TweakWindowTitle(CWnd& wnd, const CString& path)
{
	CString windowTitle;
	wnd.GetWindowText(windowTitle);

	TweakWindowTitle(wnd, windowTitle, path);
}

void TweakWindowTitle(CWnd& wnd, const CString& title, const CString& path)
{
	CString newWindowTitle = internal::GetTweakedWindowTitle(title, path, GetAppName());

	wnd.SetWindowText(newWindowTitle);
}
