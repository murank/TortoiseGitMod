// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2012 - TortoiseGit
// Copyright (C) 2005-2007 Marco Costalba

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
// GitLogList.cpp : implementation file
//
#include "stdafx.h"
#include "TortoiseProc.h"
#include "GitLogList.h"
#include "GitRev.h"
//#include "VssStyle.h"
#include "IconMenu.h"
// CGitLogList
#include "cursor.h"
#include "InputDlg.h"
#include "GITProgressDlg.h"
#include "ProgressDlg.h"
#include "SysProgressDlg.h"
//#include "RepositoryBrowser.h"
//#include "CopyDlg.h"
//#include "StatGraphDlg.h"
#include "Logdlg.h"
#include "MessageBox.h"
#include "Registry.h"
#include "AppUtils.h"
#include "PathUtils.h"
#include "StringUtils.h"
#include "UnicodeUtils.h"
#include "TempFile.h"
//#include "GitInfo.h"
//#include "GitDiff.h"
//#include "RevisionRangeDlg.h"
//#include "BrowseFolder.h"
//#include "BlameDlg.h"
//#include "Blame.h"
//#include "GitHelpers.h"
#include "GitStatus.h"
//#include "LogDlgHelper.h"
//#include "CachedLogInfo.h"
//#include "RepositoryInfo.h"
//#include "EditPropertiesDlg.h"
#include "FileDiffDlg.h"
#include "CommitDlg.h"
#include "RebaseDlg.h"
#include "GitDiff.h"
#include "../TGitCache/CacheInterface.h"

IMPLEMENT_DYNAMIC(CGitLogList, CHintListCtrl)

int CGitLogList::RevertSelectedCommits()
{
	CSysProgressDlg progress;
	int ret = -1;

#if 0
	if(!g_Git.CheckCleanWorkTree())
	{
		CMessageBox::Show(NULL,_T("Revert requires a clean working tree"),_T("TortoiseGit"),MB_OK);

	}
#endif

	if (progress.IsValid() && (this->GetSelectedCount() > 1) )
	{
		progress.SetTitle(_T("Revert Commit"));
		progress.SetAnimation(IDR_MOVEANI);
		progress.SetTime(true);
		progress.ShowModeless(this);
	}

	CBlockCacheForPath cacheBlock(g_Git.m_CurrentDir);

	POSITION pos = GetFirstSelectedItemPosition();
	int i=0;
	while(pos)
	{
		int index = GetNextSelectedItem(pos);
		GitRev * r1 = reinterpret_cast<GitRev*>(m_arShownList.GetAt(index));

		if (progress.IsValid() && (this->GetSelectedCount() > 1) )
		{
			progress.FormatPathLine(1, _T("Revert %s"), r1->m_CommitHash.ToString());
			progress.FormatPathLine(2, _T("%s"),        r1->GetSubject());
			progress.SetProgress(i,         this->GetSelectedCount());
		}
		i++;

		if(r1->m_CommitHash.IsEmpty())
			continue;

		CString cmd, output;
		cmd.Format(_T("git.exe revert --no-edit --no-commit %s"), r1->m_CommitHash.ToString());
		if(g_Git.Run(cmd, &output, CP_ACP))
		{
			CString str;
			str=_T("Revert fail\n");
			str+= cmd;
			str+= _T("\n")+output;
			if( GetSelectedCount() == 1)
				CMessageBox::Show(NULL,str, _T("TortoiseGit"),MB_OK|MB_ICONERROR);
			else
			{
				if(CMessageBox::Show(NULL, str, _T("TortoiseGit"),2 , IDI_ERROR, _T("&Skip"), _T("&Abort")) == 2)
				{
					return ret;
				}
			}
		}
		else
		{
			ret =0;
		}

		if ((progress.IsValid())&&(progress.HasUserCancelled()))
			break;
	}
	return ret;
}
int CGitLogList::CherryPickFrom(CString from, CString to)
{
	CLogDataVector logs(&m_LogCache);
	if(logs.ParserFromLog(NULL,-1,0,&from,&to))
		return -1;

	if(logs.size() == 0)
		return 0;

	CSysProgressDlg progress;
	if (progress.IsValid())
	{
		progress.SetTitle(_T("Cherry Pick"));
		progress.SetAnimation(IDR_MOVEANI);
		progress.SetTime(true);
		progress.ShowModeless(this);
	}

	CBlockCacheForPath cacheBlock(g_Git.m_CurrentDir);

	for(int i=logs.size()-1;i>=0;i--)
	{
		if (progress.IsValid())
		{
			progress.FormatPathLine(1, _T("Pick up %s"), logs.GetGitRevAt(i).m_CommitHash.ToString());
			progress.FormatPathLine(2, _T("%s"), logs.GetGitRevAt(i).GetSubject());
			progress.SetProgress(logs.size()-i, logs.size());
		}
		if ((progress.IsValid())&&(progress.HasUserCancelled()))
		{
			//CMessageBox::Show(hwndExplorer, IDS_SVN_USERCANCELLED, IDS_APPNAME, MB_ICONINFORMATION);
			throw std::exception(CUnicodeUtils::GetUTF8(CString(_T("User canceled\r\n\r\n"))));
			return -1;
		}
		CString cmd,out;
		cmd.Format(_T("git.exe cherry-pick %s"),logs.GetGitRevAt(i).m_CommitHash.ToString());
		out.Empty();
		if(g_Git.Run(cmd,&out,CP_UTF8))
		{
			throw std::exception(CUnicodeUtils::GetUTF8(CString(_T("Cherry Pick Failure\r\n\r\n"))+out));
			return -1;
		}
	}

	return 0;
}

