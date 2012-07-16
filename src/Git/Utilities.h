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

#ifndef UTILITIES_H
#define UTILITIES_H

bool StartsWith(const CString& str, const CString& pattern);
bool EndsWith(const CString& str, const CString& pattern);

CString GetRepositoryRoot(CString path);
bool IsInGitRepository(const CString& path);

std::string GetLastErrorString();

CString Id2Str(int nID);

CString GetDirectory(const CString& path);

CString GetCurrentDir();
CString GetAppPath();
CString GetAppDirectory();

bool LaunchCommand(CString command, bool bWait = false);
bool LaunchProcCommand(const CString& cmdLine);

#endif