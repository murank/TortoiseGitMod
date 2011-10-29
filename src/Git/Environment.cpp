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

#include "stdafx.h"

#include "Environment.h"

Environment::Environment()
	: m_env()
{
	m_env.push_back(_T('\0'));
}

Environment::~Environment()
{
}

void Environment::CopyProcessEnvironment()
{
	m_env.clear();

	shared_ptr<TCHAR> envStr = GetEnvironmentStrings();
	TCHAR* p = envStr.get();
	while(*p !=0 || *(p+1) !=0)
		m_env.push_back(*p++);

	m_env.push_back(_T('\0'));
	m_env.push_back(_T('\0'));
}


static CString GetKey(const CString& entry)
{
	if(entry.IsEmpty() || entry[0]==_T('=')) {
		return CString();
	}

	int start = 0;
	return entry.Tokenize(_T("="), start);
}

static CString GetValue(const CString& entry)
{
	if(entry.IsEmpty()) {
		return CString();
	}
	if(entry[0]==_T('=')) {
		return entry.Mid(1);
	}

	int start = 0;
	entry.Tokenize(_T("="), start);

	return entry.Mid(start);
}

static std::pair<size_t, size_t> FindKey(const std::vector<TCHAR>& env, const TCHAR* key)
{
	assert(key);

	for(size_t i=0; i<env.size(); ++i) {

		CString entry(&(env[i]));
		CString keyInVec = GetKey(entry);

		if(keyInVec.CompareNoCase(key)==0) {
			return std::make_pair(i, min(i+entry.GetLength()+1, env.size()-1)); // positions of this entry and next one
		}

		i += entry.GetLength();
	}

	// key is not found, and it should be inserted before the last '\0'
	return std::make_pair(env.size()-1, env.size()-1);
}


CString Environment::GetEnv(const TCHAR* key) const
{
	assert(key);

	std::pair<size_t, size_t> keyPos = FindKey(m_env, key);
	return GetValue(&(m_env[keyPos.first]));
}

static void AppendEntry(std::vector<TCHAR>& env, const TCHAR* key, const TCHAR* value)
{
	assert(key);
	assert(value);

	std::copy(key, key+_tcsclen(key), std::back_inserter(env));
	env.push_back(_T('='));
	std::copy(value, value+_tcsclen(value), std::back_inserter(env));
	env.push_back(_T('\0'));
}

void Environment::SetEnv(const TCHAR* key, const TCHAR* value)
{
	assert(key);
	assert(value);

	std::pair<size_t, size_t> keyPos = FindKey(m_env, key);


	std::vector<TCHAR> newEnv(m_env.begin(), m_env.begin()+keyPos.first);

	AppendEntry(newEnv, key, value);

	newEnv.insert(newEnv.end(), m_env.begin()+keyPos.second, m_env.end());

	m_env.swap(newEnv);
}


LPTSTR Environment::GetPointer()
{
	return (m_env.size()>1 ? &(m_env[0]) : NULL);
}

shared_ptr<TCHAR> Environment::GetEnvironmentStrings() const
{
	return shared_ptr<TCHAR>(::GetEnvironmentStrings(), ::FreeEnvironmentStrings);
}
