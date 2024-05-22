/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <windows.h>
#include <stdio.h>
#include "wiz.h"

int fSoundOption; /* 0 = no sound
                     1 = beep
                     2 = Play sound during extraction
                     3 = Play sound after extraction
                   */
RECT MainRc;
TCHAR szDateString[PATH_MAX];
extern char szDfltWaveFile[];

TCHAR szRegKey[]          = TEXT("software\\Info-ZIP");
TCHAR szRegPreferences[]  = TEXT("Preferences");
TCHAR szRegTargetZip[]    = TEXT("Target Zip Directory");
TCHAR szRegTargetUnzip[]  = TEXT("Target Unzip Directory");
TCHAR szRegSourceUnzip[]  = TEXT("Source Unzip Directory");
TCHAR szRegRescanDir[]    = TEXT("Rescan Directory");
TCHAR szRegPlacement[]    = TEXT("Window Placement");
TCHAR szRegSound[]        = TEXT("WAV file");
TCHAR szRegNumTempFiles[] = TEXT("Temp Files");
TCHAR szRegDate[]         = TEXT("Misc Options");
TCHAR szRegTempDirPath[]  = TEXT("Temporary Zip Directory");
TCHAR szRescanDir[PATH_MAX];

extern TCHAR szTempDirPath[];

/* Forward references */
void ReadZipOptionsProfile(void);
void SaveWindowPos(HWND);

void SaveWindowPos(HWND hwnd)
{
   HKEY hkey;
   POINT pt;
   /* We go through this business rather than using GetWindowPosition and
      SetWindowPosition, because the SetWindowPosition takes too long
      if there are multiple child windows.
    */
   if (!hwnd)
      return;
   GetWindowRect(hwnd, &MainRc);
   pt.x = MainRc.left;
   pt.y = MainRc.top;
   ScreenToClient(GetDesktopWindow(), &pt);
   MainRc.left = pt.x;
   MainRc.top = pt.y;

   pt.x = MainRc.right;
   pt.y = MainRc.bottom;
   ScreenToClient(GetDesktopWindow(), &pt);
   MainRc.right = pt.x - MainRc.left;
   MainRc.bottom = pt.y - MainRc.top;

   if (RegCreateKeyEx(HKEY_CURRENT_USER, szRegKey, 0, szAppName, 0,
                      KEY_ALL_ACCESS, NULL, &hkey, NULL) == ERROR_SUCCESS)
      {
      RegSetValueEx(hkey, szRegPlacement, 0, REG_BINARY, (LPBYTE) &MainRc,
         sizeof(MainRc));
      RegCloseKey(hkey);
      }
}


BOOL FRestoreWindowPos(RECT *pRc)
{
   BOOL fReturn = FALSE;
   DWORD cb;
   HKEY hkey = 0;

    if(RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, &hkey)
      == ERROR_SUCCESS)
   {
      cb = sizeof(*pRc);
      if(RegQueryValueEx(hkey, szRegPlacement, 0, 0, (LPBYTE) pRc,
         &cb) == ERROR_SUCCESS)
      {
         fReturn = (cb == sizeof(*pRc));
      }
      RegCloseKey(hkey);
   }

   return fReturn;
}

void GetWizOptions()
{
ReadZipOptionsProfile();
}

