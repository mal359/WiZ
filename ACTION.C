/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/
/*
 Copyright (C) 1997 Mike White
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <direct.h>
#include <stdio.h>

#include "wiz.h"
//MW #include "helpids.h"
#ifdef ZE_MEM
#undef ZE_MEM
#define ZE_MEM 4
#endif

#include "unzip\unzip.h"
#include "zip\ziperr.h"

unsigned long dSpaceToUse;
unsigned long dFilesToProcess = 0;
int NumberTempFiles = 0;

void IsArchiveOpen(LPSTR szArchiveName)
{
RECT  rClient;
HWND hStart;
HWND hTemp= NULL, hTmp  = NULL;
TCHAR szTemp[PATH_MAX];

/* Checking to see if the archive is already loaded. If so, then
   simply refresh the archive display, else create a new archive
   window.
 */
hStart = GetActiveMDIChild();
if (hStart)
   while (hStart != hTemp)
         {
         SendMessage(hWndListParent, WM_MDINEXT, (WPARAM)(HWND)NULL, (LPARAM) 0);
         hTemp = GetActiveMDIChild();
         GetWindowText(hTemp, szTemp, PATH_MAX);
         if (!lstrcmpi(szTemp, szArchiveName))
            {
            hTmp = hTemp;
            break;
            }
         }
if (hTmp == NULL)
   {
   lstrcpy(lpumb->szFileName, szArchiveName);
   CreateMDIChild(szArchiveName);
   InitializeArchiveStatusBar(GetActiveMDIChild());
   WinAssert(hWndListParent);
   GetClientRect( hWndListParent, &rClient );
   OffsetRect( &rClient, 0, dyChar );
   rClient.top = rClient.bottom;
   rClient.bottom = rClient.top + (6*dyChar);
   ListViewSetSel(0, TRUE);
   }
UpdateListBox();
UpdateButtons(); /* update state of buttons */
InvalidateRect(GetArchiveWindow(NULL), &rClient, TRUE);
UpdateWindow(GetArchiveWindow(NULL));
}

int IsValidArchive(LPSTR archive)
{
int retcode, AllCodes = FALSE;

fDisplay = 3;
retcode = (*UnzipValidate)((char *)archive, AllCodes);
fDisplay = 0;
return retcode;
}


/* Get Selection Count returns a count of the selected
 * list box items. If the count is  greater than zero, it also returns
 * a pointer to a locked list in local memory of the item nos.
 * and their local memory handle.
 *
 * A value of -1 indicates an error.
 *
 */

#ifdef __BORLANDC__
#pragma argsused
#endif
int CLBItemsGet(HWND hListBox, int __far * __far *ppnSelItems, HANDLE *phnd)
{
int i, j, k = 0;
LV_ITEM lvi;
int cSelLBItems = ListView_GetSelectedCount(hListBox);

if ( !phnd )
   return -1;

*phnd = 0;
if (cSelLBItems)
   {
   *phnd = GlobalAlloc(GMEM_FIXED, cSelLBItems * sizeof(int));
   if ( !*phnd )
      return -1;

   *ppnSelItems = (int __far *)GlobalLock( *phnd );
   if ( !*ppnSelItems )
      {
      GlobalFree( *phnd );
      *phnd = 0;
      return -1;
      }

   /* Get list of selected items. Return value is number of selected items */
   j = ListView_GetItemCount(hListBox);
   for (i = 0; i < j; i++)
      {
      lvi.mask      = LVIF_STATE;
      lvi.stateMask = LVIS_SELECTED;
      lvi.iItem     = i;
      lvi.iSubItem  = 0;

      ListView_GetItem(hListBox, &lvi);

      if (lvi.state)
         {
         (*ppnSelItems)[k++] = i;
         }
      }
   }
   return cSelLBItems;
}

/* Action is called on double-clicking, or selecting one of the
 * main action buttons.
 */
