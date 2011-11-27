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

#ifndef REGISTRY_H
#define REGISTRY_H

class RegKey {
private:
	typedef void (RegKey::*bool_type)() const;
	void this_type_does_not_support_comparisons() const {}

public:
	RegKey(HKEY base, const CString& subkey);
	~RegKey();

	bool Read(const CString& entryName, DWORD& value) const;
	bool Read(const CString& entryName, CString& value) const;

	bool Write(const CString& entryName, DWORD value) const;
	bool Write(const CString& entryName, const CString& value) const;

	operator bool_type() const;

private:

	bool Read(const CString& entryName, std::vector<BYTE>& value, DWORD& type) const;
	bool Write(const CString& entryName, const BYTE* value, size_t size, DWORD type) const;

	HKEY m_hKey;

};

#endif
