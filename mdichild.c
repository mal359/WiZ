/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

//
//  MODULE:   mdichild.c
//
//  PURPOSE:
//    To implement the basic mdi child commands.
//
//  FUNCTIONS:
//    InitMDIChild     - To register the MDI child window class.
//    MDIChildWndProc  - Processes messages for MDI child windows.
//    MsgMCCommand     - Handle the WM_COMMAND messages for mdi children.
//    MsgMCMenuSelect  - Handle the WM_MENUSELECT message for the mdi child
//    MsgMCCreate      - Handle the WM_CREATE message for the MDI child
//    MsgMCSize        - Handle the WM_SIZE message for the MDI child
//    MsgMCSetFocus    - Handle the WM_SETFOCUS message for the MDI child
//    MsgMCDestroy     - Handle the WM_DESTROY message for the MDI child
//    CmdArchive       - Handle the notification messages for the listview
//                       control contained within the MDI child
//
//  COMMENTS:
#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <richedit.h>
#include "wiz.h"
#include "mdichild.h"

UINT cOpen = 0;
TCHAR szTitle[PATH_MAX];

LRESULT MsgMDNotify(HWND, UINT, WPARAM, LPARAM);

static MSD rgmsd[] =
{
    {WM_COMMAND,     MsgMCCommand },
    {WM_CREATE,      MsgMCCreate  },
    {WM_DESTROY,     MsgMCDestroy },
    {WM_MENUSELECT,  MsgMenuSelect},
    {WM_NOTIFY,      MsgMDNotify  },
    {WM_SETFOCUS,    MsgMCSetFocus},
    {WM_SIZE,        MsgMCSize    },
};

MSDI msdiChild =
{
    sizeof(rgmsd) / sizeof(MSD),
    rgmsd,
    edwpMDIChild
};

static CMD rgcmd[] =
{
    {IDC_ARCHIVE,           CmdArchive},
};

CMDI cmdiChild =
{
    sizeof(rgcmd) / sizeof(CMD),
    rgcmd,
    edwpMDIChild
};

//Module specific globals

static char szChildName[12];


//
//  FUNCTION: InitMDIChild(HINSTANCE)
//
//  PURPOSE: To register the MDI child window class.
//
//  PARAMETERS:
//    hinst - The instance of the application used to register the class.
//
//  RETURN VALUE:
//    TRUE - Succeeded in registering the class.
//    FALSE - Failed to register the class.
//
//  COMMENTS:
//
//

BOOL InitMDIChild(HINSTANCE hinst)
{
    WNDCLASSEX wc;

    lstrcpy(szChildName, "MdiChild");
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC)MDIChildWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hinst;
    wc.hIcon         = LoadIcon(hinst, MAKEINTRESOURCE(IDI_CHILDICON));
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = szChildName;
    wc.hIconSm       = LoadImage(hinst,      // Load small icon image
                                MAKEINTRESOURCE(IDI_CHILDICON),
                                IMAGE_ICON,
                                16, 16,
                                0);

    // Register the window class and return FALSE if unsuccesful.

    if (!RegisterClassEx(&wc))
    {
        //Assume we are running on NT where RegisterClassEx() is
        //not implemented, so let's try calling RegisterClass().

        if (!RegisterClass((LPWNDCLASS)&wc.style))
           return FALSE;
    }

    return TRUE;
}


