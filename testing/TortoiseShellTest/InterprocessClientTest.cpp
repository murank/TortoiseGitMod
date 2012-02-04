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

#include "StdAfxTest.h"

#include "InterprocessClient.h"

namespace {

class MockInterprocessClient : public InterprocessClient {
public:

	MockInterprocessClient() : InterprocessClient(_T("MockClient")) {}

	MOCK_CONST_METHOD0(RunTGitCache, bool());
	MOCK_CONST_METHOD0(DoConnect, shared_ptr<InterprocessIo>());
	MOCK_CONST_METHOD0(WaitBeforeRetryRunning, void());
	MOCK_CONST_METHOD0(WaitBeforeRetryConnection, void());
};

struct FakeDeletor : std::unary_function<InterprocessIo*, void> {
	void operator()(InterprocessIo*) {}
};

} // namespace

using namespace ::testing;

static shared_ptr<InterprocessIo> CreateFakePtr()
{
	return shared_ptr<InterprocessIo>(reinterpret_cast<InterprocessIo*>(0x1234), FakeDeletor());
}

TEST(InterprocessClient, Connect)
{
	MockInterprocessClient mic;
	shared_ptr<InterprocessIo> io = CreateFakePtr();

	EXPECT_CALL(mic, DoConnect())
		.WillOnce(Return(io));
	EXPECT_CALL(mic, RunTGitCache())
		.Times(0);
	EXPECT_CALL(mic, WaitBeforeRetryConnection())
		.Times(0);
	EXPECT_CALL(mic, WaitBeforeRetryRunning())
		.Times(0);

	EXPECT_EQ(io, mic.Connect());
}

TEST(InterprocessClient, FailToConnect)
{
	MockInterprocessClient mic;
	shared_ptr<InterprocessIo> io = CreateFakePtr();

	EXPECT_CALL(mic, DoConnect())
		.WillOnce(Return(shared_ptr<InterprocessIo>()))
		.WillOnce(Return(io));
	EXPECT_CALL(mic, RunTGitCache())
		.WillOnce(Return(true));
	EXPECT_CALL(mic, WaitBeforeRetryConnection())
		.Times(1);
	EXPECT_CALL(mic, WaitBeforeRetryRunning())
		.Times(0);

	EXPECT_EQ(io, mic.Connect());
}

TEST(InterprocessClient, FailToConnectInMaxTimes)
{
	MockInterprocessClient mic;

	EXPECT_CALL(mic, DoConnect())
		.Times(InterprocessClient::MAX_RETRY_CONNECTION)
		.WillRepeatedly(Return(shared_ptr<InterprocessIo>()));
	EXPECT_CALL(mic, RunTGitCache())
		.Times(InterprocessClient::MAX_RETRY_CONNECTION * InterprocessClient::MAX_RETRY_RUNNING)
		.WillRepeatedly(Return(false));
	EXPECT_CALL(mic, WaitBeforeRetryConnection())
		.Times(InterprocessClient::MAX_RETRY_CONNECTION);
	EXPECT_CALL(mic, WaitBeforeRetryRunning())
		.Times(InterprocessClient::MAX_RETRY_CONNECTION * InterprocessClient::MAX_RETRY_RUNNING);

	EXPECT_EQ(shared_ptr<InterprocessIo>(), mic.Connect());
}

TEST(InterprocessClient, FailToRunTGitCache)
{
	MockInterprocessClient mic;
	shared_ptr<InterprocessIo> io = CreateFakePtr();

	EXPECT_CALL(mic, DoConnect())
		.WillOnce(Return(shared_ptr<InterprocessIo>()))
		.WillOnce(Return(io));
	EXPECT_CALL(mic, RunTGitCache())
		.WillOnce(Return(false))
		.WillOnce(Return(true));
	EXPECT_CALL(mic, WaitBeforeRetryConnection())
		.Times(1);
	EXPECT_CALL(mic, WaitBeforeRetryRunning())
		.Times(1);

	EXPECT_EQ(io, mic.Connect());
}

TEST(InterprocessClient, FailToRunTGitCacheInMaxTimes)
{
	MockInterprocessClient mic;
	shared_ptr<InterprocessIo> io = CreateFakePtr();

	EXPECT_CALL(mic, DoConnect())
		.WillOnce(Return(shared_ptr<InterprocessIo>()))
		.WillOnce(Return(io));
	EXPECT_CALL(mic, RunTGitCache())
		.Times(InterprocessClient::MAX_RETRY_RUNNING)
		.WillRepeatedly(Return(false));
	EXPECT_CALL(mic, WaitBeforeRetryConnection())
		.Times(1);
	EXPECT_CALL(mic, WaitBeforeRetryRunning())
		.Times(InterprocessClient::MAX_RETRY_RUNNING);

	EXPECT_EQ(io, mic.Connect());
}
