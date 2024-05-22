/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#ifdef __BORLANDC__
#include <dir.h>
#endif
#include <direct.h>
#include <string.h>
#include <io.h>
#include <stdio.h>
#include <ctype.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include "wiz.h"

//MW #include "helpids.h"

BOOL fMakeSFX;
BOOL fMakeDosSFX;
char szTempFileName[PATH_MAX];
char szTargetName[PATH_MAX];

char szTargetFileName[PATH_MAX] = "";
char szTargetZipDir[PATH_MAX];
char szTargetZipFile[PATH_MAX] = "";

void MakeArchive(HWND hWnd)
{
char tempFileName[PATH_MAX], tempPath[PATH_MAX];
ZPOPT TempZpOpt;
TCHAR szSaveDirPath[PATH_MAX];

/* temporary zip options flags */
BOOL fmakesfx, fdossfx;

/* Save zip options flags */
TempZpOpt = ZpOpt;
fmakesfx      = fMakeSFX;
fdossfx       = fMakeDosSFX;
lstrcpy(szSaveDirPath, szTempDirPath);

memset(&lpumb->ofn, '\0', sizeof(OPENFILENAME)); /* initialize struct */
WinAssert(hWnd);

lpumb->szUnzipToDirNameTmp[0] = '\0';

lpumb->ofn.lStructSize = sizeof(OPENFILENAME);
lpumb->ofn.hwndOwner = hWnd;
lpumb->ofn.hInstance = hInst;
lpumb->ofn.lpstrFilter = "Zip Files (*.zip)\0*.zip\0Self-extracting Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0\0";
lpumb->ofn.nFilterIndex = 1;
if (bUseCurrentZip)
   {
   GetWindowText(GetActiveMDIChild(), lpumb->szUnzipToDirNameTmp, PATH_MAX);
   }
lpumb->ofn.lpstrFile = lpumb->szUnzipToDirNameTmp; /* result goes here! */
lpumb->ofn.nMaxFile = PATH_MAX;
lpumb->ofn.lpstrFileTitle = NULL; /* NULL causes nMaxFileTitle to be ignored */
lpumb->ofn.nMaxFileTitle = PATH_MAX; /* ignored ! */
lpumb->ofn.lpstrTitle = (LPSTR)"Archive Name";
lpumb->ofn.lpstrInitialDir =
   (LPSTR)(!szTargetZipDir[0] ? NULL : szTargetZipDir);
//MW lpumb->ofn.Flags = OFN_SHOWHELP | OFN_HIDEREADONLY;
lpumb->ofn.Flags = OFN_HIDEREADONLY;
uf.fTrailingSlash = FALSE; /* set trailing slash to FALSE */
lpumb->szZipFileName[0] = '\0';

//MW dwCommDlgHelpId = HELPID_ZIP_GET_ARCHIVE_NAME;

if (bUseCurrentZip || GetOpenFileName(&lpumb->ofn)) /* successfully got archive name ? */
   {
   if (lstrcmpi(lpumb->szUnzipToDirNameTmp, lpumb->szZipFileName)!=0)
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
      /* Okay, make the directory, if necessary */
      lstrcpy(szTargetZipDir, lpumb->szUnzipToDirNameTmp);
      if (lpumb->szUnzipToDirNameTmp[0] != '\0')
         lstrcat(lpumb->szUnzipToDirNameTmp, "\\");
      lstrcat(lpumb->szUnzipToDirNameTmp, tempFileName);
         /* lpumb->szUnzipToDirNameTmp now contains the fully qualified
            zip file name.
         */
      if (strrchr(lpumb->szUnzipToDirNameTmp, '.') == NULL)
         {
         if (!fMakeSFX)
            lstrcat(lpumb->szUnzipToDirNameTmp, ".zip");
         else
            lstrcat(lpumb->szUnzipToDirNameTmp, ".exe");
           }
         lstrcpy(szTargetFileName, lpumb->szUnzipToDirNameTmp);
         lstrcpy(lpumb->szZipFileName, lpumb->szUnzipToDirNameTmp);
         ZpZCL.lpszZipFN = szTargetFileName;


      GetCurrentDirectory(PATH_MAX, szTargetZipDir);
      WriteZipOptionsProfile();

   /* NOTE: Here is where we must actually call to get the files
      to go into the archive. All we've done so far is get the
      archive name.
   */
      {
//MW      dwCommDlgHelpId = HELPID_ZIP_SELECT_FILES;
      memset(&lpumb->ofn, '\0', sizeof(OPENFILENAME)); /* initialize struct */
      WinAssert(hWnd);
      szTargetZipFile[0] = '\0';
      lpumb->ofn.lStructSize = sizeof(OPENFILENAME);
      lpumb->ofn.hwndOwner = hWnd;
      lpumb->ofn.hInstance = hInst;
      lpumb->ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
      lpumb->ofn.nFilterIndex = 1;

      lpumb->ofn.lpstrFile = szTargetZipFile;
      lpumb->ofn.nMaxFile = PATH_MAX;
      lpumb->ofn.lpstrFileTitle = NULL;
      lpumb->ofn.nMaxFileTitle = PATH_MAX; /* ignored ! */

      wsprintf(lpumb->szBuffer, "Archive %s", lpumb->szZipFileName);

      lpumb->ofn.lpstrTitle = (LPSTR) lpumb->szBuffer;
      lpumb->ofn.lpstrInitialDir =
         (LPSTR)(!szTargetZipDir[0] ? NULL : szTargetZipDir);
//MW      lpumb->ofn.Flags = OFN_SHOWHELP | OFN_ENABLEHOOK | OFN_CREATEPROMPT |
      lpumb->ofn.Flags = OFN_ENABLEHOOK | OFN_CREATEPROMPT |
         OFN_HIDEREADONLY|OFN_ENABLETEMPLATE; //|OFN_NONETWORKBUTTON;
      lpumb->ofn.lpfnHook = (LPOFNHOOKPROC)GetFilesProc;
      lpumb->ofn.lpTemplateName = "GETFILES";   /* see getfiles.dlg   */
      if (GetOpenFileName(&lpumb->ofn) && (hFileList &&
             (ZpZCL.lpszZipFN[0] != '\0')) ||
             ((ZpOpt.fRepair || ZpOpt.fOffsets || ZpOpt.fFreshen) &&
             (ZpZCL.lpszZipFN[0] != '\0')))
         {
         CleanScreen();

         ZpZCL.FNV = (char **)szFileList;
         if (!ZpOpt.fRepair)
            BufferOut( "Creating/Updating Archive: %s\n",
               lpumb->szZipFileName);
         else
            if (ZpZCL.argc != 0)
               BufferOut( "Repairing/Updating Archive: %s\n",
                  lpumb->szZipFileName);
            else
               BufferOut( "Repairing Archive: %s\n",
                  lpumb->szZipFileName);
         if (fMakeSFX)
            {
            char *ptr;
            WIN32_FIND_DATA ffblk;
            HANDLE hfile;
            int in;

            ptr = strrchr(lpumb->szZipFileName, '.') + 1;
            *ptr = '\0';
            lstrcat(lpumb->szZipFileName, "exe");
            lstrcpy(ZpZCL.lpszZipFN, lpumb->szZipFileName);
            ZpOpt.fOffsets = TRUE;

            /* Get the path we are executing from */
            GetModuleFileName(hInst, szTargetName,
                  sizeof(szTargetName));
            ptr = strrchr(szTargetName, '\\') + 1;
            *ptr = '\0'; /* We've now got the path we are executing from */
            if (fMakeDosSFX)
               lstrcat(szTargetName, "sfx16.dat");
            else
               lstrcat(szTargetName, "sfx32.dat");

            if ((hfile = FindFirstFile(ZpZCL.lpszZipFN, &ffblk)) == INVALID_HANDLE_VALUE)
               {
                // Just to check if file exists
               in = open(szTargetName, O_RDONLY | O_BINARY);
               close(in);

               if (in < 0)
                  {
                  if (fMakeDosSFX)
                     MessageBox (hWnd, "Can't find SFX16.DAT, continuing without making SFX exe.",
                        "Can't find SFX16.DAT", MB_ICONINFORMATION | MB_OK);
                  else
                     MessageBox (hWnd, "Can't find SFX32.DAT, continuing without making SFX exe.",
                        "Can't find SFX32.DAT", MB_ICONINFORMATION | MB_OK);
                  }
               else
                  {
                  CopyFile(szTargetName, ZpZCL.lpszZipFN, FALSE);
                  }
               }
            else
               {
               int in, out;
               FindClose(hfile);

               if (fMakeDosSFX)
                  lstrcat(szTargetName, "sfx16.dat");
               else
                  lstrcat(szTargetName, "sfx32.dat");

               /* Copy the prefix file over to the temp file */
               // Just to check if file exists
               in = open(szTargetName, O_RDONLY | O_BINARY);
               close(in);
               if (in < 0)
                  {
                  if (fMakeDosSFX)
                     MessageBox (hWnd, "Can't find SFX16.DAT, continuing without making SFX exe.",
                        "Can't find SFX16.DAT", MB_ICONINFORMATION | MB_OK);
                  else
                     MessageBox (hWnd, "Can't find SFX32.DAT, continuing without making SFX exe.",
                        "Can't find SFX32.DAT", MB_ICONINFORMATION | MB_OK);
                  }
               else
                  {
                  char buf[READ_WRITE_BUF];
                  /* Get a temp file name and directory (Goes in directory
                   * designated by tmp, temp or the current directory)
                   */
                  GetTempPath(sizeof(szTargetName), szTargetName);
                  GetTempFileName(szTargetName, "WIZ~", 0, szTempFileName);

                  CopyFile(szTargetName, szTempFileName, FALSE);

                  /* We've got the prefix moved over, now append the
                   * archive file itself.
                   */
                  out = open(szTempFileName, O_RDWR | O_APPEND | O_BINARY );
                  in = open(ZpZCL.lpszZipFN, O_RDONLY | O_BINARY);

                  lseek(out, SEEK_CUR, SEEK_END);
                  while (read(in, buf, READ_WRITE_BUF) > 0)
                     {
                     write(out, buf, READ_WRITE_BUF);
                     }

                  close(in);
                  close(out);

                  /* Copy the temp file back over the original file */
                  // Just to check if file exists
                  in = open(szTargetName, O_RDONLY | O_BINARY);
                  close(in);
                  if (in < 0)
                     {
                     MessageBox (hWnd, "Can't find temp file, continuing without making SFX exe.",
                        "Can't find temp file", MB_ICONINFORMATION | MB_OK);
                     }
                  else
                     {
                     CopyFile(szTempFileName, ZpZCL.lpszZipFN, FALSE);

                     remove(szTempFileName);
                     lstrcpy(szTempFileName, ZpZCL.lpszZipFN);
                     if (lstrcmpi(&szTempFileName[lstrlen(ZpZCL.lpszZipFN) - 4],
                        ".exe") != 0)
                        {
                        lstrcpy(&ZpZCL.lpszZipFN[lstrlen(ZpZCL.lpszZipFN) - 4],
                           ".exe");
                        rename(szTempFileName, ZpZCL.lpszZipFN);
                        }
                     }
                  }
               }
            }
         DialogBox(hInst, // This is where we actually call to do the zipping
                   "SERVICEAPP",
                   hWndMain,
                   (DLGPROC)ServiceProc);
         }
      }

      if ((toupper(lpumb->ofn.lpstrFile[0]) != 'A') &&
          (toupper(lpumb->ofn.lpstrFile[0]) != 'B'))
          {
          lstrcpy(szTargetZipFile, lpumb->ofn.lpstrFile);
          if ((lpchLast = lstrrchr(szTargetZipFile, '\\'))!=0)
             {
             *lpchLast = '\0';
             }
           else if ((lpchLast = lstrrchr(szTargetZipFile, ':'))!=0)
             {
             *(lpchLast+1) = '\0';
             lstrcat(szTargetZipFile, "\\");
             }
           }
        }
      }
else /* either real error or canceled */
   {
   DWORD dwExtdError = CommDlgExtendedError(); /* debugging */

   if (dwExtdError != 0L) /* if not canceled then real error */
      {
      wsprintf (lpumb->szBuffer,
                GetLoadString(IDS_CHG_DIR_ERROR),
                dwExtdError);
      MessageBox (hWnd,
                  lpumb->szBuffer,
                  szAppName,
                  MB_ICONINFORMATION | MB_OK);
      }
   else
      {
      CleanScreen();
      if (ZpZCL.lpszZipFN != NULL)
         ZpZCL.lpszZipFN[0] = '\0';
      }
   }
/* Restore zip options flags */
ZpOpt = TempZpOpt;
fMakeSFX            = fmakesfx;
fMakeDosSFX         = fdossfx;
lstrcpy(szTempDirPath, szSaveDirPath);
if (hFileList)
   {
   GlobalUnlock(hFileList);
   GlobalFree(hFileList);
   }
}