//
//  FUNCTION: MsgMDNotify(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles the WM_NOTIFY message
//
//  PARAMETERS:
//    hwnd - The window handing the message.
//    uMessage - WM_NOTIFY
//    wparam - ID of control sending notification
//    lparam - LPNOTIFY pointer to the NOTIFY structure
//
//  RETURN VALUE:
//    Always returns 0 - message handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgMDNotify(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
NM_LISTVIEW *pNm = (NM_LISTVIEW *)lparam;
DWORD          dwpos;
WinAssert(hwnd);
/* Are we dealing with a message from the list box? */
if (pNm->hdr.hwndFrom == GetArchiveWindow(NULL))
   {
   switch (pNm->hdr.code)
      {
      case NM_RCLICK: /* Now is it a right mouse button click? */
           {
           dwpos = GetMessagePos();
           PostMessage(hWndMain, WM_RBUTTONUP, (WPARAM) 0L, (LPARAM) dwpos); /* Yep */
           break;
           }

      case NM_DBLCLK: /* Double-click in ListView control */
           {
           if ( uf.fCanDragDrop )
              DragAcceptFiles( hwnd, FALSE );
           MapCursorToListItem(pNm->hdr.hwndFrom);
           UpdateButtons();
           if (wLBSelection == IDM_DISPLAY)
              DisplayFiles(GetArchiveWindow(NULL), FALSE);
           else
              Action(GetArchiveWindow(NULL), (int)wLBSelection);

           if (wLBSelection == IDM_TEST)
              {
              // Switch to the message tab
              TabCtrl_SetCurSel(hTabCtrl, 4);
              ShowWindow(hWndMessage, SW_SHOW);
              ShowWindow(hWndEditor, SW_HIDE);
              ShowWindow(hWndListParent, SW_HIDE);
              ShowWindow(hWndGrep, SW_HIDE);
              ShowWindow(hWndArchivesFound, SW_HIDE);
              }
           if ( uf.fCanDragDrop )
               DragAcceptFiles( hwnd, TRUE );
           break;
           }

         /* This is for "sorting" and is a kludge as it requires reading
            from disk each time you do a sort, but I don't know a better
            way other than keeping a list in memory which results in a
            whole bunch of memory being eaten up just for a "fancy". If
            the time delay bothers you - don't do it, just take this out.
          */
      case LVN_COLUMNCLICK: /* Column clicked on - now sort */
           ListViewSortOnColumns(pNm->iSubItem);
           break;
      case LVN_ITEMCHANGED:
           {
           DisplaySelectionTotals();
           UpdateButtons();
           break;
           }
      }
   }
return 0;
}

//
//  FUNCTION: MDIChildWndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for MDI child windows.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - message number
//    wparam   - additional information (dependant of message number)
//    lparam   - additional information (dependant of message number)
//
//  RETURN VALUE:
//    Depends on the message number.
//
//  COMMENTS:
//    Uses the combination of the message structure defined in mditable.c
//    and the DispMessage function defined in WndProc.c to handle all
//    messages directed to an MDI child window.
//

LRESULT CALLBACK MDIChildWndProc(HWND hwnd,
                                 UINT uMessage,
                                 WPARAM wparam,
                                 LPARAM lparam)
{
    return DispMessage( &msdiChild, hwnd, uMessage, wparam, lparam );
}


//
//  FUNCTION: MsgMCCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_COMMAND messages for mdi children.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - message number (Unused)
//    GET_WM_COMMAND_ID(wparam,lparam)   - Command identifier
//    GET_WM_COMMAND_HWND(wparam,lparam) - Control handle
//
//  RETURN VALUE:
//
//  COMMENTS:
//    Uses the combination of the command structure defined in mditable.c
//    and the DispCommand function defined in WndProc.c to handle all
//    commands directed to an MDI child window.
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgMCCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispCommand(&cmdiChild, hwnd, wparam, lparam);
}


//
//  FUNCTION: MsgMCCreate(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgMCCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{

    WinAssert(hwnd);
    if (!CreateArchiveControl(hwnd))
        return -1;

    return 0;
}


//
//  FUNCTION: MsgMCSize(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Resize the listview control window within the MDI Child
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//    
//
//  COMMENTS:
//    We must call DefMDIChildProc so that if the child is being maximized
//    we'll get the system menu, min, max and close buttons painted properly.
//

LRESULT MsgMCSize(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
LRESULT ret;
char sz[PATH_MAX + 21];

WinAssert(hwnd);

// Position the MDI client window between the tool and status bars
if (wparam != SIZE_MINIMIZED)
   {
   RECT rc, rcClient;

   GetClientRect(hwnd, &rcClient);

   GetWindowRect(GetStatusWindow(hwnd), &rc);
   rcClient.bottom = rcClient.bottom -
                     (rc.bottom - rc.top);

   MoveWindow(GetArchiveWindow(hwnd),
              0,
              0,
              rcClient.right,
              rcClient.bottom,
              TRUE);
   // Re-position the panes in the status bar
   InitializeArchiveStatusBar(hwnd);
   SendMessage(GetStatusWindow(hwnd),  uMessage, wparam, lparam);
   }

ret = DefMDIChildProc(hwnd, uMessage, wparam, lparam);
// This little tap dance is here because the DefMDIChildProc, for some reason
// is clearing the parent window title also when an archive window is maximized
// or restored from being maximized. The title is being changed as you can no
// longer see a window title of the MDI child when maximized.
if (wparam == SIZE_MAXIMIZED)
   {
   GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);
   sprintf(sz, "Wiz extracts to: %s. Current Archive = %s",
           szUnzipToDirName,
           lpumb->szFileName);
   SendMessage(hWndMain, WM_SETTEXT, 0,
              (LPARAM)(LPCTSTR)sz);
   }
