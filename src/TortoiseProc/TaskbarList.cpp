// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2012 - TortoiseGit

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

#include "TaskbarList.h"

namespace {

typedef CComPtr<ITaskbarList3> TaskbarListPtr;

class ConcreteTaskbarList : public TaskbarList {
public:
	ConcreteTaskbarList(HWND hwnd, TaskbarListPtr& pTaskbarList)
		: m_hwnd(hwnd), m_pTaskbarList(pTaskbarList)
	{
	}
	virtual ~ConcreteTaskbarList()
	{
	}
	virtual HRESULT SetProgressState(TBPFLAG tbpFlags)
	{
		return m_pTaskbarList->SetProgressState(m_hwnd, tbpFlags);
	}
	virtual HRESULT SetProgressValue(ULONGLONG ullCompleted, ULONGLONG ullTotal)
	{
		return m_pTaskbarList->SetProgressValue(m_hwnd, ullCompleted, ullTotal);
	}

private:
	HWND m_hwnd;
	TaskbarListPtr m_pTaskbarList;
};

class FakeTaskbarList : public TaskbarList {
public:
	virtual ~FakeTaskbarList()
	{
	}
	virtual HRESULT SetProgressState(TBPFLAG /*tbpFlags*/)
	{
		// do nothing
		return ERROR_SUCCESS;
	}
	virtual HRESULT SetProgressValue(ULONGLONG /*ullCompleted*/, ULONGLONG /*ullTotal*/)
	{
		// do nothing
		return ERROR_SUCCESS;
	}
};

} // namespace


TaskbarList::~TaskbarList()
{
}

shared_ptr<TaskbarList> TaskbarList::Create(HWND hwnd)
{
	TaskbarListPtr pTaskbarList;
	pTaskbarList.CoCreateInstance(CLSID_TaskbarList);

	if(pTaskbarList) {
		return shared_ptr<TaskbarList>(new ConcreteTaskbarList(hwnd, pTaskbarList));
	} else {
		return shared_ptr<TaskbarList>(new FakeTaskbarList());
	}
}

