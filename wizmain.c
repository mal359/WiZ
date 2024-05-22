/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

/****************************************************************************

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        WizInit() - initializes window data and registers window
        WizMainWndProc() - processes messages
        About() - processes messages for "About" dialog box

****************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include "wiz.h"
#include "wizver.h"
#include "unzip\windll\unzver.h"
#include "zip\windll\zipver.h"
#include <winver.h>
#include "zip\api.h"

TCHAR szWindowsTempDir[PATH_MAX];
TCHAR szExplorer[PATH_MAX];
TCHAR szUnzipToDirName[PATH_MAX];    /* extraction ("unzip to") directory name in ANSI */

#if (!defined(USE_UNZIP_LIB) || !defined(USE_ZIP_LIB))
#define DLL_WARNING "Cannot find %s."\
            " The Dll must be in the application directory, the path, "\
            "the Windows directory or the Windows System directory."
#define DLL_VERSION_WARNING "%s has the wrong version number."\
            " Insure that you have the correct dll's installed, and that "\
            "an older dll is not in your path or Windows System directory."
#endif

/* Splash screen stuff */
#define TIME_DELAY 0
extern HWND hSplashScreen;
void ShowSplashScreen(void);

BOOL fExecutedFile = FALSE;
HANDLE hListbox;            /* current instance */
HMENU  hMenu;               /* main menu handle */
HANDLE hAccTable;

HANDLE hHourGlass;          /* handle to hourglass cursor */
HANDLE hSaveCursor;         /* current cursor handle */
//MW HANDLE hHelpCursor;         /* help cursor */
HANDLE hFixedFont;          /* handle to fixed font */
HANDLE hOldFont;            /* handle to old font */

HINSTANCE hInst;            /* current instance */

int hFile;              /* file handle */
HWND hWndMain;          /* the main window handle. */
HWND hWndListParent;    /* list box handle */
HWND hWndMessage;       /* Messages window */
HWND hStatusBar;
int fLB_Selection;

UF  uf;

WPARAM wLBSelection = IDM_DISPLAY; /* default listbox selection action */

HBRUSH hBrush ;         /* brush for  standard window backgrounds  */
LPUMB lpumb;

LPUSERFUNCTIONS lpUserFunctions;
LPZIPUSERFUNCTIONS lpZipUserFunctions;

ZPOPT ZpOpt;
LPDCL lpDCL         = NULL;
HWND hRTF           = NULL;
#ifndef USE_UNZIP_LIB
HINSTANCE hUnzipDll = NULL;
#endif
#ifndef USE_ZIP_LIB
HINSTANCE hZipDll   = NULL;
#endif

PARAMETERS uzpParameters;

HANDLE hUF          = (HANDLE)NULL;
HANDLE hZUF         = (HANDLE)NULL;
HANDLE hDCL         = (HANDLE)NULL;
HANDLE hRTFLib      = (HANDLE)NULL;
HANDLE hStrings     = (HANDLE)NULL;
HANDLE hEditor      = (HANDLE)NULL;
HANDLE hRootDir     = (HANDLE)NULL;
HANDLE hDate        = (HANDLE)NULL;

WORD cZippedFiles;      /* total files in archive */
DWORD dwPlatformId = 0xFFFFFFFF; /* True if running on NT */

HWND hWndEditor;
HINSTANCE hEditLib;

UINT RegisteredMessage;
extern PROCESS_INFORMATION WizDiff_Process_Information;

/* Forward References */
#ifndef USE_UNZIP_LIB
_DLL_UNZIP_TO_MEMORY UnzipToMemory;
_DLL_UNZIP Unz_SingleEntryPoint;
_DLL_UNZVAL UnzipValidate;
_USER_FUNCTIONS UzInit;
_DLL_GREP_ARCHIVE GrepArchive;
#endif

#ifndef USE_ZIP_LIB
_DLL_ZIP ZipArchive;
_ZIP_USER_FUNCTIONS ZipInit;
ZIPSETOPTIONS ZipSetOptions;
ZIPGETOPTIONS ZipGetOptions;
#endif

