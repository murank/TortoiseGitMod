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

#include "GitRepositoryCommand.h"

namespace {

class MockGitRepositoryCommand : public GitRepositoryCommand {
public:

	MockGitRepositoryCommand(const CString& root, int encoding)
		: GitRepositoryCommand(root, encoding)
	{}
	MOCK_CONST_METHOD1(GetStatusStrings, std::vector<CString>(const CString& path));
	MOCK_CONST_METHOD2(Run, int(const CString& command, std::vector<CString>& out));
	MOCK_CONST_METHOD1(IsEmptyDir, bool(const CString& path));

	std::vector<CString> ParentGetStatusStrings(const CString& path) const
	{
		return GitRepositoryCommand::GetStatusStrings(path);
	}
};

} // namespace

using namespace ::testing;

TEST(GitRepositoryCommandTest, GetStatusStrings)
{
	MockGitRepositoryCommand rep(CString("C:\\somedir"), CP_ACP);

	std::vector<CString> ret;
	ret.push_back(CString("aaa"));
	ret.push_back(CString("bbb"));

	EXPECT_CALL(rep, Run(CString(_T("git.exe status --porcelain --ignored -z \"\"")), _))
		.WillOnce(DoAll(SetArgReferee<1>(ret), Return(0)));
	EXPECT_CALL(rep, Run(CString(_T("git.exe status --porcelain --ignored -z \"some dir/some file\"")), _))
		.WillOnce(DoAll(SetArgReferee<1>(ret), Return(0)));
	EXPECT_CALL(rep, Run(CString(_T("git.exe status --porcelain --ignored -z \"with failure\"")), _))
		.WillOnce(Return(1));

	EXPECT_EQ(ret, rep.ParentGetStatusStrings(CString("C:\\somedir")));
	EXPECT_EQ(ret, rep.ParentGetStatusStrings(CString("C:\\somedir\\some dir\\some file")));
	EXPECT_EQ(std::vector<CString>(), rep.ParentGetStatusStrings(CString("C:\\somedir\\with failure")));
}

static void AddData(const CString& statusCode, std::vector<CString>& data)
{
	data.push_back(CString(statusCode)+_T(" somefile"));
	if((statusCode.GetLength()>=2) && ((statusCode[0]==_T('R')) || (statusCode[1]==_T('R')))) {
		data.push_back(CString("anotherfile"));
	}
}

static void GetStatusFromOneResultTestHelper(const CString& statusCode, git_status_type expectedStatus, int line)
{
	std::vector<CString> data;
	AddData(statusCode, data);

	MockGitRepositoryCommand rep(CString("C:\\somedir"), CP_ACP);

	EXPECT_CALL(rep, GetStatusStrings(_))
		.WillOnce(Return(data));

	EXPECT_EQ(expectedStatus, rep.GetStatus(CString())) << " called from " __FILE__ "(" << line << ")";
}

TEST(GitRepositoryTest, GetStatusFromOneResult)
{
	GetStatusFromOneResultTestHelper(_T(" M"), git_status_type_modified, __LINE__);
	GetStatusFromOneResultTestHelper(_T(" D"), git_status_type_deleted, __LINE__);
	GetStatusFromOneResultTestHelper(_T("M "), git_status_type_modified, __LINE__);
	GetStatusFromOneResultTestHelper(_T("MM"), git_status_type_modified, __LINE__);
	GetStatusFromOneResultTestHelper(_T("MD"), git_status_type_modified, __LINE__);
	GetStatusFromOneResultTestHelper(_T("A "), git_status_type_added, __LINE__);
	GetStatusFromOneResultTestHelper(_T("AM"), git_status_type_modified, __LINE__);
	GetStatusFromOneResultTestHelper(_T("AA"), git_status_type_unmerged, __LINE__);
	GetStatusFromOneResultTestHelper(_T("AD"), git_status_type_deleted, __LINE__);
	GetStatusFromOneResultTestHelper(_T("AU"), git_status_type_unmerged, __LINE__);
	GetStatusFromOneResultTestHelper(_T("D "), git_status_type_deleted, __LINE__);
	GetStatusFromOneResultTestHelper(_T("DM"), git_status_type_modified, __LINE__);
	GetStatusFromOneResultTestHelper(_T("DD"), git_status_type_deleted, __LINE__);
	GetStatusFromOneResultTestHelper(_T("DU"), git_status_type_unmerged, __LINE__);
	GetStatusFromOneResultTestHelper(_T("R "), git_status_type_renamed, __LINE__);
	GetStatusFromOneResultTestHelper(_T("RM"), git_status_type_modified, __LINE__);
	GetStatusFromOneResultTestHelper(_T("RD"), git_status_type_renamed, __LINE__);
	GetStatusFromOneResultTestHelper(_T("C "), git_status_type_copied, __LINE__);
	GetStatusFromOneResultTestHelper(_T("CM"), git_status_type_modified, __LINE__);
	GetStatusFromOneResultTestHelper(_T("CD"), git_status_type_deleted, __LINE__);
	GetStatusFromOneResultTestHelper(_T("UD"), git_status_type_unmerged, __LINE__);
	GetStatusFromOneResultTestHelper(_T("UA"), git_status_type_unmerged, __LINE__);
	GetStatusFromOneResultTestHelper(_T("UU"), git_status_type_unmerged, __LINE__);
	GetStatusFromOneResultTestHelper(_T("??"), git_status_type_untracked, __LINE__);
	GetStatusFromOneResultTestHelper(_T("!!"), git_status_type_ignored, __LINE__);

	GetStatusFromOneResultTestHelper(_T(""), git_status_type_none, __LINE__);
	GetStatusFromOneResultTestHelper(_T("X"), git_status_type_none, __LINE__);
	GetStatusFromOneResultTestHelper(_T("XX"), git_status_type_none, __LINE__);
}

static void GetStatusFromResultsTestHelper(const CString& statusCode1, const CString& statusCode2, git_status_type expectedStatus, int line)
{
	std::vector<CString> data;
	AddData(statusCode1, data);
	AddData(statusCode2, data);

	MockGitRepositoryCommand rep(CString("C:\\somedir"), CP_ACP);

	EXPECT_CALL(rep, GetStatusStrings(_))
		.WillOnce(Return(data));

	EXPECT_EQ(expectedStatus, rep.GetStatus(CString())) << " called from " __FILE__ "(" << line << ")";
}

TEST(GitRepositoryTest, GetStatusFromResults)
{
	// untracked > none
	GetStatusFromResultsTestHelper(_T("??"), _T(""), git_status_type_untracked, __LINE__);
	// ignore > untracked
	GetStatusFromResultsTestHelper(_T("!!"), _T("??"), git_status_type_ignored, __LINE__);
	// added > ignore
	GetStatusFromResultsTestHelper(_T("A "), _T("!!"), git_status_type_added, __LINE__);
	// deleted > added
	GetStatusFromResultsTestHelper(_T(" D"), _T("A "), git_status_type_deleted, __LINE__);
	// renamed > deleted
	GetStatusFromResultsTestHelper(_T("R "), _T(" D"), git_status_type_renamed, __LINE__);
	// modified > renamed
	GetStatusFromResultsTestHelper(_T(" M"), _T("R "), git_status_type_modified, __LINE__);
	// unmerged > modified
	GetStatusFromResultsTestHelper(_T("UU"), _T(" M"), git_status_type_unmerged, __LINE__);
}