void ReadZipOptionsProfile()
{
HKEY hkey;
DWORD cb;
BOOL fReturn = FALSE;
char szZipOptions[80];
/* NOTE: ZpOpt.fLevel defaults to 6 unless specified otherwise */
char szZipOptionsDefault[] = "6000000000000000000001000100100000101";
int i=0;
BOOL flag = FALSE;
DWORD length = PATH_MAX;
TCHAR sz[PATH_MAX + 22];

lstrcpy(szTempDirPath, "");
ZpOpt.szTempDir = szTempDirPath;

if (RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, &hkey)
      == ERROR_SUCCESS)
   {
   cb = lstrlen(szZipOptionsDefault)+1;
   if (RegQueryValueEx(hkey, szRegPreferences, 0, 0, (LPBYTE) szZipOptions,
      &cb) == ERROR_SUCCESS)
      {
      fReturn = (BOOL)(cb == (DWORD)lstrlen(szZipOptions) + 1);
      }
   cb = PATH_MAX;
   if (RegQueryValueEx(hkey, szRegTargetZip, 0, 0,
                      (LPBYTE) szTargetZipDir,
                      &cb) == ERROR_SUCCESS)
      {
      fReturn = fReturn && (BOOL)(cb == (DWORD)lstrlen(szTargetZipDir) + 1);
      }
   else
      fReturn = FALSE;
   cb = PATH_MAX;
   if (RegQueryValueEx(hkey, szRegTargetUnzip, 0, 0,
                      (LPBYTE) szUnzipToDirName,
                      &cb) == ERROR_SUCCESS)
      {
      fReturn = fReturn && (BOOL)(cb == (DWORD)lstrlen(szUnzipToDirName) + 1);
      }
   else
      fReturn = FALSE;
   if (!szUnzipToDirName[0])
      GetCurrentDirectory(length, (LPTSTR)szUnzipToDirName);
   cb = PATH_MAX;
   if (RegQueryValueEx(hkey, szRegRescanDir, 0, 0,
                      (LPBYTE) szRescanDir,
                      &cb) == ERROR_SUCCESS)
      {
      fReturn = fReturn && (BOOL)(cb == (DWORD)lstrlen(szRescanDir) + 1);
      }
   else
      fReturn = FALSE;
   cb = PATH_MAX;
   if (RegQueryValueEx(hkey, szRegTempDirPath, 0, 0,
                      (LPBYTE) szTempDirPath,
                      &cb) == ERROR_SUCCESS)
      {
      fReturn = fReturn && (BOOL)(cb == (DWORD)lstrlen(szTempDirPath) + 1);
      }
   else
      fReturn = FALSE;
   cb = PATH_MAX;
   if (RegQueryValueEx(hkey, szRegSourceUnzip, 0, 0,
                      (LPBYTE) lpumb->szUnzipFromDirName,
                       &cb) == ERROR_SUCCESS)
      {
      fReturn = fReturn && (BOOL)(cb == (DWORD)lstrlen(lpumb->szUnzipFromDirName) + 1);
      }
   else
      fReturn = FALSE;
   cb = PATH_MAX;
   if (RegQueryValueEx(hkey, szRegDate, 0, 0,
                      (LPBYTE) szDateString,
                       &cb) == ERROR_SUCCESS)
      {
      fReturn = fReturn && (BOOL)(cb == (DWORD)lstrlen(szDateString) + 1);
      }
   else
      fReturn = FALSE;
   cb = PATH_MAX;
   if (RegQueryValueEx(hkey, szRegSound, 0, 0,
                      (LPBYTE) lpumb->szSoundName,
                       &cb) == ERROR_SUCCESS)
      {
      if (lstrlen(lpumb->szSoundName) == 0)
         fReturn = FALSE;
      else
         fReturn = fReturn && (BOOL)(cb == (DWORD)lstrlen(lpumb->szSoundName) + 1);
      }
   else
      fReturn = FALSE;
   RegCloseKey(hkey);
   }
if (fReturn == FALSE)
   if (RegCreateKeyEx(HKEY_CURRENT_USER, szRegKey, 0, szAppName, 0,
      KEY_ALL_ACCESS, NULL, &hkey, NULL) == ERROR_SUCCESS)
      {
      SYSTEMTIME SysTime;
      unsigned long iBadRegNum = 75382468L;

      GetLocalTime(&SysTime);
      sprintf(szDateString,"%02x%03x%02x%07x",
              SysTime.wMonth,
              SysTime.wYear,
              SysTime.wDay,
              iBadRegNum);
      GetCurrentDirectory(length, (LPTSTR)szUnzipToDirName);

      RegSetValueEx(hkey, szRegDate, 0, REG_SZ,
         (LPBYTE) szDateString,
         lstrlen(szDateString)+1); // Need to include null terminator
      RegSetValueEx(hkey, szRegPreferences, 0, REG_SZ,
         (LPBYTE) szZipOptionsDefault,
         lstrlen(szZipOptionsDefault)+1); // Need to include null terminator
      RegSetValueEx(hkey, szRegTargetZip, 0, REG_SZ,
         (LPBYTE) "",
         1); // Need to include null terminator
      RegSetValueEx(hkey, szRegTargetUnzip, 0, REG_SZ,
         (LPBYTE) szUnzipToDirName,
         1); // Need to include null terminator
      RegSetValueEx(hkey, szRegSourceUnzip, 0, REG_SZ,
         (LPBYTE) "",
         1); // Need to include null terminator
      RegSetValueEx(hkey, szRegRescanDir, 0, REG_SZ,
         (LPBYTE) "",
         1); // Need to include null terminator
      RegSetValueEx(hkey, szRegTempDirPath, 0, REG_SZ,
         (LPBYTE) "",
         1); // Need to include null terminator
      RegSetValueEx(hkey, szRegSound, 0, REG_SZ,
         (LPBYTE) szDfltWaveFile,
         lstrlen(szDfltWaveFile)+1); // Need to include null terminator
      lstrcpy(lpumb->szSoundName, szDfltWaveFile);
      RegCloseKey(hkey);
      }

