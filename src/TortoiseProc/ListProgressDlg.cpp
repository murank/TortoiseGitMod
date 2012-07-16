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

#include "ListProgressDlg.h"

#include "ProcUtils.h"
#include "ProgressListCtrl.h"
#include "TaskbarList.h"
#include "Utilities.h"
#include "Win7.h"

BEGIN_MESSAGE_MAP(ListProgressDlg, CResizableStandAloneDialog)
	ON_REGISTERED_MESSAGE(WM_TASKBARBTNCREATED, OnTaskbarListCreated)
	ON_BN_CLICKED(IDC_LOGBUTTON, OnExtraButtonClicked)
	ON_WM_CLOSE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

ListProgressDlg::ListProgressDlg()
	: CResizableStandAloneDialog(IDD_SVNPROGRESS),
	  m_bRunning(), m_listCtrl(new ProgressListCtrl), m_taskbarList(),
	  m_listBkImage(-1), m_windowTitle(-1), m_baseDir(),
	  m_entryCount(), m_totalCount()
{
	m_taskbarList = TaskbarList::Create(m_hWnd);
}

ListProgressDlg::~ListProgressDlg()
{
}

void ListProgressDlg::SetBackgroundImage(int nID)
{
	m_listBkImage = nID;
}

void ListProgressDlg::SetWindowTitle(int nID)
{
	m_windowTitle = nID;
}

void ListProgressDlg::SetBaseDirectory(const CString& path)
{
	m_baseDir = path;
}

void ListProgressDlg::SetTotalCount(int count)
{
	m_totalCount = count;
}

void ListProgressDlg::AddEntry(const CString& action, const CString& path, const CString& mime, COLORREF color)
{
	DoAddEntry(action, path, mime, color);
	AdjustColumnWidths();
	EnsureLastItemVisible();

	if(m_totalCount>0) {
		++m_entryCount;
		UpdateProgress(m_entryCount, m_totalCount);
	}
}

void ListProgressDlg::AddAuxEntry(const CString& action, const CString& path, const CString& mime, COLORREF color)
{
	DoAddEntry(action, path, mime, color);
	AdjustColumnWidths();
	EnsureLastItemVisible();
}

void ListProgressDlg::NotifySucceeded()
{
	AtomicBoolLocker lock(m_bRunning, true);

	OnSucceded();
	OnFinished();
}

void ListProgressDlg::NotifyFailed()
{
	AtomicBoolLocker lock(m_bRunning, true);

	OnFailed();
	OnFinished();
}

void ListProgressDlg::ShowExtraButton()
{
	GetDlgItem(IDC_LOGBUTTON)->ShowWindow(SW_SHOW);
}

void ListProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableStandAloneDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SVNPROGRESS, m_listCtrl->GetCtrl());
}

BOOL ListProgressDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	InitDialog();

	m_bRunning.Acquire();

	return TRUE;
}

void ListProgressDlg::InitDialog()
{
	InitListCtrl();

	RegisterChildContrlToResize();
	EnableSaveAndRestore();

	InitBackgroundImage();
	InitWindowTitle();

	InitChildControl();

	InitTaskbarList();
}

LRESULT ListProgressDlg::OnTaskbarListCreated(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_taskbarList = TaskbarList::Create(m_hWnd);
	return 0;
}

void ListProgressDlg::OnExtraButtonClicked()
{
	CString cmd;
	cmd.Format(_T("/command:commit /path:\"%s\""), GetCurrentDir());

	LaunchProcCommand(cmd);
	EndDialog(IDOK);
}

BOOL ListProgressDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(m_bRunning) {
		// only show the wait cursor over the list control
		if(pWnd == GetDlgItem(IDC_SVNPROGRESS)) {
			HCURSOR hCur = LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT));
			SetCursor(hCur);
			return TRUE;
		}
	}

	HCURSOR hCur = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	SetCursor(hCur);
	return CResizableStandAloneDialog::OnSetCursor(pWnd, nHitTest, message);
}

void ListProgressDlg::OnOK()
{
	if(m_bRunning) {
		CancelAsync();
		return;
	}

	CResizableStandAloneDialog::OnOK();
}

void ListProgressDlg::OnCancel()
{
	if(m_bRunning) {
		CancelAsync();
		return;
	}

	CResizableStandAloneDialog::OnCancel();
}

void ListProgressDlg::OnClose()
{
	if(m_bRunning) {
		CancelAsync();
		return;
	}

	DialogEnableWindow(IDCANCEL, TRUE);
	CResizableStandAloneDialog::OnClose();
}

void ListProgressDlg::RegisterChildContrlToResize()
{
	AddAnchor(IDC_SVNPROGRESS, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_PROGRESSLABEL, BOTTOM_LEFT, BOTTOM_CENTER);
	AddAnchor(IDC_PROGRESSBAR, BOTTOM_CENTER, BOTTOM_RIGHT);
	AddAnchor(IDC_INFOTEXT, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_NONINTERACTIVE, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT);
	AddAnchor(IDOK, BOTTOM_RIGHT);
	AddAnchor(IDC_LOGBUTTON, BOTTOM_RIGHT);
}

