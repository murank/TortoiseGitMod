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

#include "AbstractListCtrl.h"

#include "SharedResource.h"

BEGIN_MESSAGE_MAP(AbstractListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

AbstractListCtrl::AbstractListCtrl()
	: m_columnCount()
{
}

AbstractListCtrl::~AbstractListCtrl()
{
}

void AbstractListCtrl::Initialize()
{
	if(!IsInitialized()) {
		return;
	}

	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | GetExtendedStyle());
}

void AbstractListCtrl::AddColumn(const TCHAR* label)
{
	assert(label);

	bool ret = DoAddColumn(label);
	assert(ret && "Fail to add a column");
	if(!ret) {
		return;
	}

	++m_columnCount;
}

static void FillBackground(HDC hDC, int width, int height)
{
	// Fill the background of the compatible DC by drawing an empty string
	RECT rect = {0, 0, width, height};
	ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}

static HBITMAP DrawBkIcon(HWND hWnd, HICON hIcon, int width, int height)
{
	assert(hWnd);

	shared_resource<HDC> parentDC(GetDC(hWnd), std::bind1st(std::ptr_fun(ReleaseDC), hWnd));
	if(!parentDC) {
		return NULL;
	}
	HDC hParentDC = parentDC.get();

	// Create a compatible DC
	shared_resource<HDC> tmpDC(CreateCompatibleDC(hParentDC), DeleteDC);
	if(!tmpDC) {
		return NULL;
	}
	HDC hTmpDC = tmpDC.get();

	// Create a new bitmap of icon size
	HBITMAP hBmp = CreateCompatibleBitmap(hParentDC, width, height);
	if(!hBmp) {
		return NULL;
	}

	// Select it into the compatible DC
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hTmpDC, hBmp);
	FillBackground(hTmpDC, width, height);

	// Draw the icon into the compatible DC
	DrawIconEx(hTmpDC, 0, 0, hIcon, width, height, 0, NULL, DI_NORMAL);
	SelectObject(hTmpDC, hOldBmp);

	return hBmp;
}

bool AbstractListCtrl::SetBackgroundImage(int nID)
{
	const int width = 128;
	const int height = 128;

	SetTextBkColor(CLR_NONE);

	// create a bitmap from the icon
	shared_resource<HICON> hIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nID), IMAGE_ICON, width, height, LR_DEFAULTCOLOR), DestroyIcon);
	if(!hIcon) {
		return false;
	}

	HBITMAP hBmp = DrawBkIcon(m_hWnd, hIcon.get(), width, height);
	if(!hBmp) {
		return false;
	}

	LVBKIMAGE lv;
	lv.ulFlags = LVBKIF_TYPE_WATERMARK;
	lv.hbm = hBmp;
	lv.xOffsetPercent = 100;
	lv.yOffsetPercent = 100;

	SetBkImage(&lv);

	return true;
}

int AbstractListCtrl::GetColumnCount() const
{
	return m_columnCount;
}

BOOL AbstractListCtrl::DoEnsureVisible(int nItem, BOOL bPartialOK)
{
	if(!IsInitialized()) {
		return FALSE;
	}
	return EnsureVisible(nItem, bPartialOK);
}

BOOL AbstractListCtrl::DoSetItemCountEx(int iCount, DWORD dwFlags)
{
	if(!IsInitialized()) {
		return FALSE;
	}
	return SetItemCountEx(iCount, dwFlags);
}

void AbstractListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = CustomDraw(pLVCD);
}

bool AbstractListCtrl::DoAddColumn(const TCHAR* label)
{
	int columnCount = GetColumnCount();
	return (InsertColumn(columnCount, label)>=0);
}

bool AbstractListCtrl::IsInitialized() const
{
	return (::IsWindow(m_hWnd)==TRUE);
}

LONG AbstractListCtrl::CustomDraw(NMLVCUSTOMDRAW* pLVCD)
{
	NMCUSTOMDRAW& customDraw = pLVCD->nmcd;

	switch(customDraw.dwDrawStage) {
		case CDDS_PREPAINT:
			return CDRF_NOTIFYITEMDRAW;

		case CDDS_ITEMPREPAINT:
			OnItemPrePaint(pLVCD);
			return CDRF_NOTIFYPOSTPAINT;

		case CDDS_ITEMPOSTPAINT:
			OnItemPostPaint(pLVCD);
			return CDRF_DODEFAULT;

		default:
			return CDRF_DODEFAULT;
	}
}

inline static int GetIndex(NMLVCUSTOMDRAW* pLVCD)
{
	return static_cast<int>(pLVCD->nmcd.dwItemSpec);
}

void AbstractListCtrl::OnItemPrePaint(NMLVCUSTOMDRAW* pLVCD)
{
	int iItem = GetIndex(pLVCD);

	pLVCD->clrText = GetItemColor(iItem);
}

void AbstractListCtrl::OnItemPostPaint(NMLVCUSTOMDRAW* /*pLVCD*/)
{
}
