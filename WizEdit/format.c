// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995 Microsoft Corporation.  All Rights Reserved.
//
// Modifications Copyright (C) 1997  Mike White
//
//  MODULE:   format.c
//
//  PURPOSE:  Implement the editor formatting functions
//
//  FUNCTIONS:
//    CmdBold            - Applys/Removes bold formatting to selected text
//    CmdItalic          - Applys/Removes italic formatting to selected text
//    CmdUnderline       - Applys/Removes underline formatting to selected text
//    CmdFontDialog      - Lets user change text with Common Font dialog
//    CmdIncreaseFont    - Increase fonts size of selected text 2 points
//    CmdDecreaseFont    - Decrease fonts size of selected text 2 points
//
//
//  COMMENTS:
//    Message dispatch table -
//      For every message to be handled by the main window procedure
//      place the message number and handler function pointer in
//      rgmsd (the message dispatch table).  Place the prototype
//      for the function in globals.h and the definition of the
//      function in the appropriate module.
//    Command dispatch table -
//      For every command to be handled by the main window procedure
//      place the command number and handler function pointer in
//      rgcmd (the command dispatch table).  Place the prototype
//      for the function in globals.h and the definition of the
//      function in the appropriate module.
//    Globals.h Contains the definitions of the structures and dispatch.c
//      contains the functions that use these structures.
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>

#define NOTREEVIEW
#define NOIMAGEAPIS

#include <commctrl.h>
#include <richedit.h>
#include "..\wizedit\globals.h"            // prototypes specific to this application
#include "..\wizedit\resource.h"
#include "..\wizedit\mdichild.h"
#include "..\wizedit\toolbar.h"
#include "..\wizedit\statbar.h"
#include "..\wizedit\rtf.h"

//
//  FUNCTION: IndentFirst(BOOL)
//
//  PURPOSE: To indent or outdent selected text
//
//  PARAMETERS:
//    fIndent - If TRUE, then indent, else outdent
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//
VOID IndentFirst(BOOL fIndent)
{
   PARAFORMAT pf;
   HWND hTemp;

   pf.cbSize = sizeof(PARAFORMAT);

   if ((hTemp = GetEditWindow(NULL)) == NULL)
      return;
   WinAssert(hTemp);
   SendMessage(hTemp, EM_GETPARAFORMAT, 0, (LPARAM) &pf);

   pf.dwMask = PFM_OFFSET | PFM_OFFSETINDENT;
   if(fIndent)
   {
      if(pf.dxOffset < 0)
      {
         pf.dxStartIndent = pf.dxOffset;
         pf.dxOffset = 0;
      }
      else
      {// lDefaultTab is defined in richedit.h as 720
         pf.dxOffset = pf.dxOffset ? -pf.dxOffset : -lDefaultTab;
         pf.dxStartIndent = -pf.dxOffset;
      }
   }
   else
   {
      if(pf.dxOffset < 0)
      {
         pf.dxStartIndent = pf.dxOffset;
         pf.dxOffset = -pf.dxOffset;
      }
      else
      {// lDefaultTab is defined in richedit.h as 720
         pf.dxStartIndent = 0;
         pf.dxOffset = pf.dxOffset ? 0 : lDefaultTab;
      }
   }

   SendMessage(hTemp, EM_SETPARAFORMAT, 0, (LPARAM) &pf);
}

//
//  FUNCTION: CmdIndent(HWND, WORD, WORD, HWND)
//
//  PURPOSE: To indent selected text
//
//  PARAMETERS:
//    fIndent - If TRUE, then indent, else outdent
//
//  RETURN VALUE:
//    return TRUE, always handled
//
//  COMMENTS:
//
#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdIndent(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
IndentFirst(TRUE);
return TRUE;
}

//
//  FUNCTION: CmdOutdent(HWND, WORD, WORD, HWND)
//
//  PURPOSE: To outdent selected text
//
//  PARAMETERS:
//    None used
//
//  RETURN VALUE:
//    Return TRUE, command always handled
//
//  COMMENTS:
//
#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdOutdent(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
IndentFirst(FALSE);
return TRUE;
}

