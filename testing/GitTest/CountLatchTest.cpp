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

#include "CountLatch.h"

using namespace ::testing;

namespace {

class MockCountLatch : public CountLatch {
public:
	MOCK_METHOD1(Wait, bool(AutoLocker& lock));
	MOCK_METHOD0(Notify, void());

	using CountLatch::SetCount;
};

} // namespace

TEST(CountLatch, CountUp)
{
	MockCountLatch mcl;

	EXPECT_CALL(mcl, Wait(_))
		.Times(0);
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	EXPECT_EQ(0, mcl.GetCount());

	EXPECT_TRUE(mcl.CountUp());

	EXPECT_EQ(1, mcl.GetCount());
}

TEST(CountLatch, CountUpUntilSaturation)
{
	MockCountLatch mcl;

	EXPECT_CALL(mcl, Wait(_))
		.Times(0);
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	EXPECT_EQ(0, mcl.GetCount());

	typedef CountLatch::counter_type c_type;
	const c_type max_value = std::numeric_limits<CountLatch::counter_type>::max();

	mcl.SetCount(max_value-1);

	EXPECT_EQ(max_value-1, mcl.GetCount());

	EXPECT_TRUE(mcl.CountUp());
	EXPECT_EQ(max_value, mcl.GetCount());

	EXPECT_FALSE(mcl.CountUp());
	EXPECT_EQ(max_value, mcl.GetCount());
}

TEST(CountLatch, CountDown)
{
	MockCountLatch mcl;

	EXPECT_CALL(mcl, Wait(_))
		.Times(0);
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	EXPECT_EQ(0, mcl.GetCount());

	mcl.CountDown();

	EXPECT_EQ(-1, mcl.GetCount());
}

TEST(CountLatch, CountDownUntilSaturation)
{
	MockCountLatch mcl;

	EXPECT_CALL(mcl, Wait(_))
		.Times(0);
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	EXPECT_EQ(0, mcl.GetCount());

	typedef CountLatch::counter_type c_type;
	const c_type min_value = std::numeric_limits<CountLatch::counter_type>::min();

	mcl.SetCount(min_value+1);

	EXPECT_EQ(min_value+1, mcl.GetCount());

	EXPECT_TRUE(mcl.CountDown());
	EXPECT_EQ(min_value, mcl.GetCount());

	EXPECT_FALSE(mcl.CountDown());
	EXPECT_EQ(min_value, mcl.GetCount());
}

TEST(CountLatch, NotifyWhenCountDown)
{
	MockCountLatch mcl;

	EXPECT_CALL(mcl, Wait(_))
		.Times(0);
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	mcl.CountUp();

	EXPECT_CALL(mcl, Notify())
		.Times(1);

	mcl.CountDown();
}

TEST(CountLatch, NotifyWhenCountUp)
{
	MockCountLatch mcl;

	EXPECT_CALL(mcl, Wait(_))
		.Times(0);
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	mcl.CountDown();

	EXPECT_CALL(mcl, Notify())
		.Times(1);

	mcl.CountUp();
}

TEST(CountLatch, AwaitWhenCountIs0)
{
	MockCountLatch mcl;

	EXPECT_CALL(mcl, Wait(_))
		.Times(0);
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	EXPECT_EQ(0, mcl.GetCount());

	mcl.Await();
}

TEST(CountLatch, AwaitWhenCountIsNot0)
{
	MockCountLatch mcl;

	EXPECT_CALL(mcl, Wait(_))
		.Times(0);
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	mcl.CountUp();

	EXPECT_CALL(mcl, Wait(_))
		.Times(1)
		.WillOnce(DoAll(IgnoreResult(InvokeWithoutArgs(&mcl, &MockCountLatch::CountDown)), Return(true)));
	EXPECT_CALL(mcl, Notify())
		.Times(1);

	EXPECT_NE(0, mcl.GetCount());

	mcl.Await();
}

TEST(CountLatch, AwaitWithWaitFailure)
{
	MockCountLatch mcl;

	EXPECT_CALL(mcl, Wait(_))
		.Times(0);
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	mcl.CountUp();

	EXPECT_CALL(mcl, Wait(_))
		.Times(1)
		.WillOnce(Return(false));
	EXPECT_CALL(mcl, Notify())
		.Times(0);

	EXPECT_NE(0, mcl.GetCount());

	mcl.Await();
}
