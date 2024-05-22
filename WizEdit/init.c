// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995 Microsoft Corporation.  All Rights Reserved.
//
// Modifications Copyright (C) 1997  Mike White
//
//  MODULE:   init.c
//
//  PURPOSE:   Performs application and instance specific initialization,
//             as well as provides all entry points for the dll.
//
//  FUNCTIONS:
//    InitDll() - Initializes window data and registers window.
//    StreamBuffer - Creates a new MDI child, and reads (EM_STREAMIN) the
//                   contents of the buffer into the edit control for that
//                   MDI child.
//
//  COMMENTS:
//

#include <windows.h>            // required for all Windows applications

#define NOTREEVIEW
#define NOIMAGEAPIS

#include <commctrl.h>
#include <richedit.h>
#include "..\wizedit\globals.h"            // prototypes specific to this application
#include "..\wizedit\resource.h"

HINSTANCE hInst;                // current instance
HWND      hwndMain;             // Main window handle
HANDLE hAccelTable;             // Accelerator table handle

char szAppName[12];             // The name of this application
char szTitle[40];               // The title bar text
extern HINSTANCE hDllInst;
LPCTSTR szClassName = "EditClass";
LPSTR lpStr;
DWORD dwBytesRead;

DWORD CALLBACK BufferRead(DWORD, LPBYTE, LONG, LONG *);

//
//  FUNCTION: InitDll(HINSTANCE, int, HWND)
//
//  PURPOSE: Initializes window data and registers window class.
//
//  PARAMETERS:
//    hInstance - The handle to the instance of this application that
//                is currently being executed.
//    nCmdShow  - Specifies how the main window is to be displayed.
//    hWindow   - Parent window handle if to be a child, else null;
//
//  RETURN VALUE:
//    TRUE  - Success
//    FALSE - Initialization failed
//
//  COMMENTS:
//
//    This function is called at dll initialization time.  It
//    performs initialization tasks for the current application instance.
//    Unlike Win16, in Win32, each instance of an application must register
//    window classes.
//
//    In this function, we initialize a window class by filling out a data
//    structure of type WNDCLASS and calling the Windows RegisterClass()
//    function.  Then we create the main window and show it.
//
//
HWND WINAPI InitDll(HINSTANCE hInstance, int nCmdShow, HWND hWindow)
{
    WNDCLASSEX wc;
    DWORD dwStyle = WS_EX_CONTEXTHELP | WS_EX_WINDOWEDGE;

    // Save the instance handle in static variable, which will be used in
    // many subsequence calls from this application to Windows.

    hInst = hDllInst; // Store instance handle in our global variable

    // Load the application name and description strings.
    LoadString(hDllInst, IDS_APPNAME, szAppName, sizeof(szAppName));
    LoadString(hDllInst, IDS_DESCRIPTION, szTitle, sizeof(szTitle));

    // Fill in window class structure with parameters that describe the
    // main window.

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW; // Class style(s).
    wc.lpfnWndProc   = (WNDPROC)WndProc;        // Window Procedure
    wc.cbClsExtra    = 0;                       // No per-class extra data.
    wc.cbWndExtra    = 0;                       // No per-window extra data.
    wc.hInstance     = hDllInst;               // Owner of this class
    wc.hIcon         = LoadIcon(hDllInst, MAKEINTRESOURCE(IDI_APPICON)); // Icon name from .RC
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); // Cursor
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Default color
    wc.lpszMenuName  = szAppName;               // Menu name from .RC
    wc.lpszClassName = szClassName;               // Name to register as
    wc.hIconSm       = LoadImage(hDllInst,      // Load small icon image
                                MAKEINTRESOURCE(IDI_APPICON),
                                IMAGE_ICON,
                                16, 16,
                                0);

    // Register the window class and return FALSE if unsuccesful.
    if (!RegisterClassEx(&wc))
    {
        //Assume we are running on NT where RegisterClassEx() is
        //not implemented, so let's try calling RegisterClass().

        if (!RegisterClass((LPWNDCLASS)&wc.style))
           {
           return NULL;
           }
    }

    if (hWindow)
       dwStyle = dwStyle | WS_CHILD;

    // Create a main window for this application instance.
    hwndMain = CreateWindow(//Ex(
//                        dwStyle,
                        szClassName,         // See RegisterClass() call
                        szTitle,             // Text for window title bar
                        WS_OVERLAPPEDWINDOW, // Window style
                        CW_USEDEFAULT, 0,    // Use default positioning
                        CW_USEDEFAULT, 0,    // Use default size
                        hWindow,             // Overlapped has no parent
                        NULL,                // Use the window class menu
                        hDllInst,            // This instance owns this window
                        NULL                 // Don't need data in WM_CREATE
    );

    // If window could not be created, return "failure"
    if (!hwndMain)
        {
        MessageBox(NULL, "Failed to create main window", "Wiz", MB_OK);
        return NULL;
        }

    // **BlandMDI** Initialize the Multiple Document Interface

    if (!InitMDIChild(hInst))
       {
       return NULL;
       }

    if (!InitFindReplace())
       {
       return NULL;
       }

    if (!InitRuler(hDllInst))
       {
       return NULL;
       }

    if (!InitPreview(hInst))
       {
       return NULL;
       }

    // Make the window visible; update its client area; and return "success"
    if (!hWindow)
       ShowWindow(hwndMain, nCmdShow);  // Show the window
    UpdateWindow(hwndMain);             // Sends WM_PAINT message

    // Load accelerator table
    hAccelTable = LoadAccelerators(hInstance, szAppName);

    return hwndMain;                 // We succeeded...
}

