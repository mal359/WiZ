/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <windows.h>            
#include "wiz.h"

// Global Variable for the toolbar control.

HWND    hWndToolbar;

//  **TODO**  Change the following values to match your toolbar bitmap
//
// NUMIMAGES    = Number of images in toolbar.bmp.  Note that this is not
//                the same as the number of elements on the toolbar.
// IMAGEWIDTH   = Width of a single button image in toolbar.bmp
// IMAGEHEIGHT  = Height of a single button image in toolbar.bmp
// BUTTONWIDTH  = Width of a button on the toolbar (zero = default)
// BUTTONHEIGHT = Height of a button on the toolbar (zero = default)

#define NUMIMAGES       23
#define IMAGEWIDTH      17
#define IMAGEHEIGHT     18
#define BUTTONWIDTH     0
#define BUTTONHEIGHT    0

//  **TODO**  Add/remove entries in the following array to define the
//            toolbar buttons (see documentation for TBBUTTON).

TBBUTTON tbButton[] =  // array of buttons for the toolbar

   // the struct goes like this:
   // bitmap index, WM_COMMAND id, state, style, reserved bytes, app data,
   // string index
   // Note that the reserved bytes exist only for Win32 targets

{
     {19, IDM_ZIP_TARGET,         TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {22, IDM_ZIP_TARGET_CURRENT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
     {12, IDM_OPEN,               TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
     {18, IDM_CHDIR,              TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {14, IDM_SELECT_ALL,         TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {4,  IDM_DESELECT_ALL,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {15, IDM_SELECT_BY_PATTERN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
     {20, IDM_UPDATE_ZIP,         TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
     {21, IDM_ZIP_DELETE_ENTRIES, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
     {7,  IDM_EXTRACT,            TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {5,  IDM_DISPLAY,            TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {16, IDM_TEST,               TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {1,  IDM_SHOW_COMMENT,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {9,  IDM_GET_ZIPINFO,        TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
     {2,  IDM_COPY_ARCHIVE,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
#if 0
     {11, IDM_MOVE_ARCHIVE,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {13, IDM_RENAME_ARCHIVE,     TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
#endif
     {3,  IDM_SEARCH_ARCHIVE,     TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
     {17, IDM_DELETE_ARCHIVE,     TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
     {0,  IDM_CLEAR_STATUS,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
#if 0
     {10,  IDM_CANCEL_OPERATION,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
#endif
     {6,  IDM_EXIT,               TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
     {8,  IDM_HELP,               TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
     {0, 0,                       TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
};

//
//  FUNCTION: CreateButtonBar(HWND)
//
//  PURPOSE:  Calls CreateToolBarEx()
//
//
//  PARAMETERS:
//
//  hwnd - Window handle : Used for the hWndParent parameter of the control.
//
//  RETURN VALUE:
//
//  If toolbar control was created successfully Return TRUE,
//  else returns FALSE.
//
//  COMMENTS:
//
//

BOOL CreateButtonBar(HWND hwnd)
{
    hWndToolbar = CreateToolbarEx(hwnd,
                                  WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS|
                                  TBSTYLE_WRAPABLE,//|CCS_ADJUSTABLE,
                                  IDM_TOOLBAR,
                                  NUMIMAGES,
                                  hInst,
                                  IDB_BMP,
                                  tbButton,
                                  sizeof(tbButton)/sizeof(TBBUTTON),
                                  BUTTONWIDTH,
                                  BUTTONHEIGHT,
                                  IMAGEWIDTH,
                                  IMAGEHEIGHT,
                                  sizeof(TBBUTTON));

    return (hWndToolbar != NULL);
}

//
//  FUNCTION: ToolBarNotify(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  WM_NOTIFY is sent to the parent window to get the
//            tooltip text assoc'd with that toolbar button.
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - TOOLTIPTEXT FAR*
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//
//  COMMENTS:
//    This message fills in the lpszText field of the TOOLTIPTEXT
//    structure if code == TTN_NEEDTEXT
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT ToolBarNotify(HWND hwnd, WORD uMessage, WORD wparam, HWND lparam)
{
LPTOOLTIPTEXT lpToolTipText;

switch (((LPNMHDR)lparam)->code)
   {
   case TTN_NEEDTEXT:
        lpToolTipText = (LPTOOLTIPTEXT)lparam;
        if (lpToolTipText->hdr.code == TTN_NEEDTEXT)
           {
           lpToolTipText->lpszText = GetLoadString(lpToolTipText->hdr.idFrom);
           }
        return 0;

    // We do this to customize the toolbar; otherwise
    // the customize dialog box goes away.
  
    case TBN_QUERYINSERT:
        return TRUE;

    case TBN_QUERYDELETE:
        return TRUE;
/*
    case TB_CUSTOMIZE:
    // fill the listbox on the left side.
       {
       LPTBNOTIFY lpTbNotify = (LPTBNOTIFY)lparam;
       char  szBuffer [20];
       if (lpTbNotify->iItem < 12) // 20 == the total number of buttons
          {                        // tbButton and tbButtonNew
                                   // Since initially we displayed
                                   // 8 buttons
          // send back information about the rest of
          // 12 buttons that can be added the toolbar.

          lpTbNotify->tbButton = tbButtonNew[lpTbNotify->iItem];

          LoadString(hInst,
                     NEWBUTTONIDS + lpTbNotify->iItem, // string
                                                       //ID == command ID
                     szBuffer,
                     sizeof(szBuffer));

          lstrcpy (lpTbNotify->pszText, szBuffer);
          lpTbNotify->cchText = sizeof (szBuffer);
          return TRUE;
          }
       }
*/
    default:
        return TRUE;
   }
}