if (wparam == SIZE_RESTORED)
   {
   sprintf(sz, "Wiz extracts to: %s",
           szUnzipToDirName);
   SendMessage(hWndMain, WM_SETTEXT, 0,
              (LPARAM)(LPCTSTR)sz);
   }
return ret;
}


//
//  FUNCTION: MsgMCMenuSelect(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

LRESULT MsgMCMenuSelect(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    WinAssert(hwnd);
    return DefMDIChildProc(hwnd, uMessage, wparam, lparam);
}


//
//  FUNCTION: MsgMCSetFocus(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:
//    Sets the focus to the listview control window contained within this MDI
//    child window.
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgMCSetFocus(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    HWND hTemp;

    if ((hTemp = GetArchiveWindow(NULL)) == NULL)
       return 0;
    SetFocus(hTemp);
    UpdateButtons();
    DisplaySelectionTotals();
    return 0;
}


//
//  FUNCTION: MsgMCDestroy(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:
//
//  PARAMETERS:
//    hwnd - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam - Message specific data (unused).
//    lparam - Message specific data (unused).
//
//  RETURN VALUE:
//    Always returns 0 - message handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT MsgMCDestroy(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    HDC hdc;
    TCHAR sz[PATH_MAX], c;
    int i;

    WinAssert(hwnd);
    /* If the archive window is associated with a file in the Windows
       temp directory, then we will delete it when the window is closed.
     */
    GetWindowText(hwnd, sz, PATH_MAX);

    GetTempPath(PATH_MAX, szWindowsTempDir);
    i = lstrlen(szWindowsTempDir);
    if (i < lstrlen(sz))
       {
       c = sz[i];
       sz[i] = '\0';
       if (lstrcmpi(szWindowsTempDir, sz) == 0)
          {
          sz[i] = c;
          DeleteFile(sz);
          }
       }
    cOpen -= 1; // We had a zip archive
    RemoveProp(hwnd, "FilenameSet");

    hdc = RemoveProp(hwnd, "TargetDC");
    if (hdc)
        DeleteDC(hdc);
    if (GetArchiveWindow(NULL) == NULL)
       {
       lpumb->szFileName[0] = '\0';
       UpdateStatusBar("Info-ZIP's Wiz", 0, 0);
       }
    else
       {
       GetWindowText(GetActiveMDIChild(), lpumb->szFileName, PATH_MAX);
       }
    UpdateButtons();
    return 0;
}

//
//  FUNCTION: CmdArchive(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handle listview control notifications.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDC_EXIT (unused)
//    wNotify  - EN_*
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//    Handle the edit control's out of space error by putting up an
//    "Out of Memory" warning dialog.
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdArchive(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    WinAssert(hwnd);
    switch (wNotify)
    {
        case EN_ERRSPACE:
            MessageBox(hwnd,
                       "Out of memory.",
                       szTitle,
                       MB_ICONHAND | MB_OK);
            break;
    }
    return 0;
}

//
//  FUNCTION: CreateMDIChild(LPSTR)
//
//  PURPOSE: Create an MDI child window, setting caption to szFileName
//
//  PARAMETERS:
//    hwnd - The main application window.
//
//  RETURN VALUE:
//    TRUE  - If creation was successful.
//    FALSE - If creation failed.
//
//  COMMENTS:
//

