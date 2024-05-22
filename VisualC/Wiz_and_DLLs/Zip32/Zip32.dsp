# Microsoft Developer Studio Project File - Name="Zip32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Zip32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Zip32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Zip32.mak" CFG="Zip32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Zip32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Zip32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Zip32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\wiz_and_dlls\Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "c:\wiz\zip" /I "c:\wiz\zip\win32" /I "c:\wiz\zip\windll" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "Zip32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\wiz_and_dlls\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "c:\wiz\zip" /I "c:\wiz\zip\win32" /I "c:\wiz\zip\windll" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Zip32 - Win32 Release"
# Name "Zip32 - Win32 Debug"
# Begin Source File

SOURCE=\Wiz\zip\api.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\crc32.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\crctab.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\crypt.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\deflate.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\fileio.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\globals.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\Win32\nt.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\trees.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\util.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\Win32\win32.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\Win32\win32zip.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\windll\windll.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\windll\windll.rc
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\windll\windll32.def
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\zip.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\zipfile.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\zipup.c
# End Source File
# End Target
# End Project
