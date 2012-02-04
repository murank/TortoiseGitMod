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

#ifndef GIT_REPOSITORY_H
#define GIT_REPOSITORY_H

#include "SharedPtr.h"
#include "GitStatusType.h"

class GitRepository {
public:
	
	virtual ~GitRepository();

	CString GetProjectRoot() const;
	int GetEncoding() const;
	CString GetRelativePath(const CString& path) const;

	static shared_ptr<GitRepository> Create(const CString& root);

	virtual git_status_type GetStatus(const CString& path) const = 0;

protected:

	GitRepository(const CString& root, int encoding);

private:

	CString m_root;
	int m_encoding;
};

#endif
