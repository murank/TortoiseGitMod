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

#ifndef OWNER_DATA_LIST_CTRL_H
#define OWNER_DATA_LIST_CTRL_H

#include "AbstractListCtrl.h"

class OwnerDataListCtrl : public AbstractListCtrl {
protected:

	OwnerDataListCtrl();
	~OwnerDataListCtrl();

	virtual void ResetItemCount(int count);

	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

private:

	virtual CString GetSubItemText(int iItem, int iSubItem) const = 0;

};

#endif
