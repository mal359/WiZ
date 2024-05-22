/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "wiz.h"
#include "mdichild.h"
//MW #include "helpids.h"
/* For a some reason I haven't bothered to try to figure out
 * shellapi.h must follow wiz.h
 */
#include <shellapi.h>

char szTotalsLine[80];              /* text for totals of zip archive */

BOOL fUpdateEntries, bSizing = FALSE, bMouseButtonDown = FALSE;
OPENFILENAME ofnTemp; /* Open file name structure for edit box save and open */
HWND hPatternSelectDlg; /* pattern select modeless dialog   */
UINT uCommDlgHelpMsg;   /* common dialog help message ID */
//MW DWORD dwCommDlgHelpId = HELPID_HELP; /* what to pass to WinHelp() */
BOOL move_flag = FALSE;
BOOL rename_flag = FALSE;
//MW DWORD dwHelpContextId;
LPSTR lpchLast;

/* size of char in SYSTEM font in pixels */
long dxChar, dyChar;

// Main window message table definition.
MSD rgmsd[] =
{
    {WM_SYSCOMMAND,           MsgSysCommand},
    {WM_SETCURSOR,            MsgSetCursor },
//MW    {WM_INITMENU,             MsgInitMenu  },
//MW    {WM_ENTERIDLE,            MsgEnterIdle },
    {WM_CLOSE,                MsgClose     },
    {WM_DROPFILES,            MsgDropfiles },

    {WM_CREATE,               MsgCreate    },
    {WM_SIZE,                 MsgSize      },
    {WM_WINDOWPOSCHANGING,    MsgSize},
    {WM_WINDOWPOSCHANGED,     MsgSize},
    {WM_TIMER,                MsgTimer     },
    {WM_MENUSELECT,           MsgMenuSelect},
    {WM_RBUTTONUP,            MsgRButtonUp },
    {WM_SETFOCUS,             MsgMCSetFocus},
    {WM_COMMAND,              MsgCommand   },
    {WM_NOTIFY,               MsgNotify    },
    {WM_ACTIVATE,             MsgActivate  },
    {WM_DESTROY,              MsgDestroy   },
};

MSDI msdiMain =
{
    sizeof(rgmsd) / sizeof(MSD),
    rgmsd,
    edwpWindow
};


// Main window command table definition.
CMD rgcmd[] =
{
    {IDM_EXPLORER,              CmdCommand   },
    {IDM_TOGGLE_TABS,           CmdCommand   },
    {IDM_WIZDIFF,               CmdCommand   },
    {IDM_DIFF_ARCHIVE_CONTENTS, CmdCommand   },
    {IDM_SCAN_ARCHIVE,          CmdCommand   },
    {IDM_CLEAR_ARCHIVEBOX,      CmdCommand   },
    {IDM_CLEAR_GREPBOX,         CmdCommand   },
    {IDM_SEARCH_ARCHIVE,        CmdCommand   },
    {IDM_ZIP_TARGET,            CmdCommand   },
    {IDM_ZIP_TARGET_CURRENT,    CmdCommand   },
    {IDM_PREFERENCES,           CmdCommand   },
    {IDM_OPEN,                  CmdCommand   },
    {IDM_CHDIR,                 CmdCommand   },
    {IDM_DELETE_ARCHIVE,        CmdCommand   },
#if 0
    {IDM_CANCEL_OPERATION,      CmdCommand   },
    {IDM_MOVE_ARCHIVE,          CmdCommand   },
    {IDM_RENAME_ARCHIVE,        CmdCommand   },
    {IDM_COPY_ARCHIVE,          CmdCommand   },
#endif
    {IDM_COPY_ARCHIVE,          CmdCommand   },
    {IDM_EXIT,                  CmdCommand   },
    {IDM_HELP,                  CmdCommand   },
//MW    {IDM_SHIFT_HELP,            CmdCommand   },
    {IDM_ESCAPE,                CmdCommand   },
    {IDM_HELP_HELP,             CmdCommand   },
    {IDM_ABOUT,                 CmdCommand   },
    {IDM_SHOW_COMMENT,          CmdCommand   },
    {IDM_GET_ZIPINFO,           CmdCommand   },
    {IDM_UPDATE_ZIP,            CmdCommand   },
    {IDM_DISPLAY,               CmdCommand   },
    {IDM_TEST,                  CmdCommand   },
    {IDM_EXTRACT,               CmdCommand   },
    {IDM_ZIP_DELETE_ENTRIES,    CmdCommand   },
    {IDM_SELECT_ALL,            CmdCommand   },
    {IDM_DESELECT_ALL,          CmdCommand   },
    {IDM_SELECT_BY_PATTERN,     CmdCommand   },
    {IDM_CLEAR_STATUS,          CmdCommand   },
    {IDM_NOTIFY_CLEARING,       CmdCommand   },
    {IDM_EXTERNAL_RTF,          CmdCommand   },
    {IDM_DIFF_ARCHIVE,          CmdCommand   },
    {IDM_GREP_ARCHIVE_CONTENTS, CmdCommand   },
    {IDM_GREP_ARCHIVE,          CmdCommand   },
    {IDM_WINDOWTILEHORIZONTAL,  CmdWindowTileHorizontal},
    {IDM_WINDOWTILEVERTICAL,    CmdWindowTileVertical},
    {IDM_WINDOWCASCADE,         CmdWindowCascade},
    {IDM_WINDOWICONS,           CmdWindowIcons},
    {IDM_WINDOWCLOSEALL,        CmdWindowCloseAll},
};

