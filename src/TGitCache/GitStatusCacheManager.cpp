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

#include "stdafx.h"

#include "GitStatusCacheManager.h"

#include "FakeGitStatusCacheEntry.h"
#include "SimpleGitStatusCacheEntry.h"
#include "StaticInit.h"
#include "Utilities.h"


static shared_ptr<GitStatusCacheManager> g_manager;

STATIC_INIT()
{
	g_manager.reset(new GitStatusCacheManager);
}

shared_ptr<GitStatusCacheManager> GetGlobalGitStatusCacheManager()
{
	return g_manager;
}


GitStatusCacheManager::GitStatusCacheManager()
{
}

GitStatusCacheManager::~GitStatusCacheManager()
{
}

git_status_type GitStatusCacheManager::GetStatus(const CString& path) const
{
	shared_ptr<GitStatusCacheEntry> entry = GetEntry(path);

	return entry->GetStatus();
}

shared_ptr<GitStatusCacheEntry> GitStatusCacheManager::GetEntry(const CString& path) const
{
	if(!IsInGitRepository(path)) {
		return GetFakeEntry();
	}

	return AllocateEntry(path);
}

bool GitStatusCacheManager::IsInGitRepository(const CString& path) const
{
	return ::IsInGitRepository(path);
}

shared_ptr<GitStatusCacheEntry> GitStatusCacheManager::GetFakeEntry() const
{
	return FakeGitStatusCacheEntry::GetInstance();
}

shared_ptr<GitStatusCacheEntry> GitStatusCacheManager::AllocateEntry(const CString& path) const
{
	return shared_ptr<GitStatusCacheEntry>(new SimpleGitStatusCacheEntry(path));
}