void Action(HWND hWnd, int wActionCode)
{
HANDLE  hMem;
int __far *pnSelItems;  /* pointer to list of selected items */
char **pszIndex;
char *sz;
LPSTR lpszT;
int i;
int iSelection;
int cch;
unsigned long dFreeSpace, dStrlen = 0;
int cSelLBItems;
DWORD SectorsPerCluster, BytesPerSector, FreeClusters, Clusters;
HANDLE  hnd = 0;

cSelLBItems = CLBItemsGet(hWnd, &pnSelItems, &hnd);

/* if no items were selected */
if (cSelLBItems < 1)
   return;

dFilesToProcess = cSelLBItems; // Get total number of files to process

/* Note: this global value can be overriden in replace.c */
uf.fDoAll = (lpDCL->noflag) ? 1 : 0;

    /* If all the files are selected pass in no filenames */
    /* since unzipping all files is the default */
hMem = GlobalAlloc( GPTR, (PATH_MAX * 2));
if ( !hMem )
   goto done;
lpszT = (LPSTR)GlobalLock( hMem );
if ( !lpszT )
   goto done;

i = ((PATH_MAX * 2) * cSelLBItems) + (sizeof(char *) * (cSelLBItems+1));
hFileList = GlobalAlloc( GPTR, i);
if (!hFileList)
   {
   MessageBox(hWndMain, "Try selecting fewer files at a time",
            "Insufficient Memory", MB_OK |MB_SYSTEMMODAL|
            MB_ICONEXCLAMATION);
   goto done;
   }
szFileList = (LPSTR)GlobalLock(hFileList);
if (!szFileList)
   goto done;

uzpParameters.argc = ((WORD)cSelLBItems == cZippedFiles) ? 0 : 1;
iSelection = 0;
do
   {
   char *p;
   if ((cSelLBItems) || (fUpdateEntries))
      {
      dSpaceToUse = 0;
      pszIndex = (char **)szFileList;
      cch = (sizeof(char *) * ((cSelLBItems > cchFilesMax-1 ) ? cchFilesMax :
         cSelLBItems+1));
      sz = szFileList + cch;

      for (i=0; ((i+iSelection) < cSelLBItems) && (i < cchFilesMax-1); ++i)
         {
         /* Get uncompressed totals to pre-flight the extraction process
          * The pre-flight portion of this needs to be modified to occur
          * only during actual extraction to disk.
          */
         ListView_GetItemText(hWnd, pnSelItems[i+iSelection], 1, lpszT,
               (PATH_MAX * 2));
         lstrcpy(sz, lpszT);
         dSpaceToUse += atol(sz);

         /* Now we get the actual name */
         ListView_GetItemText(hWnd, pnSelItems[i+iSelection], 0, lpszT,
               (PATH_MAX * 2));
         lstrcpy(sz, lpszT+1);
         dStrlen += lstrlen(sz) + 1;
         for (p = sz; *p; p++)
             if (*p == '\\')
                 *p = '/';
         if (dStrlen > (unsigned long)((PATH_MAX + 1) * cSelLBItems))
            {
            MessageBox(hWndMain, "Try selecting fewer files at a time",
            "Insufficient Memory", MB_OK |MB_SYSTEMMODAL|
            MB_ICONEXCLAMATION);
            goto done;
            }
         pszIndex[i] = sz;
         sz += (strlen(lpszT) + 1);
         } /* end of for loop */
      if (i == 0)
         {
         goto done;
         }
      uzpParameters.argc = i;

      pszIndex[i] = 0;
      iSelection += i;
      }
   else
      {
      iSelection = cSelLBItems;
      }

SendMessage(GetActiveMDIChild(), WM_GETTEXT, (WPARAM) PATH_MAX,
   (LPARAM)lpumb->szFileName);
// Set the range and increment of the progress bar.
SendMessage(hProgressBar, PBM_SETRANGE, 0,
            MAKELPARAM(0, dFilesToProcess));
SendMessage(hProgressBar, PBM_SETSTEP, (WPARAM) 1, 0);


switch (wActionCode)
   {
   char tempChar;
   int Ok;

   case IDM_EXTRACT:         /* extract button */
        /* Get amount of free space on target drive */
        tempChar = szUnzipToDirName[3];
        szUnzipToDirName[3] = '\0';
        GetDiskFreeSpace(szUnzipToDirName, &SectorsPerCluster, &BytesPerSector, &FreeClusters, &Clusters);
        szUnzipToDirName[3] = tempChar;
        dFreeSpace = (long)SectorsPerCluster * (long)BytesPerSector * (long)FreeClusters;
        if (dFreeSpace < dSpaceToUse)
           {
           sprintf(sz, "Free Disk Space = %lu bytes\nUncompressed Files Total %lu bytes\n",
              dFreeSpace, dSpaceToUse);
           Ok = MessageBox(hWndMain, sz, "Insufficient Disk Space?", MB_OKCANCEL |
              MB_ICONEXCLAMATION);
           }
        else
           Ok = IDOK;
        if (Ok != IDOK)
           break;
        lpDCL->ncflag = 0;
        lpDCL->fQuiet = 0;
        lpDCL->ntflag = 0;
        lpDCL->nvflag = 0;
        lpDCL->nfflag = lpDCL->ExtractOnlyNewer;
        lpDCL->nzflag = 0;
        lpDCL->ndflag = (int)(uf.fRecreateDirs ? 1 : 0);
        lpDCL->noflag = uf.fDoAll;
        lpDCL->naflag = (int)(uf.fTranslate ? 1 : 0);
        uzpParameters.argv = (char **)szFileList;
        fDisplay = 0;
        if (szUnzipToDirName[0])
           lpDCL->lpszExtractDir = szUnzipToDirName;
        else
           lpDCL->lpszExtractDir = NULL;

        GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);

        lpDCL->lpszZipFN = lpumb->szFileName;

        iGlobalReturnCode =
               (*Unz_SingleEntryPoint)(uzpParameters.argc,
                                       uzpParameters.argv,
                                       uzpParameters.xfnc,
                                       uzpParameters.xfnv,
                                       uzpParameters.lpDCL,
                                       uzpParameters.lpUserFunctions);

        if (szUnzipToDirName[0])
              {
              lstrcpy(lpumb->szBuffer, lpumb->szDirName); /* OK to clobber szBuffer! */
              }
        break;
        case IDM_TEST:     /* test */
            lpDCL->ncflag = 0;
            lpDCL->fQuiet = 0;
            lpDCL->ntflag = 1;
            lpDCL->nvflag = 0;
            lpDCL->nfflag = 0;
            lpDCL->nzflag = 0;
            lpDCL->ndflag = 0;
            lpDCL->noflag = 0;
            lpDCL->naflag = 0;
            lpDCL->nZIflag = 0;
            fDisplay = 0;
            uzpParameters.argv = (char **)szFileList;
            lpDCL->lpszExtractDir = NULL;

            GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);
            lpDCL->lpszZipFN = lpumb->szFileName;

            iGlobalReturnCode =
                   (*Unz_SingleEntryPoint)(uzpParameters.argc,
                                           uzpParameters.argv,
                                           uzpParameters.xfnc,
                                           uzpParameters.xfnv,
                                           uzpParameters.lpDCL,
                                           uzpParameters.lpUserFunctions);


            break;
        case IDM_GET_ZIPINFO:     /* Get Zip Information */
            {
            lpDCL->fQuiet = 0;
            lpDCL->ncflag = 0;
            lpDCL->ntflag = 0;
            lpDCL->nvflag = 0;
            lpDCL->nfflag = 0;
            lpDCL->nzflag = 0;
            lpDCL->ndflag = 0;
            lpDCL->noflag = 0;
            lpDCL->naflag = 0;
            lpDCL->nZIflag = 1;
            lpDCL->lpszZipFN = lpumb->szFileName;
            fDisplay = 0;
            uzpParameters.argv = (char **)szFileList;
            lpDCL->lpszExtractDir = NULL;

            GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);
            lpDCL->lpszZipFN = lpumb->szFileName;

            iGlobalReturnCode =
                (*Unz_SingleEntryPoint)(uzpParameters.argc,
                                        uzpParameters.argv,
                                        uzpParameters.xfnc,
                                        uzpParameters.xfnv,
                                        uzpParameters.lpDCL,
                                        uzpParameters.lpUserFunctions);

            lpDCL->nZIflag = 0;
            break;
            }
        case IDM_ZIP_DELETE_ENTRIES:     /* Delete Archive Entries */
            {
            ZPOPT tempOpt;
            int i, j, k;
            TCHAR szOrigName[PATH_MAX], szTFileName[PATH_MAX];
            TCHAR szTArcName[PATH_MAX];

            ZpZCL.lpszZipFN = lpumb->szFileName;

            /* Save off zip flags */
            tempOpt = ZpOpt;
            /* Set all zip flags to false */
            memset(&ZpOpt, 0, sizeof(ZPOPT));

            ZpOpt.fDeleteEntries = TRUE;
            ZpZCL.argc = uzpParameters.argc;
            ZpZCL.FNV = (char **)szFileList;
            fDisplay = 0;

            lstrcpy(szOrigName, lpumb->szFileName);
            BufferOut( "Deleting Files From Archive: %s\n",
                       lpumb->szFileName);

            ZipSetOptions(&ZpOpt);
            iGlobalReturnCode = ZipArchive(ZpZCL);

            j = ListView_GetItemCount(hWndGrep);
            if ((j > 0) && (iGlobalReturnCode))
               for (i = j-1; i >= 0; i--)
                   {
                   ListView_GetItemText(hWndGrep,
                                        i, // item itself
                                        1, // sub-item
                                        szTArcName, // Name goes here
                                        PATH_MAX); // Max length of name
                   if (!lstrcmpi(szTArcName, szOrigName))
                      {
                      ListView_GetItemText(hWndGrep,
                                           i, // item itself
                                           0, // sub-item
                                           szTFileName, // Name goes here
                                           PATH_MAX); // Max length of name
                      for (k = 0; k < uzpParameters.argc; k++)
                          {
                          if (!lstrcmpi(szTFileName+1, ZpZCL.FNV[k]))
                              ListView_DeleteItem(hWndGrep, i);
                          }
                      }
                   }
            if (iGlobalReturnCode == ZE_OK)
               UpdateListBox();

            /* Restore zip flags */
            ZpOpt = tempOpt;
            }
            break;
        case IDM_UPDATE_ZIP:     /* Update Archive Entries */
            {
            TCHAR tempName[PATH_MAX];
            lstrcpy(tempName, lpumb->szFileName);
            ZpZCL.lpszZipFN = lpumb->szFileName;

            fDisplay = 0;
            ZpZCL.argc = uzpParameters.argc;
            ZpZCL.FNV = (char **)szFileList;

            BufferOut("Updating Files in Archive: %s\n",
                      lpumb->szFileName);

            ZipSetOptions(&ZpOpt);
            iGlobalReturnCode = ZipArchive(ZpZCL);

            SetCurrentDirectory(szHomeDir);

            if (iGlobalReturnCode == ZE_OK)
               {
               lstrcpy(lpumb->szFileName, tempName);
               UpdateListBox();
               }
            break;
            }
          }
    } while (iSelection < cSelLBItems);


    /* march through list box checking what's selected
     * and what is not.
     */

