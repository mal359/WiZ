/*
 Copyright (C) 1998 Mike White
*/

#include <stdio.h>
#include "wiz.h"

HWND hLauncherDlg;

/****************************************************************************

    FUNCTION: LauncherProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Launch" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI
LauncherProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
static HWND hOK, hCANCEL, hProgress;

   switch (wMessage) {
   case WM_INITDIALOG:
      hOK = GetDlgItem(hwndDlg, IDOK);
      hCANCEL = GetDlgItem(hwndDlg, IDCANCEL);
      hProgress = GetDlgItem(hwndDlg, IDC_PROGRESS_TEXT);
      CenterDialog(NULL, hwndDlg);
      hLauncherDlg = hwndDlg;
      return TRUE;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDC_PROGRESS_UPDATE:
            break;

        case IDOK:
            Action(GetArchiveWindow(NULL), (int)IDM_EXTRACT);
            PostMessage(hwndDlg, WM_CLOSE, 0, 0L);
            break;
        case IDCANCEL:
            PostMessage(hwndDlg, WM_CLOSE, 0, 0L);
            break;
      }
      return TRUE;
   case WM_CLOSE:
      hLauncherDlg = hWndMain;
      DestroyWindow(hwndDlg);
      return TRUE;
   }
   return FALSE;
}

