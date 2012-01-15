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

#include "IPCommandTask.h"

#include "FakeIPCommandReceiver.h"
#include "GetStatusCommandReceiver.h"

namespace {

class MockIPCommandTask : public IPCommandTask {
public:

	MockIPCommandTask() : IPCommandTask(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD0(ReadCommandId, int());
	MOCK_METHOD1(DoRunReceiver, void(shared_ptr<IPCommandReceiverBase>& receiver));

};

} // namespace

using namespace ::testing;

template <typename T>
static const T* ReceiverCast(const shared_ptr<IPCommandReceiverBase>& receiver)
{
	return dynamic_cast<const T*>(receiver.get());
}

TEST(IPCommandTask, FakeCommand)
{
	MockIPCommandTask mict;

	EXPECT_CALL(mict, ReadCommandId())
		.WillOnce(Return(0));
	EXPECT_CALL(mict, DoRunReceiver(ResultOf(ReceiverCast<FakeIPCommandReceiver>, NotNull())))
		.Times(1);

	mict.Run();
}

TEST(IPCommandTask, GetStatusCommand)
{
	MockIPCommandTask mict;

	EXPECT_CALL(mict, ReadCommandId())
		.WillOnce(Return(GetStatusCommandReceiver::COMMAND_ID));
	EXPECT_CALL(mict, DoRunReceiver(ResultOf(ReceiverCast<GetStatusCommandReceiver>, NotNull())))
		.Times(1);

	mict.Run();
}
