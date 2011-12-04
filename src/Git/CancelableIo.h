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

#ifndef CANCELABLE_IO_H
#define CANCELABLE_IO_H

#include "Cancelable.h"

#include "SharedHandle.h"

class CancelableIo : public Cancelable {
public:

	CancelableIo(const shared_handle& h);
	~CancelableIo();

	BOOL ReadFile(void* buf, DWORD bytesToRead, LPDWORD bytesRead);
	BOOL WriteFile(const void* buf, DWORD bytesToWrite, LPDWORD bytesWritten);

private:

	bool WaitUntilIoComplete(HANDLE hFile);

	virtual void OnCancel();

	shared_handle m_hFile;

	shared_handle m_hIoEvent;
	shared_handle m_hCancelEvent;
};

#endif
