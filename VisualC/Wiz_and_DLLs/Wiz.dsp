# Microsoft Developer Studio Project File - Name="Wiz" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Wiz - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Wiz.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wiz.mak" CFG="Wiz - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Wiz - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Wiz - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Wiz - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "API" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib comctl32.lib /nologo /subsystem:windows /machine:I386 /libpath:"c:\projects\Wiz\release"

!ELSEIF  "$(CFG)" == "Wiz - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "API" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"c:\projects\Wiz\debug"

!ENDIF 

# Begin Target

# Name "Wiz - Win32 Release"
# Name "Wiz - Win32 Debug"
# Begin Source File

SOURCE=..\..\icons\a.ico
# End Source File
# Begin Source File

SOURCE=\Wiz\about.c
# End Source File
# Begin Source File

SOURCE=\Wiz\action.c
# End Source File
# Begin Source File

SOURCE=..\..\icons\awiz.ico
# End Source File
# Begin Source File

SOURCE=..\..\icons\child.ico
# End Source File
# Begin Source File

SOURCE=\Wiz\cmds.c
# End Source File
# Begin Source File

SOURCE=\Wiz\comment.c
# End Source File
# Begin Source File

SOURCE=\Wiz\copyfile.c
# End Source File
# Begin Source File

SOURCE=\Wiz\diff.c
# End Source File
# Begin Source File

SOURCE=\Wiz\dispatch.c
# End Source File
# Begin Source File

SOURCE=\Wiz\display.c
# End Source File
# Begin Source File

SOURCE=\Wiz\filefind.c
# End Source File
# Begin Source File

SOURCE=\Wiz\getdate.c
# End Source File
# Begin Source File

SOURCE=\Wiz\getdir.c
# End Source File
# Begin Source File

SOURCE=\Wiz\getfiles.c
# End Source File
# Begin Source File

SOURCE=\Wiz\grep.c
# End Source File
# Begin Source File

SOURCE=..\..\icons\help.cur
# End Source File
# Begin Source File

SOURCE=\Wiz\makezip.c
# End Source File
# Begin Source File

SOURCE=\wiz\unzip\match.c
# End Source File
# Begin Source File

SOURCE=\Wiz\mdichild.c
# End Source File
# Begin Source File

SOURCE=\Wiz\misc.c
# End Source File
# Begin Source File

SOURCE=\Wiz\password.c
# End Source File
# Begin Source File

SOURCE=\Wiz\pattern.c
# End Source File
# Begin Source File

SOURCE=\Wiz\property.c
# End Source File
# Begin Source File

SOURCE=\Wiz\registry.c
# End Source File
# Begin Source File

SOURCE=\Wiz\rename.c
# End Source File
# Begin Source File

SOURCE=\Wiz\replace.c
# End Source File
# Begin Source File

SOURCE=\Wiz\seldir.c
# End Source File
# Begin Source File

SOURCE=\Wiz\service.c
# End Source File
# Begin Source File

SOURCE=\Wiz\sizewndw.c
# End Source File
# Begin Source File

SOURCE=\Wiz\sound.c
# End Source File
# Begin Source File

SOURCE=..\..\icons\sounder.ico
# End Source File
# Begin Source File

SOURCE=\Wiz\splash.c
# End Source File
# Begin Source File

SOURCE=\Wiz\statbar.c
# End Source File
# Begin Source File

SOURCE=\Wiz\strings.c
# End Source File
# Begin Source File

SOURCE=\Wiz\tabctrl.c
# End Source File
# Begin Source File

SOURCE=\Wiz\toolbar.c
# End Source File
# Begin Source File

SOURCE=..\..\icons\unzipped.ico
# End Source File
# Begin Source File

SOURCE=\Wiz\unzopts.c
# End Source File
# Begin Source File

SOURCE=\Wiz\updatelb.c
# End Source File
# Begin Source File

SOURCE=\Wiz\winit.c
# End Source File
# Begin Source File

SOURCE=\Wiz\wiz.rc
# End Source File
# Begin Source File

SOURCE=\Wiz\wizmain.c
# End Source File
# Begin Source File

SOURCE=\Wiz\wndproc.c
# End Source File
# Begin Source File

SOURCE=\Wiz\zipopts.c
# End Source File
# Begin Source File

SOURCE=..\..\icons\zipped.ico
# End Source File
# End Target
# End Project
