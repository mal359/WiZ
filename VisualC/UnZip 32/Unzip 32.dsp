# Microsoft Developer Studio Project File - Name="Unzip 32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Unzip 32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Unzip 32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Unzip 32.mak" CFG="Unzip 32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Unzip 32 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Unzip 32 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Unzip 32 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "USE_EF_UT_TIME" /D "REENTRANT" /D "LARGE_FILE_SUPPORT" /D "ZIP64_SUPPORT" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "Unzip 32 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "USE_EF_UT_TIME" /D "REENTRANT" /D "LARGE_FILE_SUPPORT" /D "ZIP64_SUPPORT" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Unzip 32 - Win32 Release"
# Name "Unzip 32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\unzip\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\crctab.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\crypt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\envargs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\explode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\extract.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\fileio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\globals.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\inflate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\match.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\win32\nt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\process.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\ttyio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\unreduce.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\unshrink.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\unzip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\win32\win32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\win32\win32i64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\zipinfo.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\unzip\consts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\crypt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\globals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\inflate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\win32\nt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\win32\rsxntwin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\tables.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\ttyio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\unzip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\unzpriv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\unzvers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\version.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\win32\w32cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\win32\w32i64.h
# End Source File
# Begin Source File

SOURCE=..\..\..\unzip\zip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
