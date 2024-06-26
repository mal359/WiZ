# Microsoft Developer Studio Project File - Name="Unzip32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Unzip32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Unzip32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Unzip32.mak" CFG="Unzip32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Unzip32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Unzip32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Unzip32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\wiz_and_dlls\Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "DLL" /D "REENTRANT" /D "USE_EF_UT_TIME" /D "NO_ASM" /D "LZW_CLEAN" /YX /FD /c
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

!ELSEIF  "$(CFG)" == "Unzip32 - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "DLL" /D "REENTRANT" /D "USE_EF_UT_TIME" /D "NO_ASM" /D "LZW_CLEAN" /YX /FD /c
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

# Name "Unzip32 - Win32 Release"
# Name "Unzip32 - Win32 Debug"
# Begin Source File

SOURCE=\Wiz\unzip\api.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\crc32.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\crctab.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\crypt.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\explode.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\extract.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\fileio.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\globals.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\inflate.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\list.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\match.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\Win32\nt.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\process.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\unshrink.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\Win32\win32.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\windll\windll.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\windll\windll.rc
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\windll\windll32.def
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\zipinfo.c
# End Source File
# End Target
# End Project
