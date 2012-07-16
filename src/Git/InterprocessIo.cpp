// TortoiseSVN - a Windows shell extension for easy version control

// External Cache Copyright (C) 2005 - 2006 - Will Dean, Stefan Kueng

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

#include "InterprocessIo.h"

#include "CancelableIo.h"
#include "PipeError.h"
#include "Utilities.h"

InterprocessIo::InterprocessIo(const shared_handle& hPipe)
	: m_io(new CancelableIo(hPipe))
{
}

InterprocessIo::~InterprocessIo()
{
}

CString InterprocessIo::ReadString()
{
	int len = Read<int>();
	if(len<=0) {
		return CString();
	}

	std::vector<char> buf(sizeof(TCHAR)*len);
	Read(&(buf[0]), buf.size());

	return CString(reinterpret_cast<wchar_t*>(&(buf[0])), len);
}

void InterprocessIo::Read(char* buf, size_t len)
{
	assert(buf && "buf must not be NULL");

	DWORD bytesRead = 0;

	while(len>0) {
		if(!ReadPipe(buf, len, &bytesRead)) {
			throw pipe_error(std::string("Failed at InterprocessIo::Read()\r\n")+GetLastErrorString());
		}
		if(bytesRead==0) {
			// reach to EOF, but data to be read remains
			throw pipe_error("Reach to EOF, but data to be read remains");
		}

		buf += bytesRead;
		len -= bytesRead;
	}
}

void InterprocessIo::Write(const CString& str)
{
	int len = str.GetLength();
	assert(len>=0 && "the length of the string must not be negative");

	Write(str.GetLength());
	Write(reinterpret_cast<const char*>((LPCTSTR)str), len*sizeof(TCHAR));
}

void InterprocessIo::Write(const char* data, size_t len)
{
	assert(data && "data must not be NULL");

	DWORD bytesWritten = 0;

	while(len>0) {
		if(!WritePipe(data, len, &bytesWritten)) {
			throw pipe_error(std::string("Failed at InterprocessIo::Write()\r\n")+GetLastErrorString());
		}

		data += bytesWritten;
		len -= bytesWritten;
	}
}

BOOL InterprocessIo::ReadPipe(char* buf, DWORD bytesToRead, LPDWORD bytesRead)
{
	return m_io->ReadFile(buf, bytesToRead, bytesRead);
}

BOOL InterprocessIo::WritePipe(const char* buf, DWORD bytesToWrite, LPDWORD bytesWritten)
{
	return m_io->WriteFile(buf, bytesToWrite, bytesWritten);
}

void InterprocessIo::OnCancelRequired()
{
	m_io->CancelAsync();
}
