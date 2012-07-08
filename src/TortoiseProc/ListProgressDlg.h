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

#ifndef LIST_PROGRESS_DLG_H
#define LIST_PROGRESS_DLG_H

#include "Cancelable.h"
#include "StandAloneDlg.h"

#include "Atomic.h"
#include "SharedPtr.h"

class ProgressListCtrl;
class TaskbarList;

class ListProgressDlg : public CResizableStandAloneDialog, public Cancelable {
public:

	ListProgressDlg();
	virtual ~ListProgressDlg();

	void SetBackgroundImage(int nID);
	void SetWindowTitle(int nID);
	void SetBaseDirectory(const CString& path);
	void SetTotalCount(int count);

	void AddEntry(const CString& action, const CString& path, const CString& mime, COLORREF color);
	void AddAuxEntry(const CString& action, const CString& path, const CString& mime, COLORREF color);

	void NotifySucceeded();
	void NotifyFailed();

	void ShowExtraButton();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	void InitDialog();

	afx_msg LRESULT	OnTaskbarListCreated(WPARAM wParam, LPARAM lParam);
	afx_msg void OnExtraButtonClicked();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

private:

	virtual void RegisterChildContrlToResize();
	virtual void EnableSaveAndRestore();
	virtual void InitListCtrl();

	void InitChildControl();
	virtual void EnableDlgItem(int nID, bool bEnable);
	virtual void DoSetDlgItemText(int nID, LPCTSTR str);

	void InitBackgroundImage();
	virtual void DoSetBackgroundImage(int nID);

	void InitWindowTitle();
	virtual void TweakWindowTitle(int nID, const CString& dir);

	void InitTaskbarList();

	void DoAddEntry(const CString& action, const CString& path, const CString& mime, COLORREF color);
	virtual void EnsureLastItemVisible();
	virtual void UpdateProgress(int count, int totalCount);
	void UpdateTaskbarList(int count, int totalCount);
	void UpdateProgressBar(int count, int totalCount);

	void AdjustColumnWidths();
	void DoAdjustColumnWidths();

	void OnSucceded();
	void OnFailed();
	void OnFinished();
	virtual void ChangeWindowTitleOnFinished(int origTitleId, int stateId);
	virtual void ShowDlgItem(int nID, bool bVisible);
	virtual void AppendFinishedMessage();

	Atomic<bool> m_bRunning;

	shared_ptr<ProgressListCtrl> m_listCtrl;
	shared_ptr<TaskbarList> m_taskbarList;

	int m_listBkImage;
	int m_windowTitle;
	CString m_baseDir;

	int m_entryCount;
	int m_totalCount;
};

#endif
