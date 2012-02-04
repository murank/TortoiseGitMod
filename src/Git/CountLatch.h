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

#ifndef COUNT_LATCH_H
#define COUNT_LATCH_H

#include "Condition.h"

#include "AutoLocker.h"

class CountLatch {
public:
	typedef long counter_type;

	CountLatch(long value=0);
	~CountLatch();

	bool CountDown();
	bool CountUp();

	void Await();

	int GetCount() const;

	class Decrementor {
	public:
		Decrementor(CountLatch& latch) : m_latch(latch) {
		}
		~Decrementor() {
			m_latch.CountDown();
		}
	private:
		CountLatch& m_latch;
	};

protected:

	void SetCount(counter_type count);

private:

	virtual void Notify();
	virtual bool Wait(AutoLocker& lock);

	mutable CComAutoCriticalSection m_critSec;
	volatile counter_type m_count;
	Condition m_cond;
};

#endif
