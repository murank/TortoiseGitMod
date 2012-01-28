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

#ifndef IP_COMMAND_TASK_H
#define IP_COMMAND_TASK_H

#include "Task.h"

#include "SharedPtr.h"

class InterprocessIo;
class IPCommandReceiverBase;

class IPCommandTask : public Task {
public:

	IPCommandTask(const shared_ptr<InterprocessIo>& io);
	virtual ~IPCommandTask();

	virtual void Run();

	const shared_ptr<InterprocessIo>& GetIo() const;

private:

	virtual int ReadCommandId();
	shared_ptr<IPCommandReceiverBase> GetCommandReceiver(int commandId) const;
	virtual void DoRunReceiver(shared_ptr<IPCommandReceiverBase>& receiver);

	shared_ptr<InterprocessIo> m_io;
};

#endif