//
//  FUNCTION: StreamBuffer(LPSTR buffer, LPSTR name)
//
//  PURPOSE: Creates a new MDI child, and reads (EM_STREAMIN) the contents of
//           the buffer into the edit control of the child.
//
//  PARAMETERS:
//    buffer - Buffer to be read into the newly created MDI child
//    name   - Title of name to be displayed on child title bar.
//
//  RETURN VALUE:
//    TRUE  - Success
//    FALSE - Read failed
//
//  COMMENTS:
//

BOOL WINAPI StreamBuffer(LPSTR lpsz, LPSTR name)
{
   HWND hTemp;
   EDITSTREAM es;
   DWORD dwFormat;
   TCHAR szType[cchRTFSig + 1];

   es.dwError = 0;
   es.pfnCallback = BufferRead;

   es.dwCookie = (DWORD)lstrlen(lpsz); // Total bytes to read
   dwBytesRead = 0;                    // Bytes read by BufferRead
   lpStr = lpsz;
   if (lstrlen(lpsz) > cchRTFSig)
      {
      lstrcpyn(szType, lpsz, cchRTFSig+1);
      dwFormat = lstrcmpi(szRTFSig, szType) ? SF_TEXT : SF_RTF;
      }
   else
      dwFormat = SF_TEXT; // Too short to be .rtf file

   CreateMDIChild(name);
   hTemp = GetEditWindow(NULL);

   WinAssert(hTemp);
   SendMessage(hTemp, EM_STREAMIN,
      (WPARAM) dwFormat,
      (LPARAM) &es);

    RTF_ShowCharAttributes();

    // Clear the dirty bit
    SendMessage(hTemp, EM_SETMODIFY, FALSE, 0L);

    return 0;
}

//
//  FUNCTION: BufferRead(DWORD dwCookie, LPBYTE lpsz, LONG dwSize,
//   LONG *bytes_read)
//
//  PURPOSE: Callback function to read contents of buffer into an edit control
//
//  PARAMETERS:
//    dwCookie - Size of buffer to be read into
//    lpsz     - Buffer to be read into
//    dwSize   - Number of bytes requested
//    bytes_read - Number of bytes actually read.
//
//  RETURN VALUE:
//    Number of bytes read
//
//  COMMENTS:
//

DWORD CALLBACK BufferRead(DWORD dwCookie, LPBYTE lpsz, LONG dwSize,
   LONG *bytes_read)
{
LONG lSize = dwSize;
if ((dwSize + dwBytesRead) > dwCookie)
   lSize = dwCookie - dwBytesRead;
memcpy(lpsz, lpStr, lSize);
lpStr += lSize;
dwBytesRead += lSize;
*bytes_read = lSize;
return (DWORD) (*bytes_read >= 0 ? NOERROR : (*bytes_read = 0, E_FAIL));
}

//
//  FUNCTION: OpenCmdLineFile(LPSTR name)
//
//  PURPOSE: Creates a new MDI child passed in from the command line.
//
//  PARAMETERS:
//    name   - Title of name to be displayed on child title bar.
//
//  RETURN VALUE:
//    TRUE  - Success
//    FALSE - Read failed
//
//  COMMENTS:
//

BOOL WINAPI OpenCmdLineFile(LPSTR fn)
{
   HWND hTemp;

   CreateMDIChild(fn);
   Open(fn,
        FALSE,
        NULL);
   hTemp = GetEditWindow(NULL);

   WinAssert(hTemp);

   RTF_ShowCharAttributes();

   // Clear the dirty bit
   SendMessage(hTemp, EM_SETMODIFY, FALSE, 0L);

   return 0;
}


