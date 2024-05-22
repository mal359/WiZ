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
#ifdef __BORLANDC__
#include <dir.h>
#else
#include <direct.h>
#endif
#include "wiz.h"
#include "unzip\unzip.h"

//MW #include "helpids.h"

TCHAR SearchPattern[PATH_MAX];
char DELIMS[] = " \t,"; /* delimiters, mostly whitespace */
PSTR psLBEntry = NULL;    /* list box entry */
PSTR psPatternBuf = NULL; /* pattern collection */
PSTR psGrepBuf = NULL;    /* pattern collection */
BOOL fGreppingArchives = FALSE;

void freePatMem(void);

void freePatMem(void)
{
if (psLBEntry)
   {
   GlobalFree((HGLOBAL)psLBEntry);
   psLBEntry = NULL;
   }

if (psGrepBuf)
   {
   GlobalFree((HGLOBAL)psGrepBuf);
   psGrepBuf = NULL;
   }

if (psPatternBuf)
   {
   GlobalFree((HGLOBAL)psPatternBuf);
   psPatternBuf = NULL;
   }
}

/****************************************************************************

    FUNCTION: GrepArchiveProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Grep Archive" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI
GrepArchiveProc(HWND hDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{

switch (wMessage) {
   case WM_INITDIALOG:
      SearchPattern[0] = '\0'; /* Init pattern */

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
      CommDlg_OpenSave_HideControl(GetParent(hDlg), cmb1);
      CommDlg_OpenSave_HideControl(GetParent(hDlg), stc2);
      if (fScanArchives)
         {
         CommDlg_OpenSave_HideControl(GetParent(hDlg), edt1);
         CommDlg_OpenSave_HideControl(GetParent(hDlg), stc3);
         CommDlg_OpenSave_SetControlText(GetParent(hDlg),
            IDOK, "Scan");
         }
      else
         {
         char sz1[RESOURCE_STR_LEN];
         lstrcpy(sz1, GetLoadString(IDS_FILESPEC));
         CommDlg_OpenSave_SetControlText(GetParent(hDlg),
            stc3, sz1);
         lstrcpy(sz1, GetLoadString(IDS_SEARCH_NAME));
         CommDlg_OpenSave_SetControlText(GetParent(hDlg),
            IDOK, sz1);
         }
      break;
   }
return DefWindowProc(hDlg, wMessage, wParam, lParam);
}

char ScannedName[PATH_MAX];
BOOL fScanning = FALSE;

void SearchArchive(LPSTR sz)
{
lpDCL->ncflag = 0;
lpDCL->ntflag = 0;
lpDCL->nvflag = 2; //Tells dll to give verbose information
lpDCL->nfflag = 0;
lpDCL->nzflag = 0;
lpDCL->ndflag = 0;
lpDCL->noflag = 0;
lpDCL->naflag = 0;

fScanning = TRUE;
lpDCL->lpszExtractDir = NULL;
uzpParameters.argc = 0;
uzpParameters.argv = NULL;

lpDCL->lpszZipFN = sz;
iGlobalReturnCode =
   (*Unz_SingleEntryPoint)(uzpParameters.argc,
                           uzpParameters.argv,
                           uzpParameters.xfnc,
                           uzpParameters.xfnv,
                           uzpParameters.lpDCL,
                           uzpParameters.lpUserFunctions);
fScanning = FALSE;
}


