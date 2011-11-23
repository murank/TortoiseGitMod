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

#include "GitRepositoryCommand.h"

#include "CommandRunner.h"
#include "Utilities.h"

GitRepositoryCommand::GitRepositoryCommand(const CString& root, int encoding)
	: GitRepository(root, encoding)
{
}

GitRepositoryCommand::~GitRepositoryCommand()
{
}

static git_status_type GetMoreImportantStatus(git_status_type status1, git_status_type status2)
{
	return std::max(status1, status2);
}

static git_status_type GetStatusFromStatusCode(TCHAR code)
{
	/*
	  Meanings of the status code
	    ' ' = unmodified
	    M = modified
	    A = added
	    D = deleted
	    R = renamed
	    C = copied
	    U = updated but unmerged
	    ? = untracked
	    ! = ignored
	*/
	switch(code) {
		case _T(' '):
			return git_status_type_unmodified;
		case _T('M'):
			return git_status_type_modified;
		case _T('A'):
			return git_status_type_added;
		case _T('D'):
			return git_status_type_deleted;
		case _T('R'):
			return git_status_type_renamed;
		case _T('C'):
			return git_status_type_copied;
		case _T('U'):
			return git_status_type_unmerged;
		case _T('?'):
			return git_status_type_untracked;
		case _T('!'):
			return git_status_type_ignored;

		default:
			throw std::invalid_argument("invalid git status");
	}
}

static git_status_type ParseStatus(const CString& line)
{
	if(line.GetLength() < 3) {
		throw std::invalid_argument("invalid git status");
	}

	/*
	  status is represented as follows:
	    XY PATH

		X: status of index
		Y: status of work tree
	*/
	TCHAR indexStatusCode = line[0];
	TCHAR worktreeStatusCode = line[1];

	git_status_type indexStatus = GetStatusFromStatusCode(indexStatusCode);
	git_status_type worktreeStatus = GetStatusFromStatusCode(worktreeStatusCode);

	if((worktreeStatus==git_status_type_added) && (indexStatus==git_status_type_added)) {
		// treat as a special case of conflict
		return git_status_type_unmerged;
	}

	return GetMoreImportantStatus(indexStatus, worktreeStatus);
}

static bool IsStatusRenamed(const CString& line)
{
	return ((line[0]==_T('R')) || (line[1]==_T('R')));
}

static git_status_type MergeGitStatuses(const std::vector<CString>& statuses)
{
	git_status_type status = git_status_type_none;

	for(std::vector<CString>::const_iterator it=statuses.begin(); it!=statuses.end(); ++it) {

		git_status_type newStatus = git_status_type_none;
		
		try {
			newStatus = ParseStatus(*it);
			if(IsStatusRenamed(*it)) {
				// the string after a 'renamed' file represents an original filename
				++it;
			}
		} catch(std::invalid_argument&) {
			// do nothing
		}

		status = GetMoreImportantStatus(status, newStatus);
	}

	return status;
}

git_status_type GitRepositoryCommand::GetStatus(const CString& path) const
{
	std::vector<CString> statuses = GetStatusStrings(path);

	if(statuses.empty()) {
		if(IsEmptyDir(path)) {
			return git_status_type_untracked;
		} else {
			return git_status_type_unmodified;
		}
	}

	return MergeGitStatuses(statuses);
}

static CString EscapeCommand(CString str)
{
	str.Replace(_T("\\"), _T("/"));
	return str;
}

static CString BuildGitStatusCommand(const CString& relPath)
{
	CString command;
	command.Format(CString(_T("git.exe status --porcelain --ignored -z \"%s\"")), EscapeCommand(relPath));

	return command;
}

std::vector<CString> GitRepositoryCommand::GetStatusStrings(const CString& path) const
{
	assert(StartsWith(path, GetProjectRoot()) && "the path must be in a git repository");
	if(!StartsWith(path, GetProjectRoot())) {
		throw std::invalid_argument("the path must be in a git repository");
	}

	CString command = BuildGitStatusCommand(GetRelativePath(path));

	std::vector<CString> ret;
	if(Run(command, ret)) {
		return std::vector<CString>();
	}

	return ret;
}

int GitRepositoryCommand::Run(const CString& command, std::vector<CString>& out) const
{
	CommandRunner runner;
	return runner.Run(command, GetProjectRoot(), GetEncoding(), out);
}

bool GitRepositoryCommand::IsEmptyDir(const CString& path) const
{
	return (PathIsDirectoryEmpty(path)!=FALSE);
}
