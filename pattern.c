/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <stdio.h>
#include "wiz.h"

//MW #include "helpids.h"


/****************************************************************************

    FUNCTION: PatternSelectProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Select Files by Pattern" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI
PatternSelectProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
static HWND hSelect, hDeselect, hPattern;
PSTR psLBEntry = NULL;  /* list box entry */
PSTR psPatternBuf = NULL;   /* pattern collection   */
DWORD cListItems;   /* no. items in listbox   */
UINT Fname_Inx;      /* index into LB entry   */
int nPatternLength;   /* length of data in pattern edit window */
char *p;

   switch (wMessage) {
   case WM_INITDIALOG:
      hSelect = GetDlgItem(hwndDlg, IDOK);
      hDeselect = GetDlgItem(hwndDlg, IDC_PATTERN_DESELECT);
      hPattern = GetDlgItem(hwndDlg, IDC_FILE_PATTERN);
      CenterDialog(NULL, hwndDlg);
      return TRUE;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDC_FILE_PATTERN:
            if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
               {
               if ((nPatternLength = GetWindowTextLength(hPattern))==1)
                  {
         /* Enable or disable buttons depending on fullness of
          * "Suffix" box.
          */
                  BOOL fButtonState = TRUE ;
                  WinAssert(hSelect);
                  EnableWindow(hSelect, fButtonState);
                  WinAssert(hDeselect);
                  EnableWindow(hDeselect, fButtonState);
                  }
               if (nPatternLength == 0)
                  {
                  BOOL fButtonState = FALSE;
                  WinAssert(hSelect);
                  EnableWindow(hSelect, fButtonState);
                  WinAssert(hDeselect);
                  EnableWindow(hDeselect, fButtonState);
                  }
               }
            break;

      case IDC_PATTERN_DESELECT:
         /* Be sure that listbox contains at least 1 item,
          * that a pattern exists (is non-null), and
          * that we can buffer them.
          */
         #define FNAME_OFFSET 41
         if ((cListItems = ListView_GetItemCount(GetArchiveWindow(NULL))) != 0 &&
            (nPatternLength = GetWindowTextLength(hPattern)) >= 1 &&
            (psPatternBuf =   /* get a buffer   for the file name               */
              (PSTR)GlobalAlloc(GMEM_FIXED, nPatternLength+1)) != NULL &&
           (psLBEntry =   /* get a buffer   for the file name               */
              (PSTR)GlobalAlloc(GMEM_FIXED, PATH_MAX+FNAME_OFFSET)) != NULL &&
            GetWindowText(hPattern, psPatternBuf, nPatternLength+1) > 0)
         {
         DWORD cItemsSelected = 0; /* no. "hits" during pattern search   */
         UINT uLBInx;
         LPSTR psPattern;   /* points to any one pattern in buffer      */
         static char DELIMS[] = " \t,"; /* delimiters, mostly whitespace */

#ifndef __BORLANDC__
            _strlwr(psPatternBuf);   /* convert pattern to lower case   */
#else
            strlwr(psPatternBuf);   /* convert pattern to lower case   */
#endif
            for (p = psPatternBuf; *p; p++)
                if (*p == '\\')
                    *p = '/';
            Fname_Inx = 1;
            /* march through list of patterns in edit field            */
            for (psPattern = (PSTR)strtok(psPatternBuf, DELIMS);
                psPattern != NULL; psPattern = (PSTR)strtok(NULL, DELIMS))
            {

               /* March thru listbox matching the complete path with every entry.
                * Note: unzip's match() function probably won't work for national
                * characters above the ASCII range.
                */
               for (uLBInx = 0; uLBInx < cListItems; uLBInx++)
               {
                  /* Retrieve listbox entry                        */
                   ListView_GetItemText(GetArchiveWindow(NULL), uLBInx, 0, psLBEntry, PATH_MAX);
                  {
#ifndef __BORLANDC__
                     _strlwr(&psLBEntry[Fname_Inx]); /* convert filename to lower case */
#else
                     strlwr(&psLBEntry[Fname_Inx]); /* convert filename to lower case */
#endif
                     /* Use UnZip's match() function                  */
                     if (UzpMatch(&psLBEntry[Fname_Inx], psPattern, TRUE))
                     {
                        ListViewSetSel(uLBInx, TRUE);
                        cItemsSelected++;
                     }
                  }
               }
            }
            if (!cItemsSelected)   /* If no pattern match               */
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
         if (psLBEntry)
            GlobalFree((HGLOBAL)psLBEntry);

         if (psPatternBuf)
            GlobalFree((HGLOBAL)psPatternBuf);
         break;
      case IDOK: /* Select items using pattern */
         /* Be sure that listbox contains at least 1 item,
          * that a pattern exists (is non-null), and
          * that we can buffer them.
          */
         #define LONG_FORM_FNAME_INX 41
         if ((cListItems = ListView_GetItemCount(GetArchiveWindow(NULL))) != 0 &&
            (nPatternLength = GetWindowTextLength(hPattern)) >= 1 &&
            (psPatternBuf =   /* get a buffer   for the file name               */
              (PSTR)GlobalAlloc(GMEM_FIXED, nPatternLength+1)) != NULL &&
           (psLBEntry =   /* get a buffer   for the file name               */
              (PSTR)GlobalAlloc(GMEM_FIXED, PATH_MAX+LONG_FORM_FNAME_INX)) != NULL &&
            GetWindowText(hPattern, psPatternBuf, nPatternLength+1) > 0)
         {
         DWORD cItemsSelected = 0; /* no. "hits" during pattern search   */
         UINT uLBInx;
         LPSTR psPattern;   /* points to any one pattern in buffer      */
         static char DELIMS[] = " \t,"; /* delimiters, mostly whitespace */

#ifndef __BORLANDC__
            _strlwr(psPatternBuf);   /* convert pattern to lower case   */
#else
            strlwr(psPatternBuf);   /* convert pattern to lower case   */
#endif
            for (p = psPatternBuf; *p; p++)
                if (*p == '\\')
                    *p = '/';
            Fname_Inx = 1;
            /* march through list of patterns in edit field            */
            for (psPattern = (PSTR)strtok(psPatternBuf, DELIMS);
                psPattern != NULL; psPattern = (PSTR)strtok(NULL, DELIMS))
            {

               /* March thru listbox matching the complete path with every entry.
                * Note: unzip's match() function probably won't work for national
                * characters above the ASCII range.
                */
               for (uLBInx = 0; uLBInx < cListItems; uLBInx++)
               {
                  /* Retrieve listbox entry                        */
                   ListView_GetItemText(GetArchiveWindow(NULL), uLBInx, 0, psLBEntry, PATH_MAX);
                  {
#ifndef __BORLANDC__
                     _strlwr(&psLBEntry[Fname_Inx]); /* convert filename to lower case */
#else
                     strlwr(&psLBEntry[Fname_Inx]); /* convert filename to lower case */
#endif
                     /* Use UnZip's match() function                  */
                     if (UzpMatch(&psLBEntry[Fname_Inx], psPattern, TRUE))
                     {
                        ListViewSetSel(uLBInx, TRUE);
                        cItemsSelected++;
                     }
                  }
               }
            }
            if (!cItemsSelected)   /* If no pattern match               */
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
         if (psLBEntry)
            GlobalFree((HGLOBAL)psLBEntry);

         if (psPatternBuf)
            GlobalFree((HGLOBAL)psPatternBuf);
         break;
      case IDCANCEL:
         PostMessage(hwndDlg, WM_CLOSE, 0, 0L);
         break;
/* MW
      case IDC_PATTERN_HELP:
            WinHelp(hwndDlg,szHelpFileName,HELP_CONTEXT, (DWORD)(HELPID_SELECT_BY_PATTERN));
         break;
*/         
      }
      return TRUE;
   case WM_CLOSE:
      DestroyWindow(hwndDlg);
      hPatternSelectDlg = 0; /* flag dialog inactive   */
      return TRUE;
   }
   return FALSE;
}