DLL_LRESULT DllOnIdle;
DLL_VOID DllTranslate;
DLL_HWND InitDll;
DLL_STREAM StreamBuffer;

void ZapWizDirectories(void);
void FreeUpMemory(void);
void UnloadLibraries(void);
BOOL LoadLibraries(HINSTANCE, int);
BOOL IsNT(VOID);
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
long WINAPI WizMainWndProc(HWND, WORD, WPARAM, LPARAM);

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

        This will initialize the window class if it is the first time this
        application is run.  It then creates the window, and processes the
        message loop until a WM_QUIT message is received.  It exits the
        application by returning the value passed by the PostQuitMessage.

****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
int WINAPI WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HINSTANCE hInstance;         /* current instance             */
HINSTANCE hPrevInstance;     /* previous instance            */
LPSTR lpCmdLine;          /* command line                 */
int nCmdShow;             /* show-window type (open/icon) */
{
DWORD second;   /* Used for timing the splash screen */
int i, fZipFile = 0;
BOOL bRet;

hInst = hInstance;
InitCommonControls(); // Have to use this to initialize the editor

if (!WizInit(hInstance))
   {
   MessageBox(NULL, "Unable to initialize", szAppName, MB_OK );
   return FALSE;           /* Exits if unable to initialize */
   }

if (!LoadLibraries(hInstance, nCmdShow))
   return FALSE;
GetWizOptions();             /* Get options from registry */
InitSoundOptions();              /* initialize sound options */
wLBSelection = IDM_DISPLAY;      /* assume default is to display */

UpdateButtons();
SizeWindow();

/* Timeout for splash screen */
second = ((GetTickCount()/1000) + TIME_DELAY);
while ((GetTickCount()/1000) <= second);

if (!uf.fRescan)
   DestroyWindow(hSplashScreen);
//MW hHelpCursor = LoadCursor(hInstance, "HelpCursor");

WinAssert(hWndMain);
hProgressWnd = hWndMain;

if (uf.fRescan)
   {
   CmdCommand(hWndMain, IDM_SCAN_ARCHIVE, 0, 0L);
   DestroyWindow(hSplashScreen);
   }
ShowWindow(hWndMain, nCmdShow);
UpdateWindow(hWndMain);

memset(&WizDiff_Process_Information, 0, sizeof(PROCESS_INFORMATION));

RegisteredMessage = RegisterWindowMessage("WIZ_FORCE_CLOSE");

/*
 Note that we do not need to worry about explorer passing in more than
 one file name at a time, as explorer launches multiple instances of
 an application if multiple files are selected. Hence no checks are
 made here for more than one file name.
 */
if (strlen(lpCmdLine))           /* if filename(s) passed on start-up */
   {
    char * p;

    if (lpCmdLine[0] == '"')
        p = &lpCmdLine[1];
    else
        p = &lpCmdLine[0];
    lstrcpy(lpumb->szFileName, p);
    if (lpumb->szFileName[lstrlen(lpumb->szFileName) - 1] == '"')
        lpumb->szFileName[lstrlen(lpumb->szFileName) - 1] = '\0';

   if (!IsValidArchive(lpumb->szFileName))
      {
      wsprintf (lpumb->szBuffer,
                GetLoadString(IDS_CANT_OPEN),
                lpumb->szFileName);
      MessageBox (hWndMain, lpumb->szBuffer,
                  szAppName,
                  MB_ICONINFORMATION | MB_OK);
      lpumb->szFileName[0] = '\0'; /* pretend filename doesn't exist  */
      lpumb->szDirName[0] = '\0';  /* pretend archive dir. doesn't exist  */
      }

   /* If valid filename change dir to where it lives */
   if (lpumb->szFileName[0] != '\0')
      {
	  lstrcpy(lpumb->szDirName, lpumb->szFileName);
      GetArchiveDir(lpumb->szDirName); /* get archive dir. in ANSI char set */
      if (szUnzipToDirName[0] == '\0') /* or no default */
          {
          /* take only path portion */
          lstrcpy(szUnzipToDirName, lpumb->szDirName);
          }
      lstrcpy(lpumb->szBuffer, lpumb->szDirName);
      CreateMDIChild(lpumb->szFileName);
      InitializeArchiveStatusBar(GetActiveMDIChild());
      UpdateListBox();
      ListViewSetSel(0, TRUE);
      UpdateButtons();
      }
   }

if (GetArchiveWindow(NULL))
   {
   /* All this is here because if the window is being created as we are
      loading, it doesn't report the window size properly to begin with,
      and we get an itsy-bitsy window.
    */
   RECT rc;
   GetClientRect(hWndListParent, &rc);
   MoveWindow(GetActiveMDIChild(),
              0, 0,
              (int)(rc.right*.8),
              (int)(rc.bottom*.7),
              TRUE);
   GetClientRect(GetActiveMDIChild(), &rc);
   MoveWindow(GetArchiveWindow(NULL),
              0, 0,
             rc.right,
             rc.bottom,
              TRUE);
   SetFocus(GetArchiveWindow(NULL));
   }

UpdateStatusBar("Info-ZIP's Wiz", 0, 0); // Initialize the status bar

for (; ;)
    {
    while (!PeekMessage(&lpumb->msg,
                        NULL,
                        (UINT)NULL,
                        (UINT)NULL,
                        PM_NOREMOVE) &&
                        (*DllOnIdle)())
        {
        ;  /* Do nothing; we either have a message to process, or
              have already called OnIdle() and can now call
              GetMessage() again.  GetMessage() will either retrieve
              the next message, or yield to another application
           */
        }

    /* Acquire and dispatch the next message
       GetMessage() will return 0 when WM_QUIT is retrieved
     */  
    bRet = GetMessage(&lpumb->msg, 0, 0, 0);
    if (bRet)
       {
       if (hPatternSelectDlg == 0 || /* Pattern select dialog is non-modal */
          !IsDialogMessage(hPatternSelectDlg, &lpumb->msg))
          {
        /* What we are doing here is to intercept the right mouse button up
           command if it is being sent to Wiz's main window, or any of it's
           child windows - such as the list box or edit box. We then process
           the WM_RBUTTONUP message for either the listbox or the editbox
           as though it were sent to the main window. Another way to do this
           would be to subclass both those windows, but this seems a lot
           easier, with less code involved.
         */
         if (lpumb->msg.hwnd == hWndEditor)
            {
            (*DllTranslate)(lpumb->msg);
            }
         else
         if ((lpumb->msg.message == WM_RBUTTONUP) &&
            (lpumb->msg.hwnd == GetArchiveWindow(NULL)))
            {
            lpumb->msg.hwnd = hWndMain;
            DispatchMessage(&lpumb->msg);
            }
         else
            if ( !TranslateAccelerator(hWndMain, hAccTable, &lpumb->msg) )
               {
               TranslateMessage(&lpumb->msg);
               DispatchMessage(&lpumb->msg);
               }
         }
       }
    if (!bRet)
       break;
  }


/* Don't turn on compiler aliasing or C7 will move
   the following assignment after the GlobalFree()
   which contains the memory for pumb!
*/
i = (int)lpumb->msg.wParam;
UnloadLibraries();
ZapWizDirectories();
FreeUpMemory();
ExitProcess(i);
return i;
}

