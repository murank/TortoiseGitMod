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

#include "Thread.h"


Thread::Thread()
	: m_hThread(), m_counter()
{
}

Thread::~Thread()
{
	Join();
}

static unsigned int WINAPI ThreadEntryDelegate(LPVOID arg)
{
	static_cast<Thread*>(arg)->ThreadEntry();
	return 0;
}

bool Thread::BeginThread()
{
	if(m_hThread) {
		return true;
	}

	HANDLE h = (HANDLE)_beginthreadex(NULL, 0, ThreadEntryDelegate, this, CREATE_SUSPENDED, NULL);
	if(h == INVALID_HANDLE_VALUE) {
		return false;
	}

	m_counter.CountUp();
	m_hThread.reset(h);

	ResumeThread(m_hThread.get());

	return m_hThread;
}

void Thread::Join()
{
	m_counter.Await();
}

void Thread::ThreadEntry()
{
	CountLatch::Decrementor dec(m_counter);

	if(IsCancelPending()) {
		return;
	}

	Run();
}

void Thread::Run()
{
}
