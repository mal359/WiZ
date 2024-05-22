/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <stdio.h>
#include "wiz.h"

HWND hProgressWnd;
HWND hProgressBar;

TCHAR szGlobalTemp[PATH_MAX * 2];
unsigned long sizeProcessed;
int iFilesProcessed;
int iGlobalCancel = 0;
WORD wGlobalMessage;
int FirstPass = 0;

/****************************************************************************

    FUNCTION: ProgressBar(void)

    PURPOSE:  Shows progress of operation

****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
void ProgressBar(void)
{
RECT rcClient;  // client area of parent window
int i;
CHARRANGE cr;
int cyVScroll;  // height of scroll bar arrow

GetClientRect(hWndMain, &rcClient);
cyVScroll = GetSystemMetrics(SM_CYVSCROLL);
hProgressWnd = CreateWindowEx(0,//WS_EX_CLIENTEDGE,
                              PROGRESS_CLASS,
                              (LPSTR) NULL,
                              WS_CHILD | WS_VISIBLE,
                              rcClient.left,
                              rcClient.bottom - (2 *cyVScroll),
                              rcClient.right,
                              cyVScroll,
                              hWndMain,
                              (HMENU) 0,
                              hInst,
                              NULL);

// Set the range and increment of the progress bar.
SendMessage(hProgressWnd, PBM_SETRANGE, 0,
            MAKELPARAM(0, dFilesToProcess));
SendMessage(hProgressWnd, PBM_SETSTEP, (WPARAM) 1, 0);

//CenterDialog(NULL, hwndDlg);
FirstPass = 0;
iFilesProcessed = 0;
iGlobalCancel = FALSE;
SendMessage(hWndMessage, WM_SETREDRAW, FALSE, 0L);
if (wGlobalMessage == IDM_ZIP_TARGET)
   {
   ZipSetOptions(&ZpOpt);
   iGlobalReturnCode = ZipArchive(ZpZCL);
   }
else
   Action(GetArchiveWindow(NULL), (int)wGlobalMessage);
SendMessage(hWndMessage, WM_SETREDRAW, TRUE, 0L);
InvalidateRect(hWndMessage, NULL, FALSE);
cr.cpMin = cr.cpMax = -1;
i = SendMessage(hWndMessage, EM_GETLINECOUNT, 0, 0);
if (i > 10)
   SendMessage(hWndMessage, EM_LINESCROLL, (WPARAM) 0, (LPARAM) (i-10));
SendMessage(hWndMessage, EM_EXSETSEL, 0, (LPARAM) &cr);
DestroyWindow(hProgressWnd);
}

/****************************************************************************

    FUNCTION: ServiceProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Service App" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI
ServiceProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
static HWND hOK, hCANCEL, hProgress;
RECT rcClient;  // client area of parent window
int i;
CHARRANGE cr;
int cyVScroll;  // height of scroll bar arrow

   switch (wMessage) {
   case WM_INITDIALOG:
      hOK = GetDlgItem(hwndDlg, IDOK);
      hCANCEL = GetDlgItem(hwndDlg, IDCANCEL);
      hProgress = GetDlgItem(hwndDlg, IDC_PROGRESS_TEXT);
      CenterDialog(NULL, hwndDlg);
      hProgressWnd = hwndDlg;
      SetDlgItemText(hwndDlg, IDC_PROGRESS_TEXT, "Pre-Processing Archive");
      SetDlgItemText(hwndDlg, IDC_PROGRESS_FILE_NAME, "");
      FirstPass = 0;
      iFilesProcessed = 0;
      iGlobalCancel = FALSE;
      switch (wGlobalMessage)
         {
         case IDM_EXTRACT:
            {
            SetWindowText(hProgressWnd, "Extracting");
            break;
            }
         case IDM_UPDATE_ZIP:
            {
            SetWindowText(hProgressWnd, "Updating");
            break;
            }
         case IDM_ZIP_TARGET:
            {
            SetWindowText(hProgressWnd, "Archiving");
            break;
            }
         case IDM_TEST:
            {
            SetWindowText(hProgressWnd, "Testing");
            break;
            }
         case IDM_GET_ZIPINFO:
            {
            SetWindowText(hProgressWnd, "Getting Archive Information");
            break;
            }
         case IDM_ZIP_DELETE_ENTRIES:
            {
            SetWindowText(hProgressWnd, "Deleting Files in Archive");
            break;
            }
         }

         if (wGlobalMessage != IDM_ZIP_TARGET)
         {
         SetDlgItemText(hwndDlg, IDC_PROGRESS_TEXT, "");
         SetDlgItemText(hwndDlg, IDC_PROGRESS_FILE_NAME, "");

         GetClientRect(hwndDlg, &rcClient);
         cyVScroll = GetSystemMetrics(SM_CYVSCROLL);
         hProgressBar = CreateWindowEx(WS_EX_CLIENTEDGE,
                              PROGRESS_CLASS,
                              (LPSTR) NULL,
                              WS_CHILD | WS_VISIBLE,
                              rcClient.left + cyVScroll,
                              rcClient.top + (2 *cyVScroll),
                              rcClient.right - (2 *cyVScroll),
                              cyVScroll,
                              hwndDlg,
                              (HMENU) 0,
                              hInst,
                              NULL);
      }
      PostMessage(hwndDlg, WM_COMMAND, IDOK, 0);
      return TRUE;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDC_PROGRESS_UPDATE:
            {
            TCHAR szStr[PATH_MAX * 2];
            sprintf(szStr, "Processed file %d (size = %ld)",
                    iFilesProcessed, sizeProcessed);
            SetDlgItemText(hwndDlg, IDC_PROGRESS_TEXT, szStr);
            SetDlgItemText(hwndDlg, IDC_PROGRESS_FILE_NAME, szGlobalTemp);
            break;
            }
        case IDOK:
            FirstPass = 1; /* So that cancel button doesn't cause early
                              exit from dialog. */
            SendMessage(hWndMessage, WM_SETREDRAW, FALSE, 0L);
            if (wGlobalMessage == IDM_ZIP_TARGET)
               {
               // Set the range and increment of the progress bar.
               SendMessage(hProgressBar, PBM_SETRANGE, 0,
                           MAKELPARAM(0, dFilesToProcess));
               SendMessage(hProgressBar, PBM_SETSTEP, (WPARAM) 1, 0);

               ZipSetOptions(&ZpOpt);
               iGlobalReturnCode = ZipArchive(ZpZCL);
               }
            else
               Action(GetArchiveWindow(NULL), (int)wGlobalMessage);
            SendMessage(hWndMessage, WM_SETREDRAW, TRUE, 0L);
            InvalidateRect(hWndMessage, NULL, FALSE);
            cr.cpMin = cr.cpMax = -1;
            i = SendMessage(hWndMessage, EM_GETLINECOUNT, 0, 0);
            if (i > 10)
               SendMessage(hWndMessage, EM_LINESCROLL, (WPARAM) 0, (LPARAM) (i-10));
            SendMessage(hWndMessage, EM_EXSETSEL, 0, (LPARAM) &cr);
            PostMessage(hwndDlg, WM_CLOSE, 0, 0L);
            break;
        case IDCANCEL:
            if (!FirstPass)
               PostMessage(hwndDlg, WM_CLOSE, 0, 0L);
            iGlobalCancel = TRUE;
            break;
      }
      return TRUE;
   case WM_CLOSE:
      FirstPass = 0;
      hProgressWnd = hWndMain;
      DestroyWindow(hProgressBar);
      EndDialog(hwndDlg, TRUE);
      return TRUE;
   }
   return FALSE;
}

/****************************************************************************

    FUNCTION: ServiceQueue(LPSTR, unsigned long)

    PURPOSE: Process cancel messages, enable cancel, update progress meter

****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
int WINAPI ServiceQueue(LPCSTR filename, unsigned long size)
{
lstrcpy(szGlobalTemp, filename);
sizeProcessed = size;
iFilesProcessed++;
if (wGlobalMessage != IDM_ZIP_TARGET)
   SendMessage(hProgressBar, PBM_STEPIT, 0, 0);
else
   SendMessage(hProgressWnd, WM_COMMAND, (WPARAM) IDC_PROGRESS_UPDATE, 0);
EmptyMessageQueue();
return iGlobalCancel;
}

