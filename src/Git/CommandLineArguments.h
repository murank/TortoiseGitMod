// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2012 - TortoiseGit

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

#ifndef COMMAND_LINE_ARGUMETNS_H
#define COMMAND_LINE_ARGUMETNS_H

class CommandLineArguments {
public:

	CommandLineArguments();
	~CommandLineArguments();

	void Add(CString key, const CString& value);

	bool HasKey(const CString& key) const;
	CString GetAsString(const CString& key) const;

	size_t GetCount() const;

private:

	typedef std::map<CString, CString> map_type;

	map_type::const_iterator FindValue(CString key) const;

	map_type m_values;
};

#endif
