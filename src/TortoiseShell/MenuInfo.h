#pragma once

enum GitCommands
{
		ShellSeparator = 0,
		ShellSubMenu = 1,
		ShellSubMenuFolder,
		ShellSubMenuFile,
		ShellSubMenuLink,
		ShellSubMenuMultiple,
		ShellMenuCheckout,
		ShellMenuUpdate,
		ShellMenuCommit,
		ShellMenuAdd,
		ShellMenuAddAsReplacement,
		ShellMenuRevert,
		ShellMenuCleanup,
		ShellMenuResolve,
		ShellMenuSwitch,
		ShellMenuImport,
		ShellMenuExport,
		ShellMenuAbout,
		ShellMenuCreateRepos,
		ShellMenuCopy,
		ShellMenuMerge,
		ShellMenuMergeAll,
		ShellMenuSettings,
		ShellMenuRemove,
		ShellMenuRemoveKeep,
		ShellMenuRename,
		ShellMenuUpdateExt,
		ShellMenuDiff,
		ShellMenuPrevDiff,
		ShellMenuUrlDiff,
		ShellMenuDiffTwo,
		ShellMenuDropCopyAdd,
		ShellMenuDropMoveAdd,
		ShellMenuDropMove,
		ShellMenuDropMoveRename,
		ShellMenuDropCopy,
		ShellMenuDropCopyRename,
		ShellMenuDropExport,
		ShellMenuDropExportExtended,
		ShellMenuLog,
		ShellMenuConflictEditor,
		ShellMenuRelocate,
		ShellMenuHelp,
		ShellMenuShowChanged,
		ShellMenuIgnoreSub,
		ShellMenuIgnore,
		ShellMenuIgnoreFile,
		ShellMenuIgnoreCaseSensitive,
		ShellMenuIgnoreCaseInsensitive,
		ShellMenuRefLog,
		ShellMenuRefBrowse,
		ShellMenuBlame,
		ShellMenuApplyPatch,
		ShellMenuCreatePatch,
		ShellMenuRevisionGraph,
		ShellMenuUnIgnoreSub,
		ShellMenuUnIgnoreCaseSensitive,
		ShellMenuUnIgnore,
//		ShellMenuLock,
//		ShellMenuUnlock,
//		ShellMenuUnlockForce,
		ShellMenuProperties,
		ShellMenuDelUnversioned,
		ShellMenuClipPaste,
		ShellMenuPull,
		ShellMenuPush,
		ShellMenuClone,
		ShellMenuBranch,
		ShellMenuTag,
		ShellMenuFormatPatch,
		ShellMenuImportPatch,
		ShellMenuCherryPick,
		ShellMenuFetch,
		ShellMenuRebase,
		ShellMenuStashSave,
		ShellMenuStashApply,
		ShellMenuStashList,
		ShellMenuStashPop,
		ShellMenuSubAdd,
		ShellMenuSubSync,
		ShellMenuSendMail,
		ShellMenuGitSVNRebase,
		ShellMenuGitSVNDCommit,
		ShellMenuGitSVNIgnore,      //import svn ignore
		ShellMenuSync,
		ShellMenuLastEntry			// used to mark the menu array end
};

	// helper struct for context menu entries
typedef struct MenuInfo
{
		GitCommands			command;		///< the command which gets executed for this menu entry
		unsigned __int64	menuID;			///< the menu ID to recognize the entry. NULL if it shouldn't be added to the context menu automatically
		UINT				iconID;			///< the icon to show for the menu entry
		UINT				menuTextID;		///< the text of the menu entry
		UINT				menuDescID;		///< the description text for the menu entry
		/// the following 8 params are for checking whether the menu entry should
		/// be added automatically, based on states of the selected item(s).
		/// The 'yes' states must be set, the 'no' states must not be set
		/// the four pairs are OR'ed together, the 'yes'/'no' states are AND'ed together.
		DWORD				firstyes;
		DWORD				firstno;
		DWORD				secondyes;
		DWORD				secondno;
		DWORD				thirdyes;
		DWORD				thirdno;
		DWORD				fourthyes;
		DWORD				fourthno;
};