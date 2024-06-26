// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995 Microsoft Corporation.  All Rights Reserved.
//
// Modifications Copyright (C) 1997  Mike White
//
//  MODULE:   rtf.c
//
//  PURPOSE:  Provide an interface to the RichEdit control
//
//  FUNCTIONS:
//    RTF_ChangeCharAttribute -  To set the selected text to 
//                               have the supplied attributes.
//    RTF_GetCharAttribute -     To determine if the selected 
//                               text has the requested attributes
//    RTF_ChangeSizeAttribute -  Change the point size of the selected text
//    RTF_ChangeFont -           Change the attributes of the selected text 
//                               with the Common Font Dialog.
//    SaveCallback -             Callback routine for the 
//                               EM_STREAMOUT Message from 
//                               the RichEdit control.
//    RTF_Save -                 Save the contents of the Richedit 
//                               control to a file       
//    OpenCallback -             Callback routine for the EM_STREAMIN 
//                               Message from the RichEdit control.
//    RTF_Open -                 Load the contents of a file ito the 
//                               Richedit control
//    RTF_GetCurrentFont -       Get the facename of the selected text's font
//    RTF_SetCurrentFont -       Set the facename of the selected text's font
//    RTF_ChangeMargins -        Set the margins of the current paragraph.
//    RTF_ShowMargins -          Get the margins of the current paragraph 
//                               and reflect them on the ruler.
//    SetToolbarButtonToReflectCharacterAttribute - Show the status of an 
//                                                  attribute on a toolbar button
//    RTF_ShowCharAttributes -   Show the status of the bold, itailc, 
//                               and underline attributes on the toolbar
//
//  COMMENTS:
//

 
#include <windows.h>     // required for all Windows applications
#include <windowsx.h>
#include <malloc.h>

#define NOTREEVIEW
#define NOIMAGEAPIS

#include <commctrl.h>
#include <richedit.h>
#include "..\wizedit\globals.h"     // prototypes specific to this application
#include "..\wizedit\resource.h"
#include "..\wizedit\rtf.h"         // prototypes specific to this application
#include "..\wizedit\ruler.h"
#include "..\wizedit\toolbar.h"

const TCHAR szRTFSig[] = TEXT("{\\rtf");

//
//  FUNCTION: RTF_ChangeCharAttribute(HWND, DWORD, DWORD)
//
//  PURPOSE: To set the selected text to have the supplied
//           attributes.
//
//  PARAMETERS:
//    hRTFWnd - Handle to RichEdit control
//    dwMask - Mask of which attributes to change
//    dwEffects - State of each attribute
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//
//

void RTF_ChangeCharAttribute(HWND hRTFWnd, DWORD dwMask, DWORD dwEffects)
{
    CHARFORMAT cf; // This is defined in RICHEDIT.H

    WinAssert(hRTFWnd);
    cf.cbSize = sizeof(cf);

    // Get the current status of the text into the CHARFORMAT structure,
    // use the selected text. wParam is TRUE to indicate the
    // selected textt, FALSE for the first character in the
    // RTF Edit control.
    SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);

    // Set the mask to tell the RTF Edit control to pay attention to
    // the appropriate bit of the dwEffects field.
    cf.dwMask      = dwMask;
    // Twiddle de bits, captain!
    cf.dwEffects  ^= dwEffects;

    // Set the new underline status to the selected text
    SendMessage(hRTFWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

//
//  FUNCTION: RTF_GetCharAttribute(HWND, DWORD, DWORD)
//
//  PURPOSE: To determine if the selected text has the requested
//           attributes
//
//  PARAMETERS:
//    hRTFWnd - Handle to RichEdit control
//    dwMask - Mask of which attributes to check
//    dwEffects - State of each attribute
//
//  RETURN VALUE:
//    One of three constants:
//
//      CHAR_ATTRIB_YES - All of the selected text has the attribute
//      CHAR_ATTRIB_NO  - None of the selected text has the attribute
//      CHAR_ATTRIB_WAFFLE - Some, but not all of the selected text
//                           has the attribute
//
//  COMMENTS:
//
//

int RTF_GetCharAttribute(HWND hRTFWnd, DWORD dwMask, DWORD dwEffects)
{
    CHARFORMAT cf; // This is defined in RICHEDIT.H

    WinAssert(hRTFWnd);
    cf.cbSize = sizeof(cf);

    // Get the current status of the text into the CHARFORMAT structure,
    // use the selected text. wParam is TRUE to indicate the
    // selected text, FALSE for the first character in the
    // RTF Edit control.
    SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);

    if (cf.dwMask & dwMask)
    {
        if (cf.dwEffects & dwEffects)
            return CHAR_ATTRIB_YES;
        else
            return CHAR_ATTRIB_NO;
    }
    else
    {
        return CHAR_ATTRIB_WAFFLE;
    }
}

