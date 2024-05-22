/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "wiz.h"
#ifdef __BORLANDC__
#include <dir.h>
#else
#include <direct.h>
#endif

//MW #include "helpids.h"

// This is the length of a string that is required before a horizontal
// scroll bar shows up, and the basis for computing the horizontal extent
// required for the scroll bar.
#define SCROLLWIDTH 15

int cSelLBItems;
int numFiles;
char **pszIndex;
char *sz;
TCHAR baseDir[PATH_MAX], tempName[PATH_MAX];
LPSTR szFileList;
HANDLE hFileList;
char szPattern[PATH_MAX], szLastPat[PATH_MAX];

/****************************************************************************
    FUNCTION GetCapitalization(TCHAR *)

    PURPOSE  Get actual case sensitive directory names

****************************************************************************/

void GetCapitalization(TCHAR *s)
{
if (lstrlen(s) > 3)
   {
   HANDLE hFoundFile;
   WIN32_FIND_DATA fileinfo;
   TCHAR sz1[PATH_MAX], sz2[PATH_MAX], *p1, *p2, c;

   lstrcpy(sz1, s);
   lstrcpy(sz2, s);
   sz2[3] = '\0';
   p1 = p2 = &sz1[3];
   while (p2 != NULL)
         {
         p2 = strchr(p1, '\\');
         if (p2 != NULL)
            {
            c = p2[0];
            p2[0] = '\0';
            hFoundFile = FindFirstFile(sz1, &fileinfo);
            lstrcat(sz2, fileinfo.cFileName);
            lstrcat(sz2, "\\");
            FindClose(hFoundFile);
            p2[0] = c;
            p1 = ++p2;
            }
         }
   hFoundFile = FindFirstFile(sz1, &fileinfo);
   lstrcat(sz2, fileinfo.cFileName);
   FindClose(hFoundFile);
   lstrcpy(s, sz2);
   }
}

