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

#include "StdAfxTest.h"

#include "ListProgressDlg.h"

using namespace ::testing;

namespace {

class MockListProgressDlg : public ListProgressDlg {
public:

	MOCK_METHOD0(InitListCtrl, void());
	MOCK_METHOD2(EnableDlgItem, void(int nID, bool bEnable));
	MOCK_METHOD2(DoSetDlgItemText, void(int nID, LPCTSTR str));
	MOCK_METHOD1(DoSetBackgroundImage, void(int nID));
	MOCK_METHOD2(TweakWindowTitle, void(int nID, const CString& dir));
	MOCK_METHOD2(UpdateProgress, void(int count, int totalCount));
	MOCK_METHOD2(ChangeWindowTitleOnFinished, void(int origTitleId, int stateId));

	using ListProgressDlg::InitDialog;

	virtual void RegisterChildContrlToResize() {}
	virtual void EnableSaveAndRestore() {}
	virtual void ShowDlgItem(int /* nID */, bool /* bVisible */) {}
	virtual void AppendFinishedMessage() {}
};

} // namespace

TEST(ListProgressDlgTest, InitDialog)
{
	MockListProgressDlg dlg;
	int bkImage = 1234;
	int titleID = 4321;
	CString baseDir("C:\\some dir");

	EXPECT_CALL(dlg, InitListCtrl())
		.Times(1);

	EXPECT_CALL(dlg, EnableDlgItem(IDOK, false))
		.Times(1);
	EXPECT_CALL(dlg, EnableDlgItem(IDCANCEL, true))
		.Times(1);
	EXPECT_CALL(dlg, DoSetDlgItemText(IDC_LOGBUTTON,_))
		.Times(1);

	EXPECT_CALL(dlg, DoSetBackgroundImage(bkImage))
		.Times(1);

	EXPECT_CALL(dlg, TweakWindowTitle(titleID, baseDir))
		.Times(1);

	dlg.SetBackgroundImage(bkImage);
	dlg.SetWindowTitle(titleID);
	dlg.SetBaseDirectory(baseDir);
	dlg.InitDialog();
}

TEST(ListProgressDlgTest, AddEntryTotalCountIsKnown)
{
	MockListProgressDlg dlg;
	int total = 10;

	InSequence s;
	EXPECT_CALL(dlg, UpdateProgress(1, total))
		.Times(1);
	EXPECT_CALL(dlg, UpdateProgress(2, total))
		.Times(1);

	dlg.SetTotalCount(total);
	dlg.AddEntry(CString(), CString(), CString(), 0);
	dlg.AddAuxEntry(CString(), CString(), CString(), 0);
	dlg.AddEntry(CString(), CString(), CString(), 0);
}

TEST(ListProgressDlgTest, AddEntryTotalCountIsNotKnown)
{
	MockListProgressDlg dlg;

	EXPECT_CALL(dlg, UpdateProgress(_, _))
		.Times(0);

	// total count is unknown
	//dlg.SetTotalCount(total);
	dlg.AddEntry(CString(), CString(), CString(), 0);
	dlg.AddEntry(CString(), CString(), CString(), 0);
}

TEST(ListProgressDlgTest, FinishedSuccessfully)
{
	MockListProgressDlg dlg;
	int titleID = 4321;

	EXPECT_CALL(dlg, EnableDlgItem(IDOK, true))
		.Times(1);
	EXPECT_CALL(dlg, EnableDlgItem(IDCANCEL, false))
		.Times(1);

	EXPECT_CALL(dlg, ChangeWindowTitleOnFinished(titleID, IDS_PROGRS_TITLEFIN))
		.Times(1);

	dlg.SetWindowTitle(titleID);
	dlg.NotifySucceeded();
}

TEST(ListProgressDlgTest, FinishedUnsuccessfully)
{
	MockListProgressDlg dlg;
	int titleID = 4321;

	EXPECT_CALL(dlg, EnableDlgItem(IDOK, true))
		.Times(1);
	EXPECT_CALL(dlg, EnableDlgItem(IDCANCEL, false))
		.Times(1);

	EXPECT_CALL(dlg, ChangeWindowTitleOnFinished(titleID, IDS_PROGRS_TITLEFAILED))
		.Times(1);

	dlg.SetWindowTitle(titleID);
	dlg.NotifyFailed();
}
