// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2012 - TortoiseGit

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

#include "Atomic.h"

TEST(AtomicBoolTest, DefaultConstructor)
{
	Atomic<bool> a;

	EXPECT_FALSE(a);
}

TEST(AtomicBoolTest, Constructor)
{
	{
		Atomic<bool> a(true);
		EXPECT_TRUE(a);
	}
	{
		Atomic<bool> a(false);
		EXPECT_FALSE(a);
	}
}

TEST(AtomicBoolTest, AssignBoolean)
{
	{
		Atomic<bool> a(true);
		EXPECT_TRUE(a);

		a = false;
		EXPECT_FALSE(a);
	}
	{
		Atomic<bool> a(false);
		EXPECT_FALSE(a);

		a = true;
		EXPECT_TRUE(a);
	}
}

TEST(AtomicBoolTest, Acquire)
{
	{
		Atomic<bool> a(true);
		EXPECT_TRUE(a);

		EXPECT_FALSE(a.Acquire());
		EXPECT_TRUE(a);
	}
	{
		Atomic<bool> a(false);
		EXPECT_FALSE(a);

		EXPECT_TRUE(a.Acquire());
		EXPECT_TRUE(a);
	}
}

TEST(AtomicBoolTest, Release)
{
	{
		Atomic<bool> a(true);
		EXPECT_TRUE(a);

		EXPECT_TRUE(a.Release());
		EXPECT_FALSE(a);
	}
	{
		Atomic<bool> a(false);
		EXPECT_FALSE(a);

		EXPECT_FALSE(a.Release());
		EXPECT_FALSE(a);
	}
}
