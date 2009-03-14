#pragma once
#include "afxcmn.h"
#include "StandAloneDlg.h"
#include "HistoryCombo.h"
// CRefLogDlg dialog

class CRefLogDlg : public CResizableStandAloneDialog
{
	DECLARE_DYNAMIC(CRefLogDlg)

public:
	CRefLogDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRefLogDlg();

// Dialog Data
	enum { IDD = IDD_REFLOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CHistoryCombo m_ChooseRef;
public:
	CListCtrl m_RefList;
public:
	afx_msg void OnBnClickedOk();
};
