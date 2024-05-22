/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <stdio.h>
#include <windows.h>
#include "wiz.h"
#include "unzip\unzip.h"

BOOL bSorting = FALSE; /* For redraw flagging */

int iColumn = 0; /* Set externally to indicate which column is being sorted on
                    Should always be reset to zero after doing a sort!!!
                  */
TCHAR date_format[2];  /* LOCALE_IDATE has a maximum value of 2 */

/* Update Buttons is called when an event possibly modifies the
 * number of selected items in the listbox.
 * The function reads the number of selected items.
 * A non-zero value enables relevant buttons and menu items.
 * A zero value disables them.
 */
void UpdateButtons(void)
{
static BOOL fButtonState = FALSE;
int mf;
TCHAR szTempArchiveName[2 * PATH_MAX];

lstrcpy(szTempArchiveName, lpumb->szFileName);

GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);

if (lpumb->szFileName[0] &&
   ListView_GetSelectedCount(GetArchiveWindow(NULL)))
   {
   fButtonState = TRUE;
   }
else
   {
   fButtonState = FALSE;
   }

SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_DESELECT_ALL,
      (LPARAM) MAKELONG(fButtonState, 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_EXTRACT,
      (LPARAM) MAKELONG(fButtonState, 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_DISPLAY,
      (LPARAM) MAKELONG(fButtonState, 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_TEST,
      (LPARAM) MAKELONG(fButtonState, 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_UPDATE_ZIP,
      (LPARAM) MAKELONG(fButtonState, 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_ZIP_DELETE_ENTRIES,
      (LPARAM) MAKELONG(fButtonState, 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_GET_ZIPINFO,
      (LPARAM) MAKELONG(fButtonState, 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_SHOW_COMMENT,
      (LPARAM) MAKELONG((((BOOL)(fButtonState && lpUserFunctions->cchComment) > 0) ? TRUE : FALSE), 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_COPY_ARCHIVE,
      (LPARAM) MAKELONG(lpumb->szFileName[0], 0));
#if 0
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_MOVE_ARCHIVE,
      (LPARAM) MAKELONG(lpumb->szFileName[0], 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_RENAME_ARCHIVE,
      (LPARAM) MAKELONG(lpumb->szFileName[0], 0));
#endif
      
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_DELETE_ARCHIVE,
      (LPARAM) MAKELONG(lpumb->szFileName[0], 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_ZIP_TARGET_CURRENT,
      (LPARAM) MAKELONG(lpumb->szFileName[0], 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_SELECT_ALL,
      (LPARAM) MAKELONG(lpumb->szFileName[0], 0));
SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM) IDM_SELECT_BY_PATTERN,
      (LPARAM) MAKELONG(lpumb->szFileName[0], 0));

EnableMenuItem(hMenu, IDM_GET_ZIPINFO,
   (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
EnableMenuItem(hMenu, IDM_EXTRACT,
   (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
EnableMenuItem(hMenu, IDM_DISPLAY,
   (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
EnableMenuItem(hMenu, IDM_TEST,
   (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
EnableMenuItem(hMenu, IDM_UPDATE_ZIP,
   (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
EnableMenuItem(hMenu, IDM_ZIP_DELETE_ENTRIES,
   (fButtonState ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);
EnableMenuItem(hMenu, IDM_SHOW_COMMENT,
   (BOOL)(fButtonState && lpUserFunctions->cchComment ? MF_ENABLED : MF_DISABLED|MF_GRAYED)|MF_BYCOMMAND);

if (lpumb->szFileName[0] != '\0')
   mf = MF_ENABLED | MF_BYCOMMAND;
else
   mf = MF_DISABLED|MF_GRAYED|MF_BYCOMMAND;

EnableMenuItem(hMenu, IDM_DELETE_ARCHIVE, mf);
if (cOpen)
   EnableMenuItem(hMenu, IDM_ZIP_TARGET_CURRENT, mf);
else
   EnableMenuItem(hMenu, IDM_ZIP_TARGET_CURRENT,
      MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);

EnableMenuItem(hMenu, IDM_COPY_ARCHIVE, mf);
#if 0
EnableMenuItem(hMenu, IDM_MOVE_ARCHIVE, mf);
EnableMenuItem(hMenu, IDM_RENAME_ARCHIVE, mf);
#endif

EnableMenuItem(hMenu, IDM_SELECT_ALL, mf);
EnableMenuItem(hMenu, IDM_DESELECT_ALL, mf);
EnableMenuItem(hMenu, IDM_SELECT_BY_PATTERN, mf);
if (cOpen)
   EnableMenuItem(hMenu, IDM_GREP_ARCHIVE_CONTENTS, mf);
else
   EnableMenuItem(hMenu, IDM_GREP_ARCHIVE_CONTENTS,
                  MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
if (cOpen > 1)
   {
   EnableMenuItem(hMenu, IDM_DIFF_ARCHIVE, MF_ENABLED | MF_BYCOMMAND);
   EnableMenuItem(hMenu, IDM_DIFF_ARCHIVE_CONTENTS, MF_ENABLED | MF_BYCOMMAND);
   }
else
   {
   EnableMenuItem(hMenu,
                  IDM_DIFF_ARCHIVE,
                  MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
   EnableMenuItem(hMenu,
                  IDM_DIFF_ARCHIVE_CONTENTS,
                  MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
   }
lstrcpy(lpumb->szFileName, szTempArchiveName);
}

/* UpdateListBox fills in the listbox (or ListView), and sorts it
 * depending on what the column number is.
 *
 * Note that iColumn determines the data the sort is done on.
 */
void UpdateListBox(void)
{
char szCompFactor[12];
char sgn;
char CompFactorStr[] = "%c%d%%";
char CompFactor100[] = "100%%";

cZippedFiles = 0;

CleanScreen();

GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);

if (lpumb->szFileName[0])       /* archive selected? */
   {
   /* if so -- stuff list box */
   if (!bSorting)
      SendMessage(GetArchiveWindow(NULL), WM_SETREDRAW, FALSE, 0L);
   /* Call here when a file has been initially selected */
   ListView_DeleteAllItems(GetArchiveWindow(NULL));

   lpDCL->ncflag = 0;
   lpDCL->ntflag = 0;
   lpDCL->nvflag = 2; //Tells dll to give verbose information
   lpDCL->nfflag = 0;
   lpDCL->nzflag = 0;
   lpDCL->ndflag = 0;
   lpDCL->noflag = 0;
   lpDCL->naflag = 0;
   uzpParameters.argc   = 0;
   uzpParameters.argv = NULL;
   lpDCL->lpszExtractDir = NULL;

   lpDCL->lpszZipFN = lpumb->szFileName;
   iGlobalReturnCode =
         (*Unz_SingleEntryPoint)(uzpParameters.argc,
                                 uzpParameters.argv,
                                 uzpParameters.xfnc,
                                 uzpParameters.xfnv,
                                 uzpParameters.lpDCL,
                                 uzpParameters.lpUserFunctions);

   if (!bSorting)
      {
      SendMessage(GetArchiveWindow(NULL), WM_SETREDRAW, TRUE, 0L);
      InvalidateRect(hWndListParent, NULL, TRUE);   /* force redraw */
      UpdateWindow(hWndListParent);
      }
   cZippedFiles = (WORD)ListView_GetItemCount(GetArchiveWindow(NULL));
   WinAssert((int)cZippedFiles != LB_ERR);
   if (cZippedFiles)   /* if anything went into listbox set to top */
      ListView_EnsureVisible(GetArchiveWindow(NULL), 0, FALSE);

   if (lpUserFunctions->TotalSizeComp > lpUserFunctions->TotalSize)
       sgn = '-';
   else
       sgn = ' ';

   if (lpUserFunctions->CompFactor == 100)
       sprintf(szCompFactor, CompFactor100);
   else
       sprintf(szCompFactor, CompFactorStr, sgn,
               lpUserFunctions->CompFactor);
   UpdateArchiveStatusBar(szCompFactor, 7, 0);
   wsprintf(szCompFactor, "%lu", lpUserFunctions->NumMembers);
   UpdateArchiveStatusBar(szCompFactor, 1, 0);
   wsprintf(szCompFactor, "%lu", lpUserFunctions->TotalSize);
   UpdateArchiveStatusBar(szCompFactor, 3, 0);
   wsprintf(szCompFactor, "%lu", lpUserFunctions->TotalSizeComp);
   UpdateArchiveStatusBar(szCompFactor, 5, 0);
   }
else
   {
   UpdateArchiveStatusBar("", 1, 0);
   UpdateArchiveStatusBar("", 3, 0);
   UpdateArchiveStatusBar("", 5, 0);
   UpdateArchiveStatusBar("", 7, 0);
   }
}

#ifdef __BORLANDC__
#pragma argsused
#endif
void WINAPI ReceiveDllMessage(unsigned long ucsize,unsigned long csiz,
   unsigned cfactor, unsigned mo, unsigned dy, unsigned yr,
   unsigned hh, unsigned mm, char c, LPSTR filename, LPSTR methbuf,
   unsigned long crc, char fCrypt)
{
LV_ITEM lvi;
TCHAR Length[9];
TCHAR Size[9];
TCHAR Ratio[5];
TCHAR Date[13];
TCHAR Time[6];
TCHAR Encrypted[4];
TCHAR Name[PATH_MAX+1];
TCHAR CompFactorStr[] = "%c%d%%";
TCHAR CompFactor100[] = "100%%";
TCHAR szCompFactor[10];
TCHAR sgn;
TCHAR szPicture[] = "yyyy MM  dd";
TCHAR *p;
extern BOOL fScanning;

memset(&lvi, 0, sizeof(LV_ITEM));
for (p = filename; *p; p++)
     if (*p == '/')
         *p = '\\';

// Note that the dll (or library) returns the date already in the
// correct national order, we simply need to determine the locale
// information in order to be able to sort on the date.

if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IDATE, date_format, 2) != 0) 
   {
   switch (date_format[0])
      {
      case '0': // Month, day, year
            if (yr < 50)
               yr = (ush)(yr + (ush)2000);
            else
               yr = (ush)(yr + (ush)1900);
            if (bSorting && (iColumn == 4))
                wsprintf(Date,"%04u/%02u/%02u", yr, mo, dy);
            else
                wsprintf(Date,"%02u/%02u/%04u", mo, dy, yr);
            break;
      case '1': // Day, month, year
            if (yr < 50)
               yr = (ush)(yr + (ush)2000);
            else
               yr = (ush)(yr + (ush)1900);
            if (bSorting && (iColumn == 4))
                wsprintf(Date,"%04u/%02u/%02u", yr, dy, mo);
            else
                wsprintf(Date,"%02u/%02u/%04u", mo, dy, yr);
            break;
      case '2': // Year, month, day
            if (mo < 50)
               mo = (ush)(mo + (ush)2000);
            else
               mo = (ush)(mo + (ush)1900);
            wsprintf(Date,"%04u/%02u/%02u", mo, dy, yr);
            break;
      }
    }
else
    { // If the call fails, assume Month, Day, Year ordering.
    if (yr < 30)
       yr = (ush)(yr + (ush)2000);
    else
       yr = (ush)(yr + (ush)1900);
    wsprintf(Date,"%04u/%02u/%02u", yr, mo, dy);
    }

if (fScanning)
   {
#ifndef __BORLANDC__
   _strlwr(SearchPattern); /* convert filename to lower case */
#else
   strlwr(SearchPattern); /* convert filename to lower case */
#endif
   /* Use UnZip's match() function                  */
   if (UzpMatch(filename, SearchPattern, TRUE))
      {
      if (fGreppingArchives)
         {
         int Code;

         if (uf.fWholeWords)
            {
            if (uf.fCaseSensitive)
               Code = 3;
            else
               Code = 2;
            }
         else
            Code = uf.fCaseSensitive;

         if (GrepArchive(lpDCL->lpszZipFN,
                         filename,
                         psGrepBuf, Code, uf.fIgnoreBinary,
                         lpUserFunctions) < 1)
            return;
         }

      wsprintf(Length, "%7lu", ucsize);
      wsprintf(Time, "%02u:%02u", hh, mm);
      wsprintf(Name, "%c%s", c, filename);

      lvi.mask        = LVIF_TEXT;
      lvi.state       = 0;
      lvi.stateMask   = LVIS_STATEIMAGEMASK;

      lvi.iItem       = 0;
      lvi.iSubItem    = 0;
      lvi.cchTextMax  = 9;
      lvi.iImage      = 0;

      lvi.pszText = Name;                                           /* sub-item 0 */
      lvi.iItem = ListView_InsertItem(hWndGrep, &lvi);              /* Insert */
      ListView_SetItemText(hWndGrep, lvi.iItem, 1,
         (LPSTR) lpDCL->lpszZipFN);                                 /* sub-item 1 */
      ListView_SetItemText(hWndGrep, lvi.iItem, 2, (LPSTR) Length); /* sub-item 2 */
      ListView_SetItemText(hWndGrep, lvi.iItem, 3, (LPSTR) Date);   /* sub-item 3 */
      ListView_SetItemText(hWndGrep, lvi.iItem, 4, (LPSTR) Time);   /* sub-item 4 */
      }
   return;
   }

if (csiz > ucsize)
   sgn = '-';
else
   sgn = ' ';
if (cfactor == 100)
   lstrcpy(szCompFactor, CompFactor100);
else
   sprintf(szCompFactor, CompFactorStr, sgn, cfactor);

wsprintf(Length, "%7lu", ucsize);
wsprintf(Size, "%7lu", csiz);
wsprintf(Ratio, "%4s", szCompFactor);

wsprintf(Time, "%02u:%02u", hh, mm);
Name[0] = c;
lstrcpy(&Name[1], filename);
wsprintf(Name, "%c%s", c, filename);

if (fCrypt == 'E')
   lstrcpy(Encrypted, "Yes");
else
   lstrcpy(Encrypted, "No");

lvi.mask        = LVIF_TEXT;
lvi.state       = 0;
lvi.stateMask   = LVIS_STATEIMAGEMASK;

lvi.iItem       = 0;
lvi.iSubItem    = 0;
lvi.cchTextMax  = 9;
lvi.iImage      = 0;

// iColumn is set externally to determine which column we are sorting on
switch (iColumn)
   {
   case 0:
      lvi.pszText = Name;                                          /* sub-item 0 */
      lvi.iItem = ListView_InsertItem(GetArchiveWindow(NULL), &lvi);
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 1, (LPSTR) Length);/* sub-item 1 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 2, (LPSTR) Size);  /* sub-item 2 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 3, (LPSTR) Ratio); /* sub-item 3 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 4, (LPSTR) Date);  /* sub-item 4 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 5, (LPSTR) Time);  /* sub-item 5 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 6, (LPSTR) Encrypted);  /* sub-item 6 */
      break;
   case 1:
      lvi.pszText = Length;                                        /* sub-item 0 */
      lvi.iItem = ListView_InsertItem(GetArchiveWindow(NULL), &lvi);
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 1, (LPSTR) Name);/* sub-item 0 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 2, (LPSTR) Size);  /* sub-item 2 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 3, (LPSTR) Ratio); /* sub-item 3 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 4, (LPSTR) Date);  /* sub-item 4 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 5, (LPSTR) Time);  /* sub-item 5 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 6, (LPSTR) Encrypted);  /* sub-item 6 */
      break;
   case 2:
      lvi.pszText = Size;                                          /* sub-item 0 */
      lvi.iItem = ListView_InsertItem(GetArchiveWindow(NULL), &lvi);
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 1, (LPSTR) Length);/* sub-item 1 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 2, (LPSTR) Name);  /* sub-item 0 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 3, (LPSTR) Ratio); /* sub-item 3 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 4, (LPSTR) Date);  /* sub-item 4 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 5, (LPSTR) Time);  /* sub-item 5 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 6, (LPSTR) Encrypted);  /* sub-item 6 */
      break;
   case 3:
      lvi.pszText = Ratio;                                          /* sub-item 0 */
      lvi.iItem = ListView_InsertItem(GetArchiveWindow(NULL), &lvi);
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 1, (LPSTR) Length);/* sub-item 1 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 2, (LPSTR) Size);  /* sub-item 2 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 3, (LPSTR) Name);  /* sub-item 0 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 4, (LPSTR) Date);  /* sub-item 4 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 5, (LPSTR) Time);  /* sub-item 5 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 6, (LPSTR) Encrypted);  /* sub-item 6 */
      break;
   case 4:
      lvi.pszText = Date;                                          /* sub-item 0 */
      lvi.iItem = ListView_InsertItem(GetArchiveWindow(NULL), &lvi);
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 1, (LPSTR) Length);/* sub-item 1 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 2, (LPSTR) Size);  /* sub-item 2 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 3, (LPSTR) Ratio); /* sub-item 3 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 4, (LPSTR) Name);  /* sub-item 0 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 5, (LPSTR) Time);  /* sub-item 5 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 6, (LPSTR) Encrypted);  /* sub-item 6 */
      break;
   case 5:
      lvi.pszText = Time;                                          /* sub-item 0 */
      lvi.iItem = ListView_InsertItem(GetArchiveWindow(NULL), &lvi);
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 1, (LPSTR) Length);/* sub-item 1 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 2, (LPSTR) Size);  /* sub-item 2 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 3, (LPSTR) Ratio); /* sub-item 3 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 4, (LPSTR) Date);  /* sub-item 4 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 5, (LPSTR) Name);  /* sub-item 0 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 6, (LPSTR) Encrypted);  /* sub-item 6 */
      break;
   case 6:
      lvi.pszText = Encrypted;                                          /* sub-item 0 */
      lvi.iItem = ListView_InsertItem(GetArchiveWindow(NULL), &lvi);
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 1, (LPSTR) Length);/* sub-item 1 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 2, (LPSTR) Size);  /* sub-item 2 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 3, (LPSTR) Ratio); /* sub-item 3 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 4, (LPSTR) Date);  /* sub-item 4 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 5, (LPSTR) Time);  /* sub-item 5 */
      ListView_SetItemText(GetArchiveWindow(NULL), lvi.iItem, 6, (LPSTR) Name);  /* sub-item 0 */
      break;
   }
}

/* All this does is force a selection of the item number passed in */
void ListViewSetSel(int item, BOOL b)
{
/* Sure is a pain to force a selection within a program */
LV_ITEM lvi;

lvi.mask      = LVIF_STATE;
lvi.stateMask = LVIS_SELECTED;
lvi.iItem     = item;
lvi.iSubItem  = 0;

ListView_GetItem(GetArchiveWindow(NULL), &lvi);

if (b)
    lvi.state = LVIS_SELECTED;
else
    lvi.state = 0;

SendMessage(GetArchiveWindow(NULL), LVM_SETITEMSTATE, (WPARAM)item,
         (LPARAM)&lvi);
}

void ListViewSortOnColumns(int iCol)
{
int i, j;
LPTSTR lpStr1, lpStr2, p;
HANDLE hMem1, hMem2;

iColumn = iCol;

hMem1 = GlobalAlloc( GPTR, (PATH_MAX * 2));
if (!hMem1)
   return;
hMem2 = GlobalAlloc( GPTR, (PATH_MAX * 2));
if (!hMem2)
   {
   GlobalFree(hMem1);
   return;
   }
lpStr1 = (LPSTR)GlobalLock(hMem1);
if (!lpStr1)
   {
   GlobalFree(hMem1);
   GlobalFree(hMem2);
   return;
   }
lpStr2 = (LPSTR)GlobalLock(hMem2);
if (!lpStr2)
   {
   GlobalUnlock(lpStr1);
   GlobalFree(hMem1);
   GlobalFree(hMem2);
   return;
   }

hSaveCursor = SetCursor(hHourGlass);
ShowCursor(TRUE);

bSorting = TRUE;

/* Restore the ListView to the appropriate columns */

SendMessage(GetArchiveWindow(NULL), WM_SETREDRAW, FALSE, 0L);

UpdateListBox();

j = ListView_GetItemCount(GetArchiveWindow(NULL));
for (i = 0; i < j; i++)
    {
    switch (iColumn)
       {
       case 1: /* Swap subitem 0 and 1 (Sort on Length) */
          ListView_GetItemText(GetArchiveWindow(NULL), i, 0, lpStr1, PATH_MAX);
          ListView_GetItemText(GetArchiveWindow(NULL), i, 1, lpStr2, PATH_MAX);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 0, lpStr2);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 1, lpStr1);
          break;
       case 2: /* Swap subitem 0 and 2 (Sort on size) */
          ListView_GetItemText(GetArchiveWindow(NULL), i, 0, lpStr1, PATH_MAX);
          ListView_GetItemText(GetArchiveWindow(NULL), i, 2, lpStr2, PATH_MAX);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 0, lpStr2);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 2, lpStr1);
          break;
       case 3: /* Swap subitem 0 and 3 (Sort on ratio) */
          ListView_GetItemText(GetArchiveWindow(NULL), i, 0, lpStr1, PATH_MAX);
          ListView_GetItemText(GetArchiveWindow(NULL), i, 3, lpStr2, PATH_MAX);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 0, lpStr2);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 3, lpStr1);
          break;
       case 4: /* Swap subitem 0 and 4 (Sort on date) */
          ListView_GetItemText(GetArchiveWindow(NULL), i, 0, lpStr1, PATH_MAX);
          ListView_GetItemText(GetArchiveWindow(NULL), i, 4, lpStr2, PATH_MAX);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 0, lpStr2);
          switch (date_format[0])
          {
              case '0': // Month, day, year
                p = lpStr1+5;
                lstrcpy(lpStr2, p);
                lstrcat(lpStr2, "/");
                lstrcat(lpStr2, lpStr1);
                lpStr2[lstrlen(lpStr1)] = '\0';
                break;
              case '1': // Day, month, year
                p = lpStr1+8;
                lstrcpy(lpStr2, p);
                lstrcat(lpStr2, "/");
                p = lpStr1+5;
                lstrcat(lpStr2, p);
                lpStr2[5] = '\0';
                lstrcat(lpStr2, "/");
                lstrcat(lpStr2, lpStr1);
                lpStr2[lstrlen(lpStr1)] = '\0';
                break;
          }
          ListView_SetItemText(GetArchiveWindow(NULL), i, 4, lpStr2);
          break;
       case 5: /* Swap subitem 0 and 5 (Sort on time) */
          ListView_GetItemText(GetArchiveWindow(NULL), i, 0, lpStr1, PATH_MAX);
          ListView_GetItemText(GetArchiveWindow(NULL), i, 5, lpStr2, PATH_MAX);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 0, lpStr2);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 5, lpStr1);
          break;
       case 6: /* Swap subitem 0 and 6 (Sort on encrypted) */
          ListView_GetItemText(GetArchiveWindow(NULL), i, 0, lpStr1, PATH_MAX);
          ListView_GetItemText(GetArchiveWindow(NULL), i, 6, lpStr2, PATH_MAX);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 0, lpStr2);
          ListView_SetItemText(GetArchiveWindow(NULL), i, 6, lpStr1);
          break;
       }
    }
iColumn = 0; /* restore back to default of filename */
bSorting = FALSE;

SendMessage(GetArchiveWindow(NULL), WM_SETREDRAW, TRUE, 0L);
InvalidateRect(hWndListParent, NULL, TRUE);   /* force redraw */
UpdateWindow(hWndListParent);

ShowCursor(FALSE);
SetCursor(hSaveCursor);
ShowCursor(TRUE);

GlobalUnlock(lpStr1);
GlobalUnlock(lpStr2);
GlobalFree(hMem1);
GlobalFree(hMem2);
}


