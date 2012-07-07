// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2012 - TortoiseGit

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

#include "OwnerDataListCtrl.h"

BEGIN_MESSAGE_MAP(OwnerDataListCtrl, AbstractListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
END_MESSAGE_MAP()

OwnerDataListCtrl::OwnerDataListCtrl()
{
}

OwnerDataListCtrl::~OwnerDataListCtrl()
{
}

void OwnerDataListCtrl::ResetItemCount(int count)
{
	DoSetItemCountEx(count, LVSICF_NOSCROLL|LVSICF_NOINVALIDATEALL);
}

void OwnerDataListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LVITEM& item = pDispInfo->item;

	if(item.mask & LVIF_TEXT) {
		int iItem = item.iItem;
		int iSubItem = item.iSubItem;

		CString text = GetSubItemText(iItem, iSubItem);
		_tcsncpy_s(item.pszText, item.cchTextMax, text, _TRUNCATE);
	}

	*pResult = 0;
}
