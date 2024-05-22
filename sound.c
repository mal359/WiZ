/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <stdio.h>
#include "sound.h"
#include "wiz.h"
//MW #include "helpids.h"
#include <mmsystem.h>

/* wiz sound control module, sound.c.
 * This module controls what sound wiz makes after during or after extracting a file.
 * All users can optionally produce a beep after unzipping.
 * wiz uses the presence of the DLL MMSYSTEM.DLL to determine
 * whether MM capability is present.  It further queries the number of wave
 * devices in the system to see if wave playing hardware is present.
 * This approach gives maximum system useability without causing Windows
 * errors such as "Can't find dynalink!"
 */

HWND hSoundWaveDuring, hSoundWaveAfter, hFileText, hSoundEdit, hPlay, hBrowse;
UINT uMaxSoundRadioButton; /* upper boundary of uSoundButtonSelected   */
TCHAR szTemp[PATH_MAX];
extern int fSoundOption;
char szMMSystemDll[]         = "WINMM.DLL";
char szPlaySound[]           = "PlaySound";
char szDfltWaveFile[]        = "WIZ.WAV";
char szWaveBrowseTitle[]     = "Browse Sound Files";
char gszFilter[MAXFILTERBUF] =
                           "Wave Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0\0";

HINSTANCE hinstMMSystem;     /* MMSystem DLL instance */
FARPROC lpSndPlaySound;      /* pointer to SndPlaySound() */
FARPROC lpWaveOutGetNumDevs; /* pointer to WaveOutGetNumDevs() */
BOOL CanPlayWave(void);
WORD uSoundButtonSelected = IDM_SOUND_NONE; /* state of sound option */
WORD uSoundButtonSelectedTmp;/* state of sound option while in dialog box */


/* Forward Refs
 */
static BOOL DoOpenFile(HWND hWnd, LPSTR lpDestFileName);
static BOOL CanPlayWave(void);
void InitSoundPrefs(HWND);

/* Test for Wave Playing Capability
 */
static BOOL CanPlayWave(void)
{
static bTestedForWave = FALSE; /* true if test for wave playing has been done */
static bCanPlayWave = FALSE;   /* true if system can play wave   */
int   nPrevErrorMode;         /* previous error mode               */

if (bTestedForWave)         /* deja vu ? */
   return(bCanPlayWave);

bTestedForWave = TRUE;
nPrevErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);
hinstMMSystem = LoadLibrary(szMMSystemDll);
SetErrorMode(nPrevErrorMode);
if (hinstMMSystem != NULL)
   {
/* If can't load the function which looks up number of wav out devices or
 * number of wav output devices is zero, we can't play wav files.
 */

#ifdef __BORLANDC__
#pragma warn -pro
#endif
   if ((lpWaveOutGetNumDevs =
         GetProcAddress(hinstMMSystem, "waveOutGetNumDevs")) == NULL ||
         (*lpWaveOutGetNumDevs)() == 0 ||
         (lpSndPlaySound =
         GetProcAddress(hinstMMSystem, szPlaySound)) == NULL)
      {
      FreeLibrary(hinstMMSystem);   /* unload library   */
      }
   else /* we're set to play waves */
      {
      bCanPlayWave = TRUE;   /* flag that we can play waves   */
      }
#ifdef __BORLANDC__
#pragma warn .pro
#endif
   }
return bCanPlayWave;
}

/* Initialize Sound Options is called on wiz start-up to read
 * the sound option and sound name.
 * Read the Sound Option to see if user wants beep, sound, or nothing.
 * Read chosen sound name or wave file.
 */
void InitSoundOptions(void)
{
uSoundButtonSelected = IDM_SOUND_NONE;
uSoundButtonSelected += (WORD)fSoundOption;

   /* Do range check on sound option. Set to none if necessary.
    */
if (uSoundButtonSelected > IDM_SOUND_WAVE_AFTER ||
   (uSoundButtonSelected > IDM_SOUND_BEEP &&
   !CanPlayWave()))
   uSoundButtonSelected = IDM_SOUND_NONE;
}


