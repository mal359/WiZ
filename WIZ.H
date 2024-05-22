/*
 Wiz 5.0 header file
*/

/*
Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

For the purposes of this copyright and license, "Info-ZIP" is defined as
the following set of individuals:  

   Mark Adler, John Bush, Karl Davis, Harald Denker, Jean-loup Gailly,
   Hunter Goatley, Ian Gorman, Chris Herborth, Dirk Haase, Greg Hartwig,
   Robert Heath, Jonathan Hudson, Paul Kienitz, David Kirschbaum, Johnny
   Lee, Onno van der Linden, Igor Mandrichenko, Steve P. Miller, Sergio
   Monesi, Keith Owens, George Petrov, Greg Roelofs, Kai Uwe Rommel,
   Steve Salisbury, Dave Smith, Christian Spieler, Antoine Verheijen,
   Paul von Behren, Rich Wales, Mike White

This software is provided "as is," without warranty of any kind, express
or implied.  In no event shall Info-ZIP or its contributors be held liable
for any direct, indirect, incidental, special or consequential damages
arising out of the use of or inability to use this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. Redistributions of source code must retain the above copyright notice,
       definition, disclaimer, and this list of conditions.

    2. Redistributions in binary form must reproduce the above copyright
       notice, definition, disclaimer, and this list of conditions in
       documentation and/or other materials provided with the distribution.

    3. Altered versions--including, but not limited to, ports to new operating
       systems, existing ports with new graphical interfaces, and dynamic,
       shared, or static library versions--must be plainly marked as such
       and must not be misrepresented as being the original source.  Such
       altered versions also must not be misrepresented as being Info-ZIP
       releases--including, but not limited to, labeling of the altered
       versions with the names "Info-ZIP" (or any variation thereof, including,
       but not limited to, different capitalizations), "Pocket UnZip," "WiZ"
       or "MacZip" without the explicit permission of Info-ZIP.  Such altered
       versions are further prohibited from misrepresentative use of the
       Zip-Bugs or Info-ZIP e-mail addresses or of the Info-ZIP URL(s).
*/
#ifndef _WIZ_H
#define _WIZ_H

#ifndef STRICT
   #define STRICT
#endif
// The below definition is to prevent some of the Windows 2000 code from
// trying to execute on earlier versions of Win32. Structures such as
// OPENFILENAME have increased in size on Windows 2000.
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <windowsx.h>
#include <assert.h>    /* required for all Windows applications */
#include <stdlib.h>
#include <stdio.h>
#include <commdlg.h>
#include <dlgs.h>

#define NOTREEVIEW
#define NOIMAGEAPIS

#include <commctrl.h>  /* for ListView controls */
#include <richedit.h>  /* for RichEdit controls */

#ifndef UzpMatch
#define UzpMatch match
#endif

#include "cmds.h"
#include "resource.h"
#include "unzip\windll\structs.h"
#include "zip\windll\structs.h"

#ifdef USE_UNZIP_LIB
#include "unzip\windll\decs.h"
#endif


/***********************************************************************
 *
 * Global Defines
 *
 ***********************************************************************/
#ifndef MSWIN
   #define MSWIN
#endif
#ifndef USE_UNZIP_LIB
   #define UNZ_DLL_NAME "UNZIP32.DLL\0"
#endif
#ifndef USE_ZIP_LIB
#define ZIP_DLL_NAME "ZIP32.DLL\0"
   #endif
#define EDIT_DLL_NAME "WIZEDIT.DLL\0"

#ifdef USE_UNZIP_LIB
   #define UnzipValidate Wiz_Validate
   #define Unz_SingleEntryPoint Wiz_SingleEntryUnzip
   #define GrepArchive Wiz_Grep
#endif

#ifdef USE_ZIP_LIB
   #define ZipInit ZpInit
   #define ZipArchive ZpArchive
   #define ZipSetOptions ZpSetOptions
   #define ZipGetOptions ZpGetOptions
#endif

#define MAXFILTERBUF 50
// The max length of resource strings
#define RESOURCE_STR_LEN 300
#define CRLF_BUF_SIZE 0x200
#define FPRINTF_BUF_SIZE 0x4000
/* Define size of read/write buffer for copying files */
#define READ_WRITE_BUF 4096