CMDI cmdiMain =
{
    sizeof(rgcmd) / sizeof(CMD),
    rgcmd,
    edwpMDIFrame
};




int MapCursorToListItem(HWND hWnd)
{
DWORD          dwpos;
LV_HITTESTINFO lvhti;
UINT           state;
LV_ITEM        lvi;
int            iItemClicked;
if (ListView_GetItemCount(hWnd))
   {
   /* Find out where the cursor was */
   dwpos = GetMessagePos();
   lvhti.pt.x = LOWORD(dwpos);
   lvhti.pt.y = HIWORD(dwpos);

   MapWindowPoints(HWND_DESKTOP, hWnd, &lvhti.pt, 1);
   state = lvhti.pt.x;
   lvhti.pt.x = 0;
   /* Now do a hittest with this point. We keep testing so we can
      find out if the mouse click was anywhere on the row.
    */
   while (((iItemClicked = ListView_HitTest(hWnd, &lvhti)) < 0) &&
         (lvhti.pt.x < (int)state))
         {
         lvhti.pt.x++;
         }

   /* Okay, have we clicked on a live item? */
   if (lvhti.flags & LVHT_ONITEM)
      {
      lvi.mask      = LVIF_STATE;
      lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
      lvi.iItem     = iItemClicked;
      lvi.iSubItem  = 0;

      ListView_GetItem(hWnd, &lvi);

      lvi.state = LVIS_SELECTED | LVIS_FOCUSED;

      ListView_SetItem(hWnd, &lvi);
      // tell caller what item was clicked on
      return iItemClicked;
      }
   }
return -1;
}

LPSTR lstrrchr(LPSTR lpszSrc, char chFind)
{
LPSTR   lpszFound = (LPSTR)0;
LPSTR   lpszT;

if ( lpszSrc )
   {
   for (lpszT = lpszSrc; *lpszT; ++lpszT)
       {
       if ((*lpszT) == chFind)
          lpszFound = lpszT;
       }
   }
return lpszFound;
}

/* Copy only the path portion of current file name into
 * given buffer, lpszDestDir, translate into ANSI.
 */
void GetArchiveDir(LPSTR lpszDestDir)
{
LPSTR lpchLast;

/* strip off filename to make directory name */
if ((lpchLast = lstrrchr(lpszDestDir, '\\'))!=0)
   *lpchLast = '\0';
else if ((lpchLast = lstrrchr(lpszDestDir, ':'))!=0)
   *(++lpchLast) = '\0'; /* clobber char AFTER the colon! */
}

