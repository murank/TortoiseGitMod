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

#include "stdafx.h"

#include "RegUtils.h"

#include "RegKey.h"

namespace internal {

std::pair<CString, CString> ExtractRegKey(const CString& regKey)
{
	int valuePos = regKey.ReverseFind(_T('\\'));
	if(valuePos <= 0) {
		throw std::invalid_argument("invalid reg key");
	}

	return std::make_pair(regKey.Left(valuePos), regKey.Mid(valuePos+1));
}

} // namespace internal

using internal::ExtractRegKey;

template <typename T>
static T DoReadRegistry(const CString& regkey, T defaultValue)
{
	try {
		std::pair<CString, CString> keyAndValue = ExtractRegKey(regkey);

		CString subkey = keyAndValue.first;
		CString entryName = keyAndValue.second;

		RegKey reg(HKEY_CURRENT_USER, subkey);

		T value;
		if(!reg.Read(entryName, value)) {
			return defaultValue;
		}

		return value;

	} catch(std::invalid_argument&) {
		// do nothing
	}

	return defaultValue;
}

DWORD ReadRegistry(const CString& regkey, DWORD defaultValue)
{
	return DoReadRegistry(regkey, defaultValue);
}

CString ReadRegistry(const CString& regkey, const CString& defaultValue)
{
	return DoReadRegistry(regkey, defaultValue);
}

template <typename T>
static bool DoWriteRegistry(const CString& regkey, T value)
{
	try {
		std::pair<CString, CString> keyAndValue = ExtractRegKey(regkey);

		CString subkey = keyAndValue.first;
		CString entryName = keyAndValue.second;

		RegKey reg(HKEY_CURRENT_USER, subkey);

		return reg.Write(entryName, value);

	} catch(std::invalid_argument&) {
		// do nothing
	}

	return false;
}

bool WriteRegistry(const CString& regkey, DWORD value)
{
	return DoWriteRegistry(regkey, value);
}

bool WriteRegistry(const CString& regkey, const CString& value)
{
	return DoWriteRegistry(regkey, value);
}
