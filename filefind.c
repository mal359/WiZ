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
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <io.h>
#include <dos.h>
#ifndef __BORLANDC__
#include <direct.h>
#else
#include <dir.h>
#endif
#include <process.h>
#include <commdlg.h>
#include "wiz.h"

BOOL fDoingArchives = FALSE; /* TRUE only if looking for archives */
BOOL fZapTemps = FALSE;      /* TRUE only if destroying temp directories */
/*
 * FindFile - find files matching a pattern files on specified drives
 * parameters:
 *            drives   - An array of drive letters to search.
 *            pattern  - A file name pattern.
 */
void FindFile(char *drives, char *pattern, HWND hDlg)
{
char tree[] = "Z:";

/* Process all selected drives. Not truly necessary for current usage, but
   retained for potential future use as file finder, grep etc.
 */
while (*drives)
   {
   tree[0] = *drives++;

   if (lstrrchr(pattern, '\\') != NULL)
      {
      char str[80], *p;
      lstrcpy(str, pattern);
      p = lstrrchr(str, '\\');
      str[(int)(p-str)] = '\0';
      p = lstrrchr(pattern, '\\');
      p++;
      lstrcpy(pattern, p);
      DoADir(str, "*.*", pattern, hDlg, tree[0]);
      }
   else
      DoADir(tree, "*.*", pattern, hDlg, tree[0]);
    }
}

/*
 * DoADir - search a directory for files matching a file name pattern
 *          recursivly search sub directories
 * parameters:
 *             patternp  - A path to search.
 *             patternn  - A file name pattern to use to find directories.
 *             include   - A file name pattern to use to select files.
 *             hDlg      - handle to the count dialog window
 *             drive     - Drive letter to search on
 */
void DoADir(char *patternp, char *patternn,
   char *include, HWND hDlg, char drive)
{
char  patternw[PATH_MAX],npatternp[PATH_MAX];
BOOL  have_subs, included_files;
HANDLE hFoundFile;
WIN32_FIND_DATA fileinfo;

lstrcpy(patternw, patternp);
if (patternp[lstrlen(patternw)-1] != '\\')
   lstrcat(patternw, "\\");
lstrcat(patternw, patternn);

have_subs = FALSE;
if ((hFoundFile = FindFirstFile(patternw, &fileinfo)) != INVALID_HANDLE_VALUE)
   {
   included_files = FALSE;
   do
      {
      if (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  /* subdirectory */
         {
         if (fileinfo.cFileName[0] != '.')  /* ignore . and .. */
            if (uf.fRecurseOnSearch)
               have_subs = TRUE;
         }
       else                                 /* file */
         {
         if (FnMatch(include, fileinfo.cFileName))
            {
            char buf[PATH_MAX];
            /* If hDlg == NULL, then we got here from the Search
               Archive menu item.
             */
            if ((!included_files) && (hDlg != NULL))
               {
               buf[0] = drive;
               buf[1] = ':';
               buf[2] = '\0';
               lstrcat(buf, patternp);
               if (buf[lstrlen(buf)-1] != '\\')
                  lstrcat(buf, "\\");
               if (!fScanArchives)
                  {
                  lstrcat(buf, include);
                  if (fDoingArchives)
                     {
                     if (IsValidArchive(buf))
                        {
                        SendMessage(GetDlgItem(hDlg, IDC_FILE_LIST),
                           LB_INSERTSTRING, 0, (LONG) ((LPSTR) buf));
                        included_files = TRUE;
                        }
                     }
                  else
                     {
                     SendMessage(GetDlgItem(hDlg, IDC_FILE_LIST),
                        LB_INSERTSTRING, 0, (LONG) ((LPSTR) buf));
                     included_files = TRUE;
                     }
                  }
               else
                  {
                  LV_ITEM lvi;
                  lstrcat(buf, fileinfo.cFileName);
                  lvi.mask        = LVIF_TEXT;
                  lvi.state       = 0;
                  lvi.stateMask   = LVIS_STATEIMAGEMASK;

                  lvi.iItem       = 0;
                  lvi.iSubItem    = 0;
                  lvi.cchTextMax  = 9;
                  lvi.iImage      = 0;

                  lvi.pszText = buf;
                  if (IsValidArchive(buf))
                     lvi.iItem = ListView_InsertItem(hWndArchivesFound, &lvi);
                  }
               }
            else if (hDlg == NULL)
               {
               buf[0] = drive;
               buf[1] = ':';
               buf[2] = '\0';
               lstrcat(buf, patternp);
               if (buf[lstrlen(buf)-1] != '\\')
                  lstrcat(buf, "\\");
               lstrcat(buf, fileinfo.cFileName);
               if (fZapTemps)
                  DeleteFile(buf);
               else
                  SearchArchive(buf);
               }
            }
         }
      } while (FindNextFile(hFoundFile, &fileinfo));
   FindClose(hFoundFile);
   }

if (have_subs)
   {
   if ((hFoundFile = FindFirstFile(patternw, &fileinfo)) != INVALID_HANDLE_VALUE)
      {
      do
         {
         if (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  /* subdirectory */
            {
            if (fileinfo.cFileName[0] != '.')  /* ignore . and .. */
               {
               lstrcpy(npatternp, patternp);
               if (patternp[lstrlen(npatternp)-1] != '\\')
                  lstrcat(npatternp, "\\");
               lstrcat(npatternp, fileinfo.cFileName);
               DoADir(npatternp, patternn, include, hDlg, drive);
               if (fZapTemps)
                  RemoveDirectory(npatternp);
               }
            }
         else
            if (fZapTemps)
               DeleteFile(fileinfo.cFileName);
         } while (FindNextFile(hFoundFile, &fileinfo));
      FindClose(hFoundFile);
      }
   }
}

/*
 *FnMatch - test if a file name matches a file name pattern.
 *           handles * and ? wildcard characters.
 * parameters:
 *             pat   - A file name pattern (ie. xyz?.*)
 *             name  - A file name to test against pat (ie. xyz1.c)
 * returns:
 *             1 - if name_match
 *             0 - if not a name_match
 */
BOOL FnMatch(char *pat, char *name)
{
BOOL name_match = TRUE, ndone = TRUE;
char *cpp, *cpn;

if (!lstrcmpi(pat, "*.*"))
   return TRUE;

cpp = pat;
cpn = name;
while (ndone)
   {
   switch (*cpp)
      {
      case '*':
         /* skip to . or end of pat */
         while (*cpp && *cpp != '.')
            cpp++;

         /* skip to . or end of name */
         while (*cpn && *cpn != '.')
            cpn++;
         break;

      case '?':
         cpp++;
         cpn++;
         break;

      case 0:
         if (*cpn != 0)
            name_match = FALSE;
         ndone = FALSE;
         break;

      default:
         if (tolower(*cpp) == tolower(*cpn))
            {
            cpp++;
            cpn++;
            }
         else
            {
            name_match = FALSE;
            ndone = FALSE;
            }
         break;
      }
   }
return(name_match);
}

