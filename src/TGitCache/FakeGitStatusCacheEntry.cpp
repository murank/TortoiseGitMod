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

#include "FakeGitStatusCacheEntry.h"

#include "StaticInit.h"


static shared_ptr<GitStatusCacheEntry> g_fake;

STATIC_INIT(g_fake.reset(new FakeGitStatusCacheEntry));


FakeGitStatusCacheEntry::FakeGitStatusCacheEntry()
{
}

FakeGitStatusCacheEntry::~FakeGitStatusCacheEntry()
{
}

git_status_type FakeGitStatusCacheEntry::GetStatus() const
{
	return git_status_type_none;
}

void FakeGitStatusCacheEntry::Invalidate()
{
}

shared_ptr<GitStatusCacheEntry> FakeGitStatusCacheEntry::GetInstance()
{
	return g_fake;
}
