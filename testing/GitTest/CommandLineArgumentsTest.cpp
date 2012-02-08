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

#include "CommandLineArguments.h"


TEST(CommandLineArguments, GetCount)
{
	CommandLineArguments parser;
	EXPECT_EQ(0, parser.GetCount());

	parser.Add(CString("key"), CString("value"));
	EXPECT_EQ(1, parser.GetCount());

	parser.Add(CString("KEY"), CString("value")); // case-insensitive
	EXPECT_EQ(1, parser.GetCount());
}

TEST(CommandLineArguments, HasKey)
{
	CommandLineArguments parser;
	EXPECT_FALSE(parser.HasKey(CString("key")));

	parser.Add(CString("key"), CString("value"));
	EXPECT_TRUE(parser.HasKey(CString("key")));
	EXPECT_TRUE(parser.HasKey(CString("KEY")));		// case-insensitive
}

TEST(CommandLineArguments, GetAsString)
{
	CommandLineArguments parser;
	EXPECT_EQ(CString(), parser.GetAsString(CString("key")));

	parser.Add(CString("key"), CString("value1"));
	EXPECT_EQ(CString("value1"), parser.GetAsString(CString("key")));
	EXPECT_EQ(CString("value1"), parser.GetAsString(CString("KEY")));	// case-insensitive

	parser.Add(CString("KEY"), CString("value2"));
	EXPECT_EQ(CString("value1"), parser.GetAsString(CString("key")));	// can't override value

	EXPECT_EQ(CString(), parser.GetAsString(CString("non_existent_key")));	// can't override value
}
