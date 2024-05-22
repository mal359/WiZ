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
#include <shlobj.h>
#include "wiz.h"

//MW #include "helpids.h"

TCHAR szTempDirPath[PATH_MAX];

// Forward references
void InitZipPrefs(HWND);
/****************************************************************************

    FUNCTION: InitZipPrefs(HWND)

    PURPOSE:  Initializes dialog box controls

****************************************************************************/
void InitZipPrefs(HWND hwndDlg)
{
ZPOPT zo;

zo = ZipGetOptions();

if (!zo.fEncryption)
   ShowWindow(GetDlgItem(hwndDlg, IDC_ENCRYPT), SW_HIDE);
CheckDlgButton(hwndDlg, IDC_COMPRESSION_0  + (ZpOpt.fLevel - '0'),
               TRUE);
CheckDlgButton(hwndDlg, IDC_VERBOSE, ZpOpt.fVerbose);
CheckDlgButton(hwndDlg, IDC_MSDOS, ZpOpt.fForce);
CheckDlgButton(hwndDlg, IDC_SYS_FILES, ZpOpt.fSystem);
CheckDlgButton(hwndDlg, IDC_VOL_LABEL, ZpOpt.fVolume);
CheckDlgButton(hwndDlg, IDC_NO_DIR_ENTRY, ZpOpt.fNoDirEntries);
CheckDlgButton(hwndDlg, IDC_IGNORE_DIR, ZpOpt.fJunkDir);
CheckDlgButton(hwndDlg, IDC_CRLFLF, ZpOpt.fCRLF_LF);
CheckDlgButton(hwndDlg, IDC_LFCRLF, ZpOpt.fLF_CRLF);
CheckDlgButton(hwndDlg, IDC_ENCRYPT, ZpOpt.fEncrypt);
CheckDlgButton(hwndDlg, IDC_RECURSE, ZpOpt.fRecurse);
CheckDlgButton(hwndDlg, IDC_MOVE, ZpOpt.fMove);
CheckDlgButton(hwndDlg, IDC_FILETIME, ZpOpt.fLatestTime);
CheckDlgButton(hwndDlg, IDC_UPDATE, ZpOpt.fUpdate);
CheckDlgButton(hwndDlg, IDC_FRESHEN, ZpOpt.fFreshen);
CheckDlgButton(hwndDlg, IDC_COMMENT, ZpOpt.fComment);
CheckDlgButton(hwndDlg, IDC_QUIET, ZpOpt.fQuiet);
CheckDlgButton(hwndDlg, IDC_MAKE_DOS_SFX, (fMakeSFX & fMakeDosSFX));
CheckDlgButton(hwndDlg, IDC_MAKE_W32_SFX, (fMakeSFX & !fMakeDosSFX));
CheckDlgButton(hwndDlg, IDC_ACL, ZpOpt.fExtra);

if ((ZpOpt.szTempDir != NULL) && (ZpOpt.szTempDir[0] != '\0'))
   CheckDlgButton(hwndDlg, IDC_TEMPDIR, TRUE);
else
   CheckDlgButton(hwndDlg, IDC_TEMPDIR, FALSE);
SetWindowText(GetDlgItem(hwndDlg, IDC_TEMPDIR_PATH), szTempDirPath);

if (ZpOpt.fRepair == 1)
   CheckDlgButton(hwndDlg, IDC_REPAIR, TRUE);
else
   CheckDlgButton(hwndDlg, IDC_REPAIR, FALSE);
if (ZpOpt.fRepair ==2)
   CheckDlgButton(hwndDlg, IDC_REPAIR_MORE, TRUE);
else
   CheckDlgButton(hwndDlg, IDC_REPAIR_MORE, FALSE);
CheckDlgButton(hwndDlg, IDC_PRIVILEGE, ZpOpt.fPrivilege);
}

