// TortoiseGit - a Windows shell extension for easy version control

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

#ifndef STDAFX_COMMON_H
#define STDAFX_COMMON_H

#include "..\targetver.h"

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headersicit
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers

#ifdef _AFXDLL
#	include "stdafx_mfc.h"
#else
#	include "stdafx_atl.h"
#endif // _AFXDLL

#include <atlbase.h>

#include <string>
#include <set>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <deque>
#include <cassert>
#include <memory>
#include <type_traits>


#endif
