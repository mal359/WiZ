# Microsoft Developer Studio Project File - Name="WizEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WizEdit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WizEdit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WizEdit.mak" CFG="WizEdit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WizEdit - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WizEdit - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WizEdit - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O1 /I "c:\Wiz\WizEdit" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib version.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "WizEdit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\wiz_and_dlls\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "c:\Wiz\WizEdit" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib version.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WizEdit - Win32 Release"
# Name "WizEdit - Win32 Debug"
# Begin Source File

SOURCE=\Wiz\WizEdit\about.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\dispatch.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\dllmain.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\file.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\filedlg.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\finddlg.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\format.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\init.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\mdichild.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\misc.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\preview.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\print.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\printdlg.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\rtf.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\ruler.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\search.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\statbar.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\toolbar.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\WizEdit.c
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\WizEdit.def
# End Source File
# Begin Source File

SOURCE=\Wiz\WizEdit\WizEdit.rc
# End Source File
# End Target
# End Project
