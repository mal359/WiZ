// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995 Microsoft Corporation.  All Rights Reserved.
//
// Modifications Copyright (C) 1997  Mike White
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
//    MsgMCClose       - Handle the WM_CLOSE message for the MDI child
//    MsgMCDestroy     - Handle the WM_DESTROY message for the MDI child
//    CmdEdit          - Handle the notification messages for the edit control
//                       contained within the MDI child
//
//  COMMENTS:
//
//
//  SPECIAL INSTRUCTIONS: N/A
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <richedit.h>
#include "..\wizedit\globals.h"            // prototypes specific to this application
#include "..\wizedit\resource.h"
#include "..\wizedit\mdichild.h"
#include "..\wizedit\toolbar.h"
#include "..\wizedit\statbar.h"
#include "..\wizedit\ruler.h"
#include "..\wizedit\rtf.h"

LRESULT MsgMDNotify(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam);

static MSD rgmsd[] =
{
    {WM_CLOSE,       MsgMCClose   },
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
    {IDC_EDIT,           CmdEdit},
};

CMDI cmdiChild =
{
    sizeof(rgcmd) / sizeof(CMD),
    rgcmd,
    edwpMDIChild
};

//Module specific globals

static char szChildName[12];
static HANDLE hszEditBuffer = NULL;


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

    LoadString(hinst, IDS_CHILDNAME, szChildName, sizeof(szChildName));

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
    if (!CreateEditControl(hwnd))
        return -1;
   
    if (!CreateRuler(hwnd, ID_RULER, 1024, 32))
        return -1;

    return 0;
}


//
//  FUNCTION: MsgMCSize(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Resize the edit control window within the MDI Child
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
LRESULT lRet;
WinAssert(hwnd);

// This stuff is in here so that the edit window gets drawn properly
// when resizing it.
SendMessage (GetEditWindow(hwnd), WM_SETREDRAW, FALSE, 0L);
MoveWindow(GetEditWindow(hwnd),
           0, 32,
           LOWORD(lparam), HIWORD(lparam)-32,
           TRUE);
SendMessage (GetEditWindow(hwnd), WM_SETREDRAW, TRUE, 0L);
InvalidateRect(GetEditWindow(hwnd), NULL, FALSE);

lRet = DefMDIChildProc(hwnd, uMessage, wparam, lparam);
return lRet;
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
//    Sets the focus to the edit control window contained within this MDI
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

    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return 0;
    WinAssert(hTemp);
    SetFocus(hTemp);

    // Update the ruler to reflect the margins
    RTF_ShowMargins(GetDlgItem(hwnd, ID_RULER));

    // Update the toolbar to show the current text attributes
    RTF_ShowCharAttributes();

    return 0;
}


//
//  FUNCTION: MsgMCClose(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Query user to save file before closing window
//
//  PARAMETERS:
//    hwnd - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam - Message specific data (unused).
//    lparam - Message specific data (unused).
//
//  RETURN VALUE:
//    Return 0 if user hits "Cancel" from the query save dialog.  This
//    prevents the MDI child from closing.
//
//  COMMENTS:
//    Call DefMDIChildProc so that the window gets destroyed.
//

LRESULT MsgMCClose(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    WinAssert(hwnd);
    if (QuerySaveFile(hwnd))
        return DefMDIChildProc(hwnd, uMessage, wparam, lparam);
    else
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

    WinAssert(hwnd);
    cOpen -= 1;
    RemoveProp(hwnd, "FilenameSet");

    hdc = RemoveProp(hwnd, "TargetDC");
    if (hdc)
        DeleteDC(hdc);

    return 0;
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
    WinAssert(hwnd);
    switch ( ((LPNMHDR)lparam)->code )
    {
        // User changed the ruler's "knobbies" or tabs- See RULER.H
        case RN_CHANGEDSETTINGS:

            RTF_ChangeMargins(GetDlgItem(hwnd, ID_RULER),  // HwndRuler
                              ((LPNMRULER)lparam)->iFirstLineIndent,
                              ((LPNMRULER)lparam)->iRight,
                              ((LPNMRULER)lparam)->iLeft -
                                ((LPNMRULER)lparam)->iFirstLineIndent);
            break;

        // User changed text in RichEdit control- See COMMCTRL.H
        case EN_SELCHANGE:

            RTF_ShowMargins(GetDlgItem(hwnd, ID_RULER));
            RTF_ShowCharAttributes();
            break;

    }
    return 0;
}

//
//  FUNCTION: CmdEdit(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handle edit control notifications.
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
LRESULT CmdEdit(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
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
   HWND  hwndChild;
   DWORD dwVersion;

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

        hwndChild = (HWND) SendMessage(hwndMDIClient,
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
                                   hwndMDIClient,   // parent
                                   NULL,            // menu (child ID)
                                   hInst,           // owner
                                   0);              // lparam
    }

    WinAssert(hwndChild);
    if (hwndChild != NULL)
    {
        ShowWindow(hwndChild, SW_SHOW);
        SetProp(hwndChild, "FilenameSet", 0);
        SetFocus(hwndChild);

        cOpen++;
        return TRUE;
    }
    else
        return FALSE;
}


//
//  FUNCTION: CreateEditControl(HWND)
//
//  PURPOSE: Create the RichEdit control with the MDI child as the parent
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

