// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2012 - TortoiseGit

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

#include "CommandLineParser.h"

#include "CommandLineArguments.h"

using namespace ::testing;
using namespace ::internal;

static void ReadSeparatorTestHelper(const char* cmdLine, bool expectRet, int expectPos, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	int pos = 0;
	EXPECT_EQ(expectRet, ReadSeparator(CString(cmdLine), pos));
	EXPECT_EQ(expectPos, pos);
}

TEST(CommandLineParser, ReadSeparator)
{
	ReadSeparatorTestHelper("", false, 0, __LINE__);
	ReadSeparatorTestHelper("   ", false, 3, __LINE__);
	ReadSeparatorTestHelper(" a  ", true, 1, __LINE__);
	ReadSeparatorTestHelper("a   ", true, 0, __LINE__);
	ReadSeparatorTestHelper("   a", true, 3, __LINE__);
}

static void ReadCommandSwitchTestHelper(const char* cmdLine, bool expectRet, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	int pos = 0;
	EXPECT_EQ(expectRet, ReadCommandSwitch(CString(cmdLine), pos));
	if(expectRet) {
		EXPECT_EQ(1, pos);
	} else {
		EXPECT_EQ(0, pos);
	}
}

TEST(CommandLineParser, ReadCommandSwitch)
{
	ReadCommandSwitchTestHelper("", false, __LINE__);

	ReadCommandSwitchTestHelper("/", true, __LINE__);
	ReadCommandSwitchTestHelper("-", true, __LINE__);
	ReadCommandSwitchTestHelper(":", false, __LINE__);
	ReadCommandSwitchTestHelper("a", false, __LINE__);
}

static void ReadKeyTestHelper(const char* cmdLine, const char* expectedKey, int expectPos, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	int pos = 0;
	EXPECT_EQ(CString(expectedKey), ReadKey(CString(cmdLine), pos));
	EXPECT_EQ(expectPos, pos);
}

TEST(CommandLineParser, ReadKey)
{
	ReadKeyTestHelper("", "", 0, __LINE__);

	ReadKeyTestHelper("aa00", "aa00", 4, __LINE__);
	ReadKeyTestHelper("aa:bb", "aa", 2, __LINE__);
}

static void ReadValueSeparatorTestHelper(const char* cmdLine, bool expectRet, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	int pos = 0;
	EXPECT_EQ(expectRet, ReadValueSeparator(CString(cmdLine), pos));
	if(expectRet) {
		EXPECT_EQ(1, pos);
	} else {
		EXPECT_EQ(0, pos);
	}
}

TEST(CommandLineParser, ReadValueSeparator)
{
	ReadValueSeparatorTestHelper("", false, __LINE__);

	ReadValueSeparatorTestHelper("/", false, __LINE__);
	ReadValueSeparatorTestHelper("-", false, __LINE__);
	ReadValueSeparatorTestHelper(":", true, __LINE__);
	ReadValueSeparatorTestHelper("a", false, __LINE__);
}

static void ReadValueTestHelper(const char* cmdLine, const char* expectedValue, int expectPos, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	int pos = 0;
	EXPECT_EQ(CString(expectedValue), ReadValue(CString(cmdLine), pos));
	EXPECT_EQ(expectPos, pos);
}

TEST(CommandLineParser, ReadValue)
{
	ReadValueTestHelper("", "", 0, __LINE__);

	ReadValueTestHelper("aa00", "aa00", 4, __LINE__);
	ReadValueTestHelper("aa:bb ccdd", "aa:bb", 5, __LINE__);
	ReadValueTestHelper("\"aa:bb ccdd\"", "aa:bb ccdd", 12, __LINE__);
	ReadValueTestHelper("\"aa:bb ccdd", "aa:bb ccdd", 11, __LINE__);
	ReadValueTestHelper("aa:\"bb cc\"dd", "aa:\"bb", 6, __LINE__);
}

