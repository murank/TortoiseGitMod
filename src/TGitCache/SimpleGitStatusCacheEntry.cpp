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

#include "SimpleGitStatusCacheEntry.h"

#include "CommandRunner.h"
#include "GitRepository.h"
#include "Utilities.h"


SimpleGitStatusCacheEntry::SimpleGitStatusCacheEntry(const CString& path)
	: m_path(path), m_repository()
{
	InitRepository(path);
}

SimpleGitStatusCacheEntry::SimpleGitStatusCacheEntry()
{
}

SimpleGitStatusCacheEntry::~SimpleGitStatusCacheEntry()
{
}

git_status_type SimpleGitStatusCacheEntry::GetStatus() const
{
	return m_repository->GetStatus(m_path);
}

void SimpleGitStatusCacheEntry::Invalidate()
{
}

static shared_ptr<GitRepository> AllocateRepository(const CString& root)
{
	return GitRepository::Create(root);
}

void SimpleGitStatusCacheEntry::InitRepository(const CString& path)
{
	CString root = GetRepositoryRoot(path);

	if(root.IsEmpty()) {
		throw std::invalid_argument("the path isn't a repository");
	}

	m_repository = AllocateRepository(root);
}
