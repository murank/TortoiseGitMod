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

#ifndef PROC_COMMAND_FACTORY_H
#define PROC_COMMAND_FACTORY_H

#include "SharedPtr.h"

class ProcCommand;
typedef shared_ptr<ProcCommand> (*ProcCommandFactoryFunc)();


class ProcCommandFactory {
public:

	static void Register(const CString& command, ProcCommandFactoryFunc f);
	static shared_ptr<ProcCommand> GetCommand(const CString& command);
	static bool HasCommand(const CString& command);
};

#endif
