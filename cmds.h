/* Wiz header file for command processing
 *
 * Processes commands for the main window procedures
 *
 * Author Mike White
 */

/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#ifndef _CMDS_H
#define _CMDS_H

//#define hwndMDIClient NULL        /* (1) Stub for NON-MDI applications. */
extern HWND hwndMDIClient;     /* (2) For MDI applications.          */

//-------------------------------------------------------------------------
// Message and command dispatch infrastructure.  The following type
// definitions and functions are used by the message and command dispatching
// mechanism and do not need to be changed.

// Function pointer prototype for message handling functions.
typedef LRESULT (*PFNMSG)(HWND,UINT,WPARAM,LPARAM);

// Function pointer prototype for command handling functions.
typedef LRESULT (*PFNCMD)(HWND,WORD,WORD,HWND);

// Enumerated type used to determine which default window procedure
// should be called by the message- and command-dispatching mechanism
// if a message or command is not handled explicitly.
typedef enum
{
edwpNone,            // Do not call any default procedure.
edwpWindow,          // Call DefWindowProc.
edwpDialog,          // Call DefDlgProc (This should be used only for
                     // custom dialogs - standard dialog use edwpNone).
edwpMDIChild,        // Call DefMDIChildProc.
edwpMDIFrame         // Call DefFrameProc.
} EDWP;              // Enumeration for Default Window Procedures

// This structure maps messages to message handling functions.
typedef struct _MSD
{
UINT   uMessage;
PFNMSG pfnmsg;
} MSD; // Message Dispatch structure

// This structure contains all of the information that a window
// procedure passes to DispMessage in order to define the message
// dispatching behavior for the window.
typedef struct _MSDI
{
int  cmsd;             // Number of message dispatch structs in rgmsd
MSD *rgmsd;            // Table of message dispatch structures
EDWP edwp;             // Type of default window handler needed.
} MSDI, FAR *LPMSDI;   // MeSsage Dipatch Information

// This structure maps command IDs to command handling functions.
typedef struct _CMD
{
WORD   wCommand;
PFNCMD pfncmd;
} CMD;                 // Command Dispatch structure

// This structure contains all of the information that a command
// message procedure passes to DispCommand in order to define the
// command dispatching behavior for the window.
typedef struct _CMDI
{
int  ccmd;             // Number of command dispatch structs in rgcmd
CMD *rgcmd;            // Table of command dispatch structures
EDWP edwp;             // Type of default window handler needed.
} CMDI, FAR *LPCMDI;   // CoMmand Dispatch Information

// Message and command dispatching functions.  They look up messages
// and commands in the dispatch tables and call the appropriate handler
// function.
LRESULT DispMessage(LPMSDI, HWND, UINT, WPARAM, LPARAM);
LRESULT DispCommand(LPCMDI, HWND, WPARAM, LPARAM);

// Message dispatch information for the main window
extern MSDI msdiMain;
// Command dispatch information for the main window
extern CMDI cmdiMain;

extern CMD rgcmd[];

//-------------------------------------------------------------------------
// Global function prototypes.

// **TODO**  Add global function prototypes here.

BOOL CreateArchiveControl(HWND);
BOOL CenterWindow(HWND, HWND);
BOOL InitMDIChild(HINSTANCE);
HWND GetArchiveWindow(HWND);
HWND GetStatusWindow(HWND);
VOID Open(LPSTR, BOOL, HWND);                               // File
VOID SaveAs(LPSTR, LPSTR, HWND);
BOOL CreateMDIChild(LPSTR);
HWND GetActiveMDIChild(VOID);

char *GetFName(VOID);

extern char szFName[256];

//-------------------------------------------------------------------------
// Functions for handling main window messages.  The message-dispatching
// mechanism expects all message-handling functions to have the following
// prototype:
//
//     LRESULT FunctionName(HWND, UINT, WPARAM, LPARAM);

// **TODO**  Add message-handling function prototypes here.  Be sure to
//           add the function names to the main window message table in
//           wndproc.c.

LRESULT MsgSysCommand           (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgSetCursor            (HWND, UINT, WPARAM, LPARAM);
//MW LRESULT MsgInitMenu             (HWND, UINT, WPARAM, LPARAM);
//MW LRESULT MsgEnterIdle            (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgClose                (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDropfiles            (HWND, UINT, WPARAM, LPARAM);

LRESULT MsgMenuSelect           (HWND, UINT, WPARAM, LPARAM);         // Search
LRESULT MsgCommand              (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgCreate               (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDestroy              (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgSize                 (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgTimer                (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgRButtonUp            (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgActivate             (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgNotify               (HWND, UINT, WPARAM, LPARAM);


//-------------------------------------------------------------------------
// Functions for handling main window commands--ie. functions for
// processing WM_COMMAND messages based on the wParam value.
// The message-dispatching mechanism expects all command-handling
// functions to have the following prototype:
//
//     LRESULT FunctionName(HWND, WORD, WORD, HWND);

// **TODO**  Add message-handling function prototypes here.  Be sure to
//           add the function names to the main window command table

LRESULT CmdSearchFindReplace    (HWND, WORD, WORD, HWND);  // Search
LRESULT CmdSearchFindNext       (HWND, WORD, WORD, HWND);
LRESULT CmdCreate               (HWND, WORD, WORD, HWND);
LRESULT CmdTimer                (HWND, WORD, WORD, HWND);
LRESULT CmdSetFocus             (HWND, WORD, WORD, HWND);
LRESULT CmdActivate             (HWND, WORD, WORD, HWND);
LRESULT CmdSize                 (HWND, WORD, WORD, HWND);
LRESULT CmdCommand              (HWND, WORD, WORD, HWND);
LRESULT CmdDestroy              (HWND, WORD, WORD, HWND);
LRESULT CmdDefault              (HWND, WORD, WORD, HWND);
LRESULT CmdWindowTileHorizontal (HWND, WORD, WORD, HWND);
LRESULT CmdWindowTileVertical   (HWND, WORD, WORD, HWND);
LRESULT CmdWindowCascade        (HWND, WORD, WORD, HWND);
LRESULT CmdWindowIcons          (HWND, WORD, WORD, HWND);
LRESULT CmdWindowCloseAll       (HWND, WORD, WORD, HWND);
LRESULT CmdArchive              (HWND, WORD, WORD, HWND);

LRESULT CALLBACK MDIChildWndProc(HWND, UINT, WPARAM, LPARAM);

#endif