/* For checking rtf file internal signatures */
#define cchRTFSig 5

#define EDIT_BUF_SIZE     0x8001L

/* Arbitrary Constants
 */
#define BG_SYS_COLOR COLOR_GRAYTEXT /* background color is a system color */

/* Help Window Menu and Message ID's
 */
#define INDEX_MENU_ITEM_POS       0

#ifndef NDEBUG
#  define WinAssert(exp) \
        {\
        if (!(exp))\
            {\
            char szBuffer[40];\
            sprintf(szBuffer, "File %s, Line %d",\
                    __FILE__, __LINE__) ;\
            if (IDABORT == MessageBox((HWND)NULL, szBuffer,\
                "Assertion Error",\
                MB_ABORTRETRYIGNORE|MB_ICONSTOP))\
                    FatalExit(-1);\
            }\
        }
#else
#  define WinAssert(exp)
#endif

#define MAKEBOOL(i) ((BOOL) !!(i))

#define cchFilesMax 4096

/***********************************************************************
 *
 * Flags
 *
 ***********************************************************************/
extern BOOL fNotifyClearing;
extern BOOL fExternalRTF;
extern BOOL fMakeDosSFX;
extern BOOL fDoingArchives;
extern BOOL bUseCurrentZip;
extern BOOL fUseOldDirSelect;
/***********************************************************************
 *
 * Typedefs
 *
 ***********************************************************************/
/* For rich edit control find/replace functions */
typedef enum {
    frtFind,
    frtReplace,
    frtRepAll
} FRT;  // Find/Replace tag

// structure defining what is passed to Uzp_SingleEntryPoint
typedef struct
{
int argc;
char **argv;
int xfnc;
char **xfnv;
DCL *lpDCL;
USERFUNCTIONS *lpUserFunctions;
} PARAMETERS;

// lpumb structure passed to unzip dll
typedef struct
{
char szFileName[PATH_MAX];          // fully-qualified archive file name in OEM char set
char szZipFileName[PATH_MAX];       // fully-qualified archive file name in OEM char set
char szDirName[PATH_MAX];           // directory of archive file in ANSI char set
char szUnzipToDirNameTmp[PATH_MAX]; // temp extraction ("unzip to") directory name in ANSI
char szUnzipFromDirName[PATH_MAX];  // extraction ("unzip from") directory name in ANSI
char szBuffer[PATH_MAX];            // option strings from .INI, & gen'l scratch buf
char szSoundName[PATH_MAX];         // wave file name or sound from .INI [sounds] in ANSI
char szPassword[81];
LPSTR lpPasswordStr;
OPENFILENAME ofn;
OPENFILENAME wofn;                  // wave open file name struct
MSG msg;
OFSTRUCT of;                        // archive open file struct
OFSTRUCT wof;                       // wave open file struct
} UMB, *LPUMB;

/* structure of unzip flags */
typedef struct
{
BOOL fRecreateDirs;
BOOL fTranslate;
BOOL fRecurseOnSearch;
BOOL fBeepOnFinish;
BOOL fDoAll;
BOOL fHelp;
BOOL fCanDragDrop;
BOOL fTrailingSlash;
BOOL fPromptToOverwrite;
BOOL fCaseSensitive;
BOOL fWholeWords;
BOOL fIgnoreBinary;
BOOL fRescan;
} UF, *PUF;

typedef struct {               /* rxstr */
    ulg   strlength;           /* length of string */
    char  *strptr;             /* pointer to string */
    USERFUNCTIONS far* lpUserFunctions; /* Pointer to structure of functions */
    int   NoSensitivity;       /* True if ignoring case sensitivity of
                                  filenames */
} UZPBUFFER;