BOOL CreateMDIChild(LPSTR szFileName)
{
   HWND  hwndChild, hstatusbar;
   DWORD dwVersion;
   RECT rc, rs;

    // Create the MDI child window

    // Windows NT and Windows 95 present different options for creating
    // an MDI child window.  While using the WM_MDICREATE message will
    // work on both Windows versions, Windows 95 presents a new window
    // style which simplifies the process.  Here the function uses the
    // method apropriate for the system it's running on to illustrate
    // both methods.

    dwVersion = GetVersion();
    if ((dwVersion < 0x80000000) || (LOBYTE(LOWORD(dwVersion)) < 4))
    {
        // This is Windows NT or Win32s, so use the WM_MDICREATE message
        MDICREATESTRUCT mcs;

        mcs.szClass = szChildName;      // window class name
        mcs.szTitle = szFileName;       // window title
        mcs.hOwner  = hInst;            // owner
        mcs.x       = CW_USEDEFAULT;    // x position
        mcs.y       = CW_USEDEFAULT;    // y position
        mcs.cx      = CW_USEDEFAULT;    // width
        mcs.cy      = CW_USEDEFAULT;    // height
        mcs.style   = 0;                // window style
        mcs.lParam  = 0;                // lparam

        hwndChild = (HWND) SendMessage(hWndListParent,
                                       WM_MDICREATE,
                                       0,
                                       (LPARAM)(LPMDICREATESTRUCT) &mcs);
    }
    else
    {
        // This method will only work with Windows 95, not Windows NT or Win32s
        hwndChild = CreateWindowEx(WS_EX_MDICHILD,  // EX window style
                                   szChildName,     // window class name
                                   szFileName,      // window title
                                   0,               // window style
                                   CW_USEDEFAULT,   // x position
                                   CW_USEDEFAULT,   // y position
                                   CW_USEDEFAULT,   // width
                                   CW_USEDEFAULT,   // height
                                   hWndListParent,  // parent
                                   NULL,            // menu (child ID)
                                   hInst,           // owner
                                   0);              // lparam
    }

    if (!hwndChild)
       return FALSE;

    /* Set up the status bar */
    hstatusbar = CreateStatusWindow(WS_CHILD|WS_VISIBLE|WS_BORDER,
                                      "",
                                      hwndChild,
                                      IDC_STATUSBAR);
    if (!hstatusbar)
       return FALSE;
    ShowWindow(hwndChild, SW_SHOW);
    SetProp(hwndChild, "FilenameSet", 0);

    GetClientRect(hwndChild, &rc);
    GetWindowRect(hstatusbar, &rs);
    MoveWindow(GetArchiveWindow(NULL),
               0, 0,
               rc.right,
               (rc.bottom-(rs.bottom-rs.top)),
               TRUE);
    ShowWindow(hstatusbar, SW_SHOW);
    SetFocus(hwndChild);
    cOpen++;
    UpdateButtons();
    return TRUE;
}


//
//  FUNCTION: CreateArchiveControl(HWND)
//
//  PURPOSE: Create the listview control with the MDI child as the parent
//
//  PARAMETERS:
//    hwnd - The MDI child window.
//
//  RETURN VALUE:
//    TRUE - If initialization was successful.
//    FALSE - If initialization failed.
//
//  COMMENTS:
//

