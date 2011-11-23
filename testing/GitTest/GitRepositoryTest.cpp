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
#include <gmock/gmock.h>
#include "TestHelper.h"

#include "GitRepository.h"

namespace {

class MockGitRepository : public GitRepository {
public:

	MockGitRepository(const CString& root, int encoding)
		: GitRepository(root, encoding)
	{}
	virtual git_status_type GetStatus(const CString& path) const
	{
		return git_status_type_none;
	}
};

} // namespace

static CString GetRelativePathTestHelper(const CString& root, const CString& path)
{
	MockGitRepository rep(root, CP_ACP);

	return rep.GetRelativePath(path);
}

TEST(GitRepositoryTest, GetRelativePath)
{
	EXPECT_EQ(CString("some dir\\some file.txt"), GetRelativePathTestHelper(_T("C:"), _T("C:\\some dir\\some file.txt")));
	EXPECT_EQ(CString(""), GetRelativePathTestHelper(_T("C:"), _T("C:")));
	EXPECT_EQ(CString("some dir\\some file.txt"), GetRelativePathTestHelper(_T("C:\\somedir"), _T("C:\\somedir\\some dir\\some file.txt")));
	EXPECT_EQ(CString(""), GetRelativePathTestHelper(_T("C:\\somedir"), _T("C:\\somedir")));
	EXPECT_EQ(CString("some dir\\some file.txt"), GetRelativePathTestHelper(_T("\\\\server\\dir"), _T("\\\\server\\dir\\some dir\\some file.txt")));
	EXPECT_EQ(CString(""), GetRelativePathTestHelper(_T("\\\\server\\dir"), _T("\\\\server\\dir")));

	EXPECT_THROW(GetRelativePathTestHelper(_T("C:\\somedir"), _T("")), std::invalid_argument);
	EXPECT_THROW(GetRelativePathTestHelper(_T("C:\\somedir"), _T("C:\\anotherdir\\some file.txt")), std::invalid_argument);
}
