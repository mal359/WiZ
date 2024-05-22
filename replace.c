/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/
#include <stdio.h>
#include "wiz.h"
#include "replace.h"
#include "rename.h"

//MW #include "helpids.h"

/****************************************************************************

    FUNCTION: Replace(HWND, WORD, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Replace" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/

BOOL WINAPI ReplaceProc(HWND hDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
    static char __far *lpsz;
	char *p, szTemp[PATH_MAX];

    switch (wMessage)
    {
    case WM_INITDIALOG:
        lpsz = (char __far *)lParam;
        wsprintf(lpumb->szBuffer, "Replace %s ?", (LPSTR)lpsz);
        for (p = lpumb->szBuffer; *p; p++)
            if (*p == '/')
                *p = '\\';
        CharToOem(lpumb->szBuffer, szTemp);
        SetDlgItemText(hDlg, IDM_REPLACE_TEXT, szTemp);
        CenterDialog(NULL, hDlg); /* center on parent */
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:              /* ESC key      */
        case IDOK:                  /* Enter key    */
            EndDialog(hDlg, IDM_REPLACE_NO);
            break;
        case IDM_REPLACE_RENAME:
            {
                if (DialogBoxParam(hInst, "Rename", hWndMain, 
                                   (DLGPROC)RenameProc, (DWORD)(LPSTR)lpsz) != IDM_RENAME_RENAME)
                   {
                   wParam = 0;
                   }
            }
            if (LOWORD(wParam))
                EndDialog(hDlg, wParam);
            else
                SetFocus(hDlg);
            SetCursor(LoadCursor(0,IDC_ARROW));
            break;
        case IDM_REPLACE_ALL:
            uf.fDoAll = 1;
        case IDM_REPLACE_NONE:
        case IDM_REPLACE_YES:
        case IDM_REPLACE_NO:
            EndDialog(hDlg, wParam);
            break;
/* MW
        case IDM_REPLACE_HELP:
            WinHelp(hDlg,szHelpFileName,HELP_CONTEXT, (DWORD)(HELPID_OVERWRITE));
*/            
        }
        return TRUE;
    }
    return FALSE;
}

