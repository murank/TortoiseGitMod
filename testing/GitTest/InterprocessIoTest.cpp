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

#include "InterprocessIo.h"

#include "PipeError.h"

namespace {

class MockInterprocessIo : public InterprocessIo {
public:

	MockInterprocessIo() : InterprocessIo(shared_handle()) {}

	MOCK_METHOD3(ReadPipe, BOOL(char* buf, DWORD bytesToRead, LPDWORD bytesRead));
	MOCK_METHOD3(WritePipe, BOOL(const char* buf, DWORD bytesToWrite, LPDWORD bytesWritten));

};

} // namespace

using namespace ::testing;

TEST(InterprocessIo, ReadInt)
{
	MockInterprocessIo mii;

	int value = 99;
	char* p = reinterpret_cast<char*>(&value);

	EXPECT_CALL(mii, ReadPipe(NotNull(), sizeof(int), NotNull()))
		.WillOnce(DoAll(SetArrayArgument<0>(p, p+sizeof(int)), SetArgPointee<2>(sizeof(int)), Return(TRUE)));

	EXPECT_EQ(value, mii.Read<int>());
}

TEST(InterprocessIo, ReadIntWithFailureThatReturnsFalse)
{
	MockInterprocessIo mii;

	EXPECT_CALL(mii, ReadPipe(NotNull(), sizeof(int), NotNull()))
		.WillOnce(Return(FALSE));

	EXPECT_THROW(mii.Read<int>(), pipe_error);
}

TEST(InterprocessIo, ReadIntWithFailureThatRead0Bytes)
{
	MockInterprocessIo mii;

	EXPECT_CALL(mii, ReadPipe(NotNull(), sizeof(int), NotNull()))
		.WillOnce(DoAll(SetArgPointee<2>(0), Return(TRUE)));

	EXPECT_THROW(mii.Read<int>(), pipe_error);
}

TEST(InterprocessIo, PartialRead)
{
	MockInterprocessIo mii;

	int value = 99;
	char* p = reinterpret_cast<char*>(&value);

	Sequence s;

	for(int i=0; i<sizeof(int); ++i) {
		EXPECT_CALL(mii, ReadPipe(NotNull(), sizeof(int)-i, NotNull()))
			.InSequence(s)
			.WillOnce(DoAll(SetArrayArgument<0>(p+i, p+i+1), SetArgPointee<2>(1), Return(TRUE)));
	}

	EXPECT_EQ(value, mii.Read<int>());
}

TEST(InterprocessIo, PartialReadWithFailure)
{
	MockInterprocessIo mii;

	int value = 99;
	char* p = reinterpret_cast<char*>(&value);

	Sequence s;

	for(int i=0; i<sizeof(int)/2; ++i) {
		EXPECT_CALL(mii, ReadPipe(NotNull(), sizeof(int)-i, NotNull()))
			.InSequence(s)
			.WillOnce(DoAll(SetArrayArgument<0>(p+i, p+i+1), SetArgPointee<2>(1), Return(TRUE)));
	}

	EXPECT_CALL(mii, ReadPipe(NotNull(), _, NotNull()))
		.InSequence(s)
		.WillOnce(Return(FALSE));

	EXPECT_THROW(mii.Read<int>(), pipe_error);
}

TEST(InterprocessIo, ReadCString)
{
	MockInterprocessIo mii;

	CString value(_T("some string"));
	int len = value.GetLength();
	char* p1 = reinterpret_cast<char*>(&len);
	const char* p2 = reinterpret_cast<const char*>((LPCTSTR)value);

	EXPECT_CALL(mii, ReadPipe(NotNull(), sizeof(int), NotNull()))
		.WillOnce(DoAll(SetArrayArgument<0>(p1, p1+sizeof(int)), SetArgPointee<2>(sizeof(int)), Return(TRUE)));

	int clen = len*sizeof(TCHAR);
	EXPECT_CALL(mii, ReadPipe(NotNull(), clen, NotNull()))
		.WillOnce(DoAll(SetArrayArgument<0>(p2, p2+clen), SetArgPointee<2>(clen), Return(TRUE)));

	EXPECT_EQ(value, mii.Read<CString>());
}

namespace {

class InterprocessIo_P : public ::testing::TestWithParam<int> {
};

} // namepsace

TEST_P(InterprocessIo_P, ReadCStringWithInvalidLength)
{
	MockInterprocessIo mii;

	CString value(_T("some string"));
	int invalid_len = GetParam();
	char* p1 = reinterpret_cast<char*>(&invalid_len);

	EXPECT_CALL(mii, ReadPipe(NotNull(), sizeof(int), NotNull()))
		.WillOnce(DoAll(SetArrayArgument<0>(p1, p1+sizeof(int)), SetArgPointee<2>(sizeof(int)), Return(TRUE)));

	EXPECT_EQ(CString(), mii.Read<CString>());
}

INSTANTIATE_TEST_CASE_P(Inst,
                        InterprocessIo_P,
                        Values(0, -1, -2));

TEST(InterprocessIo, ReadCStringWithFailureOnReadingStringLength)
{
	MockInterprocessIo mii;

	CString value(_T("some string"));
	int len = value.GetLength();
	char* p1 = reinterpret_cast<char*>(&len);
	const char* p2 = reinterpret_cast<const char*>((LPCTSTR)value);

	EXPECT_CALL(mii, ReadPipe(NotNull(), sizeof(int), NotNull()))
		.WillOnce(Return(FALSE));

	EXPECT_THROW(mii.Read<CString>(), pipe_error);
}