BOOL CreateArchiveControl(HWND hwndMDIChild)
{
    RECT rc;
    HWND hwndList;
    int iCol;
    LV_COLUMN lvc;
    char      szText[7][20];
    //szText = {"Name", "Orig Size", "Comp Size","Ratio","  Date", "Time", "Encrypted"};

    lstrcpy(szText[0], GetLoadString(IDS_NAME));

    lstrcpy(szText[1], GetLoadString(IDS_ORIG_SIZE));
    lstrcpy(szText[2], GetLoadString(IDS_COMP_SIZE));
    lstrcpy(szText[3], GetLoadString(IDS_RATIO));
    lstrcpy(szText[4], GetLoadString(IDS_DATE));
    lstrcpy(szText[5], GetLoadString(IDS_TIME));
    lstrcpy(szText[6], GetLoadString(IDS_CRYPT));

    GetClientRect(hwndMDIChild, &rc);

    hwndList = CreateWindow(WC_LISTVIEW, NULL,
                        WS_CHILD | WS_DLGFRAME |
                        LVS_REPORT | LVS_SHOWSELALWAYS |
                        LVS_SORTASCENDING,
                        0, 0,
                        (rc.right-rc.left), (rc.bottom-rc.top),
                        hwndMDIChild,
                        (HMENU)IDC_ARCHIVE,
                        hInst,
                        NULL);

    WinAssert(hwndList);
#ifndef __BORLANDC__
    ListView_SetExtendedListViewStyle(hwndList,LVS_EX_FULLROWSELECT);
#endif     
    if (!hwndList)
    {
        DestroyWindow(hwndMDIChild);
        return FALSE;
    }
    SendMessage(hwndList, WM_SETFONT, (WPARAM)hFixedFont, FALSE);

    /* Initialize the LV_COLUMN structure. */
    lvc.mask    = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt     = LVCFMT_LEFT;
    lvc.cx      = 95;

    for (iCol = 0; iCol < 7; iCol++)
        {
        lvc.pszText = szText[iCol];
        lvc.iSubItem = iCol;
        if (ListView_InsertColumn(hwndList, iCol, &lvc) == -1)
           {
           DestroyWindow(hwndMDIChild);
           return FALSE;
           }
        }

    ListView_SetColumnWidth(hwndList, 0, MAKELPARAM((int)30*dxChar, 0)); // Name
    ListView_SetColumnWidth(hwndList, 1, MAKELPARAM((int)11*dxChar, 0)); // Orig size
    ListView_SetColumnWidth(hwndList, 2, MAKELPARAM((int)11*dxChar, 0)); // Comp size
    ListView_SetColumnWidth(hwndList, 3, MAKELPARAM((int)7*dxChar, 0));  // Ratio
    ListView_SetColumnWidth(hwndList, 4, MAKELPARAM((int)12*dxChar, 0)); // Date
    ListView_SetColumnWidth(hwndList, 5, MAKELPARAM((int)7*dxChar, 0));  // Time
    ListView_SetColumnWidth(hwndList, 6, MAKELPARAM((int)11*dxChar, 0)); // Encrypted
    ShowWindow(hwndList, SW_SHOW);
    return TRUE;
}


//
//  FUNCTION: GetArchiveWindow(HWND)
//
//  PURPOSE: Return a handle to the current listview control.  The "current"
//           listview control is contained within the active MDI child
//
//  PARAMETERS:
//    hwdnMDIChild - handle to the MDI Child window
//
//  RETURN VALUE:
//    A handle to the current listview control.
//
//  COMMENTS:
//    If hwndMDIChild is NULL on entry, we'll return the handle to the
//    listview control in the active MDI child
//

HWND GetArchiveWindow(HWND hwndMDIChild)
{
    HWND hwndArchive;

    if (!hwndMDIChild)
        hwndMDIChild = GetActiveMDIChild();
    if (!hwndMDIChild)
       return NULL;
    hwndArchive = GetDlgItem(hwndMDIChild, IDC_ARCHIVE);

    return hwndArchive;
}


//
//  FUNCTION: GetStatusWindow(HWND)
//
//  PURPOSE: Return a handle to the current status bar.  The "current"
//           status bar is contained within the active MDI child
//
//  PARAMETERS:
//    hwdnMDIChild - handle to the MDI Child window
//
//  RETURN VALUE:
//    A handle to the current status bar.
//
//  COMMENTS:
//    If hwndMDIChild is NULL on entry, we'll return the handle to the
//    status bar in the active MDI child
//

HWND GetStatusWindow(HWND hwndMDIChild)
{
    HWND hwndArchive;

    if (!hwndMDIChild)
        hwndMDIChild = GetActiveMDIChild();
    if (!hwndMDIChild)
       return NULL;
    hwndArchive = GetDlgItem(hwndMDIChild, IDC_STATUSBAR);

    return hwndArchive;
}


//
//  FUNCTION: GetActiveMDIChild(void)
//
//  PURPOSE: Return a handle to the current active MDI child
//
//  PARAMETERS:
//    void
//
//  RETURN VALUE:
//    A handle to the current active MDI child
//
//  COMMENTS:
//

HWND GetActiveMDIChild(void)
{
    HWND hwndMDIChild;

    hwndMDIChild = (HWND)SendMessage(hWndListParent,
                                     WM_MDIGETACTIVE,
                                     0, 0L);
    return hwndMDIChild;
}

