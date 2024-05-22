/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <windows.h>
#include <windowsx.h>
#include "wiz.h"

//  **TODO**  Add entries to the string table in wiz.rc for each menu
//            command.  MsgMenuSelect (below) loads these strings to display
//            information in the status bar.  MsgMenuSelect assumes that the
//            string ID is the same as the command ID and that a string
//            exists for every command.
//
// The following array contains resource string ID's for popup menus
// in the main application menu.  This array is used by MsgMenuSelect
// to display information in the status bar.
//
//  **TODO**  Add entries to this array for each popup menu in the same
//            positions as they appear in the main menu.  Remember to define
//            the ID's in resource.h and add the strings to wiz.rc.

UINT idPopup[] =
{
    IDS_MDISYSMENU,     // String ID for the MDI child sysmenu
    IDS_FILEMENU,
    IDS_EDITMENU,
    IDS_ARCHIVEMENU,
    IDS_SEARCHMENU,
    IDS_OPTIONSMENU,
    IDS_WINDOWMENU,
    IDS_HELPMENU,
};

//  FUNCTION: MsgMenuSelect(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Upadates menu selections on the status bar.
//
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Used)
//    uMessage  - Message number (Used)
//    wparam    - Extra data     (Used)
//    lparam    - Extra data     (Used)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//    This message is sent when the user selects menu items by
//    by pulling down  a popup menu move the mouse around to highlite
//    different menu items.
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgMenuSelect(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    static char szBuffer[128];
    char   szTitle[32];
    LPSTR  lpTitle;
    UINT   nStringID;
    UINT   fuFlags = GET_WM_MENUSELECT_FLAGS(wparam, lparam) & 0xffff;
    UINT   uCmd    = GET_WM_MENUSELECT_CMD(wparam, lparam);
    HMENU  hMenu   = GET_WM_MENUSELECT_HMENU(wparam, lparam);


    szBuffer[0] = 0;                            // First reset the buffer


    if (fuFlags == 0xffff && hMenu == NULL)     // Menu has been closed
       {
       if (GetArchiveWindow(NULL) == NULL)
          nStringID = IDS_DESCRIPTION;
       else
          {
          DisplaySelectionTotals();
          return 0;
          }
       }

    else if (fuFlags & MFT_SEPARATOR)           // Ignore separators
        nStringID = 0;

    else if (fuFlags & MF_POPUP)                // Popup menu
    {
        if (fuFlags & MF_SYSMENU)               // System menu
            nStringID = IDS_SYSMENU;

        else
        {
            // If there is a maximized MDI child window,
            // its system menu will be added to the main
            // window's menu bar.  Since the string ID for
            // the MDI child's sysmenu is already in the
            // idPopup array, all we need to do is patch up
            // the popup menu index (uCmd) when the child's
            // system menu is NOT present.

            HWND hwndChild = GetActiveMDIChild();

            if (!hwndChild || !IsZoomed(hwndChild)) // No MDI child sysmenu
                uCmd++;

            // Get string ID for popup menu from idPopup array.
            nStringID = ((uCmd < sizeof(idPopup)/sizeof(idPopup[0])) ?
                            idPopup[uCmd] : 0);
        }
    }  // for MF_POPUP

    else                                        // Must be a command item
    {
        // The Window menu has a dynamic part at the bottom
        // where the MDI Client window adds entries for each
        // child window that is open.  By getting the menu
        // item string we can customize the status bar string
        // with the name of the document.

        if (uCmd >= IDM_WINDOWCHILD && uCmd < IDS_HELPMENU)
        {
            LoadString(hInst, IDM_WINDOWCHILD, szBuffer, sizeof(szBuffer));

            GetMenuString(hMenu,
                          uCmd,
                          szTitle,
                          sizeof(szTitle),
                          MF_BYCOMMAND);

            lpTitle = szTitle;

            while (*lpTitle && *lpTitle != ' ')
                lpTitle++;

            lstrcat(szBuffer, lpTitle);

            nStringID = 0;
        }
        else
            nStringID = uCmd;               // String ID == Command ID
    }

    // Load the string if we have an ID
    if (0 != nStringID)
        LoadString(hInst, nStringID, szBuffer, sizeof(szBuffer));

    // Finally... send the string to the status bar
    UpdateStatusBar(szBuffer, 0, 0);

    return 0;
}