void CGitLogList::ContextMenuAction(int cmd,int FirstSelect, int LastSelect, CMenu *popmenu)
{
	POSITION pos = GetFirstSelectedItemPosition();
	int indexNext = GetNextSelectedItem(pos);
	if (indexNext < 0)
		return;

	GitRev* pSelLogEntry = reinterpret_cast<GitRev*>(m_arShownList.GetAt(indexNext));

	theApp.DoWaitCursor(1);
	switch (cmd&0xFFFF)
		{
			case ID_COMMIT:
			{
				CTGitPathList pathlist;
				CTGitPathList selectedlist;
				pathlist.AddPath(this->m_Path);
				bool bSelectFilesForCommit = !!DWORD(CRegStdDWORD(_T("Software\\TortoiseGit\\SelectFilesForCommit"), TRUE));
				CString str;
				CAppUtils::Commit(CString(),false,str,
								  pathlist,selectedlist,bSelectFilesForCommit);
				//this->Refresh();
				this->GetParent()->PostMessage(WM_COMMAND,ID_LOGDLG_REFRESH,0);
			}
			break;
			case ID_GNUDIFF1: // compare with WC, unified
			{
				CString tempfile=GetTempFile();
				CString command;
				GitRev * r1 = reinterpret_cast<GitRev*>(m_arShownList.GetAt(FirstSelect));
				if(!r1->m_CommitHash.IsEmpty())
				{
					CString merge;
					CString hash2;
					cmd >>= 16;
					if( (cmd&0xFFFF) == 0xFFFF)
					{
						merge=_T("-m");
					}
					else if((cmd&0xFFFF) == 0xFFFE)
					{
						merge=_T("-c");
					}
					else
					{
						if(cmd > r1->m_ParentHash.size())
						{
							CString str;
							str.Format(_T("Parent %d does not exist"), cmd);
							CMessageBox::Show(NULL,str,_T("TortoiseGit"),MB_OK|MB_ICONERROR);
							return;
						}
						else
						{
							if(cmd>0)
								hash2 = r1->m_ParentHash[cmd-1].ToString();
						}
					}
					command.Format(_T("git.exe diff-tree %s -r -p --stat %s %s"), merge, hash2, r1->m_CommitHash.ToString());
				}
				else
					command.Format(_T("git.exe diff -r -p --stat"));

				g_Git.RunLogFile(command,tempfile);
				CAppUtils::StartUnifiedDiffViewer(tempfile,r1->m_CommitHash.ToString().Left(6)+_T(":")+r1->GetSubject());
			}
			break;

			case ID_GNUDIFF2: // compare two revisions, unified
			{
				CString tempfile=GetTempFile();
				CString cmd;
				GitRev * r1 = reinterpret_cast<GitRev*>(m_arShownList.GetAt(FirstSelect));
				GitRev * r2 = reinterpret_cast<GitRev*>(m_arShownList.GetAt(LastSelect));

				if( r1->m_CommitHash.IsEmpty()) {
					cmd.Format(_T("git.exe diff -r -p --stat %s"),r2->m_CommitHash.ToString());
				}
				else if( r2->m_CommitHash.IsEmpty()) {
					cmd.Format(_T("git.exe diff -r -p --stat %s"),r1->m_CommitHash.ToString());
				}
				else
				{
					cmd.Format(_T("git.exe diff-tree -r -p --stat %s %s"),r2->m_CommitHash.ToString(),r1->m_CommitHash.ToString());
				}

				g_Git.RunLogFile(cmd,tempfile);
				CAppUtils::StartUnifiedDiffViewer(tempfile,r2->m_CommitHash.ToString().Left(6)+_T(":")+r1->m_CommitHash.ToString().Left(6));

			}
			break;

		case ID_COMPARETWO: // compare two revisions
			{
				GitRev * r1 = reinterpret_cast<GitRev*>(m_arShownList.GetAt(FirstSelect));
				GitRev * r2 = reinterpret_cast<GitRev*>(m_arShownList.GetAt(LastSelect));
				CGitDiff::DiffCommit(this->m_Path, r1,r2);

			}
			break;

		case ID_COMPARE: // compare revision with WC
			{
				GitRev * r1 = &m_wcRev;
				GitRev * r2 = pSelLogEntry;

				CGitDiff::DiffCommit(this->m_Path, r1,r2);

				//user clicked on the menu item "compare with working copy"
				//if (PromptShown())
				//{
				//	GitDiff diff(this, m_hWnd, true);
				//	diff.SetAlternativeTool(!!(GetAsyncKeyState(VK_SHIFT) & 0x8000));
				//	diff.SetHEADPeg(m_LogRevision);
				//	diff.ShowCompare(m_path, GitRev::REV_WC, m_path, revSelected);
				//}
				//else
				//	CAppUtils::StartShowCompare(m_hWnd, m_path, GitRev::REV_WC, m_path, revSelected, GitRev(), m_LogRevision, !!(GetAsyncKeyState(VK_SHIFT) & 0x8000));
			}
			break;

		case ID_COMPAREWITHPREVIOUS:
			{

				CFileDiffDlg dlg;

				if(pSelLogEntry->m_ParentHash.size()>0)
				//if(m_logEntries.m_HashMap[pSelLogEntry->m_ParentHash[0]]>=0)
				{
					cmd>>=16;
					cmd&=0xFFFF;

					if(cmd == 0)
						cmd=1;

					CGitDiff::DiffCommit(this->m_Path, pSelLogEntry->m_CommitHash.ToString(),pSelLogEntry->m_ParentHash[cmd-1].ToString());

				}
				else
				{
					CMessageBox::Show(NULL,_T("No previous version"),_T("TortoiseGit"),MB_OK);
				}
				//if (PromptShown())
				//{
				//	GitDiff diff(this, m_hWnd, true);
				//	diff.SetAlternativeTool(!!(GetAsyncKeyState(VK_SHIFT) & 0x8000));
				//	diff.SetHEADPeg(m_LogRevision);
				//	diff.ShowCompare(CTGitPath(pathURL), revPrevious, CTGitPath(pathURL), revSelected);
				//}
				//else
				//	CAppUtils::StartShowCompare(m_hWnd, CTGitPath(pathURL), revPrevious, CTGitPath(pathURL), revSelected, GitRev(), m_LogRevision, !!(GetAsyncKeyState(VK_SHIFT) & 0x8000));
			}
			break;
		case ID_COPYCLIPBOARD:
			{
				CopySelectionToClipBoard();
			}
			break;
		case ID_COPYHASH:
			{
				CopySelectionToClipBoard(TRUE);
			}
			break;
		case ID_EXPORT:
			{
				CString str=pSelLogEntry->m_CommitHash.ToString();
				CAppUtils::Export(&str);
			}
			break;
		case ID_CREATE_BRANCH:
			{
				CString str = pSelLogEntry->m_CommitHash.ToString();
				CAppUtils::CreateBranchTag(FALSE,&str);
				ReloadHashMap();
				Invalidate();
			}
			break;
		case ID_CREATE_TAG:
			{
				CString str = pSelLogEntry->m_CommitHash.ToString();
				CAppUtils::CreateBranchTag(TRUE,&str);
				ReloadHashMap();
				Invalidate();
				::PostMessage(this->GetParent()->m_hWnd,MSG_REFLOG_CHANGED,0,0);
			}
			break;
		case ID_SWITCHTOREV:
			{
				CString str = pSelLogEntry->m_CommitHash.ToString();
				CAppUtils::Switch(&str);
			}
			ReloadHashMap();
			Invalidate();
			::PostMessage(this->GetParent()->m_hWnd,MSG_REFLOG_CHANGED,0,0);
			break;
		case ID_SWITCHBRANCH:
			if(popmenu)
			{
				CString *branch = (CString*)((CIconMenu*)popmenu)->GetMenuItemData(cmd);
				if(branch)
				{
					CString name;
					if(branch->Find(_T("refs/heads/")) ==0 )
						name = branch->Mid(11);
					else
						name = *branch;
					
					CAppUtils::PerformSwitch(name);
				}
				ReloadHashMap();
				Invalidate();
				::PostMessage(this->GetParent()->m_hWnd,MSG_REFLOG_CHANGED,0,0);
			}
			break;
		case ID_RESET:
			{
				CString str = pSelLogEntry->m_CommitHash.ToString();
				CAppUtils::GitReset(&str);
				ReloadHashMap();
				Invalidate();
			}
			break;
		case ID_REBASE_PICK:
			SetSelectedAction(CTGitPath::LOGACTIONS_REBASE_PICK);
			break;
		case ID_REBASE_EDIT:
			SetSelectedAction(CTGitPath::LOGACTIONS_REBASE_EDIT);
			break;
		case ID_REBASE_SQUASH:
			SetSelectedAction(CTGitPath::LOGACTIONS_REBASE_SQUASH);
			break;
		case ID_REBASE_SKIP:
			SetSelectedAction(CTGitPath::LOGACTIONS_REBASE_SKIP);
			break;
		case ID_COMBINE_COMMIT:
		{
			CString head;
			CGitHash headhash;
			CGitHash hashFirst,hashLast;

			int headindex=GetHeadIndex();
			if(headindex>=0) //incase show all branch, head is not the first commits.
			{
				head.Format(_T("HEAD~%d"),FirstSelect-headindex);
				hashFirst=g_Git.GetHash(head);

				head.Format(_T("HEAD~%d"),LastSelect-headindex);
				hashLast=g_Git.GetHash(head);
			}

			GitRev* pFirstEntry = reinterpret_cast<GitRev*>(m_arShownList.GetAt(FirstSelect));
			GitRev* pLastEntry = reinterpret_cast<GitRev*>(m_arShownList.GetAt(LastSelect));
			if(pFirstEntry->m_CommitHash != hashFirst || pLastEntry->m_CommitHash != hashLast)
			{
				CMessageBox::Show(NULL,_T("Cannot combine commits now.\r\nMake sure you are viewing the log of your current branch and no filters are applied."),_T("TortoiseGit"),MB_OK);
				break;
			}

			headhash=g_Git.GetHash(_T("HEAD"));

			if(!g_Git.CheckCleanWorkTree())
			{
				CMessageBox::Show(NULL,_T("Combine needs a clean work tree"),_T("TortoiseGit"),MB_OK);
				break;
			}
			CString cmd,out;

			//Use throw to abort this process (reset back to original HEAD)
			try
			{
				cmd.Format(_T("git.exe reset --hard %s"),pFirstEntry->m_CommitHash.ToString());
				if(g_Git.Run(cmd,&out,CP_UTF8))
				{
					CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
					throw std::exception(CUnicodeUtils::GetUTF8(_T("Could not reset to first commit (first step) aborting...\r\n\r\n")+out));
				}
				cmd.Format(_T("git.exe reset --mixed %s"),hashLast.ToString());
				if(g_Git.Run(cmd,&out,CP_UTF8))
				{
					CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
					throw std::exception(CUnicodeUtils::GetUTF8(_T("Could not reset to last commit (second step) aborting...\r\n\r\n")+out));
				}

				CTGitPathList PathList;
				/* don't why must add --stat to get action status*/
				/* first -z will be omitted by gitdll*/
				if(g_Git.GetDiffPath(&PathList,&pFirstEntry->m_CommitHash,&hashLast,"-z --stat -r"))
				{
					CMessageBox::Show(NULL,_T("Get Diff file list error"),_T("TortoiseGit"),MB_OK);
					throw std::exception(CUnicodeUtils::GetUTF8(_T("Could not get changed file list aborting...\r\n\r\n")+out));
				}

				for(int i=0;i<PathList.GetCount();i++)
				{
					if(PathList[i].m_Action & CTGitPath::LOGACTIONS_ADDED)
					{
						cmd.Format(_T("git.exe add -- \"%s\""), PathList[i].GetGitPathString());
						if(g_Git.Run(cmd,&out,CP_ACP))
						{
							CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
							throw std::exception(CUnicodeUtils::GetUTF8(_T("Could not add new file aborting...\r\n\r\n")+out));

						}
					}
					if(PathList[i].m_Action & CTGitPath::LOGACTIONS_DELETED)
					{
						cmd.Format(_T("git.exe rm -- \"%s\""), PathList[i].GetGitPathString());
						if(g_Git.Run(cmd,&out,CP_ACP))
						{
							CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
							throw std::exception(CUnicodeUtils::GetUTF8(_T("Could not rm file aborting...\r\n\r\n")+out));
						}
					}
				}

				CCommitDlg dlg;
				for(int i=FirstSelect;i<=LastSelect;i++)
				{
					GitRev* pRev = reinterpret_cast<GitRev*>(m_arShownList.GetAt(i));
					dlg.m_sLogMessage+=pRev->GetSubject()+_T("\n")+pRev->GetBody();
					dlg.m_sLogMessage+=_T("\n");
				}
				dlg.m_bWholeProject=true;
				dlg.m_bSelectFilesForCommit = true;
				dlg.m_bCommitAmend=true;
				dlg.m_bNoPostActions=true;
				dlg.m_AmendStr=dlg.m_sLogMessage;

				if (dlg.DoModal() == IDOK)
				{
					if(pFirstEntry->m_CommitHash!=headhash)
					{
						//Commitrange firstEntry..headhash (from top of combine to original head) needs to be 'cherry-picked'
						//on top of new commit.
						//Use the rebase --onto command for it.
						//
						//All this can be done in one step using the following command:
						//cmd.Format(_T("git.exe format-patch --stdout --binary --full-index -k %s..%s | git am -k -3"),
						//	pFirstEntry->m_CommitHash,
						//	headhash);
						//But I am not sure if a '|' is going to work in a CreateProcess() call.
						//
						//Later the progress dialog could be used to execute these steps.

						if(CherryPickFrom(pFirstEntry->m_CommitHash.ToString(),headhash))
						{
							CString msg;
							msg.Format(_T("Error while cherry pick commits on top of combined commits. Aborting.\r\n\r\n"));
							throw std::exception(CUnicodeUtils::GetUTF8(msg));
						}
#if 0
						CString currentBranch=g_Git.GetCurrentBranch();
						cmd.Format(_T("git.exe rebase --onto \"%s\" %s %s"),
							currentBranch,
							pFirstEntry->m_CommitHash,
							headhash);
						if(g_Git.Run(cmd,&out,CP_UTF8)!=0)
						{
							CString msg;
							msg.Format(_T("Error while rebasing commits on top of combined commits. Aborting.\r\n\r\n%s"),out);
//							CMessageBox::Show(NULL,msg,_T("TortoiseGit"),MB_OK);
							g_Git.Run(_T("git.exe rebase --abort"),&out,CP_UTF8);
							throw std::exception(CUnicodeUtils::GetUTF8(msg));
						}

						//HEAD is now on <no branch>.
						//The following steps are to get HEAD back on the original branch and reset the branch to the new HEAD
						//To avoid 2 working copy changes, we could use git branch -f <original branch> <hash new head>
						//And then git checkout <original branch>
						//But I don't know if 'git branch -f' removes tracking options. So for now, do a checkout and a reset.

						//Store new HEAD
						CString newHead=g_Git.GetHash(CString(_T("HEAD")));

						//Checkout working branch
						cmd.Format(_T("git.exe checkout -f \"%s\""),currentBranch);
						if(g_Git.Run(cmd,&out,CP_UTF8))
							throw std::exception(CUnicodeUtils::GetUTF8(_T("Could not checkout original branch. Aborting...\r\n\r\n")+out));

						//Reset to new HEAD
						cmd.Format(_T("git.exe reset --hard  %s"),newHead);
						if(g_Git.Run(cmd,&out,CP_UTF8))
							throw std::exception(CUnicodeUtils::GetUTF8(_T("Could not reset to new head. Aborting...\r\n\r\n")+out));
#endif
					}
				}
				else
					throw std::exception("User aborted the combine process");
			}
			catch(std::exception& e)
			{
				CMessageBox::Show(NULL,CUnicodeUtils::GetUnicode(CStringA(e.what())),_T("TortoiseGit: Combine error"),MB_OK|MB_ICONERROR);
				cmd.Format(_T("git.exe reset --hard  %s"),headhash.ToString());
				out.Empty();
				if(g_Git.Run(cmd,&out,CP_UTF8))
				{
					CMessageBox::Show(NULL,_T("Could not reset to original HEAD\r\n\r\n")+out,_T("TortoiseGit"),MB_OK);
				}
			}
			Refresh();
		}
			break;

		case ID_CHERRY_PICK:
			if(!g_Git.CheckCleanWorkTree())
			{
				CMessageBox::Show(NULL,_T("Cherry Pick requires a clean working tree"),_T("TortoiseGit"),MB_OK);

			}
			else
			{
				CRebaseDlg dlg;
				dlg.m_IsCherryPick = TRUE;
				dlg.m_Upstream = this->m_CurrentBranch;
				POSITION pos = GetFirstSelectedItemPosition();
				while(pos)
				{
					int indexNext = GetNextSelectedItem(pos);
					dlg.m_CommitList.m_logEntries.push_back( ((GitRev*)m_arShownList[indexNext])->m_CommitHash );
					dlg.m_CommitList.m_LogCache.m_HashMap[((GitRev*)m_arShownList[indexNext])->m_CommitHash]=*(GitRev*)m_arShownList[indexNext];
					dlg.m_CommitList.m_logEntries.GetGitRevAt(dlg.m_CommitList.m_logEntries.size()-1).GetAction(this) |= CTGitPath::LOGACTIONS_REBASE_PICK;
				}

				if(dlg.DoModal() == IDOK)
				{
					Refresh();
				}
			}
			break;
		case ID_REBASE_TO_VERSION:
			if(!g_Git.CheckCleanWorkTree())
			{
				CMessageBox::Show(NULL,_T("Rebase requires a clean working tree"),_T("TortoiseGit"),MB_OK);

			}
			else
			{
				CRebaseDlg dlg;
				dlg.m_Upstream = pSelLogEntry->m_CommitHash;

				if(dlg.DoModal() == IDOK)
				{
					Refresh();
				}
			}

			break;

		case ID_STASH_SAVE:
			if (CAppUtils::StashSave())
				Refresh();
			break;

		case ID_STASH_POP:
			if (CAppUtils::StashPop())
				Refresh();
			break;

		case ID_STASH_LIST:
			CAppUtils::RunTortoiseProc(_T("/command:reflog /ref:refs/stash"));
			break;

		case ID_REFLOG_STASH_APPLY:
			CAppUtils::StashApply(pSelLogEntry->m_Ref);
			break;

		case ID_REFLOG_DEL:
			{
				CString str;
				if (GetSelectedCount() > 1)
					str.Format(_T("Do you really want to permanently delete the %d selected refs? It can <ct=0x0000FF><b>NOT</b></ct> be recovered!"), GetSelectedCount());
				else
					str.Format(_T("Warning: \"%s\" will be permanently deleted. It can <ct=0x0000FF><b>NOT</b></ct> be recovered!\r\n\r\nDo you really want to continue?"), pSelLogEntry->m_Ref);

				if (CMessageBox::Show(NULL, str, _T("TortoiseGit"), 1, IDI_QUESTION, _T("&Delete"), _T("&Abort")) == 2)
					return;

				POSITION pos = GetFirstSelectedItemPosition();
				while (pos)
				{
					CString ref = ((GitRev *)m_arShownList[GetNextSelectedItem(pos)])->m_Ref;
					if (ref.Find(_T("refs/")) == 0)
						ref = ref.Mid(5);
					int refpos = ref.ReverseFind('{');
					if (refpos > 0 && ref.Mid(refpos, 2) != _T("@{"))
						ref = ref.Left(refpos) + _T("@")+ ref.Mid(refpos);

					CString cmd, out;
					if (ref.Find(_T("stash")) == 0)
						cmd.Format(_T("git.exe stash drop %s"), ref);
					else
						cmd.Format(_T("git.exe reflog delete %s"), ref);

					if(g_Git.Run(cmd,&out,CP_ACP))
						CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);

					::PostMessage(this->GetParent()->m_hWnd,MSG_REFLOG_CHANGED,0,0);
				}
			}
			break;
		case ID_LOG:
			{
				CString cmd = _T("/command:log");
				cmd += _T(" /path:\"")+g_Git.m_CurrentDir+_T("\" ");
				GitRev * r1 = reinterpret_cast<GitRev*>(m_arShownList.GetAt(FirstSelect));
				cmd += _T(" /endrev:")+r1->m_CommitHash.ToString();
				CAppUtils::RunTortoiseProc(cmd);
			}
			break;
		case ID_CREATE_PATCH:
			{
				int select=this->GetSelectedCount();
				CString cmd = _T("/command:formatpatch");
				cmd += _T(" /path:\"")+g_Git.m_CurrentDir+_T("\" ");

				GitRev * r1 = reinterpret_cast<GitRev*>(m_arShownList.GetAt(FirstSelect));
				GitRev * r2 = NULL;
				if(select == 1)
				{
					cmd += _T(" /startrev:")+r1->m_CommitHash.ToString();
				}
				else
				{
					r2 = reinterpret_cast<GitRev*>(m_arShownList.GetAt(LastSelect));
					if( this->m_IsOldFirst )
					{
						cmd += _T(" /startrev:")+r1->m_CommitHash.ToString()+_T("~1");
						cmd += _T(" /endrev:")+r2->m_CommitHash.ToString();

					}
					else
					{
						cmd += _T(" /startrev:")+r2->m_CommitHash.ToString()+_T("~1");
						cmd += _T(" /endrev:")+r1->m_CommitHash.ToString();
					}

				}

				CAppUtils::RunTortoiseProc(cmd);
			}
			break;
		case ID_PUSH:
			{
				CString guessAssociatedBranch;
				if (m_HashMap[pSelLogEntry->m_CommitHash].size() > 0)
					guessAssociatedBranch = m_HashMap[pSelLogEntry->m_CommitHash].at(0);
				if (CAppUtils::Push(guessAssociatedBranch))
					Refresh();
			}
			break;
		case ID_DELETE:
			{
				CString *branch = (CString*)((CIconMenu*)popmenu)->GetMenuItemData(cmd);
				if (!branch)
				{
					CMessageBox::Show(NULL,IDS_ERROR_NOREF,IDS_APPNAME,MB_OK|MB_ICONERROR);
					return;
				}
				CString msg;
				msg=CString(_T("Do you really want to <ct=0x0000FF>delete</ct> <b>")) + *branch;
				msg+=_T("</b>?");
				if( CMessageBox::Show(NULL, msg, _T("TortoiseGit"), 2, IDI_QUESTION, _T("&Delete"), _T("&Abort")) == 1 )
				{
					CString shortname;
					CString cmd;
					if(this->GetShortName(*branch,shortname,_T("refs/heads/")))
					{
						cmd.Format(_T("git.exe branch -D -- %s"),shortname);
					}

					if(this->GetShortName(*branch,shortname,_T("refs/remotes/")))
					{
						cmd.Format(_T("git.exe branch -r -D -- %s"),shortname);
					}

					if(this->GetShortName(*branch,shortname,_T("refs/tags/")))
					{
						cmd.Format(_T("git.exe tag -d -- %s"),shortname);
					}

					if(this->GetShortName(*branch,shortname,_T("refs/stash")))
					{
						if(CMessageBox::Show(NULL, _T("<ct=0x0000FF>Do you really want to delete <b>ALL</b> stash?</ct>"),
											   _T("TortoiseGit"), 2, IDI_QUESTION, _T("&Delete"), _T("&Abort")) == 1)
							cmd.Format(_T("git.exe stash clear"));
						else
							return;
					}

					CString out;
					if(g_Git.Run(cmd,&out,CP_UTF8))
					{
						CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
					}
					this->ReloadHashMap();
					CRect rect;
					this->GetItemRect(FirstSelect,&rect,LVIR_BOUNDS);
					this->InvalidateRect(rect);
				}
			}
			break;

		case ID_FINDENTRY:
			{
				m_nSearchIndex = GetSelectionMark();
				if (m_nSearchIndex < 0)
					m_nSearchIndex = 0;
				if (m_pFindDialog)
				{
					break;
				}
				else
				{
					m_pFindDialog = new CFindDlg();
					m_pFindDialog->Create(this);
				}
			}
			break;
		case ID_MERGEREV:
			{
				CString str = pSelLogEntry->m_CommitHash.ToString();
				if (m_HashMap[pSelLogEntry->m_CommitHash].size() > 0)
					str = m_HashMap[pSelLogEntry->m_CommitHash].at(0);
				// we need an URL to complete this command, so error out if we can't get an URL
				if(CAppUtils::Merge(&str))
				{
					this->Refresh();
				}
			}
		break;
		case ID_REVERTREV:
			{
				if(!this->RevertSelectedCommits())
					this->Refresh();
			}
			break;
		case ID_EDITNOTE:
			{
				CAppUtils::EditNote(pSelLogEntry);
				this->SetItemState(FirstSelect,  0, LVIS_SELECTED);
				this->SetItemState(FirstSelect,  LVIS_SELECTED, LVIS_SELECTED);
			}
			break;
		default:
			//CMessageBox::Show(NULL,_T("Have not implemented"),_T("TortoiseGit"),MB_OK);
			break;
#if 0

		case ID_BLAMECOMPARE:
			{
				//user clicked on the menu item "compare with working copy"
				//now first get the revision which is selected
				if (PromptShown())
				{
					GitDiff diff(this, this->m_hWnd, true);
					diff.SetHEADPeg(m_LogRevision);
					diff.ShowCompare(m_path, GitRev::REV_BASE, m_path, revSelected, GitRev(), false, true);
				}
				else
					CAppUtils::StartShowCompare(m_hWnd, m_path, GitRev::REV_BASE, m_path, revSelected, GitRev(), m_LogRevision, false, false, true);
			}
			break;
		case ID_BLAMETWO:
			{
				//user clicked on the menu item "compare and blame revisions"
				if (PromptShown())
				{
					GitDiff diff(this, this->m_hWnd, true);
					diff.SetHEADPeg(m_LogRevision);
					diff.ShowCompare(CTGitPath(pathURL), revSelected2, CTGitPath(pathURL), revSelected, GitRev(), false, true);
				}
				else
					CAppUtils::StartShowCompare(m_hWnd, CTGitPath(pathURL), revSelected2, CTGitPath(pathURL), revSelected, GitRev(), m_LogRevision, false, false, true);
			}
			break;
		case ID_BLAMEWITHPREVIOUS:
			{
				//user clicked on the menu item "Compare and Blame with previous revision"
				if (PromptShown())
				{
					GitDiff diff(this, this->m_hWnd, true);
					diff.SetHEADPeg(m_LogRevision);
					diff.ShowCompare(CTGitPath(pathURL), revPrevious, CTGitPath(pathURL), revSelected, GitRev(), false, true);
				}
				else
					CAppUtils::StartShowCompare(m_hWnd, CTGitPath(pathURL), revPrevious, CTGitPath(pathURL), revSelected, GitRev(), m_LogRevision, false, false, true);
			}
			break;

		case ID_OPENWITH:
			bOpenWith = true;
		case ID_OPEN:
			{
				CProgressDlg progDlg;
				progDlg.SetTitle(IDS_APPNAME);
				progDlg.SetAnimation(IDR_DOWNLOAD);
				CString sInfoLine;
				sInfoLine.Format(IDS_PROGRESSGETFILEREVISION, m_path.GetWinPath(), (LPCTSTR)revSelected.ToString());
				progDlg.SetLine(1, sInfoLine, true);
				SetAndClearProgressInfo(&progDlg);
				progDlg.ShowModeless(m_hWnd);
				CTGitPath tempfile = CTempFiles::Instance().GetTempFilePath(false, m_path, revSelected);
				bool bSuccess = true;
				if (!Cat(m_path, GitRev(GitRev::REV_HEAD), revSelected, tempfile))
				{
					bSuccess = false;
					// try again, but with the selected revision as the peg revision
					if (!Cat(m_path, revSelected, revSelected, tempfile))
					{
						progDlg.Stop();
						SetAndClearProgressInfo((HWND)NULL);
						CMessageBox::Show(this->m_hWnd, GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
						EnableOKButton();
						break;
					}
					bSuccess = true;
				}
				if (bSuccess)
				{
					progDlg.Stop();
					SetAndClearProgressInfo((HWND)NULL);
					SetFileAttributes(tempfile.GetWinPath(), FILE_ATTRIBUTE_READONLY);
					int ret = 0;
					if (!bOpenWith)
						ret = (int)ShellExecute(this->m_hWnd, NULL, tempfile.GetWinPath(), NULL, NULL, SW_SHOWNORMAL);
					if ((ret <= HINSTANCE_ERROR)||bOpenWith)
					{
						CString cmd = _T("RUNDLL32 Shell32,OpenAs_RunDLL ");
						cmd += tempfile.GetWinPathString() + _T(" ");
						CAppUtils::LaunchApplication(cmd, NULL, false);
					}
				}
			}
			break;
		case ID_BLAME:
			{
				CBlameDlg dlg;
				dlg.EndRev = revSelected;
				if (dlg.DoModal() == IDOK)
				{
					CBlame blame;
					CString tempfile;
					CString logfile;
					tempfile = blame.BlameToTempFile(m_path, dlg.StartRev, dlg.EndRev, dlg.EndRev, logfile, _T(""), dlg.m_bIncludeMerge, TRUE, TRUE);
					if (!tempfile.IsEmpty())
					{
						if (dlg.m_bTextView)
						{
							//open the default text editor for the result file
							CAppUtils::StartTextViewer(tempfile);
						}
						else
						{
							CString sParams = _T("/path:\"") + m_path.GetGitPathString() + _T("\" ");
							if(!CAppUtils::LaunchTortoiseBlame(tempfile, logfile, CPathUtils::GetFileNameFromPath(m_path.GetFileOrDirectoryName()),sParams))
							{
								break;
							}
						}
					}
					else
					{
						CMessageBox::Show(this->m_hWnd, blame.GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
					}
				}
			}
			break;
		case ID_UPDATE:
			{
				CString sCmd;
				CString url = _T("tgit:")+pathURL;
				sCmd.Format(_T("%s /command:update /path:\"%s\" /rev:%ld"),
					(LPCTSTR)(CPathUtils::GetAppDirectory()+_T("TortoiseProc.exe")),
					(LPCTSTR)m_path.GetWinPath(), (LONG)revSelected);
				CAppUtils::LaunchApplication(sCmd, NULL, false);
			}
			break;

		case ID_EDITLOG:
			{
				EditLogMessage(selIndex);
			}
			break;
		case ID_EDITAUTHOR:
			{
				EditAuthor(selEntries);
			}
			break;
		case ID_REVPROPS:
			{
				CEditPropertiesDlg dlg;
				dlg.SetProjectProperties(&m_ProjectProperties);
				CTGitPathList escapedlist;
				dlg.SetPathList(CTGitPathList(CTGitPath(pathURL)));
				dlg.SetRevision(revSelected);
				dlg.RevProps(true);
				dlg.DoModal();
			}
			break;

		case ID_EXPORT:
			{
				CString sCmd;
				sCmd.Format(_T("%s /command:export /path:\"%s\" /revision:%ld"),
					(LPCTSTR)(CPathUtils::GetAppDirectory()+_T("TortoiseProc.exe")),
					(LPCTSTR)pathURL, (LONG)revSelected);
				CAppUtils::LaunchApplication(sCmd, NULL, false);
			}
			break;
		case ID_VIEWREV:
			{
				CString url = m_ProjectProperties.sWebViewerRev;
				url = GetAbsoluteUrlFromRelativeUrl(url);
				url.Replace(_T("%REVISION%"), revSelected.ToString());
				if (!url.IsEmpty())
					ShellExecute(this->m_hWnd, _T("open"), url, NULL, NULL, SW_SHOWDEFAULT);
			}
			break;
		case ID_VIEWPATHREV:
			{
				CString relurl = pathURL;
				CString sRoot = GetRepositoryRoot(CTGitPath(relurl));
				relurl = relurl.Mid(sRoot.GetLength());
				CString url = m_ProjectProperties.sWebViewerPathRev;
				url = GetAbsoluteUrlFromRelativeUrl(url);
				url.Replace(_T("%REVISION%"), revSelected.ToString());
				url.Replace(_T("%PATH%"), relurl);
				if (!url.IsEmpty())
					ShellExecute(this->m_hWnd, _T("open"), url, NULL, NULL, SW_SHOWDEFAULT);
			}
			break;
#endif

		} // switch (cmd)

		theApp.DoWaitCursor(-1);
}

void CGitLogList::SetSelectedAction(int action)
{
	POSITION pos = GetFirstSelectedItemPosition();
	int index;
	while(pos)
	{
		index = GetNextSelectedItem(pos);
		((GitRev*)m_arShownList[index])->GetAction(this) = action;
		CRect rect;
		this->GetItemRect(index,&rect,LVIR_BOUNDS);
		this->InvalidateRect(rect);

	}
}
void CGitLogList::ShiftSelectedAction()
{
	POSITION pos = GetFirstSelectedItemPosition();
	int index;
	while(pos)
	{
		index = GetNextSelectedItem(pos);
		int action = ((GitRev*)m_arShownList[index])->GetAction(this);
		switch (action)
		{
		case CTGitPath::LOGACTIONS_REBASE_PICK:
			action = CTGitPath::LOGACTIONS_REBASE_SKIP;
			break;
		case CTGitPath::LOGACTIONS_REBASE_SKIP:
			action= CTGitPath::LOGACTIONS_REBASE_EDIT;
			break;
		case CTGitPath::LOGACTIONS_REBASE_EDIT:
			action = CTGitPath::LOGACTIONS_REBASE_SQUASH;
			break;
		case CTGitPath::LOGACTIONS_REBASE_SQUASH:
			action= CTGitPath::LOGACTIONS_REBASE_PICK;
			break;
		}
		((GitRev*)m_arShownList[index])->GetAction(this) = action;
		CRect rect;
		this->GetItemRect(index, &rect, LVIR_BOUNDS);
		this->InvalidateRect(rect);
	}
}
