/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/
/*
 Copyright (C) 1997 Mike White
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "wiz.h"
#include <shlobj.h>
//MW #include "helpids.h"

#ifdef ZE_MEM
#undef ZE_MEM
#define ZE_MEM 4
#endif

#include "zip\ziperr.h"

/* For a some reason I haven't bothered to try to figure out
 * shellapi.h must follow wiz.h
 */
#include <shellapi.h>

ZCL  ZpZCL;
HINSTANCE hExplorer;
HWND hTabCtrl;
HWND hWndGrep;
HWND hWndArchivesFound;

BOOL fScanArchives = FALSE;
BOOL fDiffArchiveContents = FALSE;
BOOL bUseCurrentZip = FALSE;
BOOL fUseOldDirSelect = TRUE;

extern int iZipWarn;

TCHAR szHomeDir[512];   // For some strange reason, this needs to be
                        // larger than MAX_PATH, or GetCurrentDirectory
                        // does not return a value in the string under
                        // Windows 2000
HDC hDC;                /* device context       */
TEXTMETRIC    tm;       /* text metric structure    */
char drive;
char *ptr;

PROCESS_INFORMATION WizDiff_Process_Information;
STARTUPINFO StartupInfo;

BOOL GetUnzipDirectory()
{
char sz1[RESOURCE_STR_LEN];
char sz[PATH_MAX + 21];
size_t uDirNameLen;
BOOL retcode = TRUE;

memset(&lpumb->ofn, '\0', sizeof(OPENFILENAME)); /* initialize struct */
lpumb->ofn.lStructSize = sizeof(OPENFILENAME);
lpumb->ofn.hwndOwner = hWndMain;
lpumb->ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
lpumb->ofn.nFilterIndex = 1;
lstrcpy(lpumb->szUnzipToDirNameTmp, szUnzipToDirName); /* initialize */

uDirNameLen = strlen(lpumb->szUnzipToDirNameTmp);
/* If '\\' not at end of directory name, add it now.
 */
if (uDirNameLen > 0 && lpumb->szUnzipToDirNameTmp[uDirNameLen-1] != '\\')
   lstrcat(lpumb->szUnzipToDirNameTmp, "\\");

lstrcpy(lpumb->szUnzipToDirNameTmp, "WIZ~");//szTempName);
lpumb->ofn.lpstrFile = lpumb->szUnzipToDirNameTmp; /* result goes here! */
lpumb->ofn.nMaxFile = PATH_MAX;
lpumb->ofn.lpstrFileTitle = NULL;
lpumb->ofn.nMaxFileTitle = PATH_MAX; /* ignored ! */
lpumb->ofn.lpstrInitialDir = szUnzipToDirName;
lstrcpy(sz1, GetLoadString(IDS_UNZIP_TO));
lpumb->ofn.lpstrTitle = sz1;
lpumb->ofn.Flags = OFN_PATHMUSTEXIST | OFN_ENABLEHOOK |
                   OFN_HIDEREADONLY|OFN_EXPLORER|OFN_NOVALIDATE|OFN_NOCHANGEDIR;
lpumb->ofn.lpfnHook = (LPOFNHOOKPROC)SelectDirProc;
if (GetSaveFileName(&lpumb->ofn)) /* successfully got dir name ? */
   {
   ptr = strrchr(lpumb->ofn.lpstrFile, '\\');
   if (ptr != NULL)
      lpumb->ofn.lpstrFile[(int)(ptr - lpumb->ofn.lpstrFile)] = '\0';
   lstrcpy(szUnzipToDirName, lpumb->ofn.lpstrFile); /* save result */

   if (szUnzipToDirName[1] == ':')
      {
      drive = szUnzipToDirName[0];
      if (lstrlen(szUnzipToDirName) == 2)
         { /* We only have a drive letter and a colon */
         lstrcat(szUnzipToDirName, "\\");
         }
      }
   else
      drive = '\0';
   if ((toupper(drive) != 'A') &&
      (toupper(drive) != 'B'))
      {
      /* Always save last directory written to, if not
         drive A or B.
       */
      WriteZipOptionsProfile();
      }
   }
else /* either real error or canceled */
   {
   DWORD dwExtdError = CommDlgExtendedError(); /* debugging */

   if (dwExtdError != 0L) /* if not canceled then real error */
      {
      wsprintf (lpumb->szBuffer,
                GetLoadString(IDS_CHG_DIR_ERROR),
                dwExtdError);
      MessageBox (hWndMain,
                  lpumb->szBuffer,
                  szAppName,
                  MB_ICONINFORMATION | MB_OK);
      }
   retcode = FALSE;
   }
lstrcpy(sz, "Wiz extracts to: ");
lstrcat(sz, szUnzipToDirName);
SendMessage(hWndMain, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)sz);
return retcode;
}


#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdCreate(HWND hWnd, WORD wMessage, WORD wParam, HWND lParam)
{
LV_COLUMN lvc;
CLIENTCREATESTRUCT ccs = {0};
int       iCol;
char      szGrepText[5][20];
/* szGrepText= {"Name", "Archive", "Orig Size","  Date", "Time"}; */
PARAFORMAT pf;
WIN32_FIND_DATA fd;

lstrcpy(szGrepText[0], GetLoadString(IDS_NAME));
lstrcpy(szGrepText[1], GetLoadString(IDS_ARCHIVE_NAME));
lstrcpy(szGrepText[2], GetLoadString(IDS_ORIG_SIZE));
lstrcpy(szGrepText[3], GetLoadString(IDS_DATE));
lstrcpy(szGrepText[4], GetLoadString(IDS_TIME));
hListbox = ((LPCREATESTRUCT)lParam)->hInstance;
hAccTable = LoadAccelerators(hInst, "WizAccels");
hBrush = CreateSolidBrush(GetSysColor(BG_SYS_COLOR)); /* background */

hMenu = GetMenu(hWnd);

/* Get an hourglass cursor to use during file transfers */
hHourGlass = LoadCursor(0, IDC_WAIT);

hFixedFont = GetStockObject(SYSTEM_FIXED_FONT);
hDC = GetDC(hWnd);  /* get device context */
hOldFont = SelectObject(hDC, hFixedFont);
GetTextMetrics(hDC, &tm);
ReleaseDC(hWnd, hDC);
dxChar = tm.tmAveCharWidth;
dyChar = tm.tmHeight + tm.tmExternalLeading;

WinAssert(hWnd);
TabInit(hWnd);
WinAssert(hTabCtrl);

hWndGrep = CreateWindow(WC_LISTVIEW, NULL,
                        WS_CHILD | WS_DLGFRAME |
                        LVS_REPORT | LVS_SHOWSELALWAYS |
                        LVS_SORTASCENDING,
                        0, 0,
                        0, 0,
                        hWnd, (HMENU)IDM_GREPBOX,
                        (HANDLE)GetWindowLong (hWnd, GWL_HINSTANCE),
                        NULL);
WinAssert(hWndGrep);
SendMessage(hWndGrep, WM_SETFONT, (WPARAM)hFixedFont, FALSE);

/* Initialize the LV_COLUMN structure. */
lvc.mask    = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
lvc.fmt     = LVCFMT_LEFT;
lvc.cx      = 95;

for (iCol = 0; iCol < 5; iCol++)
    {
    lvc.pszText = szGrepText[iCol];
    lvc.iSubItem = iCol;
    if (ListView_InsertColumn(hWndGrep, iCol, &lvc) == -1)
       return FALSE;
    }

ListView_SetColumnWidth(hWndGrep, 0, MAKELPARAM((int)20*dxChar, 0));
ListView_SetColumnWidth(hWndGrep, 1, MAKELPARAM((int)30*dxChar, 0));
ListView_SetColumnWidth(hWndGrep, 2, MAKELPARAM((int)11*dxChar, 0));
ListView_SetColumnWidth(hWndGrep, 3, MAKELPARAM((int)10*dxChar, 0));
ListView_SetColumnWidth(hWndGrep, 4, MAKELPARAM((int)10*dxChar, 0));

hWndArchivesFound = CreateWindow(WC_LISTVIEW, NULL,
                        WS_CHILD | WS_DLGFRAME |
                        LVS_REPORT | LVS_SHOWSELALWAYS |
                        LVS_SORTASCENDING,
                        0, 0,
                        0, 0,
                        hWnd, (HMENU)IDM_ARCHIVEBOX,
                        (HANDLE)GetWindowLong (hWnd, GWL_HINSTANCE),
                        NULL);
                        
WinAssert(hWndArchivesFound);
SendMessage(hWndArchivesFound, WM_SETFONT, (WPARAM)hFixedFont, FALSE);

/* Initialize the LV_COLUMN structure. */
lvc.mask    = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
lvc.fmt     = LVCFMT_LEFT;
lvc.cx      = 95;

lvc.pszText = szGrepText[1];
lvc.iSubItem = 0;
if (ListView_InsertColumn(hWndArchivesFound, 0, &lvc) == -1)
   return FALSE;

ListView_SetColumnWidth(hWndArchivesFound, 0, MAKELPARAM((int)60*dxChar, 0));

hWndMessage = CreateWindow("RICHEDIT", NULL, WS_DLGFRAME | WS_THICKFRAME |
                            WS_CHILD |ES_MULTILINE|ECO_AUTOWORDSELECTION|
                            WS_VSCROLL|ES_NOHIDESEL|ES_AUTOVSCROLL|
                            WS_HSCROLL|ES_AUTOHSCROLL|ES_READONLY,
                            0, 0,
                            0, 0,
                            hWnd,
                            NULL,
                            (HANDLE)GetWindowLong (hWnd, GWL_HINSTANCE),
                            NULL);
WinAssert(hWndMessage);

/* Now we set the maximum size the edit control can utilize */
SendMessage(hWndMessage, EM_EXLIMITTEXT, (WPARAM)0, (LPARAM)EDIT_BUF_SIZE);
SendMessage(hWndMessage, WM_SETFONT, (WPARAM)hFixedFont, TRUE);

/* Insure rich edit control alignment is set to left */
pf.cbSize = sizeof(pf);
pf.dwMask = PFM_ALIGNMENT;
pf.wAlignment = PFA_LEFT;
SendMessage(hWndMessage, EM_SETPARAFORMAT, 0, (LPARAM)&pf);

/* Set up wordwrapping to occur at 7 inches */
hDC = CreateCompatibleDC(NULL);
SendMessage(hWndMessage, EM_SETTARGETDEVICE,
   (WPARAM) (HDC)hDC, (LPARAM) (1440 * 7)); /* 1440 twips per inch */
ReleaseDC(hWndMessage, hDC);

// Find window menu where children will be listed
ccs.hWindowMenu  = GetSubMenu(GetMenu(hWnd), WINDOWMENU);
ccs.idFirstChild = IDM_WINDOWCHILD;

// Create the MDI client filling the client area
hWndListParent = CreateWindowEx(WS_EX_CLIENTEDGE,    // 3D edge (sunken)
                               "mdiclient",
                               NULL,
                               WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL |
                               WS_HSCROLL,
                               0, 0, 0, 0,
                               hWnd,
                               (HMENU)0xCAC,
                               hInst,
                               (LPVOID)&ccs);

WinAssert(hWndListParent);
ShowWindow(hWndListParent, SW_SHOW);

/* Set up the status bar */
hStatusBar = CreateStatusWindow(WS_CHILD|WS_VISIBLE|WS_BORDER,
                                      "",
                                      hWnd,
                                      IDM_STATUSBAR);
WinAssert(hStatusBar);
InitializeStatusBar(hWnd);
/* Initialize the timer */
SetTimer(hWnd, IDM_TIMER, TIMER_TIMEOUT, NULL);

CreateButtonBar(hWnd);

if (SearchPath(
   NULL,               /* address of search path               */
   "WIZDIFF.EXE",      /* address of filename                  */
   NULL,               /* address of extension                 */
   PATH_MAX,           /* size, in characters, of buffer       */
   szExplorer,         /* address of buffer for found filename */
   &ptr                /* address of pointer to file component */
   ) == 0)
   {
   EnableMenuItem(hMenu,IDM_WIZDIFF,MF_BYCOMMAND|
      MF_GRAYED|MF_DISABLED);
   DeleteMenu(hMenu, IDM_WIZDIFF, MF_BYCOMMAND);
   EnableMenuItem(hMenu,IDM_DIFF_ARCHIVE_CONTENTS,MF_BYCOMMAND|
      MF_GRAYED|MF_DISABLED);
   DeleteMenu(hMenu, IDM_DIFF_ARCHIVE_CONTENTS, MF_BYCOMMAND);
   }

GetWindowsDirectory(szExplorer, PATH_MAX);
lstrcat(szExplorer, "\\EXPLORER.EXE");
hExplorer = FindFirstFile(szExplorer, &fd);
if (hExplorer == INVALID_HANDLE_VALUE)
   {
   EnableMenuItem(hMenu,IDM_EXPLORER,MF_BYCOMMAND|
      MF_GRAYED|MF_DISABLED);
   DeleteMenu(hMenu, IDM_EXPLORER, MF_BYCOMMAND);
   }
else
   FindClose(hExplorer);

uCommDlgHelpMsg = RegisterWindowMessage((LPSTR)HELPMSGSTRING); /* register open help message */
if (uf.fCanDragDrop)
     DragAcceptFiles( hWnd, TRUE );
return 0;
}