//
//  FUNCTION: MsgTimer(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Calls GetLocalTime() to set the time on the status bar
//
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//    Every time the window procedure receives a Timer message, it calls
//    GetLocalTime() to obtain the time and then formats the time into
//    a string. The time sting is then displayed on the status bar.
//
#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgTimer(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    TCHAR        szBuf[16];      // Temp buffer.
    TCHAR        szDay[5];
    TCHAR        szDate[20];
    SYSTEMTIME   sysTime;

    GetLocalTime(&sysTime);

    GetTimeFormat(LOCALE_USER_DEFAULT,
                 0,
                 &sysTime,
                 NULL,
                 szBuf,
                 (int) 16);
    UpdateStatusBar(szBuf, 2, 0);

    GetDateFormat(LOCALE_USER_DEFAULT,
                 DATE_SHORTDATE,
                 &sysTime,
                 NULL,
                 szBuf,
                 (int) 16);
    GetDateFormat(LOCALE_USER_DEFAULT,
                 0,
                 &sysTime,
                 "ddd",
                 szDay,
                 (int) 5);

    sprintf(szDate, "%s %s", szDay, szBuf);
    UpdateStatusBar(szDate, 1, 0);
    return 0;
}

//
//  FUNCTION: UpdateStatusBar(HWND)
//
//  PURPOSE:  Updates the statusbar control with appropriate text
//
//
//  PARAMETERS:
//
//  lpszStatusString - text to be displayed
//  partNumber       - which part of the status bar to display text in
//  displayFlags     - display flags
//
//
//  RETURN VALUE: NONE
//
//
//  COMMENTS:
//      None
//
//

void UpdateStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags)
{
    SendMessage(hStatusBar,
                SB_SETTEXT,
                partNumber | displayFlags,
                (LPARAM)lpszStatusString);
}

//
//  FUNCTION: UpdateArchiveStatusBar(HWND)
//
//  PURPOSE:  Updates the statusbar control with appropriate text
//
//
//  PARAMETERS:
//
//  lpszStatusString - text to be displayed
//  partNumber       - which part of the status bar to display text in
//  displayFlags     - display flags
//
//
//  RETURN VALUE: NONE
//
//
//  COMMENTS:
//      None
//
//

void UpdateArchiveStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags)
{
    if (!GetStatusWindow(NULL))
       return;
    SendMessage(GetStatusWindow(NULL),
                SB_SETTEXT,
                partNumber | displayFlags,
                (LPARAM)lpszStatusString);
}

//
//  FUNCTION: InitializeStatusBar(HWND)
//
//  PURPOSE:  Initialize statusbar control with time.
//
//
//  PARAMETERS:
//
//  hwndParent - Window handle of the status bar's parent
//
//
//  RETURN VALUE:  NONE
//
//
//  COMMENTS:
//
//   This function initializes the time section of
//   the statubar window.
//   The WPARAM of SB_SETTEXT is divided into 2 parameters. The LOWORD
//   determines which section/part the text goes into, and the HIWORD
//   tells how the bar is drawn (popin or popout).
//

