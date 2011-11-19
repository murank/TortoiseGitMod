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

#include "CommandRunner.h"

#include "CommandResult.h"
#include "Environment.h"
#include "MsysGitDir.h"

using namespace ::testing;

namespace {

class MockCommandRunner : public CommandRunner {
public:

	MOCK_METHOD4(DoRunAsync, shared_ptr<CommandResult>(CString command, const CString& currentDir, LPTSTR env, int encoding));
	MOCK_METHOD5(DoRun, int(const CString& command, const CString& currentDir, int encoding, std::vector<char>& out, size_t BUFFER_SIZE));

	int CommandRunnerDoRun(const CString& command, const CString& currentDir, int encoding, std::vector<char>& out, size_t BUFFER_SIZE)
	{
		return CommandRunner::DoRun(command, currentDir, encoding, out, BUFFER_SIZE);
	}
};

class CommandRunnerTest : public ::testing::Test {
protected:
	virtual void SetUp() {
		SetGlobalMsysGitDir(_T("C:\\Program Files\\msys\\bin"));

		shared_ptr<::Environment> env(new ::Environment);
		SetGlobalEnvironment(env);
	}
};

class MockCommandResultHelper {
public:
	std::deque<char> data;

	MockCommandResultHelper(std::string d) : data(d.begin(), d.end())
	{	
	}

	size_t Read(char* buf, size_t len) {
		size_t size_write = std::min(data.size(), len);

		if(size_write == 0) {
			return 0;
		}

		std::copy(data.begin(), data.begin()+size_write, buf);
		data.erase(data.begin(), data.begin()+size_write);

		return size_write;
	}
};

class MockCommandResult : public CommandResult {
public:

	MOCK_METHOD2(Read, size_t(char* buf, size_t len));
	MOCK_METHOD0(Join, bool());
	MOCK_CONST_METHOD0(GetExitCode, int());
};

} // namespace

static void RunAsyncTestHelper(const char* command, const char* expectedCommand, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	MockCommandRunner mcr;
	shared_ptr<MockCommandResult> ret(new MockCommandResult);

	CString currentDir("C:\\somedir");
	int encoding = CP_UTF8;

	ON_CALL(mcr, DoRunAsync(_, _, _, _))
		.WillByDefault(Return(shared_ptr<CommandResult>()));
	EXPECT_CALL(mcr, DoRunAsync(CString(expectedCommand), currentDir, IsNull(), encoding))
		.WillOnce(Return(ret));

	EXPECT_EQ(ret, mcr.RunAsync(CString(command), currentDir, encoding));
	
	EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mcr));
}

TEST_F(CommandRunnerTest, RunAsync)
{
	RunAsyncTestHelper("git", "\"C:\\Program Files\\msys\\bin\\git\"", __LINE__);
	RunAsyncTestHelper("git diff --cached \"somedir\\another\"", "\"C:\\Program Files\\msys\\bin\\git\" diff --cached \"somedir\\another\"", __LINE__);
	RunAsyncTestHelper("git.exe", "\"C:\\Program Files\\msys\\bin\\git.exe\"", __LINE__);
	RunAsyncTestHelper("git.exe diff --cached \"somedir\\another\"", "\"C:\\Program Files\\msys\\bin\\git.exe\" diff --cached \"somedir\\another\"", __LINE__);
	RunAsyncTestHelper("git.cmd", "\"C:\\Program Files\\msys\\bin\\git.cmd\"", __LINE__);
	RunAsyncTestHelper("git.cmd diff --cached \"somedir\\another\"", "\"C:\\Program Files\\msys\\bin\\git.cmd\" diff --cached \"somedir\\another\"", __LINE__);
	RunAsyncTestHelper("gitcommand", "gitcommand", __LINE__);
	RunAsyncTestHelper("gitcommand diff --cached \"somedir\\another\"", "gitcommand diff --cached \"somedir\\another\"", __LINE__);

	RunAsyncTestHelper("somecommand \"with space\"", "somecommand \"with space\"", __LINE__);
}

TEST_F(CommandRunnerTest, DoRunWithRunAsyncFailure)
{
	MockCommandRunner mcr;

	EXPECT_CALL(mcr, DoRunAsync(_, _, _, _))
		.WillOnce(Return(shared_ptr<CommandResult>()));

	std::vector<char> dummy;
	EXPECT_EQ(-1, mcr.CommandRunnerDoRun(_T("command"), _T("C:\\somedir"), CP_UTF8, dummy, 1024));
}

static void DoRunTestHelper(const std::string& data, size_t BUFFER_SIZE, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	MockCommandResultHelper helper(data);
	shared_ptr<MockCommandResult> ret(new MockCommandResult);

	EXPECT_CALL(*ret, Read(_, _))
		.WillRepeatedly(Invoke(&helper, &MockCommandResultHelper::Read));
	EXPECT_CALL(*ret, Join())
		.WillOnce(Return(true));
	EXPECT_CALL(*ret, GetExitCode())
		.WillOnce(Return(99));

	MockCommandRunner mcr;

	EXPECT_CALL(mcr, DoRunAsync(_, _, _, _))
		.WillOnce(Return(ret));

	std::vector<char> out;
	EXPECT_EQ(99, mcr.CommandRunnerDoRun(_T("dummy command"), _T("dummy dir"), CP_UTF8, out, BUFFER_SIZE));
	EXPECT_EQ(std::vector<char>(data.begin(), data.end()), out);
}

