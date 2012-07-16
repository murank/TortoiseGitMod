// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2008 - TortoiseSVN

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

#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include "Cancelable.h"
#include "TaskListener.h"

#include "SharedPtr.h"

class Task;
class WorkerThread;

class WorkerManager : public TaskListener, public Cancelable {
public:

	WorkerManager();
	virtual ~WorkerManager();

	void Initialize(size_t numWorkers);

	void RegisterTask(const shared_ptr<Task>& task);

	virtual void OnCancel();
	virtual void OnFinishTask(WorkerThread* worker);

	size_t GetMaxWorkers() const;
	std::pair<size_t, size_t> GetNumWorkers() const;

protected:

	void RegisterActiveWorker(const shared_ptr<WorkerThread>& worker);
	void RegisterInactiveWorker(const shared_ptr<WorkerThread>& worker);

private:

	bool InactiveWorkerExists() const;
	bool ExceedsMaxWorkers() const;

	shared_ptr<WorkerThread> GetInactiveWorker();
	shared_ptr<WorkerThread> GetWorker();

	virtual shared_ptr<WorkerThread> CreateWorker() const;

	mutable CComAutoCriticalSection m_critSec;

	size_t m_maxWorkers;

	typedef shared_ptr<WorkerThread> WorkerPtr;
	typedef std::list<WorkerPtr> WorkerList;
	WorkerList m_activeWorkers;
	WorkerList m_inactiveWorkers;

	// to avoid dead-lock which occurs when calling a worker's destructor in the worker's main loop (via OnFinishTask()),
	// temporarily keep a worker that is to be removed
	WorkerPtr m_workerGrave;
};

void SetGlobalWorkerManager(const shared_ptr<WorkerManager>& manager);
shared_ptr<WorkerManager> GetGlobalWorkerManager();

#endif;
