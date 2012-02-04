// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2011 Sven Strickroth, <email@cs-ware.de>

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

#include "HorizontalResizableStandAloneDialog.h"
#include "HistoryCombo.h"

class CBisectStartDlg : public CHorizontalResizableStandAloneDialog
{
	DECLARE_DYNAMIC(CBisectStartDlg)

public:
	CBisectStartDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBisectStartDlg();

	// Dialog Data
	enum { IDD = IDD_BISECTSTART };

protected:
	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonGood();
	afx_msg void OnBnClickedButtonBad();
	afx_msg void OnChangedRevision();

	CHistoryCombo	m_cLastGoodRevision;
	CHistoryCombo	m_cFirstBadRevision;

public:
	CString			m_LastGoodRevision;
	CString			m_FirstBadRevision;
};