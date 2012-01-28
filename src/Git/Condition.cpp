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

#include "Condition.h"

#include "AutoLocker.h"
#include "Utilities.h"

namespace {

class ReLocker {
public:
	ReLocker(CComCritSecLock<CComCriticalSection>& lock)
		: m_lock(lock), m_unlocked(false)
	{}
	~ReLocker()
	{
		if(m_unlocked) {
			m_lock.Lock();
		}
	}
	void Unlock()
	{
		m_lock.Unlock();
		m_unlocked = true;
	}
private:
	CComCritSecLock<CComCriticalSection>& m_lock;
	bool m_unlocked;
};

} // namespace


Condition::Condition()
	: m_critSec(), m_hEvent(CreateEvent(NULL, FALSE, FALSE, NULL)), m_count(0)
{
}

Condition::~Condition()
{
}

void Condition::Notify()
{
	AutoLocker lock(m_critSec);

	--m_count;
	SetEvent(m_hEvent.get());
}

void Condition::NotifyAll()
{
	AutoLocker lock(m_critSec);

	for(long i=0; i<m_count; ++i) {
		SetEvent(m_hEvent.get());
	}
	m_count = 0;
}

bool Condition::Wait(CComCritSecLock<CComCriticalSection>& lock)
{
	return Wait(lock, INFINITE);
}

bool Condition::Wait(CComCritSecLock<CComCriticalSection>& lock, DWORD ms)
{
	{
		AutoLocker lock(m_critSec);
		++m_count;
	}

	ReLocker relock(lock);
	relock.Unlock();

	DWORD result = WaitForSingleObject(m_hEvent.get(), ms);
	if(result == WAIT_FAILED) {
		throw wait_failed(GetLastErrorString());
	}
	return (result == WAIT_OBJECT_0);
}
