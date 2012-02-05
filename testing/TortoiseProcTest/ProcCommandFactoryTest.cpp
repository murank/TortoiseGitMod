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

#include "ProcCommandFactory.h"

#include "FakeCommand.h"
#include "CreateRepositoryCommand.h"

using namespace ::testing;

TEST(ProcCommandFactory, GetFakeCommand)
{
	shared_ptr<ProcCommand> command = ProcCommandFactory::GetCommand(CString("non-existent-command"));
	EXPECT_THAT(DynamicCast<FakeCommand>(command), NotNull());
}

namespace {

template <typename T>
class ProcCommandFactoryTest : public ::testing::Test {
public:

};

typedef ::testing::Types<CreateRepositoryCommand> CommandTypes;
TYPED_TEST_CASE(ProcCommandFactoryTest, CommandTypes);

} // namespace

TYPED_TEST(ProcCommandFactoryTest, HasCommand)
{
	EXPECT_TRUE(ProcCommandFactory::HasCommand(TypeParam::GetName()));
}

TYPED_TEST(ProcCommandFactoryTest, GetCommand)
{
	EXPECT_THAT(DynamicCast<TypeParam>(ProcCommandFactory::GetCommand(TypeParam::GetName())), NotNull());
}
