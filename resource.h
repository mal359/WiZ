/*

      Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

      See the accompanying file LICENSE (the contents of which are also
      included in unzip.h, zip.h and wiz.h) for terms of use.  If, for
      some reason, all of these files are missing, the Info-ZIP license
      also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

*/
/*
 Wiz resource header file
*/
#ifndef _RESOURCE_H
#define _RESOURCE_H

#define WINDOWMENU                 5
#define IDS_DESCRIPTION            2
#define IDS_CHILDNAME              4
#define IDI_CHILDICON            102

/* Progress Meter defines */
#define IDC_PROGRESS_TEXT         60
#define IDC_PROGRESS_FILE_NAME    61
#define IDC_PROGRESS_UPDATE       62

/* Group Box defines for resource compiler, not actually used */
#define IDC_OTHER_BOX            100
#define IDC_ZIP_BOX              101
#define IDC_UNZIP_BOX            102
#define IDC_EXTRACTION_BOX       103
#define IDC_SELECTION_BOX        104
#define IDC_GENERAL_BOX          105
#define IDC_DIRECTORY_BOX        106

/* Main Window Message Codes
 */

#define IDM_EXIT                 107

#define IDM_SHIFT_HELP           110
#define IDM_ESCAPE               112

#define IDM_RECR_DIR_STRUCT      114
#define IDM_OVERWRITE            115
#define IDM_EXTRACT_ONLY_NEWER   118
#define IDM_TRANSLATE            119
#define IDM_SPACE_TO_UNDERSCORE  120

#define IDM_UNZIP_FROM_DIR       125
#define IDM_COMMENT              126

#define IDM_CURRENT_PATH         157
#define IDM_PROMPT_TO_OVERWRITE  158

#define IDM_PASSWORD             159
/*
 * About box identifiers used to display the current version number
 * information.
 */
#define IDM_ABOUT_VERSION_INFO   160
#define IDM_ABOUT_ZIP_INFO       161
#define IDM_ABOUT_UNZIP_INFO     162

/* Pattern selection defines */

#define IDC_PATTERN_TEXT         170
#define IDC_FILE_PATTERN         171
#define IDC_PATTERN_SELECT       172
#define IDC_PATTERN_CANCEL       173
#define IDC_PATTERN_HELP         174
#define IDC_PATTERN_DESELECT     175
#define IDC_GREP_PATTERN         176
#define IDC_FILE_PATTERN_TEXT    177
#define IDC_FILE_PATTERN_EXAM    178
#define IDC_CASE_SENSITIVE       179
#define IDC_WHOLEWORDS           180
#define IDC_IGNORE_BINARY        181

/* Password defines */
#define IDC_PASSWORD_TEXT          190
#define IDC_NEW_PASSWORD_NAME_TEXT 191
#define IDC_PASSWORD_RENAME        192
#define IDC_PASSWORD_CANCEL        193
#define IDC_PASSWORD_INCORRECT     194
#define IDC_PASSWORD_HELP          195


#define IDC_ZIP_EXCLUDE_DATE     202
#define IDC_ZIP_INCLUDE_DATE     203

#define IDC_SELECT_ALL           242
#define IDC_DESELECT_ALL         243
#define IDC_ADD                  245
#define IDC_DELETE               246
#define IDC_FILE_LIST            244


#define IDC_SET_ROOT             251
#define IDC_FREE_ROOT            252
#define IDC_ROOT_DIR             253
#define IDC_MONTH                254
#define IDC_DAY                  255
#define IDC_YEAR                 256
#define IDC_DOWN                 257
#define IDC_UP                   258
#define IDC_DONE                 259

#define IDC_SET_REF              265
#define IDC_FREE_REF             266
#define IDC_REF_DIR              267

#define IDC_RECURSE              270
#define IDC_INCLUDE              271
#define IDC_RECURSE_EDITBOX      272

/* Defines for check boxes in preferences dialog box */
/* Zip Options */
#define IDC_REPAIR               299
#define IDC_REPAIR_MORE          300
#define IDC_MSDOS                301
#define IDC_SYS_FILES            302
#define IDC_VOL_LABEL            303
#define IDC_NO_DIR_ENTRY         304
#define IDC_IGNORE_DIR           305
#define IDC_CRLFLF               306
#define IDC_LFCRLF               307
#define IDC_ENCRYPT              308
#define IDC_MOVE                 309
#define IDC_FILETIME             310
#define IDC_UPDATE               311
#define IDC_FRESHEN              312
#define IDC_QUIET                313
#define IDC_VERBOSE              314
#define IDC_COMMENT              315
#define IDC_MAKE_DOS_SFX         316
#define IDC_MAKE_W32_SFX         317
#define IDC_OFFSETS              318
#define IDC_TEMPDIR              319

