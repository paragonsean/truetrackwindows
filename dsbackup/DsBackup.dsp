# Microsoft Developer Studio Project File - Name="DsBackup" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=DsBackup - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DsBackup.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DsBackup.mak" CFG="DsBackup - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DsBackup - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "DsBackup - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE "DsBackup - Win32 Ufree" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "DsBackup - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f DsBackup.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "DsBackup.exe"
# PROP BASE Bsc_Name "DsBackup.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt ""
# PROP Target_File "debug\DsBackup.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "DsBackup - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt ""
# PROP BASE Target_File "free\DsBackup.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Free"
# PROP Intermediate_Dir "Free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt ""
# PROP Target_File "free\DsBackup.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "DsBackup - Win32 Ufree"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DsBackup___Win32_Ufree"
# PROP BASE Intermediate_Dir "DsBackup___Win32_Ufree"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt ""
# PROP BASE Target_File "free\DsBackup.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DsBackup___Win32_Ufree"
# PROP Intermediate_Dir "DsBackup___Win32_Ufree"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt ""
# PROP Target_File "free\DsBackup.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "DsBackup - Win32 Debug"
# Name "DsBackup - Win32 Free"
# Name "DsBackup - Win32 Ufree"

!IF  "$(CFG)" == "DsBackup - Win32 Debug"

!ELSEIF  "$(CFG)" == "DsBackup - Win32 Free"

!ELSEIF  "$(CFG)" == "DsBackup - Win32 Ufree"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\backupcl.cpp
# End Source File
# Begin Source File

SOURCE=.\DsBackup.cpp
# End Source File
# Begin Source File

SOURCE=.\DsBackup.rc
# End Source File
# Begin Source File

SOURCE=.\DsBackupc.rc
# End Source File
# Begin Source File

SOURCE=.\dsbackupg.rc
# End Source File
# Begin Source File

SOURCE=.\global_file_settings.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\backupcl.h
# End Source File
# Begin Source File

SOURCE=.\extern.h
# End Source File
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

SOURCE=.\DsBackup.ico
# End Source File
# End Group
# End Target
# End Project
