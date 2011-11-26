// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2010 - TortoiseGit

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

#include "stdafx.h"

#include "Utilities.h"

bool StartsWith(const CString& str, const CString& pattern)
{
	int pattern_len = pattern.GetLength();

	if(str.GetLength() < pattern_len) {
		return false;
	}

	for(int i=pattern_len-1; i>=0; --i) {
		if(str[i] != pattern[i]) {
			return false;
		}
	}

	return true;
}

bool EndsWith(const CString& str, const CString& pattern)
{
	int len = str.GetLength();
	int pattern_len = pattern.GetLength();

	if(len < pattern_len) {
		return false;
	}

	for(int i=1; i<=pattern_len; ++i) {
		if(str[len-i] != pattern[pattern_len-i]) {
			return false;
		}
	}

	return true;
}

bool HasGitDir(const CString& path)
{
	return (PathIsDirectory(path+_T("\\.git"))!=FALSE);
}

static CString GetParentDir(const CString& path)
{
	int pos = path.ReverseFind(_T('\\'));
	if(pos < 0) {
		return CString();
	}

	return path.Left(pos);
}

CString GetRepositoryRoot(CString path)
{
	while(!HasGitDir(path)) {
		path = GetParentDir(path);
		if(path.IsEmpty()) {
			return CString();
		}
	}

	return path;
}

