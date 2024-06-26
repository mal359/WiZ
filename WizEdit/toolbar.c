// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995 Microsoft Corporation.  All Rights Reserved.
//
// Modifications Copyright (C) 1997  Mike White
//
//  MODULE: toolbar.c
//
//  PURPOSE: Handles general routines for the Toolbar control
//
//  FUNCTIONS:
//    CreateTBar    - Creates the Toolbar control for the sample.
//    MsgNotify     - Handles the WM_NOTIFY message that gets sent to
//                    the parent window to get ToolTip Text.
//  COMMENTS:
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>

#define NOTREEVIEW
#define NOIMAGEAPIS

#include <commctrl.h>           // prototypes and defs for common controls
#include "..\wizedit\globals.h"            // prototypes specific to this application
#include "..\wizedit\resource.h"
#include "..\wizedit\toolbar.h"            // prototypes and #defines for toolbar.c

// Global Variables for toolbar control.

HWND hWndToolbar;

//  **TODO**  Change the following values to match your toolbar bitmap
//
// NUMIMAGES    = Number of images in toolbar.bmp.  Note that this is not
//                the same as the number of elements on the toolbar.
// IMAGEWIDTH   = Width of a single button image in toolbar.bmp
// IMAGEHEIGHT  = Height of a single button image in toolbar.bmp
// BUTTONWIDTH  = Width of a button on the toolbar (zero = default)
// BUTTONHEIGHT = Height of a button on the toolbar (zero = default)

#define NUMIMAGES       16
#define IMAGEWIDTH      18
#define IMAGEHEIGHT     17
#define BUTTONWIDTH     0
#define BUTTONHEIGHT    0

//  **TODO**  Add/remove entries in the following array to define the 
//            toolbar buttons (see documentation for TBBUTTON).

