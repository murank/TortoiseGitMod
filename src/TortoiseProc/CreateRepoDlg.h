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

#ifndef CREATE_REPO_DLG_H
#define CREATE_REPO_DLG_H

#include "StandAloneDlg.h"

class CreateRepoDlg : public CStandAloneDialog {
public:

	CreateRepoDlg(const CString& dir);
	virtual ~CreateRepoDlg();

	bool IsBare() const;

protected:

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:

	void TweakWindowTitle();

	BOOL m_bBare;
	CString m_dir;
};

#endif