int WINAPI GetReplaceDlgRetVal(char *filename)
{
int ReplaceDlgRetVal;   /* replace dialog return value */

ReplaceDlgRetVal = DialogBoxParam(hInst, "Replace",
   hWndMain, (DLGPROC)ReplaceProc, (DWORD)(LPSTR)filename);
return ReplaceDlgRetVal;
}

void UnloadLibraries(void)
{
if (hEditLib)  FreeLibrary(hEditLib);

#ifndef USE_UNZIP_LIB
if (hUnzipDll) FreeLibrary(hUnzipDll);
#endif

#ifndef USE_ZIP_LIB
if (hZipDll)   FreeLibrary(hZipDll);
#endif
// This call causes Wiz to take forever to finish unloading
//if (hRTFLib)   FreeLibrary (hRTFLib);

}

void FreeUpMemory(void)
{
if (hStrings)
   {
   GlobalUnlock(hStrings);
   GlobalFree(hStrings);
   }
if (hDCL)
   {
   GlobalUnlock(hDCL);
   GlobalFree(hDCL);
   }
if (hEditor)
   {
   GlobalUnlock(hEditor);
   GlobalFree(hEditor);
   }
if (hRootDir)
   {
   GlobalUnlock(hRootDir);
   GlobalFree(hRootDir);
   }
if (hDate)
   {
   GlobalUnlock(hDate);
   GlobalFree(hDate);
   }
if (hUF)
   {
   GlobalUnlock(hUF);
   GlobalFree(hUF);
   }
if (hZUF)
   {
   GlobalUnlock(hZUF);
   GlobalFree(hZUF);
   }
}

