/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "wiz.h"

//MW #include "helpids.h"

UF ufTemp;

BOOL  overwrite,newer,prompt_overwrite, oldstyle,
      space_underscore, lbs_extract, lbs_display, lbs_test;
int fPrivilege;

void InitUnzipPrefs(HWND);


/****************************************************************************

    FUNCTION: InitUnZipPrefs(HWND)

    PURPOSE:  Initializes Unzip dialog box controls

****************************************************************************/
void InitUnzipPrefs(HWND hwndDlg)
{
/* UnZip Options */
ufTemp = uf;
CheckDlgButton(hwndDlg, IDC_RECREATE_DIR, ufTemp.fRecreateDirs);

overwrite = lpDCL->noflag;
if (overwrite)
   CheckRadioButton(hwndDlg,
                    IDC_OVERWRITE,
                    IDC_PROMPT_OVERWRITE,
                    IDC_OVERWRITE);

newer = lpDCL->ExtractOnlyNewer;
if (newer)
   CheckRadioButton(hwndDlg,
                    IDC_OVERWRITE,
                    IDC_PROMPT_OVERWRITE,
                    IDC_NEWER);

prompt_overwrite = lpDCL->PromptToOverwrite;
if (prompt_overwrite)
   CheckRadioButton(hwndDlg,
                    IDC_OVERWRITE,
                    IDC_PROMPT_OVERWRITE,
                    IDC_PROMPT_OVERWRITE);

CheckDlgButton(hwndDlg, IDC_LF_CRLF, ufTemp.fTranslate);

CheckDlgButton(hwndDlg, IDC_RECURSE_ON_SEARCH,
               ufTemp.fRecurseOnSearch);

space_underscore = lpDCL->SpaceToUnderscore;
CheckDlgButton(hwndDlg, IDC_SPACE_UNDERSCORE, space_underscore);

oldstyle = fUseOldDirSelect;
CheckDlgButton(hwndDlg, IDC_OLDSTYLE, oldstyle);

fPrivilege = lpDCL->fPrivilege;
if (fPrivilege == 1)
   CheckDlgButton(hwndDlg, IDC_ACL, TRUE);
if (fPrivilege == 2)
   CheckDlgButton(hwndDlg, IDC_PRIVILEGE, TRUE);
switch (fLB_Selection)
       {
       case 0:
              CheckRadioButton(hwndDlg,
                               IDC_LBS_EXTRACT,
                               IDC_LBS_TEST,
                               IDC_LBS_EXTRACT);
              lbs_extract = TRUE;
              lbs_display = FALSE;
              lbs_test = FALSE;
              break;
       case 1:
              CheckRadioButton(hwndDlg,
                               IDC_LBS_EXTRACT,
                               IDC_LBS_TEST,
                               IDC_LBS_DISPLAY);
              lbs_display = TRUE;
              lbs_extract = FALSE;
              lbs_test = FALSE;
              break;
       case 2:
              CheckRadioButton(hwndDlg,
                               IDC_LBS_EXTRACT,
                               IDC_LBS_TEST,
                               IDC_LBS_TEST);
              lbs_test = TRUE;
              lbs_extract = FALSE;
              lbs_display = FALSE;
              break;
       }
CheckDlgButton(hwndDlg, IDC_RESCAN_FOR_ARCHIVES,
               ufTemp.fRescan);

}