/* The compression defines need to be kept in order, as the order defines
   what value is put into the fLevel flag.
 */
#define IDC_COMPRESSION_0        320
#define IDC_COMPRESSION_1        321
#define IDC_COMPRESSION_2        322
#define IDC_COMPRESSION_3        323
#define IDC_COMPRESSION_4        324
#define IDC_COMPRESSION_5        325
#define IDC_COMPRESSION_6        326
#define IDC_COMPRESSION_7        327
#define IDC_COMPRESSION_8        328
#define IDC_COMPRESSION_9        329

/* IDC_PRIVILEGE is also used for the unzip options */
#define IDC_PRIVILEGE            350

/* UnZip Options */
#define IDC_RECREATE_DIR         351
#define IDC_OVERWRITE            352
#define IDC_NEWER                353
#define IDC_PROMPT_OVERWRITE     354
#define IDC_LF_CRLF              355
#define IDC_SPACE_UNDERSCORE     356
#define IDC_LBS_EXTRACT          357
#define IDC_LBS_DISPLAY          358
#define IDC_LBS_TEST             359
#define IDC_ACL                  360
#define IDC_RESCAN_FOR_ARCHIVES  361

/* General Options */
#define IDC_SHOW_TOOLBAR         373
#define IDC_CLEAR_STATUS         374
#define IDC_CLEAR_DISPLAY        375
#define IDC_RECURSE_ON_SEARCH    376
#define IDC_OLDSTYLE             377

#define ID_HELP                  399

#define IDM_EDIT_BOX             400

#define IDC_TEMP_PREFERENCES     403
//Temporary path to be shown in zip options
#define IDC_TEMPDIR_PATH         404

#define IDC_PATTERN              410

/* Clock values */
#define IDM_STATUSBAR            412

#define IDM_TOOLBAR              500 // ID for the toolbar.

#define IDB_BMP                  700 // Bitmap ID for the toolbar.
#define IDM_TIMER                701

#define IDC_LISTBOX_DIFF         800
#define IDC_STATICTEXT           801

#define TIMER_TIMEOUT            1000

#define IDM_GREPBOX              1251 // ID for grep window
#define IDM_ARCHIVEBOX           1258
#define IDM_TABCONTROL           1260

#define IDS_ARCHIVE              1271
#define IDS_EDITOR               1272
#define IDS_SCAN                 1273
#define IDS_SEARCH               1274
#define IDS_MESSAGES             1275

#define IDS_ARCHIVE_TIP          1276
#define IDS_EDITOR_TIP           1277
#define IDS_SCAN_TIP             1278
#define IDS_SEARCH_TIP           1279
#define IDS_MESSAGES_TIP         1280

#define IDS_CLEARBUFFER          1300
#define IDS_NO_MATCH             1301
#define IDS_INTERNAL_COPY_ERROR  1302
#define IDS_NO_MEMORY            1303
#define IDS_CHG_DIR_ERROR        1304
#define IDS_MAKE_ZIP_ERROR       1305
#define IDS_ERROR                1306
#define IDS_ERROR_TAB_CTRL       1307
#define IDS_CANT_OPEN            1308
// MW#define IDS_NO_CONTEXT_HELP      1309
#define IDS_HELP_EXAMPLE         1310
#define IDS_NO_EXPLORER          1311
#define IDS_FILE_NOT_FOUND       1312
#define IDS_PATH_NOT_FOUND       1313
#define IDS_BAD_FORMAT           1314
#define IDS_SHARING_VIOLATION    1315
#define IDS_UNDETERMINED_ERROR   1316
#define IDS_EXECUTION_ERROR      1317
#define IDS_DELETE_TITLE         1318
#define IDS_DELETE_MESSAGE       1319
#define IDS_ABOUT                1320
#define IDS_SOUND                1321
#define IDS_FILESPEC             1322
#define IDS_NAME                 1323
#define IDS_ORIG_SIZE            1324
#define IDS_COMP_SIZE            1325
#define IDS_RATIO                1326
#define IDS_DATE                 1327
#define IDS_TIME                 1328
#define IDS_CRYPT                1329
#define IDS_ARCHIVE_NAME         1330
#define IDS_FIND_FILES           1331
#define IDS_UNZIP_TO             1332
#define IDS_SEARCH_NAME          1333
#define IDS_SEARCH_FOR           1334
#define IDS_CRLF_WARNING         1335
#define IDS_CRLF_TITLE           1336
#define IDS_SELECT               1337
#define IDS_PASSWORD_NAME        1338
#define IDS_RENAME_STR           1339
#define IDS_RENAME_NAME          1340
#define IDS_LOADLIBRARY          1341
#define IDS_NO_LOAD              1342
#define IDS_NO_INIT              1343
#define IDS_REPLACE              1344
#define IDS_FOUND_MATCH_STR      1345
#define IDS_NOTE                 1346
#define IDS_CONFIRM_DELETE       1347
#define IDS_NO_WIZDIFF           1348