static void ReadOptionTestHelper(const char* cmdLine, bool expectedRet, const char* expectedKey, const char* expectedValue, int expectPos, int line)
{
	SCOPED_TRACE(Message()  << "called from " __FILE__ "(" << line << ")");

	int pos = 0;
	CString key;
	CString value;
	EXPECT_EQ(expectedRet, ReadOption(CString(cmdLine), pos, key, value));
	EXPECT_EQ(expectPos, pos);
	EXPECT_EQ(CString(expectedKey), key);
	EXPECT_EQ(CString(expectedValue), value);
}

TEST(CommandLineParser, ReadOption)
{
	ReadOptionTestHelper("", false, "", "", 0, __LINE__);
	ReadOptionTestHelper("key", false, "", "", 0, __LINE__);

	ReadOptionTestHelper("/key", true, "key", "", 4, __LINE__);
	ReadOptionTestHelper("/key  ", true, "key", "", 6, __LINE__);
	ReadOptionTestHelper("/key aa ", true, "key", "", 5, __LINE__);
	
	ReadOptionTestHelper("-key", true, "key", "", 4, __LINE__);
	ReadOptionTestHelper("-key  ", true, "key", "", 6, __LINE__);
	ReadOptionTestHelper("-key aa ", true, "key", "", 5, __LINE__);

	ReadOptionTestHelper("/key:value", true, "key", "value", 10, __LINE__);
	ReadOptionTestHelper("/key:value  ", true, "key", "value", 12, __LINE__);
	ReadOptionTestHelper("/key:value aa ", true, "key", "value", 11, __LINE__);
	ReadOptionTestHelper("/key:\"value aa\" ", true, "key", "value aa", 16, __LINE__);
	ReadOptionTestHelper("/key:\"value aa ", true, "key", "value aa ", 15, __LINE__);

	ReadOptionTestHelper("-key:value", true, "key", "value", 10, __LINE__);
	ReadOptionTestHelper("-key:value  ", true, "key", "value", 12, __LINE__);
	ReadOptionTestHelper("-key:value aa ", true, "key", "value", 11, __LINE__);
	ReadOptionTestHelper("-key:\"value aa\" ", true, "key", "value aa", 16, __LINE__);
	ReadOptionTestHelper("-key:\"value aa ", true, "key", "value aa ", 15, __LINE__);
}

TEST(CommandLineParser, ParseCommandLineWithEmptyString)
{
	CommandLineArguments args = ParseCommandLine(CString());
	EXPECT_EQ(0, args.GetCount());
}

TEST(CommandLineParser, ParseCommandLine)
{
	CommandLineArguments args = ParseCommandLine(CString("  /key1 /key2:value2 -key3:\"value3 value3  \""));
	EXPECT_EQ(3, args.GetCount());

	EXPECT_TRUE(args.HasKey(CString("KEY1")));
	EXPECT_EQ(CString(), args.GetAsString(CString("KEY1")));

	EXPECT_TRUE(args.HasKey(CString("KEY2")));
	EXPECT_EQ(CString("value2"), args.GetAsString(CString("KEY2")));

	EXPECT_TRUE(args.HasKey(CString("KEY3")));
	EXPECT_EQ(CString("value3 value3  "), args.GetAsString(CString("KEY3")));
}

TEST(CommandLineParser, ParseCommandLineWithInvalieString)
{
	CommandLineArguments args = ParseCommandLine(CString("  /key1 /key2:value2 key3:\"value3 value3  \""));
	EXPECT_EQ(2, args.GetCount());	// give up parsing when fails to parse

	EXPECT_TRUE(args.HasKey(CString("KEY1")));
	EXPECT_EQ(CString(), args.GetAsString(CString("KEY1")));

	EXPECT_TRUE(args.HasKey(CString("KEY2")));
	EXPECT_EQ(CString("value2"), args.GetAsString(CString("KEY2")));

	EXPECT_FALSE(args.HasKey(CString("KEY3")));
	EXPECT_EQ(CString(), args.GetAsString(CString("KEY3")));
}