void GetDirectory(LPSTR lpDir)
{
LPSTR lpchLast;
/* If no '\\' then set directory name to "" */
if ((lpchLast = lstrrchr(lpDir, '\\')) == 0)
   {
   lpDir[0] = '\0';
   return;
   }
/* strip off filename to make directory name    */
if ((lpchLast = lstrrchr(lpDir, '\\'))!=0)
   *lpchLast = '\0';
else if ((lpchLast = lstrrchr(lpDir, ':'))!=0)
   *(++lpchLast) = '\0'; /* clobber char AFTER the colon! */
}

/*
 * FUNCTION: wizWndMainProc(HWND, unsigned, WORD, LONG)
 *
 * PURPOSE:  Processes messages
 *
 */

LRESULT WINAPI WizMainWndProc(HWND hWnd, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
return DispMessage(&msdiMain, hWnd, wMessage, wParam, lParam);
}
/* MW
void GetHelpContext(WPARAM wParam)
{
switch (LOWORD(wParam))
   {
   case IDM_ESCAPE:
      if (uf.fHelp)
         {
         uf.fHelp = FALSE;
         SetCursor((HCURSOR)GetClassLong(hWndMain,GCL_HCURSOR));
         dwHelpContextId = (DWORD) 0L;
         }
      break;
   case IDM_OPEN:
      dwHelpContextId = (DWORD) HELPID_OPEN;
      break;
   case IDM_EXIT:
      dwHelpContextId = (DWORD) HELPID_EXIT_CMD;
      break;
   case IDM_HELP:
      dwHelpContextId = (DWORD) HELPID_HELP;
      break;
   case IDM_HELP_HELP:
      dwHelpContextId = (DWORD) HELPID_HELP_HELP;
      break;
   case IDM_ABOUT:
      dwHelpContextId = (DWORD) HELPID_ABOUT;
      break;
   case IDM_RECR_DIR_STRUCT:
      dwHelpContextId = (DWORD) HELPID_RECR_DIR_STRUCT;
      break;
   case IDM_EXTRACT:
      dwHelpContextId = (DWORD) HELPID_EXTRACT;
      break;
   case IDM_DISPLAY:
      dwHelpContextId = (DWORD) HELPID_DISPLAY;
      break;
   case IDM_TEST:
      dwHelpContextId = (DWORD) HELPID_TEST;
      break;
   case IDM_DESELECT_ALL:
      dwHelpContextId = (DWORD) HELPID_DESELECT_ALL;
      break;
   case IDM_SELECT_ALL:
      dwHelpContextId = (DWORD) HELPID_SELECT_ALL;
      break;
   case IDM_SELECT_BY_PATTERN:
      dwHelpContextId = (DWORD) HELPID_SELECT_BY_PATTERN;
      break;
   case IDM_CLEAR_STATUS:
      dwHelpContextId = (DWORD) HELPID_CLEAR_STATUS;
      break;
   case IDM_CHDIR:
      dwHelpContextId = (DWORD) HELPID_CHDIR;
      break;
   case IDM_COPY_ARCHIVE:
      dwHelpContextId = (DWORD) HELPID_COPY_ARCHIVE;
      break;
   case IDM_MOVE_ARCHIVE:
      dwHelpContextId = (DWORD) HELPID_MOVE_ARCHIVE;
      break;
   case IDM_DELETE_ARCHIVE:
      dwHelpContextId = (DWORD) HELPID_DELETE_ARCHIVE;
      break;
   case IDM_RENAME_ARCHIVE:
      dwHelpContextId = (DWORD) HELPID_RENAME_ARCHIVE;
      break;
   default:
      dwHelpContextId = (DWORD) HELPID_BUTTONS;
   }
}
*/
//
//  FUNCTION: MsgCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_COMMAND messages for the main window.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - WM_COMMAND (Unused)
//    GET_WM_COMMAND_ID(wparam, lparam)   - Command identifier
//    GET_WM_COMMAND_HWND(wparam, lparam) - Control handle
//
//  RETURN VALUE:
//    The return value depends on the message number.  If the message
//    is implemented in the message dispatch table, the return value is
//    the value returned by the message handling function.  Otherwise,
//    the return value is the value returned by the default window procedure.
//
//  COMMENTS:
//    Call the DispCommand() function with the main window's command dispatch
//    information (cmdiMain) and the command specific information.
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
return DispCommand(&cmdiMain, hwnd, wparam, lparam);
}


