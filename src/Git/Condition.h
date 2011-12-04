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

#ifndef CONDITION_H
#define CONDITION_H

#include "SharedHandle.h"

class Condition {
public:

	Condition();
	~Condition();

	void Notify();
	void NotifyAll();
	bool Wait(CComCritSecLock<CComCriticalSection>& lock);
	bool Wait(CComCritSecLock<CComCriticalSection>& lock, DWORD ms);

private:
	CComAutoCriticalSection m_critSec;
	shared_handle m_hEvent;
	volatile long m_count;
};

#include "ExceptionHelper.h"

DEFINE_RUNTIME_ERROR(wait_failed);

#endif