TEST_F(CommandRunnerTest, DoRun)
{
	DoRunTestHelper("ABC", 2, __LINE__);
	DoRunTestHelper("ABC", 3, __LINE__);
	DoRunTestHelper("ABC", 4, __LINE__);
	DoRunTestHelper("ABC", 5, __LINE__);
	DoRunTestHelper("ABC", 1024, __LINE__);
}

TEST_F(CommandRunnerTest, RunAsCStringWithDoRunFailure)
{
	MockCommandRunner mcr;

	EXPECT_CALL(mcr, DoRun(_, _, _, _, _))
		.WillOnce(Return(99));

	CString out("XXX");
	EXPECT_EQ(99, mcr.Run(_T("dummy command"), _T("dummy dir"), CP_UTF8, out));
	EXPECT_EQ(CString("XXX"), out);
}

static void RunAsCStringTestHelper(const std::string& data, const CString& expectedOut, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	MockCommandRunner mcr;

	EXPECT_CALL(mcr, DoRun(_, _, _, _, _))
		.WillOnce(DoAll(SetArgReferee<3>(std::vector<char>(data.begin(), data.end())), Return(0)));

	CString out;
	EXPECT_EQ(0, mcr.Run(_T("dummy command"), _T("dummy dir"), CP_UTF8, out));
	EXPECT_PRED2(StrictlyCompareCString, expectedOut, out);
}

TEST_F(CommandRunnerTest, RunAsCString)
{
	RunAsCStringTestHelper(std::string(), CString(), __LINE__);
	RunAsCStringTestHelper(std::string("aaa"), CString(_T("aaa")), __LINE__);
	RunAsCStringTestHelper(std::string("aaa\0bbb\0", 8), CString(_T("aaa\0bbb\0"), 8), __LINE__);
}

TEST_F(CommandRunnerTest, RunAsCStringVectorWithDoRunFailure)
{
	MockCommandRunner mcr;

	EXPECT_CALL(mcr, DoRun(_, _, _, _, _))
		.WillOnce(Return(99));

	std::vector<CString> out;
	EXPECT_EQ(99, mcr.Run(_T("dummy command"), _T("dummy dir"), CP_UTF8, out));
	EXPECT_TRUE(out.empty());
}

static void RunAsCStringVectorTestHelper(const std::string& data, const std::vector<CString>& expectedOut, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	MockCommandRunner mcr;

	EXPECT_CALL(mcr, DoRun(_, _, _, _, _))
		.WillOnce(DoAll(SetArgReferee<3>(std::vector<char>(data.begin(), data.end())), Return(0)));

	std::vector<CString> out;
	EXPECT_EQ(0, mcr.Run(_T("dummy command"), _T("dummy dir"), CP_UTF8, out));
	EXPECT_EQ(expectedOut, out);
}

TEST_F(CommandRunnerTest, RunAsCStringVector)
{
	{
		std::vector<CString> expectedOut;

		RunAsCStringVectorTestHelper(std::string(""), expectedOut, __LINE__);
	}
	{
		std::vector<CString> expectedOut;
		expectedOut.push_back(CString("aaa"));

		RunAsCStringVectorTestHelper(std::string("aaa", 3), expectedOut, __LINE__);
	}
	{
		std::vector<CString> expectedOut;
		expectedOut.push_back(CString(""));
		expectedOut.push_back(CString(""));

		RunAsCStringVectorTestHelper(std::string("\0", 1), expectedOut, __LINE__);
	}
	{
		std::vector<CString> expectedOut;
		expectedOut.push_back(CString(""));
		expectedOut.push_back(CString("aaa"));
		expectedOut.push_back(CString("bbb ccc"));

		RunAsCStringVectorTestHelper(std::string("\0aaa\0bbb ccc", 12), expectedOut, __LINE__);
	}
	{
		std::vector<CString> expectedOut;
		expectedOut.push_back(CString("aaa"));
		expectedOut.push_back(CString(""));
		expectedOut.push_back(CString("bbb ccc"));

		RunAsCStringVectorTestHelper(std::string("aaa\0\0bbb ccc", 12), expectedOut, __LINE__);
	}
	{
		std::vector<CString> expectedOut;
		expectedOut.push_back(CString("aaa"));
		expectedOut.push_back(CString("bbb ccc"));
		expectedOut.push_back(CString(""));

		RunAsCStringVectorTestHelper(std::string("aaa\0bbb ccc\0", 12), expectedOut, __LINE__);
	}
	{
		std::vector<CString> expectedOut;
		expectedOut.push_back(CString(""));
		expectedOut.push_back(CString(""));
		expectedOut.push_back(CString("aaa"));

		RunAsCStringVectorTestHelper(std::string("\0\0aaa", 5), expectedOut, __LINE__);
	}
	{
		std::vector<CString> expectedOut;
		expectedOut.push_back(CString("aaa"));
		expectedOut.push_back(CString(""));
		expectedOut.push_back(CString(""));
		expectedOut.push_back(CString("bbb"));

		RunAsCStringVectorTestHelper(std::string("aaa\0\0\0bbb", 9), expectedOut, __LINE__);
	}
	{
		std::vector<CString> expectedOut;
		expectedOut.push_back(CString("aaa"));
		expectedOut.push_back(CString(""));
		expectedOut.push_back(CString(""));

		RunAsCStringVectorTestHelper(std::string("aaa\0\0", 5), expectedOut, __LINE__);
	}
}