done:

    if ( hMem )
       {
       GlobalUnlock( hMem );
       GlobalFree( hMem );
       }
    if (hFileList)
       {
       GlobalUnlock(hFileList);
       GlobalFree(hFileList);
       }
    if (hnd)
       {
       GlobalUnlock(hnd);
       GlobalFree(hnd);
       }
    SoundAfter();      /* play sound afterward if requested */
}

/* Display the archive comment using the Info-ZIP engine. */
void DisplayComment(void)
{
SendMessage(GetActiveMDIChild(), WM_GETTEXT, (WPARAM) PATH_MAX,
   (LPARAM)lpumb->szFileName);
lpDCL->lpszZipFN = lpumb->szFileName;

bRealTimeMsgUpdate = FALSE;
/* Called here when showing zipfile comment */
lpDCL->ncflag = 0;
lpDCL->fQuiet = 0;
lpDCL->ntflag = 0;
lpDCL->nvflag = 0;
lpDCL->nfflag = 0;
lpDCL->nzflag = 1;
lpDCL->ndflag = 0;
lpDCL->noflag = 0;
lpDCL->naflag = 0;
uzpParameters.argc   = 0;
uzpParameters.argv = NULL;
lpDCL->lpszExtractDir = NULL;
iGlobalReturnCode =
    (*Unz_SingleEntryPoint)(uzpParameters.argc,
                            uzpParameters.argv,
                            uzpParameters.xfnc,
                            uzpParameters.xfnv,
                            uzpParameters.lpDCL,
                            uzpParameters.lpUserFunctions);
bRealTimeMsgUpdate = TRUE;
}

