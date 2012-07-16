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

#ifndef PROGRESS_LIST_CTRL_H
#define PROGRESS_LIST_CTRL_H

#include "OwnerDataListCtrl.h"

#include "SharedPtr.h"

struct ProgressListCtrlItem;

class ProgressListCtrl : public OwnerDataListCtrl {
public:

	ProgressListCtrl();
	~ProgressListCtrl();

	virtual void Initialize();

	void AddItem(const CString& action, const CString& path, const CString& mime, COLORREF color);

	void AdjustColumnWidths();

	void EnsureLastItemVisible();

	int GetCount() const;
	CListCtrl& GetCtrl();

protected:

	virtual CString GetSubItemText(int iItem, int iSubItem) const;
	virtual COLORREF GetItemColor(int iItem) const;

private:

	bool IsValidItemIndex(int iItem) const;
	bool IsValidSubItemIndex(int iSubItem) const;

	void InitColumn();

	virtual void AddColumn(int labelId);
	virtual void AdjustColumnWidth(int column);
	CString GetColumnLabel(int column) const;

	CString GetRawSubItemText(int iItem, int iSubItem) const;
	CString GetCompactPath(CString str, int width) const;

	typedef shared_ptr<ProgressListCtrlItem> ItemPtr;
	typedef std::vector<ItemPtr> ItemPtrList;

	ItemPtrList m_itemList;
};

#endif
