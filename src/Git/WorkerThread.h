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

#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include "Thread.h"

#include "AutoLocker.h"
#include "Condition.h"
#include "SharedPtr.h"

class TaskListener;
class Task;

class WorkerThread : public Thread {
public:

	explicit WorkerThread(TaskListener* pool);
	virtual ~WorkerThread();

	virtual bool AssignTask(const shared_ptr<Task>& task);

	virtual void Run();
	virtual void OnCancel();

	bool HasTask() const;

protected:

	void DoRun();

private:

	shared_ptr<Task> GetTask();
	void ResetTask();

	bool WaitForTaskArrival(AutoLocker& lock);
	virtual bool DoWait(AutoLocker& lock);
	virtual void DoNotify();

	virtual void NotifyTaskFinished();

	mutable CComAutoCriticalSection m_critSec;

	TaskListener* m_listener;
	Condition m_cond;

	shared_ptr<Task> m_task;
};

#endif
