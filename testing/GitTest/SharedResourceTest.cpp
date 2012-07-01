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

#include "SharedResource.h"

using namespace ::testing;

namespace {

template <typename T>
class MockCloser {
public:

	MOCK_CONST_METHOD1_T(DoClose, void(T value));
};

template <typename T>
class MockWrapper : public std::unary_function<T, void> {
public:

	MockWrapper(const MockCloser<T>& mock)
		: m_mock(mock)
	{}
	void operator()(T value) const
	{
		m_mock.DoClose(value);
	}

private:
	const MockCloser<T>& m_mock;
};

} // namespace

template <typename T>
static MockWrapper<T> wrap(const MockCloser<T>& mock)
{
	return MockWrapper<T>(mock);
}

TEST(SharedResourceTest, NormalHandle)
{
	MockCloser<HANDLE> closer;
	EXPECT_CALL(closer, DoClose(_))
		.Times(1);

	shared_resource<HANDLE> ptr(reinterpret_cast<HANDLE>(0x1234), wrap(closer));
	EXPECT_TRUE(ptr);
	EXPECT_FALSE(!ptr);
}

TEST(SharedResourceTest, NullHandle)
{
	MockCloser<HANDLE> closer;
	EXPECT_CALL(closer, DoClose(_))
		.Times(0);

	shared_resource<HANDLE> ptr(reinterpret_cast<HANDLE>(0), wrap(closer));
	EXPECT_FALSE(ptr);
	EXPECT_TRUE(!ptr);
}

TEST(SharedResourceTest, InvalidHandleValue)
{
	MockCloser<HANDLE> closer;
	EXPECT_CALL(closer, DoClose(_))
		.Times(0);

	shared_resource<HANDLE> ptr(reinterpret_cast<HANDLE>(INVALID_HANDLE_VALUE), wrap(closer));
	EXPECT_FALSE(ptr);
	EXPECT_TRUE(!ptr);
}

static void DummyCloser(HANDLE) {}

class SharedResourceTest2 : public ::testing::Test {
protected:

	SharedResourceTest2()
		: ptr1(reinterpret_cast<HANDLE>(0x1234), DummyCloser),
		  ptr2(reinterpret_cast<HANDLE>(0x4321), DummyCloser)
	{}

	typedef shared_resource<HANDLE> PtrType;
	PtrType ptr1;
	PtrType ptr2;
};


TEST_F(SharedResourceTest2, Comparisons)
{
	EXPECT_FALSE(ptr1 == ptr2);
	EXPECT_TRUE(ptr1 != ptr2);
}
