# Microsoft Developer Studio Project File - Name="fastrak" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=fastrak - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fastrak.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fastrak.mak" CFG="fastrak - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fastrak - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "fastrak - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "fastrak - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f fastrak.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "fastrak.exe"
# PROP BASE Bsc_Name "fastrak.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "debug\fastrak.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "fastrak - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "fastrak___Win32_Free"
# PROP BASE Intermediate_Dir "fastrak___Win32_Free"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "..\freelib\fastrak.lib"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "fastrak___Win32_Free"
# PROP Intermediate_Dir "fastrak___Win32_Free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "..\freelib\fastrak.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "fastrak - Win32 Debug"
# Name "fastrak - Win32 Free"

!IF  "$(CFG)" == "fastrak - Win32 Debug"

!ELSEIF  "$(CFG)" == "fastrak - Win32 Free"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\FTCMD.CPP
# End Source File
# Begin Source File

SOURCE=.\FTHIGH.CPP
# End Source File
# Begin Source File

SOURCE=.\FTLOW.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
