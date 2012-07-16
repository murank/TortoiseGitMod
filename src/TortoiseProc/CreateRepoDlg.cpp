// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2012 - TortoiseGit

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

#include "CreateRepoDlg.h"

#include "ProcUtils.h"
#include "Resource.h"
#include "SmartHandle.h"

static void MarkWindowAsUnpinnable(HWND hWnd)
{
	typedef HRESULT (WINAPI *SHGPSFW) (HWND hwnd,REFIID riid,void** ppv);

	CAutoLibrary hShell = LoadLibrary(_T("Shell32.dll"));

	if (!hShell.IsValid()) {
		return;
	}

	SHGPSFW pfnSHGPSFW = (SHGPSFW)::GetProcAddress(hShell, "SHGetPropertyStoreForWindow");
	if (!pfnSHGPSFW) {
		return;
	}

	IPropertyStore *pps;
	HRESULT hr = pfnSHGPSFW(hWnd, IID_PPV_ARGS(&pps));
	if (FAILED(hr)) {
		return;
	}

	PROPVARIANT var;
	var.vt = VT_BOOL;
	var.boolVal = VARIANT_TRUE;
	hr = pps->SetValue(PKEY_AppUserModel_PreventPinning, var);
	pps->Release();
}

CreateRepoDlg::CreateRepoDlg(const CString& dir)
	: CStandAloneDialog(IDD_CREATEREPO, NULL), // no parent
	  m_bBare(FALSE), m_dir(dir)
{
}

CreateRepoDlg::~CreateRepoDlg()
{
}

bool CreateRepoDlg::IsBare() const
{
	return (m_bBare != FALSE);
}

BOOL CreateRepoDlg::OnInitDialog()
{
	CStandAloneDialog::OnInitDialog();

	MarkWindowAsUnpinnable(m_hWnd);

	TweakWindowTitle();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CreateRepoDlg::OnOK()
{
	UpdateData(TRUE);

	CStandAloneDialog::OnOK();
}

void CreateRepoDlg::OnCancel()
{
	CStandAloneDialog::OnCancel();
}

void CreateRepoDlg::DoDataExchange(CDataExchange* pDX)
{
	CStandAloneDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_BARE, m_bBare);
}

void CreateRepoDlg::TweakWindowTitle()
{
	::TweakWindowTitle(*this, m_dir);
}