if (lstrlen(szZipOptions) != lstrlen(szZipOptionsDefault))
   {
   lstrcpy(szZipOptions, szZipOptionsDefault);
   flag = TRUE;
   }

ZpOpt.fLevel               = szZipOptions[i++];
ZpOpt.fSuffix              = szZipOptions[i++]-'0';
ZpOpt.fEncrypt             = szZipOptions[i++]-'0';
ZpOpt.fSystem              = szZipOptions[i++]-'0';
ZpOpt.fVolume              = szZipOptions[i++]-'0';
ZpOpt.fExtra               = szZipOptions[i++]-'0';
ZpOpt.fNoDirEntries        = szZipOptions[i++]-'0';
ZpOpt.fVerbose             = szZipOptions[i++]-'0';
ZpOpt.fQuiet               = szZipOptions[i++]-'0';
ZpOpt.fCRLF_LF             = szZipOptions[i++]-'0';
ZpOpt.fLF_CRLF             = szZipOptions[i++]-'0';
ZpOpt.fJunkDir             = szZipOptions[i++]-'0';
ZpOpt.fRecurse             = szZipOptions[i++]-'0';
ZpOpt.fGrow                = szZipOptions[i++]-'0';
ZpOpt.fForce               = szZipOptions[i++]-'0';
ZpOpt.fMove                = szZipOptions[i++]-'0';
ZpOpt.fUpdate              = szZipOptions[i++]-'0';
ZpOpt.fFreshen             = szZipOptions[i++]-'0';
ZpOpt.fLatestTime          = szZipOptions[i++]-'0';
ZpOpt.fComment             = szZipOptions[i++]-'0';
lpDCL->noflag              = szZipOptions[i++]-'0';
lpDCL->PromptToOverwrite   = szZipOptions[i++]-'0';
lpDCL->ExtractOnlyNewer    = szZipOptions[i++]-'0';
lpDCL->SpaceToUnderscore   = szZipOptions[i++]-'0';
uf.fTranslate              = szZipOptions[i++]-'0';
uf.fRecreateDirs           = szZipOptions[i++]-'0';
uf.fRecurseOnSearch        = szZipOptions[i++]-'0';
fMakeSFX                   = szZipOptions[i++]-'0';
fLB_Selection              = szZipOptions[i++]-'0';
lpDCL->fPrivilege          = szZipOptions[i++]-'0';
ZpOpt.fPrivilege           = szZipOptions[i++]-'0';
fSoundOption               = szZipOptions[i++]-'0';
fNotifyClearing            = szZipOptions[i++]-'0';
fExternalRTF               = szZipOptions[i++]-'0';
fMakeDosSFX                = szZipOptions[i++]-'0';
uf.fRescan                 = szZipOptions[i++]-'0';
fUseOldDirSelect           = szZipOptions[i]  -'0';

if (!dwPlatformId)
   {
   lpDCL->fPrivilege = FALSE;
   ZpOpt.fPrivilege = FALSE;
   }
/* Verify that all entries are valid */
if ((ZpOpt.fLevel < '0') || (ZpOpt.fLevel > '9'))
   {
   ZpOpt.fLevel = '6'; /* Set compression level to default */
   flag = TRUE;
   }
