# Microsoft Developer Studio Project File - Name="Unzip32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

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
!MESSAGE "Unzip32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Unzip32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Unzip32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "UNZIPLIB" /D "USE_EF_UT_TIME" /D "DLL" /D "REENTRANT" /D "LZW_CLEAN" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Unzip32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "UNZIPLIB" /D "USE_EF_UT_TIME" /D "DLL" /D "REENTRANT" /D "LZW_CLEAN" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Unzip32 - Win32 Release"
# Name "Unzip32 - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\unzip\api.c
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

SOURCE=..\..\..\unzip\windll\unziplib.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\Win32\win32.c
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\windll\windll.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\windll\windll.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=\Wiz\unzip\zipinfo.c
# End Source File
# End Target
# End Project
