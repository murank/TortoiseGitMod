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

#ifndef COMMAND_RESULT_ACP_H
#define COMMAND_RESULT_ACP_H

#include "CommandResult.h"

#include "SharedHandle.h"

class CommandResultACP : public CommandResult {
public:

	CommandResultACP(const PROCESS_INFORMATION& pi, const shared_handle& hRead);
	virtual ~CommandResultACP();

	virtual size_t Read(char* buf, size_t len);
	virtual bool Join();
	virtual int GetExitCode() const;

private:
	PROCESS_INFORMATION m_Pi;
	shared_handle m_hRead;
	DWORD m_exitCode;

};

#endif
