/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <stdio.h>
#include <stdarg.h>

#include "wiz.h"
#include "unzip\unzip.h"

extern unsigned long dSpaceToUse;

const TCHAR szRTFSig[] = TEXT("{\\rtf");
const TCHAR szHTMLSig[] = TEXT("<HTML>");

/* Globals */
BOOL bRealTimeMsgUpdate = TRUE; /* update messages window in real-time.
                                 * Reset by callers when update can be
                                 * be deferred.
                                 */
int fDisplay = 0;  /* Display flag.
                      0 = text or rtf, send to WizEdit
                      1 = archive, display ourselves
                      2 = wiz unable to display, let Windows
                          try.
                    */
BOOL fExternalRTF = FALSE;   // Do we want to read rtf files ourselves?
BOOL fNotifyClearing = TRUE; // Do we want to notify user if buffer full?
LPSTR szTmpBuffer, lpStr;
HANDLE hMemory;
LPSTR pszBuffer; /* Buffer used to feed the edit control */
BOOL fSpool = FALSE;
DWORD dwBytesRead, dwEditBufSize = EDIT_BUF_SIZE;
HANDLE hDisplayFile = NULL; // File handle for writing non-displayable files
int iGlobalReturnCode, iZipWarn;

/* Forward references */
BOOL MapCRtoCRLF(LPSTR pszOrigBuffer);
DWORD CALLBACK BufferRead(DWORD, LPBYTE, LONG, LONG *);

/* Map CR to CRLF is called by BufferOut below
 * to guarantee that lines added to the Status window will be
 * properly terminated DOS-style in case they are copied
 * to the Windows clipboard.
 * This converts the buffer in-place, provided it won't
 * grow beyond the original buffer allocation.
 */

BOOL MapCRtoCRLF(LPSTR pszOrigBuffer)
{
LPSTR pC, pDest;
UINT cBufLen = 0;  /* no. chars in buffer, including final null */
UINT cLFs = 0;     /* no. LF's in string. \n is linefeed */

for (pC = pszOrigBuffer; *pC && (cBufLen < dwEditBufSize); pC++)
   {
   cBufLen++;
   /* Take care of the case of a leading '\n' */
   if ((cBufLen == 1) && (*pC == '\n'))
      cLFs++;
   else
      {
      if (*pC == 0x0C)   /* found a formfeed */
         *pC = '\n';     /* Make a linefeed */
      if ((*pC == '\n') && (*(pC-1) != '\r'))   /* found a linefeed */
         cLFs++;
      }
   }
cBufLen++;   /* buffer length includes final null */
pC = &pszOrigBuffer[cBufLen-1]; /* point to old end's null char */
if (cBufLen + cLFs >= dwEditBufSize) /* room to add CR's ? */
   return FALSE;             /* if not, don't bother */

/* copy data back-to-front, effectively inserting CR before LF */
pDest = &pszOrigBuffer[cBufLen+cLFs-1]; /* point to new end */
for  (; cBufLen; pC--, cBufLen--)
   {
   *pDest-- = *pC ;    /* copy data byte */
   if ((*pC == '\n') && ((*(pC-1)) != '\r'))    /* was that a linefeed? */
      *pDest-- = '\r'; /* if so, insert CR */
   }
return TRUE;
}

