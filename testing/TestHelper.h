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

#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <iostream>
#include "SharedPtr.h"

inline ::std::ostream& operator<<(::std::ostream& os, const CString& str) {
	CStringA tmp(str, str.GetLength());
	return os << std::string(tmp, tmp.GetLength());
}

inline ::testing::AssertionResult StrictlyCompareCString(const CString& lhs, const CString& rhs) {
	if(lhs.GetLength()!=rhs.GetLength()) {
		return ::testing::AssertionFailure();
	}

	for(int i=0; i<lhs.GetLength(); ++i) {
		if(lhs[i] != rhs[i]) {
			return ::testing::AssertionFailure();
		}
	}

	return ::testing::AssertionSuccess();
}

template <typename To, typename From>
static const To* DynamicCast(const shared_ptr<From>& ptr)
{
	return dynamic_cast<const To*>(ptr.get());
}


#endif