BOOL CreateEditControl(HWND hwndMDIChild)
{
    RECT rc;
    HWND hwndEdit;
    HDC  hDC;
    CHARFORMAT cf;

    GetClientRect(hwndMDIChild, &rc);

    hwndEdit = CreateWindow("RichEdit",
                            NULL,
                            WS_CHILD   | WS_VISIBLE | ES_MULTILINE   |
                            WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL |
                            ES_AUTOVSCROLL | ES_NOHIDESEL,
                            0, 0,
                            (rc.right-rc.left), (rc.bottom-rc.top),
                            hwndMDIChild,
                            (HMENU)IDC_EDIT,           // Child control i.d.
                            hInst,
                            NULL);

    WinAssert(hwndEdit);
    if (!hwndEdit)
    {
        DestroyWindow(hwndMDIChild);
        return FALSE;
    }

    // Set the RichEdit control to send the EN_SELCHANGE notification
    // via the WM_NOTIFY message.
    SendMessage(hwndEdit, EM_SETEVENTMASK, 0, ENM_SELCHANGE);

    // REVIEW
    // Set up the RichEdit control to act as WSYWIG as possible,
    // here we force a 7" wide page size.
    hDC = CreateCompatibleDC(NULL);
//    hDC = GetCurrentPrinterDC();
    if (SendMessage(hwndEdit, EM_SETTARGETDEVICE, (WPARAM)hDC, (LPARAM)(1440*7)))
        SetProp(hwndMDIChild, "TargetDC", hDC);
    else
        DeleteDC(hDC);

    // Set default character format...  The point here is to avoid having
    // the system font as default since it generally doesn't scale well.

    // Set up CHARFORMAT for default 10 point Times New Roman with no
    // extra effects (bold, italic, etc.)
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_ITALIC | CFM_STRIKEOUT
                | CFM_UNDERLINE | CFM_COLOR | CFM_OFFSET | CFM_PROTECTED;
    cf.dwEffects = CFE_AUTOCOLOR;   // use COLOR_WINDOWTEXT
    cf.yHeight = 200; // 200 twips == 10 points
    cf.yOffset = 0;
    cf.crTextColor = 0;
    cf.bPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
    lstrcpy(cf.szFaceName, TEXT("Arial"));

    // Set the default character format.
    // Changed the selection from SCF_SELECTION to SCF_ALL
    SendMessage(hwndEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);

    return TRUE;
}


//
//  FUNCTION: GetEditWindow(HWND)
//
//  PURPOSE: Return a handle to the current edit control.  The "current"
//           edit control is contained within the active MDI child
//
//  PARAMETERS:
//    hwdnMDIChild - handle to the MDI Child window
//
//  RETURN VALUE:
//    A handle to the current edit control.
//
//  COMMENTS:
//    If hwndMDIChild is NULL on entry, we'll return the handle to the
//    edit control in the active MDI child
//

HWND GetEditWindow(HWND hwndMDIChild)
{
    HWND hwndEdit;

    if (!hwndMDIChild)
        hwndMDIChild = GetActiveMDIChild();
    if (!hwndMDIChild)
       return NULL;
    hwndEdit = GetDlgItem(hwndMDIChild, IDC_EDIT);

    return hwndEdit;
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

    hwndMDIChild = (HWND)SendMessage(hwndMDIClient,
                                     WM_MDIGETACTIVE,
                                     0, 0L);
    return hwndMDIChild;
}


//
//  FUNCTION: SetEditText(HWND, hsz)
//
//  PURPOSE: Set the text of the edit control hwnd.
//
//  PARAMETERS:
//    hwnd - The edit control to set.
//    hsz  - A local handle to the text to set.
//
//  RETURN VALUE:
//    NONE
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
VOID SetEditText(HWND hwnd, HANDLE hsz)
{
    char *sz;
    HWND hTemp;

    WinAssert(hwnd);
    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return;

    WinAssert(hTemp);
    sz = LocalLock(hsz);
    SendMessage(hTemp, WM_SETTEXT, 0, (LPARAM)(LPSTR)sz);
    LocalUnlock(hsz);
    LocalFree(hsz);
}


//
//  FUNCTION: LockEditText(HWND)
//
//  PURPOSE: Return a handle to the text associated with the edit control.
//
//  PARAMETERS:
//    hwnd - The edit control whose text is to be locked.
//
//  RETURN VALUE:
//    A local buffer containing the text associated with the editor control.
//
//  COMMENTS:
//
//

char *LockEditText(HWND hwnd)
{
    INT cbText;
    char *sz;

    WinAssert(hwnd);
    cbText = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0L) + 1;
    hszEditBuffer = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, cbText);
    if (hszEditBuffer == NULL) return NULL;
    sz = LocalLock(hszEditBuffer);
    if (sz == NULL)
    {
        LocalFree(hszEditBuffer);
        hszEditBuffer = NULL;
        return NULL;
    }
    SendMessage(hwnd, WM_GETTEXT, cbText, (LPARAM)(LPSTR)sz);
    return sz;
}


//
//  FUNCTION: UnlockEditText(HWND)
//
//  PURPOSE: Return a handle to the text associated with the edit control.
//
//  PARAMETERS:
//    hwnd - The edit control whose text is to be unlocked.
//
//  RETURN VALUE:
//    NONE
//
//  COMMENTS:
//
//

VOID UnlockEditText(HWND hwnd)
{
    WinAssert(hwnd);
    SetEditText(hwnd, hszEditBuffer);
    hszEditBuffer = NULL;
}
