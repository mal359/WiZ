/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <string.h>
#include <io.h>
#include <stdio.h>

#include "wiz.h"

//MW #include "helpids.h"

/****************************************************************************

    FUNCTION: SelectDirProc(HWND, WORD, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Unzip To..." dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box

****************************************************************************/

BOOL WINAPI SelectDirProc(HWND hDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
    switch (wMessage)
    {
    case WM_INITDIALOG:
        {
/*
Common control identifiers for GetOpenFileName and GetSaveFileName
Control identifier   Control Description
cmb2                 Drop-down combo box that displays the current drive
                     or folder, and that allows the user to select a
                     drive or folder to open
stc4                 Label for the cmb2 combo box
lst1                 List box that displays the contents of the current drive or folder
stc1                 Label for the lst1 list box
edt1                 Edit control that displays the name of the current file, or in which the user can type the name of the file to open
stc3                 Label for the edt1 edit control
cmb1                 Drop-down combo box that displays the list of file type filters
stc2                 Label for the cmb1 combo box
chx1                 The read-only check box
IDOK                 The OK command button (push button)
IDCANCEL             The Cancel command button (push button)
pshHelp              The Help command button (push button)

*/
      char sz1[RESOURCE_STR_LEN];
      CommDlg_OpenSave_HideControl(GetParent(hDlg), cmb1);
      CommDlg_OpenSave_HideControl(GetParent(hDlg), stc2);
      CommDlg_OpenSave_HideControl(GetParent(hDlg), edt1);
      CommDlg_OpenSave_HideControl(GetParent(hDlg), stc3);
      lstrcpy(sz1, GetLoadString(IDS_SELECT));
      CommDlg_OpenSave_SetControlText(GetParent(hDlg),
         IDOK, sz1);
        }
    default:
        break;
    }

/* message not handled */
return DefWindowProc(hDlg, wMessage, wParam, lParam);
}