if ((ZpOpt.fPrivilege < 0) || (ZpOpt.fPrivilege > 3))
   {
   ZpOpt.fPrivilege = 0; /* Turn off privilege */
   flag = TRUE;
   }
if ((lpDCL->fPrivilege < 0) || (lpDCL->fPrivilege > 3))
   {
   lpDCL->fPrivilege = 0; /* Turn off privilege */
   flag = TRUE;
   }
if ((ZpOpt.fSuffix != 0) && (ZpOpt.fSuffix != 1))
   {
   ZpOpt.fSuffix = 0;
   flag = TRUE;
   }
if ((ZpOpt.fEncrypt!= 0) && (ZpOpt.fEncrypt!= 1))
   {
   ZpOpt.fEncrypt= 0;
   flag = TRUE;
   }
if ((ZpOpt.fSystem!= 0) && (ZpOpt.fSystem!= 1))
   {
   ZpOpt.fSystem= 0;
   flag = TRUE;
   }
if ((ZpOpt.fVolume!= 0) && (ZpOpt.fVolume!= 1))
   {
   ZpOpt.fVolume= 0;
   flag = TRUE;
   }
if ((ZpOpt.fExtra!= 0) && (ZpOpt.fExtra!= 1))
   {
   ZpOpt.fExtra= 0;
   flag = TRUE;
   }
if ((ZpOpt.fNoDirEntries!= 0) && (ZpOpt.fNoDirEntries!= 1))
   {
   ZpOpt.fNoDirEntries= 0;
   flag = TRUE;
   }
if ((ZpOpt.fVerbose!= 0) && (ZpOpt.fVerbose!= 1))
   {
   ZpOpt.fVerbose= 0;
   flag = TRUE;
   }
if ((ZpOpt.fQuiet!= 0) && (ZpOpt.fQuiet!= 1))
   {
   ZpOpt.fQuiet= 0;
   flag = TRUE;
   }
if ((ZpOpt.fCRLF_LF!= 0) && (ZpOpt.fCRLF_LF!= 1))
   {
   ZpOpt.fCRLF_LF= 0;
   flag = TRUE;
   }
if ((ZpOpt.fLF_CRLF!= 0) && (ZpOpt.fLF_CRLF!= 1))
   {
   ZpOpt.fLF_CRLF= 0;
   flag = TRUE;
   }
if ((ZpOpt.fJunkDir!= 0) && (ZpOpt.fJunkDir!= 1))
   {
   ZpOpt.fJunkDir= 0;
   flag = TRUE;
   }
if ((ZpOpt.fRecurse!= 0) && (ZpOpt.fRecurse!= 1))
   {
   ZpOpt.fRecurse= 0;
   flag = TRUE;
   }
if ((ZpOpt.fGrow!= 0) && (ZpOpt.fGrow!= 1))
   {
   ZpOpt.fGrow= 0;
   flag = TRUE;
   }
if ((ZpOpt.fForce!= 0) && (ZpOpt.fForce!= 1))
   {
   ZpOpt.fForce= 0;
   flag = TRUE;
   }
if ((ZpOpt.fMove!= 0) && (ZpOpt.fMove!= 1))
   {
   ZpOpt.fMove= 0;
   flag = TRUE;
   }
if ((ZpOpt.fUpdate!= 0) && (ZpOpt.fUpdate!= 1))
   {
   ZpOpt.fUpdate= 0;
   flag = TRUE;
   }
if ((ZpOpt.fFreshen!= 0) && (ZpOpt.fFreshen!= 1))
   {
   ZpOpt.fFreshen= 0;
   flag = TRUE;
   }
ZpOpt.fJunkSFX = 0;
if ((ZpOpt.fLatestTime != 0) && (ZpOpt.fLatestTime != 1))
   {
   ZpOpt.fLatestTime = 0;
   flag = TRUE;
   }
if ((ZpOpt.fComment != 0) && (ZpOpt.fComment != 1))
   {
   ZpOpt.fComment = 0;
   flag = TRUE;
   }
