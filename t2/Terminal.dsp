# Microsoft Developer Studio Project File - Name="Terminal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Terminal - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Terminal.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Terminal.mak" CFG="Terminal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Terminal - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE "Terminal - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "Terminal - Win32 UFree" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "Terminal - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "terminal.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Free"
# PROP Intermediate_Dir "Free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "free\terminal.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Terminal - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f debug.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "terminal.exe"
# PROP BASE Bsc_Name "terminal.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "terminal.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Terminal - Win32 UFree"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ufree"
# PROP BASE Intermediate_Dir "ufree"
# PROP BASE Cmd_Line "nmake /f "ufree.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "terminal.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ufree"
# PROP Intermediate_Dir "ufree"
# PROP Cmd_Line "nmake /f "ufree.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "terminal.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Terminal - Win32 Free"
# Name "Terminal - Win32 Debug"
# Name "Terminal - Win32 UFree"

!IF  "$(CFG)" == "Terminal - Win32 Free"

!ELSEIF  "$(CFG)" == "Terminal - Win32 Debug"

!ELSEIF  "$(CFG)" == "Terminal - Win32 UFree"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\socket_monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\terminal.cpp
# End Source File
# Begin Source File

SOURCE=.\Terminal.rc
# End Source File
# Begin Source File

SOURCE=.\Terminalc.rc
# End Source File
# Begin Source File

SOURCE=.\Terminalg.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resrc1.h
# End Source File
# Begin Source File

SOURCE=.\resrc3.h
# End Source File
# Begin Source File

SOURCE=.\resrcg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\terminal.ico
# End Source File
# End Group
# End Target
# End Project
