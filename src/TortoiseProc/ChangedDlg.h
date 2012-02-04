// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2003-2006,2008 - Stefan Kueng

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

#include "StandAloneDlg.h"
#include "Git.h"
#include "Registry.h"
#include "Tooltip.h"

#include "GitStatusListCtrl.h"

/**
 * \ingroup TortoiseProc
 * Shows the "check for modifications" dialog.
 */
class CChangedDlg : public CResizableStandAloneDialog
{
	DECLARE_DYNAMIC(CChangedDlg)

public:
	CChangedDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CChangedDlg();

// Dialog Data
	enum { IDD = IDD_CHANGEDFILES };

protected:
	virtual void			DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL			OnInitDialog();
	virtual void			OnOK();
	virtual void			OnCancel();
	virtual BOOL			PreTranslateMessage(MSG* pMsg);
	afx_msg void			OnBnClickedRefresh();
	afx_msg void			OnBnClickedCommit();
	afx_msg void			OnBnClickedStash();
	afx_msg void			OnBnClickedShowunversioned();
	afx_msg void			OnBnClickedShowUnmodified();
	afx_msg void			OnBnClickedShowignored();
	afx_msg void			OnBnClickedShowexternals();
	afx_msg void			OnBnClickedShowUserProps();
	afx_msg LRESULT			OnSVNStatusListCtrlNeedsRefresh(WPARAM, LPARAM);
	afx_msg LRESULT			OnSVNStatusListCtrlItemCountChanged(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

private:
	CButton					m_ctrlStash;
	static UINT				ChangedStatusThreadEntry(LPVOID pVoid);
	UINT					ChangedStatusThread();
	void					UpdateStatistics();
	DWORD					UpdateShowFlags();

	enum
	{
		// needs to start with 1, since 0 is the return value if *nothing* is clicked on in the context menu
		ID_STASH_SAVE = 1,
		ID_STASH_POP,
		ID_STASH_APPLY,
		ID_STASH_LIST,
	};

public:
	CTGitPathList			m_pathList;

private:
	CRegDWORD				m_regAddBeforeCommit;
	CGitStatusListCtrl		m_FileListCtrl;
	bool					m_bRemote;
	BOOL					m_bShowUnversioned;
	int						m_iShowUnmodified;
	volatile LONG			m_bBlock;
	CString					m_sTitle;
	bool					m_bCanceled;
	BOOL					m_bShowIgnored;
	BOOL					m_bShowExternals;
	BOOL					m_bShowUserProps;
	CToolTips				m_tooltips;
};