/****************************************************************************

    FUNCTION: GrepPatternSelectProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Select Files by grepping" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI
GrepPatternSelectProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
static HWND hSelect, hDeselect, hPattern, hGrepPattern, hTemp, hTmp;
DWORD cListItems;   /* no. items in listbox   */
UINT Fname_Inx;      /* index into LB entry   */
int nPatternLength = 0, nGrepLength = 0;   /* length of data in pattern edit windows */
int ret, iTemp;
BOOL fSelect;
char *p, szArchiveName[PATH_MAX];

   switch (wMessage) {
   case WM_INITDIALOG:
      hSelect = GetDlgItem(hwndDlg, IDOK);
      hDeselect = GetDlgItem(hwndDlg, IDC_PATTERN_DESELECT);
      hPattern = GetDlgItem(hwndDlg, IDC_FILE_PATTERN);
      hGrepPattern = GetDlgItem(hwndDlg, IDC_GREP_PATTERN);
      CheckDlgButton(hwndDlg, IDC_CASE_SENSITIVE,
                     uf.fCaseSensitive ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hwndDlg, IDC_WHOLEWORDS,
                     uf.fWholeWords ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hwndDlg, IDC_IGNORE_BINARY,
                     uf.fIgnoreBinary ? BST_CHECKED : BST_UNCHECKED);
      CenterDialog(NULL, hwndDlg);
      if (fGreppingArchives)
         {
         hTemp = GetDlgItem(hwndDlg, IDC_FILE_PATTERN_TEXT);
         hTmp = GetDlgItem(hwndDlg, IDC_FILE_PATTERN_EXAM);
         SetDlgItemText(hwndDlg, IDOK, "Ok");
         SetDlgItemText(hwndDlg, IDCANCEL, "Cancel");
         ShowWindow(hDeselect, SW_HIDE);
         ShowWindow(hTemp, SW_HIDE);
         ShowWindow(hPattern, SW_HIDE);
         ShowWindow(hTmp, SW_HIDE);
         }
      return TRUE;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDC_CASE_SENSITIVE:
             uf.fCaseSensitive = !uf.fCaseSensitive;
             break;
        case IDC_WHOLEWORDS:
             uf.fWholeWords = !uf.fWholeWords;
             break;
        case IDC_IGNORE_BINARY:
             uf.fIgnoreBinary = !uf.fIgnoreBinary;
             break;
        case IDC_FILE_PATTERN:
        case IDC_GREP_PATTERN:
            if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
               {
         /* Enable or disable buttons depending on fullness of
          * pattern boxes.
          */
               if (!fGreppingArchives)
                  nPatternLength = GetWindowTextLength(hPattern);
               else
                  nPatternLength = 1;
               nGrepLength = GetWindowTextLength(hGrepPattern);
               if (((nPatternLength == 1) &&
                  (nGrepLength > 0)) ||
                  ((nPatternLength > 0) &&
                  (nGrepLength == 1)))
                  {
                  BOOL fButtonState = TRUE ;
                  WinAssert(hSelect);
                  EnableWindow(hSelect, fButtonState);
                  if (!fGreppingArchives)
                     {
                     WinAssert(hDeselect);
                     EnableWindow(hDeselect, fButtonState);
                     }
                  }
               if ((nPatternLength == 0) || (nGrepLength == 0))
                  {
                  BOOL fButtonState = FALSE;
                  WinAssert(hSelect);
                  EnableWindow(hSelect, fButtonState);
                  if (!fGreppingArchives)
                     {
                     WinAssert(hDeselect);
                     EnableWindow(hDeselect, fButtonState);
                     }
                  }
               }
            break;

      case IDC_PATTERN_DESELECT: /* De-select items using pattern */
      case IDOK: /* Select items using pattern */
         /* Be sure that listbox contains at least 1 item,
          * that a pattern exists (is non-null), and
          * that we can buffer them.
          */
         #define FILENAME_OFFSET 41
         fSelect = (BOOL)(LOWORD(wParam) == IDOK);
         psPatternBuf =   /* get a buffer for the file search pattern */
              (PSTR)GlobalAlloc(GMEM_FIXED, nPatternLength+1);
         psLBEntry =   /* get a buffer   for the file name */
              (PSTR)GlobalAlloc(GMEM_FIXED, PATH_MAX+FILENAME_OFFSET);
         if (!fGreppingArchives)
            {
            cListItems = ListView_GetItemCount(GetArchiveWindow(NULL));
            nPatternLength = GetWindowTextLength(hPattern);
            iTemp = GetWindowText(hPattern, psPatternBuf, nPatternLength+1);
            }
         else
            {
            cListItems = 1; /* These items are only used when performing a grep
                               on a single, active archive window.
                             */
            nPatternLength = 1;
            iTemp = 1;
            }

         if ((cListItems != 0) &&
            (nPatternLength >= 1) &&
            (nGrepLength = GetWindowTextLength(hGrepPattern)) >= 1 &&
            (psPatternBuf != NULL) &&
            (psGrepBuf =   /* get a buffer for the search pattern */
              (PSTR)GlobalAlloc(GMEM_FIXED, nGrepLength+1)) != NULL &&
            (psLBEntry != NULL) &&
            (GetWindowText(hGrepPattern, psGrepBuf, nGrepLength+1) > 0) &&
            iTemp > 0)
            {
            DWORD cItemsSelected = 0; /* no. "hits" during pattern search   */
            UINT uLBInx;
            LPSTR psPattern;   /* points to any one pattern in buffer      */
            int Code;

#ifndef __BORLANDC__
            _strlwr(psPatternBuf);   /* convert pattern to lower case   */
#else
            strlwr(psPatternBuf);   /* convert pattern to lower case   */
#endif
            for (p = psPatternBuf; *p; p++)
                if (*p == '\\')
                    *p = '/';
            /* If we are examining a bunch of archives, we bail out here, as
               we have no archive window to get file information from. Note
               that the allocated memory will need to get freed up elsewhere.
             */
            if (fGreppingArchives)
               {
               PostMessage(hwndDlg, WM_CLOSE, 0, 0L);
               break;
               }
            Fname_Inx = 1;
            if (uf.fWholeWords)
               {
               if (uf.fCaseSensitive)
                  Code = 3;
               else
                  Code = 2;
               }
            else
               Code = uf.fCaseSensitive;
            /* march through list of patterns in file selection field */
            for (psPattern = (PSTR)strtok(psPatternBuf, DELIMS);
                psPattern != NULL; psPattern = (PSTR)strtok(NULL, DELIMS))
                {
                /* March thru listbox matching the complete path with every entry.
                 * Note: unzip's match() function probably won't work for national
                 * characters above the ASCII range.
                 */
                for (uLBInx = 0; uLBInx < cListItems; uLBInx++)
                    {
                    /* Retrieve listbox entry */
                    ListView_GetItemText(GetArchiveWindow(NULL), uLBInx, 0, psLBEntry, PATH_MAX);
#ifndef __BORLANDC__
                    _strlwr(&psLBEntry[Fname_Inx]); /* convert filename to lower case */
#else
                    strlwr(&psLBEntry[Fname_Inx]); /* convert filename to lower case */
#endif
                    /* Use UnZip's match() function                  */
                    if (UzpMatch(&psLBEntry[Fname_Inx], psPattern, TRUE))
                       {
                       ListView_GetItemText(GetArchiveWindow(NULL),
                                            uLBInx, 0,
                                            psLBEntry,
                                            PATH_MAX);
                       GetWindowText(GetActiveMDIChild(),
                                     szArchiveName, PATH_MAX);

                       if ((ret =
                            GrepArchive(szArchiveName,
                                        &psLBEntry[Fname_Inx],
                                        psGrepBuf, Code, uf.fIgnoreBinary,
                                        lpUserFunctions))>0)
                            {
                            ListViewSetSel(uLBInx, fSelect);
                            cItemsSelected++;
                            }
                       if (ret < 0)   /* error */
                          {
                          char sz1[RESOURCE_STR_LEN];
                          sprintf(sz1, "Unable to extract %s to memory",
                                  &psLBEntry[Fname_Inx]);
                          MessageBox(hwndDlg,
                                     sz1,
                                     szAppName,
                                     MB_OK | MB_ICONASTERISK);
                          }
                       }
                    }
                }
            if (!cItemsSelected)   /* If no pattern match */
               {
               char sz1[RESOURCE_STR_LEN];
               lstrcpy(sz1, GetLoadString(IDS_NO_MATCH));
               MessageBox(hwndDlg,
                          sz1,
                          szAppName,
                          MB_OK | MB_ICONASTERISK);
               }
            else /* one or more items were selected */
               {
               UpdateButtons(); /* turn main push buttons on or off */
               }
            }
         freePatMem();
         break;

      case IDCANCEL:
         PostMessage(hwndDlg, WM_CLOSE, 0, 0L);
         break;
      }
      return TRUE;
   case WM_CLOSE:
      DestroyWindow(hwndDlg);
      hPatternSelectDlg = 0; /* flag dialog inactive */
      if (fGreppingArchives)
         PostMessage(hWndMain, WM_COMMAND, IDM_SEARCH_ARCHIVE, 0L);
      return TRUE;
   }
   return FALSE;
#ifdef __BORLANDC__
#pragma warn -aus
#endif
}
#ifdef __BORLANDC__
#pragma warn .aus
#endif