//
//  FUNCTION: RTF_ChangeSizeAttribute(HWND, int)
//
//  PURPOSE: Change the point size of the selected text
//
//  PARAMETERS:
//    hRTFWnd - Handle to RichEdit control
//    iPointChange - Number of points, positive or neagtive to
//                   change text
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
//

void RTF_ChangeSizeAttribute(HWND hRTFWnd, int iPointChange)
{
    CHARFORMAT cf; // This is defined in RICHEDIT.H

    WinAssert(hRTFWnd);
    cf.cbSize = sizeof(cf);

    // Get the current status of the text into the CHARFORMAT structure,
    // use the selected text. wParam is TRUE to indicate the
    // selected text, FALSE for the first character in the
    // RTF Edit control.
    SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);

    // Set the mask to tell the RTF Edit control to pay attention to
    // the size bit of the dwEffects field.
    cf.dwMask      = CFM_SIZE;
    // Increase or decrease the font size, keeping it between 6 and 128 points
    if ( 
         ((cf.yHeight + 20*iPointChange) <= (128*20))
         &&
         ((cf.yHeight + 20*iPointChange) >= (6*20))
       )
        cf.yHeight += 20*iPointChange;

    // Set the new underline status to the selected text
    SendMessage(hRTFWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

//
//  FUNCTION: RTF_ChangeFont(HWND hWnd, HWND hRTFWnd)
//
//  PURPOSE: Change the attributes of the selected text with the
//           Common Font Dialog.
//
//  PARAMETERS:
//    hWnd - Handle to a frame suitable as a parent to the Common
//           font dialog.
//    hRTFWnd - Handle to RichEdit control
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
//

void RTF_ChangeFont(HWND hWnd, HWND hRTFWnd)
{
    CHARFORMAT cf;                // This is defined in RICHEDIT.H
    CHOOSEFONT ChooseFontStruct;  // Common Dialog fun
    LOGFONT    lf;                // Log font information
    HDC        hDC;

    WinAssert(hRTFWnd);
    WinAssert(hWnd);
    cf.cbSize = sizeof(cf);

    // Get the font formatting status into the CHARFORMAT strucuture,
    // use the selected text. wParam is TRUE to indicate the
    // selected text, FALSE for the first character in the
    // RTF Edit control.
    SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);

    // Fill in the font info for the font common fialog

    ZeroMemory(&ChooseFontStruct, sizeof(CHOOSEFONT));
    ZeroMemory(&lf, sizeof(LOGFONT));

    hDC = GetDC(hWnd); // Need a screen DC, use the parent's DC

    // The RTF Edit control measures in twips, but LOGFONT measures
    // in logical units (here, pixels) so we must convert.
    //   1 point == 20 twips
    //   1 inch  == 72 points
    //   ==> 1 inch  == 1440 twips

    lf.lfHeight = MulDiv(cf.yHeight,
                         GetDeviceCaps(hDC, LOGPIXELSY),
                         -1440);

    // Done with this
    ReleaseDC(hWnd, hDC);

    // Set up the rest of the logfont structure according to the
    // information retrieved from the EM_GETCHARFORMAT message

    if (cf.dwEffects & CFE_BOLD)
        lf.lfWeight = FW_BOLD;
    else
        lf.lfWeight = FW_NORMAL;
    lf.lfItalic = (BYTE)(cf.dwEffects & CFE_ITALIC);
    lf.lfUnderline = (BYTE)(cf.dwEffects & CFE_UNDERLINE);

    lf.lfStrikeOut = (BYTE)((cf.dwEffects & CFE_STRIKEOUT) ? TRUE : FALSE);
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfCharSet = cf.bCharSet;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = cf.bPitchAndFamily;
    lstrcpy(lf.lfFaceName, cf.szFaceName);

    // Fire up the common dialog.
    ChooseFontStruct.lStructSize = sizeof(ChooseFontStruct);
    ChooseFontStruct.hwndOwner = hWnd;
    ChooseFontStruct.hDC = GetCurrentPrinterDC();
    ChooseFontStruct.lpLogFont = &lf;
    ChooseFontStruct.Flags = CF_BOTH | CF_SCALABLEONLY | CF_WYSIWYG |
                             CF_NOVECTORFONTS | CF_INITTOLOGFONTSTRUCT |
                             CF_EFFECTS;

    ChooseFontStruct.rgbColors = cf.crTextColor;
    ChooseFontStruct.nFontType = REGULAR_FONTTYPE | SCREEN_FONTTYPE;

    if (ChooseFont(&ChooseFontStruct))
    {
        // Set the mask to tell the RTF Edit control to pay attention to
        // the font formatting bits.
        cf.dwMask = CFM_BOLD | CFM_FACE | CFM_ITALIC | CFM_COLOR | CFM_CHARSET |
                    CFM_OFFSET | CFM_SIZE | CFM_UNDERLINE | CFM_STRIKEOUT;

        // ChooseFont returns the new point size in tenths of a point so
        // so we can multiply by 2 to get twips for the richedit control.

        cf.yHeight = 2 * ChooseFontStruct.iPointSize;

        // Fill in the rest of the character formatting

        cf.dwEffects = 0;
        if (lf.lfWeight >= FW_BOLD)
            cf.dwEffects |= CFE_BOLD;
        if (lf.lfItalic)
            cf.dwEffects |= CFE_ITALIC;

        if (lf.lfUnderline)
            cf.dwEffects |= CFE_UNDERLINE;

        if (lf.lfStrikeOut)
           cf.dwEffects |= CFE_STRIKEOUT;

        cf.bPitchAndFamily = lf.lfPitchAndFamily;

        cf.crTextColor = ChooseFontStruct.rgbColors;

        lstrcpy(cf.szFaceName, lf.lfFaceName);

        // Set the new formatting to the selected text
        SendMessage(hRTFWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    }

    if (ChooseFontStruct.hDC)
        DeleteDC(ChooseFontStruct.hDC);
}

//
//  FUNCTION: SaveCallback(DWORD, LPBYTE, LONG, LONG *)
//
//  PURPOSE: Callback routine for the EM_STREAMOUT Message from
//           the RichEdit control.
//
//  PARAMETERS:
//    dwCookie - App supplied value (a file handle for this example)
//    pbBuff - Pointer to data to write
//    cb - size of data to write
//    pcb - Number of bytes written to the file
//
//  RETURN VALUE:
//    Always 0
//
//  COMMENTS:
//
//

DWORD CALLBACK SaveCallback(DWORD    dwCookie, // App supplied value
                            LPBYTE   pbBuff,   // Pointer to data
                            LONG     cb,       // size of data
                            LONG     *pcb)     // Not used in streamout
{
   HANDLE hf = (HANDLE) dwCookie;

//   if(hf == (HANDLE)HFILE_ERROR)
//      return (DWORD) E_FAIL;

   WriteFile(hf, pbBuff, (DWORD)cb, (DWORD *)pcb, NULL);
   return (DWORD) (*pcb == cb ? NOERROR : E_FAIL);
}

//
//  FUNCTION: RTF_Save(LPSTR, HWND)
//
//  PURPOSE: Save the contents of the Richedit control to a file
//
//  PARAMETERS:
//    hRTFWnd - Handle to RichEdit control
//    szFileName - Name of file to save to
//
//  RETURN VALUE:
//    TRUE if successful, FALSE otherwise
//
//  COMMENTS:
//    This function utilizes the SaveCallback function above
//

BOOL RTF_Save(LPSTR szFileName, HWND hRTFWnd)
{
    HANDLE        hFile; // File handle
    EDITSTREAM    es;    // The EDITSTREAM structure

    WinAssert(hRTFWnd);
    // Call the file save as dialog only if the szFileName is empty,
    // otherwise szFileName already has a valid filename in it.

    if (*szFileName)
    {
        DWORD dwFormat;

        hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        es.dwCookie = (DWORD) hFile;

        es.dwError = 0;
        es.pfnCallback = SaveCallback;

        dwFormat = lstrcmpi(".rtf", &szFileName[lstrlen(szFileName)-4])
                           ? SF_TEXT : SF_RTF;

        SendMessage(hRTFWnd, EM_STREAMOUT,   (WPARAM) dwFormat,
                   (LPARAM) &es);

        CloseHandle(hFile);

        // Clear the dirty bit
        SendMessage(hRTFWnd, EM_SETMODIFY, FALSE, 0L);
        return TRUE;
    }

    return FALSE;
}

//
//  FUNCTION: OpenCallback(DWORD, LPBYTE, LONG, LONG *)
//
//  PURPOSE: Callback routine for the EM_STREAMIN Message from
//           the RichEdit control.
//
//  PARAMETERS:
//    dwCookie - App supplied value (a file handle for this example)
//    pbBuff - Pointer to data to read from disk
//    cb - size of data to read
//    pcb - Number of bytes read from the file
//
//  RETURN VALUE:
//    Always 0
//
//  COMMENTS:
//
//

DWORD CALLBACK OpenCallback(DWORD    dwCookie,
                            LPBYTE   pbBuff,
                            LONG     cb,
                            LONG     *pcb)
{
HANDLE hf = (HANDLE) dwCookie;

//XXX This was commented out for some reason
   if(hf == (HANDLE)HFILE_ERROR)
      return (DWORD) E_FAIL;
if (hf)
  // Read as much data as requested by the cb variable
   ReadFile(hf, pbBuff, (DWORD)cb, (DWORD *)pcb, NULL);
return (DWORD) (*pcb >= 0 ? NOERROR : (*pcb = 0, E_FAIL));
}

//
//  FUNCTION: RTF_Open(LPSTR, HWND)
//
//  PURPOSE: Load the contents of a file into the Richedit control
//
//  PARAMETERS:
//    szFileName - Name of file to save to
//    hRTFWnd - Handle to RichEdit control
//
//  RETURN VALUE:
//    TRUE if successful, FALSE otherwise
//
//  COMMENTS:
//    This function utilizes the OpenCallback function above
//

BOOL RTF_Open(LPSTR szFileName, HWND hRTFWnd)
{
    HANDLE        hFile;  // File handle
    EDITSTREAM    es;     // The EDITSTREAM structure

    WinAssert(hRTFWnd);
    // Get a filename to load from
    if (*szFileName)
    {
        // Open the file for reading
        es.dwCookie = (DWORD)CreateFile(szFileName, GENERIC_READ, 0, NULL,
                      OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
        hFile = (HANDLE)es.dwCookie;

        if (hFile)
        {
            UINT cb = cchRTFSig * sizeof(TCHAR);
            DWORD bytes_read;
            TCHAR szType[cchRTFSig + 1];
            DWORD dwFormat;
            es.dwError     = 0;            // No error
            es.pfnCallback = OpenCallback; // Use above callback

            ReadFile(hFile, szType, (DWORD)cb, &bytes_read, NULL);
            cb = bytes_read;

            szType[cb / sizeof(TCHAR)] = TEXT('\0');
            if (cb == cchRTFSig * sizeof(TCHAR))
               dwFormat = lstrcmpi(szRTFSig, szType) ? SF_TEXT : SF_RTF;
            else
               dwFormat = SF_TEXT;     // not big enough to be RTF, assume text

            // move back to the beginning of the file
            SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

            // Make it so!
            SendMessage(hRTFWnd, EM_STREAMIN, dwFormat, (LPARAM)&es);

            // Clear the dirty bit
            SendMessage(hRTFWnd, EM_SETMODIFY, 0, 0L);

            // Close the file
            CloseHandle(hFile);
        }

        // Update the toolbar
        RTF_ShowCharAttributes();

        return TRUE;
    }

    return FALSE;
}

//
//  FUNCTION: RTF_GetCurrentFont(HWND, LPSTR)
//
//  PURPOSE: Get the facename of the selected text's font
//
//  PARAMETERS:
//    hRTFWnd - Handle to RichEdit control
//    szFont - String to contain facename
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//    The string is a null string if the selection has more than
//    one facename of font
//

void RTF_GetCurrentFont(HWND hRTFWnd, LPSTR szFont)
{
    CHARFORMAT cf;                // This is defined in RICHEDIT.H

    WinAssert(hRTFWnd);
    cf.cbSize = sizeof(cf);

    // Get the font formatting status into the CHARFORMAT strucuture,
    // use the selected text. wParam is TRUE to indicate the
    // selected text, FALSE for the first character in the
    // RTF Edit control.
    SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);

    if (cf.dwMask & CFM_FACE)
        lstrcpy(szFont, cf.szFaceName);
    else
        *szFont = 0;

}

//
//  FUNCTION: RTF_SetCurrentFont(HWND, LPSTR)
//
//  PURPOSE: Set the facename of the selected text's font
//
//  PARAMETERS:
//    hRTFWnd - Handle to RichEdit control
//    szFont - String that contains facename
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//

void RTF_SetCurrentFont(HWND hRTFWnd, LPSTR szFont)
{
    CHARFORMAT cf;                // This is defined in RICHEDIT.H

    WinAssert(hRTFWnd);
    cf.cbSize = sizeof(cf);

    // Get the font formatting status into the CHARFORMAT strucuture,
    // use the selected text. wParam is TRUE to indicate the
    // selected text, FALSE for the first character in the
    // RTF Edit control.
    SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);

    cf.dwMask = CFM_FACE;

    lstrcpy(cf.szFaceName, szFont);

    SendMessage(hRTFWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

//
//  FUNCTION: RTF_ChangeMargins(HWND, UINT, UINT, UINT)
//
//  PURPOSE: Set the margins of the current paragraph.
//
//  PARAMETERS:
//    hRTFWnd - Handle to RichEdit control
//    dxStartIndent - Indent in twips of first line's left margin
//    dxRightIndent - Indent in twips from right edge of printable area
//                    of right margin.
//    dxOffset - Distance in twips of second, third, etc line's left
//               margin
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//    This function uses the RM_GETTABS (RULER.H) message to set the
//    tabs.
//

void RTF_ChangeMargins(HWND hwndRuler,
                       UINT dxStartIndent,
                       UINT dxRightIndent,
                       UINT dxOffset)
{
    PARAFORMAT pf;
    HWND hTemp;

    WinAssert(hwndRuler);
    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return;
    pf.cbSize = sizeof(pf);
    pf.dwMask = PFM_STARTINDENT | PFM_RIGHTINDENT | PFM_OFFSET | PFM_TABSTOPS;
    pf.dxStartIndent = dxStartIndent;
    pf.dxRightIndent = dxRightIndent;
    pf.dxOffset = dxOffset;

    pf.cTabCount = (short)SendMessage(hwndRuler,
                                      RM_GETTABS,
                                      (WPARAM)25,
                                      (LPARAM)(pf.rgxTabs));

    SendMessage(hTemp, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
}

//
//  FUNCTION: RTF_ShowMargins(HWND)
//
//  PURPOSE: Get the margins of the current paragraph and reflect them
//           on the ruler.
//
//  PARAMETERS:
//    hwndRuler - Handle to Ruler control (RULER.H)
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//

void RTF_ShowMargins(HWND hwndRuler)
{
     PARAFORMAT pf;
     HWND hTemp;

    WinAssert(hwndRuler);
     if ((hTemp = GetEditWindow(NULL)) == NULL)
        return;

     pf.cbSize = sizeof(pf);
     SendMessage (hTemp, EM_GETPARAFORMAT, 0, (LPARAM)&pf);

     if (
          (pf.dwMask & (PFM_STARTINDENT|PFM_RIGHTINDENT|PFM_OFFSET)) ==
          (PFM_STARTINDENT|PFM_RIGHTINDENT|PFM_OFFSET)
        )
     {
         int iMargins[5];
         iMargins[0] = pf.dxStartIndent + pf.dxOffset;
         iMargins[1] = pf.dxRightIndent;
         iMargins[2] = pf.dxStartIndent;
         iMargins[3] = 0;
         iMargins[4] = 7*1440;
         SendMessage(hwndRuler, RM_SETMARGINS, 0, (LPARAM)iMargins);
     }
     SendMessage (hwndRuler, 
                  RM_SETTABS, 
                  (WPARAM)(pf.cTabCount), 
                  (LPARAM)(pf.rgxTabs));

}

//
//  FUNCTION: SetToolbarButtonToReflectCharacterAttribute(UINT,DWORD,DWORD)
//
//  PURPOSE: Show the status of an attribute on a toolbar button
//
//  PARAMETERS:
//    uID - ID of the toolbar button
//    dwMask - Attribute mask to check for
//    dwEffects - Attribute to check for
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//

void SetToolbarButtonToReflectCharacterAttribute(UINT uID, 
                                                 DWORD dwMask, 
                                                 DWORD dwEffects)
{
    HWND hTemp;
    int iState;

    if ((hTemp = GetEditWindow(NULL)) == NULL)
       return;
    iState = RTF_GetCharAttribute(hTemp,
                                  dwMask,
                                  dwEffects);
    switch (iState)
    {
        case CHAR_ATTRIB_YES:    

            UpdateButton (uID, TBSTATE_ENABLED | TBSTATE_CHECKED); 
            break;

        case CHAR_ATTRIB_NO :    

            UpdateButton (uID, TBSTATE_ENABLED);
            break;

        case CHAR_ATTRIB_WAFFLE: 

            UpdateButton (uID, TBSTATE_ENABLED | TBSTATE_INDETERMINATE); 
            break;
    }
}

//
//  FUNCTION: RTF_ShowCharAttributes()
//
//  PURPOSE: Show the status of the bold, italic, and underline
//           attributes on the toolbar
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//    Need to add ability to check/uncheck menu items

void RTF_ShowCharAttributes(void)
{
  SetToolbarButtonToReflectCharacterAttribute(IDM_EDITBOLD,
                                              CFM_BOLD,
                                              CFE_BOLD);
  SetToolbarButtonToReflectCharacterAttribute(IDM_EDITITALIC, 
                                              CFM_ITALIC, 
                                              CFE_ITALIC);
  SetToolbarButtonToReflectCharacterAttribute(IDM_EDITUNDERLINE, 
                                              CFM_UNDERLINE, 
                                              CFE_UNDERLINE);
}
