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

#include "ProgressListCtrl.h"

#include "resource.h"
#include "SharedResource.h"
#include "Utilities.h"

namespace {
	enum {
		COL_ACTION = 0,
		COL_PATH = 1,
		COL_MIME = 2
	};
} // namespace

struct ProgressListCtrlItem {

	ProgressListCtrlItem()
	{}
	ProgressListCtrlItem(const CString& action_, const CString& path_, const CString& mime_, COLORREF color_)
		: action(action_), path(path_), mime(mime_), color(color_)
	{}

	CString action;
	CString path;
	CString mime;
	COLORREF color;
};

ProgressListCtrl::ProgressListCtrl()
	: m_itemList()
{
}

ProgressListCtrl::~ProgressListCtrl()
{
}

void ProgressListCtrl::Initialize()
{
	AbstractListCtrl::Initialize();

	InitColumn();
	AdjustColumnWidths();

	ResetItemCount(GetCount());
}

void ProgressListCtrl::AddItem(const CString& action, const CString& path, const CString& mime, COLORREF color)
{
	ItemPtr newItem(new ProgressListCtrlItem(action, path, mime, color));
	m_itemList.push_back(newItem);
	ResetItemCount(GetCount());
}

void ProgressListCtrl::AdjustColumnWidths()
{
	int columnCount = GetColumnCount();
	for(int i=0; i<columnCount; ++i) {
		AdjustColumnWidth(i);
	}
}

void ProgressListCtrl::EnsureLastItemVisible()
{
	DoEnsureVisible(GetCount()-1, FALSE);
}

int ProgressListCtrl::GetCount() const
{
	return static_cast<int>(m_itemList.size());
}

CListCtrl& ProgressListCtrl::GetCtrl()
{
	return *this;
}

static CString GetValue(const ProgressListCtrlItem& item, int iSubItem)
{
	switch(iSubItem) {
		case COL_ACTION:
			return item.action;
		case COL_PATH:
			return item.path;
		case COL_MIME:
			return item.mime;
	};

	assert(!"Never reach here");
	return CString();
}

CString ProgressListCtrl::GetSubItemText(int iItem, int iSubItem) const
{
	CString ret = GetRawSubItemText(iItem, iSubItem);

	if(iSubItem == COL_PATH) {
		ret = GetCompactPath(ret, GetColumnWidth(COL_PATH));
	}

	return ret;
}

COLORREF ProgressListCtrl::GetItemColor(int iItem) const
{
	if(!IsValidItemIndex(iItem)) {
		return RGB(0, 0, 0);
	}

	const ItemPtr& ptr = m_itemList[iItem];
	return ptr->color;
}

bool ProgressListCtrl::IsValidItemIndex(int iItem) const
{
	return ((0 <= iItem) && (iItem < GetCount()));
}

bool ProgressListCtrl::IsValidSubItemIndex(int iSubItem) const
{
	return ((0 <= iSubItem) && (iSubItem < GetColumnCount()));
}

void ProgressListCtrl::InitColumn()
{
	AddColumn(IDS_PROGRS_ACTION);
	AddColumn(IDS_PROGRS_PATH);
	AddColumn(IDS_PROGRS_MIMETYPE);
}

void ProgressListCtrl::AddColumn(int labelId)
{
	AbstractListCtrl::AddColumn(Id2Str(labelId));
}

void ProgressListCtrl::AdjustColumnWidth(int column)
{
	assert(IsValidSubItemIndex(column));

	CString columnLabel = GetColumnLabel(column);
	const int columnMargin = 20;
	int maxWidth = GetStringWidth(columnLabel) + columnMargin;

	int itemCount = GetCount();
	const int subItemMargin = 12;
	for(int i=0; i<itemCount; ++i) {
		CString subItemText = GetRawSubItemText(i, column);
		int colWidth = GetStringWidth(subItemText) + subItemMargin;

		maxWidth = std::max(maxWidth, colWidth);
	}

	SetColumnWidth(column, maxWidth);
}

CString ProgressListCtrl::GetColumnLabel(int column) const
{
	std::vector<TCHAR> buf(MAX_PATH+1);
	TCHAR* p = &(buf[0]);

	HDITEM hdi = {};
	hdi.mask = HDI_TEXT;
	hdi.pszText = p;
	hdi.cchTextMax = static_cast<int>(buf.size()-1);

	GetHeaderCtrl()->GetItem(column, &hdi);

	return CString(p);
}

CString ProgressListCtrl::GetRawSubItemText(int iItem, int iSubItem) const
{
	if(!IsValidItemIndex(iItem)) {
		return CString();
	}
	if(!IsValidSubItemIndex(iSubItem)) {
		return CString();
	}

	const ItemPtr& ptr = m_itemList[iItem];
	return GetValue(*ptr, iSubItem);
}

CString ProgressListCtrl::GetCompactPath(CString str, int width) const
{
	const int MINIMUM_WIDTH = 12;
	const int MARGIN = 12;
	width = std::max(MINIMUM_WIDTH, width-MARGIN);

	shared_resource<HDC> dc(::GetDC(m_hWnd), std::bind1st(std::ptr_fun(::ReleaseDC), m_hWnd));
	if(!dc) {
		return str;
	}

	shared_resource<HGDIOBJ> font(::SelectObject(dc.get(), GetFont()->GetSafeHandle()),
		std::bind1st(std::ptr_fun(::SelectObject), dc.get()));

	PathCompactPath(dc.get(), str.GetBuffer(MAX_PATH), width);
	str.ReleaseBuffer();

	return str;
}