BOOL IsNT(VOID)
{
if(dwPlatformId != 0xFFFFFFFF)
   return dwPlatformId;
else
/* note: GetVersionEx() doesn't exist on WinNT 3.1 */
   {
   if(GetVersion() < 0x80000000)
      {
      (BOOL)dwPlatformId = TRUE;
      }
   else
      {
      (BOOL)dwPlatformId = FALSE;
      }
    }
return dwPlatformId;
}

BOOL LoadLibraries(HINSTANCE hInstance, int nCmdShow)
{
char szFullPath[PATH_MAX];
char *ptr;
#if (!defined(USE_UNZIP_LIB) || !defined(USE_ZIP_LIB))
DWORD dwVerInfoSize;
DWORD dwVerHnd;
HANDLE  hMem;         /* handle to mem alloc'ed */
#endif

if (!hRTFLib)
   hRTFLib = LoadLibrary("RICHED32.DLL");

if (!hRTFLib)
   {
   char szErr[RESOURCE_STR_LEN];
   wsprintf(szErr, GetLoadString(IDS_LOADLIBRARY), GetLastError());
   MessageBox(NULL, szErr, szAppName, MB_OK );
   return FALSE;
   }

IsNT(); /* Are we running on NT? */
hStrings = GlobalAlloc( GPTR, (DWORD)sizeof(UMB));
if ( !hStrings )
   {
   MessageBox(NULL, "Unable to allocate hStrings", szAppName, MB_OK );
   UnloadLibraries();
   return FALSE;
   }

lpumb = (LPUMB)GlobalLock( hStrings );
if ( !lpumb )
   {
   MessageBox(NULL, "Unable to lock hStrings", szAppName, MB_OK );
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }

hDCL = GlobalAlloc( GPTR, (DWORD)sizeof(DCL));
if (!hDCL)
   {
   MessageBox(NULL, "Unable to allocate hDCL", szAppName, MB_OK );
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }
lpDCL = (LPDCL)GlobalLock(hDCL);
if (!lpDCL)
   {
   MessageBox(NULL, "Unable to lock hDCL", szAppName, MB_OK );
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }

hUF = GlobalAlloc( GPTR, (DWORD)sizeof(USERFUNCTIONS));
if (!hUF)
   {
   MessageBox(NULL, "Unable to allocate hUF", szAppName, MB_OK );
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }
lpUserFunctions = (LPUSERFUNCTIONS)GlobalLock(hUF);

if (!lpUserFunctions)
   {
   MessageBox(NULL, "Unable to lock lpUserFunctions", szAppName, MB_OK );
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }

hZUF = GlobalAlloc( GPTR, (DWORD)sizeof(ZIPUSERFUNCTIONS));
if (!hZUF)
   {
   MessageBox(NULL, "Unable to allocate hZUF", szAppName, MB_OK );
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }
lpZipUserFunctions = (LPZIPUSERFUNCTIONS)GlobalLock(hZUF);

if (!lpZipUserFunctions)
   {
   MessageBox(NULL, "Unable to lock lpZipUserFunctions", szAppName, MB_OK );
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }

uf.fCanDragDrop = TRUE;

lpZipUserFunctions->print = DisplayBuf;
lpZipUserFunctions->password = password;
lpUserFunctions->password = password;
lpZipUserFunctions->comment = comment;
lpZipUserFunctions->ServiceApplication = ServiceQueue;

lpUserFunctions->print = DisplayBuf;
lpUserFunctions->sound = SoundDuring;
lpUserFunctions->replace = GetReplaceDlgRetVal;
lpUserFunctions->SendApplicationMessage = ReceiveDllMessage;
lpUserFunctions->ServCallBk = ServiceQueue;

uzpParameters.lpUserFunctions = lpUserFunctions;
uzpParameters.lpDCL = lpDCL;
uzpParameters.xfnc = 0;
uzpParameters.xfnv = NULL;

hWndMain = CreateWindow(//Ex(
//        WS_EX_MDICHILD,                 /* EX window style  */
        szAppName,                      /* window class     */
        NULL,                           /* window name      */
        WS_OVERLAPPEDWINDOW,            /* window style     */
        CW_USEDEFAULT,                  /* x position       */
        CW_USEDEFAULT,                  /* y position       */
        CW_USEDEFAULT,                  /* width            */
        CW_USEDEFAULT,                  /* height           */
        (HWND)0,                        /* parent handle    */
        (HMENU)0,                       /* menu or child ID */
        hInstance,                      /* instance         */
        NULL);                          /* additional info  */

if ( !hWndMain )
   {
   MessageBox(NULL, "Unable to create main window", szAppName, MB_OK );
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }

ShowSplashScreen();

/* Let's try to find the help file first */
if (SearchPath(
    NULL,               /* address of search path               */
    szHelpFileName,     /* address of filename                  */
    NULL,               /* address of extension                 */
    PATH_MAX,           /* size, in characters, of buffer       */
    szFullPath,         /* address of buffer for found filename */
    &ptr                /* address of pointer to file component */
   ) != 0)
   {
   lstrcpy(szHelpFileName, szFullPath);
   }
#ifndef USE_UNZIP_LIB
if (SearchPath(
    NULL,               /* address of search path               */
    UNZ_DLL_NAME,       /* address of filename                  */
    NULL,               /* address of extension                 */
    PATH_MAX,           /* size, in characters, of buffer       */
    szFullPath,         /* address of buffer for found filename */
    &ptr                /* address of pointer to file component */
   ) == 0)
   {
   char str[256];
   wsprintf (str, DLL_WARNING, UNZ_DLL_NAME);
   MessageBox ((HWND)NULL, str, szAppName, MB_ICONHAND | MB_SYSTEMMODAL | MB_OK);
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }

/* First we'll check the unzip dll version information */
dwVerInfoSize =
    GetFileVersionInfoSize(szFullPath, &dwVerHnd);

if (dwVerInfoSize)
   {
   BOOL  fRet, fRetName;
   char str[256];
   LPSTR   lpstrVffInfo; /* Pointer to block to hold info */
   LPSTR lszVer = NULL;
   LPSTR lszVerName = NULL;
   UINT  cchVer = 0;

   /* Get a block big enough to hold the version information */
   hMem          = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
   lpstrVffInfo  = GlobalLock(hMem);

   /* Get the version information */
   if (GetFileVersionInfo(szFullPath, 0L, dwVerInfoSize, lpstrVffInfo))
      {
      fRet = VerQueryValue(lpstrVffInfo,
               TEXT("\\StringFileInfo\\040904E4\\FileVersion"),
              (LPVOID)&lszVer,
              &cchVer);
      fRetName = VerQueryValue(lpstrVffInfo,
               TEXT("\\StringFileInfo\\040904E4\\CompanyName"),
               (LPVOID)&lszVerName,
               &cchVer);
      if (!fRet || !fRetName ||
         (lstrcmpi(lszVer, UNZ_DLL_VERSION) != 0) ||
         (lstrcmpi(lszVerName, COMPANY_NAME) != 0))
         {
         wsprintf (str, DLL_VERSION_WARNING, UNZ_DLL_NAME);
         MessageBox((HWND)NULL, str, szAppName, MB_ICONSTOP | MB_OK);
         FreeUpMemory();
         UnloadLibraries();
         GlobalUnlock(hMem);
         GlobalFree(hMem);
         return FALSE;
         }
      }
      /* free memory */
   GlobalUnlock(hMem);
   GlobalFree(hMem);
   }
else
   {
   char str[256];
   wsprintf (str, DLL_VERSION_WARNING, UNZ_DLL_NAME);
   MessageBox (NULL, str, szAppName, MB_ICONSTOP | MB_OK);
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }
#endif /* !USE_UNZIP_LIB */

#ifndef USE_ZIP_LIB
if (SearchPath(
    NULL,               /* address of search path               */
    ZIP_DLL_NAME,       /* address of filename                  */
    NULL,               /* address of extension                 */
    PATH_MAX,           /* size, in characters, of buffer       */
    szFullPath,         /* address of buffer for found filename */
    &ptr                /* address of pointer to file component */
   ) == 0)
   {
   char str[256];
   wsprintf (str, DLL_WARNING, ZIP_DLL_NAME);
   MessageBox((HWND)NULL, str, szAppName, MB_ICONSTOP | MB_OK);
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }

/* Now we'll check the zip dll version information */
dwVerInfoSize =
    GetFileVersionInfoSize(szFullPath, &dwVerHnd);

if (dwVerInfoSize)
   {
   BOOL  fRet, fRetName;
   char str[256];
   LPSTR   lpstrVffInfo; /* Pointer to block to hold info */
   LPSTR lszVer = NULL;
   LPSTR lszVerName = NULL;
   UINT  cchVer = 0;

   /* Get a block big enough to hold the version information */
   hMem          = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
   lpstrVffInfo  = GlobalLock(hMem);

   /* Get the version information */
   GetFileVersionInfo(szFullPath, 0L, dwVerInfoSize, lpstrVffInfo);
   fRet = VerQueryValue(lpstrVffInfo,
              TEXT("\\StringFileInfo\\040904E4\\FileVersion"),
               (LPVOID)&lszVer,
               &cchVer);
   fRetName = VerQueryValue(lpstrVffInfo,
               TEXT("\\StringFileInfo\\040904E4\\CompanyName"),
              (LPVOID)&lszVerName,
              &cchVer);
   if (!fRet || !fRetName ||
      (lstrcmpi(lszVer, ZIP_DLL_VERSION) != 0) ||
      (lstrcmpi(lszVerName, COMPANY_NAME) != 0))
      {
      wsprintf (str, DLL_VERSION_WARNING, ZIP_DLL_NAME);
      MessageBox((HWND)NULL, str, szAppName, MB_ICONSTOP | MB_OK);
      FreeUpMemory();
      GlobalUnlock(hMem);
      GlobalFree(hMem);
      UnloadLibraries();
      return FALSE;
      }
   /* free memory */
   GlobalUnlock(hMem);
   GlobalFree(hMem);
   }
else
   {
   char str[256];
   wsprintf (str, DLL_VERSION_WARNING, ZIP_DLL_NAME);
   MessageBox((HWND)NULL, str, szAppName, MB_ICONSTOP | MB_OK);
   FreeUpMemory();
   GlobalUnlock(hMem);
   GlobalFree(hMem);
   UnloadLibraries();
   return FALSE;
   }
#endif /* !USE_ZIP_LIB */

#ifndef USE_UNZIP_LIB
/* Okay, now we know that the dll's exist, and have the proper version
 * information in them. We can go ahead and load them.
 */

hUnzipDll = LoadLibrary(UNZ_DLL_NAME);
if (hUnzipDll != NULL)
   {
   (_DLL_UNZIP)Unz_SingleEntryPoint =
      (_DLL_UNZIP)GetProcAddress(hUnzipDll, "Wiz_SingleEntryUnzip");
   (_DLL_UNZVAL)UnzipValidate =
      (_DLL_UNZVAL)GetProcAddress(hUnzipDll, "Wiz_Validate");
   (_DLL_UNZIP_TO_MEMORY)UnzipToMemory =
      (_DLL_UNZIP_TO_MEMORY)GetProcAddress(hUnzipDll, "Wiz_UnzipToMemory");
   (_DLL_GREP_ARCHIVE)GrepArchive =
      (_DLL_GREP_ARCHIVE)GetProcAddress(hUnzipDll, "Wiz_Grep");
   }
else
   {
   char str[256];
   wsprintf (str, GetLoadString(IDS_NO_LOAD), UNZ_DLL_NAME);
   MessageBox((HWND)NULL, str, szAppName, MB_ICONSTOP | MB_OK);
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }
#endif /* !USE_UNZIP_LIB */

#ifndef USE_ZIP_LIB
hZipDll = LoadLibrary(ZIP_DLL_NAME);
if (hZipDll != NULL)
   {
   (_DLL_ZIP)ZipArchive = (_DLL_ZIP)GetProcAddress(hZipDll, "ZpArchive");
   (ZIPSETOPTIONS)ZipSetOptions = (ZIPSETOPTIONS)GetProcAddress(hZipDll, "ZpSetOptions");
   (ZIPGETOPTIONS)ZipGetOptions = (ZIPGETOPTIONS)GetProcAddress(hZipDll, "ZpGetOptions");
   if (!ZipArchive || !ZipSetOptions || !ZipGetOptions)
      {
      char str[256];
      wsprintf (str, "Could not get entry point to %s", ZIP_DLL_NAME);
      MessageBox((HWND)NULL, str, szAppName, MB_ICONSTOP | MB_OK);
      FreeUpMemory();
      DestroyWindow(hSplashScreen);
      FreeLibrary(hRTFLib);
      return 0;
      }
   }
else
   {
   char str[256];
   wsprintf (str, GetLoadString(IDS_NO_LOAD), ZIP_DLL_NAME);
   MessageBox((HWND)NULL, str, szAppName, MB_ICONSTOP | MB_OK);
#ifndef USE_UNZIP_LIB
   FreeLibrary(hUnzipDll);
#endif   
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }

(_ZIP_USER_FUNCTIONS)ZipInit = (_ZIP_USER_FUNCTIONS)GetProcAddress(hZipDll, "ZpInit");

if (!ZipInit)
   {
   char sz1[RESOURCE_STR_LEN];
   lstrcpy(sz1, GetLoadString(IDS_NO_INIT));
   MessageBox((HWND)NULL, sz1,
      szAppName, MB_ICONSTOP | MB_OK);
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }
#endif /* !USE_ZIP_LIB */
if (!(*ZipInit)(lpZipUserFunctions))
   {
   char sz1[RESOURCE_STR_LEN];
   lstrcpy(sz1, GetLoadString(IDS_NO_INIT));
   MessageBox((HWND)NULL, sz1,
      szAppName, MB_ICONSTOP | MB_OK);
   UnloadLibraries();
   FreeUpMemory();
   return FALSE;
   }

hEditor = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, (DWORD)EDIT_BUF_SIZE);
if (!hEditor)
   {
   char sz1[RESOURCE_STR_LEN];
   lstrcpy(sz1, GetLoadString(IDS_NO_MEMORY));
   MessageBox((HWND)NULL,
              sz1,
              szAppName,
              MB_ICONSTOP | MB_OK);
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }
pszBuffer = (LPSTR)GlobalLock(hEditor);