//
//  FUNCTION: CmdWindowTileHorizontal(HWND, UINT, WPARAM, LPARAM);
//
//  PURPOSE: To tile the mdi child windows.
//
//  PARAMETERS:
//    hwnd - The window handling the command.
//    wCommand - IDM_WINDOWTILE (unused).
//    hwndCtrl - NULL (unused).
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdWindowTileHorizontal(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
     WinAssert(hWndListParent);
     SendMessage(hWndListParent,
                 WM_MDITILE,
                 MDITILE_HORIZONTAL,
                 0L);

    return 0;
}


//
//  FUNCTION: CmdWindowTileVertical(HWND, UINT, WPARAM, LPARAM);
//
//  PURPOSE: To tile the mdi child windows.
//
//  PARAMETERS:
//    hwnd - The window handling the command.
//    wCommand - IDM_WINDOWTILE (unused).
//    hwndCtrl - NULL (unused).
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdWindowTileVertical(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
     WinAssert(hWndListParent);
     SendMessage(hWndListParent,
                 WM_MDITILE,
                 MDITILE_VERTICAL,
                 0L);

    return 0;
}

//
//  FUNCTION: CmdWindowCascade(HWND, UINT, WPARAM, LPARAM);
//
//  PURPOSE: To cascade the mdi child windows.
//
//  PARAMETERS:
//    hwnd - The window handling the command.
//    wCommand - IDM_WINDOWCASCADE (unused).
//    hwndCtrl - NULL (unused).
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdWindowCascade(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    WinAssert(hWndListParent);
    SendMessage(hWndListParent, WM_MDICASCADE, 0, 0L);

    return 0;
}

//
//  FUNCTION: CmdWindowIcons(HWND, UINT, WPARAM, LPARAM);
//
//  PURPOSE: To arrange the mdi child icons.
//
//  PARAMETERS:
//    hwnd - The window handling the command.
//    wCommand - IDM_WINDOWICONS (unused).
//    hwndCtrl - NULL (unused).
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdWindowIcons(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    WinAssert(hWndListParent);
    SendMessage(hWndListParent, WM_MDIICONARRANGE, 0, 0L);

    return 0;
}


//
//  FUNCTION: CmdWindowCloseAll(HWND, UINT, WPARAM, LPARAM);
//
//  PURPOSE: To close all of the mdi child windows.
//
//  PARAMETERS:
//    hwnd - The window handling the command.
//    wCommand - IDM_WINDOWCLOSEALL (unused).
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused).
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdWindowCloseAll(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HWND hwndT;

    WinAssert(hwnd);
    // As long as the MDI client has a child, destroy it
    while ((hwndT = GetWindow(hWndListParent, GW_CHILD))!=NULL)
    {
        // Skip the icon and title windows
        while (hwndT && GetWindow(hwndT, GW_OWNER))
            hwndT = GetWindow(hwndT, GW_HWNDNEXT);

        if (hwndT)
        {
               SendMessage(hWndListParent, WM_MDIDESTROY, (WPARAM)hwndT, 0L);
        }
        else
            break;
    }

    return 0;
}

LRESULT CmdTimer(HWND hWnd, WORD wMessage, WORD wParam, HWND lParam)
{
MsgTimer(hWnd, (UINT)wMessage, (WPARAM)wParam, (LPARAM)lParam);
return 0;
}