if ((lpDCL->noflag != 0) && (lpDCL->noflag != 1))
   {
   lpDCL->noflag = 0;
   flag = TRUE;
   }
if ((lpDCL->PromptToOverwrite != 0) && (lpDCL->PromptToOverwrite != 1))
   {
   lpDCL->PromptToOverwrite = 0;
   flag = TRUE;
   }
if ((lpDCL->ExtractOnlyNewer != 0) && (lpDCL->ExtractOnlyNewer != 1))
   {
   lpDCL->ExtractOnlyNewer = 0;
   flag = TRUE;
   }
if ((lpDCL->SpaceToUnderscore != 0) && (lpDCL->SpaceToUnderscore != 1))
   {
   lpDCL->SpaceToUnderscore = 0;
   flag = TRUE;
   }
if ((uf.fTranslate != 0) && (uf.fTranslate != 1))
   {
   uf.fTranslate = 0;
   flag = TRUE;
   }
if ((uf.fRecreateDirs != 0) && (uf.fRecreateDirs != 1))
   {
   uf.fRecreateDirs = 0;
   flag = TRUE;
   }
if ((uf.fRecurseOnSearch != 0) && (uf.fRecurseOnSearch != 1))
   {
   uf.fRecurseOnSearch = 0;
   flag = TRUE;
   }
if ((fMakeSFX != 0) && (fMakeSFX != 1))
   {
   fMakeSFX = 0;
   flag = TRUE;
   }
if ((fSoundOption < 0) || (fSoundOption > 3))
   {
   fSoundOption = 0;
   flag = TRUE;
   }
if ((fNotifyClearing != 0) && (fNotifyClearing != 1))
   {
   fNotifyClearing = 0;
   flag = TRUE;
   }
if ((fExternalRTF != 0) && (fExternalRTF != 1))
   {
   fExternalRTF = 0;
   flag = TRUE;
   }
if ((fMakeDosSFX != 0) && (fMakeDosSFX != 1))
   {
   fMakeDosSFX = 1;
   flag = TRUE;
   }
if ((fUseOldDirSelect != 0) && (fUseOldDirSelect != 1))
   {
   fUseOldDirSelect = 1;
   flag = TRUE;
   }
if ((uf.fRescan != 0) && (uf.fRescan != 1))
   {
   uf.fRescan = 0;
   flag = TRUE;
   }
if ((fLB_Selection < 0) || (fLB_Selection > 2))
   {
   fLB_Selection = 1;
   wLBSelection = IDM_DISPLAY; /* default listbox selection action */
   flag = TRUE;
   }
else
   {
   switch (fLB_Selection)
      {
      case 0:
              wLBSelection = IDM_EXTRACT;
              break;
      case 1:
              wLBSelection = IDM_DISPLAY;
              break;
      case 2:
              wLBSelection = IDM_TEST;
              break;
      }
   }

   /* We never want these options to be saved, or restored */
ZpOpt.fExcludeDate= FALSE;     /* Exclude Date option */
ZpOpt.fIncludeDate= FALSE;     /* Include Date option */
ZpOpt.fOffsets = FALSE;        /* Adjust offsets option */
ZpOpt.fRepair = FALSE;         /* Fix archive options */

/* These always need to start out as FALSE, and are retained
   while wiz is loaded, but not saved off.
 */
uf.fCaseSensitive = uf.fIgnoreBinary = uf.fWholeWords = FALSE;

/* All entries have now been validated. */
CheckMenuItem(hMenu, IDM_RECR_DIR_STRUCT, MF_BYCOMMAND |
             (uf.fRecreateDirs ? MF_CHECKED : MF_UNCHECKED));

EnableMenuItem(hMenu, IDM_CHDIR, MF_BYCOMMAND | MF_ENABLED);
CheckMenuItem(hMenu,IDM_NOTIFY_CLEARING,MF_BYCOMMAND|
              (WORD)(fNotifyClearing ? MF_CHECKED: MF_UNCHECKED));
CheckMenuItem(hMenu,IDM_EXTERNAL_RTF,MF_BYCOMMAND|
              (WORD)(fExternalRTF ? MF_CHECKED: MF_UNCHECKED));