/* typedef's for calls into the unzip dll, not used for the library version */
#ifndef USE_UNZIP_LIB
typedef int   (WINAPI * _DLL_UNZIP)(int, char **, int, char **, LPDCL, LPUSERFUNCTIONS);
typedef int   (WINAPI * _USER_FUNCTIONS)(LPUSERFUNCTIONS);
typedef int   (WINAPI * _DLL_UNZIP_TO_MEMORY)(char *,char *,UZPBUFFER *, USERFUNCTIONS far*, int);
typedef int   (WINAPI * _DLL_GREP_ARCHIVE)(char *, char *, char *, int, int, USERFUNCTIONS far*);
typedef BOOL  (WINAPI * _DLL_UNZVAL)(char *, int);
#endif

#ifndef USE_ZIP_LIB
/* typedef's for calls into the zip dll, not used for the library version */
typedef int   (WINAPI * _DLL_ZIP)(ZCL);
typedef int   (WINAPI * _ZIP_USER_FUNCTIONS)(LPZIPUSERFUNCTIONS);
typedef BOOL  (WINAPI * ZIPSETOPTIONS)(LPZPOPT);
typedef ZPOPT (WINAPI * ZIPGETOPTIONS)(void);
#endif

/* typedef's for calls into the editor dll */
typedef LRESULT (WINAPI * DLL_LRESULT)(void);
typedef void (WINAPI * DLL_VOID)(MSG);
typedef HWND (WINAPI * DLL_HWND)(HINSTANCE, int, HWND);
typedef BOOL (WINAPI * DLL_STREAM)(LPSTR, LPSTR);

/***********************************************************************
 *
 * Global Variables
 *
 ***********************************************************************/
extern RECT MainRc;        /* Used for saving and restoring app position */
extern HWND hWndGrep;      /* Grep results window */
extern HWND hWndListParent;/* Archive display window handle */
extern HWND hWndMessage;   /* Message window */
extern HWND hWndComment;   /* comment window */
extern HWND hTabCtrl;      /* Tab Control window handle */
extern HWND hWndMain;      /* the main window handle. */
extern HWND hGetFilesDlg;
extern HWND hProgressWnd;
extern HWND hProgressBar;
extern HWND hWndEditor;    /* Dll edit window */
extern HWND hWndArchivesFound;
extern HWND hStatusBar;
extern HWND hWndToolbar;
extern HWND hPatternSelectDlg; /* pattern select modeless dialog */
extern HANDLE hFixedFont;
extern HANDLE hAccTable;
extern HANDLE hHourGlass;  /* handle to hourglass cursor */
extern HANDLE hSaveCursor; /* current cursor handle */
extern HANDLE hFileList;
//MW extern HANDLE hHelpCursor; /* help cursor */
extern HANDLE hFixedFont;  /* handle to fixed font */
extern HANDLE hOldFont;    /* handle to old font */
extern HANDLE hListbox;
extern HANDLE hEditor;
extern HANDLE hRootDir;
extern HINSTANCE hInst;    /* current instance */
extern HINSTANCE hUnzipDll;
extern HINSTANCE hZipDll;
extern HINSTANCE hExplorer; // Instance of Windows Explorer
extern HBRUSH hBrush ;          /* brush for  standard window backgrounds  */
extern HMENU  hMenu;           /* main menu handle */
extern WPARAM wLBSelection;     /* default listbox selection action */
extern WPARAM wWindowSelection; /* window selection: listbox, status, both   */
extern char **pszIndex;
extern char *sz;
extern char szTargetDirName[];
extern char szNoMatch[];
extern char szCantChDir[];
extern char szCantCopyFile[];
extern char szAppName[];     /* application name             */
extern char szDefaultUnzipToDir[]; /* default Unz_SingleEntryPoint to dir */
extern char szDefaultUnzipFromDir[]; /* default Unz_SingleEntryPoint from dir */
extern char szNoMemory[] ;       /* error message            */
extern char szHelpFileName[];        /* help file name                       */
extern char szTargetZipDir[];
extern char szUnzipToDirName[];    /* extraction ("Unz_SingleEntryPoint to") directory name in ANSI */
extern char * Headers[];        /* headers to display           */
extern char *lpEditBuf;
extern char szExplorer[];
extern char rgszFiles[];
extern char szClearBufferMsg[];
extern char szRefDir[];
extern char szCantMakeZip[];
extern char DELIMS[]; /* delimiters, mostly whitespace */
extern PSTR psLBEntry;    /* list box entry */
extern PSTR psPatternBuf; /* pattern collection */
extern PSTR psGrepBuf;    /* pattern collection */
extern UINT uMessage;   /* Unique message identifier for find/replace */
extern UINT uCommDlgHelpMsg;   /* common dialog help message ID */
extern UINT cOpen; /* Number of archive windows open */
extern LPSTR lpchLast;
extern LPSTR szFileList;
extern LPSTR szRootDir;
extern LPSTR pszBuffer;
extern TCHAR SearchPattern[];
extern TCHAR szTempDirPath[];
extern TCHAR baseDir[];
extern TCHAR szDateString[];
extern TCHAR szWindowsTempDir[];
extern TCHAR szRegKey[];
extern TCHAR szArchive1[];
extern TCHAR szArchive2[];
extern TCHAR szHomeDir[];
extern HWND hArchive1;
extern HWND hArchive2;
extern int iGlobalCancel;
extern int iGlobalReturnCode;
extern int hFile;          /* file handle */
extern int fLB_Selection;
extern int NumberTempFiles;
extern int fDisplay;      /* Display flag.
                             0 = send to WizEdit
                             1 = archive, display ourselves
                             2 = Wiz unable to display, let Windows
                                 try.
                           */