//
//  FUNCTIONS: CmdBold(HWND, WORD, WORD, HWND)
//             CmdItalic(HWND, WORD, WORD, HWND)
//             CmdUnderline(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Apply/Remove Bold, Italic, or underline attributes
//           to selected text.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDM_EDITBOLD, IDM_EDITITALIC, IDM_EDITUNDERLINE
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdBold(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HWND hTemp;

    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return 0L;
    WinAssert(hTemp);
    RTF_ChangeCharAttribute(hTemp, CFM_BOLD, CFE_BOLD);
    RTF_ShowCharAttributes();
    return 0L;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdItalic(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HWND hTemp;

    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return 0L;
    WinAssert(hTemp);
    RTF_ChangeCharAttribute(hTemp, CFM_ITALIC, CFE_ITALIC);
    RTF_ShowCharAttributes();
    return 0L;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdUnderline(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HWND hTemp;

    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return 0L;
    RTF_ChangeCharAttribute(hTemp, CFM_UNDERLINE, CFE_UNDERLINE);
    RTF_ShowCharAttributes();
    return 0L;
}

//
//  FUNCTION: CmdFontDialog(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Bring up Common FOnt Dialog to let User change
//           any attributes they want on the selected text's font.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDM_EDITFONTDIALOG
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdFontDialog(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HWND hTemp;

    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return 0L;
    WinAssert(hTemp);
    RTF_ChangeFont(GetParent(hTemp), hTemp);
    RTF_ShowCharAttributes();
    return 0L;
}

//
//  FUNCTIONS: CmdIncreaseFont(HWND, WORD, WORD, HWND)
//             CmdDecreaseFont(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Increase or Decrease the selected text's font size
//           by 2 points.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDM_EDITINCREASEFONT, IDM_EDITDECREASEFONT
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdIncreaseFont(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HWND hTemp;

    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return 0L;
    WinAssert(hTemp);
    RTF_ChangeSizeAttribute(hTemp,1); // Increase 1 point
    return 0L;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdDecreaseFont(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HWND hTemp;

    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return 0L;
    WinAssert(hTemp);
    RTF_ChangeSizeAttribute(hTemp,-1); // Decrease 1 point
    return 0L;
}

//
//  FUNCTIONS: CmdSuperscript(HWND, WORD, WORD, HWND)
//             CmdSubscript(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Turn on or off superscripting and subscripting
//
//  PARAMETERS:
//    None used
//
//  RETURN VALUE:
//    Always 0 - command handled
//
//  COMMENTS:
//
//
#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdSuperscript(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    SetScripting(TRUE);
    return 0L;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdSubscript(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    SetScripting(FALSE);
    return 0L;
}

//
//  FUNCTIONS: SetScripting(BOOL)
//
//  PURPOSE: Turn on or off superscripting and subscripting
//
//  PARAMETERS:
//    fSuperscript - TRUE if to superscript, FALSE if to subscript
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//
//

VOID SetScripting(BOOL fSuperscript)
{
   CHARFORMAT cf;
   HWND hTemp;

   cf.cbSize = sizeof(CHARFORMAT);
   if ((hTemp = GetEditWindow(NULL)) == NULL)
      return;
   WinAssert(hTemp);
   SendMessage(hTemp, EM_GETCHARFORMAT, TRUE, (LPARAM) &cf);

   cf.dwMask = CFM_OFFSET;         // only change the yOffset
   if(fSuperscript)
   {
      if(cf.yOffset > 0)
         cf.yOffset = 0;
      else
         cf.yOffset = 50;
   }
   else
   {
      if(cf.yOffset < 0)
         cf.yOffset = 0;
      else
         cf.yOffset = -50;
   }

   SendMessage(hTemp, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
}

//
//  FUNCTIONS: CmdAlignLeft(HWND, WORD, WORD, HWND)
//             CmdAlignRight(HWND, WORD, WORD, HWND)
//             CmdAlignCenter(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Set edit control alignment
//
//  PARAMETERS:
//    None used
//
//  RETURN VALUE:
//    0 - Always handles
//
//  COMMENTS:
//
//
#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdAlignLeft(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    SetAlignment(PFA_LEFT);
    return 0L;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdAlignRight(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    SetAlignment(PFA_RIGHT);
    return 0L;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
LRESULT CmdAlignCenter(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    SetAlignment(PFA_CENTER);
    return 0L;
}

//
//  FUNCTIONS: SetAlignment(WORD)
//
//  PURPOSE: Set edit control alignment
//
//  PARAMETERS:
//    wAlignment - PFA_LEFT for left alignment
//               - PFA_RIGHT for right alignment
//               - PFA_CENTER for center alignment
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//
//

VOID SetAlignment(WORD wAlignment)
{
      PARAFORMAT pf;
      HWND hTemp;

      if ((hTemp = GetEditWindow(NULL)) == NULL)
         return;

      WinAssert(hTemp);
      pf.cbSize = sizeof(PARAFORMAT);

      SendMessage(hTemp, EM_GETPARAFORMAT, 0, (LPARAM) &pf);

      if(!(pf.dwMask & PFM_ALIGNMENT) || pf.wAlignment != wAlignment)
      {
         pf.dwMask = PFM_ALIGNMENT;      // only change the alignment
         pf.wAlignment = wAlignment;
         SendMessage(hTemp, EM_SETPARAFORMAT, 0, (LPARAM) &pf);
      }
}