hRootDir = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, (DWORD)PATH_MAX);
if (!hRootDir)
   {
   char sz1[RESOURCE_STR_LEN];
   lstrcpy(sz1, GetLoadString(IDS_NO_MEMORY));
   MessageBox((HWND)NULL,
              sz1,
              szAppName,
              MB_ICONSTOP | MB_OK);
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }
ZpOpt.szRootDir = (LPSTR)GlobalLock(hRootDir);

hDate = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, (DWORD)9);
if (!hDate)
   {
   char sz1[RESOURCE_STR_LEN];
   lstrcpy(sz1, GetLoadString(IDS_NO_MEMORY));
   MessageBox((HWND)NULL,
              sz1,
              szAppName,
              MB_ICONSTOP | MB_OK);
   FreeUpMemory();
   UnloadLibraries();
   return FALSE;
   }
ZpOpt.Date = (LPSTR)GlobalLock(hDate);

hEditLib = LoadLibrary(EDIT_DLL_NAME);
if (!hEditLib)
   {
   char str[256];
   wsprintf (str, "Could not load %s. Terminating", "WizEdit DLL");
   MessageBox((HWND)NULL, str, szAppName, MB_ICONSTOP | MB_OK);

   UnloadLibraries();
   FreeUpMemory();
   return FALSE;
   }
