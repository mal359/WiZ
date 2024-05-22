/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <stdio.h>
#include "wiz.h"
#include "cmds.h"

/****************************************************************************

    FUNCTION: WizInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

    COMMENTS:

        Sets up a structures to register the window class.  Structure includes
        such information as what function will process messages, what cursor
        and icon to use, etc.

****************************************************************************/
BOOL WizInit(HANDLE hInstance)
{
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = (LRESULT (WINAPI*)(HWND,unsigned ,
                             WPARAM,LPARAM)) WizMainWndProc;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(hInstance, "Wiz");
    wndclass.hCursor       = (HCURSOR)LoadCursor((HINSTANCE)NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)COLOR_BACKGROUND; /* set background color */
    wndclass.lpszMenuName  = (LPSTR) "wiz";
    wndclass.lpszClassName = (LPSTR) szAppName;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;

    if ( !RegisterClass(&wndclass) )
       {
        return FALSE;
       }
    if (!InitMDIChild(hInstance))
       return FALSE;

return TRUE;
}