int __far __cdecl BufferOut(const char *format, ...)
{
char c;
va_list argptr;
unsigned int len, txtlen;
EDITSTREAM es;

va_start(argptr, format);
hMemory = LocalAlloc(LMEM_MOVEABLE, (FPRINTF_BUF_SIZE + CRLF_BUF_SIZE));
WinAssert(hMemory);
if (!hMemory)
   {
   return 0;
   }
szTmpBuffer = (LPSTR)LocalLock(hMemory);
WinAssert(szTmpBuffer);
vsprintf(szTmpBuffer, format, argptr);
va_end(argptr);
MapCRtoCRLF(szTmpBuffer);
len = lstrlen(szTmpBuffer);

// Check to see if the zip library has found something to warn about.
c = szTmpBuffer[9];
szTmpBuffer[9] = '\0';
if (lstrcmp("\tzip warn", szTmpBuffer) == 0)
   iZipWarn = 1;
szTmpBuffer[9] = c;

/* WM_GETTEXTLENGTH returns the length of the text in the edit control, but
   does not include the terminating NULL, hence, add 1.

   Get the text in the edit control in case the user has added or deleted
   text.
 */
txtlen = (unsigned int)SendMessage(hWndMessage, WM_GETTEXTLENGTH, 0, 0);
if (txtlen != 0)
   txtlen++;
SendMessage(hWndMessage, WM_GETTEXT, txtlen, (LPARAM)pszBuffer);
if ((unsigned long)(len + txtlen) >= dwEditBufSize-1)
   {
   char sz1[RESOURCE_STR_LEN], sz2[RESOURCE_STR_LEN];
   if (fNotifyClearing)
      {
      lstrcpy(sz1, GetLoadString(IDS_CLEARBUFFER));
      lstrcpy(sz2, GetLoadString(IDS_NOTE));
      MessageBox (hWndMain,
                 sz1,
                 sz2,
                 MB_ICONINFORMATION | MB_OK);
      }
   lstrcpy(pszBuffer, szTmpBuffer);
   }
else
   lstrcat(pszBuffer, szTmpBuffer);

es.dwError = 0;
es.pfnCallback = BufferRead;

es.dwCookie = (DWORD)lstrlen(pszBuffer); // Total bytes to read
dwBytesRead = 0;                         // Bytes read by BufferRead
lpStr = pszBuffer;
SendMessage(hWndMessage, EM_STREAMIN,
   (WPARAM) SF_TEXT,
   (LPARAM) &es);
LocalUnlock(hMemory);
LocalFree(hMemory);
/* Return the actual number of bytes being printed */
return len;
}

//
//  FUNCTION: BufferRead(DWORD dwCookie, LPBYTE lpsz, LONG dwSize,
//   LONG *bytes_read)
//
//  PURPOSE: Callback function to read contents of buffer into an edit control
//
//  PARAMETERS:
//    dwCookie - Size of buffer to be read into
//    lpsz     - Buffer to be read into
//    dwSize   - Number of bytes requested
//    bytes_read - Number of bytes actually read.
//
//  RETURN VALUE:
//    Number of bytes read
//
//  COMMENTS:
//

DWORD CALLBACK BufferRead(DWORD dwCookie, LPBYTE lpsz, LONG dwSize,
   LONG *bytes_read)
{
LONG lSize = dwSize;
if ((dwSize + dwBytesRead) > dwCookie)
   lSize = dwCookie - dwBytesRead;
memcpy(lpsz, lpStr, lSize);
lpStr += lSize;
dwBytesRead += lSize;
*bytes_read = lSize;
return (DWORD) (*bytes_read >= 0 ? NOERROR : (*bytes_read = 0, E_FAIL));
}

