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

#include "IPCommandSender.h"

using namespace ::testing;

namespace {

class MockIPCommandSender0void : public IPCommandSender<void()> {
public:

	MockIPCommandSender0void() : IPCommandSender(shared_ptr<InterprocessIo>()) {}
	virtual void WriteCommandId(int /*id*/) {
	}
	virtual int ReadResult() {
		return 0;
	}
	virtual int GetCommandId() const {
		return 99;
	}
};

} // namespace

TEST(MockIPCommandSender0void, Call)
{
	MockIPCommandSender0void mics;

	EXPECT_NO_THROW(mics.Call());
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandSender0int : public IPCommandSender<int()> {
public:

	MockIPCommandSender0int() : IPCommandSender(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD0(ReadResponse, int());

	virtual void WriteCommandId(int /*id*/) {
	}
	virtual int ReadResult() {
		return 0;
	}
	virtual int GetCommandId() const {
		return 99;
	}
};

} // namespace

TEST(MockIPCommandSender0int, Call)
{
	MockIPCommandSender0int mics;
	int ret = 999;

	EXPECT_CALL(mics, ReadResponse())
		.WillOnce(Return(ret));

	EXPECT_EQ(ret, mics.Call());
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandSender1voidInt : public IPCommandSender<void(int)> {
public:

	MockIPCommandSender1voidInt() : IPCommandSender(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD1(WriteArguments, void(int arg1));

	virtual void WriteCommandId(int /*id*/) {
	}
	virtual int ReadResult() {
		return 0;
	}
	virtual int GetCommandId() const {
		return 99;
	}
};

} // namespace

TEST(MockIPCommandSender1voidInt, Call)
{
	MockIPCommandSender1voidInt mics;
	int arg = 999;

	EXPECT_CALL(mics, WriteArguments(arg))
		.Times(1);

	EXPECT_NO_THROW(mics.Call(arg));
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandSender1voidCString : public IPCommandSender<void(const CString&)> {
public:

	MockIPCommandSender1voidCString() : IPCommandSender(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD1(WriteArguments, void(const CString& arg1));

	virtual void WriteCommandId(int /*id*/) {
	}
	virtual int ReadResult() {
		return 0;
	}
	virtual int GetCommandId() const {
		return 99;
	}
};

} // namespace

TEST(MockIPCommandSender1voidCString, Call)
{
	MockIPCommandSender1voidCString mics;
	CString arg("some str");

	EXPECT_CALL(mics, WriteArguments(arg))
		.Times(1);

	EXPECT_NO_THROW(mics.Call(arg));
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandSender1intInt : public IPCommandSender<int(int)> {
public:

	MockIPCommandSender1intInt() : IPCommandSender(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD1(WriteArguments, void(int arg1));
	MOCK_METHOD0(ReadResponse, int());

	virtual void WriteCommandId(int /*id*/) {
	}
	virtual int ReadResult() {
		return 0;
	}
	virtual int GetCommandId() const {
		return 99;
	}
};

} // namespace

TEST(MockIPCommandSender1intInt, Call)
{
	MockIPCommandSender1intInt mics;
	int arg = 999;
	int ret = 9;

	EXPECT_CALL(mics, WriteArguments(arg))
		.Times(1);
	EXPECT_CALL(mics, ReadResponse())
		.WillOnce(Return(ret));

	EXPECT_EQ(ret, mics.Call(arg));
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandSender1intCString : public IPCommandSender<int(const CString&)> {
public:

	MockIPCommandSender1intCString() : IPCommandSender(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD1(WriteArguments, void(const CString& arg1));
	MOCK_METHOD0(ReadResponse, int());

	virtual void WriteCommandId(int /*id*/) {
	}
	virtual int ReadResult() {
		return 0;
	}
	virtual int GetCommandId() const {
		return 99;
	}
};

} // namespace

TEST(MockIPCommandSender1intCString, Call)
{
	MockIPCommandSender1intCString mics;
	CString arg("some str");
	int ret = 9;

	EXPECT_CALL(mics, WriteArguments(arg))
		.Times(1);
	EXPECT_CALL(mics, ReadResponse())
		.WillOnce(Return(ret));

	EXPECT_EQ(ret, mics.Call(arg));
}
