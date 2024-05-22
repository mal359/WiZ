/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <string.h>
#include <io.h>
#include <stdio.h>
#include <ctype.h>
#include "wiz.h"

//MW #include "helpids.h"

char szTargetDirName[PATH_MAX] = "";

void StripDirectory(LPSTR lpDir);
/* Strip off the directory leaving only the file name */
void StripDirectory(LPSTR lpDir)
{
LPSTR lpchLast, i, j;
/* If no '\\' then simply return */
if ((lpchLast = lstrrchr(lpDir, '\\')) == 0)
   {
   return;
   }
if ((lpchLast = lstrrchr(lpDir, '\\'))!=0)
   {
   for (i = lpchLast+1, j = lpDir; *i; i++, j++)
      {
      *j = *i;
      }
   *j = '\0';
   }

else if ((lpchLast = lstrrchr(lpDir, ':'))!=0)
   {
   for (i = lpchLast+1, j = lpDir; *i; i++, j++)
      {
      *j = *i;
      }
   *j = '\0';
   }
}

//#if 0
BOOL WINAPI CopyFileProc(HWND hDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
    switch (wMessage)
    {
    case WM_INITDIALOG:
        {
/*
Common control identifiers for GetOpenFileName and GetSaveFileName
Control identifier   Control Description
cmb2                 Drop-down combo box that displays the current drive
                     or folder, and that allows the user to select a
                     drive or folder to open
stc4                 Label for the cmb2 combo box
lst1                 List box that displays the contents of the current drive or folder
stc1                 Label for the lst1 list box
edt1                 Edit control that displays the name of the current file, or in
                     which the user can type the name of the file to open
stc3                 Label for the edt1 edit control
cmb1                 Drop-down combo box that displays the list of file type filters
stc2                 Label for the cmb1 combo box
chx1                 The read-only check box
IDOK                 The OK command button (push button)
IDCANCEL             The Cancel command button (push button)
pshHelp              The Help command button (push button)

*/
        LPSTR sz;
        if (move_flag)
           sz = "Move";
        else
           if (rename_flag)
              sz = "Rename";
           else
              sz = "Copy";
        CommDlg_OpenSave_SetControlText(GetParent(hDlg), IDOK, sz);
        CommDlg_OpenSave_SetControlText(GetParent(hDlg),
                                        edt1,
                                        lpumb->szUnzipToDirNameTmp);
        }
    default:
        break;
    }

    /* message not handled */
return DefWindowProc(hDlg, wMessage, wParam, lParam);
}

