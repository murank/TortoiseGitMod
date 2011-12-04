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

#include "stdafx.h"

#include "WorkerManager.h"

#include "AutoLocker.h"
#include "WorkerThread.h"

static shared_ptr<WorkerManager> g_manager;

void SetGlobalWorkerManager(const shared_ptr<WorkerManager>& manager)
{
	g_manager = manager;
}

shared_ptr<WorkerManager> GetGlobalWorkerManager()
{
	return g_manager;
}

WorkerManager::WorkerManager()
	: m_critSec(), m_maxWorkers(0), m_activeWorkers(), m_inactiveWorkers()
{
}

WorkerManager::~WorkerManager()
{
	CancelAsync();
}

void WorkerManager::Initialize(size_t numWorkers)
{
	m_maxWorkers = numWorkers;
}

void WorkerManager::RegisterTask(const shared_ptr<Task>& task)
{
	AutoLocker lock(m_critSec);

	while(true) {
		if(IsCancelPending()) {
			return;
		}

		shared_ptr<WorkerThread> worker = GetWorker();
		if(!worker->AssignTask(task)) {
			continue;
		}

		RegisterActiveWorker(worker);
		return;
	}
}

template <typename workers_type>
static void CancelWorkers(workers_type& workers)
{
	for(workers_type::iterator it=workers.begin(); it!=workers.end(); ++it) {
		(*it)->CancelAsync();
	}
}

void WorkerManager::OnCancel()
{
	AutoLocker lock(m_critSec);

	CancelWorkers(m_activeWorkers);
	CancelWorkers(m_inactiveWorkers);
}

namespace {

class SharedPtrCompare : public std::unary_function<const shared_ptr<WorkerThread>&, bool> {
public:
	SharedPtrCompare(WorkerThread* ptr) : m_ptr(ptr) {}
	bool operator()(const shared_ptr<WorkerThread>& obj) {
		return (obj.get()==m_ptr);
	}
private:
	WorkerThread* m_ptr;
};

} // namespace

template <typename workers_type>
static typename std::iterator_traits<typename workers_type::iterator>::difference_type CountWorker(const workers_type& workers, WorkerThread* worker)
{
	return std::count_if(workers.begin(), workers.end(), SharedPtrCompare(worker));
}

template <typename workers_type>
static typename workers_type::iterator FindWorker(workers_type& workers, WorkerThread* worker)
{
	return std::find_if(workers.begin(), workers.end(), SharedPtrCompare(worker));
}

void WorkerManager::OnFinishTask(WorkerThread* worker)
{
	AutoLocker lock(m_critSec);

	assert(CountWorker(m_activeWorkers, worker) == 1 && "activeWorkers must contain the worker");


	workers_type::iterator it = FindWorker(m_activeWorkers, worker);

	if(!ExceedsMaxWorkers()) {
		RegisterInactiveWorker(*it);
	}

	m_activeWorkers.erase(it);
}

size_t WorkerManager::GetMaxWorkers() const
{
	return m_maxWorkers;
}

std::pair<size_t, size_t> WorkerManager::GetNumWorkers() const
{
	AutoLocker lock(m_critSec);

	return std::make_pair(m_activeWorkers.size(), m_inactiveWorkers.size());
}

void WorkerManager::RegisterActiveWorker(const shared_ptr<WorkerThread>& worker)
{
	m_activeWorkers.push_back(worker);
}

void WorkerManager::RegisterInactiveWorker(const shared_ptr<WorkerThread>& worker)
{
	m_inactiveWorkers.push_back(worker);
}

bool WorkerManager::ExceedsMaxWorkers() const
{
	return (m_activeWorkers.size()+m_inactiveWorkers.size() > m_maxWorkers);
}

bool WorkerManager::InactiveWorkerExists() const
{
	return !m_inactiveWorkers.empty();
}

shared_ptr<WorkerThread> WorkerManager::GetInactiveWorker()
{
	assert(!m_inactiveWorkers.empty());

	shared_ptr<WorkerThread> worker = m_inactiveWorkers.front();
	m_inactiveWorkers.pop_front();
	return worker;
}

shared_ptr<WorkerThread> WorkerManager::GetWorker()
{
	if(InactiveWorkerExists()) {
		return GetInactiveWorker();
	} else {
		return CreateWorker();
	}
}

shared_ptr<WorkerThread> WorkerManager::CreateWorker() const
{
	WorkerManager* unsafeThis = const_cast<WorkerManager*>(this);
	
	shared_ptr<WorkerThread> ret(new WorkerThread(unsafeThis));
	ret->BeginThread();

	return ret;
}
