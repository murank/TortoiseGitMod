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

#include "StdAfxTest.h"

#include "CreateRepositoryCommand.h"

#include "CommandLineArguments.h"

namespace {

class MockCreateRepositoryCommand : public CreateRepositoryCommand {
public:

	MOCK_METHOD3(DoCreateRepository, bool(const CString& dir, bool bBare, CString& output));
	MOCK_CONST_METHOD1(PathIsDirectoryEmpty, bool(const CString& path));
	MOCK_CONST_METHOD2(IsCreateRepository, bool(const CString& dir, bool& bBare));
	MOCK_CONST_METHOD1(IsForceCreateRepository, bool(const CString& dir));
	MOCK_CONST_METHOD1(ShowSucceededMessage, void(const CString& output));
	MOCK_CONST_METHOD1(ShowErrorMessage, void(const CString& output));

};

const CString THE_PATH("C:\\somedir");

class CraeteRepositoryCommandTest : public ::testing::TestWithParam<bool> {
protected:
	virtual void SetUp() {
		m_args.Add(CString("path"), "C:\\somedir");
	}

	CommandLineArguments m_args;
};

} // namespace

using namespace ::testing;

TEST_P(CraeteRepositoryCommandTest, ExecuteWithCancelation)
{
	MockCreateRepositoryCommand crc;

	EXPECT_CALL(crc, IsCreateRepository(THE_PATH, _))
		.WillOnce(Return(false));
	EXPECT_CALL(crc, PathIsDirectoryEmpty(_))
		.Times(0);
	EXPECT_CALL(crc, IsForceCreateRepository(_))
		.Times(0);
	EXPECT_CALL(crc, DoCreateRepository(_, _, _))
		.Times(0);
	EXPECT_CALL(crc, ShowSucceededMessage(_))
		.Times(0);
	EXPECT_CALL(crc, ShowErrorMessage(_))
		.Times(0);

	EXPECT_FALSE(crc.Execute(m_args));
}

TEST_P(CraeteRepositoryCommandTest, ExecuteThatCreatesRepository)
{
	MockCreateRepositoryCommand crc;
	bool bBare = GetParam();
	CString output("output");

	EXPECT_CALL(crc, IsCreateRepository(THE_PATH, _))
		.WillOnce(DoAll(SetArgReferee<1>(bBare), Return(true)));
	EXPECT_CALL(crc, PathIsDirectoryEmpty(THE_PATH))
		.WillOnce(Return(true));
	EXPECT_CALL(crc, IsForceCreateRepository(_))
		.Times(0);
	EXPECT_CALL(crc, DoCreateRepository(THE_PATH, bBare, _))
		.WillOnce(DoAll(SetArgReferee<2>(output), Return(true)));
	EXPECT_CALL(crc, ShowSucceededMessage(output))
		.Times(1);
	EXPECT_CALL(crc, ShowErrorMessage(_))
		.Times(0);

	EXPECT_TRUE(crc.Execute(m_args));
}

TEST_P(CraeteRepositoryCommandTest, ExecuteThatCreateRepositoryWithFailure)
{
	MockCreateRepositoryCommand crc;
	bool bBare = GetParam();
	CString output("output");

	EXPECT_CALL(crc, IsCreateRepository(THE_PATH, _))
		.WillOnce(DoAll(SetArgReferee<1>(bBare), Return(true)));
	EXPECT_CALL(crc, PathIsDirectoryEmpty(THE_PATH))
		.WillOnce(Return(true));
	EXPECT_CALL(crc, IsForceCreateRepository(_))
		.Times(0);
	EXPECT_CALL(crc, DoCreateRepository(THE_PATH, bBare, _))
		.WillOnce(DoAll(SetArgReferee<2>(output), Return(false)));
	EXPECT_CALL(crc, ShowSucceededMessage(_))
		.Times(0);
	EXPECT_CALL(crc, ShowErrorMessage(output))
		.Times(1);

	EXPECT_FALSE(crc.Execute(m_args));
}

TEST_P(CraeteRepositoryCommandTest, ExecuteThatCreateRepositoryToNonEmptyDirAndProceeds)
{
	MockCreateRepositoryCommand crc;
	bool bBare = GetParam();
	CString output("output");

	EXPECT_CALL(crc, IsCreateRepository(THE_PATH, _))
		.WillOnce(DoAll(SetArgReferee<1>(bBare), Return(true)));
	EXPECT_CALL(crc, PathIsDirectoryEmpty(THE_PATH))
		.WillOnce(Return(false));
	EXPECT_CALL(crc, IsForceCreateRepository(THE_PATH))
		.WillOnce(Return(true));
	EXPECT_CALL(crc, DoCreateRepository(THE_PATH, bBare, _))
		.WillOnce(DoAll(SetArgReferee<2>(output), Return(true)));
	EXPECT_CALL(crc, ShowSucceededMessage(output))
		.Times(1);
	EXPECT_CALL(crc, ShowErrorMessage(_))
		.Times(0);

	EXPECT_TRUE(crc.Execute(m_args));
}

TEST_P(CraeteRepositoryCommandTest, ExecuteThatCreateRepositoryToNonEmptyDirAndProceedsAndFails)
{
	MockCreateRepositoryCommand crc;
	bool bBare = GetParam();
	CString output("output");

	EXPECT_CALL(crc, IsCreateRepository(THE_PATH, _))
		.WillOnce(DoAll(SetArgReferee<1>(bBare), Return(true)));
	EXPECT_CALL(crc, PathIsDirectoryEmpty(THE_PATH))
		.WillOnce(Return(false));
	EXPECT_CALL(crc, IsForceCreateRepository(THE_PATH))
		.WillOnce(Return(true));
	EXPECT_CALL(crc, DoCreateRepository(THE_PATH, bBare, _))
		.WillOnce(DoAll(SetArgReferee<2>(output), Return(false)));
	EXPECT_CALL(crc, ShowSucceededMessage(_))
		.Times(0);
	EXPECT_CALL(crc, ShowErrorMessage(output))
		.Times(1);

	EXPECT_FALSE(crc.Execute(m_args));
}

TEST_P(CraeteRepositoryCommandTest, ExecuteThatCreateRepositoryToNonEmptyDirAndAborts)
{
	MockCreateRepositoryCommand crc;
	bool bBare = GetParam();

	EXPECT_CALL(crc, IsCreateRepository(THE_PATH, _))
		.WillOnce(DoAll(SetArgReferee<1>(bBare), Return(true)));
	EXPECT_CALL(crc, PathIsDirectoryEmpty(THE_PATH))
		.WillOnce(Return(false));
	EXPECT_CALL(crc, IsForceCreateRepository(THE_PATH))
		.WillOnce(Return(false));
	EXPECT_CALL(crc, DoCreateRepository(THE_PATH, bBare, _))
		.Times(0);
	EXPECT_CALL(crc, ShowSucceededMessage(_))
		.Times(0);
	EXPECT_CALL(crc, ShowErrorMessage(_))
		.Times(0);

	EXPECT_FALSE(crc.Execute(m_args));
}

INSTANTIATE_TEST_CASE_P(Inst, CraeteRepositoryCommandTest, Bool());
