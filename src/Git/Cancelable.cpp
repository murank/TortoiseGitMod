// TortoiseGit - a Windows shell extension for easy version control

// External Cache Copyright (C) 2005 - 2006 - Will Dean, Stefan Kueng
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

#include "stdafx.h"

#include "Cancelable.h"

namespace {

enum {
	NOT_CANCELED = 0,
	CANCELED = 1
};

} // namespace

Cancelable::Cancelable()
	: m_isCancelPending(NOT_CANCELED)
{
}

Cancelable::~Cancelable()
{
}

bool Cancelable::CancelAsync()
{
	if(InterlockedCompareExchange(&m_isCancelPending, CANCELED, NOT_CANCELED)!=NOT_CANCELED) {
		return false;
	}

	OnCancel();
	return true;
}

bool Cancelable::IsCancelPending() const
{
	return (InterlockedCompareExchange(const_cast<volatile long*>(&m_isCancelPending), NOT_CANCELED, NOT_CANCELED)!=NOT_CANCELED);
}

void Cancelable::OnCancel()
{
}