LRESULT MsgNotify(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
int tabIndex;
LRESULT retcode;
LPNMHDR   lpnmhdr = (LPNMHDR)lParam;
NM_LISTVIEW *pNm = (NM_LISTVIEW *)lParam;

/* Is the message from the Toolbar? */
if (lpnmhdr->hwndFrom == hWndToolbar)
   /* Okay, this is to handle "toolbar adjust" messages */
   return ToolBarNotify(hWnd, (WORD)wMessage, (WORD)wParam, (HWND)lParam);

/* Now we need to know if we need to show toolbar tips */   
retcode = ToolBarNotify(hWnd, (WORD)wMessage, (WORD)wParam, (HWND)lParam);
if (!retcode)
   {
   return retcode;
   }

/* Are we dealing with a message from the TAB CONTROL? */
if ((lpnmhdr->code == TCN_SELCHANGE) &&
   (lpnmhdr->hwndFrom == hTabCtrl))
   {
   tabIndex = (int) SendMessage(hTabCtrl,
                               TCM_GETCURSEL,
                                (WPARAM)0,
                               (LPARAM)0);
   switch (tabIndex) {
   case 0: // Show archive listbox
      ShowWindow(hWndArchivesFound, SW_HIDE);
      ShowWindow(hWndGrep, SW_HIDE);
      ShowWindow(hWndMessage, SW_HIDE);
      ShowWindow(hWndEditor, SW_HIDE);
      ShowWindow(hWndListParent, SW_SHOW);
      break;
   case 1: // Show wiz editor
      ShowWindow(hWndListParent, SW_HIDE);
      ShowWindow(hWndArchivesFound, SW_HIDE);
      ShowWindow(hWndGrep, SW_HIDE);
      ShowWindow(hWndMessage, SW_HIDE);
      ShowWindow(hWndEditor, SW_SHOW);
      break;
   case 2: // Show results of search for archives
      ShowWindow(hWndListParent, SW_HIDE);
      ShowWindow(hWndArchivesFound, SW_SHOW);
      ShowWindow(hWndGrep, SW_HIDE);
      ShowWindow(hWndMessage, SW_HIDE);
      ShowWindow(hWndEditor, SW_HIDE);
      break;
   case 3: // Show results of grep
      ShowWindow(hWndListParent, SW_HIDE);
      ShowWindow(hWndArchivesFound, SW_HIDE);
      ShowWindow(hWndMessage, SW_HIDE);
      ShowWindow(hWndEditor, SW_HIDE);
      ShowWindow(hWndGrep, SW_SHOW);
      break;
   case 4: // Show wiz messages
      ShowWindow(hWndListParent, SW_HIDE);
      ShowWindow(hWndArchivesFound, SW_HIDE);
      ShowWindow(hWndGrep, SW_HIDE);
      ShowWindow(hWndEditor, SW_HIDE);
      ShowWindow(hWndMessage, SW_SHOW);
      break;
   }
   InvalidateRect(hTabCtrl, NULL, TRUE);
   UpdateWindow(hTabCtrl);
   }
// Are we dealing with a message from the search in archives window?
if (pNm->hdr.hwndFrom == hWndGrep)
   {
   char sz[PATH_MAX];
   int iItemClicked;
   RECT rc;

   switch (pNm->hdr.code)
      {
      case NM_RCLICK: /* Now is it a right mouse button click? */
           {
           /* This "selects" the item, which gives us the file name we want */
           iItemClicked = MapCursorToListItem(pNm->hdr.hwndFrom);

           /* Did we click on a live item? */
           if (iItemClicked < 0)
              return 0; // No - bail out

           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, FALSE );

           /* This gets the archive name (sub-item 1) */
           ListView_GetItemText(hWndGrep,
                                iItemClicked,
                                1,
                                lpumb->szFileName,
                                PATH_MAX);
            /* Checking to see if the archive is already loaded. If so, then
               simply refresh the archive display, else create a new archive
               window.
             */
            IsArchiveOpen(lpumb->szFileName);

           /* May need to resize the tool bar because of additional buttons
              We also need to resize the listview window if the toolbar
              gets resized. Hence send the message to hWndMain, so
              everything gets done at once.
            */
           GetClientRect(hWndMain, &rc);
           SendMessage(hWndMain, WM_SIZE, (WPARAM)SIZE_RESTORED,
              MAKELPARAM(rc.right, rc.bottom));
           TabCtrl_SetCurSel(hTabCtrl, 0);
           ShowWindow(hWndListParent, SW_SHOW);
           ShowWindow(hWndGrep, SW_HIDE);
           ShowWindow(hWndArchivesFound, SW_HIDE);
           ShowWindow(hWndEditor, SW_HIDE);

           if ( uf.fCanDragDrop )
               DragAcceptFiles( hWnd, TRUE );
           break;
           }
      case NM_DBLCLK: /* Double-click in ListView control */
           {
           /* This "selects" the item, which gives us the file name we want */
           iItemClicked = MapCursorToListItem(pNm->hdr.hwndFrom);

           /* Did we click on a live item? */
           if (iItemClicked < 0)
              return 0; // No - bail out

           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, FALSE );

           // Save the displayed archive file name
           lstrcpy(sz, lpumb->szFileName);

           // Extract the requested file(s) to the edit window
           DisplayFiles(hWndGrep, TRUE);

           // Restore the displayed archive file name
           lstrcpy(lpumb->szFileName, sz);

           if ( uf.fCanDragDrop )
               DragAcceptFiles( hWnd, TRUE );
           break;
           }
     }
   }
// Are we dealing with a message from the search for archives window?   
if (pNm->hdr.hwndFrom == hWndArchivesFound)
   {
   RECT rc;

   switch (pNm->hdr.code)
      {
      case NM_DBLCLK: /* Double-click in ListView control */
           {
           int i, j;
           LV_ITEM lvi;

           /* This "selects" the item, which gives us the file name we want */
           MapCursorToListItem(pNm->hdr.hwndFrom);

           j = ListView_GetItemCount(pNm->hdr.hwndFrom);
           /* Did we click on a live item? */
           if (j < 1)
              return 0; // No - bail out

           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, FALSE );

           for (i = 0; i < j; i++)
               {
               // Note: we have to do this rather than use ListView_GetItemState
               //       because that macro doesn't work! It always returns zero,
               //       unless I have a warped understanding of the macro. This
               //       is actually a problem with LVM_GETITEMSTATE returning the
               //       same zero.
               // Is the item selected?
               lvi.mask      = LVIF_STATE;
               lvi.stateMask = LVIS_SELECTED;
               lvi.iItem     = i;
               lvi.iSubItem  = 0;
               ListView_GetItem(pNm->hdr.hwndFrom, &lvi);

               if (lvi.state == LVIS_SELECTED)
                  {
                  /* This gets the archive name (sub-item 0) */
                  ListView_GetItemText(hWndArchivesFound,
                                       i, // item itself
                                       0, // sub-item
                                       lpumb->szFileName, // Name goes here
                                       PATH_MAX);         // Max length of name

                  // Create a window for the archive
                  CreateMDIChild(lpumb->szFileName);
                  InitializeArchiveStatusBar(GetActiveMDIChild());
                  // Extract the requested file to the edit window
                  DisplayFiles(GetArchiveWindow(NULL),FALSE);
                  UpdateListBox(); /* fill in list box */
                  ListViewSetSel(0, TRUE);
                  }
               }
           UpdateButtons(); /* update state of buttons */

           /* May need to resize the tool bar because of additional buttons
              We also need to resize the listview window if the toolbar
              gets resized. Hence send the message to hWndMain, so
              everything gets done at once.
            */
           GetClientRect(hWndMain, &rc);
           SendMessage(hWndMain, WM_SIZE, (WPARAM)SIZE_RESTORED,
              MAKELPARAM(rc.right, rc.bottom));

           TabCtrl_SetCurSel(hTabCtrl, 0);
           ShowWindow(hWndListParent, SW_SHOW);
           ShowWindow(hWndGrep, SW_HIDE);
           ShowWindow(hWndArchivesFound, SW_HIDE);
           ShowWindow(hWndEditor, SW_HIDE);

           if ( uf.fCanDragDrop )
               DragAcceptFiles( hWnd, TRUE );
           break;
           }
     }
   }
return 0;
}


