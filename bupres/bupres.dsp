# Microsoft Developer Studio Project File - Name="bupres" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=bupres - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bupres.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bupres.mak" CFG="bupres - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bupres - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "bupres - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE "bupres - Win32 UFree" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "bupres - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f bupres.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "bupres.exe"
# PROP BASE Bsc_Name "bupres.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "debug\bupres.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "bupres - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "free"
# PROP BASE Intermediate_Dir "free"
# PROP BASE Cmd_Line "nmake /f free.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "free\bupres.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "free"
# PROP Intermediate_Dir "free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "free\bupres.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "bupres - Win32 UFree"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ufree"
# PROP BASE Intermediate_Dir "ufree"
# PROP BASE Cmd_Line "nmake /f "ufree.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ufree\bupres.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ufree"
# PROP Intermediate_Dir "ufree"
# PROP Cmd_Line "nmake /f "ufree.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "ufree\bupres.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "bupres - Win32 Debug"
# Name "bupres - Win32 Free"
# Name "bupres - Win32 UFree"

!IF  "$(CFG)" == "bupres - Win32 Debug"

!ELSEIF  "$(CFG)" == "bupres - Win32 Free"

!ELSEIF  "$(CFG)" == "bupres - Win32 UFree"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BACKUP.CPP
# End Source File
# Begin Source File

SOURCE=.\BUPRES.CPP
# End Source File
# Begin Source File

SOURCE=.\Bupres.rc
# End Source File
# Begin Source File

SOURCE=.\bupresc.rc
# End Source File
# Begin Source File

SOURCE=.\bupresg.rc
# End Source File
# Begin Source File

SOURCE=.\bupresj.rc
# End Source File
# Begin Source File

SOURCE=.\dd.cpp
# End Source File
# Begin Source File

SOURCE=.\delete.cpp
# End Source File
# Begin Source File

SOURCE=.\fill.cpp
# End Source File
# Begin Source File

SOURCE=.\loadzip.cpp
# End Source File
# Begin Source File

SOURCE=.\purge.cpp
# End Source File
# Begin Source File

SOURCE=.\renum.cpp
# End Source File
# Begin Source File

SOURCE=.\restore.cpp
# End Source File
# Begin Source File

SOURCE=.\save.cpp
# End Source File
# Begin Source File

SOURCE=.\tb.cpp
# End Source File
# Begin Source File

SOURCE=.\tr.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\extern.h
# End Source File
# Begin Source File

SOURCE=.\fill.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resrc1.h
# End Source File
# Begin Source File

SOURCE=.\resrc2.h
# End Source File
# Begin Source File

SOURCE=.\resrcg.h
# End Source File
# Begin Source File

SOURCE=.\save.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bupres.ico
# End Source File
# End Group
# End Target
# End Project
