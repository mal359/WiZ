//  Copyright (C) 1997  Mike White
//
//  MODULE:   dllmain.c
//
//  PURPOSE:   Calls initialization functions for the dll
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
#include "..\wizedit\globals.h"            // prototypes specific to this application

HANDLE hDllInst;

/*  DLL Entry Point */

#ifdef __BORLANDC__
#pragma argsused
#define DllMain DllEntryPoint
#endif
BOOL WINAPI DllMain( HANDLE hInstance,
                     DWORD dwReason,
                     LPVOID plvReserved)
{
BOOL rc = TRUE;
HWND hwndT;

switch( dwReason )
   {
   case DLL_PROCESS_ATTACH:
      // DLL is loaded. Do your initialization here.
      // If cannot init, set rc to FALSE.
      hDllInst = hInstance;
      break;

   case DLL_PROCESS_DETACH:
      // DLL is unloaded. Do your cleanup here.

      hwndT = GetActiveMDIChild();
      while ((hwndT != NULL) && (hwndMDIClient != NULL))
        {
        SendMessage(hwndMDIClient, WM_MDIDESTROY, (WPARAM)hwndT, 0);
        hwndT = GetActiveMDIChild();
        }
      if (hwndMDIClient != NULL)
         SendMessage(hwndMDIClient, WM_CLOSE, 0, 0L);
      break;

   default:
      break;
   }
return rc;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
int FAR PASCAL WEP ( int bSystemExit )
{
return 1;
}


