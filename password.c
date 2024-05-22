/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include "wiz.h"
#include "unzip\zip.h"

//MW #include "helpids.h"

int pwflag;
char *prompt;
char *szPassword;
LPSTR lpPasswordStr;

/****************************************************************************

    FUNCTION: PasswordProc(HWND, WORD, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Password" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/
BOOL WINAPI PasswordProc(HWND hDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
    switch (wMessage)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hDlg, IDC_PASSWORD_INCORRECT, prompt);

        if ((LPSTR)lParam != NULL)
           {
           SetDlgItemText(hDlg, IDC_PASSWORD_TEXT, (LPSTR)lParam);
           }
        SetDlgItemText(hDlg, IDC_NEW_PASSWORD_NAME_TEXT, "");
        CenterDialog(NULL, hDlg); /* center on parent */
        return TRUE;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hDlg, IDC_NEW_PASSWORD_NAME_TEXT));
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_PASSWORD_RENAME:
            GetDlgItemText(hDlg, IDC_NEW_PASSWORD_NAME_TEXT, lpPasswordStr, 80);
            pwflag = IZ_PW_ENTERED;
            EndDialog(hDlg, wParam);
            break;
        case IDC_PASSWORD_CANCEL:
            szPassword[0] = '\0';
            pwflag = IZ_PW_CANCELALL;
            EndDialog(hDlg, wParam);
            break;
/* MW
        case IDC_PASSWORD_HELP:
            WinHelp(hDlg,szHelpFileName,HELP_CONTEXT, (DWORD)(HELPID_PASSWORD));
            break;
*/            
        }
        return TRUE;
    }
    return FALSE;
}

int WINAPI password(LPSTR p, int n, LPCSTR m, LPCSTR name)
{
    prompt = (char *)malloc(strlen(m) + 1);
    szPassword = (char *)malloc(n);
    if ((prompt == (char *)NULL) || (szPassword == (char *)NULL)) {
        return IZ_PW_ERROR;
    }
    lpPasswordStr = szPassword;
    pwflag = IZ_PW_ERROR;
    /* Temporary hide progress bar */
    ShowWindow(hProgressWnd, FALSE);
    lstrcpy(prompt, m);

    DialogBoxParam(hInst, "Password", hWndMain, (DLGPROC)PasswordProc,
       (DWORD)(LPSTR)name);
    // Show progress bar again
    ShowWindow(hProgressWnd, TRUE);

    lstrcpy(p, szPassword);
    free(szPassword);
    free(prompt);
    return pwflag;
}

