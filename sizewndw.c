/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <stdio.h>
#include "wiz.h"

/* Call this when the window size changes or needs to change. */
void SizeWindow(void)
{
RECT rectT, rc, rd;
static BOOL doneOnce = FALSE;

WinAssert(hWndMain);
if (!doneOnce)
   {
   HWND hWndDesktop;
   int width, height;
   HMENU hSysMenu;
   int top;
   doneOnce = TRUE;
   if (FRestoreWindowPos(&MainRc))
      {
      MoveWindow(hWndMain,
                 MainRc.left,
                 MainRc.top,
                 MainRc.right,
                 MainRc.bottom,
                 FALSE);
      }
   else
      {
      hWndDesktop = GetDesktopWindow();
      GetClientRect(hWndDesktop, &rectT);
      /* Constant starting location */
      rectT.top =  2 * dyChar;
      /*
       * Adjust for 640 x 480 displays
       */
      width = GetSystemMetrics(SM_CXSCREEN);
      height = GetSystemMetrics(SM_CYSCREEN);
      if ((width == 640) || (height == 480))
         {
         /* If we are on a 640 x 480 display, take up two thirds of the
            real estate top to bottom, and three fourths of the screen
            display left to right.
          */
         rectT.bottom = ((rectT.bottom/3) * 2) +
            (2 * GetSystemMetrics(SM_CYDLGFRAME));
         rectT.right =  ((rectT.right/4) * 3) +
            (4 * GetSystemMetrics(SM_CXFRAME) +
            (2 * GetSystemMetrics(SM_CXVSCROLL)));
         }
      else
         {
         /* If we are not on a 640 x 480 display, take up one half of the
            real estate top to bottom, and one half of the screen display
            from left to right.
          */
         rectT.bottom = (rectT.bottom/2) +
            (2 * GetSystemMetrics(SM_CYDLGFRAME));
         rectT.right =  (rectT.right/2) +
            (4 * GetSystemMetrics(SM_CXFRAME) +
            (2 * GetSystemMetrics(SM_CXVSCROLL)));
         }
      rectT.left = 0;
      MoveWindow(hWndMain,
                 rectT.left, rectT.top,
                 rectT.right, rectT.bottom,
                 FALSE);
      }
   /* Limit user actions on window */
   hSysMenu = GetSystemMenu(hWndEditor, FALSE);
   DeleteMenu(hSysMenu, SC_CLOSE, MF_BYCOMMAND);
   DeleteMenu(hSysMenu, SC_TASKLIST, MF_BYCOMMAND);
   DeleteMenu(hSysMenu, SC_MOVE, MF_BYCOMMAND);
   DeleteMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
   DeleteMenu(hSysMenu, SC_RESTORE, MF_BYCOMMAND);
   DeleteMenu(hSysMenu, SC_MINIMIZE, MF_BYCOMMAND);
   DeleteMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
   /* walk thru menu and delete all separator bars */
   for (top = GetMenuItemCount(hSysMenu); top; top--)
       if (GetMenuState(hSysMenu, top-1, MF_BYPOSITION) & MF_SEPARATOR)
          DeleteMenu(hSysMenu, top-1, MF_BYPOSITION);
   /* For some reason I haven't figured out, it doesn't delete properly
      using IDM_EXIT and MF_BYCOMMAND.

      Delete the last menu separator bar and the exit menu item.
    */
   hSysMenu = GetMenu(hWndEditor);
   DeleteMenu(GetSubMenu(hSysMenu, 0), 10, MF_BYPOSITION);
   DeleteMenu(GetSubMenu(hSysMenu, 0), 9, MF_BYPOSITION);
   DrawMenuBar(hWndEditor);
   }

/*
 * Position the archive display listbox.
 */
WinAssert(hWndListParent);

GetClientRect(hWndMain, &rectT);
GetClientRect(hWndToolbar, &rc);
rectT.top = rc.bottom + GetSystemMetrics(SM_CYDLGFRAME);
TabCtrl_GetItemRect(hTabCtrl, 0, &rd);
MoveWindow(hTabCtrl,
   0, rectT.top,
   rectT.right,
   rd.bottom,
   TRUE);
// Get client area of tab control
GetClientRect(hWndMain, &rectT);
GetClientRect(hWndToolbar, &rc);
TabCtrl_GetItemRect(hTabCtrl, 0, &rd);
rectT.top = rc.bottom + GetSystemMetrics(SM_CYDLGFRAME)+rd.bottom;
rectT.bottom = rectT.bottom - rd.bottom - rc.bottom - GetSystemMetrics(SM_CYDLGFRAME);
GetClientRect(hStatusBar, &rc);
/* Fit display windows into client area */
/* Position the archive listview window */
MoveWindow(hWndListParent,
   0, rectT.top,
   rectT.right,
   rectT.bottom - rc.bottom,
   TRUE);
/* Position the "grep" listview */
MoveWindow(hWndGrep,
   0, rectT.top,
   rectT.right,
   rectT.bottom - rc.bottom,
   TRUE);
/* Position the "scan archive" listview */
MoveWindow(hWndArchivesFound,
   0, rectT.top,
   rectT.right,
   rectT.bottom - rc.bottom,
   TRUE);
/* Position the "wiz Message" edit window */
MoveWindow(hWndMessage,
   0, rectT.top,
   rectT.right,
   rectT.bottom - rc.bottom,
   TRUE);
/* Position the editor window */
GetWindowRect(hWndArchivesFound, &rectT);
MoveWindow(hWndEditor,
   rectT.left, rectT.top,
   rectT.right-rectT.left,
   rectT.bottom-rectT.top,
   TRUE);
/* Position status bar */
InitializeStatusBar(hWndMain);
ShowWindow(hTabCtrl, SW_SHOW);
}