LRESULT CmdActivate(HWND hWnd, WORD wMessage, WORD wParam, HWND lParam)
{
return DefWindowProc(hWnd, (UINT)wMessage, (WPARAM)wParam, (LPARAM)lParam);
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdSize(HWND hWnd, WORD wMessage, WORD wParam, HWND lParam)
{
SendMessage(hStatusBar, wMessage, wParam, (LPARAM)lParam);
SendMessage(hWndToolbar, wMessage, wParam, (LPARAM)lParam);
SizeWindow();
return 0;
}

LRESULT MsgSysCommand(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
return DefWindowProc( hWnd, wMessage, wParam, lParam );
}

LRESULT CmdCommand(HWND hWnd, WORD wMessage, WORD wParam, HWND lParam)
{
RECT rc;
/* Was F1 just pressed in a menu, or are we in help mode */
/* (Shift-F1)? */
/*
if (uf.fHelp)
   {
   GetHelpContext(wParam);

   if ((!dwHelpContextId) && (uf.fHelp))
      {
      char sz1[RESOURCE_STR_LEN], sz2[RESOURCE_STR_LEN];
      lstrcpy(sz1, GetLoadString(IDS_NO_CONTEXT_HELP));
      lstrcpy(sz2, GetLoadString(IDS_HELP_EXAMPLE));
      MessageBox( hWnd, sz1, sz2, MB_OK);
      }
   else
      if (uf.fHelp)
         WinHelp(hWnd,szHelpFileName,HELP_CONTEXT,dwHelpContextId);
   uf.fHelp = FALSE;
   }
else
*/
/* not in help mode */
   {
   switch (wMessage) {
      case IDM_WIZDIFF:
           {
           char szPath[PATH_MAX], szTemp[PATH_MAX];

           if (SearchPath(
               NULL,               /* address of search path               */
               "WIZDIFF.EXE",      /* address of filename                  */
               NULL,               /* address of extension                 */
               PATH_MAX,           /* size, in characters, of buffer       */
               szPath,             /* address of buffer for found filename */
               &ptr                /* address of pointer to file component */
               ) == 0)
               {
               char sz1[RESOURCE_STR_LEN];
               lstrcpy(sz1, GetLoadString(IDS_NO_WIZDIFF));
               MessageBox(NULL, sz1,
                          "Wiz Error",
                          MB_OK|MB_ICONSTOP);
               break;
               }
           StartupInfo.lpReserved = NULL;
           StartupInfo.lpDesktop = NULL;
           StartupInfo.lpTitle = NULL;
           StartupInfo.dwX = StartupInfo.dwY = 0;
           StartupInfo.dwXSize = StartupInfo.dwYSize = 0;
           StartupInfo.dwXCountChars = StartupInfo.dwYCountChars = 0;
           StartupInfo.dwFillAttribute = 0;
           StartupInfo.dwFlags = 0;
           StartupInfo.wShowWindow = 0;
           StartupInfo.cbReserved2 = 0;
           StartupInfo.lpReserved2 = NULL;
           StartupInfo.hStdInput = StartupInfo.hStdOutput = StartupInfo.hStdError = NULL;
           StartupInfo.cb = sizeof(STARTUPINFO);
           sprintf(szTemp, "%s -c", szPath);
           CreateProcess(szPath,
                         szTemp,  /* Command Line */
                         NULL,  /* Process Attributes */
                         NULL,  /* Thread Attributes */
                         FALSE, /* Inherit Flag */
                         0,     /* Process Flags */
                         NULL,  /* Environment */
                         NULL,  /* Process current drive/directory */
                         &StartupInfo,
                         &WizDiff_Process_Information);
           }
           break;
      case IDM_EXPLORER:
           {
           char sz[PATH_MAX];
           char szPath[PATH_MAX];
           GetWindowsDirectory(sz, PATH_MAX);
           lstrcpy(szPath, sz);
           lstrcat(sz, "\\EXPLORER.EXE");
           hExplorer = ShellExecute(hWndMain, NULL, sz, NULL,
              sz, SW_SHOWNORMAL);
           if ((int)hExplorer < 33)
              {
              char sz1[RESOURCE_STR_LEN];
              switch ((int)hExplorer) {
                 case ERROR_FILE_NOT_FOUND:
                      sprintf(sz, "%s\n", GetLoadString(IDS_FILE_NOT_FOUND));
                      break;
                 case ERROR_PATH_NOT_FOUND:
                      sprintf(sz, "%s\n", GetLoadString(IDS_PATH_NOT_FOUND));
                      break;
                 case ERROR_BAD_FORMAT:
                      sprintf(sz, "%s\n", GetLoadString(IDS_BAD_FORMAT));
                      break;
                 case SE_ERR_SHARE:
                      sprintf(sz, "%s\n", GetLoadString(IDS_SHARING_VIOLATION));
                      break;
                 case 0:
                      sprintf(sz, "%s\n", GetLoadString(IDS_NO_MEMORY));
                      break;
                 default:
                      sprintf(sz, "%s\n", GetLoadString(IDS_UNDETERMINED_ERROR));
                      break;
                 }
              lstrcpy(sz1, GetLoadString(IDS_NO_EXPLORER));
              MessageBox(NULL, sz,
                         sz1,
                         MB_OK|MB_ICONSTOP);
              }
           }
           break;
      case IDM_TOGGLE_TABS:
          {
           /* This section simply allows you to cycle through the 
              different tabs from the keyboard rather than having
              to use the mouse.
            */
           int tabIndex;
           NMHDR nmhdr;

           tabIndex = (int) SendMessage(hTabCtrl,
                               TCM_GETCURSEL,
                               (WPARAM)0,
                               (LPARAM)0);
           if (tabIndex == 4)
               tabIndex = 0;
           else
               tabIndex++;
           TabCtrl_SetCurSel(hTabCtrl, tabIndex);
           nmhdr.code = TCN_SELCHANGE;
           nmhdr.hwndFrom = hTabCtrl;
           /* Send message that selection has changed as
              TabCtrl_SetCurSel does not cause the message
              TCN_SELCHANGE to be generated.
            */
           tabIndex = (int) SendMessage(hWnd,
                               WM_NOTIFY, 
                               (int)tabIndex,
                               (LPARAM)&nmhdr);
           break;
          }

      case IDM_CLEAR_ARCHIVEBOX:
           ListView_DeleteAllItems(hWndArchivesFound);
           TabCtrl_SetCurSel(hTabCtrl, 0);
           ShowWindow(hWndListParent, SW_SHOW);
           ShowWindow(hWndGrep, SW_HIDE);
           ShowWindow(hWndArchivesFound, SW_HIDE);
           ShowWindow(hWndMessage, SW_HIDE);
           ShowWindow(hWndEditor, SW_HIDE);
           break;
      case IDM_CLEAR_GREPBOX:
           ListView_DeleteAllItems(hWndGrep);
           TabCtrl_SetCurSel(hTabCtrl, 0);
           ShowWindow(hWndListParent, SW_SHOW);
           ShowWindow(hWndGrep, SW_HIDE);
           ShowWindow(hWndArchivesFound, SW_HIDE);
           ShowWindow(hWndMessage, SW_HIDE);
           ShowWindow(hWndEditor, SW_HIDE);
           break;
      case IDM_SCAN_ARCHIVE:
           {
           char saveDir[PATH_MAX];
           char *p, drive[2], szTmp[PATH_MAX]="wiz.ini";
           BROWSEINFO lpbi;
           ITEMIDLIST * pidList;
           TCHAR szDisplayName[PATH_MAX];
           OPENFILENAME ofn;
           int iImage = 0;

           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, FALSE );
           fDisplay = 3;
           GetCurrentDirectory(PATH_MAX, szHomeDir);
           fScanArchives = TRUE;

           if (!fUseOldDirSelect)
              {
              lpbi.hwndOwner = hWnd;
              lpbi.pidlRoot = NULL;
              lpbi.pszDisplayName = szDisplayName;
              lpbi.lpszTitle = "Select starting search directory";
              lpbi.ulFlags = BIF_RETURNONLYFSDIRS;
              lpbi.lpfn = NULL;
              lpbi.lParam = 0;
              lpbi.iImage = iImage;
              if ((pidList = SHBrowseForFolder(&lpbi)) == NULL)
                 {
                 fScanArchives = FALSE;
                 fDisplay = 0;
                 if (uf.fCanDragDrop)
                    DragAcceptFiles(hWnd, TRUE);
                 break;
                 }
              SHGetPathFromIDList(pidList, baseDir);
              }
           else
              {
              char * p;

              memset(&ofn, '\0', sizeof(OPENFILENAME)); /* initialize struct */
              WinAssert(hWnd);
              ofn.lStructSize = sizeof(OPENFILENAME);
              ofn.hwndOwner = hWnd;
              ofn.hInstance = hInst;
              ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
              ofn.nFilterIndex = 1;

              ofn.lpstrFile = szTmp;
              ofn.nMaxFile = PATH_MAX;
              ofn.lpstrFileTitle = NULL;
              ofn.nMaxFileTitle = PATH_MAX; /* ignored ! */
              ofn.lpstrTitle = (LPSTR)"Scan for Archives";
              ofn.lpstrInitialDir = lpumb->szUnzipFromDirName;
              ofn.Flags = OFN_ENABLEHOOK | OFN_EXPLORER |
                          OFN_HIDEREADONLY|OFN_NOVALIDATE;
              ofn.lpfnHook = (LPOFNHOOKPROC)GrepArchiveProc;
              if (!GetOpenFileName(&ofn))
                 {
                 fScanArchives = FALSE;
                 fDisplay = 0;
                 if (uf.fCanDragDrop)
                    DragAcceptFiles(hWnd, TRUE);
                 break;
                 }
              lstrcpy(baseDir, szTmp);
              p = strrchr(baseDir, '\\');
              p++;
              *p = '\0';

              }

           CleanScreen();
           /* Get directory to start in */
           SetCurrentDirectory(baseDir);
           if ((baseDir[lstrlen(baseDir) -1] == '\\') &&
              (lstrlen(baseDir) > 3))
              baseDir[lstrlen(baseDir) - 1] = '\0';
           ListView_DeleteAllItems(hWndArchivesFound);
           /* Clean up the screen, and hold off displaying the
              listview until the scan is complete.
            */  
           LockWindowUpdate(hWndArchivesFound);

           if (baseDir[lstrlen(baseDir) -1] != '\\')
              lstrcat(baseDir, "\\");
           lstrcpy(saveDir, baseDir);
           lstrcat(baseDir, "*.zip");
           lstrcpy(SearchPattern, "*.zip");
           if ((lstrlen(SearchPattern) != 0) &&
               (lstrlen(baseDir) != 0))
              {
              drive[0] = baseDir[0];
              drive[1] = '\0';
              p = &baseDir[2];

              hSaveCursor = SetCursor(hHourGlass);
              ShowCursor(TRUE);

              /* Off we go - the control handle is set to 1 as a flag */
              fDoingArchives = TRUE;
              FindFile(drive, p, (HWND)1); /* First we look for .zip files */
              lstrcpy(baseDir, saveDir);
              lstrcat(baseDir, "*.exe");
              p = &baseDir[2];
              FindFile(drive, p, (HWND)1); /* Now we look for .exe files */
              fDoingArchives = FALSE;
              ShowCursor(FALSE);
              SetCursor(hSaveCursor);
              ShowCursor(TRUE);
              }
           TabCtrl_SetCurSel(hTabCtrl, 2);
           ShowWindow(hWndListParent, SW_HIDE);
           ShowWindow(hWndGrep, SW_HIDE);
           ShowWindow(hWndArchivesFound, SW_SHOW);
           ShowWindow(hWndMessage, SW_HIDE);
           ShowWindow(hWndEditor, SW_HIDE);
           SetCurrentDirectory(szHomeDir);
           }
           fScanArchives = FALSE;
           fDisplay = 0;
           LockWindowUpdate(NULL);
           if (uf.fCanDragDrop)
              DragAcceptFiles(hWnd, TRUE);
           break;
      case IDM_GREP_ARCHIVE:
           fGreppingArchives = TRUE;
           fDisplay = 3;
      case IDM_GREP_ARCHIVE_CONTENTS:
           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, FALSE );
           if (!hPatternSelectDlg)
              {
              hPatternSelectDlg =
                 CreateDialog(hInst, "GREPPATTERN", hWnd, (DLGPROC)GrepPatternSelectProc);
              WinAssert(hPatternSelectDlg);
              }
           fDisplay = 0;
           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, TRUE );
           break;
#if 0
      case IDM_CANCEL_OPERATION:
           iGlobalCancel = TRUE;
           break;
#endif
      case IDM_SEARCH_ARCHIVE:
           {
           char saveDir[PATH_MAX];
           char *p, drive[2], szTmp[PATH_MAX], sz1[RESOURCE_STR_LEN];
           OPENFILENAME ofn;
           if (fGreppingArchives && (psGrepBuf == NULL))
              break;
           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, FALSE );

           fDisplay = 3; /* Turn off all messages */

           memset(&ofn, '\0', sizeof(OPENFILENAME)); /* initialize struct */
           WinAssert(hWnd);
           szTmp[0] = '\0';
           ofn.lStructSize = sizeof(OPENFILENAME);
           ofn.hwndOwner = hWnd;
           ofn.hInstance = hInst;
           ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
           ofn.nFilterIndex = 1;

           ofn.lpstrFile = szTmp;
           ofn.nMaxFile = PATH_MAX;
           ofn.lpstrFileTitle = NULL;
           ofn.nMaxFileTitle = PATH_MAX; /* ignored ! */
           lstrcpy(sz1, GetLoadString(IDS_FIND_FILES));
           ofn.lpstrTitle = sz1;
           ofn.lpstrInitialDir = lpumb->szUnzipFromDirName;
           ofn.Flags = OFN_ENABLEHOOK | OFN_EXPLORER |
                  OFN_HIDEREADONLY | OFN_NOVALIDATE;
           ofn.lpfnHook = (LPOFNHOOKPROC)GrepArchiveProc;
           GetCurrentDirectory(PATH_MAX, szHomeDir);
           if (GetOpenFileName(&ofn))
              {
              /* Get directory to start in */
              GetCurrentDirectory(PATH_MAX, baseDir);
              if (baseDir[lstrlen(baseDir) -1] != '\\')
                 lstrcat(baseDir, "\\");
              lstrcpy(saveDir, baseDir);
              lstrcat(baseDir, "*.zip");
              p = &ofn.lpstrFile[ofn.nFileOffset];
              lstrcpy(SearchPattern, p);
              if ((lstrlen(SearchPattern) != 0) &&
                 (lstrlen(baseDir) != 0))
                 {
                 drive[0] = baseDir[0];
                 drive[1] = '\0';
                 p = &baseDir[2];

                 hSaveCursor = SetCursor(hHourGlass);
                 ShowCursor(TRUE);

                 // Grep listview stuff
                 ListView_DeleteAllItems(hWndGrep);

               /* Off we go - the control handle is set to null as a flag */
                 fDoingArchives = TRUE;
                 FindFile(drive, p, NULL); /* look for .zip files */
                 lstrcpy(baseDir, saveDir);
                 lstrcat(baseDir, "*.exe");
                 p = &baseDir[2];
                 FindFile(drive, p, NULL); /* look for .exe files */
                 fDoingArchives = FALSE;
                 ShowCursor(FALSE);
                 SetCursor(hSaveCursor);
                 ShowCursor(TRUE);
                 TabCtrl_SetCurSel(hTabCtrl, 3);
                 ShowWindow(hWndListParent, SW_HIDE);
                 ShowWindow(hWndGrep, SW_SHOW);
                 ShowWindow(hWndArchivesFound, SW_HIDE);
                 ShowWindow(hWndMessage, SW_HIDE);
                 ShowWindow(hWndEditor, SW_HIDE);
                 }
              }
           }
           SetCurrentDirectory(szHomeDir);
           if (fGreppingArchives)
              {
              if (psLBEntry)
                 {
                 GlobalFree((HGLOBAL)psLBEntry);
                 psLBEntry = NULL;
                 }

              if (psGrepBuf)
                 {
                 GlobalFree((HGLOBAL)psGrepBuf);
                 psGrepBuf = NULL;
                 }

              if (psPatternBuf)
                 {
                 GlobalFree((HGLOBAL)psPatternBuf);
                 psPatternBuf = NULL;
                 }
              fGreppingArchives = FALSE;
              }
           fSpool = FALSE;
           fDisplay = 0;
           if (uf.fCanDragDrop)
               DragAcceptFiles(hWnd, TRUE);
           break;

      case IDM_PREFERENCES:
           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, FALSE );
           if (-1 == DoPropertySheet(hWnd))
              MessageBox(hWnd,
                         "Failure creating Property Sheet!",
                         szAppName,
                         MB_OK);
           else
              WriteZipOptionsProfile();
           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, TRUE );
           break;
      case IDM_ZIP_TARGET_CURRENT:
          bUseCurrentZip = TRUE;
      case IDM_ZIP_TARGET:
           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, FALSE );

         iGlobalReturnCode = FALSE;
         wGlobalMessage = IDM_ZIP_TARGET;
         MakeArchive(hWnd);
         bUseCurrentZip = FALSE;
         lstrcpy(lpumb->szZipFileName, ZpZCL.lpszZipFN);
         if ((iGlobalReturnCode == ZE_OK) && (lpumb->szZipFileName[0]))
            {
            lstrcpy(lpumb->szFileName, lpumb->szZipFileName);
            /* Checking to see if the archive is already loaded. If so, then
               simply refresh the archive display, else create a new archive
               window.
             */
            IsArchiveOpen(lpumb->szFileName);
            }
          WriteZipOptionsProfile();

          if ((iGlobalReturnCode != 0) ||
              (iZipWarn != 0))
              {
                // Switch to the messages tab
                ShowWindow(hWndListParent, SW_HIDE);
                ShowWindow(hWndGrep, SW_HIDE);
                ShowWindow(hWndArchivesFound, SW_HIDE);
                ShowWindow(hWndEditor, SW_HIDE);
                ShowWindow(hWndMessage, SW_SHOW);
                TabCtrl_SetCurSel(hTabCtrl, 4);
                }
              else
              {
                // Switch to the archive tab
                ShowWindow(hWndMessage, SW_HIDE);
                ShowWindow(hWndGrep, SW_HIDE);
                ShowWindow(hWndEditor, SW_HIDE);
                ShowWindow(hWndArchivesFound, SW_HIDE);
                ShowWindow(hWndListParent, SW_SHOW);
                TabCtrl_SetCurSel(hTabCtrl, 0);
              }
              InvalidateRect(hTabCtrl, NULL, TRUE);
              UpdateWindow(hTabCtrl);


          if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, TRUE );
          break;
      case IDM_OPEN:
           /* If unzipping separately and previous file exists,
            * go to directory where archive lives.
            */
           {
           TCHAR szTempFileName[PATH_MAX], szDirBuffer[PATH_MAX], 
                 *ptr;
           BOOL OpenFileRetCode = FALSE;
           LPSTR lpBuffer;
           unsigned long BufSize = PATH_MAX * 4;

           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, FALSE );

           /* If not unzipping to same directory as archive and
            * file already open, go to where file lives.
            * If extracting to different directory, return to
            * that directory after selecting archive to open.
            */
           if (lpumb->szUnzipFromDirName[0])
              {
              lstrcpy(lpumb->szDirName, lpumb->szUnzipFromDirName);
              }
           if (lpumb->szFileName[0])
              {
              /* strip off filename to make directory name    */
              GetArchiveDir(lpumb->szDirName);
              }
           else
              {
              if (!lpumb->szUnzipFromDirName[0])
                 lpumb->szDirName[0] = '\0'; /* assume no dir   */
              }
           lpumb->szBuffer[0] = '\0';
           memset(&lpumb->ofn, '\0', sizeof(OPENFILENAME)); /* initialize struct */
           lpumb->ofn.lStructSize = sizeof(OPENFILENAME);
           lpumb->ofn.hwndOwner = hWnd;
           lpumb->ofn.lpstrFilter = "Zip Files (*.zip)\0*.zip\0"\
                                    "All archives\0*.zip;*.exe\0"\
                                    "Self-extracting Files (*.exe)\0*.exe\0"\
                                    "All Files (*.*)\0*.*\0\0";
           lpumb->ofn.nFilterIndex = 1;
           if (lpumb->szFileName[0])
              lstrcpy(szTempFileName, lpumb->szFileName);
           else
              szTempFileName[0] = '\0';
           lpBuffer = (LPSTR) GlobalAlloc(GPTR, BufSize);
           lpumb->ofn.lpstrFile = lpBuffer;
           lpumb->szFileName[0] = '\0';   /* no initial filename   */
           lpumb->ofn.nMaxFile = BufSize;
           lpumb->ofn.lpstrFileTitle = lpumb->szBuffer; /* ignored */
           lpumb->ofn.lpstrInitialDir = (LPSTR)(!lpumb->szDirName[0] ? NULL : lpumb->szDirName);

           lpumb->ofn.nMaxFileTitle = PATH_MAX;
           lpumb->ofn.Flags = OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT |
                              OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |
                              OFN_EXPLORER;
           OpenFileRetCode = GetOpenFileName(&lpumb->ofn);
