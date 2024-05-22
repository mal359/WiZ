/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

/* Routines for showing the difference between two zip archives.
 * Author: Mike White
 */

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <richedit.h>
#include "wiz.h"
#include "mdichild.h"

HWND hArchive1;
HWND hArchive2;
BOOL fDiffError;
TCHAR szArchive1[PATH_MAX];
TCHAR szArchive2[PATH_MAX];

/* Purpose: Get the window handles associated with the archive names
 * passed in.
 *
 * If successful, the all MDI windows will be minimized except for the two
 * windows wanted for comparison. Those two will be tiled vertically.
 *
 * returns TRUE if successful, FALSE otherwise
 */

BOOL GetWindowHandles(LPSTR Archive1, LPSTR Archive2)
{
HWND hStart;
HWND hTemp  = NULL;
TCHAR szTemp[PATH_MAX];
int i, j;

hArchive1 = hArchive2 = NULL;
hStart = GetActiveMDIChild();
if (!hStart)
   return FALSE;
while (hStart != hTemp)
   {
   SendMessage(hWndListParent, WM_MDINEXT, (WPARAM)(HWND)NULL, (LPARAM) 0);
   hTemp = GetActiveMDIChild();
   GetWindowText(hTemp, szTemp, PATH_MAX);
   if (!lstrcmpi(szTemp, Archive1))
      hArchive1 = hTemp;
   if (!lstrcmpi(szTemp, Archive2))
      hArchive2 = hTemp;
   }
if (!hArchive1 || !hArchive2)
   {
   SendMessage(hWndListParent, WM_MDIACTIVATE, (WPARAM)(HWND)hStart, 0);
   hArchive1 = hArchive2 = NULL;
   return FALSE;
   }
// De-select all listbox items
SendMessage(hWndListParent, WM_MDIACTIVATE, (WPARAM)(HWND)hArchive1, 0);
SendMessage(hWndListParent, WM_MDIRESTORE, (WPARAM)(HWND)hArchive1, 0);
j = ListView_GetItemCount(GetArchiveWindow(NULL)); // Get count of items in listbox
for (i = 0; i < j; i++)
       ListViewSetSel(i, FALSE);

// De-select all listbox items
SendMessage(hWndListParent, WM_MDIACTIVATE, (WPARAM)(HWND)hArchive2, 0);
SendMessage(hWndListParent, WM_MDIRESTORE, (WPARAM)(HWND)hArchive2, 0);
j = ListView_GetItemCount(GetArchiveWindow(NULL)); // Get count of items in listbox
for (i = 0; i < j; i++)
       ListViewSetSel(i, FALSE);
SendMessage(hWndListParent, WM_MDIACTIVATE, (WPARAM)(HWND)hStart, 0);
hTemp = NULL;
while (hStart != hTemp)
   {
   SendMessage(hWndListParent, WM_MDINEXT, (WPARAM)(HWND)NULL, (LPARAM) 0);
   hTemp = GetActiveMDIChild();
   if ((hTemp != hArchive1) && (hTemp != hArchive2))
      {
      ShowWindow(hTemp, SW_HIDE);
      ShowWindow(hTemp, SW_SHOWMINIMIZED);
      }
   }
SendMessage(hWndListParent, WM_MDITILE, (WPARAM)(UINT)MDITILE_VERTICAL, 0);
return TRUE;
}

/*
 * Purpose: Ascertain if there are differences between the archives previously
 * selected (hArchive1 and hArchive2). If there are differences, then the
 * archive entries are hi-lighted in the appropriate listboxes.
 *
 * Returns: TRUE if there are differences.
 *          FALSE if no differences, or if there was a failure in allocating
 *          memory.
 */

