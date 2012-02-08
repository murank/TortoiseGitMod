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

#include "CommandLineParser.h"

#include "CommandLineArguments.h"

static bool IsSeparator(TCHAR ch)
{
	return (ch == _T(' '));
}

static bool IsCommandSwitch(TCHAR ch)
{
	return ((ch == _T('/')) || (ch == _T('-')));
}

static bool IsKey(TCHAR ch)
{
	return (_istalnum(ch) != 0);
}

static bool IsValueSeparator(TCHAR ch)
{
	return (ch == _T(':'));
}

static bool IsQuote(TCHAR ch)
{
	return (ch == _T('"'));
}

template <typename F>
static bool ReadChar(const CString& cmdLine, int& pos, F forwardIf)
{
	if(!forwardIf(cmdLine[pos])) {
		return false;
	}

	++pos;
	return true;
}

template <typename F>
static bool ForwardString(const CString& cmdLine, int& pos, F forwardUntil)
{
	int len = cmdLine.GetLength();
	for(; pos<len; ++pos) {

		if(forwardUntil(cmdLine[pos])) {
			return true;
		}
	}

	return false;
}

template <typename F>
static CString ReadString(const CString& cmdLine, int& pos, F readUntil)
{
	int begin = pos;
	ForwardString(cmdLine, pos, readUntil);

	return cmdLine.Mid(begin, (pos - begin));
}

namespace internal {

bool ReadSeparator(const CString& cmdLine, int& pos)
{
	return ForwardString(cmdLine, pos, std::not1(std::ptr_fun(IsSeparator)));
}

bool ReadCommandSwitch(const CString& cmdLine, int& pos)
{
	return ReadChar(cmdLine, pos, IsCommandSwitch);
}

CString ReadKey(const CString& cmdLine, int& pos)
{
	return ReadString(cmdLine, pos, std::not1(std::ptr_fun(IsKey)));
}

bool ReadValueSeparator(const CString& cmdLine, int& pos)
{
	return ReadChar(cmdLine, pos, IsValueSeparator);
}

static CString ReadQuotedValue(const CString& cmdLine, int& pos)
{
	return ReadString(cmdLine, pos, IsQuote);
}

static CString ReadNormalValue(const CString& cmdLine, int& pos)
{
	return ReadString(cmdLine, pos, IsSeparator);
}

CString ReadValue(const CString& cmdLine, int& pos)
{
	CString value;
	if(ReadChar(cmdLine, pos, IsQuote)) {
		value = ReadQuotedValue(cmdLine, pos);
		ReadChar(cmdLine, pos, IsQuote);
	} else {
		value = ReadNormalValue(cmdLine, pos);
	}

	return value;
}

bool ReadOption(const CString& cmdLine, int& pos, CString& key, CString& value)
{
	if(!ReadCommandSwitch(cmdLine, pos)) {
		return false;
	}

	key = ReadKey(cmdLine, pos);

	if(ReadValueSeparator(cmdLine, pos)) {
		value = ReadValue(cmdLine, pos);
	}

	ReadSeparator(cmdLine, pos);

	return true;
}

} // namespace internal

using internal::ReadOption;
using internal::ReadSeparator;

CommandLineArguments ParseCommandLine(const CString& cmdLine)
{
	CommandLineArguments args;

	int pos = 0;
	ReadSeparator(cmdLine, pos);

	int len = cmdLine.GetLength();
	while(pos<len) {
		CString key;
		CString value;

		if(!ReadOption(cmdLine, pos, key, value)) {
			break;
		}

		args.Add(key, value);
	}

	return args;
}