/* An error return needs to be handled here. Something is not right with
   the GetOpenFileName returns.
*/
           if (!OpenFileRetCode)
           {
               if (FNERR_BUFFERTOOSMALL == CommDlgExtendedError())
               {
                  MessageBox(hWndMain,
                             "Try selecting fewer files to open",
                             szAppName,
                             MB_OK);
                  
                  if ( uf.fCanDragDrop )
                     DragAcceptFiles( hWnd, TRUE );
                  break;
               }
           }

           if (OpenFileRetCode)   /* if successful file open  */
              {
              CleanScreen();

              lstrcpy(lpumb->szDirName, lpBuffer);
              ptr = &lpumb->szDirName[lstrlen(lpBuffer) + 1];
              if (*ptr == '\0') // Only a single file has been selected.
                 GetArchiveDir(lpumb->szDirName);
              else
                  *ptr = '\0';
              /*
              Save the last "unzip from" directory
              */
              if (lpumb->szDirName[1] == ':')
                 drive = lpumb->szDirName[0];
              else
                 drive = '\0';

              if ((toupper(drive) != 'A') &&
                 (toupper(drive) != 'B'))
                 {
                 lstrcpy(lpumb->szUnzipFromDirName, lpumb->szDirName);
                 if (lpumb->szUnzipFromDirName[strlen(lpumb->szUnzipFromDirName)-1]
                    == ':')
                    lstrcat(lpumb->szUnzipFromDirName, "\\");
                 WriteZipOptionsProfile();
                 }
              else
                 WriteZipOptionsProfile();
              if (szUnzipToDirName[0] == '\0') /* or no default */
                 {
                 /* strip off filename to make directory name    */
                 lstrcpy(szUnzipToDirName, lpumb->szDirName);
                 }

              lstrcpy(szDirBuffer, lpBuffer);
              ptr = &lpBuffer[lstrlen(szDirBuffer) + 1];
              if (*ptr == '\0') // Only a single file has been selected.
                  ptr = &szDirBuffer[0];
              while (*ptr != '\0')
              {
                 lstrcpy(lpumb->szFileName, szDirBuffer);
                 if (ptr != &szDirBuffer[0]) // True if multiple files selected.
                 {
                    lstrcat(lpumb->szFileName, "\\");
                    lstrcat(lpumb->szFileName, ptr);
                    ptr = ptr + (lstrlen(ptr) + 1);
                 }
                 else
                     *ptr = '\0';

                 if (!IsValidArchive(lpumb->szFileName))
                    {
                    TCHAR sz[PATH_MAX * 2];
                    sprintf(sz, "%s is not a valid archive", lpumb->szFileName);
                    MessageBox(hWndMain,
                               sz,
                               szAppName,
                               MB_OK);
                    continue;
                    }
                 CreateMDIChild(lpumb->szFileName);
                 InitializeArchiveStatusBar(GetActiveMDIChild());
                 UpdateListBox(); /* fill in list box */

                 /* This simply selects the first entry in the list box
                 ListViewSetSel(0, TRUE);
                 */
                 }
           }
           else
           {
           /* lpumb->szFileName is used to determine if we have an archive
              open, and thus should display buttons.
            */  
              if (szTempFileName[0])
                 lstrcpy(lpumb->szFileName, szTempFileName);
           }

           GlobalFree(lpBuffer);
           UpdateButtons(); /* update state of buttons */

           /* May need to resize the tool bar because of additional buttons
              We also need to resize the listview window if the toolbar
              gets resized. Hence send the message to hWndMain, so
              everything gets done at once.
            */
           GetClientRect(hWndMain, &rc);
           SendMessage(hWndMain, WM_SIZE, (WPARAM)SIZE_RESTORED,
                 MAKELPARAM(rc.right, rc.bottom));

           TabCtrl_SetCurSel(hTabCtrl, 0);
           ShowWindow(hWndListParent, SW_SHOW);
           ShowWindow(hWndArchivesFound, SW_HIDE);
           ShowWindow(hWndGrep, SW_HIDE);
           ShowWindow(hWndEditor, SW_HIDE);
           ShowWindow(hWndMessage, SW_HIDE);

           if ( uf.fCanDragDrop )
              DragAcceptFiles( hWnd, TRUE );
           break;
           }
      case IDM_CHDIR:
              {
              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, FALSE );
              GetUnzipDirectory();

              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, TRUE );
              break;
              }
      case IDM_DELETE_ARCHIVE:
              {
              char szStr[PATH_MAX];

              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, FALSE );

              GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);
              sprintf(szStr,
                      GetLoadString(IDS_CONFIRM_DELETE),
                      lpumb->szFileName);
              if (MessageBox(hWnd, szStr, GetLoadString(IDS_DELETE_TITLE),
                             MB_ICONSTOP | MB_OKCANCEL) == IDOK)
                 {
                 HWND hwndT;

                 hwndT = GetActiveMDIChild();
                 if (hwndT != NULL)
                    {
                    int i, j;
                    TCHAR szTName[PATH_MAX], szOrigName[PATH_MAX];

                    lstrcpy(szOrigName, lpumb->szFileName);
                    DeleteFile(lpumb->szFileName);
                    BufferOut(GetLoadString(IDS_DELETE_MESSAGE), lpumb->szFileName);
                    SendMessage(hwndT, WM_CLOSE, 0, 0L);
                    /* If this file was from the scan for archives
                       window, then delete it from that listbox.
                     */
                    j = ListView_GetItemCount(hWndArchivesFound);
                    if (j > 0)
                       for (i = j-1; i >= 0; i--)
                           {
                           ListView_GetItemText(hWndArchivesFound,
                                                i, // item itself
                                                0, // sub-item
                                                szTName, // Name goes here
                                                PATH_MAX); // Max length of name
                           if (!lstrcmpi(szTName, szOrigName))
                              {
                              ListView_DeleteItem(hWndArchivesFound, i);
                              }
                           }

                    j = ListView_GetItemCount(hWndGrep);
                    if (j > 0)
                       for (i = j-1; i >= 0; i--)
                           {
                           ListView_GetItemText(hWndGrep,
                                                i, // item itself
                                                1, // sub-item
                                                szTName, // Name goes here
                                                PATH_MAX); // Max length of name
                           if (!lstrcmpi(szTName, szOrigName))
                              {
                              ListView_DeleteItem(hWndGrep, i);
                              }
                           }

                    /* Get next active archive window */
                    lpumb->szFileName[0] = '\0';
                    GetWindowText(GetActiveMDIChild(),
                                  lpumb->szFileName,
                                  PATH_MAX);
                    }
                 UpdateButtons(); /* update state of buttons */
                 }
              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, TRUE );
              break;
              }