void InitializeStatusBar(HWND hwndParent)
{
    const cSpaceInBetween = 8;
    int   ptArray[3];   // Array defining the number of parts/sections
    SIZE  size;         // the Status bar will display.
    RECT  rect;
    HDC   hDC;
   /*
    * Fill in the ptArray...
    */

    hDC = GetDC(hwndParent);
    GetClientRect(hwndParent, &rect);

    ptArray[2] = rect.right;

    if (GetTextExtentPoint(hDC, "00:00:00 PM", 12, &size))
        ptArray[1] = ptArray[2] - (size.cx) - cSpaceInBetween;
    else
        ptArray[1] = 0;

    if (GetTextExtentPoint(hDC, "Thur 00/00/00", 14, &size))
        ptArray[0] = ptArray[1] - (size.cx) - cSpaceInBetween;
    else
        ptArray[0] = 0;

    ReleaseDC(hwndParent, hDC);

    SendMessage(hStatusBar,
                SB_SETPARTS,
                sizeof(ptArray)/sizeof(ptArray[0]),
                (LPARAM)(LPINT)ptArray);

}

//
//  FUNCTION: InitializeArchiveStatusBar(HWND)
//
//  PURPOSE:  Initialize statusbar control
//
//
//  PARAMETERS:
//
//  hwndParent - Window handle of the status bar's parent
//
//
//  RETURN VALUE:  NONE
//
//
//  COMMENTS:
//
//   The WPARAM of SB_SETTEXT is divided into 2 parameters. The LOWORD
//   determines which section/part the text goes into, and the HIWORD
//   tells how the bar is drawn (popin or popout).
//

void InitializeArchiveStatusBar(HWND hwndParent)
{
    const cSpaceInBetween = 8;
    int   ptArray[8];   // Array defining the number of parts/sections
    SIZE  size;         // the Status bar will display.
    RECT  rect;
    HDC   hDC;
   /*
    * Fill in the ptArray...
    */

    hDC = GetDC(hwndParent);
    GetClientRect(hwndParent, &rect);

    if (GetTextExtentPoint32(hDC, "Files:", 7, &size))
        ptArray[0] = rect.left + (size.cx);
    else
        ptArray[0] = 0;

    if (GetTextExtentPoint32(hDC, "1234", 5, &size))
        ptArray[1] = ptArray[0] + (size.cx) + cSpaceInBetween;
    else
        ptArray[1] = 0;

    if (GetTextExtentPoint32(hDC, "Compressed:", 12, &size))
        ptArray[2] = ptArray[1] + (size.cx) + cSpaceInBetween;
    else
        ptArray[2] = 0;

    if (GetTextExtentPoint32(hDC, "12345678", 9, &size))
        ptArray[3] = ptArray[2] + (size.cx) + cSpaceInBetween;
    else
        ptArray[3] = 0;

    if (GetTextExtentPoint32(hDC, "Uncompressed:", 14, &size))
        ptArray[4] = ptArray[3] + (size.cx) + cSpaceInBetween;
    else
        ptArray[4] = 0;

    if (GetTextExtentPoint32(hDC, "12345678", 9, &size))
        ptArray[5] = ptArray[4] + (size.cx) + cSpaceInBetween;
    else
        ptArray[5] = 0;

    if (GetTextExtentPoint32(hDC, "Ratio:", 7, &size))
        ptArray[6] = ptArray[5] + (size.cx) + cSpaceInBetween;
    else
        ptArray[6] = 0;

    if (GetTextExtentPoint32(hDC, "32%", 4, &size))
        ptArray[7] = ptArray[6] + (size.cx) + cSpaceInBetween;
    else
        ptArray[7] = 0;

    ReleaseDC(hwndParent, hDC);

    SendMessage(GetStatusWindow(NULL),
                SB_SETPARTS,
                sizeof(ptArray)/sizeof(ptArray[0]),
                (LPARAM)(LPINT)ptArray);

    UpdateArchiveStatusBar("Files:", 0, SBT_POPOUT);
    UpdateArchiveStatusBar("Uncompressed:", 2, SBT_POPOUT);
    UpdateArchiveStatusBar("Compressed:", 4, SBT_POPOUT);
    UpdateArchiveStatusBar("Ratio:", 6, SBT_POPOUT);
}


