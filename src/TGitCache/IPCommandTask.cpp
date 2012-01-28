// TortoiseGit - a Windows shell extension for easy version control

// External Cache Copyright (C) 2005-2008 - TortoiseSVN
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

#include "IPCommandTask.h"

#include "GetStatusCommandReceiver.h"
#include "InterprocessIo.h"
#include "IPCommandReceiverBase.h"
#include "FakeIPCommandReceiver.h"

IPCommandTask::IPCommandTask(const shared_ptr<InterprocessIo> &io)
	: m_io(io)
{
}

IPCommandTask::~IPCommandTask()
{
}

void IPCommandTask::Run()
{
	int commandId = ReadCommandId();

	shared_ptr<IPCommandReceiverBase> receiver = GetCommandReceiver(commandId);

	DoRunReceiver(receiver);
}

const shared_ptr<InterprocessIo>& IPCommandTask::GetIo() const
{
	return m_io;
}


int IPCommandTask::ReadCommandId()
{
	return m_io->Read<int>();
}

shared_ptr<IPCommandReceiverBase> IPCommandTask::GetCommandReceiver(int commandId) const
{
	switch(commandId) {
		case GetStatusCommandReceiver::COMMAND_ID:
			return shared_ptr<IPCommandReceiverBase>(new GetStatusCommandReceiver(GetIo()));

		default:
			return FakeIPCommandReceiver::Instance();
	}
}

void IPCommandTask::DoRunReceiver(shared_ptr<IPCommandReceiverBase>& receiver)
{
	receiver->Run();
}
