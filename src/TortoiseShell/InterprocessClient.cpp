// TortoiseSVN - a Windows shell extension for easy version control

// External Cache Copyright (C) 2005 - 2006 - Will Dean, Stefan Kueng

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

#include "InterprocessClient.h"

#include "InterprocessIo.h"
#include "RegUtils.h"
#include "SharedHandle.h"

static shared_ptr<InterprocessClient> g_client;

shared_ptr<InterprocessClient> GetGlobalInterprocessClient()
{
	return g_client;
}

void SetGlobalInterprocessClient(const shared_ptr<InterprocessClient>& client)
{
	g_client = client;
}

InterprocessClient::InterprocessClient(const CString& pipeName)
	: m_pipeName(pipeName)
{
}

InterprocessClient::~InterprocessClient()
{
}

CString InterprocessClient::GetPipeName() const
{
	return m_pipeName;
}

shared_ptr<InterprocessIo> InterprocessClient::Connect() const
{
	for(size_t i=0; i<MAX_RETRY_CONNECTION; ++i) {
		shared_ptr<InterprocessIo> io = DoConnect();
		if(io) {
			return io;
		}

		for(size_t j=0; j<MAX_RETRY_RUNNING; ++j) {
			if(RunTGitCache()) {
				break;
			}

			WaitBeforeRetryRunning();
		}

		WaitBeforeRetryConnection();
	}

	return shared_ptr<InterprocessIo>();
}

bool InterprocessClient::RunTGitCache() const
{
	STARTUPINFO si = {};
	si.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION pi = {};

	CString cacheExePath = ReadRegistry(HKEY_LOCAL_MACHINE, _T("Software\\TortoiseGit\\CachePath"), _T("TGitCache.exe"));

	if(!CreateProcess(cacheExePath, NULL, NULL, NULL, FALSE, 0, 0, 0, &si, &pi)) {
		return false;
	}
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

	return true;
}

shared_ptr<InterprocessIo> InterprocessClient::DoConnect() const
{
	shared_handle hPipe(CreateFile(
		GetPipeName(),				// pipe name
		GENERIC_READ |				// read and write access
		GENERIC_WRITE,
		0,							// no sharing
		NULL,						// default security attributes
		OPEN_EXISTING,				// opens existing pipe
		FILE_FLAG_OVERLAPPED,		// default attributes
		NULL)						// no template file
	);
	if(!hPipe) {
		return shared_ptr<InterprocessIo>();
	}

	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if(!SetNamedPipeHandleState(hPipe.get(), &dwMode, NULL, NULL)) {
		return shared_ptr<InterprocessIo>();
	}

	return shared_ptr<InterprocessIo>(new InterprocessIo(hPipe));
}

void InterprocessClient::WaitBeforeRetryRunning() const
{
	Sleep(100);
}

void InterprocessClient::WaitBeforeRetryConnection() const
{
	Sleep(100);
}
