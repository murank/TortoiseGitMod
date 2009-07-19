// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2009 - TortoiseGit

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

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "StandAloneDlg.h"
#include "HistoryCombo.h"
#include "MenuButton.h"
#include "registry.h"
#include "Balloon.h"
#include "BranchCombox.h"
// CSyncDlg dialog
#define IDC_SYNC_TAB 0x1000000

class CSyncDlg : public CResizableStandAloneDialog,public CBranchCombox
{
	DECLARE_DYNAMIC(CSyncDlg)

public:
	CSyncDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSyncDlg();

// Dialog Data
	enum { IDD = IDD_SYNC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BRANCH_COMBOX_EVENT_HANDLE();

	CRegDWORD m_regPullButton;
	CRegDWORD m_regPushButton;
	CMFCTabCtrl m_ctrlTabCtrl;
	CBalloon			m_tooltips;
	
	void SetRemote(CString remote)
	{
		if(!remote.IsEmpty())
		{
			this->m_ctrlURL.AddString(remote);
		}
	}
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bAutoLoadPuttyKey;
	
	CHistoryCombo m_ctrlURL;
	CButton m_ctrlDumyButton;
	CMenuButton m_ctrlPull;
	CMenuButton m_ctrlPush;
	CMenuButton m_ctrlStatus;
	afx_msg void OnBnClickedButtonPull();
	afx_msg void OnBnClickedButtonPush();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonEmail();
	CProgressCtrl m_ctrlProgress;
	CAnimateCtrl m_ctrlAnimate;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonManage();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};