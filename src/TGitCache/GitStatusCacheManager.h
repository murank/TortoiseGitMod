// TortoiseGit - a Windows shell extension for easy version control

// External Cache Copyright (C) 2005 - 2006 - Will Dean, Stefan Kueng
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

#ifndef GIT_STATUS_CACHE_MANAGER_H
#define GIT_STATUS_CACHE_MANAGER_H

#include "GitStatusType.h"
#include "SharedPtr.h"

class GitStatusCacheEntry;

class GitStatusCacheManager {
public:

	GitStatusCacheManager();
	virtual ~GitStatusCacheManager();

	git_status_type GetStatus(const CString& path) const;

private:

	shared_ptr<GitStatusCacheEntry> GetEntry(const CString& path) const;

	virtual bool IsInGitRepository(const CString& path) const;
	virtual shared_ptr<GitStatusCacheEntry> GetFakeEntry() const;
	virtual shared_ptr<GitStatusCacheEntry> AllocateEntry(const CString& path) const;

};

shared_ptr<GitStatusCacheManager> GetGlobalGitStatusCacheManager();

#endif
