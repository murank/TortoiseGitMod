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

#include "IPCommandReceiverBase.h"

#include "PipeError.h"

namespace {

class MockIPCommandReceiverBase : public IPCommandReceiverBase {
public:

	MockIPCommandReceiverBase() : IPCommandReceiverBase(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD0(CallCommand, void());

	MOCK_METHOD0(WriteResultSuccess, void());
	MOCK_METHOD0(WriteResultFailure, void());

	MOCK_METHOD0(ReceiveRequest, void());
	MOCK_METHOD0(SendResponse, void());
};

} // namespace

using namespace ::testing;

TEST(IPCommandReceiverBase, Run)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.Times(1);
	EXPECT_CALL(micr, WriteResultSuccess())
		.Times(1);
	EXPECT_CALL(micr, SendResponse())
		.Times(1);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiverBase, RunWithFailureThatReceiveRequestThrowsPipeError)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.WillOnce(Throw(pipe_error("mock error")));
	EXPECT_CALL(micr, CallCommand())
		.Times(0);
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiverBase, RunWithFailureThatReceiveRequestThrowsOtherException)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, CallCommand())
		.Times(0);
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	EXPECT_THROW(micr.Run(), std::exception);
}

TEST(IPCommandReceiverBase, RunWithFailureThatCallCommandThrowsPipeError)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.WillOnce(Throw(pipe_error("mock error")));
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiverBase, RunWithFailureThatCallCommandThrowsOtherException)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	EXPECT_THROW(micr.Run(), std::exception);
}

TEST(IPCommandReceiverBase, RunWithFailureThatWriteResultSuccessThrowsPipeError)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.Times(1);
	EXPECT_CALL(micr, WriteResultSuccess())
		.WillOnce(Throw(pipe_error("mock error")));
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiverBase, RunWithFailureThatWriteResultSuccessThrowsOtherException)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.Times(1);
	EXPECT_CALL(micr, WriteResultSuccess())
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	EXPECT_THROW(micr.Run(), std::exception);
}

TEST(IPCommandReceiverBase, RunWithFailureThatSendResponseThrowsPipeError)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.Times(1);
	EXPECT_CALL(micr, WriteResultSuccess())
		.Times(1);
	EXPECT_CALL(micr, SendResponse())
		.WillOnce(Throw(pipe_error("mock error")));
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(0);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiverBase, RunWithFailureThatSendResponseThrowsOtherException)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.Times(1);
	EXPECT_CALL(micr, WriteResultSuccess())
		.Times(1);
	EXPECT_CALL(micr, SendResponse())
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(0);

	EXPECT_THROW(micr.Run(), std::exception);
}

TEST(IPCommandReceiverBase, RunThatThrowsPipeErrors)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.WillOnce(Throw(pipe_error("mock error")));
	EXPECT_CALL(micr, WriteResultFailure())
		.WillOnce(Throw(pipe_error("mock error2")));
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiverBase, RunThatThrowsIntAndPipeError)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.WillOnce(Throw(99));
	EXPECT_CALL(micr, WriteResultFailure())
		.WillOnce(Throw(pipe_error("mock error")));
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	// ignore exception from WriteResultFailure()
	EXPECT_THROW(micr.Run(), int);
}

TEST(IPCommandReceiverBase, RunThatThrowsPipeErrorAndInt)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.WillOnce(Throw(pipe_error("mock error")));
	EXPECT_CALL(micr, WriteResultFailure())
		.WillOnce(Throw(99));
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	// ignore any exception from pipe_error
	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiverBase, RunThatThrowsExceptionAndInt)
{
	MockIPCommandReceiverBase micr;

	InSequence s;

	EXPECT_CALL(micr, ReceiveRequest())
		.Times(1);
	EXPECT_CALL(micr, CallCommand())
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, WriteResultFailure())
		.WillOnce(Throw(99));
	EXPECT_CALL(micr, SendResponse())
		.Times(0);

	// ignore exception from WriteResultFailure()
	EXPECT_THROW(micr.Run(), std::exception);
}