#if 0
      case IDM_MOVE_ARCHIVE:
              /*
               * Yes - this can be done under Win32 differently, but
               * you have to go through some hoops, and this just makes
               * it easier.
               */
              move_flag = TRUE;
//MW              dwCommDlgHelpId = HELPID_MOVE_ARCHIVE; /* in case user hits "help" button */
      case IDM_RENAME_ARCHIVE:
              if (!move_flag)
                 {
                 rename_flag = TRUE;
//MW                 dwCommDlgHelpId = HELPID_RENAME_ARCHIVE; /* in case user hits "help" button */
                 }
#endif
      case IDM_COPY_ARCHIVE:
              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, FALSE );
//MW              if ((!move_flag) && (!rename_flag))
//MW                 dwCommDlgHelpId = HELPID_COPY_ARCHIVE; /* in case user hits "help" button */
              GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);
              CopyArchive(hWnd, move_flag, rename_flag);
              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, TRUE );
              move_flag = FALSE;
              rename_flag = FALSE;
              break;
//#endif
      case IDM_ABOUT:
              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, FALSE );
              DialogBox(hInst, "About", hWnd, (DLGPROC)AboutProc);
              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, TRUE );
              break;
      case IDM_DIFF_ARCHIVE_CONTENTS:
              fDiffArchiveContents = TRUE;
      case IDM_DIFF_ARCHIVE:
              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, FALSE );
              DialogBox(hInst, (LPCTSTR)"DIFFARCHIVES", hWnd, (DLGPROC)DiffArchiveProc);
              // Make sure window is restored before copying
              CleanScreen();
              if (GetWindowHandles(szArchive1, szArchive2))
                 {
                 if (DiffArchives() && !fDiffError)
                    {
                    UpdateButtons();
                    if (!fDiffArchiveContents)
                        MessageBox(hWnd,
                                   "The differing entries are hi-lighted",
                                   "wiz Message",
                                   MB_OK);
                    else
                       {
                       TCHAR szPath[PATH_MAX];
                       TCHAR szCurDir[PATH_MAX];
                       TCHAR szLoadDir[PATH_MAX];
                       TCHAR szLeft[PATH_MAX], szRight[PATH_MAX],
                             szTemp[PATH_MAX];
                       TCHAR szCurrentExtractDir[PATH_MAX];
                       TCHAR *ptr;
                       LPTSTR lpCurDir = szCurDir;
                       LPSTR i, j;
                       int fExtractOnlyNewer, fRecreateDirs, fDoAll;

                       UpdateButtons();
                       /* Save Current Directory */
                       GetCurrentDirectory(PATH_MAX, lpCurDir);

                       if (SearchPath(
                          NULL,               /* address of search path               */
                          "wiz.EXE",       /* address of filename                  */
                          NULL,               /* address of extension                 */
                          PATH_MAX,           /* size, in characters, of buffer       */
                          szLoadDir,          /* address of buffer for found filename */
                          &ptr                /* address of pointer to file component */
                          ) == 0)
                          {
                          /* This should never happen, we're loaded! */
                          SetCurrentDirectory(lpCurDir);
                          break;
                          }
                       *ptr = '\0';
                       lstrcpy(szLeft, szLoadDir);
                       lstrcat(szLeft, "WizTemp1");
                       CreateDirectory(szLeft, NULL);
                       lstrcat(szLeft, "\\");
                       lstrcpy(szTemp, szArchive1);
                       if ((ptr = lstrrchr(szTemp, '\\'))!=0)
                          {
                          for (i = ptr+1, j = szTemp; *i; i++, j++)
                              {
                              *j = *i;
                              }
                          *j = '\0';
                          }
                       /* strip . and file extension off */
                       if ((ptr = lstrrchr(szTemp, '.'))!=0)
                          {
                          *ptr = '\0';
                          }
                       lstrcat(szLeft, szTemp);
                       CreateDirectory(szLeft, NULL);

                       lstrcpy(szRight, szLoadDir);
                       lstrcat(szRight, "WizTemp2");
                       CreateDirectory(szRight, NULL);
                       lstrcat(szRight, "\\");
                       lstrcpy(szTemp, szArchive2);
                       /* Strip directory name off */
                       if ((ptr = lstrrchr(szTemp, '\\'))!=0)
                          {
                          for (i = ptr+1, j = szTemp; *i; i++, j++)
                              {
                              *j = *i;
                              }
                          *j = '\0';
                          }
                       /* strip . and file extension off */
                       if ((ptr = lstrrchr(szTemp, '.'))!=0)
                          {
                          *ptr = '\0';
                          }
                       lstrcat(szRight, szTemp);
                       CreateDirectory(szRight, NULL);
                       /* We have now created two temp directories, and are
                          ready to set up to extract to them.
                        */

                       /* We are simply setting up here to always overwrite,
                          recreate directories, and extract all selected
                          files. We have to save off the current options.
                        */

                       fExtractOnlyNewer = lpDCL->ExtractOnlyNewer;
                       fRecreateDirs = uf.fRecreateDirs;
                       fDoAll = uf.fDoAll;

                       lpDCL->ExtractOnlyNewer = 0;
                       uf.fRecreateDirs = 1;
                       uf.fDoAll = 1;

                       lstrcpy(szCurrentExtractDir, szUnzipToDirName);
                       lstrcpy(szUnzipToDirName, szLeft);
                       lstrcpy(lpumb->szFileName, szArchive1);
                       SendMessage(hWndListParent, WM_MDIACTIVATE,
                                  (WPARAM)(HWND)hArchive1, 0);
                       Action(GetArchiveWindow(hArchive1), IDM_EXTRACT);

                       lstrcpy(szUnzipToDirName, szRight);
                       lstrcpy(lpumb->szFileName, szArchive2);
                       SendMessage(hWndListParent, WM_MDIACTIVATE,
                                  (WPARAM)(HWND)hArchive2, 0);
                       Action(GetArchiveWindow(hArchive2), IDM_EXTRACT);


                       if (SearchPath(
                                     NULL,               /* address of search path               */
                                     "WIZDIFF.EXE",      /* address of filename                  */
                                     NULL,               /* address of extension                 */
                                     PATH_MAX,           /* size, in characters, of buffer       */
                                     szPath,             /* address of buffer for found filename */
                                     &ptr                /* address of pointer to file component */
                                     ) == 0)
                                     {
                                     char sz1[RESOURCE_STR_LEN];
                                     lstrcpy(sz1, GetLoadString(IDS_NO_WIZDIFF));
                                     MessageBox(NULL, sz1,
                                                "Wiz Error",
                                                MB_OK|MB_ICONSTOP);
                                     break;
                                     }
                       StartupInfo.lpReserved = NULL;
                       StartupInfo.lpDesktop = NULL;
                       StartupInfo.lpTitle = NULL;
                       StartupInfo.dwX = StartupInfo.dwY = 0;
                       StartupInfo.dwXSize = StartupInfo.dwYSize = 0;
                       StartupInfo.dwXCountChars = StartupInfo.dwYCountChars = 0;
                       StartupInfo.dwFillAttribute = 0;
                       StartupInfo.dwFlags = 0;
                       StartupInfo.wShowWindow = 0;
                       StartupInfo.cbReserved2 = 0;
                       StartupInfo.lpReserved2 = NULL;
                       StartupInfo.hStdInput = StartupInfo.hStdOutput = StartupInfo.hStdError = NULL;
                       StartupInfo.cb = sizeof(STARTUPINFO);
                       SetCurrentDirectory(szLoadDir);
                       sprintf(szTemp, "%s -c %s %s", szPath, szLeft, szRight);
                       /* Launch WizDiff */
                       CreateProcess(szPath,
                                     szTemp,  /* Command Line */
                                     NULL,    /* Process Attributes */
                                     NULL,    /* Thread Attributes */
                                     FALSE,   /* Inherit Flag */
                                     0,       /* Process Flags */
                                     NULL,    /* Environment */
                                     NULL,    /* Process current drive/directory */
                                     &StartupInfo,
                                     &WizDiff_Process_Information);

                       /* Restore current options */
                       lpDCL->ExtractOnlyNewer = fExtractOnlyNewer;
                       uf.fRecreateDirs = fRecreateDirs;
                       uf.fDoAll = fDoAll;

                       /* Restore Current Directory */
                       SetCurrentDirectory(lpCurDir);
                       lstrcpy(szUnzipToDirName, szCurrentExtractDir);
                       }
                    }
                 else
                    {
                    if (!fDiffError)
                       MessageBox(hWnd,
                                  "There are no differences between the archives",
                                  "wiz Message",
                                  MB_OK);
                    else
                       MessageBox(hWnd,
                                  "Error allocating memory",
                                  "wiz Warning",
                                  MB_OK);
                    }
                 }
              UpdateButtons();
              fDiffArchiveContents = FALSE;
              if ( uf.fCanDragDrop )
                 DragAcceptFiles( hWnd, TRUE );
              break;
      case IDM_EXIT:
              SendMessage(hWnd, WM_CLOSE, 0, 0L);
              break;

      case IDM_HELP:  /* Display Help */
              WinHelp(hWnd,szHelpFileName,HELP_FINDER,0L);
              break;

      case IDM_HELP_HELP:
              WinHelp(hWnd,"WINHELP.HLP",HELP_HELPONHELP,0L);
              break;

      case IDM_SHOW_COMMENT:
             /* display the archive comment in message window */
             if ( uf.fCanDragDrop )
                DragAcceptFiles( hWnd, FALSE );
             DisplayComment();
             if ( uf.fCanDragDrop )
                DragAcceptFiles( hWnd, TRUE );
             break;

      case IDM_DISPLAY:
             if ( uf.fCanDragDrop )
                DragAcceptFiles( hWnd, FALSE );
             DisplayFiles(GetArchiveWindow(NULL),FALSE);
             UpdateButtons();
             if ( uf.fCanDragDrop )
                DragAcceptFiles( hWnd, TRUE );
             break;

      case IDM_TEST:
      case IDM_GET_ZIPINFO:
      case IDM_EXTRACT:
      case IDM_UPDATE_ZIP:
      case IDM_ZIP_DELETE_ENTRIES:

             if ( uf.fCanDragDrop )
                DragAcceptFiles( hWnd, FALSE );

             fUpdateEntries = TRUE;
             wGlobalMessage = wMessage;
			 iZipWarn = 0;
             if (wGlobalMessage == IDM_EXTRACT)
                {
                if (!GetUnzipDirectory())
                   break;
                }
             if ((wGlobalMessage == IDM_ZIP_DELETE_ENTRIES) ||
                (wGlobalMessage == IDM_UPDATE_ZIP))
                {
                int retcode;

                if (wGlobalMessage == IDM_UPDATE_ZIP)
                   retcode = MessageBox(hWndMain, "Are You Sure?",
                                        "Update Archive Entries",
                                        MB_OKCANCEL | MB_ICONEXCLAMATION);
                else
                   retcode = MessageBox(hWndMain, "Are You Sure?",
                                        "Delete Archive Entries",
                                        MB_OKCANCEL | MB_ICONEXCLAMATION);
                if (retcode != IDOK)
                   {
                   if ( uf.fCanDragDrop )
                      DragAcceptFiles( hWnd, TRUE );
                   break;
                   }
                }
             if (wGlobalMessage == IDM_UPDATE_ZIP)
                {
                OPENFILENAME ofn;
                char szTemp[PATH_MAX]="mike*";
                BROWSEINFO lpbi;
                ITEMIDLIST * pidList;
                TCHAR szDisplayName[PATH_MAX];
                int iImage = 0;

                GetCurrentDirectory(PATH_MAX, szHomeDir);

                if (fUseOldDirSelect)
                   {
                   memset(&ofn, '\0', sizeof(OPENFILENAME)); /* initialize struct */
                   WinAssert(hWnd);
                   ofn.lStructSize = sizeof(OPENFILENAME);
                   ofn.hwndOwner = hWnd;
                   ofn.hInstance = hInst;
                   ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
                   ofn.nFilterIndex = 1;

                   ofn.lpstrFile = szTemp;
                   ofn.nMaxFile = PATH_MAX;
                   ofn.lpstrFileTitle = NULL;
                   ofn.nMaxFileTitle = PATH_MAX; /* ignored ! */
                   ofn.lpstrTitle = (LPSTR)"Set Reference Directory";
                   ofn.lpstrInitialDir = NULL;
                   ofn.Flags = OFN_ENABLEHOOK |
                               OFN_HIDEREADONLY|OFN_EXPLORER|OFN_NOVALIDATE;
                   ofn.lpfnHook = (LPOFNHOOKPROC)GetDirProc;
                   if (!GetOpenFileName(&ofn))
                      {
                      if (uf.fCanDragDrop)
                         DragAcceptFiles(hWnd, TRUE);
                      break;
                      }

                   ofn.lpstrFile[ofn.nFileOffset-1] = '\0';
                   SetCurrentDirectory(ofn.lpstrFile);
                   }
                else
                   {
                   lpbi.hwndOwner = hWnd;
                   lpbi.pidlRoot = NULL;
                   lpbi.pszDisplayName = szDisplayName;
                   lpbi.lpszTitle = "Set Reference Directory";
                   lpbi.ulFlags = BIF_RETURNONLYFSDIRS;
                   lpbi.lpfn = NULL;
                   lpbi.lParam = 0;
                   lpbi.iImage = iImage;
                   if ((pidList = SHBrowseForFolder(&lpbi)) == NULL)
                      {
                      if (uf.fCanDragDrop)
                         DragAcceptFiles(hWnd, TRUE);
                      break;
                      }
                   SHGetPathFromIDList(pidList, baseDir);
                   /* Get directory to start in */
                   SetCurrentDirectory(baseDir);
                   }
                CleanScreen();
                }
             DialogBox(hInst,
                       "SERVICEAPP",
                       hWndMain,
                       (DLGPROC)ServiceProc);

             if (ZpOpt.fOffsets)
                {
                DWORD i, j;
                i = ListView_GetItemCount(GetArchiveWindow(NULL));
                for (j = 0; j < i; j++)
                    ListViewSetSel(j, TRUE);
                }
             UpdateButtons();
             if ((wMessage == IDM_TEST) ||
                (iGlobalReturnCode != 0) ||
				(iZipWarn != 0) ||
                (wMessage == IDM_GET_ZIPINFO))
                {
                // Switch to the messages tab
                ShowWindow(hWndListParent, SW_HIDE);
                ShowWindow(hWndGrep, SW_HIDE);
                ShowWindow(hWndArchivesFound, SW_HIDE);
                ShowWindow(hWndEditor, SW_HIDE);
                ShowWindow(hWndMessage, SW_SHOW);
                TabCtrl_SetCurSel(hTabCtrl, 4);
                }
             else
             {
                // Switch to the archive tab
                ShowWindow(hWndMessage, SW_HIDE);
                ShowWindow(hWndGrep, SW_HIDE);
                ShowWindow(hWndEditor, SW_HIDE);
                ShowWindow(hWndArchivesFound, SW_HIDE);
                ShowWindow(hWndListParent, SW_SHOW);
                TabCtrl_SetCurSel(hTabCtrl, 0);
             }
             InvalidateRect(hTabCtrl, NULL, TRUE);
             UpdateWindow(hTabCtrl);
             if ( uf.fCanDragDrop )
                DragAcceptFiles( hWnd, TRUE );
             break;

      case IDM_SELECT_ALL:
      case IDM_DESELECT_ALL:
             if (cZippedFiles)
                {
                int i;
                int iItems = ListView_GetItemCount(GetArchiveWindow(NULL));

                hSaveCursor = SetCursor(hHourGlass);
                ShowCursor(TRUE);

                if (wMessage == IDM_DESELECT_ALL)
                   for (i = 0; i < iItems; i++)
                       ListViewSetSel(i, FALSE);
                else
                   for (i = 0; i < iItems; i++)
                       ListViewSetSel(i, TRUE);
                UpdateButtons();
                ShowCursor(FALSE);
                SetCursor(hSaveCursor);
                ShowCursor(TRUE);
                }
             break;

      case IDM_SELECT_BY_PATTERN:
             if (!hPatternSelectDlg)
                {
//MW                dwCommDlgHelpId = HELPID_SELECT_BY_PATTERN;
                hPatternSelectDlg =
                   CreateDialog(hInst, "Pattern", hWnd, (DLGPROC)PatternSelectProc);
                WinAssert(hPatternSelectDlg);
                }
             break;

      case IDM_CLEAR_STATUS:  /* forward to message window */
             {
             PARAFORMAT pf;
             SendMessage(hWndMessage, EM_SETSEL, (WPARAM)(INT) 0, (LPARAM)(INT)-1);
             SendMessage(hWndMessage, EM_REPLACESEL, (BOOL)FALSE,
                (LPARAM)(LPCTSTR)"");
             // Insure retains left alignment. Clearing seems to cause
             // the edit control to be set to centering
             pf.cbSize = sizeof(PARAFORMAT);
             SendMessage(hWndMessage, EM_GETPARAFORMAT, 0, (LPARAM) &pf);
             pf.dwMask = PFM_ALIGNMENT;
             pf.wAlignment = PFA_LEFT;
             SendMessage(hWndMessage, EM_SETPARAFORMAT, 0, (LPARAM) &pf);
             memset(pszBuffer, '\0', dwEditBufSize);
             break;
             }
      case IDM_NOTIFY_CLEARING:
             {
             fNotifyClearing = !fNotifyClearing;
             CheckMenuItem(hMenu,IDM_NOTIFY_CLEARING,MF_BYCOMMAND|
                  (WORD)(fNotifyClearing ? MF_CHECKED: MF_UNCHECKED));
             break;
             }
      case IDM_EXTERNAL_RTF:
             {
             fExternalRTF = !fExternalRTF;
             CheckMenuItem(hMenu,IDM_EXTERNAL_RTF,MF_BYCOMMAND|
                  (WORD)(fExternalRTF ? MF_CHECKED: MF_UNCHECKED));
             break;
             }
      default:
             return DefWindowProc(hWnd, (UINT)wMessage, (WPARAM)wParam, (LPARAM)lParam);
      }
   } /* bottom of not in help mode */