//
//  FUNCTION: MsgCreate(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_CREATE messages for the main window.
//           and call InitCommonControls() API to initialize the
//           common control library. 
//
//  PARAMETERS:
//    hwnd     - window handle
//
//  RETURN VALUE:
//    Return 0 if the StatusBar and ToolBar Windows could be created
//    successfully. Otherwise, returns -1 to abort the main window
//    creation.
//
//  COMMENTS:
//    Call the CreateTSBars function with the main window's window handle
//    information (msdiMain).
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
return CmdCreate(hwnd, (WORD)uMessage, (WORD)wparam, (HWND)lparam);
}


LRESULT MsgSize(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
return CmdSize(hwnd, (WORD)uMessage, (WORD)wparam, (HWND)lparam);
}


//
//  FUNCTION: MsgDestroy(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Calls PostQuitMessage().
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgDestroy(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
return CmdDestroy(hwnd, (WORD)uMessage, (WORD)wparam, (HWND)lparam);
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgRButtonUp(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
/*
 PURPOSE: Display and track popup menu on button click.

 PARAMETERS:
    hWnd      - Window handle
    lparam    - Coordinates where the mouse was pressed

    Always returns 0 - Message handled
 */
HMENU hmenu;
HMENU hmenuTrackPopup;
RECT  rc;
BOOL fButtonState;
POINT pt;
DWORD dwpos;

/* Draw the appropriate "floating" popup in the client area
   Check the listbox first
 */
GetWindowRect(hWndListParent, &rc);
dwpos = GetMessagePos();
pt.x = LOWORD(dwpos);
pt.y = HIWORD(dwpos);
if (PtInRect(&rc, pt))
   {
   /* Get the menu for the popup from the resource file. */
   hmenu = LoadMenu(hInst, "PopupListMenu");
   if (!hmenu)
      return 0;

   /* Get the first menu in it which we will use for the call to
    * TrackPopup(). This could also be created on the fly using
    * CreatePopupMenu and then using InsertMenu() or
    * AppendMenu.
    */
   hmenuTrackPopup = GetSubMenu(hmenu, 0);
   if (lpumb->szFileName[0] &&
      ListView_GetSelectedCount(GetArchiveWindow(NULL)))
      fButtonState = TRUE;
   else
      fButtonState = FALSE;

   EnableMenuItem(hmenuTrackPopup, IDM_EXTRACT,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_DISPLAY,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_TEST,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_SHOW_COMMENT,
      (BOOL)(fButtonState && lpUserFunctions->cchComment ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_GET_ZIPINFO,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_UPDATE_ZIP,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_ZIP_DELETE_ENTRIES,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);

   if (lpumb->szFileName[0] != '\0')
      fButtonState = TRUE;
   else
      fButtonState = FALSE;
   EnableMenuItem(hmenuTrackPopup, IDM_COPY_ARCHIVE,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
#if 0
   EnableMenuItem(hmenuTrackPopup, IDM_MOVE_ARCHIVE,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_RENAME_ARCHIVE,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
#endif      
   EnableMenuItem(hmenuTrackPopup, IDM_DELETE_ARCHIVE,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_SELECT_ALL,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_DESELECT_ALL,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
   EnableMenuItem(hmenuTrackPopup, IDM_SELECT_BY_PATTERN,
      (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);

   /* Draw and track the "floating" popup */
   TrackPopupMenuEx(hmenuTrackPopup,
                 TPM_LEFTALIGN | TPM_TOPALIGN |     /* default values */
                 TPM_LEFTBUTTON | TPM_HORIZONTAL |  /* equivalent to 0 */
                 TPM_RIGHTBUTTON,                   /* Right selection */
                 pt.x, pt.y,
                 hWnd,
                 NULL);
   /* Destroy the menu since we are done with it. */
   DestroyMenu(hmenu);
   }
return 0;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgActivate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
return CmdActivate(hwnd, (WORD)uMessage, (WORD)wparam, (HWND)lparam);
}

