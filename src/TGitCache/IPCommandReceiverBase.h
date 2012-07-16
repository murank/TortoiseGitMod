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

#ifndef IP_COMMAND_RECEIVER_BASE_H
#define IP_COMMAND_RECEIVER_BASE_H

#include "Cancelable.h"

#include "InterprocessIo.h"
#include "SharedPtr.h"

class IPCommandReceiverBase : public Cancelable {
public:
	
	virtual ~IPCommandReceiverBase();

	virtual void Run();

	virtual void OnCancelRequired();

protected:

	explicit IPCommandReceiverBase(const shared_ptr<InterprocessIo>& io);

	template <typename T>
	T Read()
	{
		return m_io->Read<T>();
	}

	template <typename T>
	void Write(T value)
	{
		return m_io->Write(value);
	}

	void Write(const CString& value)
	{
		return m_io->Write(value);
	}

private:

	void DoWriteResultFailure(bool writeResult);

	virtual void CallCommand() = 0;

	virtual void WriteResultSuccess();
	virtual void WriteResultFailure();

	virtual void ReceiveRequest();
	virtual void SendResponse();

	shared_ptr<InterprocessIo> m_io;

};

#endif
