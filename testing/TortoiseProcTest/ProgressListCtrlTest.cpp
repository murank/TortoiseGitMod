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

#include "ProgressListCtrl.h"

#include "resource.h"

using namespace ::testing;

namespace {

class MockProgressListCtrl : public ProgressListCtrl {
public:

	virtual bool DoAddColumn(const TCHAR*) {return true;}
	MOCK_METHOD1(ResetItemCount, void(int count));
	MOCK_METHOD1(AdjustColumnWidth, void(int column));
	MOCK_METHOD1(AddColumn, void(int labelId));

	void MockAddColumn(int labelId) {
		CString str;
		str.Format(_T("col%d"), labelId);

		AbstractListCtrl::AddColumn(str);
	}

	using ProgressListCtrl::GetSubItemText;
	using ProgressListCtrl::GetItemColor;
};

} // namespace

TEST(ProgressListCtrlTest, DefaultConstructor)
{
	MockProgressListCtrl ctrl;
	EXPECT_EQ(0, ctrl.GetColumnCount());
	EXPECT_EQ(0, ctrl.GetCount());
}

TEST(ProgressListCtrlTest, AddItems)
{
	MockProgressListCtrl ctrl;
	Sequence s;

	EXPECT_CALL(ctrl, ResetItemCount(1))
		.Times(1)
		.InSequence(s);
	EXPECT_CALL(ctrl, ResetItemCount(2))
		.Times(1)
		.InSequence(s);

	EXPECT_EQ(0, ctrl.GetCount());

	ctrl.AddItem(CString(), CString(), CString(), RGB(0, 0, 0));
	EXPECT_EQ(1, ctrl.GetCount());

	ctrl.AddItem(CString(), CString(), CString(), RGB(0, 0, 0));
	EXPECT_EQ(2, ctrl.GetCount());
}

namespace {

class ProgressListCtrlTest2 : public ::testing::Test {
protected:

	void SetUpItem(const TCHAR* itemText, COLORREF color) {

		ctrl.AddItem(CString(_T("subitem 0 of "))+itemText,
			CString(_T("subitem 1 of "))+itemText,
			CString(_T("subitem 2 of "))+itemText,
			color);
	}

	virtual void SetUp() {
		EXPECT_CALL(ctrl, ResetItemCount(Ge(0)))
			.Times(AnyNumber());

		ON_CALL(ctrl, AddColumn(_))
			.WillByDefault(Invoke(&ctrl, &MockProgressListCtrl::MockAddColumn));

		Sequence s;
		EXPECT_CALL(ctrl, AddColumn(IDS_PROGRS_ACTION))
			.Times(1)
			.InSequence(s);
		EXPECT_CALL(ctrl, AddColumn(IDS_PROGRS_PATH))
			.Times(1)
			.InSequence(s);
		EXPECT_CALL(ctrl, AddColumn(IDS_PROGRS_MIMETYPE))
			.Times(1)
			.InSequence(s);

		EXPECT_CALL(ctrl, AdjustColumnWidth(0))
			.Times(1);
		EXPECT_CALL(ctrl, AdjustColumnWidth(1))
			.Times(1);
		EXPECT_CALL(ctrl, AdjustColumnWidth(2))
			.Times(1);

		ctrl.InitCtrl();

		SetUpItem(_T("item 0"), RGB(0, 0, 255));
		SetUpItem(_T("item 1"), RGB(0, 255, 0));
		SetUpItem(_T("item 2"), RGB(255, 0, 0));
		SetUpItem(_T("item 3"), RGB(255, 255, 255));
	}

	MockProgressListCtrl ctrl;
};

} // namespace

TEST_F(ProgressListCtrlTest2, AdjustColumnWidths)
{
	EXPECT_CALL(ctrl, AdjustColumnWidth(0))
		.Times(1);
	EXPECT_CALL(ctrl, AdjustColumnWidth(1))
		.Times(1);
	EXPECT_CALL(ctrl, AdjustColumnWidth(2))
		.Times(1);

	ctrl.AdjustColumnWidths();
}

TEST_F(ProgressListCtrlTest2, GetSubItemText)
{
	EXPECT_EQ(CString(), ctrl.GetSubItemText(-1, 2));
	EXPECT_EQ(CString("subitem 2 of item 0"), ctrl.GetSubItemText(0, 2));
	EXPECT_EQ(CString(), ctrl.GetSubItemText(0, 3));

	EXPECT_EQ(CString(), ctrl.GetSubItemText(3, -1));
	EXPECT_EQ(CString("subitem 0 of item 3"), ctrl.GetSubItemText(3, 0));
	EXPECT_EQ(CString(), ctrl.GetSubItemText(4, 0));
}

TEST_F(ProgressListCtrlTest2, GetItemColor)
{
	EXPECT_EQ(RGB(0, 0, 0), ctrl.GetItemColor(-1));
	EXPECT_EQ(RGB(0, 0, 255), ctrl.GetItemColor(0));
	EXPECT_EQ(RGB(255, 255, 255), ctrl.GetItemColor(3));
	EXPECT_EQ(RGB(0, 0, 0), ctrl.GetItemColor(4));
}