extern long dxChar, dyChar;    /* size of char in SYSTEM font in pixels    */
extern unsigned long dFilesToProcess;
//MW extern DWORD dwHelpContextId;
//MW extern DWORD dwCommDlgHelpId;   /* what to pass to WinHelp() */
extern DWORD dwPlatformId;      /* True if running on NT */
extern DWORD dwFormat;
extern DWORD dwEditBufSize;
extern WORD cZippedFiles;     /* total files in archive */
extern WORD wGlobalMessage;
extern BOOL fExecutedFile;    /* True if files have been executed from
                                 within an archive */
extern BOOL fMakeSFX;
extern BOOL fGreppingArchives;
extern BOOL fSpool;
extern BOOL fScanArchives;
extern BOOL move_flag;
extern BOOL rename_flag;
extern BOOL fUpdateEntries;
extern BOOL bRealTimeMsgUpdate; /* update messages window in real-time */
extern OPENFILENAME ofnTemp;
extern BOOL fDiffError;

// typedef'd globals
extern LPUSERFUNCTIONS lpUserFunctions;
extern LPZIPUSERFUNCTIONS lpZipUserFunctions;
extern LPUMB lpumb;
extern LPDCL lpDCL;
extern PARAMETERS uzpParameters;
extern UF uf;
extern ZCL ZpZCL;
extern ZPOPT ZpOpt;

/***********************************************************************
 *
 * Function declarations
 *
 ***********************************************************************/
/* Unzip dll function declarations */
#ifndef USE_UNZIP_LIB
extern _DLL_UNZIP_TO_MEMORY UnzipToMemory;
extern _DLL_UNZIP Unz_SingleEntryPoint;
extern _DLL_UNZVAL UnzipValidate;
extern _DLL_GREP_ARCHIVE GrepArchive;
extern _USER_FUNCTIONS UzInit;
#endif

/* Zip dll function declarations */
#ifndef USE_ZIP_LIB
extern _DLL_ZIP ZipArchive;
extern _ZIP_USER_FUNCTIONS ZipInit;
extern ZIPSETOPTIONS ZipSetOptions;
extern ZIPGETOPTIONS ZipGetOptions;
#endif

/* Editor function declarations */
extern DLL_LRESULT DllOnIdle;
extern DLL_VOID DllTranslate;
extern DLL_HWND InitDll;
extern DLL_STREAM StreamBuffer;

