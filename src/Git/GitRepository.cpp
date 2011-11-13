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

#include "stdafx.h"
#include "GitRepository.h"

#include "GitRepositoryCommand.h"
#include "registry.h"
#include "Utilities.h"

GitRepository::GitRepository(const CString& root, int encoding)
	: m_root(root), m_encoding(encoding)
{
	assert(root.GetLength() >= 2 && "root must not be empty");
	assert(!EndsWith(root, CString(_T('\\'))));
}

GitRepository::~GitRepository()
{
}

CString GitRepository::GetProjectRoot() const
{
	return m_root;
}

int GitRepository::GetEncoding() const
{
	return m_encoding;
}

CString GitRepository::GetRelativePath(const CString& path) const
{
	if(!StartsWith(path, m_root)) {
		throw std::invalid_argument("path is not in the repository");
	}

	return path.Mid(m_root.GetLength() + 1);
}

static int GetRepositoryEncodingFromRegistory()
{
	return CRegDWORD(_T("Software\\TortoiseGit\\RepositoryEncoding"), CP_ACP);
}

static shared_ptr<GitRepository> CreateInstance(const CString& root, int encoding)
{
	return shared_ptr<GitRepository>(new GitRepositoryCommand(root, encoding));
}

shared_ptr<GitRepository> GitRepository::Create(const CString& root)
{
	int encoding = GetRepositoryEncodingFromRegistory();

	return CreateInstance(root, encoding);
}
