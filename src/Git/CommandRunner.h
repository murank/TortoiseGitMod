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

#ifndef COMMAND_RUNNER_H
#define COMMAND_RUNNER_H

#include <vector>
#include "SharedPtr.h"

class CommandResult;

class CommandRunner {
private:

	virtual shared_ptr<CommandResult> DoRunAsync(CString command, const CString& currentDir, LPTSTR env, int encoding);

protected:

	virtual int DoRun(const CString& command, const CString& currentDir, int encoding, std::vector<char>& out, size_t BUFFER_SIZE);

public:
	CommandRunner();
	virtual ~CommandRunner();

	shared_ptr<CommandResult> RunAsync(const CString& command, const CString& currentDir, int encoding);
	int Run(const CString& command, const CString& currentDir, int encoding, std::vector<char>& out);
	int Run(const CString& command, const CString& currentDir, int encoding, CString& output);
	int Run(const CString& command, const CString& currentDir, int encoding, std::vector<CString>& output);

};

#endif
