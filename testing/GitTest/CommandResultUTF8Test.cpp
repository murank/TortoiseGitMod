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

#include "CommandResultUTF8.h"

using namespace ::testing;

namespace {

class MockCommandResult : public CommandResult {
public:

	MOCK_METHOD2(Read, size_t(char* buf, size_t len));
	MOCK_METHOD0(Join, bool());
	MOCK_CONST_METHOD0(GetExitCode, int());
};

class MockCommandResultHelper {
public:
	std::deque<char> data;

	MockCommandResultHelper(std::string d) : data(d.begin(), d.end())
	{	
	}

	size_t Read(char* buf, size_t len) {
		size_t size_write = std::min(data.size(), len);

		if(size_write == 0) {
			return 0;
		}

		std::copy(data.begin(), data.begin()+size_write, buf);
		data.erase(data.begin(), data.begin()+size_write);

		return size_write;
	}

};

} // namespace

TEST(CommandResultUTF8Test, Read)
{
	shared_ptr<MockCommandResult> mr(new MockCommandResult);
	const char* utf8_str = "aaa\xE3\x81\x82\xE3\x81\x82\xE3\x81\x82";
	MockCommandResultHelper helper(utf8_str);

	EXPECT_CALL(*mr, Read(_, _))
		.WillRepeatedly(Invoke(&helper, &MockCommandResultHelper::Read));

	std::string acp_str(CW2A(CA2W(utf8_str, CP_UTF8), CP_ACP));

	CommandResultUTF8 cru(mr);

	std::vector<char> buf(1024);
	char* p = &(buf[0]);

	size_t offset = 0;
	size_t size_read = cru.Read(p, 4);
	EXPECT_EQ(4, size_read);
	EXPECT_EQ(acp_str.substr(offset, size_read), std::string(p, p+size_read));
	offset += size_read;

	size_read = cru.Read(p, 1);
	EXPECT_EQ(1, size_read);
	EXPECT_EQ(acp_str.substr(offset, size_read), std::string(p, p+size_read));
	offset += size_read;

	size_read = cru.Read(p, 10);
	EXPECT_EQ(4, size_read);
	EXPECT_EQ(acp_str.substr(offset, size_read), std::string(p, p+size_read));
}

TEST(CommandResultUTF8Test, ReadWithInvalidMultiBytes)
{
	shared_ptr<MockCommandResult> mr(new MockCommandResult);
	const char* utf8_str = "aaa\xE3\x81\x82\xE3\x81\x82\xE3\x81"; // "\xE3\x81" is invalid UTF8 chars
	MockCommandResultHelper helper(utf8_str);

	EXPECT_CALL(*mr, Read(_, _))
		.WillRepeatedly(Invoke(&helper, &MockCommandResultHelper::Read));

	std::string acp_str(CW2A(CA2W(utf8_str, CP_UTF8), CP_ACP));

	CommandResultUTF8 cru(mr);

	std::vector<char> buf(1024);
	char* p = &(buf[0]);

	size_t offset = 0;
	size_t size_read = cru.Read(p, 15);
	EXPECT_EQ(7, size_read);
	EXPECT_EQ(acp_str.substr(offset, size_read), std::string(p, p+size_read));
	offset += size_read;

	EXPECT_EQ(0, cru.Read(p, 15));
	EXPECT_EQ(0, cru.Read(p, 15));
}
