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

#include <windows.h>    /* required for all Windows applications */
#include <stdio.h>
#include "wiz.h"
#include "unzip\unzip.h"
#include "wizver.h"
#include "zip\api.h"

#define UNZIP_NAME "UnZip32"
#define ZIP_NAME "Zip32"


/****************************************************************************

    FUNCTION: About(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/

#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI
AboutProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
UzpVer2 VersionUzp;
ZpVer   VersionZp;
TCHAR string[80];
#ifndef USE_UNZIP_LIB
typedef void (WINAPI * _get_unzip_ver) (UzpVer2 far *);
_get_unzip_ver GetUnzipVersion;
#endif
#ifndef USE_ZIP_LIB
typedef void (WINAPI * _get_zip_ver) (ZpVer far *);
_get_zip_ver GetZipVersion;
#endif

if ((wMessage == WM_CLOSE) ||
    (wMessage == WM_COMMAND && LOWORD(wParam) == IDOK))
        EndDialog(hwndDlg, TRUE);

if (wMessage == WM_INITDIALOG)
   {
#ifndef BETA
   sprintf(string, "%s", APPLICATION);
   SetWindowText(hwndDlg, string);
#else
   SetWindowText(hwndDlg, "BETA - Do Not Distribute");
#endif

   sprintf(string, "Info-ZIP's Wiz version %d.%d%d%s %s",
      WIZ_MAJORVER, WIZ_MINORVER, WIZ_PATCHLEVEL,
      WIZBETALEVEL, WIZ_VERSION_DATE);
   SetDlgItemText(hwndDlg, IDM_ABOUT_VERSION_INFO, string);

#ifndef USE_ZIP_LIB
   (_get_zip_ver) GetZipVersion =
      (_get_zip_ver)GetProcAddress(hZipDll, "ZpVersion");
   if (!GetZipVersion)
      {
      lstrcpy(string, "Cannot get ZpVersion address");
      }
   else
      {
      (*GetZipVersion)(&VersionZp);
      sprintf(string, "%s DLL Version %d.%d%d %s",
         ZIP_NAME,
         VersionZp.windll.major,
         VersionZp.windll.minor,
         VersionZp.windll.patchlevel,
         VersionZp.betalevel);
      }
#else
      ZpVersion(&VersionZp);
      sprintf(string, "%s Library Version %d.%d%d %s",
         ZIP_NAME,
         VersionZp.windll.major,
         VersionZp.windll.minor,
         VersionZp.windll.patchlevel,
         VersionZp.betalevel);
#endif
   SetDlgItemText(hwndDlg, IDM_ABOUT_ZIP_INFO, string);

#ifndef USE_UNZIP_LIB
   (_get_unzip_ver) GetUnzipVersion =
      (_get_unzip_ver)GetProcAddress(hUnzipDll, "UzpVersion2");
   if (!GetUnzipVersion)        
      {
      lstrcpy(string, "Cannot get UzpVersion address");
      }
   else
      {
      (*GetUnzipVersion)(&VersionUzp);
      sprintf(string, "%s DLL Version %d.%d%d %s",
         UNZIP_NAME,
         VersionUzp.windll.major,
         VersionUzp.windll.minor,
         VersionUzp.windll.patchlevel,
         VersionUzp.betalevel);
      }
#else
      UzpVersion2(&VersionUzp);
      sprintf(string, "%s Library Version %d.%d%d %s",
         UNZIP_NAME,
         VersionUzp.windll.major,
         VersionUzp.windll.minor,
         VersionUzp.windll.patchlevel,
         VersionUzp.betalevel);
#endif
   SetDlgItemText(hwndDlg, IDM_ABOUT_UNZIP_INFO, string);

   CenterDialog(NULL, hwndDlg);
   }
return ((wMessage == WM_CLOSE) || (wMessage == WM_INITDIALOG) || (wMessage == WM_COMMAND))
            ? TRUE : FALSE;
}