int WINAPI DisplayBuf(char *buf, unsigned long size)
{
static BOOL fDump = FALSE;
unsigned int i, txtlen;
char *p;

/* If fDisplay == 3, then we don't want any messages coming in at all, so
   we just return.
 */  
if (fDisplay == 3)
   {
   EmptyMessageQueue();
   return (unsigned int) size;
   }

/* Just a sanity check - size should normally not exceed 32K */
if (size > (dwEditBufSize-1))
    size = dwEditBufSize-1;

/* The fQuiet flag is only set if we are sending messages to the message
   window. This simply sends any "messages" to the message window
 */
if (!lpDCL->fQuiet)
   {
   BufferOut("%s", buf);
   EmptyMessageQueue();
   return (unsigned int) size;
   }
if (!fSpool) /* Are we just starting a dump to memory? */
   {
   fDump = FALSE;
   memset(pszBuffer, '\0', dwEditBufSize);
   }
/* Have we already started sending data to display? If so, then just
    throw away anything other than the first dwEditBufSize characters
 */
if (fDump)
   {
   EmptyMessageQueue();
   return 0;
   }

/* We're going to look at the first 100 characters. If any are control
   characters and not either a tab, line feed, form feed or carriage return,
   we assume that we have a binary file of some type.
 */
if (!fSpool)
   {
   if (size < 100)
      txtlen = size;
   else
      txtlen = 100;
   for (i = 0; i < txtlen; i++)
      {
      if (iscntrl(buf[i]))
         {
         if ((buf[i] != 0x0A) && // CR
             (buf[i] != 0x0D) && // LF
             (buf[i] != 0x0C) && // FF
             (buf[i] != 0x09))   // Tab
            {
            /* Okay, we now know we have a binary file of some sort */
            if ((buf[0] == 'P') &&
                (buf[1] == 'K'))
               fDisplay = 1; // We've possibly got a zip archive
            else
               fDisplay = 2; //Unknown binary file
            break;
            }
         }
      }
   }

/* Let's see if this is HTML, it looks like text, but we can't read it
   properly. If it is, we will pretend that it is an unknown binary file.
 */
if ((!fSpool) && (!fDisplay))
   {
   TCHAR c;
   c = buf[6];
   buf[6] = '\0';
   if (lstrcmpi(szHTMLSig, buf) == 0)
      fDisplay = 2;
   buf[6] = c;
   }
if ((!fSpool) && (!fDisplay) && fExternalRTF)
   {
   TCHAR c;
   // If we want to use an external editor for RTF, then we will pretend
   // that it is an unknown binary file.
   c = buf[5];
   buf[5] = '\0';
   if (lstrcmpi(szRTFSig, buf) == 0)
      fDisplay = 2;
   buf[5] = c;
   }
/* If we are just starting, and we are not actually sending the buffer
   to WizEdit, then we need to create a temporary file in the temporary
   directory, and send everything to that file.
 */
if ((!fSpool) && (fDisplay))
   {
   HANDLE hTemp;
   WIN32_FIND_DATA fd;
   TCHAR szTempFileName[PATH_MAX];
   LPSTR lpchLast, i, j;
   GetTempPath(PATH_MAX, szWindowsTempDir);
   if (((lpchLast = lstrrchr(uzpParameters.argv[0], '\\')) == 0) &&
      ((lpchLast  = lstrrchr(uzpParameters.argv[0], '/'))  == 0))
      lstrcat(szWindowsTempDir, uzpParameters.argv[0]);
   else
      {
      lstrcpy(szTempFileName, uzpParameters.argv[0]);
      if ((lpchLast = lstrrchr(szTempFileName, '/'))!=0)
         {
         for (i = lpchLast+1, j = szTempFileName; *i; i++, j++)
             {
             *j = *i;
             }
        *j = '\0';
        }
      if ((lpchLast = lstrrchr(szTempFileName, '\\'))!=0)
         {
         for (i = lpchLast+1, j = szTempFileName; *i; i++, j++)
             {
             *j = *i;
             }
        *j = '\0';
        }
      lstrcat(szWindowsTempDir, szTempFileName);
      }
   hTemp = FindFirstFile(szWindowsTempDir, &fd);
   if (hTemp != INVALID_HANDLE_VALUE)
      {
      LPSTR i, j, k;
      FindClose(hTemp);
      GetTempPath(PATH_MAX, szWindowsTempDir);
      GetTempFileName((LPCTSTR)szWindowsTempDir,
                      (LPCTSTR)"WIZ~",
                      1,
                      (LPTSTR)szTempFileName);
      // We need to retain the file extension for association purposes
      k = lstrrchr(szTempFileName, '.')+1;
      j = lstrrchr(uzpParameters.argv[0], '.')+1;
      for (i = k; *j; i++, j++)
          *i = *j;
      *i = '\0';
      lstrcpy(szWindowsTempDir, szTempFileName);
      }
   }
fSpool = TRUE; /* Flag that we have started dumping to memory */

switch (fDisplay) {
   case 0: // text or rtf, send to WizEdit
      txtlen = lstrlen(pszBuffer);
      if ((txtlen+size) > (dwEditBufSize - 1))
         {
         HGLOBAL hTemp;
         GlobalUnlock(hEditor);
         hTemp = GlobalReAlloc(hEditor, (DWORD)dwEditBufSize + size + 0x1000,
            GMEM_MOVEABLE);
         if (!hTemp)
            {
            MessageBox((HWND)NULL,
               "Could not re-allocate memory, file too large. Truncating file.",
               szAppName, MB_ICONSTOP | MB_OK);
            fDump = TRUE; /* Okay, start throwing away data */
            size = dwEditBufSize - txtlen - 1;
            }
         else
            {
            dwEditBufSize = dwEditBufSize + size + 0x1000;
            hEditor = hTemp;
            }
         pszBuffer = (LPSTR)GlobalLock(hEditor);
         }
      p = &pszBuffer[txtlen];
      memcpy(p, buf, (unsigned int)size);
      pszBuffer[(unsigned int)(txtlen+size+1)] = '\0';
      i = lstrlen(pszBuffer);
      while (!MapCRtoCRLF(pszBuffer) && (i != 0))
         {
         pszBuffer[--i] = '\0';
         }
      break;
   case 1: // May be archive, handle ourselves
   case 2: // we can't display, let windows try (may be executable)
      {
      DWORD dwWritten;
      DWORD SectorsPerCluster, BytesPerSector, FreeClusters, Clusters;
      unsigned long dFreeSpace;
      char tempChar;
      TCHAR sz[PATH_MAX * 2];
      int Ok;
      /* Get amount of free space on target drive */
      tempChar = szWindowsTempDir[3];
      szWindowsTempDir[3] = '\0';
      GetDiskFreeSpace(szWindowsTempDir,
                       &SectorsPerCluster,
                       &BytesPerSector,
                       &FreeClusters,
                       &Clusters);
      szWindowsTempDir[3] = tempChar;
      dFreeSpace = (long)SectorsPerCluster *
                   (long)BytesPerSector *
                   (long)FreeClusters;
      if (dFreeSpace < dSpaceToUse)
         {
         sprintf(sz, "Free Disk Space = %lu bytes\nUncompressed Files Total %lu bytes\n",
                 dFreeSpace,
                 dSpaceToUse);
         Ok = MessageBox(hWndMain, sz, "Insufficient Disk Space!",
                         MB_OKCANCEL | MB_ICONEXCLAMATION);
         }
      else
         Ok = IDOK;
      if (Ok != IDOK)
         break;
      hDisplayFile = CreateFile(szWindowsTempDir,
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         NULL,
                         OPEN_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);
      SetFilePointer(hDisplayFile, 0, NULL, FILE_END);
      WriteFile(hDisplayFile,
                buf,
                (DWORD)size,
                &dwWritten,
                NULL);
      CloseHandle(hDisplayFile);
      break;
      }
   }
EmptyMessageQueue();
return (unsigned int) size;
}

