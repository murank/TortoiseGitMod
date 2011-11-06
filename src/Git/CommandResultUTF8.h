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

#ifndef COMMAND_RESULT_UTF8_H
#define COMMAND_RESULT_UTF8_H

#include "CommandResult.h"

#include <vector>
#include "SharedPtr.h"

class shared_handle;

class CommandResultUTF8 : public CommandResult {
private:
	const int ENCODING;
	shared_ptr<CommandResult> m_resultACP;
	std::vector<char> m_rawCache;
	std::vector<char> m_convCache;

	size_t AppendReadCache(std::vector<char>& buf, size_t size_cached);
	size_t GetValidCharSequence(int encoding, std::vector<char>& data, size_t& size_cached);

public:

	CommandResultUTF8(PROCESS_INFORMATION pi, const shared_handle& hRead);
	CommandResultUTF8(const shared_ptr<CommandResult>& result);
	virtual ~CommandResultUTF8();

	virtual size_t Read(char* buf, size_t len);
	virtual bool Join();
	virtual int GetExitCode() const;

};

#endif
