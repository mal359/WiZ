# Microsoft Developer Studio Project File - Name="Zip32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

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
!MESSAGE "Zip32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Zip32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Zip32 - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_EF_UT_TIME" /D "NO_ASM" /D "ZIPLIB" /D "WINDLL" /D "API" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Zip32 - Win32 Debug"

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
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_EF_UT_TIME" /D "NO_ASM" /D "ZIPLIB" /D "WINDLL" /D "API" /YX /FD /c
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

SOURCE=..\..\..\zip\windll\windll.rc
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\zip.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\zipfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\zip\windll\ziplib.def
# End Source File
# Begin Source File

SOURCE=\Wiz\zip\zipup.c
# End Source File
# End Target
# End Project