void DisplayFiles(HWND hWnd, BOOL bSubstitute)
{
LPSTR lpszT;
LV_ITEM lvi;
BOOL fSwitchToEditor = FALSE;
unsigned int txtlen;
int i, j;
HANDLE hMem;
int cc, retcode;
char **index, *p;

fDisplay = 0;
hMem = GlobalAlloc( GPTR, (PATH_MAX * 2));
if ( !hMem )
   return;
lpszT = (LPSTR)GlobalLock( hMem );
if ( !lpszT )
   {
   GlobalFree( hMem );
   return;
   }
j = ListView_GetSelectedCount(hWnd);

/* Allocate space for an array of filenames in one step
   need to allocate space for the array of ptrs + 1 for NULL ptr
   and allocate PATH_MAX*2 chars for each filename
   since that's how much space is allocated for the temp sz buffer
*/
i = ((PATH_MAX * 2) * j) + (sizeof(char *) * (j+1));
hFileList = GlobalAlloc( GPTR, i);
if (!hFileList)
   {
   GlobalUnlock(hMem);
   GlobalFree(hMem);
   MessageBox(hWndMain, "Try selecting fewer files at a time",
            "Insufficient Memory", MB_OK |MB_SYSTEMMODAL|
            MB_ICONEXCLAMATION);
   return;
   }
szFileList = (char *)GlobalLock(hFileList);
if (!szFileList)
   {
   GlobalUnlock(hMem);
   GlobalFree(hMem);
   GlobalFree(hFileList);
   MessageBox(hWndMain, "Try selecting fewer files at a time",
            "Insufficient Memory", MB_OK |MB_SYSTEMMODAL|
            MB_ICONEXCLAMATION);
   return;
   }

GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);   

bRealTimeMsgUpdate = FALSE;
lpDCL->fQuiet = 2; // Kill all messages
lpDCL->ncflag = 1;
lpDCL->ntflag = 0;
lpDCL->nvflag = 0;
lpDCL->nfflag = 0;
lpDCL->nzflag = 0;
lpDCL->ndflag = 0;
lpDCL->noflag = 0;
lpDCL->naflag = TRUE;
lpDCL->nZIflag = 0;

