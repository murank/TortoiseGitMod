// TortoiseGit - a Windows shell extension for easy version control

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

#ifndef GIT_REPOSITORY_COMMAND_H
#define GIT_REPOSITORY_COMMAND_H

#include "GitRepository.h"

class GitRepositoryCommand : public GitRepository {
public:

	GitRepositoryCommand(const CString& root, int encoding);
	virtual ~GitRepositoryCommand();

	virtual git_status_type GetStatus(const CString& path) const;
	virtual bool InitRepository(bool bBare, CString& output);

protected:

	virtual std::vector<CString> GetStatusStrings(const CString& path) const;

private:

	virtual int Run(const CString& command, CString& out) const;
	virtual int Run(const CString& command, std::vector<CString>& out) const;

	template <typename T>
	int DoRun(const CString& command, T& out) const;

	virtual bool IsEmptyDir(const CString& path) const;
};

#endif
