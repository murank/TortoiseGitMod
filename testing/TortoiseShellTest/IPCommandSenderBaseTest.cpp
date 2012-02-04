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

#include "IPCommandSenderBase.h"

#include "PipeError.h"

namespace {

class MockIPCommandSenderBase : public IPCommandSenderBase {
public:

	MockIPCommandSenderBase() : IPCommandSenderBase(shared_ptr<InterprocessIo>()) {}

	MOCK_CONST_METHOD0(GetCommandId, int());
	MOCK_METHOD1(WriteCommandId, void(int id));
	MOCK_METHOD0(ReadResult, int());
	MOCK_METHOD0(SendRequest, void());

	using IPCommandSenderBase::DoCall;
};

} // namespace

using namespace ::testing;

TEST(IPCommandSenderBase, DoCall)
{
	MockIPCommandSenderBase mics;
	int id = 99;

	EXPECT_CALL(mics, GetCommandId())
		.WillOnce(Return(id));
	EXPECT_CALL(mics, WriteCommandId(id))
		.Times(1);
	EXPECT_CALL(mics, ReadResult())
		.WillOnce(Return(0));
	EXPECT_CALL(mics, SendRequest())
		.Times(1);

	EXPECT_NO_THROW(mics.DoCall());
}

TEST(IPCommandSenderBase, DoCallWithFailureThatWriteCommandIdThrowsException)
{
	MockIPCommandSenderBase mics;
	int id = 99;

	EXPECT_CALL(mics, GetCommandId())
		.WillOnce(Return(id));
	EXPECT_CALL(mics, WriteCommandId(id))
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(mics, ReadResult())
		.Times(0);
	EXPECT_CALL(mics, SendRequest())
		.Times(0);

	EXPECT_THROW(mics.DoCall(), std::exception);
}

TEST(IPCommandSenderBase, DoCallWithFailureThatSendRequestThrowsException)
{
	MockIPCommandSenderBase mics;
	int id = 99;

	EXPECT_CALL(mics, GetCommandId())
		.WillOnce(Return(id));
	EXPECT_CALL(mics, WriteCommandId(id))
		.Times(1);
	EXPECT_CALL(mics, ReadResult())
		.Times(0);
	EXPECT_CALL(mics, SendRequest())
		.WillOnce(Throw(std::exception()));

	EXPECT_THROW(mics.DoCall(), std::exception);
}

TEST(IPCommandSenderBase, DoCallWithFailureThatReadResultThrowsException)
{
	MockIPCommandSenderBase mics;
	int id = 99;

	EXPECT_CALL(mics, GetCommandId())
		.WillOnce(Return(id));
	EXPECT_CALL(mics, WriteCommandId(id))
		.Times(1);
	EXPECT_CALL(mics, ReadResult())
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(mics, SendRequest())
		.Times(1);

	EXPECT_THROW(mics.DoCall(), std::exception);
}

TEST(IPCommandSenderBase, DoCallWithFailureThatReadResultReturnsNonZero)
{
	MockIPCommandSenderBase mics;
	int id = 99;

	EXPECT_CALL(mics, GetCommandId())
		.WillOnce(Return(id));
	EXPECT_CALL(mics, WriteCommandId(id))
		.Times(1);
	int bad_result = 1;
	EXPECT_CALL(mics, ReadResult())
		.WillOnce(Return(bad_result));
	EXPECT_CALL(mics, SendRequest())
		.Times(1);

	EXPECT_THROW(mics.DoCall(), pipe_error);
}
