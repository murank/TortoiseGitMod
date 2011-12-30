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

#include "InterprocessServer.h"

#include "InterprocessIo.h"

namespace {

class MockInterprocessServer : public InterprocessServer {
public:

	MockInterprocessServer() : InterprocessServer(CString()) {}
	explicit MockInterprocessServer(const CString& pipeName) : InterprocessServer(pipeName) {}

	MOCK_METHOD0(Connect, shared_ptr<InterprocessIo>());
	MOCK_METHOD1(OnConnect, void(shared_ptr<InterprocessIo>& io));
	MOCK_METHOD0(OnCancel, void());

	using InterprocessServer::DoRun;
};

} // namespace

using namespace ::testing;

TEST(InterprocessServer, Initialize)
{
	CString pipeName("some pipe name");
	MockInterprocessServer mis(pipeName);

	EXPECT_EQ(pipeName, mis.GetPipeName());
}

TEST(InterprocessServer, Connect)
{
	MockInterprocessServer mis;
	shared_ptr<InterprocessIo> io(new InterprocessIo(shared_handle()));

	EXPECT_CALL(mis, Connect())
		.WillOnce(Return(io));
	EXPECT_CALL(mis, OnConnect(io))
		.Times(1);

	EXPECT_NO_THROW(mis.DoRun());
}

TEST(InterprocessServer, ConnectThatReturnsNullPtr)
{
	MockInterprocessServer mis;
	shared_ptr<InterprocessIo> io;

	EXPECT_CALL(mis, Connect())
		.WillOnce(Return(io));
	EXPECT_CALL(mis, OnConnect(io))
		.Times(0);

	EXPECT_NO_THROW(mis.DoRun());
}

TEST(InterprocessServer, ConnectAfterCancel)
{
	MockInterprocessServer mis;
	shared_ptr<InterprocessIo> io(new InterprocessIo(shared_handle()));

	EXPECT_CALL(mis, OnCancel())
		.Times(1);

	mis.CancelAsync();

	EXPECT_CALL(mis, Connect())
		.WillOnce(Return(io));
	EXPECT_CALL(mis, OnConnect(io))
		.Times(0);

	EXPECT_NO_THROW(mis.DoRun());
}
