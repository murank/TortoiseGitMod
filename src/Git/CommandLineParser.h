// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2012 - TortoiseGit

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

#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

class CommandLineArguments;

namespace internal {

bool ReadSeparator(const CString& cmdLine, int& pos);
bool ReadCommandSwitch(const CString& cmdLine, int& pos);
CString ReadKey(const CString& cmdLine, int& pos);
bool ReadValueSeparator(const CString& cmdLine, int& pos);
CString ReadValue(const CString& cmdLine, int& pos);
bool ReadOption(const CString& cmdLine, int& pos, CString& key, CString& value);

} // namespace internal

CommandLineArguments ParseCommandLine(const CString& cmdLine);

#endif
