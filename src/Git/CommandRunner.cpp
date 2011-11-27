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

#include "CommandRunner.h"

#include "CommandResultACP.h"
#include "CommandResultUTF8.h"
#include "Environment.h"
#include "MsysGitDir.h"
#include "SharedHandle.h"
#include "Utilities.h"

namespace {

enum {
	BUFFER_SIZE = 4096
};

} // namespace

CommandRunner::CommandRunner()
{
}

CommandRunner::~CommandRunner()
{
}


static SECURITY_ATTRIBUTES InitSecurityAttributes()
{
	SECURITY_ATTRIBUTES sa = {};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor=NULL;
	sa.bInheritHandle=TRUE;

	return sa;
}

static STARTUPINFO InitStartupInfo(HANDLE hOutput, HANDLE hNul)
{
	STARTUPINFO si = {};
	si.cb=sizeof(STARTUPINFO);
	GetStartupInfo(&si);

	si.hStdError = hOutput;
	si.hStdOutput = hOutput;
	si.hStdInput = hNul;

	si.wShowWindow=SW_HIDE;
	si.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;

	return si;
}

static BOOL CreatePipe(shared_handle& hReadPipe, shared_handle& hWritePipe, LPSECURITY_ATTRIBUTES lpPipeAttributes, DWORD nSize)
{
	HANDLE hRead = INVALID_HANDLE_VALUE;
	HANDLE hWrite = INVALID_HANDLE_VALUE;

	BOOL ret = ::CreatePipe(&hRead, &hWrite, lpPipeAttributes, nSize);

	hReadPipe.reset(hRead);
	hWritePipe.reset(hWrite);

	return ret;
}

static bool InitHandles(shared_handle& hRead, shared_handle& hWrite, shared_handle& hNul)
{
	SECURITY_ATTRIBUTES sa = InitSecurityAttributes();

	if(!CreatePipe(hRead, hWrite, &sa, 0))
	{
		return false;
	}

	hNul.reset(CreateFile(_T("nul"), GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
	if(hNul.get() == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

static shared_ptr<CommandResult> CreateCommandResult(PROCESS_INFORMATION& pi, shared_handle& hRead, int encoding)
{
	return (encoding == CP_UTF8 ? shared_ptr<CommandResult>(new CommandResultUTF8(pi, hRead)) : shared_ptr<CommandResult>(new CommandResultACP(pi, hRead)));
}

shared_ptr<CommandResult> CommandRunner::DoRunAsync(CString command, const CString& currentDir, LPTSTR env, int encoding)
{
	shared_handle hRead, hWrite, hNul;
	if(!InitHandles(hRead, hWrite, hNul)) {
		return shared_ptr<CommandResult>();
	}

	STARTUPINFO si = InitStartupInfo(hWrite.get(), hNul.get());

	DWORD dwFlags = env ? CREATE_UNICODE_ENVIRONMENT : 0;
	//DETACHED_PROCESS make ssh recognize that it has no console to launch askpass to input password.
	dwFlags |= DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP;

	PROCESS_INFORMATION pi = {};
	if(!CreateProcess(NULL, command.GetBuffer(), NULL, NULL, TRUE, dwFlags, env, currentDir, &si, &pi))
	{
		return shared_ptr<CommandResult>();
	}

	return CreateCommandResult(pi, hRead, encoding);
}

int CommandRunner::DoRun(const CString& command, const CString& currentDir, int encoding, std::vector<char>& out, size_t BUFFER_SIZE)
{
	shared_ptr<CommandResult> ret = RunAsync(command, currentDir, encoding);
	if(!ret) {
		return -1;
	}

	assert(BUFFER_SIZE > 0 && "buffer size must be more than 0");

	ret->Join();

	const size_t EXPANSION_THRESHOLD = 10;
	std::vector<char> buf(BUFFER_SIZE);
	size_t size = 0;
	size_t read_size = 0;

	while((read_size = ret->Read(&(buf[size]), buf.size()-size-1)) > 0) {
		size += read_size;
		if(size+EXPANSION_THRESHOLD >= buf.size()) {
			buf.resize(2*buf.size());
		}
	}

	buf.resize(size);
	out.swap(buf);

	return ret->GetExitCode();
}

static CString DecorateGitCommand(const CString& command)
{
	if(!StartsWith(command, _T("git"))) {
		return command;
	}
	if((command[3]!=_T('.')) && (command[3]!=_T(' ')) && (command[3]!=_T('\0'))) {
		return command;
	}

	CString ret;
	int firstSpace = command.Find(_T(" "));
	if (firstSpace > 0) {
		ret.Format(_T("\"%s\\%s\"%s"), GetGlobalMsysGitDir(), command.Left(firstSpace), command.Mid(firstSpace));
	} else {
		ret.Format(_T("\"%s\\%s\""), GetGlobalMsysGitDir(), command);
	}

	return ret;
}

shared_ptr<CommandResult> CommandRunner::RunAsync(const CString& command, const CString& currentDir, int encoding)
{
	CString gitCommand = DecorateGitCommand(command);

	shared_ptr<Environment> envPtr = GetGlobalEnvironment();
	LPTSTR env = envPtr->GetPointer();

	return DoRunAsync(gitCommand, currentDir, env, encoding);
}

int CommandRunner::Run(const CString& command, const CString& currentDir, int encoding, std::vector<char>& out)
{
	return DoRun(command, currentDir, encoding, out, BUFFER_SIZE);
}

int CommandRunner::Run(const CString& command, const CString& currentDir, int encoding, CString& out)
{
	std::vector<char> buf;
	int exitCode = Run(command, currentDir, encoding, buf);
	if(exitCode) {
		return exitCode;
	}

	out = (buf.empty() ? CString() : CString(&(buf[0]), buf.size()));

	return exitCode;
}

int CommandRunner::Run(const CString& command, const CString& currentDir, int encoding, std::vector<CString>& out)
{
	std::vector<char> buf;
	int exitCode = Run(command, currentDir, encoding, buf);
	if(exitCode) {
		return exitCode;
	}

	std::vector<CString> ret;

	if(buf.empty()) {
		out.swap(ret);
		return exitCode;
	}

	for(size_t pos=0; pos<buf.size(); ) {
		size_t len = strnlen(&(buf[pos]), buf.size()-pos);
		CString str(&(buf[pos]), len);

		ret.push_back(str);
		pos += len+1;
	}

	out.swap(ret);

	return exitCode;
}
