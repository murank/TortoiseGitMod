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

#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include "Cancelable.h"

#include "AutoLocker.h"
#include "Condition.h"

class pop_interrupted : public std::exception {
};

template <typename T>
class ConcurrentQueue : public Cancelable {
public:

	ConcurrentQueue()
		: m_critSec(), m_cond(), m_queue()
	{
	}

	~ConcurrentQueue()
	{
		CancelAsync();
	}

	void Push(const T& value)
	{
		AutoLocker lock(m_critSec);

		m_queue.push_back(value);
		Notify();
	}

	T Pop()
	{
		AutoLocker lock(m_critSec);

		if(IsCancelPending() || !WaitUntilQueueAvailable(lock)) {
			throw pop_interrupted();
		}

		T value = m_queue.front();
		m_queue.pop_front();

		return value;
	}

	size_t GetCount() const
	{
		AutoLocker lock(m_critSec);
		return m_queue.size();
	}

private:

	bool WaitUntilQueueAvailable(AutoLocker& lock)
	{
		while(m_queue.empty()) {
			if(!Wait(lock) || IsCancelPending()) {
				return false;
			}
		}

		return true;
	}

	virtual void OnCancelRequired() {
		Notify();
	}

	virtual bool Wait(AutoLocker& lock) {
		return m_cond.Wait(lock);
	}

	virtual void Notify() {
		m_cond.Notify();
	}

	mutable CComAutoCriticalSection m_critSec;
	Condition m_cond;

	typedef std::list<T> queue_type;
	queue_type m_queue;

};

#endif
