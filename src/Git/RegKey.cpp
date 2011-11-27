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

#include "RegKey.h"

RegKey::RegKey(HKEY base, const CString &subkey)
	: m_hKey(0)
{
	HKEY hKey = 0;
	if(RegOpenKeyEx(base, subkey, 0, KEY_ALL_ACCESS, &hKey)==ERROR_SUCCESS) {
		m_hKey = hKey;
	}
}

RegKey::~RegKey()
{
	if(m_hKey) {
		RegCloseKey(m_hKey);
	}
}

bool RegKey::Read(const CString& entryName, DWORD& value) const
{
	std::vector<BYTE> buf;
	DWORD type = REG_NONE;
	if(!Read(entryName, buf, type) || (type != REG_DWORD)) {
		return false;
	}

	value = *reinterpret_cast<DWORD*>(&buf[0]);
	return true;
}

bool RegKey::Read(const CString& entryName, CString& value) const
{
	std::vector<BYTE> buf;
	DWORD type = REG_NONE;
	if(!Read(entryName, buf, type) || (type != REG_SZ)) {
		return false;
	}

	value = reinterpret_cast<TCHAR*>(&buf[0]);
	return true;
}

bool RegKey::Write(const CString& entryName, DWORD value) const
{
	size_t DWORD_SIZE = 4;
	return Write(entryName, reinterpret_cast<const BYTE*>(&value), DWORD_SIZE, REG_DWORD);
}

bool RegKey::Write(const CString& entryName, const CString& value) const
{
	size_t size = (value.GetLength()+1)*sizeof(TCHAR);
	return Write(entryName, reinterpret_cast<const BYTE*>(static_cast<LPCTSTR>(value)), size, REG_SZ);
}

bool RegKey::Read(const CString& entryName, std::vector<BYTE>& value, DWORD& type) const
{
	assert(*this && "key must be opend successfully");

	DWORD size = 0;

	RegQueryValueEx(m_hKey, entryName, 0, &type, NULL, &size);
	if(size==0) {
		return false;
	}

	value.resize(size);
	if(RegQueryValueEx(m_hKey, entryName, 0, &type, &(value[0]), &size) != ERROR_SUCCESS) {
		return false;
	}

	return true;
}

bool RegKey::Write(const CString &entryName, const BYTE *value, size_t size, DWORD type) const
{
	assert(*this && "key must be opend successfully");
	return RegSetValueEx(m_hKey, entryName, 0, type, value, size)==ERROR_SUCCESS;
}


RegKey::operator RegKey::bool_type() const {
	return m_hKey ?
		&RegKey::this_type_does_not_support_comparisons : 0;
}
