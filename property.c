/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include "wiz.h"            // prototypes specific to this application

//
//  FUNCTION: DoPropertySheet(HWND)
//
//  PURPOSE: Fills out the property sheet data structures and displays
//           the dialog with the property sheets.
//
//  PARAMETERS:
//
//    hwndOwner  - Parent window handle of the property sheets
//
//  RETURN VALUE:
//
//    Returns value from PropertySheet()
//
//  COMMENTS:
//
//

int DoPropertySheet(HWND hwndOwner)
{
    PROPSHEETPAGE psp[3];
    PROPSHEETHEADER psh;

    //Fill out the PROPSHEETPAGE data structure for the Unzip Preferences
    //sheet

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USEICONID | PSP_USETITLE;
    psp[0].hInstance = hInst;

    // PROPSHEETPAGE uses anonymous unions, which are not supported in C.
    // The code below is conditionalized to show you how the usage differs
    // between C and C++.

#ifndef __BORLANDC__
    psp[0].pszTemplate = "UNZIP_PREFS";
    psp[0].pszIcon = "UNZIPPED";
#else
    psp[0].DUMMYUNIONNAME.pszTemplate = "UNZIP_PREFS";
    psp[0].DUMMYUNIONNAME2.pszIcon = "UNZIPPED";
#endif
    psp[0].pfnDlgProc = (DLGPROC) UnZipPreferencesProc;
    psp[0].pszTitle = "Unzip Options";
    psp[0].lParam = 0;

    //Fill out the PROPSHEETPAGE data structure for the Zip Preferences
    //sheet

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USEICONID | PSP_USETITLE;
    psp[1].hInstance = hInst;

    // PROPSHEETPAGE uses anonymous unions, which are not supported in C.

#ifndef __BORLANDC__
    psp[1].pszTemplate = "ZIP_PREFS";
    psp[1].pszIcon = "ZIPPED";
#else
    psp[1].DUMMYUNIONNAME.pszTemplate = "ZIP_PREFS";
    psp[1].DUMMYUNIONNAME2.pszIcon = "ZIPPED";
#endif
    psp[1].pfnDlgProc = (DLGPROC) ZipPreferencesProc;
    psp[1].pszTitle = "Zip Options";
    psp[1].lParam = 0;

    //Fill out the PROPSHEETPAGE data structure for the Sound Preferences
    //sheet

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = PSP_USEICONID | PSP_USETITLE;
    psp[2].hInstance = hInst;

    // PROPSHEETPAGE uses anonymous unions, which are not supported in C.

#ifndef __BORLANDC__
    psp[2].pszTemplate = "SOUND";
    psp[2].pszIcon = "SIGNAL";
#else
    psp[2].DUMMYUNIONNAME.pszTemplate = "SOUND";
    psp[2].DUMMYUNIONNAME2.pszIcon = "SIGNAL";
#endif
    psp[2].pfnDlgProc = (DLGPROC) SoundProc;
    psp[2].pszTitle = "Sound Options";
    psp[2].lParam = 0;

    //Fill out the PROPSHEETHEADER
    memset(&psh, 0, sizeof(PROPSHEETHEADER));
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_DEFAULT;
    psh.hwndParent = hwndOwner;
    psh.hInstance = hInst;

    // PROPSHEETPAGE uses anonymous unions, which are not supported in C.
#ifndef __BORLANDC__
    psh.pszIcon = NULL;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
#else
    psh.DUMMYUNIONNAME.pszIcon = NULL;
    psh.DUMMYUNIONNAME3.ppsp = (LPCPROPSHEETPAGE) &psp;
#endif
    psh.pszCaption = (LPSTR)"wiz Options";
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);

    //And finally display the dialog with the three property sheets.
    // Note: Under MSVC 5.0, if you optimize for speed, this call will
    // return immediately for some odd reason. This also seems to be
    // an intermittent problem with Borland 5.02, although the memset()
    // above seems to have resolved the problem for Borland, I haven't
    // checked MSVC
    return PropertySheet(&psh);
}



