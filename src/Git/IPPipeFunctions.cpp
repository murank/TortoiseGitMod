// TortoiseGit - a Windows shell extension for easy version control

// External Cache Copyright (C) 2007 - TortoiseSVN
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

#include "IPPipeFunctions.h"

#include "SharedHandle.h"

static CString GetCacheID()
{
	HANDLE token;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
		return CString();
	}

	shared_handle hToken(token);

	DWORD len = 0;
	GetTokenInformation(hToken.get(), TokenStatistics, NULL, 0, &len);

	std::vector<BYTE> data(len);
	GetTokenInformation(hToken.get(), TokenStatistics, &(data[0]), data.size(), &len);

	LUID uid = reinterpret_cast<TOKEN_STATISTICS*>(&(data[0]))->AuthenticationId;

	CString ret;
	ret.Format(_T("-%08x%08x"), uid.HighPart, uid.LowPart);
	return ret;
}

CString GetInterprocessPipeName()
{
	const CString INTERPRCESS_PIPE_NAME(_T("\\\\.\\pipe\\TGitCacheEx"));

	return INTERPRCESS_PIPE_NAME + GetCacheID();
}