TBBUTTON tbButton[] =           // Array defining the toolbar buttons

   // the struct goes like this:
   // bitmap index, WM_COMMAND id, state, style, reserved bytes, app data,
   // string index
   // Note that the reserved bytes exist only for Win32 targets
{
    {0, IDM_FILENEW,          TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
    {1, IDM_FILEOPEN,         TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
    {2, IDM_FILESAVE,         TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},

    {0, 0,                    TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},

    {3, IDM_EDITCUT,          TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
    {4, IDM_EDITCOPY,         TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
    {5, IDM_EDITPASTE,        TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},

    {0, 0,                    TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},

    {6, IDM_FILEPRINT,        TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
    {7, IDM_FILEPRINTPREV,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},

    {0, 0,                    TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},

    {10,IDM_EDITBOLD,         TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
    {11,IDM_EDITITALIC,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
    {12,IDM_EDITUNDERLINE,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},

    {0, 0,                    TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},

    {13,IDM_EDITINCREASEFONT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
    {14,IDM_EDITDECREASEFONT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
    {15,IDM_EDITFONTDIALOG,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},

    {0, 0,                    TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},

    {8, IDM_ABOUT,            TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
};


//
//  FUNCTION: CreateTBar(HWND)
//
//  PURPOSE:  Calls CreateToolBarEx()
//
//
//  PARAMETERS:
//
//    hwnd - Window handle : Used for the hWndParent parameter of the control.
//
//  RETURN VALUE:
//
//    If toolbar control was created successfully Return TRUE,
//    else returns FALSE.
//
//  COMMENTS:
//
//

BOOL CreateTBar(HWND hwnd)
{
    hWndToolbar = CreateToolbarEx(hwnd,
                                  WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS,
                                  //|CCS_ADJUSTABLE,
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
//  FUNCTION: MsgNotify(HWND, UINT, WPARAM, LPARAM)
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
LRESULT MsgNotify(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
LPTOOLTIPTEXT lpToolTipText;
static char   szBuffer[64];

switch (((LPNMHDR)lparam)->code)
   {
   case TTN_NEEDTEXT:
        lpToolTipText = (LPTOOLTIPTEXT)lparam;
        if (lpToolTipText->hdr.code == TTN_NEEDTEXT)
           {
           LoadString(hInst,
                      lpToolTipText->hdr.idFrom,   // string ID == command ID
                      szBuffer,
                      sizeof(szBuffer));

           lpToolTipText->lpszText = szBuffer;
           return 0;
           }
   // We do this to customize the toolbar; otherwise
   // the customize dialog box goes away.

   case TBN_QUERYINSERT:
        return TRUE;

   case TBN_QUERYDELETE:
        return TRUE;

   default:
        return TRUE;
   }
}

//
//  FUNCTION: UpdateButton()
//
//  PURPOSE:  This routine will update the state of the given button.
//            
//
//  PARAMETERS:
//
//    iID    - ID of the button to update
//    iFlags - Enable/Disabled flag used to update the button
//
//  RETURN VALUE:
//    void
//
//  COMMENTS:
//

void UpdateButton(UINT iID, UINT iFlags)
{
    int iCurrentFlags;

    WinAssert(hWndToolbar);
    iCurrentFlags = (int)SendMessage(hWndToolbar,
                                     TB_GETSTATE,
                                     iID, 0L);

    if (iCurrentFlags & TBSTATE_PRESSED)
        iFlags |= TBSTATE_PRESSED;

    SendMessage(hWndToolbar,
                TB_SETSTATE, 
                iID, 
                MAKELPARAM(iFlags, 0));
    return;
}


//
//  FUNCTION: UpdateToolbar()
//
//  PURPOSE:  This function will enable/disable the toolbar buttons
//            to reflect their current state.  The toolbar buttons
//            reflect the state of the active MDI child window.
//            
//
//  PARAMETERS:
//
//    void
//
//  RETURN VALUE:
//
//    void
//
//  COMMENTS: Called from DllOnIdle so that the buttons are constantly
//            updated to reflect their current state.
//
//            In the true Microsoft Foundation Classes spirit, we implement
//            DllOnIdle() processing.  The DllOnIdle() routine is used to
//            update the toolbar so we can accurately reflect the
//            active/inactive state of the Save, Cut, Copy and Paste buttons.

void UpdateToolbar(VOID)
{
    UINT ichStart;
    UINT ichEnd;
    UINT tf = TBSTATE_INDETERMINATE;
    HWND hTemp;

    hTemp = GetEditWindow(NULL);

    // Enable/Disable Paste button
    if (cOpen && hTemp && OpenClipboard(hwndMDIClient))
    {
        if (IsClipboardFormatAvailable(CF_TEXT) ||
            IsClipboardFormatAvailable(CF_OEMTEXT))
        {
            tf = TBSTATE_ENABLED;
        }
    }
    CloseClipboard();
    UpdateButton(IDM_EDITPASTE, tf);

    // Enable/Disable Cut/Copy/Delete button
    if (cOpen && hTemp)
    {
        SendMessage(hTemp,
                    EM_GETSEL,
                    (WPARAM)&ichStart,
                    (LPARAM)&ichEnd);
        tf = (ichEnd != ichStart) ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE;
    }
    else
       tf = TBSTATE_INDETERMINATE;

    UpdateButton(IDM_EDITCUT, tf);
    UpdateButton(IDM_EDITCOPY, tf);
    UpdateButton(IDM_EDITCLEAR, tf);

    // Enable/Disable Save button
    if (cOpen && hTemp)
        tf = SendMessage(hTemp,
                         EM_GETMODIFY, 0, 0L) ? TBSTATE_ENABLED :TBSTATE_INDETERMINATE;
    else
       tf = TBSTATE_INDETERMINATE;

    UpdateButton(IDM_FILESAVE, tf);

    // Enable/Disable Save As button
    if (cOpen && hTemp)
        tf = SendMessage(hTemp,
                         WM_GETTEXTLENGTH, 0, 0L) ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE;
    else
       tf = TBSTATE_INDETERMINATE;

    UpdateButton(IDM_FILESAVEAS, tf);
    UpdateButton(IDM_FILEPRINT, tf);
    UpdateButton(IDM_FILEPRINTPREV, tf);
}

