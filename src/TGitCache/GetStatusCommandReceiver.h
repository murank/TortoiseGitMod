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

#ifndef GET_STATUS_COMMAND_RECEIVER_H
#define GET_STATUS_COMMAND_RECEIVER_H

#include "IPCommandReceiver.h"

#include "IPCommandIds.h"

enum git_status_type;

class GetStatusCommandReceiver : public IPCommandReceiver<git_status_type (const CString&)> {
public:

	enum {
		COMMAND_ID = GET_STATUS_COMMAND
	};

	explicit GetStatusCommandReceiver(const shared_ptr<InterprocessIo>& io);
	virtual ~GetStatusCommandReceiver();

private:

	virtual git_status_type DoCommand(const CString& path);

};

#endif
