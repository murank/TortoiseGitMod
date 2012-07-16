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

#ifndef INTERPROCESS_IO_H
#define INTERPROCESS_IO_H

#include "Cancelable.h"

#include "SharedHandle.h"

class CancelableIo;

class InterprocessIo : public Cancelable {
public:

	explicit InterprocessIo(const shared_handle& hPipe);
	~InterprocessIo();

	template<typename T>
	T Read()
	{
		std::vector<char> buf(sizeof(T));
		Read(&(buf[0]), buf.size());

		return *reinterpret_cast<T*>(&(buf[0]));
	}

	template<typename T>
	void Write(T value)
	{
		Write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

	void Write(const CString& str);

	virtual void OnCancelRequired();

private:

	CString ReadString();
	virtual void Read(char* buf, size_t len);

	void Write(const char* data, size_t len);

	virtual BOOL ReadPipe(char* buf, DWORD bytesToRead, LPDWORD bytesRead);
	virtual BOOL WritePipe(const char* buf, DWORD bytesToWrite, LPDWORD bytesWritten);

	shared_ptr<CancelableIo> m_io;
};

template<>
inline CString InterprocessIo::Read<CString>()
{
	return ReadString();
}


#endif
