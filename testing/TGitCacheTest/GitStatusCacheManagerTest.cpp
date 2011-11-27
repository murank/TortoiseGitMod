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

#include "GitStatusCacheManager.h"

#include "GitStatusCacheEntry.h"

namespace {

class MockGitStatusCacheManager : public GitStatusCacheManager {
public:

	MOCK_CONST_METHOD1(IsInGitRepository, bool(const CString& path));
	MOCK_CONST_METHOD0(GetFakeEntry, shared_ptr<GitStatusCacheEntry>());
	MOCK_CONST_METHOD1(AllocateEntry, shared_ptr<GitStatusCacheEntry>(const CString& path));
};

class MockGitStatusCacheEntry : public GitStatusCacheEntry {
public:

	MOCK_CONST_METHOD0(GetStatus, git_status_type());
	MOCK_METHOD0(Invalidate, void());

};

} // namespace

using namespace ::testing;

TEST(MockGitStatusCacheManager, GetStatusNotInRepository)
{
	shared_ptr<MockGitStatusCacheEntry> me(new MockGitStatusCacheEntry);

	EXPECT_CALL(*me, GetStatus())
		.WillOnce(Return(git_status_type_modified));

	MockGitStatusCacheManager mm;

	EXPECT_CALL(mm, IsInGitRepository(_))
		.WillOnce(Return(false));
	EXPECT_CALL(mm, GetFakeEntry())
		.WillOnce(Return(me));
	EXPECT_CALL(mm, AllocateEntry(_))
		.Times(0);

	EXPECT_EQ(git_status_type_modified, mm.GetStatus(CString()));
}

TEST(MockGitStatusCacheManager, GetStatusInRepository)
{
	shared_ptr<MockGitStatusCacheEntry> me(new MockGitStatusCacheEntry);

	EXPECT_CALL(*me, GetStatus())
		.WillOnce(Return(git_status_type_modified));

	MockGitStatusCacheManager mm;

	EXPECT_CALL(mm, IsInGitRepository(_))
		.WillOnce(Return(true));
	EXPECT_CALL(mm, GetFakeEntry())
		.Times(0);
	EXPECT_CALL(mm, AllocateEntry(_))
		.WillOnce(Return(me));

	EXPECT_EQ(git_status_type_modified, mm.GetStatus(CString()));
}
