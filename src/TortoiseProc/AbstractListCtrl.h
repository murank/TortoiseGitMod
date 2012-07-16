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

#ifndef ABSTRACT_LIST_CTRL_H
#define ABSTRACT_LIST_CTRL_H

class AbstractListCtrl : protected CListCtrl {
public:

	virtual void Initialize();

	bool SetBackgroundImage(int nID);

	int GetColumnCount() const;

	using CListCtrl::operator new;
	using CListCtrl::operator delete;

protected:

	AbstractListCtrl();
	~AbstractListCtrl();

	void AddColumn(const TCHAR* label);

	virtual COLORREF GetItemColor(int iItem) const = 0;

	BOOL DoEnsureVisible(int nItem, BOOL bPartialOK);
	BOOL DoSetItemCountEx(int iCount, DWORD dwFlags = LVSICF_NOINVALIDATEALL);

	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

private:

	bool IsInitialized() const;

	virtual bool DoAddColumn(const TCHAR* label);

	LONG CustomDraw(NMLVCUSTOMDRAW* pLVCD);
	virtual void OnItemPrePaint(NMLVCUSTOMDRAW* pLVCD);
	virtual void OnItemPostPaint(NMLVCUSTOMDRAW* pLVCD);

	int m_columnCount;
};

#endif
