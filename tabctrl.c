/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

//  MODULE:    tabctrl.c
//
//  PURPOSE:   Displays 
//
//  FUNCTIONS:
//    TabInit          - Calls initialization routines
//    CreateTabControl - Creates the tab control window.
//    InitTabControl   - Initializes the tab control with tabs
//
//  COMMENTS:
//
//

#include <windows.h>        // required for all Windows applications
#include <windowsx.h>
#include "wiz.h"            // prototypes specific to this application


BOOL    CreateTabControl    (HWND hwndParent);
void    InitializeTabControl(HWND hwndTabControl);

//
//  FUNCTION: TabInit (HWND)
//
//  PURPOSE: Initializes the tab control
//
//  PARAMETERS:
//    hwnd - The window handing the message.
//
//  RETURN VALUE:
//    BOOL - to indicate success/failure
//
//  COMMENTS:
//
//

BOOL TabInit(HWND hwnd)
{

// Call CreateTabControl() to create the tab control window.
if (!CreateTabControl(hwnd))
   {
   char sz1[RESOURCE_STR_LEN], sz2[RESOURCE_STR_LEN];
   lstrcpy(sz1, GetLoadString(IDS_ERROR_TAB_CTRL));
   lstrcpy(sz2, GetLoadString(IDS_ERROR));
   MessageBox(hwnd,
      sz1,
      sz2,
      MB_OK);
   return FALSE;
   }

// Call InitializeTabControl() to initialize the control and to create
// the property page dialogs.

InitializeTabControl(hTabCtrl);
return TRUE;
}

//
//  FUNCTION: CreateTabControl(HWND)
//
//  PURPOSE:  Create the Tab Control.
//
//  PARAMETERS:
//    hwnd     - Parent Window Handle.
//
//  RETURN VALUE:
//    returns TRUE if the tab control was created, else false.
//
//  COMMENTS:
//    This function calls CreateWindow to create the tab control. The hwnd
//    that is passed to this function is the parent window handle.
//

BOOL CreateTabControl(HWND hwndParent)
{
RECT dispRect;


GetClientRect(hwndParent, &dispRect);
hTabCtrl = CreateWindow(WC_TABCONTROL,
                        "",
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                        WS_CLIPSIBLINGS,// |
//                        TCS_TOOLTIPS,
                        dispRect.left  + 20,
                        dispRect.top   + 20,
                        dispRect.right - 50,
                        dispRect.bottom- 50,
                        hwndParent,
                        (HMENU)IDM_TABCONTROL,
                        hInst,
                        (LPVOID) NULL);

if (hTabCtrl)
   return TRUE;
else
   return FALSE;
}

//
//  FUNCTION: InitializeTabControl(HWND)
//
//  PURPOSE:  Initialize the Tab Control.
//
//  PARAMETERS:
//    hwnd     - TabControl  Window Handle.
//
//  RETURN VALUE:
//    None.
//
//  COMMENTS:
//    This function calls TabCtrl_InsertItem to insert items into
//    the tab control. The hwnd that is passed to this function is
//    is the  window handle of the tab control created earlier.
//    It also creates the child modeless dialogs that are displayed
//    as property sheets.
//

void InitializeTabControl(HWND hwndTabControl)
{
int         i;                    // variable used in the FOR loop.
//RECT        tabRect, tabItemRect; // Rectangles to position Page Dialogs.
char        szBuffer[64];
TC_ITEM     myTCItem;             // Array of TC_ITEM structures


//TabCtrl_GetItemRect (hwndTabControl, 0, &tabItemRect);
//GetClientRect (hwndTabControl, &tabRect);

// Initialize the Tab control with text from the
// resource file
// item 0 = Archives window
// item 1 = Editor window
// item 2 = Scan window
// item 3 = Search window
// item 4 = Messages window

for (i = 0; i < 5; i++)
    {

    // Load the string for the TABS from the RC file.
    LoadString(hInst, IDS_ARCHIVE+i, szBuffer, sizeof(szBuffer));
    myTCItem.mask      = (UINT)TCIF_TEXT | TCIF_IMAGE;
    myTCItem.pszText   = (LPSTR)szBuffer;
    myTCItem.cchTextMax= sizeof(szBuffer);
    myTCItem.iImage    = -1;

    TabCtrl_InsertItem (hwndTabControl,
                       (int) i,
                       (TC_ITEM FAR*)&myTCItem);
    }


TabCtrl_SetCurSel (hwndTabControl, 0);
//SetFocus (hwndTabControl);
}
