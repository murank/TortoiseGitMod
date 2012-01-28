// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2006,2008-2010 - TortoiseSVN

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

#ifndef REG_UTILIS_H
#define REG_UTILIS_H

DWORD ReadRegistry(HKEY base, const CString& regkey, DWORD defaultValue);
CString ReadRegistry(HKEY base, const CString& regkey, const CString& defaultValue);

bool WriteRegistry(HKEY base, const CString& regkey, DWORD value);
bool WriteRegistry(HKEY base, const CString& regkey, const CString& value);


namespace internal {
std::pair<CString, CString> ExtractRegKey(const CString& regKey);
} // namespace internal

#endif