BOOL FRestoreWindowPos(RECT *);
BOOL TabInit(HWND);
BOOL WizInit(HANDLE);
BOOL CreateButtonBar(HWND);
BOOL FnMatch(char *, char *);
BOOL GetWindowHandles(LPSTR, LPSTR);
BOOL DiffArchives(void);
BOOL GetUnzipDirectory(void);
int IsValidArchive(LPSTR);
int __cdecl BufferOut(const char *format, ...);
int CLBItemsGet(HWND, int * *, HANDLE *);
int match (const char *, const char *, int i);          /* in unzip\match.c */
int WINAPI GetReplaceDlgRetVal(char *);
int WINAPI DisplayBuf(char *, unsigned long int);
int WINAPI password(LPSTR, int, LPCSTR, LPCSTR);
int WINAPI comment(LPSTR);
int MapCursorToListItem(HWND);
int DoPropertySheet(HWND);
long WINAPI WizMainWndProc(HWND, WORD, WPARAM, LPARAM);
LPSTR lstrrchr(LPSTR, char);
LPSTR GetLoadString(UINT);
LRESULT WINAPI ArchiveWndProc(HWND, WORD, WPARAM, LPARAM);
LRESULT MsgTimer(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgNotify(HWND, UINT, WPARAM, LPARAM);
LRESULT ToolBarNotify(HWND, WORD, WORD, HWND);
void DisplayFiles(HWND, BOOL);
void EmptyMessageQueue(void);
void IsArchiveOpen(LPSTR);
void Action(HWND, int);
void CenterDialog(HWND, HWND);
void CleanScreen(void);
void CopyStatusToClipboard(HWND);
void DisplayComment(void);
void ReselectLB(HWND, int, int *);
void InitSoundOptions(void); /* initialize sound options (sound.c)   */
void MigrateSoundOptions(void); /* translate beep into new option (sound.c) */
void SizeWindow(void);
void SoundAfter(void);
void WINAPI SoundDuring(void);
void UpdateButtons(void);
void UpdateListBox(void);
void UpdateMsgWndPos(void);
void WriteBufferToMsgWin(LPSTR, int, BOOL);
void WriteStringToMsgWin(PSTR, BOOL);
void CopyArchive(HWND, BOOL, BOOL);
void GetDirectory(LPSTR);
void MoveButtons(void);
void FindFile(char *, char *, HWND);
void DoADir(char *, char *, char *, HWND, char);
void WriteZipOptionsProfile(void);
void GetWizOptions(void);
void StripDirectory(LPSTR);
void MakeArchive(HWND hWnd);
void SearchArchive(LPSTR);
void ListViewSetSel(int, BOOL);  /* Only used for ListView control */
void ListViewSortOnColumns(int); /* Only used for ListView sorting */
void GetArchiveDir(LPSTR);
void UpdateStatusBar(LPSTR, WORD, WORD);
void UpdateArchiveStatusBar(LPSTR, WORD, WORD);
void InitializeStatusBar(HWND);
void InitializeArchiveStatusBar(HWND);
// MW void GetHelpContext(WPARAM wParam);
void DisplaySelectionTotals(void);
void ProgressBar(void);
void GetCapitalization(TCHAR *);

/***********************************************************************
 *
 * Procedure calls
 *
 ***********************************************************************/
BOOL WINAPI DiffArchiveProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI GrepPatternSelectProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI AboutProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI OrderProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI RegistrationProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI SelectDirProc(HWND, WORD, WPARAM, LPARAM);
//#if 0
BOOL WINAPI CopyFileProc(HWND, WORD, WPARAM, LPARAM);
//#endif
BOOL WINAPI PatternSelectProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI ServiceProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI ReplaceProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI SoundProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI RenameProc (HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI MakeDirProc(HWND, WORD, WPARAM, LPARAM);
BOOL CALLBACK CommentBoxProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI GetFilesProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI GetDirProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI GetDateProc(HWND, WORD, WPARAM, LPARAM);
BOOL WINAPI GrepArchiveProc(HWND, WORD, WPARAM, LPARAM);
DWORD WINAPI UnZipPreferencesProc(HWND, WORD, WPARAM, LPARAM);
DWORD WINAPI ZipPreferencesProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI ServiceQueue(LPCSTR, unsigned long);
void WINAPI ReceiveDllMessage(unsigned long,unsigned long,
   unsigned, unsigned, unsigned, unsigned, unsigned, unsigned,
   char, LPSTR, LPSTR, unsigned long, char);
#endif 


/* _WIZ_H */

