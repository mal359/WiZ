/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <windows.h>
#include "wiz.h"

/*
 *      CenterDialog
 *
 *      Purpose:
 *              Moves the dialog specified by hwndDlg so that it is centered on
 *              the window specified by hwndParent. If hwndParent is null,
 *              hwndDlg gets centered on the screen.
 *
 *              Should be called while processing the WM_INITDIALOG message
 *              from the dialog's DlgProc().
 *
 *      Arguments:
 *              HWND    parent hwnd
 *              HWND    dialog's hwnd
 *
 *      Returns:
 *              Nothing.
 *
 */
void
CenterDialog(HWND hwndParent, HWND hwndDlg)
{
RECT    rectDlg;
RECT    rect;
int             dx;
int             dy;

if (hwndParent == NULL)
   {
   rect.top = rect.left = 0;
   rect.right = GetSystemMetrics(SM_CXSCREEN);
   rect.bottom = GetSystemMetrics(SM_CYSCREEN);
   }
else
   {
   GetWindowRect(hwndParent, &rect);
   }

GetWindowRect(hwndDlg, &rectDlg);
OffsetRect(&rectDlg, -rectDlg.left, -rectDlg.top);

dx = (rect.left + (rect.right - rect.left -
     rectDlg.right) / 2 + 4) & ~7;
dy = rect.top + (rect.bottom - rect.top -
     rectDlg.bottom) / 2;
WinAssert(hwndDlg);
MoveWindow(hwndDlg, dx, dy, rectDlg.right, rectDlg.bottom, 0);
}

/*
 * CleanScreen(void)
 *
 * Used to refresh the various client areas of wiz after dialog boxes
 * have obscured them. Refreshing the client area of hWndMain doesn't work
 * because some of the MDI children are not refreshed properly.
 */
void CleanScreen(void)
{
InvalidateRect(hWndListParent, NULL, TRUE);
UpdateWindow(hWndListParent);
InvalidateRect(hWndToolbar, NULL, TRUE);
UpdateWindow(hWndToolbar);
InvalidateRect(hTabCtrl, NULL, TRUE);
UpdateWindow(hTabCtrl);
InvalidateRect(hWndArchivesFound, NULL, TRUE);
UpdateWindow(hWndArchivesFound);
InvalidateRect(hWndGrep, NULL, TRUE);
UpdateWindow(hWndGrep);
}

void DisplaySelectionTotals(void)
{
int iTotalSelected, i, j;
HWND hCurrentArchive;
LV_ITEM lvi;
unsigned long dTotalSize = 0;
char sz[PATH_MAX * 2], arcName[PATH_MAX];

hCurrentArchive = GetArchiveWindow(NULL);
if (hCurrentArchive == NULL)
   return;
iTotalSelected = ListView_GetSelectedCount(hCurrentArchive);
j = ListView_GetItemCount(hCurrentArchive);
for (i = 0; i < j; i++)
   {
   lvi.mask      = LVIF_STATE;
   lvi.stateMask = LVIS_SELECTED;
   lvi.iItem     = i;
   lvi.iSubItem  = 0;

   ListView_GetItem(hCurrentArchive, &lvi);

   if (lvi.state)
      {
      ListView_GetItemText(hCurrentArchive,
                           i,
                           1,
                           sz,
                           (PATH_MAX * 2));
      dTotalSize += atol(sz);
      }
   }
GetWindowText(GetActiveMDIChild(), arcName, PATH_MAX);   
sprintf(sz, "%d Files Selected, %u bytes in %s",
        iTotalSelected,
        dTotalSize,
        arcName);
UpdateStatusBar(sz, 0, 0);
}

