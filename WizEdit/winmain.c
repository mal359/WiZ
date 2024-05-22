// Copyright (C) 1997 Mike White
//
//  MODULE:   winmain.c
//
//  PURPOSE:   Calls initialization functions and processes the message loop
//
//
//  PLATFORMS: Windows 95, Windows NT
//
//  FUNCTIONS:
//    WinMain() - calls initialization functions, processes message loop
//
//  COMMENTS:
//
//

#include <windows.h>            // required for all Windows applications
#include <commctrl.h>
#include <richedit.h>

typedef LRESULT (WINAPI * DLL_LRESULT)(void);
typedef void (WINAPI * DLL_VOID)(MSG);
typedef HWND (WINAPI * DLL_HWND)(HINSTANCE, int, HWND);
typedef BOOL (WINAPI * DLL_BOOL)(LPSTR);

void ParseArgs(int argc, char ** argv);

DLL_LRESULT DllOnIdle;
DLL_VOID DllTranslate;
DLL_HWND InitDll;
DLL_BOOL wizedit_OpenCmdLineFile;

HANDLE hEditDll;
HANDLE hRTFLib;

/* command line parameters */
#ifdef __BORLANDC__
extern int _argc;
extern char ** _argv;
int __argc;
char ** __argv;
#else
extern int __argc;
extern char ** __argv;
#endif

//
//  FUNCTION: WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
//
//  PURPOSE: calls initialization function, processes message loop
//
//  PARAMETERS:
//
//    hInstance - The handle to the instance of this application that
//          is currently being executed.
//
//    hPrevInstance - This parameter is always NULL in Win32
//          applications.
//
//    lpCmdLine - A pointer to a null terminated string specifying the
//          command line of the application.
//
//    nCmdShow - Specifies how the main window is to be diplayed.
//
//  RETURN VALUE:
//    If the function terminates before entering the message loop,
//    return FALSE.
//    Otherwise, return the WPARAM value sent by the WM_QUIT message.
//
//
//  COMMENTS:
//
//    Windows recognizes this function by name as the initial entry point
//    for the program.  This function calls the initialization routine
//    It then executes a message retrieval and dispatch loop that is the
//    top-level control structure for the remainder of execution.  The
//    loop is terminated when a WM_QUIT  message is received, at which
//    time this function exits the application instance by returning the
//    value passed by PostQuitMessage().
//
//    If this function must abort before entering the message loop, it
//    returns the conventional value NULL.
//

#ifdef __BORLANDC__
#pragma argsused
#endif
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
   MSG msg;
   BOOL bRet;

   InitCommonControls();   // Initialize the common control library.

   hRTFLib = LoadLibrary("RICHED32.DLL");
   if (!hRTFLib)
      return FALSE;

   hEditDll = LoadLibrary("wizedit.DLL");
   if (!hEditDll)
      {
      if (hRTFLib) FreeLibrary (hRTFLib);
      return FALSE;
      }
   (DLL_LRESULT)DllOnIdle = (DLL_LRESULT)GetProcAddress(hEditDll, "DllOnIdle");
   (DLL_VOID)DllTranslate = (DLL_VOID)GetProcAddress(hEditDll, "DllTranslate");
   (DLL_HWND)InitDll = (DLL_HWND)GetProcAddress(hEditDll, "InitDll");
   (DLL_BOOL)wizedit_OpenCmdLineFile = (DLL_BOOL)GetProcAddress(hEditDll,
                                                           "OpenCmdLineFile");

    if (!DllOnIdle || !DllTranslate || !InitDll || !wizedit_OpenCmdLineFile)
       {
       if (hRTFLib)  FreeLibrary (hRTFLib);
       if (hEditDll) FreeLibrary (hEditDll);
       return FALSE;           // Exits if unable to get valid addresses
       }
    // Initialize dll by setting up the main window.
    if (!(*InitDll)(hInstance, nCmdShow, NULL))
       {
       if (hRTFLib)  FreeLibrary (hRTFLib);
       if (hEditDll) FreeLibrary (hEditDll);
       return FALSE;           // Exits if unable to initialize
       }

#ifdef __BORLANDC__
    __argv = _argv;
    __argc = _argc;
#endif
    ParseArgs(__argc, __argv);

    // In the true Microsoft Foundation Classes spirit, we implement
    // OnIdle() processing.  The OnIdle() routine is used to update
    // the toolbar so we can accurately reflect the active/inactive
    // state of the Save, Cut, Copy and Paste buttons.
    for (; ;)
    {
        while (!PeekMessage(&msg,
                            NULL,
                            (UINT)NULL,
                            (UINT)NULL,
                            PM_NOREMOVE) &&
               (*DllOnIdle)())
        {
            ;  // Do nothing; we either have a message to process, or
               // have already called OnIdle() and can now call
               // GetMessage() again.  GetMessage() will either retrieve
               // the next message, or yield to another application
        }

        // Acquire and dispatch the next message
        // GetMessage() will return 0 when WM_QUIT is retreived
        bRet=GetMessage(&msg, NULL, 0, 0);
        if (bRet)
           {
           (*DllTranslate)(msg);
           }
        else
           break;
    }

    if (hEditDll) FreeLibrary (hEditDll);
    if (hRTFLib) FreeLibrary (hRTFLib);
    return msg.wParam;  // Returns the value from PostQuitMessage
}

void ParseArgs(int argc, char ** argv)
{
int i;
WIN32_FIND_DATA wfd;
HANDLE hTemp;

for (i = 1; i < argc; i++)
    {
    hTemp = FindFirstFile(argv[i], &wfd);
    if (hTemp != INVALID_HANDLE_VALUE)
       {
       (*wizedit_OpenCmdLineFile)(argv[i]);
       }
    FindClose(hTemp);
    }
} /* ParseArgs */