void CopyArchive(HWND hWnd, BOOL move_flag, BOOL rename_flag)
{
char tempFileName[PATH_MAX], tempPath[PATH_MAX];

memset(&lpumb->ofn, '\0', sizeof(OPENFILENAME)); /* initialize struct */
WinAssert(hWnd);
lpumb->ofn.lStructSize = sizeof(OPENFILENAME);
lpumb->ofn.hwndOwner = hWnd;
lpumb->ofn.hInstance = hInst;
lpumb->ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
lpumb->ofn.nFilterIndex = 1;

/* strip off filename */
lstrcpy(lpumb->szUnzipToDirNameTmp, lpumb->szFileName);
/* Strip off path */
StripDirectory(lpumb->szUnzipToDirNameTmp);
/* Now we've got just the file name */
lpumb->ofn.lpstrFile = lpumb->szUnzipToDirNameTmp; /* result goes here! */
/* Save file name only */
lstrcpy(tempFileName, lpumb->szUnzipToDirNameTmp);
lpumb->ofn.nMaxFile = PATH_MAX;
lpumb->ofn.lpstrFileTitle = NULL;
lpumb->ofn.nMaxFileTitle = PATH_MAX; /* ignored ! */
#if 0
if (move_flag)
   {
   lpumb->ofn.lpstrTitle = (LPSTR)"Move Archive To";
   lpumb->ofn.lpstrInitialDir =
   (LPSTR)(!szTargetDirName[0] ? NULL : szTargetDirName);
   }
else
   if (rename_flag)
      {
      lpumb->ofn.lpstrTitle = (LPSTR)"Rename Archive To";
      lpumb->ofn.lpstrInitialDir = (LPSTR)((uf.fUnzipToZipDir ||
         !lpumb->szDirName[0]) ? NULL : lpumb->szDirName);
      }
   else
      {
#endif
lpumb->ofn.lpstrTitle = (LPSTR)"Copy Archive To";
lpumb->ofn.lpstrInitialDir =
       (LPSTR)(!szTargetDirName[0] ? NULL : szTargetDirName);
//      }
//MW lpumb->ofn.Flags = OFN_SHOWHELP |OFN_EXPLORER|OFN_ENABLEHOOK|
lpumb->ofn.Flags = OFN_EXPLORER|OFN_ENABLEHOOK|
   OFN_HIDEREADONLY|OFN_NOCHANGEDIR;
lpumb->ofn.lpfnHook = (LPOFNHOOKPROC)CopyFileProc;
uf.fTrailingSlash = FALSE; /* set trailing slash to FALSE */
if (GetSaveFileName(&lpumb->ofn)) /* successfully got dir name ? */
   {
   // Make sure window is restored before copying
   CleanScreen();

   hSaveCursor = SetCursor(hHourGlass);
   ShowCursor(TRUE);

   if (lstrcmpi(lpumb->szUnzipToDirNameTmp, lpumb->szFileName)!=0)
      {
      lstrcpy(tempPath, lpumb->szUnzipToDirNameTmp);
      if (!uf.fTrailingSlash) /* Do we have a trailing slash? */
         { /* No - strip file name off and save it */
         StripDirectory(tempPath);
         lstrcpy(tempFileName, tempPath);
         /* Strip file name off, we've just saved it */
         GetDirectory(lpumb->szUnzipToDirNameTmp);
         }
      uf.fTrailingSlash = FALSE;

      if (lpumb->szUnzipToDirNameTmp[0] != '\0')
         lstrcat(lpumb->szUnzipToDirNameTmp, "\\");
      lstrcat(lpumb->szUnzipToDirNameTmp, tempFileName);

      if (move_flag == TRUE)
         BufferOut( "Moving: \n%s\nto\n%s",
            lpumb->szFileName, lpumb->ofn.lpstrFile);
      else
         if (rename_flag == FALSE)
            BufferOut( "Copying: \n%s\nto\n%s",
               lpumb->szFileName, lpumb->ofn.lpstrFile);

      if ((rename_flag == TRUE) &&
         ((lpumb->szFileName[0] == lpumb->ofn.lpstrFile[0]) ||
         (lpumb->ofn.lpstrFile[1] != ':')))
         {
         BufferOut( "Renaming: \n%s\nto\n%s\n",
            lpumb->szFileName, lpumb->ofn.lpstrFile);
         rename(lpumb->szFileName, lpumb->ofn.lpstrFile);
         lstrcpy(lpumb->szFileName, lpumb->ofn.lpstrFile);
         SetWindowText(GetActiveMDIChild(), lpumb->szFileName);
         UpdateButtons(); /* update state of buttons */
         }
      else if (rename_flag == FALSE)
         {
         CopyFile(lpumb->szFileName, lpumb->ofn.lpstrFile, FALSE);
         BufferOut( " ...%s\n", "Done");
         if ((toupper(lpumb->ofn.lpstrFile[0]) != 'A') &&
            (toupper(lpumb->ofn.lpstrFile[0]) != 'B'))
            {
            lstrcpy(szTargetDirName, lpumb->ofn.lpstrFile);
            if ((lpchLast = lstrrchr(szTargetDirName, '\\'))!=0)
               {
               *lpchLast = '\0';
               }
            else if ((lpchLast = lstrrchr(szTargetDirName, ':'))!=0)
               {
               *(lpchLast+1) = '\0';
               lstrcat(szTargetDirName, "\\");
               }
            }
         }
         if (move_flag)
            {
            HWND hTemp;
            hTemp = GetActiveMDIChild();
            WinAssert(hTemp);
            DeleteFile(lpumb->szFileName);
            lstrcpy(lpumb->szFileName, lpumb->ofn.lpstrFile);
            SetWindowText(hTemp, (LPCTSTR)lpumb->szFileName);
            UpdateButtons(); /* update state of buttons */
            }
         }
   ShowCursor(FALSE);
   SetCursor(hSaveCursor);
   ShowCursor(TRUE);
   }
else /* either real error or canceled */
   {
   DWORD dwExtdError = CommDlgExtendedError(); /* debugging */

   if (dwExtdError != 0L) /* if not canceled then real error */
      {
      wsprintf (lpumb->szBuffer,
                GetLoadString(IDS_INTERNAL_COPY_ERROR),
                dwExtdError);
      MessageBox (hWnd,
                  lpumb->szBuffer,
                  szAppName,
                  MB_ICONINFORMATION | MB_OK);
      }
   else
      {
      CleanScreen();
      }
   }
}
//#endif

