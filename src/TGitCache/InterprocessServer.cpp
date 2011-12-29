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

#include "InterprocessServer.h"

#include "InterprocessIo.h"
#include "IPConnectionListener.h"

namespace {

enum {
	BUF_SIZE = 4096
};

} // namespace

static shared_ptr<InterprocessServer> g_server;

shared_ptr<InterprocessServer> GetGlobalInterprocessServer()
{
	return g_server;
}

void SetGlobalInterprocessServer(const shared_ptr<InterprocessServer>& server)
{
	g_server = server;
}

InterprocessServer::InterprocessServer(const CString& pipeName)
	: m_pipeName(pipeName), m_listener()
{
}

InterprocessServer::~InterprocessServer()
{
	CancelAsync();
	Join();
}

void InterprocessServer::SetListener(const shared_ptr<IPConnectionListener>& listener)
{
	m_listener = listener;
}

const CString& InterprocessServer::GetPipeName() const
{
	return m_pipeName;
}

static void CreateFakeConnection(const CString& pipeName)
{
	shared_handle hPipe(CreateFile(
		pipeName,		// pipe name
		GENERIC_READ |					// read and write access
		GENERIC_WRITE,
		0,								// no sharing
		NULL,							// default security attributes
		OPEN_EXISTING,					// opens existing pipe
		FILE_FLAG_OVERLAPPED,			// default attributes
		NULL)							// no template file
	);
}

void InterprocessServer::OnCancel()
{
	CreateFakeConnection(GetPipeName());
}

shared_ptr<InterprocessIo> InterprocessServer::Connect()
{
	shared_handle hPipe(CreateNamedPipe(
		GetPipeName(),
		PIPE_ACCESS_DUPLEX |       // read/write access
		FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE |       // message type pipe
		PIPE_READMODE_MESSAGE |   // message-read mode
		PIPE_WAIT,                // non-blocking mode
		PIPE_UNLIMITED_INSTANCES, // max. instances
		BUF_SIZE,                  // output buffer size
		BUF_SIZE,                  // input buffer size
		NMPWAIT_USE_DEFAULT_WAIT, // client time-out
		NULL)					  // NULL DACL
	);
	if(!hPipe) {
		return shared_ptr<InterprocessIo>();
	}

	if(!ConnectNamedPipe(hPipe.get(), NULL) && (GetLastError()!=ERROR_PIPE_CONNECTED)) {
		return shared_ptr<InterprocessIo>();
	}

	return shared_ptr<InterprocessIo>(new InterprocessIo(hPipe));
}

void InterprocessServer::DoRun()
{
	shared_ptr<InterprocessIo> io = Connect();
	if(!io) {
		return;
	}

	if(IsCancelPending()) {
		return;
	}

	OnConnect(io);
}

void InterprocessServer::Run()
{
	while(!IsCancelPending()) {
		DoRun();
	}
}

void InterprocessServer::OnConnect(shared_ptr<InterprocessIo> &io)
{
	m_listener->OnConnect(io);
}
