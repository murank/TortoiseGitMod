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

#include "CancelableIo.h"

#include <cassert>

CancelableIo::CancelableIo(const shared_handle& h)
	: m_hFile(h), m_hIoEvent(CreateEvent(NULL, FALSE, FALSE, NULL)), m_hCancelEvent(CreateEvent(NULL, FALSE, FALSE, NULL))
{
}

CancelableIo::~CancelableIo()
{
}

static bool IsIoPending()
{
	DWORD er = GetLastError();
	return (er==ERROR_IO_PENDING);
}

BOOL CancelableIo::ReadFile(void* buf, DWORD bytesToRead, LPDWORD bytesRead)
{
	if(IsCancelPending()) {
		return FALSE;
	}

	OVERLAPPED ol = {};
	ol.hEvent = m_hIoEvent.get();

	HANDLE hFile = m_hFile.get();

	BOOL ret = ::ReadFile(hFile, buf, bytesToRead, bytesRead, &ol);
	if(ret || !IsIoPending()) {
		return ret;
	}

	if(!WaitUntilIoComplete(hFile)) {
		return FALSE;
	}
	if(!GetOverlappedResult(hFile, &ol, bytesRead, FALSE)) {
		return FALSE;
	}

	return TRUE;
}

BOOL CancelableIo::WriteFile(const void* buf, DWORD bytesToWrite, LPDWORD bytesWritten)
{
	if(IsCancelPending()) {
		return FALSE;
	}

	OVERLAPPED ol = {};
	ol.hEvent = m_hIoEvent.get();

	HANDLE hFile = m_hFile.get();

	BOOL ret = ::WriteFile(hFile, buf, bytesToWrite, bytesWritten, &ol);
	if(ret || !IsIoPending()) {
		return ret;
	}

	if(!WaitUntilIoComplete(hFile)) {
		return FALSE;
	}
	if(!GetOverlappedResult(hFile, &ol, bytesWritten, FALSE)) {
		return FALSE;
	}

	return TRUE;
}

static bool IsIoEventSet(DWORD retval)
{
	return (retval==WAIT_OBJECT_0);
}

static bool IsCancelEventSet(DWORD retval)
{
	return (retval==WAIT_OBJECT_0+1);
}

bool CancelableIo::WaitUntilIoComplete(HANDLE hFile)
{
	HANDLE hEvents[] = {m_hIoEvent.get(), m_hCancelEvent.get()};

	while(true) {
		DWORD ret = WaitForMultipleObjects(sizeof(hEvents)/sizeof(HANDLE), hEvents, FALSE, INFINITE);

		if(IsIoEventSet(ret)) {
			// IO operation completed
			return true;
		}
		if(IsCancelEventSet(ret)) {
			// cancel IO operation
			CancelIo(hFile);
			continue;
		}

		// some error occured
		return false;
	}
}

void CancelableIo::OnCancelRequired()
{
	SetEvent(m_hCancelEvent.get());
}
