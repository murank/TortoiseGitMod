// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2010 - TortoiseGit

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

#ifndef SHARED_HANDLE_H
#define SHARED_HANDLE_H

#include "SharedBase.h"
#include <windows.h>

class shared_handle : public internal::shared_base<shared_handle, HANDLE>
{
public:

	typedef HANDLE handle_type;

	explicit shared_handle(handle_type h = default_value());

	static bool valid(handle_type h);
	static void close(handle_type h);
	static handle_type default_value();

};

#endif
