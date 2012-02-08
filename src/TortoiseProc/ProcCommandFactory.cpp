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

#include "stdafx.h"

#include "ProcCommandFactory.h"

#include "FakeCommand.h"


namespace {

typedef std::map<CString, ProcCommandFactoryFunc> map_type;

map_type& Instance()
{
	static map_type g_factoreis;
	return g_factoreis;
}

} // namespace


static shared_ptr<ProcCommand> CreateFakeCommand()
{
	return shared_ptr<ProcCommand>(new FakeCommand);
}


static ProcCommandFactoryFunc GetFactoryFunc(const CString& command)
{
	map_type& factories = Instance();
	map_type::iterator it = factories.find(command);

	return (it!=factories.end()) ? it->second : CreateFakeCommand;
}

void ProcCommandFactory::Register(const CString& command, ProcCommandFactoryFunc f)
{
	map_type& factories = Instance();
	factories.insert(std::make_pair(command, f));
}

shared_ptr<ProcCommand> ProcCommandFactory::GetCommand(const CString& command)
{
	ProcCommandFactoryFunc f = GetFactoryFunc(command);
	return f();
}

bool ProcCommandFactory::HasCommand(const CString& command)
{
	map_type& factories = Instance();
	return (factories.count(command) != 0);
}