#define IDS_OPTIONSMENU          1500
#define IDM_PREFERENCES          1501

#define IDS_ARCHIVEMENU           1600
#define IDM_SELECT_ALL            1601
#define IDM_DESELECT_ALL          1602
#define IDM_SELECT_BY_PATTERN     1603
#define IDM_ZIP_DELETE_ENTRIES    1604
#define IDM_UPDATE_ZIP            1605
#define IDM_EXTRACT               1606
#define IDM_DISPLAY               1607
#define IDM_TEST                  1608
#define IDM_GET_ZIPINFO           1609
#define IDM_SHOW_COMMENT          1610
#define IDM_DIFF_ARCHIVE          1611
#define IDM_GREP_ARCHIVE_CONTENTS 1612
#define IDM_DIFF_ARCHIVE_CONTENTS 1613
#define IDM_TOGGLE_TABS           1614

#define IDS_SEARCHMENU           1700
#define IDM_SEARCH_ARCHIVE       1701
#define IDM_SCAN_ARCHIVE         1702
#define IDM_GREP_ARCHIVE         1703

#define IDS_EDITMENU             1750
#define IDM_CLEAR_STATUS         1751
#define IDM_NOTIFY_CLEARING      1752
#define IDM_EXTERNAL_RTF         1753

#define IDS_FILEMENU             1800
#define IDM_ZIP_TARGET           1801
#define IDM_ZIP_TARGET_CURRENT   1802
#define IDM_OPEN                 1803
#define IDM_CHDIR                1804
#define IDM_COPY_ARCHIVE         1805
#if 0
#define IDM_CANCEL_OPERATION     1805
#define IDM_MOVE_ARCHIVE         1806
#define IDM_RENAME_ARCHIVE       1807
#endif
#define IDM_DELETE_ARCHIVE       1808
#define IDM_EXPLORER             1809
#define IDM_WIZDIFF              1811

#define IDS_WINDOWMENU           1900
#define IDM_CLEAR_ARCHIVEBOX     1901
#define IDM_CLEAR_GREPBOX        1902
#define IDM_WINDOWTILEHORIZONTAL 1904
#define IDM_WINDOWTILEVERTICAL   1905
#define IDM_WINDOWCASCADE        1906
#define IDM_WINDOWCLOSEALL       1907
#define IDM_WINDOWICONS          1908
#define IDM_WINDOWCHILD          1909

#define IDS_HELPMENU             2000
#define IDM_HELP                 2001
#define IDM_HELP_HELP            2002
#define IDM_ABOUT                2003

// System Menu string ID's
#define IDS_SYSMENU         900
#define IDS_MDISYSMENU      901

#define IDS_SCSIZE          SC_SIZE
#define IDS_SCMOVE          SC_MOVE
#define IDS_SCMINIMIZE      SC_MINIMIZE
#define IDS_SCMAXIMIZE      SC_MAXIMIZE
#define IDS_SCNEXTWINDOW    SC_NEXTWINDOW
#define IDS_SCPREVWINDOW    SC_PREVWINDOW
#define IDS_SCCLOSE         SC_CLOSE
#define IDS_SCRESTORE       SC_RESTORE
#define IDS_SCTASKLIST      SC_TASKLIST

#endif /* _RESOURCE_H */