/* Play Sound During extraction, test, or display  if requested.
 * Don't use a default if nothing specified.
 */
void WINAPI SoundDuring(void)
{
   if (uSoundButtonSelected == IDM_SOUND_WAVE_DURING && CanPlayWave())
      {
#ifdef __BORLANDC__
#pragma warn -pro
#endif
      (*lpSndPlaySound)((LPSTR)lpumb->szSoundName, NULL,
         SND_ASYNC|SND_NOSTOP|SND_NODEFAULT);
#ifdef __BORLANDC__
#pragma warn .pro
#endif
      }
}

/* Play Sound After extraction, test, or display  if requested.
 */
void SoundAfter(void)
{
   switch (uSoundButtonSelected) {
   case IDM_SOUND_BEEP:
      MessageBeep(1);
      break;
   case IDM_SOUND_WAVE_AFTER:
#ifdef __BORLANDC__
#pragma warn -pro
#endif
      if (CanPlayWave())
         (*lpSndPlaySound)((LPSTR)lpumb->szSoundName, NULL,
                           SND_ASYNC|SND_NOSTOP);
#ifdef __BORLANDC__
#pragma warn .pro
#endif
      break;
   }
}

/* DoFileOpen Dialog calls the common dialog function GetOpenFileName()
 * to browse and select a wav file.
 */
static BOOL DoOpenFile(HWND hWnd, LPSTR lpDestFileName)
{
BOOL fRetCode = FALSE;   /* assume failure */
/* DWORD dwExtdError; Not used */

memset(&lpumb->wofn, '\0', sizeof(OPENFILENAME));
lpumb->wofn.lStructSize       = sizeof(OPENFILENAME);
lpumb->wofn.hwndOwner         = (HWND)hWnd;
lpumb->wofn.hInstance         = (HANDLE)NULL;
lpumb->wofn.lpstrFilter       = gszFilter;
lpumb->wofn.lpstrCustomFilter = (LPSTR)NULL;
lpumb->wofn.nMaxCustFilter    = 0L;
lpumb->wofn.nFilterIndex      = 1L;
lpumb->wofn.lpstrFile         = lpDestFileName;
lpDestFileName[0]             = '\0';

lpumb->wofn.nMaxFile          = (DWORD)PATH_MAX;
lpumb->wofn.lpstrFileTitle    = (LPSTR)NULL;
lpumb->wofn.nMaxFileTitle     = 0;
lpumb->wofn.lpstrInitialDir   = (LPSTR)NULL;
lpumb->wofn.lpstrTitle        = (LPSTR)szWaveBrowseTitle;
lpumb->wofn.Flags             = OFN_HIDEREADONLY|
                                OFN_PATHMUSTEXIST|
                                OFN_FILEMUSTEXIST|
                                OFN_NOCHANGEDIR|
                                OFN_EXPLORER;
// MW                                OFN_SHOWHELP ;
lpumb->wofn.nFileOffset       = 0;
lpumb->wofn.nFileExtension    = 0;
lpumb->wofn.lpstrDefExt       = "WAV";
lpumb->wofn.lCustData         = 0L;
lpumb->wofn.lpTemplateName    = (LPSTR)NULL;

if ( GetOpenFileName( &(lpumb->wofn) ) )
   {
   HFILE   hFile;
   OFSTRUCT ofstruct;

   hFile=OpenFile(lpumb->wofn.lpstrFile, &ofstruct, OF_EXIST);
   if (hFile != -1)
      {
      fRetCode = TRUE;
      }
/* NOTE!!!  On a closed system (ie, not running on a network)
 * this OpenFile call should NEVER fail.  This because we passed in the
 * OFN_FILEMUSTEXIST flag.  However, on a network system,
 * there is a *very* small chance that between the time it's checked
 * for existance of the file and the time the call to OpenFile
 * was made here, someone else on the network has deleted the file.
 * MORAL: ALWAYS, ALWAYS, ALWAYS check the return code from your
 * call to OpenFile().
 */

    }
else /* dialog failed */
    {
/*         dwExtdError = CommDlgExtendedError();   dwExtdError code not used */
    CommDlgExtendedError();   /* get error code */
    }
return(fRetCode);         /* return indication      */
}

