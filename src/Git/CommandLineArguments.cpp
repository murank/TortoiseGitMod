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

#include "stdafx.h"

#include "CommandLineArguments.h"

CommandLineArguments::CommandLineArguments()
: m_values()
{
}

CommandLineArguments::~CommandLineArguments()
{
}

void CommandLineArguments::Add(CString key, const CString& value)
{
	m_values.insert(std::make_pair(key.MakeLower(), value));
}

bool CommandLineArguments::HasKey(const CString& key) const
{
	return (FindValue(key) != m_values.end());
}

CString CommandLineArguments::GetAsString(const CString& key) const
{
	map_type::const_iterator it = FindValue(key);
	if(it == m_values.end()) {
		return CString();
	}

	return it->second;
}

size_t CommandLineArguments::GetCount() const
{
	return m_values.size();
}

CommandLineArguments::map_type::const_iterator CommandLineArguments::FindValue(CString key) const
{
	return m_values.find(key.MakeLower());
}
