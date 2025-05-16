# Microsoft Developer Studio Project File - Name="Monall" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Monall - Win32 UFree
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Monall.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Monall.mak" CFG="Monall - Win32 UFree"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Monall - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE "Monall - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "Monall - Win32 UFree" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "Monall - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "free"
# PROP BASE Intermediate_Dir "free"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "monall.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir "free"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Free"
# PROP Intermediate_Dir "free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "monall.exe"
# PROP Bsc_Name ""
# PROP Target_Dir "free"

!ELSEIF  "$(CFG)" == "Monall - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Cmd_Line "NMAKE /f debug.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "monall.exe"
# PROP BASE Bsc_Name "CopyLog.bsc"
# PROP BASE Target_Dir "debug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt ""
# PROP Target_File "monall.exe"
# PROP Bsc_Name ""
# PROP Target_Dir "debug"

!ELSEIF  "$(CFG)" == "Monall - Win32 UFree"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ufree"
# PROP BASE Intermediate_Dir "ufree"
# PROP BASE Cmd_Line "nmake /f "ufree.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "monall.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir "ufree"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ufree"
# PROP Intermediate_Dir "ufree"
# PROP Cmd_Line "nmake /f "ufree.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "monall.exe"
# PROP Bsc_Name ""
# PROP Target_Dir "ufree"

!ENDIF 

# Begin Target

# Name "Monall - Win32 Free"
# Name "Monall - Win32 Debug"
# Name "Monall - Win32 UFree"

!IF  "$(CFG)" == "Monall - Win32 Free"

!ELSEIF  "$(CFG)" == "Monall - Win32 Debug"

!ELSEIF  "$(CFG)" == "Monall - Win32 UFree"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bid.cpp
# End Source File
# Begin Source File

SOURCE=.\BOARDS.CPP
# End Source File
# Begin Source File

SOURCE=.\control.cpp
# End Source File
# Begin Source File

SOURCE=.\down.cpp
# End Source File
# Begin Source File

SOURCE=.\findindx.cpp
# End Source File
# Begin Source File

SOURCE=.\getdata.cpp
# End Source File
# Begin Source File

SOURCE=.\hardware.cpp
# End Source File
# Begin Source File

SOURCE=.\ioex4.cpp
# End Source File
# Begin Source File

SOURCE=.\monall.cpp
# End Source File
# Begin Source File

SOURCE=.\Monall.rc
# End Source File
# Begin Source File

SOURCE=.\Monallg.rc
# End Source File
# Begin Source File

SOURCE=.\Monallj.rc
# End Source File
# Begin Source File

SOURCE=.\nextshot.cpp
# End Source File
# Begin Source File

SOURCE=.\sensor.cpp
# End Source File
# Begin Source File

SOURCE=.\shotsave.cpp
# End Source File
# Begin Source File

SOURCE=.\shottime.cpp
# End Source File
# Begin Source File

SOURCE=.\skip.cpp
# End Source File
# Begin Source File

SOURCE=.\wparam.cpp
# End Source File
# Begin Source File

SOURCE=.\wprofile.cpp
# End Source File
# Begin Source File

SOURCE=.\xdcrtest.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bid.h
# End Source File
# Begin Source File

SOURCE=.\boards.h
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

SOURCE=.\resrc2.h
# End Source File
# Begin Source File

SOURCE=.\resrcg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Monall.ico
# End Source File
# End Group
# End Target
# End Project