void ListProgressDlg::EnableSaveAndRestore()
{
	EnableSaveRestore(_T("GITProgressDlg"));
}

void ListProgressDlg::InitListCtrl()
{
	m_listCtrl->Initialize();
}

void ListProgressDlg::InitChildControl()
{
	EnableDlgItem(IDOK, false);
	EnableDlgItem(IDCANCEL, true);

	DoSetDlgItemText(IDC_LOGBUTTON, _T("Commit ..."));
}

void ListProgressDlg::EnableDlgItem(int nID, bool bEnable)
{
	DialogEnableWindow(nID, bEnable ? TRUE : FALSE);
}

void ListProgressDlg::DoSetDlgItemText(int nID, LPCTSTR str)
{
	assert(str);
	SetDlgItemText(nID, str);
}

void ListProgressDlg::InitBackgroundImage()
{
	DoSetBackgroundImage(m_listBkImage);
}

void ListProgressDlg::DoSetBackgroundImage(int nID)
{
	if(nID>0) {
		m_listCtrl->SetBackgroundImage(nID);
	}
}

void ListProgressDlg::InitWindowTitle()
{
	TweakWindowTitle(m_windowTitle, m_baseDir);
}

void ListProgressDlg::TweakWindowTitle(int nID, const CString& dir)
{
	CString title;
	title.LoadString(nID);

	::TweakWindowTitle(*this, title, dir);
}

void ListProgressDlg::InitTaskbarList()
{
	m_taskbarList->SetProgressState(TBPF_INDETERMINATE);
}

void ListProgressDlg::DoAddEntry(const CString& action, const CString& path, const CString& mime, COLORREF color)
{
	m_listCtrl->AddItem(action, path, mime, color);
}

void ListProgressDlg::EnsureLastItemVisible()
{
	m_listCtrl->EnsureLastItemVisible();
}

void ListProgressDlg::UpdateProgress(int count, int totalCount)
{
	UpdateProgressBar(count, totalCount);
	UpdateTaskbarList(count, totalCount);
}

void ListProgressDlg::UpdateTaskbarList(int count, int totalCount)
{
	m_taskbarList->SetProgressState(TBPF_NORMAL);
	m_taskbarList->SetProgressValue(count, totalCount);
}

void ListProgressDlg::UpdateProgressBar(int count, int totalCount)
{
	if(!m_hWnd) {
		return;
	}

	CProgressCtrl* ctrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESSBAR);
	if(!ctrl) {
		return;
	}

	ShowDlgItem(IDC_PROGRESSBAR, true);

	ctrl->SetPos(count);
	ctrl->SetRange32(0, totalCount);
}

void ListProgressDlg::AdjustColumnWidths()
{
	const int ITEM_COUNT_LIMIT = 30;
	if(m_listCtrl->GetCount() > ITEM_COUNT_LIMIT) {
		return;
	}

	DoAdjustColumnWidths();
}

void ListProgressDlg::DoAdjustColumnWidths()
{
	m_listCtrl->AdjustColumnWidths();
}

void ListProgressDlg::OnSucceded()
{
	ChangeWindowTitleOnFinished(m_windowTitle, IDS_PROGRS_TITLEFIN);

	m_taskbarList->SetProgressState(TBPF_NOPROGRESS);
}

void ListProgressDlg::OnFailed()
{
	ChangeWindowTitleOnFinished(m_windowTitle, IDS_PROGRS_TITLEFAILED);

	m_taskbarList->SetProgressState(TBPF_ERROR);
	m_taskbarList->SetProgressValue(100, 100);
}

void ListProgressDlg::OnFinished()
{
	AppendFinishedMessage();

	EnableDlgItem(IDOK, true);
	EnableDlgItem(IDCANCEL, false);

	ShowDlgItem(IDC_PROGRESSBAR, false);

	RefreshCursor();
	DoAdjustColumnWidths();
}

void ListProgressDlg::ChangeWindowTitleOnFinished(int origTitleId, int stateId)
{
	CString title;
	title.Format(_T("%s %s"), Id2Str(origTitleId), Id2Str(stateId));

	SetWindowText(title);
}

void ListProgressDlg::ShowDlgItem(int nID, bool bVisible)
{
	CProgressCtrl* ctrl = (CProgressCtrl*)GetDlgItem(nID);
	if(!ctrl) {
		return;
	}

	ctrl->ShowWindow(bVisible ? SW_SHOW : SW_HIDE);
}

void ListProgressDlg::AppendFinishedMessage()
{
	AddAuxEntry(Id2Str(IDS_PROGRS_FINISHED), CString(), CString(), RGB(0, 0, 0));
}
