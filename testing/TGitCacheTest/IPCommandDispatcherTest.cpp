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

#include "IPCommandDispatcher.h"

#include "InterprocessIo.h"
#include "IPCommandTask.h"
#include "SharedHandle.h"

namespace {

class MockIPCommandDispatcher : public IPCommandDispatcher {
public:

	MockIPCommandDispatcher() : IPCommandDispatcher() {}

	MOCK_METHOD1(DoDispatchCommand, void(shared_ptr<Task>& commandTask));
};

} // namespace

using namespace ::testing;

static const shared_ptr<InterprocessIo>& GetInterprocessIo(shared_ptr<Task>& task)
{
	return dynamic_cast<IPCommandTask*>(task.get())->GetIo();
}

TEST(IPCommandDispatcher, DispatchCommand)
{
	MockIPCommandDispatcher micd;
	shared_ptr<InterprocessIo> io(new InterprocessIo(shared_handle()));

	EXPECT_CALL(micd, DoDispatchCommand(ResultOf(GetInterprocessIo, io)))
		.Times(1);

	EXPECT_NO_THROW(micd.DispatchCommand(io));
}
