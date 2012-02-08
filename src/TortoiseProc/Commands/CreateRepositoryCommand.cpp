// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2011 - TortoiseGit

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

#include "CreateRepositoryCommand.h"

#include "CommandLineArguments.h"
#include "CreateRepoDlg.h"
#include "GitRepository.h"
#include "MessageBox.h"
#include "ProcCommandFactory.h"
#include "StaticInit.h"

static shared_ptr<ProcCommand> CreateCreateRepositoryCommand()
{
	return shared_ptr<ProcCommand>(new CreateRepositoryCommand);
}

STATIC_INIT()
{
	ProcCommandFactory::Register(CreateRepositoryCommand::GetName(), CreateCreateRepositoryCommand);
}

CreateRepositoryCommand::~CreateRepositoryCommand()
{
}

CString CreateRepositoryCommand::GetName()
{
	return CString(_T("repocreate"));
}

bool CreateRepositoryCommand::Execute(const CommandLineArguments& args)
{
	CString dir = args.GetAsString(CString(_T("path")));
	return DoExecute(dir);
}

bool CreateRepositoryCommand::DoExecute(const CString& dir)
{
	bool bBare = false;
	if(!IsCreateRepository(dir, bBare)) {
		return false;
	}

	if(!PathIsDirectoryEmpty(dir) && !IsForceCreateRepository(dir)) {
		return false;
	}

	CString output;
	if(!DoCreateRepository(dir, bBare, output)) {
		ShowErrorMessage(output);
		return false;
	}

	ShowSucceededMessage(output);

	return true;
}

bool CreateRepositoryCommand::DoCreateRepository(const CString& dir, bool bBare, CString& output)
{
	shared_ptr<GitRepository> repository = GitRepository::Create(dir);

	return repository->InitRepository(bBare, output);
}

bool CreateRepositoryCommand::PathIsDirectoryEmpty(const CString& path) const
{
	return (::PathIsDirectoryEmpty(path) != FALSE);
}

bool CreateRepositoryCommand::IsCreateRepository(const CString& dir, bool &bBare) const
{
	CreateRepoDlg dlg(dir);

	INT_PTR ret = dlg.DoModal();
	if(ret != IDOK) {
		return false;
	}

	bBare = dlg.IsBare();
	return true;
}

bool CreateRepositoryCommand::IsForceCreateRepository(const CString& dir) const
{
	CString message;
	message.Format(_T("The folder \"%s\" is not empty. Proceeding might cause loss of data."), dir);

	const UINT ABORT = 1;
	return (CMessageBox::Show(NULL, message, _T("TortoiseGit"), 1, IDI_ERROR, _T("A&bort"), _T("&Proceed")) != ABORT);
}

void CreateRepositoryCommand::ShowSucceededMessage(const CString& output) const
{
	CMessageBox::Show(NULL, output, _T("TortoiseGit"), MB_OK | MB_ICONINFORMATION);
}

void CreateRepositoryCommand::ShowErrorMessage(const CString& output) const
{
	CMessageBox::Show(NULL, output, _T("TortoiseGit"), MB_ICONERROR);
}