(DLL_LRESULT)DllOnIdle   = (DLL_LRESULT)GetProcAddress(hEditLib, "DllOnIdle");
(DLL_VOID)DllTranslate   = (DLL_VOID)GetProcAddress(hEditLib, "DllTranslate");
(DLL_HWND)InitDll        = (DLL_HWND)GetProcAddress(hEditLib, "InitDll");
(DLL_STREAM)StreamBuffer = (DLL_STREAM)GetProcAddress(hEditLib, "StreamBuffer");

// Initialize dll by setting up the main window.
if ((hWndEditor=(*InitDll)(hInstance, nCmdShow, hWndMain))==NULL)
   {
   char str[256];
   wsprintf (str, "Could not initialize %s. Terminating", "WizEdit DLL");
   MessageBox((HWND)NULL, str, szAppName, MB_ICONSTOP | MB_OK);

   UnloadLibraries();
   FreeUpMemory();
   return FALSE;           // Exits if unable to initialize
   }
return TRUE;
}

void ZapWizDirectories(void)
{
TCHAR drive[2];
BOOL i;
TCHAR *ptr, szFullPath[PATH_MAX], szTemp[PATH_MAX];
extern fZapTemps;
fZapTemps = 1;

/* Shut down any launched applications */
PostMessage(HWND_BROADCAST, RegisteredMessage, 0, 0L);
if (SearchPath(
    NULL,               /* address of search path               */
    "WIZ.EXE",     /* address of filename                  */
    NULL,               /* address of extension                 */
    PATH_MAX,           /* size, in characters, of buffer       */
    szFullPath,         /* address of buffer for found filename */
    &ptr                /* address of pointer to file component */
   ) == 0)
   {
   MessageBox(NULL, "Couldn't find myself", "Wiz", MB_OK);
   return;
   }
*ptr++ = '\0';
drive[0] = szFullPath[0];
drive[1] = '\0';
lstrcpy(szTemp, szFullPath);
lstrcat(szTemp, "Wiztemp1");
i = SetCurrentDirectory(szTemp);
lstrcat(szTemp, "\\*.*");
ptr = &szTemp[2];
uf.fRecurseOnSearch = 1;
if (i)
   FindFile(drive, ptr, NULL);
lstrcpy(szTemp, szFullPath);
lstrcat(szTemp, "Wiztemp2");
i = SetCurrentDirectory(szTemp);
lstrcat(szTemp, "\\*.*");
ptr = &szTemp[2];
if (i)
   FindFile(drive, ptr, NULL);
SetCurrentDirectory(szFullPath);
lstrcpy(szTemp, szFullPath);
lstrcat(szTemp, "Wiztemp1");
RemoveDirectory(szTemp);
lstrcpy(szTemp, szFullPath);
lstrcat(szTemp, "Wiztemp2");
RemoveDirectory(szTemp);
}