if (flag) /* Something was wrong with the options string, re-write it */
   WriteZipOptionsProfile();
lstrcpy(sz, "Wiz extracts to: ");
if (szUnzipToDirName[0])
   {
   lstrcat(sz, szUnzipToDirName);
   }
else
   {
   lstrcat(sz, "C:\\");
   }
SendMessage(hWndMain, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)sz);
}

/* WriteZipOptionsProfile simply writes the wiz options out to one string,
   rather than having an entire series of profile strings in the registry.
*/
void WriteZipOptionsProfile()
{
HKEY hkey;
char szZipOptions[80];
sprintf(szZipOptions,
   "%c%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u",
   ZpOpt.fLevel,
   ZpOpt.fSuffix,
   ZpOpt.fEncrypt,
   ZpOpt.fSystem,
   ZpOpt.fVolume,
   ZpOpt.fExtra,
   ZpOpt.fNoDirEntries,
   ZpOpt.fVerbose,
   ZpOpt.fQuiet,
   ZpOpt.fCRLF_LF,
   ZpOpt.fLF_CRLF,
   ZpOpt.fJunkDir,
   ZpOpt.fRecurse,
   ZpOpt.fGrow,
   ZpOpt.fForce,
   ZpOpt.fMove,
   ZpOpt.fUpdate,
   ZpOpt.fFreshen,
   ZpOpt.fLatestTime,
   ZpOpt.fComment,
   lpDCL->noflag,
   lpDCL->PromptToOverwrite,
   lpDCL->ExtractOnlyNewer,
   lpDCL->SpaceToUnderscore,
   uf.fTranslate,
   uf.fRecreateDirs,
   uf.fRecurseOnSearch,
   fMakeSFX,
   fLB_Selection,
   lpDCL->fPrivilege,
   ZpOpt.fPrivilege,
   fSoundOption,
   fNotifyClearing,
   fExternalRTF,
   fMakeDosSFX,
   uf.fRescan,
   fUseOldDirSelect);

/* Save current position and size */
SaveWindowPos(hWndMain);

if (RegCreateKeyEx(HKEY_CURRENT_USER, szRegKey, 0, szAppName, 0,
   KEY_ALL_ACCESS, NULL, &hkey, NULL) == ERROR_SUCCESS)
   {
   RegSetValueEx(hkey, szRegPreferences, 0, REG_SZ,
         (LPBYTE) szZipOptions,
         lstrlen(szZipOptions)+1); // Need to include null terminator
   if (szTargetZipDir[0])
      RegSetValueEx(hkey, szRegTargetZip, 0, REG_SZ,
         (LPBYTE) szTargetZipDir,
         lstrlen(szTargetZipDir)+1); // Need to include null terminator
   else
      RegSetValueEx(hkey, szRegTargetZip, 0, REG_SZ,
         (LPBYTE) "",
         1); // Need to include null terminator
   if (uf.fRescan)
      RegSetValueEx(hkey, szRegRescanDir, 0, REG_SZ,
         (LPBYTE) szRescanDir,
         lstrlen(szRescanDir)+1); // Need to include null terminator
   else
      RegSetValueEx(hkey, szRegRescanDir, 0, REG_SZ,
         (LPBYTE) "",
         1); // Need to include null terminator
   RegSetValueEx(hkey, szRegTempDirPath, 0, REG_SZ,
         (LPBYTE) szTempDirPath,
         lstrlen(szTempDirPath)+1); // Need to include null terminator
   RegSetValueEx(hkey, szRegTargetUnzip, 0, REG_SZ,
         (LPBYTE) szUnzipToDirName,
         lstrlen(szUnzipToDirName)+1); // Need to include null terminator
   RegSetValueEx(hkey, szRegSourceUnzip, 0, REG_SZ,
         (LPBYTE) lpumb->szUnzipFromDirName,
         lstrlen(lpumb->szUnzipFromDirName)+1); // Need to include null terminator
   RegSetValueEx(hkey, szRegSound, 0, REG_SZ,
         (LPBYTE) lpumb->szSoundName,
         lstrlen(lpumb->szSoundName)+1); // Need to include null terminator
   RegCloseKey(hkey);
   }
}


