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

#ifndef CREATE_REPOSITORY_COMMAND_H
#define CREATE_REPOSITORY_COMMAND_H

#include "ProcCommand.h"

class CreateRepositoryCommand : public ProcCommand {
public:

	virtual ~CreateRepositoryCommand();
	static CString GetName();

	virtual bool Execute(const CommandLineArguments& args);


private:

	bool DoExecute(const CString& dir);
	virtual bool DoCreateRepository(const CString& dir, bool bBare, CString& output);

	virtual bool PathIsDirectoryEmpty(const CString& path) const;
	virtual bool IsCreateRepository(const CString& dir, bool& bBare) const;
	virtual bool IsForceCreateRepository(const CString& dir) const;

	virtual void ShowSucceededMessage(const CString& output) const;
	virtual void ShowErrorMessage(const CString& output) const;

};

#endif
