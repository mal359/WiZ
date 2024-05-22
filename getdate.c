/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "wiz.h"
#ifdef __BORLANDC__
#include <dir.h>
#endif
#include <dos.h>
//MW #include "helpids.h"
#include <time.h>

/****************************************************************************

    FUNCTION: GetDateProc(HWND, unsigned, WPARAM, LPARAM)

    PURPOSE:  Processes messages for "Get Files For Archive" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI
GetDateProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam)
{
unsigned int i, m, d, y;
struct tm *date;
time_t long_time;

char digits[7];
char Months[13][10] = {"January", "February", "March", "April", "May", "June",
                  "July", "August", "September", "October", "November",
                  "December"};
static HWND hOK;

   switch (wMessage) {
   case WM_INITDIALOG:
      {
      hOK = GetDlgItem(hwndDlg, IDOK);

      for (i = 0; i < 12; i++)
         {
         SendMessage(GetDlgItem(hwndDlg, IDC_MONTH), CB_ADDSTRING,
            0, (LPARAM)(LPCSTR)Months[i]);
         }
      for (i = 1; i < 32; i++)
         {
         SendMessage(GetDlgItem(hwndDlg, IDC_DAY), CB_ADDSTRING,
            0, (LPARAM)(LPCSTR)itoa(i, digits, 10));
         }
      for (i = 1980; i < 2100; i++)
         {
         SendMessage(GetDlgItem(hwndDlg, IDC_YEAR), CB_ADDSTRING,
            0, (LPARAM)(LPCSTR)itoa(i, digits, 10));
         }
      /* Set date to today's date */
      time( &long_time );                 /* Get time as long integer. */
      date = localtime( &long_time );     /* Convert to local time. */


      SendMessage(GetDlgItem(hwndDlg, IDC_MONTH), CB_SELECTSTRING,
         0, (LPARAM)(LPCSTR)Months[date->tm_mon]);
      SendMessage(GetDlgItem(hwndDlg, IDC_DAY), CB_SELECTSTRING,
         0, (LPARAM)(LPCSTR)itoa(date->tm_mday, digits, 10));
      SendMessage(GetDlgItem(hwndDlg, IDC_YEAR), CB_SELECTSTRING,
         0, (LPARAM)(LPCSTR)itoa((date->tm_year+1900), digits, 10));

      CenterDialog(NULL, hwndDlg); /* center on parent */
      }
      break;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDCANCEL:
            ZpOpt.Date[0] = '\0';
            EndDialog(hwndDlg, wParam);
            break;
         case IDC_DOWN:
            /* returns are 0 based indexes into list part of combo box */
            m = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_MONTH), CB_GETCURSEL,
               0, 0) + 1;
            d = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_DAY), CB_GETCURSEL,
               0, 0) + 1;
            y = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_YEAR), CB_GETCURSEL,
               0, 0);
            if (d > 1)
               d--;
            else
               {
               if (m > 1)
                  {
                  m--;
                  }
               else
                  {
                  if (y != 0)
                     {
                     y--;
                     m = 12;
                     }
                  }
               switch (m) {
                  case 1:
                     d = 31;
                     break;
                  case 2:
                     d = 29;
                     break;
                  case 3:
                     d = 30;
                     break;
                  case 4:
                     d = 30;
                     break;
                  case 5:
                     d = 31;
                     break;
                  case 6:
                     d = 30;
                     break;
                  case 7:
                     d = 31;
                     break;
                  case 8:
                     d = 31;
                     break;
                  case 9:
                     d = 30;
                     break;
                  case 10:
                     d = 31;
                     break;
                  case 11:
                     d = 30;
                     break;
                  case 12:
                     d = 31;
                     break;
                     }
               }
            SendMessage(GetDlgItem(hwndDlg, IDC_MONTH), CB_SELECTSTRING,
               (unsigned int)-1, (LPARAM)(LPCSTR)Months[m-1]);
            SendMessage(GetDlgItem(hwndDlg, IDC_DAY), CB_SELECTSTRING,
               (unsigned int)-1, (LPARAM)(LPCSTR)itoa(d, digits, 10));
            SendMessage(GetDlgItem(hwndDlg, IDC_YEAR), CB_SELECTSTRING,
               (unsigned int)-1, (LPARAM)(LPCSTR)itoa(y+1980, digits, 10));
            break;
         case IDC_UP:
            /* returns are 0 based indexes into list part of combo box */
            m = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_MONTH), CB_GETCURSEL,
               0, 0) + 1;
            d = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_DAY), CB_GETCURSEL,
               0, 0) + 1;
            y = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_YEAR), CB_GETCURSEL,
               0, 0);
            if (d < 29)
               d++;
            else
               {
               switch (m) {
                  case 1:
                     if (d < 31)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 2:
                     if (d < 29)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 3:
                     if (d < 31)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 4:
                     if (d < 30)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 5:
                     if (d < 31)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 6:
                     if (d < 30)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 7:
                     if (d < 31)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 8:
                     if (d < 31)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 9:
                     if (d < 30)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 10:
                     if (d < 31)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 11:
                     if (d < 30)
                        d++;
                     else
                        {
                        d = 1;
                        m++;
                        }
                     break;
                  case 12:
                     if (d < 31)
                        d++;
                     else
                        {
                        d = 1;
                        m = 1;
                        y++;
                        }
                     break;
                   }
               }
            SendMessage(GetDlgItem(hwndDlg, IDC_MONTH), CB_SELECTSTRING,
               (unsigned int)-1, (LPARAM)(LPCSTR)Months[m-1]);
            SendMessage(GetDlgItem(hwndDlg, IDC_DAY), CB_SELECTSTRING,
               (unsigned int)-1, (LPARAM)(LPCSTR)itoa(d, digits, 10));
            SendMessage(GetDlgItem(hwndDlg, IDC_YEAR), CB_SELECTSTRING,
               (unsigned int)-1, (LPARAM)(LPCSTR)itoa(y+1980, digits, 10));
            break;
         case IDOK:
            /* returns are 0 based indexes into list part of combo box */
            m = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_MONTH), CB_GETCURSEL,
               0, 0) + 1;
            d = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_DAY), CB_GETCURSEL,
               0, 0) + 1;
            y = (unsigned int)SendMessage(GetDlgItem(hwndDlg, IDC_YEAR), CB_GETCURSEL,
               0, 0);
            y = 80 + y;
            if (y > 99)
               {
               y = y - 100;
               y = y + 2000;
               }
            else
               y = y+1900;
            sprintf(ZpOpt.Date, "%02d%02d%04d", m,d,y);
            EndDialog(hwndDlg, wParam);
            break;
         }
      default:
         break;
   }
   return FALSE;
}