void InitSoundPrefs(HWND hwndDlg)
{
if (CanPlayWave())
   {
   hSoundWaveDuring = GetDlgItem(hwndDlg, IDM_SOUND_WAVE_DURING);
   hSoundWaveAfter = GetDlgItem(hwndDlg, IDM_SOUND_WAVE_AFTER);
   hFileText = GetDlgItem(hwndDlg, IDM_SOUND_FILE_TEXT);
   hSoundEdit = GetDlgItem(hwndDlg, IDM_SOUND_EDIT);
   hPlay = GetDlgItem(hwndDlg, IDM_SOUND_PLAY);
   hBrowse = GetDlgItem(hwndDlg, IDM_SOUND_BROWSE);
   EnableWindow(hSoundWaveDuring, TRUE);
   EnableWindow(hSoundWaveAfter, TRUE);
   WinAssert(hFileText);
   EnableWindow(hFileText, TRUE);
   EnableWindow(hSoundEdit, TRUE);
   EnableWindow(hPlay, TRUE);
   EnableWindow(hBrowse, TRUE);
   SetDlgItemText(hwndDlg, IDM_SOUND_EDIT, lpumb->szSoundName);
   uMaxSoundRadioButton = IDM_SOUND_WAVE_AFTER;
   }
else   /* Can't play wave */
   {
   uMaxSoundRadioButton = IDM_SOUND_BEEP;
   }
lstrcpy(szTemp, lpumb->szSoundName);
uSoundButtonSelectedTmp = uSoundButtonSelected; /* initialize temp value */
CheckRadioButton(hwndDlg, IDM_SOUND_NONE, uMaxSoundRadioButton, uSoundButtonSelectedTmp);
}

/****************************************************************************

    FUNCTION: SoundProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Sound Options" dialog box of wiz
    AUTHOR:   Mike White
    
    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/

#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI
SoundProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
   switch (wMessage) {
   case WM_INITDIALOG:
      InitSoundPrefs(hwndDlg);
      break;
   case WM_NOTIFY:
          switch (((NMHDR FAR *)lParam)->code)
             {
             case PSN_SETACTIVE:
                  // Initialize the controls.
                  InitSoundPrefs(hwndDlg);
                  break;

             case PSN_APPLY:
                  uSoundButtonSelected = uSoundButtonSelectedTmp;
                  GetDlgItemText(hwndDlg, IDM_SOUND_EDIT, lpumb->szSoundName, PATH_MAX);
                  break;

             case PSN_KILLACTIVE:
                  return TRUE;

             case PSN_RESET:
                  lstrcpy(lpumb->szSoundName, szTemp);
                  // Reset to the original values.
                  break;
             }
          break;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
      case IDM_SOUND_NONE:
      case IDM_SOUND_BEEP:
      case IDM_SOUND_WAVE_DURING:
      case IDM_SOUND_WAVE_AFTER:
         uSoundButtonSelectedTmp = LOWORD(wParam);
         CheckRadioButton(hwndDlg, IDM_SOUND_NONE, uMaxSoundRadioButton,
                        uSoundButtonSelectedTmp);
         break;
      case IDM_SOUND_PLAY:
         GetDlgItemText(hwndDlg, IDM_SOUND_EDIT, lpumb->szSoundName, PATH_MAX);
#ifdef __BORLANDC__
#pragma warn -pro
#endif
         (*lpSndPlaySound)((LPSTR)lpumb->szSoundName, NULL,
                           SND_ASYNC|SND_NOSTOP);
#ifdef __BORLANDC__
#pragma warn .pro
#endif
         break;
      case IDM_SOUND_BROWSE:
         if (DoOpenFile(hwndDlg, lpumb->szSoundName))
            {
            /* transfer to command window          */
            SetDlgItemText(hwndDlg, IDM_SOUND_EDIT, lpumb->szSoundName);
            }

         break;
      }
      return TRUE;
   }
   return FALSE;
}

