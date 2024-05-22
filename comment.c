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

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wiz.h"

//MW #include "helpids.h"

/****************************************************************************

    FUNCTION: EditBoxProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Comment Edit Box" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/

LPSTR szAppCommentBuf;
HANDLE hStr;

#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI
CommentBoxProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
   switch (wMessage) {
   case WM_INITDIALOG:
      {
      if (szAppCommentBuf[0] != '\0')
         {
         SendMessage(GetDlgItem(hwndDlg, IDM_EDIT_BOX),
                     WM_SETTEXT, 0,
                     (LPARAM)szAppCommentBuf);
         }
      CenterDialog(NULL, hwndDlg); /* center on parent */
      SetFocus(GetDlgItem(hwndDlg, IDM_EDIT_BOX));
      }
      break;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDCANCEL:
            szAppCommentBuf[0] = '\0';
            EndDialog(hwndDlg, wParam);
            break;
/* MW
         case ID_HELP:
            WinHelp(hwndDlg,szHelpFileName,HELP_CONTEXT, (DWORD)(HELPID_ADD_COMMENT));
            break;
*/
         case IDOK:
            {
            DWORD iEBItems;
            /* Get multi-line comment for the zip file */
            SendMessage(GetDlgItem(hwndDlg, IDM_EDIT_BOX), EM_FMTLINES, TRUE, 0L);
            iEBItems = (DWORD) SendMessage(GetDlgItem(hwndDlg, IDM_EDIT_BOX),
               WM_GETTEXTLENGTH, 0, 0L) + 1;
            if (iEBItems)
               {
               if (iEBItems > 65534L)
                  iEBItems = (DWORD) 65534L;
               SendMessage(GetDlgItem(hwndDlg, IDM_EDIT_BOX),
                  WM_GETTEXT, (WPARAM)iEBItems, (LPARAM)szAppCommentBuf);
               }
            else
               szAppCommentBuf[0] = '\0';
            EndDialog(hwndDlg, wParam);
            break;
            }
         }
      default:
         break;
   }
   return FALSE;
}

/* RETURNS: FALSE if failure
            TRUE if okay
 */
INT WINAPI comment(LPSTR szBuf)
{
hStr = GlobalAlloc( GPTR, (DWORD)32768L);
if ( !hStr )
   {
   szBuf[0] = '\0';
   return FALSE;
   }

szAppCommentBuf = GlobalLock(hStr);
if (lstrlen(szBuf) != 0)
   {
   lstrcpy(szAppCommentBuf, szBuf);
   }
else
   {
   szAppCommentBuf[0] = '\0';
   }
ShowWindow(hProgressWnd, FALSE);
DialogBox(hInst, "COMMENTBOX", hWndMain, (DLGPROC)CommentBoxProc);
ShowWindow(hProgressWnd, TRUE);
if (szAppCommentBuf[0] != '\0')
   {
   lstrcpy(szBuf, szAppCommentBuf);
   }
else
   {
   szBuf[0] = '\0';
   }
GlobalUnlock(hStr);
GlobalFree(hStr);
return TRUE;
}