/****************************************************************************

    FUNCTION: GetFilesProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Get Files For Archive" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/
BOOL WINAPI
GetFilesProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
char szFName[PATH_MAX];
int * *ppnSelItems;
int i, *pnSelItems, iScrollWidth=0;
HGLOBAL pHND;
HWND hTemp;

   switch (wMessage) {
   case WM_INITDIALOG:
      /* This hides the actual OK button associated with the file open
         dialog. We basically want to trap this button, as pressing
         enter will otherwise cause the OK button to be pressed, and the
         dialog to terminate.
       */
      hTemp = GetDlgItem(hwndDlg, IDOK);
      WinAssert(hTemp);
      ShowWindow(hTemp, SW_HIDE);
      ZpOpt.szRootDir[0] = '\0';
      lstrcpy(szLastPat, "*.*");
      numFiles = 0;
      ZpZCL.argc = 0;
      SetWindowText(GetDlgItem(hwndDlg, IDC_RECURSE_EDITBOX), "*.*");
      CenterDialog(NULL, hwndDlg); /* center on parent */
      break;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDC_TEMP_PREFERENCES:
            {
            DoPropertySheet(hwndDlg);
            }
            break;
         case IDOK:
            if ((HWND)lParam != NULL)
               {
               GetWindowText(GetDlgItem(hwndDlg, edt1), szLastPat, PATH_MAX);
               break;
               }
         case IDC_ADD:
            {
            cSelLBItems = (int)SendMessage(GetDlgItem(hwndDlg, lst1),
               LB_GETSELCOUNT, 0, 0L);
            if (cSelLBItems == (int)SendMessage(GetDlgItem(hwndDlg, lst1),
               LB_GETCOUNT, 0, 0L))
               {
               GetCurrentDirectory(PATH_MAX, baseDir);
               GetCapitalization(baseDir);
               if (baseDir[lstrlen(baseDir)-1] != '\\')
                  lstrcat(baseDir, "\\");
               lstrcat(baseDir, szLastPat);
               SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                  LB_INSERTSTRING, 0, (LONG) ((LPSTR) baseDir));
               if (lstrlen(baseDir) > iScrollWidth)
                  {
                  iScrollWidth = lstrlen(baseDir);
                  if (iScrollWidth > SCROLLWIDTH)
                     SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                                 LB_SETHORIZONTALEXTENT,
                                 (WPARAM)(dxChar*iScrollWidth),
                                 0);
                  }
               return TRUE;
               }
            if (cSelLBItems)
               {
               pHND = GlobalAlloc(GMEM_FIXED, cSelLBItems * sizeof(int));
               WinAssert(pHND);

               pnSelItems = (int *)GlobalLock(pHND);
               ppnSelItems = &pnSelItems;
               if ( !*ppnSelItems )
                  {
                  GlobalFree(pHND);
                  return -1;
                  }

               /* Get list of selected items. Return value is number of
                  selected items */
               if (SendMessage(GetDlgItem(hwndDlg, lst1), LB_GETSELITEMS,
                  cSelLBItems, (LONG)*ppnSelItems) != cSelLBItems)
                  {
                  GlobalUnlock(pHND);
                  GlobalFree(pHND);
                  return -1;
                  }
               GetCurrentDirectory(PATH_MAX, baseDir);
               GetCapitalization(baseDir);
               if (baseDir[lstrlen(baseDir)-1] != '\\')
                  lstrcat(baseDir, "\\");
               for (i = cSelLBItems - 1; i >= 0; i--)
                  {
                  SendMessage(GetDlgItem(hwndDlg, lst1), LB_GETTEXT,
                     pnSelItems[i],(LPARAM) (LPCSTR) szFName);
                  lstrcpy(tempName, baseDir);
                  lstrcat(tempName, szFName);
                  SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                     LB_INSERTSTRING, 0, (LONG)(LPSTR) tempName);
                  if (lstrlen(tempName) > iScrollWidth)
                     {
                     iScrollWidth = lstrlen(tempName);
                     if (iScrollWidth > SCROLLWIDTH)
                        SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                                    LB_SETHORIZONTALEXTENT,
                                    (WPARAM)(dxChar*iScrollWidth),
                                    0);
                     }
                  numFiles++;
                  }
               GlobalUnlock(pHND);
               GlobalFree(pHND);
               }
            SendMessage(GetDlgItem(hwndDlg, lst1) , LB_SELITEMRANGE,
               (WPARAM)FALSE,
                MAKELONG(0,-1));
            SetWindowText(GetDlgItem(hwndDlg, edt1), szLastPat);
            }
            return TRUE;
         case IDC_DELETE:
            {
            int iNumEntries;
            cSelLBItems = (int)SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
               LB_GETSELCOUNT, 0, 0L);

            if (cSelLBItems)
               {
               pHND = GlobalAlloc(GMEM_FIXED, cSelLBItems * sizeof(int));
               WinAssert(pHND);

               pnSelItems = (int *)GlobalLock(pHND);
               ppnSelItems = &pnSelItems;
               if ( !*ppnSelItems )
                  {
                  GlobalFree(pHND);
                  return -1;
                  }

               /* Get list of selected items. */
               if (SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST), LB_GETSELITEMS,
                  cSelLBItems, (LONG)*ppnSelItems) != cSelLBItems)
                  {
                  GlobalUnlock(pHND);
                  GlobalFree(pHND);
                  return -1;
                  }

               for (i = cSelLBItems - 1; i >= 0; i--)
                  {
                  SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST), LB_DELETESTRING,
                     pnSelItems[i],0);
                  numFiles--;
                  }
               GlobalUnlock(pHND);
               GlobalFree(pHND);
               }
            iNumEntries = SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                                      LB_GETCOUNT, 0, 0);
            iScrollWidth = 0;
            if (iNumEntries > 0)
               {
               int j;
               for (i = 0; i < iNumEntries; i++)
                   {
                   j = SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                               LB_GETTEXTLEN, (WPARAM)i, 0);
                   if (j > iScrollWidth)
                      {
                      iScrollWidth = j;
                      if (iScrollWidth > SCROLLWIDTH)
                         SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                                     LB_SETHORIZONTALEXTENT,
                                    (WPARAM)(dxChar*iScrollWidth),
                                    0);
                      }
                   }
               }
            else
               {
                SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                            LB_SETHORIZONTALEXTENT,
                            (WPARAM) 0,
                            0);
               }
            }
            break;
         case IDC_SET_ROOT:
           {
           GetCurrentDirectory(PATH_MAX, ZpOpt.szRootDir);
           GetCapitalization(ZpOpt.szRootDir);
           SetWindowText(GetDlgItem(hwndDlg, IDC_ROOT_DIR), ZpOpt.szRootDir);
           }
            break;
         case IDC_FREE_ROOT:
            ZpOpt.szRootDir[0] = '\0';
            SetWindowText(GetDlgItem(hwndDlg, IDC_ROOT_DIR), ZpOpt.szRootDir);
            break;
         case IDC_INCLUDE:
            {
            GetCurrentDirectory(PATH_MAX, baseDir);
            GetCapitalization(baseDir);
            lstrcat(baseDir, "\\");
            GetWindowText(GetDlgItem(hwndDlg, IDC_RECURSE_EDITBOX),
                          tempName,
                          PATH_MAX);
            if (lstrlen(tempName) == 0)
               break;
            lstrcat(baseDir, tempName);
            SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
               LB_INSERTSTRING, 0, (LONG) ((LPSTR) baseDir));
            if (lstrlen(baseDir) > iScrollWidth)
               {
               iScrollWidth = lstrlen(baseDir);
               if (iScrollWidth > SCROLLWIDTH)
                   SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                               LB_SETHORIZONTALEXTENT,
                               (WPARAM)(dxChar*iScrollWidth),
                               0);
               }
            }
            break;
         case IDC_RECURSE:
            {
            char *p, drive[2];
            BOOL fRecurseOnSearch;
            int iNumEntries;
            /* We always want to recurse here */
            fRecurseOnSearch = uf.fRecurseOnSearch;
            uf.fRecurseOnSearch = TRUE;
            GetCurrentDirectory(PATH_MAX, baseDir);
            GetCapitalization(baseDir);
            lstrcat(baseDir, "\\");
            GetWindowText(GetDlgItem(hwndDlg, IDC_RECURSE_EDITBOX),
                          tempName,
                          PATH_MAX);
            if (lstrlen(tempName) == 0)
               break;
            lstrcat(baseDir, tempName);
            drive[0] = baseDir[0];
            drive[1] = '\0';
            p = &baseDir[2];
            FindFile(drive, p, hwndDlg);
            uf.fRecurseOnSearch = fRecurseOnSearch;
            iNumEntries = SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                                      LB_GETCOUNT, 0, 0);
            if (iNumEntries > 0)
               {
               int j;
               for (i = 0; i < iNumEntries; i++)
                   {
                   j = SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                               LB_GETTEXTLEN, (WPARAM)i, 0);
                   if (j > iScrollWidth)
                      {
                      iScrollWidth = j;
                      if (iScrollWidth > SCROLLWIDTH)
                         SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                                     LB_SETHORIZONTALEXTENT,
                                    (WPARAM)(dxChar*iScrollWidth),
                                    0);
                      }
                   }
               }
            }
            break;
         case IDC_SELECT_ALL:
         case IDC_DESELECT_ALL:
            {
            WORD cZippedFiles;
            cZippedFiles = (WORD)SendMessage(GetDlgItem(hwndDlg, lst1),
               LB_GETCOUNT, 0, 0L);
            SendMessage(GetDlgItem(hwndDlg, lst1) , LB_SELITEMRANGE,
               (WPARAM)(LOWORD(wParam) == IDC_DESELECT_ALL ? FALSE : TRUE),
               MAKELONG(0, (cZippedFiles-1)));
            }
            SetWindowText(GetDlgItem(hwndDlg, edt1), szLastPat);
            break;
         case IDCANCEL:
            ZpZCL.lpszZipFN[0] = '\0';
            lpumb->szZipFileName[0] = '\0';
            break;
         case IDC_DONE:
            {
            int cc;
            char **index;

            SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST) , LB_SETSEL,
                        (WPARAM)TRUE, -1);
            numFiles = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                        LB_GETSELCOUNT, 0, 0L);

            cSelLBItems = numFiles;

            if (cSelLBItems < 1)
               {
               if (!ZpOpt.fRepair && !ZpOpt.fOffsets &&!ZpOpt.fFreshen)
                  {
                  ZpZCL.lpszZipFN[0] = '\0';
                  lpumb->szZipFileName[0] = '\0';
                  }
               ZpZCL.argc = 0;
               goto pau;
               }

            ZpZCL.argc = cSelLBItems; /* keep count to transfer to dll */

            hFileList = GlobalAlloc( GPTR, ((PATH_MAX + 1) * cSelLBItems));
            if ( hFileList )
               {
               szFileList = (char *)GlobalLock(hFileList);
               }
            else
               {
               MessageBox(hWndMain, "Try archiving fewer files at a time",
                  "Insufficient Memory", MB_OK |MB_SYSTEMMODAL|
                  MB_ICONEXCLAMATION);
               ZpZCL.lpszZipFN[0] = '\0';
               lpumb->szZipFileName[0] = '\0';
               goto pau;
               }

            if (cSelLBItems)
               {
               index = (char **)szFileList;
               cc = (sizeof(char *) * ZpZCL.argc);
               sz = szFileList + cc;

               for (i = 0; i < ZpZCL.argc; i++)
                  {
                  cc = (int)SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST),
                     LB_GETTEXT, i,(LPARAM) (LPCSTR) szFName);
                  lstrcpy(sz, szFName);
                  index[i] = sz;
                  sz += (cc + 1);
                  }
               }
            }
            SendMessage(GetDlgItem(hwndDlg, IDC_FILE_LIST) , LB_SELITEMRANGE,
               (WPARAM)FALSE,
                MAKELONG(0,-1));
            SendMessage(GetDlgItem(hwndDlg, lst1) , LB_SELITEMRANGE,
               (WPARAM)FALSE,
                MAKELONG(0,-1));
pau:
            EndDialog(hwndDlg, TRUE);
            break;
         }
      default:
         break;
   }
   return FALSE;
}