/* Get list of selected items. Return value is number of selected items */
j = ListView_GetItemCount(hWnd);
for (i = 0; i < j; i++)
    {
    lvi.mask      = LVIF_STATE;
    lvi.stateMask = LVIS_SELECTED;
    lvi.iItem     = i;
    lvi.iSubItem  = 0;

    ListView_GetItem(hWnd, &lvi);

    if (lvi.state)
       {
       index = (char **)szFileList;
       cc = sizeof(char *) * 2; /* One more than the number of
                                   arguments is required.
                                 */
       sz = szFileList + cc;

       ListView_GetItemText(hWnd,
                            i,
                            0,
                            lpszT,
                            (PATH_MAX * 2));

       lstrcpy(sz, lpszT+1);

       for (p = sz; *p; p++)
            if (*p == '\\')
                *p = '/';

       index[0] = sz;
       sz += (cc + 1);
       uzpParameters.argv = (char **)szFileList;

       if (bSubstitute)
          /* This gets the archive name (sub-item 1) */
          ListView_GetItemText(hWnd,
                               i,
                               1,
                               lpumb->szFileName,
                               PATH_MAX);

       lpDCL->lpszZipFN = lpumb->szFileName;
       lpDCL->lpszExtractDir = NULL;

       uzpParameters.argc = 1;
       retcode = (*Unz_SingleEntryPoint)(uzpParameters.argc,
                                         uzpParameters.argv,
                                         uzpParameters.xfnc,
                                         uzpParameters.xfnv,
                                         uzpParameters.lpDCL,
                                         uzpParameters.lpUserFunctions);

       fSpool = FALSE;
       if ((retcode == PK_COOL) || (retcode == PK_WARN))
          switch (fDisplay) {
              case 0: // We can display it in WizEdit
                     (*StreamBuffer)(pszBuffer, uzpParameters.argv[0]);
                     fSwitchToEditor = TRUE;
                     break;
              case 1: // It's an archive, open it up
                     if (!IsValidArchive(szWindowsTempDir))
                        {
                        TCHAR sz[PATH_MAX * 2];
                        sprintf(sz,
                                "%s is not a valid archive, deleting file",
                                szWindowsTempDir);
                        MessageBox(hWndMain,
                                   sz,
                                   szAppName,
                                   MB_OK);
                        DeleteFile(szWindowsTempDir);
                        fDisplay = 0;
                        break;
                        }
                     lstrcpy(lpumb->szFileName, szWindowsTempDir);
                     CreateMDIChild(lpumb->szFileName);
                     InitializeArchiveStatusBar(GetActiveMDIChild());
                     fDisplay = 1;
                     UpdateListBox(); /* fill in list box */
                     ListViewSetSel(0, TRUE);
                     /* Tell wiz we were successful, so we can put up a warning
                        upon exiting.
                      */
                     if ((lstrlen(szWindowsTempDir) != 0) &&
                        (fDisplay == 1))
                        {
                        HKEY hkey;
                        TCHAR szRegTemp[PATH_MAX] = TEXT("Temp");
                        TCHAR szTemp[PATH_MAX];
                        itoa(NumberTempFiles+1, szTemp, 10);
                        lstrcat(szRegTemp, szTemp);
                        if (RegOpenKeyEx(HKEY_CURRENT_USER,
                                         szRegKey,
                                         0,
                                         KEY_ALL_ACCESS,
                                         &hkey)
                                         == ERROR_SUCCESS)
                           {
                           RegSetValueEx(hkey,
                                         szRegTemp,
                                         0,
                                         REG_SZ,
                                         (LPBYTE) szWindowsTempDir,
                                         // Need to include null terminator
                                         lstrlen(szWindowsTempDir)+1);
                           RegCloseKey(hkey);
                           NumberTempFiles++;
                           }
                        fExecutedFile = TRUE;
                     }
                     break;
              case 2: // It's a binary file, let Windows take care of it
                     {
                     HINSTANCE hShell;
                     TCHAR sz[PATH_MAX], szReturn[PATH_MAX];
                     // First let's check to see if it is an executable,
                     // and if so, is it an archive. If it is, then we
                     // can display it with wiz
                     if (lstrcmpi(&szWindowsTempDir[lstrlen(szWindowsTempDir) - 4],
                                  ".exe") == 0)
                        {
                        if (IsValidArchive(szWindowsTempDir))
                           {
                           lstrcpy(lpumb->szFileName, szWindowsTempDir);
                           CreateMDIChild(lpumb->szFileName);
                           InitializeArchiveStatusBar(GetActiveMDIChild());
                           fDisplay = 1;
                           UpdateListBox(); /* fill in list box */
                           ListViewSetSel(0, TRUE);
                           UpdateButtons(); /* update state of buttons */
                           break;
                           }
                        }
                     fDisplay = 2;
                     GetTempPath(PATH_MAX, sz);
                     hShell = FindExecutable(szWindowsTempDir,
                                             sz,
                                             szReturn);
                     if ((int)hShell < 33)
                        {
                        if ((int)hShell == 31)
                           sprintf(sz, "No executable associated with %s",
                                   szWindowsTempDir);
                        else
                           sprintf(sz, "Unable to execute %s",
                                   szWindowsTempDir);
                        MessageBox(hWndMain,
                                   sz,
                                   szAppName,
                                   MB_OK);
                        DeleteFile(szWindowsTempDir);
                        szWindowsTempDir[0] = '\0';
                        break;
                        }
                     hShell = ShellExecute(hWndMain,
                                           NULL,
                                           szWindowsTempDir,
                                           NULL,
                                           sz,
                                           SW_SHOWNORMAL);
                     if ((int)hShell < 33)
                        {
                        char sz1[RESOURCE_STR_LEN];
                        char sz[RESOURCE_STR_LEN];
                        switch ((int)hShell) {
                               case ERROR_FILE_NOT_FOUND:
                                    sprintf(sz, "%s\n", GetLoadString(IDS_FILE_NOT_FOUND));
                                    break;
                               case ERROR_PATH_NOT_FOUND:
                                    sprintf(sz, "%s\n", GetLoadString(IDS_PATH_NOT_FOUND));
                                    break;
                               case ERROR_BAD_FORMAT:
                                    sprintf(sz, "%s\n", GetLoadString(IDS_BAD_FORMAT));
                                    break;
                               case SE_ERR_SHARE:
                                    sprintf(sz, "%s\n", GetLoadString(IDS_SHARING_VIOLATION));
                                    break;
                               case 0:
                                    sprintf(sz, "%s\n", GetLoadString(IDS_NO_MEMORY));
                                    break;
                               default:
                                    sprintf(sz, "%s\n", GetLoadString(IDS_UNDETERMINED_ERROR));
                                    break;
                               }
                        lstrcpy(sz1, "Cannot execute file");
                        DeleteFile(szWindowsTempDir);
                        szWindowsTempDir[0] = '\0';
                        MessageBox(NULL, sz,
                                   sz1,
                                   MB_OK|MB_ICONSTOP);
                        }
                     }
          }
       /* Tell wiz we were successful, so we can put up a warning
          upon exiting.
        */
       if ((lstrlen(szWindowsTempDir) != 0) &&
          (fDisplay == 2))
          {
          HKEY hkey;
          TCHAR szRegTemp[PATH_MAX] = TEXT("Temp");
          TCHAR szTemp[PATH_MAX];
          itoa(NumberTempFiles+1, szTemp, 10);
          lstrcat(szRegTemp, szTemp);
          if (RegOpenKeyEx(HKEY_CURRENT_USER,
                           szRegKey,
                           0,
                           KEY_ALL_ACCESS,
                           &hkey)
                           == ERROR_SUCCESS)
              {
              RegSetValueEx(hkey,
                            szRegTemp,
                            0,
                            REG_SZ,
                            (LPBYTE) szWindowsTempDir,
                            // Need to include null terminator
                            lstrlen(szWindowsTempDir)+1);
              RegCloseKey(hkey);
              NumberTempFiles++;
              }
           fExecutedFile = TRUE;
           }
       memset(pszBuffer, '\0', dwEditBufSize);
       txtlen = (unsigned int)SendMessage(hWndMessage, WM_GETTEXTLENGTH, 0, 0);
       if (txtlen != 0)
          txtlen++;
       SendMessage(hWndMessage, WM_GETTEXT, txtlen, (LPARAM)pszBuffer);
       bRealTimeMsgUpdate = TRUE;
       }
    }
fDisplay = 0;
lpDCL->fQuiet = 0; // Kill all messages
if (hMem)
   {
   GlobalUnlock( hMem );
   GlobalFree( hMem );
   }
if (hFileList)
   {
   GlobalUnlock(hFileList);
   GlobalFree(hFileList);
   }
if (fSwitchToEditor)
   {
   // Switch to the editor tab
   TabCtrl_SetCurSel(hTabCtrl, 1);
   ShowWindow(hWndEditor, SW_SHOW);
   ShowWindow(hWndListParent, SW_HIDE);
   ShowWindow(hWndGrep, SW_HIDE);
   ShowWindow(hWndArchivesFound, SW_HIDE);
   ShowWindow(hWndMessage, SW_HIDE);
   }
}

void EmptyMessageQueue ()
{
MSG     msg;

while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
       DispatchMessage (&msg);
}
