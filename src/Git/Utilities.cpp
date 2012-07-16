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

bool IsInGitRepository(const CString& path)
{
	CString root = GetRepositoryRoot(path);
	return !root.IsEmpty();
}

std::string GetLastErrorString()
{
	LPVOID buf;

	DWORD lastError = GetLastError();

	if(FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		lastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&buf,
		0,
		NULL
	) == 0) {
		// fail to get message string

		std::ostringstream os;
		os << "Error code: " << lastError << std::ends;

		return os.str();
	}

	CStringA str((LPCTSTR)buf);
	std::string ret((LPCSTR)str);

	LocalFree(buf);

	return ret;
}

CString Id2Str(int nID)
{
	CString ret;
	ret.LoadString(nID);
	return ret;
}

static int GetSlashPosition(const CString& path)
{
	return std::max(path.ReverseFind(_T('\\')), path.ReverseFind(_T('/')));
}

CString GetDirectory(const CString& path)
{
	int slashPos = GetSlashPosition(path);
	if (slashPos < 0) {
		// no delims('\\' or '/') found
		return CString();
	}

	return path.Mid(0, slashPos);
}

template <typename F>
static CString GetPathString(F func)
{
	std::vector<TCHAR> buf(MAX_PATH);

	size_t len = 0;
	while((len=func(static_cast<DWORD>(buf.size()), &(buf[0]))) >= buf.size()) {
		buf.resize(len);
	}
	if(len==0) {
		return CString();
	}

	return CString(&(buf[0]));
}

CString GetCurrentDir()
{
	return GetPathString(std::ptr_fun(GetCurrentDirectory));
}

CString GetAppPath()
{
	std::vector<TCHAR> buf(MAX_PATH/2);

	DWORD len = 0;
    do {
		buf.resize(buf.size()*2);
        len = GetModuleFileName(NULL, &(buf[0]), static_cast<DWORD>(buf.size()));
    } while(len == buf.size());

    return CString(&(buf[0]), len);
}

CString GetAppDirectory()
{
	CString appPath = GetAppPath();
	return GetDirectory(appPath);
}

bool LaunchCommand(CString command, bool bWait /* = false */)
{
	STARTUPINFO startup = {};
	startup.cb = sizeof(startup);

	PROCESS_INFORMATION process = {};

	if(CreateProcess(NULL, command.GetBuffer(), NULL, NULL, FALSE, 0, 0, NULL, &startup, &process)==FALSE) {
		return false;
	}

	CloseHandle(process.hThread);

	if(bWait) {
		WaitForSingleObject(process.hProcess, INFINITE);
	}
	CloseHandle(process.hProcess);
	return true;
}

bool LaunchProcCommand(const CString& cmdLine)
{
	CString cmd;
	cmd.Format(_T("\"%s\\TortoiseProc.exe\" %s"), GetAppDirectory(), cmdLine);

	return LaunchCommand(cmd);
}