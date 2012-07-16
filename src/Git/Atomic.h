// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2010 - TortoiseGit

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

#ifndef ATOMIC_H
#define ATOMIC_H

template <typename T>
class Atomic {
public:

	Atomic()
		: m_critSec(), m_value()
	{}
	explicit Atomic(const T& value)
		: m_critSec(), m_value(value)
	{}
	~Atomic()
	{}

	Atomic& operator=(const T& rhs)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_critSec);

		m_value = rhs;
		return *this;
	}
	operator T()
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_critSec);

		return m_value;
	}

private:
	CComAutoCriticalSection m_critSec;
	T m_value;
};

template <>
class Atomic<bool> {
private:
	enum {
		FALSE_VALUE = 0,
		TRUE_VALUE = 1
	};

public:

	Atomic()
		: m_value()
	{}
	explicit Atomic(bool value)
		: m_value(value ? TRUE_VALUE : FALSE_VALUE)
	{}
	~Atomic()
	{}

	Atomic& operator=(bool rhs)
	{
		InterlockedExchange(&m_value, rhs ? TRUE_VALUE : FALSE_VALUE);
		return *this;
	}
	operator bool() const
	{
		return m_value!=FALSE_VALUE;
	}
	bool Acquire()
	{
		return SetValue(FALSE_VALUE, TRUE_VALUE);
	}
	bool Release()
	{
		return SetValue(TRUE_VALUE, FALSE_VALUE);
	}

private:

	bool SetValue(long oldValue, long newValue)
	{
		return InterlockedCompareExchange(&m_value, newValue, oldValue)==oldValue;
	}

	volatile long m_value;
};

class AtomicBoolLocker {
public:

	explicit AtomicBoolLocker(Atomic<bool>& a, bool forceRelease = false)
		: m_atomic(a), m_locked(false), m_forceRelease(forceRelease)
	{
		Acquire();
	}
	~AtomicBoolLocker()
	{
		Release();
	}

private:
	void Acquire()
	{
		m_locked = m_atomic.Acquire();
	}
	void Release()
	{
		if(!m_locked && !m_forceRelease) {
			return;
		}

		m_atomic.Release();
		m_locked = false;
	}

	Atomic<bool>& m_atomic;
	bool m_locked;
	bool m_forceRelease;
};

#endif
