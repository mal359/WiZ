/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <windows.h>
#include "wiz.h"

HWND hSplashScreen;

HBITMAP LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,
                           HPALETTE FAR* lphPalette);
HPALETTE CreateDIBPalette (LPBITMAPINFO lpbmi, LPINT lpiNumColors);

HPALETTE CreateDIBPalette (LPBITMAPINFO lpbmi, LPINT lpiNumColors)
{
   LPBITMAPINFOHEADER  lpbi;
   LPLOGPALETTE     lpPal;
   HANDLE           hLogPal;
   HPALETTE         hPal = NULL;
   int              i;

   lpbi = (LPBITMAPINFOHEADER)lpbmi;
   if (lpbi->biBitCount <= 8)
       *lpiNumColors = (1 << lpbi->biBitCount);
   else
       *lpiNumColors = 0;  // No palette needed for 24 BPP DIB

   if (*lpiNumColors)
      {
      hLogPal = GlobalAlloc (GHND, sizeof (LOGPALETTE) +
                             sizeof (PALETTEENTRY) * (*lpiNumColors));
      lpPal = (LPLOGPALETTE) GlobalLock (hLogPal);
      lpPal->palVersion    = 0x300;
      lpPal->palNumEntries = (WORD)*lpiNumColors;

      for (i = 0;  i < *lpiNumColors;  i++)
         {
         lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
         lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
         lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
         lpPal->palPalEntry[i].peFlags = 0;
         }
      hPal = CreatePalette (lpPal);
      GlobalUnlock (hLogPal);
      GlobalFree   (hLogPal);
   }
   return hPal;
}

HBITMAP LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,
                           HPALETTE FAR* lphPalette)
{
    HRSRC  hRsrc;
    HGLOBAL hGlobal, hTemp;
    DWORD dwSize;
    HBITMAP hBitmapFinal = NULL;
    LPBITMAPINFOHEADER  lpbi;
    LPSTR lpRes, lpNew;
    HDC hdc;
    int iNumColors;

    if ((hRsrc = FindResource(hInstance, lpString, RT_BITMAP)) != NULL)
    {
       hTemp = LoadResource(hInstance, hRsrc);
       dwSize = SizeofResource(hInstance, hRsrc);
       lpRes = LockResource(hTemp);

       hGlobal = GlobalAlloc(GHND, dwSize);
       lpNew = GlobalLock(hGlobal);
       memcpy(lpNew, lpRes, dwSize);
       FreeResource(hTemp);

       lpbi = (LPBITMAPINFOHEADER)lpNew;

       hdc = GetDC(NULL);
       *lphPalette =  CreateDIBPalette ((LPBITMAPINFO)lpbi, &iNumColors);
       if (*lphPalette)
       {
          SelectPalette(hdc,*lphPalette,FALSE);
          RealizePalette(hdc);
       }

       hBitmapFinal = CreateDIBitmap(hdc,
                 (LPBITMAPINFOHEADER)lpbi,
                 (LONG)CBM_INIT,
                 (LPSTR)lpbi + lpbi->biSize + iNumColors * sizeof(RGBQUAD),
                 (LPBITMAPINFO)lpbi,
                 DIB_RGB_COLORS );

       ReleaseDC(NULL,hdc);
       GlobalUnlock(hGlobal);
       GlobalFree(hGlobal);
    }
    return (hBitmapFinal);
}

void ShowSplashScreen(void)
{
HWND hWndDesktop;
RECT rc;
HBITMAP hBitmap,hOldBitmap;
HPALETTE hPalette;
HDC hMemDC, hdc;
BITMAP bm;

hBitmap = LoadResourceBitmap(hInst,"splash", &hPalette);
GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

hWndDesktop = GetDesktopWindow();
GetClientRect(hWndDesktop, &rc);

rc.left = (rc.right-bm.bmWidth)/2;
rc.top  = (rc.bottom-bm.bmHeight)/2;

hSplashScreen = CreateWindow("STATIC", NULL,
        WS_CHILD|WS_VISIBLE | WS_DLGFRAME |
        WS_EX_TOPMOST,
        rc.left,
        rc.top,
        bm.bmWidth+(2*GetSystemMetrics(SM_CXFIXEDFRAME)),
        bm.bmHeight+(2*GetSystemMetrics(SM_CYFIXEDFRAME)),
        hWndDesktop,
        NULL, /* Use class menu */
        (HANDLE)GetWindowLong (hWndDesktop, GWL_HINSTANCE),
        NULL);

SetWindowPos(hSplashScreen,
             HWND_TOP,
             rc.left,
             rc.top,
             bm.bmWidth+(2*GetSystemMetrics(SM_CXFIXEDFRAME)),
             bm.bmHeight+(2*GetSystemMetrics(SM_CYFIXEDFRAME)),
             SWP_SHOWWINDOW);

hdc = GetDC(hSplashScreen);
hMemDC = CreateCompatibleDC(hdc);
SelectPalette(hdc,hPalette,FALSE);
RealizePalette(hdc);
SelectPalette(hMemDC,hPalette,FALSE);
RealizePalette(hMemDC);
hOldBitmap = SelectObject(hMemDC,hBitmap);
BitBlt(hdc,0,0,bm.bmWidth,bm.bmHeight,hMemDC,0,0,SRCCOPY);
DeleteObject(SelectObject(hMemDC,hOldBitmap));
DeleteDC(hMemDC);
ReleaseDC(hSplashScreen,hdc);
DeleteObject(hPalette);
UpdateWindow(hWndDesktop);
}

