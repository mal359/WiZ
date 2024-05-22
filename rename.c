/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <stdio.h>
#include "wiz.h"
#include "rename.h"

//MW #include "helpids.h"

/****************************************************************************

    FUNCTION: RenameProc(HWND, WORD, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Rename" dialog box

    MESSAGES:
                                    
    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/

BOOL WINAPI RenameProc(HWND hDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
    static char __far *lpsz;
	char *p;

    switch (wMessage)
    {
    case WM_INITDIALOG:
        lpsz = (char __far *)lParam;
        wsprintf(lpumb->szBuffer,
                 GetLoadString(IDS_RENAME_STR),
                 (LPSTR)lParam);
        SetDlgItemText(hDlg, IDM_RENAME_TEXT, lpumb->szBuffer);
        wsprintf(lpumb->szBuffer, "%s", (LPSTR)lParam);
        for (p = lpumb->szBuffer; *p; p++)
            if (*p == '/')
                *p = '\\';
        SetDlgItemText(hDlg, IDM_NEW_NAME_TEXT, lpumb->szBuffer);
        CenterDialog(NULL, hDlg); /* center on parent */
        return TRUE;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hDlg, IDM_NEW_NAME_TEXT));
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_RENAME_RENAME:
            GetDlgItemText(hDlg, IDM_NEW_NAME_TEXT, lpsz, 80);
        case IDM_RENAME_CANCEL:
            EndDialog(hDlg, wParam);
            break;
/* MW
        case IDM_RENAME_HELP:
            WinHelp(hDlg,szHelpFileName,HELP_CONTEXT, (DWORD)(HELPID_OVERWRITE));
*/            
        }
        return TRUE;
    }
    return FALSE;
}

