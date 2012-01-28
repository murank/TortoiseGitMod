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

#include "CountLatch.h"


CountLatch::CountLatch(long value)
	: m_critSec(), m_count(value), m_cond()
{
}

CountLatch::~CountLatch()
{
}

bool CountLatch::CountDown()
{
	AutoLocker lock(m_critSec);

	if(m_count == std::numeric_limits<counter_type>::min()) {
		return false;
	}

	if(--m_count == 0) {
		Notify();
	}

	return true;
}

bool CountLatch::CountUp()
{
	AutoLocker lock(m_critSec);

	if(m_count == std::numeric_limits<counter_type>::max()) {
		return false;
	}

	if(++m_count == 0) {
		Notify();
	}

	return true;
}

void CountLatch::Await()
{
	AutoLocker lock(m_critSec);

	while(m_count!=0) {
		if(!Wait(lock)) {
			break;
		}
	}
}

int CountLatch::GetCount() const
{
	AutoLocker lock(m_critSec);

	return m_count;
}

void CountLatch::SetCount(counter_type count)
{
	m_count = count;
}

void CountLatch::Notify()
{
	m_cond.NotifyAll();
}

bool CountLatch::Wait(AutoLocker& lock)
{
	return m_cond.Wait(lock);
}