/****************************************************************************

    FUNCTION: ZipPreferencesProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Get Preferences" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/

DWORD WINAPI
ZipPreferencesProc(HWND hwndDlg, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
int i;
   switch (wMessage) {
   case WM_INITDIALOG:
          InitZipPrefs(hwndDlg);
          break;
    /*
     * NOTE: WM_CTLCOLOR is not a supported code under Win 32
     */
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
                  InitZipPrefs(hwndDlg);
                  break;

             case PSN_APPLY:
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
         /* Zip Options */
         case IDC_ZIP_INCLUDE_DATE:
               ZpOpt.fExcludeDate = !ZpOpt.fExcludeDate;
               if (ZpOpt.fExcludeDate)
                  {
                  ZpOpt.fIncludeDate = FALSE;
                  DialogBox(hInst, "GETDATE", hwndDlg, (DLGPROC)GetDateProc);
                  }
               if (ZpOpt.Date[0] == '\0')
                  ZpOpt.fExcludeDate = FALSE;
               CheckDlgButton(hwndDlg, IDC_ZIP_EXCLUDE_DATE, ZpOpt.fExcludeDate);
               CheckDlgButton(hwndDlg, IDC_ZIP_INCLUDE_DATE, ZpOpt.fIncludeDate);
               break;
         case IDC_ZIP_EXCLUDE_DATE:
               ZpOpt.fIncludeDate = !ZpOpt.fIncludeDate;
               if (ZpOpt.fIncludeDate)
                  {
                  ZpOpt.fExcludeDate = FALSE;
                  DialogBox(hInst, "GETDATE", hwndDlg, (DLGPROC)GetDateProc);
                  }
               if (ZpOpt.Date[0] == '\0')
                  ZpOpt.fIncludeDate = FALSE;
               CheckDlgButton(hwndDlg, IDC_ZIP_EXCLUDE_DATE, ZpOpt.fExcludeDate);
               CheckDlgButton(hwndDlg, IDC_ZIP_INCLUDE_DATE, ZpOpt.fIncludeDate);
               break;
         case IDC_TEMPDIR:
               if ((ZpOpt.szTempDir != NULL) && (ZpOpt.szTempDir[0] != '\0'))
                  {
                  CheckDlgButton(hwndDlg, IDC_TEMPDIR, FALSE);
                  szTempDirPath[0] = '\0';
                  ZpOpt.szTempDir = NULL;
                  SetWindowText(GetDlgItem(hwndDlg, IDC_TEMPDIR_PATH), szTempDirPath);
                  }
               else
                  {
                  OPENFILENAME ofn;
                  BROWSEINFO lpbi;
                  ITEMIDLIST * pidList;
                  TCHAR szDisplayName[PATH_MAX];
                  int iImage = 0;

                  ZpOpt.szTempDir = szTempDirPath;
                  if (fUseOldDirSelect)
                     {
                     lstrcpy(szTempDirPath, "mike*"); // Stuff the string to allow return.
                     memset(&ofn, '\0', sizeof(OPENFILENAME)); /* initialize struct */
                     ofn.lStructSize = sizeof(OPENFILENAME);
                     ofn.hwndOwner = hwndDlg;
                     ofn.hInstance = hInst;
                     ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
                     ofn.nFilterIndex = 1;

                     ofn.lpstrFile = szTempDirPath;
                     ofn.nMaxFile = PATH_MAX;
                     ofn.lpstrFileTitle = NULL;
                     ofn.nMaxFileTitle = PATH_MAX; /* ignored ! */
                     ofn.lpstrTitle = (LPSTR)"Set temporary zip directory.";
                     ofn.lpstrInitialDir = NULL;
                     ofn.Flags = OFN_ENABLEHOOK |
                               OFN_HIDEREADONLY|OFN_EXPLORER|OFN_NOVALIDATE;
                     ofn.lpfnHook = (LPOFNHOOKPROC)GetDirProc;
                     if (!GetOpenFileName(&ofn))
                        {
                        break;
                        }

                     ofn.lpstrFile[ofn.nFileOffset-1] = '\0';
                     lstrcpy(szTempDirPath, ofn.lpstrFile);
//                     SetCurrentDirectory(ofn.lpstrFile);
                     }
                  else
                     {
                     lpbi.hwndOwner = hwndDlg;
                     lpbi.pidlRoot = NULL;
                     lpbi.pszDisplayName = szDisplayName;
                     lpbi.lpszTitle = "Set temporary zip directory.";
                     lpbi.ulFlags = BIF_RETURNONLYFSDIRS;
                     lpbi.lpfn = NULL;
                     lpbi.lParam = 0;
                     lpbi.iImage = iImage;
                     if ((pidList = SHBrowseForFolder(&lpbi)) == NULL)
                        {
                        break;
                        }
                     SHGetPathFromIDList(pidList, szTempDirPath);
                     }
                  CheckDlgButton(hwndDlg, IDC_TEMPDIR, TRUE);
                  SetWindowText(GetDlgItem(hwndDlg, IDC_TEMPDIR_PATH), szTempDirPath);
                  }
               break;
         case IDC_COMPRESSION_0:
         case IDC_COMPRESSION_1:
         case IDC_COMPRESSION_2:
         case IDC_COMPRESSION_3:
         case IDC_COMPRESSION_4:
         case IDC_COMPRESSION_5:
         case IDC_COMPRESSION_6:
         case IDC_COMPRESSION_7:
         case IDC_COMPRESSION_8:
         case IDC_COMPRESSION_9:
            for (i = IDC_COMPRESSION_0; i <= IDC_COMPRESSION_9; i++)
                CheckDlgButton(hwndDlg, i, FALSE);

            CheckDlgButton(hwndDlg, LOWORD(wParam), TRUE);
            ZpOpt.fLevel = (char)(LOWORD(wParam) - IDC_COMPRESSION_0 + '0');
            break;
         case IDC_REPAIR:
            i = IsDlgButtonChecked(hwndDlg, IDC_REPAIR);
            if (i)
               {
               ZpOpt.fRepair = 0;
               CheckDlgButton(hwndDlg, IDC_REPAIR, FALSE);
               CheckDlgButton(hwndDlg, IDC_REPAIR_MORE, FALSE);
               }
            else
               {
               ZpOpt.fRepair = 1;
               CheckDlgButton(hwndDlg, IDC_REPAIR, TRUE);
               CheckDlgButton(hwndDlg, IDC_REPAIR_MORE, FALSE);
               }
            break;
         case IDC_REPAIR_MORE:
            i = IsDlgButtonChecked(hwndDlg, IDC_REPAIR_MORE);
            if (i)
               {
               ZpOpt.fRepair = 0;
               CheckDlgButton(hwndDlg, IDC_REPAIR, FALSE);
               CheckDlgButton(hwndDlg, IDC_REPAIR_MORE, FALSE);
               }
            else
               {
               ZpOpt.fRepair = 2;
               CheckDlgButton(hwndDlg, IDC_REPAIR, FALSE);
               CheckDlgButton(hwndDlg, IDC_REPAIR_MORE, TRUE);
               }
            break;
         case IDC_ACL:
            ZpOpt.fExtra = !ZpOpt.fExtra;
            CheckDlgButton(hwndDlg, IDC_ACL, ZpOpt.fExtra);
            if (ZpOpt.fExtra)
               {
               ZpOpt.fPrivilege = FALSE;
               CheckDlgButton(hwndDlg, IDC_PRIVILEGE, FALSE);
               }
            break;
         case IDC_PRIVILEGE:
            if (dwPlatformId)
               {
               ZpOpt.fPrivilege = !ZpOpt.fPrivilege;
               CheckDlgButton(hwndDlg, IDC_PRIVILEGE, ZpOpt.fPrivilege);
               ZpOpt.fExtra = FALSE;
               CheckDlgButton(hwndDlg, IDC_ACL, ZpOpt.fExtra);
               }
            break;
         case IDC_MAKE_DOS_SFX:
            fMakeDosSFX = TRUE;
            if (IsDlgButtonChecked(hwndDlg, IDC_MAKE_W32_SFX))
               fMakeSFX = TRUE;
            else
               fMakeSFX = !fMakeSFX;
            CheckDlgButton(hwndDlg, IDC_MAKE_W32_SFX, FALSE);
            CheckDlgButton(hwndDlg, IDC_MAKE_DOS_SFX, fMakeSFX);
            break;
         case IDC_MAKE_W32_SFX:
            fMakeDosSFX = FALSE;
            if (IsDlgButtonChecked(hwndDlg, IDC_MAKE_DOS_SFX))
               fMakeSFX = TRUE;
            else
               fMakeSFX = !fMakeSFX;
            CheckDlgButton(hwndDlg, IDC_MAKE_DOS_SFX, FALSE);
            CheckDlgButton(hwndDlg, IDC_MAKE_W32_SFX, fMakeSFX);
            break;
         case IDC_MSDOS:
            ZpOpt.fForce = !ZpOpt.fForce;
            CheckDlgButton(hwndDlg, IDC_MSDOS, ZpOpt.fForce);
            break;
         case IDC_SYS_FILES:
            ZpOpt.fSystem = !ZpOpt.fSystem;
            CheckDlgButton(hwndDlg, IDC_SYS_FILES, ZpOpt.fSystem);
            break;
         case IDC_VOL_LABEL:
            ZpOpt.fVolume = !ZpOpt.fVolume;
            CheckDlgButton(hwndDlg, IDC_VOL_LABEL, ZpOpt.fVolume);
            break;
         case IDC_NO_DIR_ENTRY:
            ZpOpt.fNoDirEntries = !ZpOpt.fNoDirEntries;
            CheckDlgButton(hwndDlg, IDC_NO_DIR_ENTRY, ZpOpt.fNoDirEntries);
            break;
         case IDC_IGNORE_DIR:
            ZpOpt.fJunkDir = !ZpOpt.fJunkDir;
            CheckDlgButton(hwndDlg, IDC_IGNORE_DIR, ZpOpt.fJunkDir);
            break;
         case IDC_CRLFLF:
            ZpOpt.fCRLF_LF = !ZpOpt.fCRLF_LF;
            if (ZpOpt.fCRLF_LF)
               {
               ZpOpt.fLF_CRLF = FALSE;
               CheckDlgButton(hwndDlg, IDC_LFCRLF, ZpOpt.fLF_CRLF);
               }
            CheckDlgButton(hwndDlg, IDC_CRLFLF, ZpOpt.fCRLF_LF);
            break;
         case IDC_LFCRLF:
            ZpOpt.fLF_CRLF = !ZpOpt.fLF_CRLF;
            if (ZpOpt.fLF_CRLF)
               {
               ZpOpt.fCRLF_LF = FALSE;
               CheckDlgButton(hwndDlg, IDC_CRLFLF, ZpOpt.fCRLF_LF);
               }
            CheckDlgButton(hwndDlg, IDC_LFCRLF, ZpOpt.fLF_CRLF);
            break;
         case IDC_ENCRYPT:
            ZpOpt.fEncrypt = !ZpOpt.fEncrypt;
            CheckDlgButton(hwndDlg, IDC_ENCRYPT, ZpOpt.fEncrypt);
            break;
         case IDC_RECURSE:
            ZpOpt.fRecurse = !ZpOpt.fRecurse;
            CheckDlgButton(hwndDlg, IDC_RECURSE, ZpOpt.fRecurse);
            break;
         case IDC_MOVE:
            ZpOpt.fMove = !ZpOpt.fMove;
            CheckDlgButton(hwndDlg, IDC_MOVE, ZpOpt.fMove);
            break;
         case IDC_FILETIME:
            ZpOpt.fLatestTime = !ZpOpt.fLatestTime;
            CheckDlgButton(hwndDlg, IDC_FILETIME, ZpOpt.fLatestTime);
            break;
         case IDC_UPDATE:
            ZpOpt.fUpdate = !ZpOpt.fUpdate;
            CheckDlgButton(hwndDlg, IDC_UPDATE, ZpOpt.fUpdate);
            break;
         case IDC_FRESHEN:
            ZpOpt.fFreshen = !ZpOpt.fFreshen;
            CheckDlgButton(hwndDlg, IDC_FRESHEN, ZpOpt.fFreshen);
            break;
         case IDC_COMMENT:
            ZpOpt.fComment = !ZpOpt.fComment;
            CheckDlgButton(hwndDlg, IDC_COMMENT, ZpOpt.fComment);
            break;
         case IDC_QUIET:
            ZpOpt.fQuiet = !ZpOpt.fQuiet;
            CheckDlgButton(hwndDlg, IDC_QUIET, ZpOpt.fQuiet);
            if (ZpOpt.fQuiet == TRUE)
               {
               ZpOpt.fVerbose = FALSE;
               CheckDlgButton(hwndDlg, IDC_VERBOSE, ZpOpt.fVerbose);
               }
            break;
         case IDC_VERBOSE:
            ZpOpt.fVerbose = !ZpOpt.fVerbose;
            CheckDlgButton(hwndDlg, IDC_VERBOSE, ZpOpt.fVerbose);
            if (ZpOpt.fVerbose == TRUE)
               {
               ZpOpt.fQuiet = FALSE;
               CheckDlgButton(hwndDlg, IDC_QUIET, ZpOpt.fQuiet);
               }
            break;
         case IDC_OFFSETS:
            ZpOpt.fOffsets = !ZpOpt.fOffsets;
            CheckDlgButton(hwndDlg, IDC_OFFSETS, ZpOpt.fOffsets);
            break;
         }
      default:
         break;
   }
   return FALSE;
}

