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
#include "CommandResultACP.h"

#include <cassert>

CommandResultACP::CommandResultACP(const PROCESS_INFORMATION& pi, const shared_handle& hRead)
	: m_Pi(pi), m_hRead(hRead), m_exitCode(0)
{
}

CommandResultACP::~CommandResultACP()
{
	Join();
}

size_t CommandResultACP::Read(char* buf, size_t len)
{
	assert(buf && "buf must not be NULL");
	assert(len>0 && "buffer size must be more than 0");

	DWORD read_size = 0;
	if(!ReadFile(m_hRead.get(), buf, len, &read_size, NULL)) {
		read_size = 0;
	}

	return read_size;
}

bool CommandResultACP::Join()
{
	if(m_Pi.dwProcessId == 0) {
		return true;
	}

	CloseHandle(m_Pi.hThread);

	WaitForSingleObject(m_Pi.hProcess, INFINITE);

	m_Pi.dwProcessId = 0;

	if(!GetExitCodeProcess(m_Pi.hProcess, &m_exitCode))
	{
		return false;
	}

	CloseHandle(m_Pi.hProcess);

	return true;
}

int CommandResultACP::GetExitCode() const
{
	return static_cast<int>(m_exitCode);
}
