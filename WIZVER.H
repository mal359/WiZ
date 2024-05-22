/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/
/*
 Wiz version header file
*/
#ifndef _WIZVER_H
#define _WIZVER_H

//#define BETA

#define APPLICATION "About Wiz\0"
#define WIZ_MAJORVER 5
#define WIZ_MINORVER 0
#define WIZ_PATCHLEVEL 3
#define WIZ_FILE_VER "5.03.00\0"

#define WIZ_VERSION_DATE "March 7, 2005"

#ifdef BETA
#  define WIZBETALEVEL " b Beta"
#else
#  define WIZBETALEVEL ""
#endif

#endif /* _WIZVER_H */

