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

#include "Utilities.h"

TEST(UtilitiesTest, StartsWith)
{
	EXPECT_TRUE(StartsWith(CString(""), CString("")));
	EXPECT_FALSE(StartsWith(CString(""), CString("bbb")));
	EXPECT_TRUE(StartsWith(CString("aaa"), CString("")));

	EXPECT_TRUE(StartsWith(CString("aaabbb"), CString("a")));
	EXPECT_FALSE(StartsWith(CString("baaab"), CString("a")));
}

TEST(UtilitiesTest, EndsWith)
{
	EXPECT_TRUE(EndsWith(CString(""), CString("")));
	EXPECT_FALSE(EndsWith(CString(""), CString("bbb")));
	EXPECT_TRUE(EndsWith(CString("aaa"), CString("")));

	EXPECT_TRUE(EndsWith(CString("aaa"), CString("a")));
	EXPECT_FALSE(EndsWith(CString("aaab"), CString("a")));
}
