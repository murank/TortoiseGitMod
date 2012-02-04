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

#include "IPCommandReceiver.h"

using namespace ::testing;

namespace {

class MockIPCommandReceiver0void : public IPCommandReceiver<void()> {
public:

	MockIPCommandReceiver0void() : IPCommandReceiver(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD0(DoCommand, void());

	MOCK_METHOD0(WriteResultSuccess, void());
	MOCK_METHOD0(WriteResultFailure, void());
};

} // namesapce

TEST(IPCommandReceiver0void, Run)
{
	MockIPCommandReceiver0void micr;

	EXPECT_CALL(micr, DoCommand())
		.Times(1);
	EXPECT_CALL(micr, WriteResultSuccess())
		.Times(1);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiver0void, RunWithException)
{
	MockIPCommandReceiver0void micr;

	EXPECT_CALL(micr, DoCommand())
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);

	EXPECT_THROW(micr.Run(), std::exception);
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandReceiver0int : public IPCommandReceiver<int()> {
public:

	MockIPCommandReceiver0int() : IPCommandReceiver(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD0(DoCommand, int());
	MOCK_METHOD1(WriteResult, void(int result));

	MOCK_METHOD0(WriteResultSuccess, void());
	MOCK_METHOD0(WriteResultFailure, void());
};

} // namesapce

TEST(IPCommandReceiver0int, Run)
{
	MockIPCommandReceiver0int micr;

	int value = 99;

	EXPECT_CALL(micr, DoCommand())
		.WillOnce(Return(value));
	EXPECT_CALL(micr, WriteResult(value))
		.Times(1);
	EXPECT_CALL(micr, WriteResultSuccess())
		.Times(1);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiver0int, RunWithException)
{
	MockIPCommandReceiver0int micr;

	EXPECT_CALL(micr, DoCommand())
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, WriteResult(_))
		.Times(0);
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);

	EXPECT_THROW(micr.Run(), std::exception);
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandReceiver1voidInt : public IPCommandReceiver<void(int)> {
public:

	MockIPCommandReceiver1voidInt() : IPCommandReceiver(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD1(DoCommand, void(int arg1));
	MOCK_METHOD1(ReadArguments, void(int& arg1));

	MOCK_METHOD0(WriteResultSuccess, void());
	MOCK_METHOD0(WriteResultFailure, void());
};

} // namesapce

TEST(IPCommandReceiver1voidInt, Run)
{
	MockIPCommandReceiver1voidInt micr;

	int value = 99;

	EXPECT_CALL(micr, DoCommand(value))
		.Times(1);
	EXPECT_CALL(micr, ReadArguments(_))
		.WillOnce(SetArgReferee<0>(value));
	EXPECT_CALL(micr, WriteResultSuccess())
		.Times(1);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiver1voidInt, RunWithException)
{
	MockIPCommandReceiver1voidInt micr;

	int value = 99;

	EXPECT_CALL(micr, DoCommand(value))
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, ReadArguments(_))
		.WillOnce(SetArgReferee<0>(value));
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);

	EXPECT_THROW(micr.Run(), std::exception);
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandReceiver1voidCString : public IPCommandReceiver<void(const CString&)> {
public:

	MockIPCommandReceiver1voidCString() : IPCommandReceiver(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD1(DoCommand, void(const CString& arg1));
	MOCK_METHOD1(ReadArguments, void(CString& arg1));

	MOCK_METHOD0(WriteResultSuccess, void());
	MOCK_METHOD0(WriteResultFailure, void());
};

} // namesapce

TEST(IPCommandReceiver1voidCString, Run)
{
	MockIPCommandReceiver1voidCString micr;

	CString value("some str");

	EXPECT_CALL(micr, DoCommand(value))
		.Times(1);
	EXPECT_CALL(micr, ReadArguments(_))
		.WillOnce(SetArgReferee<0>(value));
	EXPECT_CALL(micr, WriteResultSuccess())
		.Times(1);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiver1voidCString, RunWithException)
{
	MockIPCommandReceiver1voidCString micr;

	CString value("some str");

	EXPECT_CALL(micr, DoCommand(value))
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, ReadArguments(_))
		.WillOnce(SetArgReferee<0>(value));
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);

	EXPECT_THROW(micr.Run(), std::exception);
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandReceiver1intInt : public IPCommandReceiver<int(int)> {
public:

	MockIPCommandReceiver1intInt() : IPCommandReceiver(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD1(DoCommand, int(int arg1));
	MOCK_METHOD1(ReadArguments, void(int& arg1));
	MOCK_METHOD1(WriteResult, void(int result));

	MOCK_METHOD0(WriteResultSuccess, void());
	MOCK_METHOD0(WriteResultFailure, void());
};

} // namesapce

TEST(IPCommandReceiver1intInt, Run)
{
	MockIPCommandReceiver1intInt micr;

	int arg1 = 99;
	int result = 999;

	EXPECT_CALL(micr, DoCommand(arg1))
		.WillOnce(Return(result));
	EXPECT_CALL(micr, ReadArguments(_))
		.WillOnce(SetArgReferee<0>(arg1));
	EXPECT_CALL(micr, WriteResult(result))
		.Times(1);
	EXPECT_CALL(micr, WriteResultSuccess())
		.Times(1);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiver1intInt, RunWithException)
{
	MockIPCommandReceiver1intInt micr;

	int arg1 = 99;

	EXPECT_CALL(micr, DoCommand(arg1))
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, ReadArguments(_))
		.WillOnce(SetArgReferee<0>(arg1));
	EXPECT_CALL(micr, WriteResult(_))
		.Times(0);
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);

	EXPECT_THROW(micr.Run(), std::exception);
}

/////////////////////////////////////////////////////////////////////////////

namespace {

class MockIPCommandReceiver1intCString : public IPCommandReceiver<int(const CString&)> {
public:

	MockIPCommandReceiver1intCString() : IPCommandReceiver(shared_ptr<InterprocessIo>()) {}

	MOCK_METHOD1(DoCommand, int(const CString& arg1));
	MOCK_METHOD1(ReadArguments, void(CString& arg1));
	MOCK_METHOD1(WriteResult, void(int result));

	MOCK_METHOD0(WriteResultSuccess, void());
	MOCK_METHOD0(WriteResultFailure, void());
};

} // namesapce

TEST(IPCommandReceiver1intCString, Run)
{
	MockIPCommandReceiver1intCString micr;

	CString arg1("some str");
	int result = 999;

	EXPECT_CALL(micr, DoCommand(arg1))
		.WillOnce(Return(result));
	EXPECT_CALL(micr, ReadArguments(_))
		.WillOnce(SetArgReferee<0>(arg1));
	EXPECT_CALL(micr, WriteResult(result))
		.Times(1);
	EXPECT_CALL(micr, WriteResultSuccess())
		.Times(1);

	EXPECT_NO_THROW(micr.Run());
}

TEST(IPCommandReceiver1intCString, RunWithException)
{
	MockIPCommandReceiver1intCString micr;

	CString arg1("some str");

	EXPECT_CALL(micr, DoCommand(arg1))
		.WillOnce(Throw(std::exception()));
	EXPECT_CALL(micr, ReadArguments(_))
		.WillOnce(SetArgReferee<0>(arg1));
	EXPECT_CALL(micr, WriteResult(_))
		.Times(0);
	EXPECT_CALL(micr, WriteResultFailure())
		.Times(1);

	EXPECT_THROW(micr.Run(), std::exception);
}
