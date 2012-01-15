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

#ifndef INTERPROCESS_CLIENT_H
#define INTERPROCESS_CLIENT_H

#include "SharedPtr.h"

class InterprocessIo;

class InterprocessClient {
public:

	enum {
		MAX_RETRY_CONNECTION = 5,
		MAX_RETRY_RUNNING = 2
	};

	explicit InterprocessClient(const CString& pipeName);
	virtual ~InterprocessClient();

	CString GetPipeName() const;

	shared_ptr<InterprocessIo> Connect() const;

private:

	virtual bool RunTGitCache() const;
	virtual shared_ptr<InterprocessIo> DoConnect() const;

	virtual void WaitBeforeRetryRunning() const;
	virtual void WaitBeforeRetryConnection() const;

	CString m_pipeName;

};

shared_ptr<InterprocessClient> GetGlobalInterprocessClient();
void SetGlobalInterprocessClient(const shared_ptr<InterprocessClient>& client);

#endif
