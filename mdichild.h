/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/
#ifndef MDICHILD_H
#define MDICHILD_H
LRESULT MsgMCSetFocus  (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMCCommand   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMCCreate    (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMCMenuSelect(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMCSize      (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMCClose     (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMCDestroy   (HWND, UINT, WPARAM, LPARAM);
LRESULT CmdEdit        (HWND, WORD, WORD, HWND);      

BOOL    CreateArchiveControl(HWND);

#define IDC_ARCHIVE   100
#define IDC_STATUSBAR 101
#endif MDICHILD_H

