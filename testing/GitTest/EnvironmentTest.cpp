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

#include "Environment.h"

namespace {

class MockEnvironment : public Environment {
public:

	MOCK_CONST_METHOD0(GetEnvironmentStrings, shared_ptr<TCHAR>());

};

void DummyDeletor(TCHAR*)
{
	// do nothing
}

std::vector<TCHAR> ptr2vec(TCHAR* p, size_t len)
{
	return std::vector<TCHAR>(p, p+len);
}

} // namespace

using namespace ::testing;

TEST(EnvironmentTest, GetPointer)
{
	MockEnvironment menv;

	EXPECT_EQ(NULL, menv.GetPointer());
}

TEST(EnvironmentTest, CopyProcessEnvironment)
{
	TCHAR env[] = _T("=\0aaa=bbb\0cccc=dddd\0");

	shared_ptr<TCHAR> ret(env, DummyDeletor);
	size_t len = sizeof(env)/sizeof(TCHAR);

	MockEnvironment menv;
	EXPECT_CALL(menv, GetEnvironmentStrings())
		.WillOnce(Return(ret));

	menv.CopyProcessEnvironment();

	ASSERT_NE((LPTSTR)NULL, menv.GetPointer());
	EXPECT_EQ(ptr2vec(env, len), ptr2vec(menv.GetPointer(), len));

	EXPECT_CALL(menv, GetEnvironmentStrings())
		.WillOnce(Return(ret));

	menv.CopyProcessEnvironment();

	ASSERT_NE((LPTSTR)NULL, menv.GetPointer());
	EXPECT_EQ(ptr2vec(env, len), ptr2vec(menv.GetPointer(), len));
}


TEST(EnvironmentTest, SetEnvToEmptyEnvironment)
{
	MockEnvironment menv;

	EXPECT_EQ(NULL, menv.GetPointer());

	menv.SetEnv(_T("aaa"), _T("bbb"));

	{
		TCHAR expected[] = _T("aaa=bbb\0");
		size_t len = sizeof(expected)/sizeof(TCHAR);

		ASSERT_NE((LPTSTR)NULL, menv.GetPointer());
		EXPECT_EQ(ptr2vec(expected, len), ptr2vec(menv.GetPointer(), len));
	}
}

TEST(EnvironmentTest, SetEnv)
{
	MockEnvironment menv;

	EXPECT_EQ(NULL, menv.GetPointer());

	menv.SetEnv(_T("aaa"), _T("bbb"));
	menv.SetEnv(_T("ccc"), _T("ddd"));
	menv.SetEnv(_T(""), _T("xxx"));
	menv.SetEnv(_T("yyy"), _T(""));

	{
		TCHAR expected[] = _T("aaa=bbb\0ccc=ddd\0=xxx\0yyy=\0");
		size_t len = sizeof(expected)/sizeof(TCHAR);

		ASSERT_NE((LPTSTR)NULL, menv.GetPointer());
		EXPECT_EQ(ptr2vec(expected, len), ptr2vec(menv.GetPointer(), len));
	}
}

TEST(EnvironmentTest, ReplaceFirstEnv)
{
	MockEnvironment menv;

	EXPECT_EQ(NULL, menv.GetPointer());

	menv.SetEnv(_T("aaa"), _T("bbb"));
	menv.SetEnv(_T("ccc"), _T("ddd"));
	menv.SetEnv(_T("eee"), _T("fff"));

	menv.SetEnv(_T("AAA"), _T("XXX"));

	{
		TCHAR expected[] = _T("AAA=XXX\0ccc=ddd\0eee=fff\0");
		size_t len = sizeof(expected)/sizeof(TCHAR);

		ASSERT_NE((LPTSTR)NULL, menv.GetPointer());
		EXPECT_EQ(ptr2vec(expected, len), ptr2vec(menv.GetPointer(), len));
	}
}

TEST(EnvironmentTest, ReplaceMiddleEnv)
{
	MockEnvironment menv;

	EXPECT_EQ(NULL, menv.GetPointer());

	menv.SetEnv(_T("aaa"), _T("bbb"));
	menv.SetEnv(_T("ccc"), _T("ddd"));
	menv.SetEnv(_T("eee"), _T("fff"));

	menv.SetEnv(_T("CCC"), _T("XXX"));

	{
		TCHAR expected[] = _T("aaa=bbb\0CCC=XXX\0eee=fff\0");
		size_t len = sizeof(expected)/sizeof(TCHAR);

		ASSERT_NE((LPTSTR)NULL, menv.GetPointer());
		EXPECT_EQ(ptr2vec(expected, len), ptr2vec(menv.GetPointer(), len));
	}
}

TEST(EnvironmentTest, ReplaceLastEnv)
{
	MockEnvironment menv;

	EXPECT_EQ(NULL, menv.GetPointer());

	menv.SetEnv(_T("aaa"), _T("bbb"));
	menv.SetEnv(_T("ccc"), _T("ddd"));
	menv.SetEnv(_T("eee"), _T("fff"));

	menv.SetEnv(_T("EEE"), _T("XXX"));

	{
		TCHAR expected[] = _T("aaa=bbb\0ccc=ddd\0EEE=XXX\0");
		size_t len = sizeof(expected)/sizeof(TCHAR);

		ASSERT_NE((LPTSTR)NULL, menv.GetPointer());
		EXPECT_EQ(ptr2vec(expected, len), ptr2vec(menv.GetPointer(), len));
	}
}

TEST(EnvironmentTest, GetEnvFromEmptyEnvironment)
{
	MockEnvironment menv;

	EXPECT_EQ(CString(), menv.GetEnv(_T("XXX")));
}

TEST(EnvironmentTest, GetEnv)
{
	MockEnvironment menv;

	menv.SetEnv(_T("aaa"), _T("bbb"));
	menv.SetEnv(_T("ccc"), _T("ddd"));
	menv.SetEnv(_T("eee"), _T("fff"));
	menv.SetEnv(_T(""), _T("xxx"));
	menv.SetEnv(_T("yyy"), _T(""));

	EXPECT_EQ(CString("bbb"), menv.GetEnv(_T("aaa")));
	EXPECT_EQ(CString("ddd"), menv.GetEnv(_T("ccc")));
	EXPECT_EQ(CString("fff"), menv.GetEnv(_T("eee")));
	EXPECT_EQ(CString("xxx"), menv.GetEnv(_T("")));
	EXPECT_EQ(CString(""), menv.GetEnv(_T("yyy")));

	EXPECT_EQ(CString(), menv.GetEnv(_T("XXX")));
}