/****************************************************************************

    FUNCTION: UnZipPreferencesProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "UnZip Preferences" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/
DWORD WINAPI
UnZipPreferencesProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
int i;
   switch (wMessage) {
   case WM_INITDIALOG:
           InitUnzipPrefs(hwndDlg);
           break;
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
           {
           POINT point;
           SetBkMode((HDC)wParam, OPAQUE);
           SetBkColor((HDC)wParam, GetSysColor(COLOR_MENU));
           SetTextColor((HDC)wParam, GetSysColor(COLOR_MENUTEXT));
           UnrealizeObject(hBrush);
           point.x = point.y = 0;
           ClientToScreen(hwndDlg, &point);
           SetBrushOrgEx((HDC)wParam, point.x, point.y, NULL);
           hBrush = CreateSolidBrush(GetSysColor(COLOR_MENU));
           return ((DWORD)hBrush);
           }

   case WM_NOTIFY:
          switch (((NMHDR FAR *)lParam)->code)
             {
             case PSN_SETACTIVE:
                  // Initialize the controls.
                  InitUnzipPrefs(hwndDlg);
                  break;

             case PSN_APPLY:
                  if (lbs_extract == TRUE)
                     {
                     fLB_Selection = 0;
                     wLBSelection = IDM_EXTRACT;
                     }
                  if (lbs_display == TRUE)
                     {
                     fLB_Selection = 1;
                     wLBSelection = IDM_DISPLAY;
                     }
                  if (lbs_test == TRUE)
                     {
                     fLB_Selection = 2;
                     wLBSelection = IDM_TEST;
                     }
                  uf = ufTemp;
                  lpDCL->noflag = overwrite;
                  lpDCL->ExtractOnlyNewer = newer;
                  lpDCL->PromptToOverwrite = prompt_overwrite;
                  lpDCL->SpaceToUnderscore = space_underscore;
                  lpDCL->fPrivilege = fPrivilege;
                  fUseOldDirSelect = oldstyle;
                  break;

             case PSN_KILLACTIVE:
                  return TRUE;

             case PSN_RESET:
                 // Reset to the original values.
                 break;
             }
          break;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDC_RESCAN_FOR_ARCHIVES:
            ufTemp.fRescan = !ufTemp.fRescan;
            CheckDlgButton(hwndDlg, IDC_RESCAN_FOR_ARCHIVES,
                           ufTemp.fRescan);
            break;
         case IDC_RECREATE_DIR:
            ufTemp.fRecreateDirs = !ufTemp.fRecreateDirs;
            CheckDlgButton(hwndDlg, IDC_RECREATE_DIR,
                           ufTemp.fRecreateDirs);
            break;
         case IDC_RECURSE_ON_SEARCH:
            ufTemp.fRecurseOnSearch = !ufTemp.fRecurseOnSearch;
            CheckDlgButton(hwndDlg, IDC_RECURSE_ON_SEARCH,
                           ufTemp.fRecurseOnSearch);
            break;
         case IDC_ACL:
            if (!dwPlatformId)
               break;
            i = IsDlgButtonChecked(hwndDlg, IDC_ACL);
            if (i)
               {
               fPrivilege = 0;
               CheckDlgButton(hwndDlg, IDC_ACL, FALSE);
               CheckDlgButton(hwndDlg, IDC_PRIVILEGE, FALSE);
               }
            else
               {
               fPrivilege = 1;
               CheckDlgButton(hwndDlg, IDC_ACL, TRUE);
               CheckDlgButton(hwndDlg, IDC_PRIVILEGE, FALSE);
               }
            break;
         case IDC_PRIVILEGE:
            if (!dwPlatformId)
               break;
            i = IsDlgButtonChecked(hwndDlg, IDC_PRIVILEGE);
            if (i)
               {
               fPrivilege = 0;
               CheckDlgButton(hwndDlg, IDC_ACL, FALSE);
               CheckDlgButton(hwndDlg, IDC_PRIVILEGE, FALSE);
               }
            else
               {
               fPrivilege = 2;
               CheckDlgButton(hwndDlg, IDC_ACL, FALSE);
               CheckDlgButton(hwndDlg, IDC_PRIVILEGE, TRUE);
               }
            break;
         case IDC_OVERWRITE:
            overwrite = TRUE;
            newer = FALSE;
            prompt_overwrite = FALSE;
            CheckRadioButton(hwndDlg, IDC_OVERWRITE,
               IDC_PROMPT_OVERWRITE, IDC_OVERWRITE);
            break;
         case IDC_NEWER:
            newer = TRUE;
            overwrite = FALSE;
            prompt_overwrite = FALSE;
            CheckRadioButton(hwndDlg, IDC_OVERWRITE,
               IDC_PROMPT_OVERWRITE, IDC_NEWER);
            break;
         case IDC_PROMPT_OVERWRITE:
            prompt_overwrite = TRUE;
            overwrite = FALSE;
            newer = FALSE;
            CheckRadioButton(hwndDlg, IDC_OVERWRITE,
               IDC_PROMPT_OVERWRITE, IDC_PROMPT_OVERWRITE);
            break;
         case IDC_LF_CRLF:
            {
            char sz1[RESOURCE_STR_LEN], sz2[RESOURCE_STR_LEN];
            ufTemp.fTranslate = !ufTemp.fTranslate;
            if (ufTemp.fTranslate)
               {
               lstrcpy(sz1, GetLoadString(IDS_CRLF_TITLE));
               lstrcpy(sz2, GetLoadString(IDS_CRLF_WARNING));
               if (MessageBox(hwndDlg,
                              sz2,
                              sz1,
                              MB_OKCANCEL | MB_ICONEXCLAMATION) != IDOK)
                  ufTemp.fTranslate = FALSE;
               }
            CheckDlgButton(hwndDlg, IDC_LF_CRLF, ufTemp.fTranslate);
            break;
            }
         case IDC_SPACE_UNDERSCORE:
            space_underscore = !space_underscore;
            CheckDlgButton(hwndDlg, IDC_SPACE_UNDERSCORE,
               space_underscore);
            break;
         case IDC_OLDSTYLE:
            oldstyle = !oldstyle;
            CheckDlgButton(hwndDlg, IDC_OLDSTYLE, oldstyle);
            break;
         case IDC_LBS_EXTRACT:
            lbs_extract = TRUE;
            lbs_display = FALSE;
            lbs_test = FALSE;
            CheckRadioButton(hwndDlg, IDC_LBS_EXTRACT, IDC_LBS_TEST,
                  IDC_LBS_EXTRACT);
            break;
         case IDC_LBS_DISPLAY:
            lbs_display = TRUE;
            lbs_extract = FALSE;
            lbs_test = FALSE;
            CheckRadioButton(hwndDlg, IDC_LBS_EXTRACT, IDC_LBS_TEST,
               IDC_LBS_DISPLAY);
            break;
         case IDC_LBS_TEST:
            lbs_test = TRUE;
            lbs_display = FALSE;
            lbs_extract = FALSE;
            CheckRadioButton(hwndDlg, IDC_LBS_EXTRACT, IDC_LBS_TEST,
               IDC_LBS_TEST);
            break;
         }
      default:
         break;
   }
   return FALSE;
}

