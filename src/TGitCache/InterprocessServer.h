// TortoiseGit - a Windows shell extension for easy version control

// External Cache Copyright (C) 2007 - TortoiseSVN
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

#ifndef INTERPROCESS_SERVER_H
#define INTERPROCESS_SERVER_H

#include "Thread.h"

#include "SharedPtr.h"

class IPConnectionListener;
class InterprocessIo;

class InterprocessServer : public Thread {
public:

	explicit InterprocessServer(const CString& pipeName);
	virtual ~InterprocessServer();

	void SetListener(const shared_ptr<IPConnectionListener>& listener);

	const CString& GetPipeName() const;

	virtual void OnCancel();

protected:

	void DoRun();

private:

	virtual void Run();

	virtual shared_ptr<InterprocessIo> Connect();
	virtual void OnConnect(shared_ptr<InterprocessIo>& io);

	CString m_pipeName;

	shared_ptr<IPConnectionListener> m_listener;

};

shared_ptr<InterprocessServer> GetGlobalInterprocessServer();
void SetGlobalInterprocessServer(const shared_ptr<InterprocessServer>& server);

#endif