TEST(InterprocessIo, ReadCStringWithFailureOnReadingStringBytes)
{
	MockInterprocessIo mii;

	CString value(_T("some string"));
	int len = value.GetLength();
	char* p1 = reinterpret_cast<char*>(&len);

	EXPECT_CALL(mii, ReadPipe(NotNull(), sizeof(int), NotNull()))
		.WillOnce(DoAll(SetArrayArgument<0>(p1, p1+sizeof(int)), SetArgPointee<2>(sizeof(int)), Return(TRUE)));

	int clen = len*sizeof(TCHAR);
	EXPECT_CALL(mii, ReadPipe(NotNull(), clen, NotNull()))
		.WillOnce(Return(FALSE));

	EXPECT_THROW(mii.Read<CString>(), pipe_error);
}

TEST(InterprocessIo, WriteInt)
{
	MockInterprocessIo mii;

	int value = 0x11223344;
	char* p = reinterpret_cast<char*>(&value);

	EXPECT_CALL(mii, WritePipe(NotNull(), sizeof(int), NotNull()))
		.With(Args<0, 1>(ElementsAreArray(p, sizeof(int))))
		.WillOnce(DoAll(SetArgPointee<2>(sizeof(int)), Return(TRUE)));

	EXPECT_NO_THROW(mii.Write(value));
}

TEST(InterprocessIo, WriteIntWithFailureThatReturnsFalse)
{
	MockInterprocessIo mii;

	int value = 0x11223344;
	char* p = reinterpret_cast<char*>(&value);

	EXPECT_CALL(mii, WritePipe(NotNull(), sizeof(int), NotNull()))
		.With(Args<0, 1>(ElementsAreArray(p, sizeof(int))))
		.WillOnce(Return(FALSE));

	EXPECT_THROW(mii.Write(value), pipe_error);
}

TEST(InterprocessIo, PartialWriteInt)
{
	MockInterprocessIo mii;

	int value = 0x11223344;
	char* p = reinterpret_cast<char*>(&value);

	Sequence s;

	for(int i=0; i<sizeof(int); ++i) {
		EXPECT_CALL(mii, WritePipe(Pointee(*(p+i)), sizeof(int)-i, NotNull()))
			.InSequence(s)
			.WillOnce(DoAll(SetArgPointee<2>(1), Return(TRUE)));
	}

	EXPECT_NO_THROW(mii.Write(value));
}

TEST(InterprocessIo, PartialWriteIntWithFailure)
{
	MockInterprocessIo mii;

	int value = 0x11223344;
	char* p = reinterpret_cast<char*>(&value);

	Sequence s;

	for(int i=0; i<sizeof(int)/2; ++i) {
		EXPECT_CALL(mii, WritePipe(Pointee(*(p+i)), sizeof(int)-i, NotNull()))
			.InSequence(s)
			.WillOnce(DoAll(SetArgPointee<2>(1), Return(TRUE)));
	}

	EXPECT_CALL(mii, WritePipe(Pointee(*(p+sizeof(int)/2)), sizeof(int)-sizeof(int)/2, NotNull()))
		.InSequence(s)
		.WillOnce(Return(FALSE));

	EXPECT_THROW(mii.Write(value), pipe_error);
}

TEST(InterprocessIo, WriteCString)
{
	MockInterprocessIo mii;

	CString value("some string");
	int len = value.GetLength();
	char* p = const_cast<char*>(reinterpret_cast<const char*>((LPCTSTR)value));

	InSequence s;

	EXPECT_CALL(mii, WritePipe(NotNull(), sizeof(int), NotNull()))
		.With(Args<0, 1>(ElementsAreArray(reinterpret_cast<char*>(&len), sizeof(int))))
		.WillOnce(DoAll(SetArgPointee<2>(sizeof(int)), Return(TRUE)));
	EXPECT_CALL(mii, WritePipe(NotNull(), len*sizeof(TCHAR), NotNull()))
		.With(Args<0, 1>(ElementsAreArray(p, len*sizeof(TCHAR))))
		.WillOnce(DoAll(SetArgPointee<2>(len*sizeof(TCHAR)), Return(TRUE)));

	EXPECT_NO_THROW(mii.Write(value));
}

TEST(InterprocessIo, WriteCStringWithFailureOnWritingStringLength)
{
	MockInterprocessIo mii;

	CString value("some string");
	int len = value.GetLength();
	char* p = const_cast<char*>(reinterpret_cast<const char*>((LPCTSTR)value));

	EXPECT_CALL(mii, WritePipe(NotNull(), sizeof(int), NotNull()))
		.With(Args<0, 1>(ElementsAreArray(reinterpret_cast<char*>(&len), sizeof(int))))
		.WillOnce(Return(FALSE));

	EXPECT_THROW(mii.Write(value), pipe_error);
}

TEST(InterprocessIo, WriteCStringWithFailureOnWritingStringBytes)
{
	MockInterprocessIo mii;

	CString value("some string");
	int len = value.GetLength();
	char* p = const_cast<char*>(reinterpret_cast<const char*>((LPCTSTR)value));

	InSequence s;

	EXPECT_CALL(mii, WritePipe(NotNull(), sizeof(int), NotNull()))
		.With(Args<0, 1>(ElementsAreArray(reinterpret_cast<char*>(&len), sizeof(int))))
		.WillOnce(DoAll(SetArgPointee<2>(sizeof(int)), Return(TRUE)));
	EXPECT_CALL(mii, WritePipe(NotNull(), len*sizeof(TCHAR), NotNull()))
		.With(Args<0, 1>(ElementsAreArray(p, len*sizeof(TCHAR))))
		.WillOnce(Return(FALSE));

	EXPECT_THROW(mii.Write(value), pipe_error);
}
