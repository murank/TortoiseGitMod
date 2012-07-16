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

#include "WorkerThread.h"

#include "Task.h"
#include "TaskListener.h"

WorkerThread::WorkerThread(TaskListener* listener)
	: m_critSec(), m_listener(listener), m_cond(), m_task()
{
}

WorkerThread::~WorkerThread()
{
	CancelAsync();
	Join();
}

bool WorkerThread::AssignTask(const shared_ptr<Task>& task)
{
	AutoLocker lock(m_critSec);

	assert(task);

	if(IsCancelPending()) {
		return false;
	}
	if(m_task) {
		// already assigned
		return false;
	}

	m_task = task;
	DoNotify();

	return true;
}

void WorkerThread::Run()
{
	while(!IsCancelPending()) {
		DoRun();
	}
}

void WorkerThread::OnCancelRequired()
{
	AutoLocker lock(m_critSec);

	if(m_task) {
		m_task->CancelAsync();
	}

	DoNotify();
}

bool WorkerThread::HasTask() const
{
	AutoLocker lock(m_critSec);

	return m_task;
}

void WorkerThread::DoRun()
{
	shared_ptr<Task> task = GetTask();
	if(!task) {
		return;
	}

	try {
		task->Run();
	} catch(...) {
		// do nothing
	}

	ResetTask();
	NotifyTaskFinished();
}

shared_ptr<Task> WorkerThread::GetTask()
{
	AutoLocker lock(m_critSec);

	if(!WaitForTaskArrival(lock)) {
		return shared_ptr<Task>();
	}

	return m_task;
}

void WorkerThread::ResetTask()
{
	AutoLocker lock(m_critSec);

	m_task.reset();
}

bool WorkerThread::WaitForTaskArrival(AutoLocker& lock)
{
	while(!m_task) {
		if(IsCancelPending()) {
			return false;
		}

		DoWait(lock);
	}

	return true;
}

bool WorkerThread::DoWait(AutoLocker& lock)
{
	return m_cond.Wait(lock);
}

void WorkerThread::DoNotify()
{
	m_cond.Notify();
}

void WorkerThread::NotifyTaskFinished()
{
	assert(m_listener);
	m_listener->OnFinishTask(this);
}