return 0;
}

LRESULT MsgSetCursor(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
return DefWindowProc(hWnd, wMessage, wParam, lParam);
}


/* MW
#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgInitMenu(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
if (uf.fHelp)
   {
   SetCursor(hHelpCursor);
   }
return TRUE;
}
*/

/* MW
#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgEnterIdle(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
if ((LOWORD(wParam) == MSGF_MENU) && (GetKeyState(VK_F1) & 0x8000))
   {
   uf.fHelp = TRUE;
   PostMessage(hWnd, WM_KEYDOWN, VK_RETURN, 0L);
   }
return 0;
}
*/

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgClose(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
WinAssert(hWnd);
if (fExecutedFile)
   {
   TCHAR sz[PATH_MAX*2];
   HKEY hkey;
   if (RegOpenKeyEx(HKEY_CURRENT_USER,
                    szRegKey,
                    0,
                    KEY_ALL_ACCESS,
                    &hkey)
                    == ERROR_SUCCESS)
      {
      DWORD cb;
      TCHAR szRegTemp[PATH_MAX];
      TCHAR szTemp[PATH_MAX];
      while (NumberTempFiles)
         {
         lstrcpy(szRegTemp, "Temp");
         itoa(NumberTempFiles, szTemp, 10);
         lstrcat(szRegTemp, szTemp);
         cb = PATH_MAX;
         if (RegQueryValueEx(hkey, szRegTemp, 0, 0,
                            (LPBYTE) sz,
                            &cb) == ERROR_SUCCESS)
            {
            DeleteFile(sz);
            }
         if (lstrlen(szRegTemp) != 0)
            if (RegDeleteValue(hkey, szRegTemp) != ERROR_SUCCESS)
               MessageBox(NULL, sz, "Couldn't delete reg entry", MB_OK);
         NumberTempFiles--;
         }
      RegCloseKey(hkey);
      }

   }
if ( uf.fCanDragDrop )
   DragAcceptFiles( hWnd, FALSE );
WriteZipOptionsProfile();
WinHelp(hWnd, szHelpFileName, HELP_QUIT, 0L);
SendMessage(hWndEditor, WM_DESTROY, 0, 0);
SendMessage(hWndListParent, WM_DESTROY, 0, 0);
DeleteObject(hBrush);
PostQuitMessage(0);
return 0;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdDestroy(HWND hWnd, WORD wMessage, WORD wParam, HWND lParam)
{
return 0;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgDropfiles(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
WORD    cFiles, i;
RECT rc;

/* Get the number of files that have been dropped */
cFiles = (WORD)DragQueryFile( (HDROP)wParam, (UINT)-1, lpumb->szBuffer, (UINT)256);

for (i = 0; i < cFiles; i++)
   {
   DragQueryFile( (HDROP)wParam, i, lpumb->szFileName, PATH_MAX);
   
   // Create a window for the archive
   CreateMDIChild(lpumb->szFileName);
   InitializeArchiveStatusBar(GetActiveMDIChild());

   GetArchiveDir(lpumb->szDirName);
   if (szUnzipToDirName[0] == '\0') /* or no default */
      {
      /* strip off filename to make directory name    */
      lstrcpy(szUnzipToDirName, lpumb->szDirName);
      }
   lstrcpy(lpumb->szBuffer, lpumb->szDirName); /* get scratch copy */
   DlgDirList (hWnd, lpumb->szBuffer, 0, 0, 0); /* change dir */
   UpdateListBox(); /* fill in list box */
   ListViewSetSel(0, TRUE);
   UpdateButtons(); /* update state of buttons */

   }
/* May need to resize the tool bar because of additional buttons
   We also need to resize the listview window if the toolbar
   gets resized. Hence send the message to hWndMain, so
   everything gets done at once.
 */
GetClientRect(hWndMain, &rc);
SendMessage(hWndMain, WM_SIZE, (WPARAM)SIZE_RESTORED,
            MAKELPARAM(rc.right, rc.bottom));
TabCtrl_SetCurSel(hTabCtrl, 0);
ShowWindow(hWndListParent, SW_SHOW);
ShowWindow(hWndGrep, SW_HIDE);
ShowWindow(hWndArchivesFound, SW_HIDE);
ShowWindow(hWndEditor, SW_HIDE);
SendMessage(hWndListParent, WM_MDICASCADE, 0, 0L);
DragFinish( (HDROP)wParam );
return 0;
}

LRESULT CmdDefault(HWND hWnd, WORD wMessage, WORD wParam, HWND lParam)
{
/*MW
if (wMessage == (WORD)uCommDlgHelpMsg)   
   {
   WinHelp(hWnd, szHelpFileName, HELP_CONTEXT, dwCommDlgHelpId );
   return 0;
   }
*/   
return DefWindowProc(hWnd, (UINT)wMessage, (WPARAM)wParam, (LPARAM)lParam);
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdStub(HWND hwnd, WORD wMessage, WORD wparam, HWND lparam)
{
return 0;
}