BOOL DiffArchives(void)
{
HWND hCurrent;       // Stores currently active MDI child
int iList1, iList2;  // Number of entries in each archive
HWND hList1, hList2; // Handles to archive window listboxes
LPTSTR lpStr1, lpStr2;
HANDLE hMem1, hMem2;
int i, j;
BOOL flag;    // flag that we got a match
int flagT1;    // flag that we got a match
int flagT2;    // flag that we got a match

fDiffError = TRUE;
hCurrent = GetActiveMDIChild();

if (!hArchive1 || !hArchive2 || !hCurrent)
   return FALSE;
hMem1 = GlobalAlloc( GPTR, (PATH_MAX * 2));
if (!hMem1)
   return FALSE;
hMem2 = GlobalAlloc( GPTR, (PATH_MAX * 2));
if (!hMem2)
   {
   GlobalFree(hMem1);
   return FALSE;
   }
lpStr1 = (LPSTR)GlobalLock(hMem1);
if (!lpStr1)
   {
   GlobalFree(hMem1);
   GlobalFree(hMem2);
   return FALSE;
   }
lpStr2 = (LPSTR)GlobalLock(hMem2);
if (!lpStr2)
   {
   GlobalUnlock(lpStr1);
   GlobalFree(hMem1);
   GlobalFree(hMem2);
   return FALSE;
   }
fDiffError = FALSE;
SendMessage(hWndListParent, WM_MDIACTIVATE, (WPARAM)(HWND)hArchive1, 0);
hList1 = GetArchiveWindow(NULL); // Get handle to archive listbox
iList1 = ListView_GetItemCount(hList1); // Get count of items in listbox

SendMessage(hWndListParent, WM_MDIACTIVATE, (WPARAM)(HWND)hArchive2, 0);
hList2 = GetArchiveWindow(NULL); // Get handle to archive listbox
iList2 = ListView_GetItemCount(hList2); // Get count of items in listbox

SendMessage(hWndListParent, WM_MDIACTIVATE, (WPARAM)(HWND)hArchive1, 0);
flagT1 = FALSE;
for (i = 0; i < iList1; i++)
    {
    ListView_GetItemText(hList1, i, 0, lpStr1, PATH_MAX);
    flag = FALSE;
    for (j = 0; j < iList2; j++)
        {
        ListView_GetItemText(hList2, j, 0, lpStr2, PATH_MAX);
        if (!lstrcmpi(lpStr1, lpStr2))
           {
           ListView_GetItemText(hList1, i, 1, lpStr1, PATH_MAX);
           ListView_GetItemText(hList2, i, 1, lpStr2, PATH_MAX);
           if (!lstrcmpi(lpStr1, lpStr2))
              {
              ListView_GetItemText(hList1, i, 2, lpStr1, PATH_MAX);
              ListView_GetItemText(hList2, i, 2, lpStr2, PATH_MAX);
              if (!lstrcmpi(lpStr1, lpStr2))
                 {
                 ListView_GetItemText(hList1, i, 3, lpStr1, PATH_MAX);
                 ListView_GetItemText(hList2, i, 3, lpStr2, PATH_MAX);
                 if (!lstrcmpi(lpStr1, lpStr2))
                    {
                    ListView_GetItemText(hList1, i, 4, lpStr1, PATH_MAX);
                    ListView_GetItemText(hList2, i, 4, lpStr2, PATH_MAX);
                    if (!lstrcmpi(lpStr1, lpStr2))
                       {
                       ListView_GetItemText(hList1, i, 5, lpStr1, PATH_MAX);
                       ListView_GetItemText(hList2, i, 5, lpStr2, PATH_MAX);
                       if (!lstrcmpi(lpStr1, lpStr2))
                          {
                          flag = TRUE;
                          flagT1++;
                          }
                       }
                    }
                 }
              }
           }
        }
    if (!flag)
       ListViewSetSel(i, TRUE);
    }

flagT2 = FALSE;
SendMessage(hWndListParent, WM_MDIACTIVATE, (WPARAM)(HWND)hArchive2, 0);
for (i = 0; i < iList2; i++)
    {
    ListView_GetItemText(hList2, i, 0, lpStr2, PATH_MAX);
    flag = FALSE;
    for (j = 0; j < iList1; j++)
        {
        ListView_GetItemText(hList1, j, 0, lpStr1, PATH_MAX);
        if (!lstrcmpi(lpStr1, lpStr2))
           {
           ListView_GetItemText(hList2, i, 1, lpStr2, PATH_MAX);
           ListView_GetItemText(hList1, i, 1, lpStr1, PATH_MAX);
           if (!lstrcmpi(lpStr1, lpStr2))
              {
              ListView_GetItemText(hList2, i, 2, lpStr2, PATH_MAX);
              ListView_GetItemText(hList1, i, 2, lpStr1, PATH_MAX);
              if (!lstrcmpi(lpStr1, lpStr2))
                 {
                 ListView_GetItemText(hList2, i, 3, lpStr2, PATH_MAX);
                 ListView_GetItemText(hList1, i, 3, lpStr1, PATH_MAX);
                 if (!lstrcmpi(lpStr1, lpStr2))
                    {
                    ListView_GetItemText(hList2, i, 4, lpStr2, PATH_MAX);
                    ListView_GetItemText(hList1, i, 4, lpStr1, PATH_MAX);
                    if (!lstrcmpi(lpStr1, lpStr2))
                       {
                       ListView_GetItemText(hList2, i, 5, lpStr2, PATH_MAX);
                       ListView_GetItemText(hList1, i, 5, lpStr1, PATH_MAX);
                       if (!lstrcmpi(lpStr1, lpStr2))
                          {
                          flag = TRUE;
                          flagT2++;
                          }
                       }
                    }
                 }
              }
           }
        }
    if (!flag)
       ListViewSetSel(i, TRUE);
    }

// Free up memory
GlobalUnlock(lpStr1);
GlobalUnlock(lpStr2);
GlobalFree(hMem1);
GlobalFree(hMem2);
// Set the active MDI window back to the original window
SendMessage(hWndListParent, WM_MDIACTIVATE, (WPARAM)(HWND)hCurrent, 0);
if ((flagT1 != iList1) || (flagT2 != iList2))
   return TRUE;
else
   return FALSE;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI DiffArchiveProc(HWND hDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
int i;
TCHAR szTemp[PATH_MAX];
HWND hStart, hTemp = NULL;

    switch (wMessage)
    {

    case WM_INITDIALOG:
         szArchive1[0] = '0';
         szArchive2[0] = '0';
         hStart = GetActiveMDIChild();
         while (hStart != hTemp)
            {
            SendMessage(hWndListParent, WM_MDINEXT, (WPARAM)(HWND)NULL, (LPARAM) 0);
            hTemp = GetActiveMDIChild();
            GetWindowText(hTemp, szTemp, PATH_MAX);
            }
         CenterDialog(NULL, hDlg);
         break;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDCANCEL:
            EndDialog(hDlg, wParam);
            break;
         case IDOK:
            {
            int ppSelItems[2];
            i = SendMessage(GetDlgItem(hDlg, IDC_LISTBOX_DIFF), LB_GETSELCOUNT, 0, 0L);
            if (i == 2)
               {
               SendMessage(GetDlgItem(hDlg, IDC_LISTBOX_DIFF),
                           LB_GETSELITEMS,
                           (WPARAM) 2,
                           (LPARAM)(LPINT)(long)&ppSelItems);
               SendMessage(GetDlgItem(hDlg, IDC_LISTBOX_DIFF),
                           LB_GETTEXT,
                           (WPARAM)ppSelItems[0],
                           (LPARAM)(LPCTSTR)&szArchive1);
               SendMessage(GetDlgItem(hDlg, IDC_LISTBOX_DIFF),
                           LB_GETTEXT,
                           (WPARAM)ppSelItems[1],
                           (LPARAM)(LPCTSTR)&szArchive2);
               EndDialog(hDlg, wParam);
               }
            }
            break;
         }
      default:
         break;
   }
   return FALSE;
}

