/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include "wiz.h"

TCHAR szLoadString[RESOURCE_STR_LEN];
LPSTR lpLoadString;

/* File and Path Name variables */
char szAppName[] = "wiz";       /* application title        */

/* Note that szHelpFileName will be expanded to a fully qualified path
   and file name when we go search for the help file.
 */
char szHelpFileName[PATH_MAX] = "WIZ.HLP";

//
//  FUNCTION: GetLoadString(UINT)
//
//  PURPOSE:  Load a resource string associated with the id passed in
//
//  PARAMETERS:
//
//    UINT      - Resource string id
//
//  RETURN VALUE:
//    LPSTR to resource string
//
//  COMMENTS:
//

LPSTR GetLoadString(UINT id)
{
lpLoadString = szLoadString;
LoadString(hInst, id, lpLoadString, sizeof(szLoadString));
return lpLoadString;
}

