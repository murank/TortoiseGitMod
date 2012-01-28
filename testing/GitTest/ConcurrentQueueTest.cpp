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

#include "ConcurrentQueue.h"

using namespace ::testing;

namespace {

class MockConcurrentQueue : public ConcurrentQueue<int> {
public:

	MOCK_METHOD1(Wait, bool(AutoLocker& lock));
	MOCK_METHOD0(Notify, void());

	void PushVal() {
		Push(99);
	}

};

} // namespace

TEST(ConcurrentQueue, PopAfterCancelation)
{
	MockConcurrentQueue mcq;

	EXPECT_CALL(mcq, Wait(_))
		.Times(0);
	EXPECT_CALL(mcq, Notify())
		.Times(1);

	mcq.CancelAsync();

	EXPECT_THROW(mcq.Pop(), pop_interrupted);
}

TEST(ConcurrentQueue, CanceleWhileWaiting)
{
	MockConcurrentQueue mcq;

	EXPECT_CALL(mcq, Notify())
		.Times(1);
	EXPECT_CALL(mcq, Wait(_))
		.Times(1)
		.WillOnce(DoAll(IgnoreResult(InvokeWithoutArgs(&mcq, &MockConcurrentQueue::CancelAsync)), Return(true)));

	EXPECT_THROW(mcq.Pop(), pop_interrupted);
}

TEST(ConcurrentQueue, Push)
{
	MockConcurrentQueue mcq;

	EXPECT_CALL(mcq, Notify())
		.Times(1);

	int x = 99;

	mcq.Push(x);
}

TEST(ConcurrentQueue, PopAfterPushing)
{
	MockConcurrentQueue mcq;

	EXPECT_CALL(mcq, Notify())
		.Times(2);
	EXPECT_CALL(mcq, Wait(_))
		.Times(0);

	int x1 = 100;
	int x2 = 200;

	mcq.Push(x1);
	mcq.Push(x2);

	EXPECT_EQ(x1, mcq.Pop());
	EXPECT_EQ(x2, mcq.Pop());
}

TEST(ConcurrentQueue, PushWhileWaiting)
{
	MockConcurrentQueue mcq;

	EXPECT_CALL(mcq, Notify())
		.Times(1);
	EXPECT_CALL(mcq, Wait(_))
		.Times(1)
		.WillOnce(DoAll(InvokeWithoutArgs(&mcq, &MockConcurrentQueue::PushVal), Return(true)));

	EXPECT_EQ(99, mcq.Pop());
}
