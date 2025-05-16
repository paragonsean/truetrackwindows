# Microsoft Developer Studio Project File - Name="EMachine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=EMachine - Win32 UDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EMachine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EMachine.mak" CFG="EMachine - Win32 UDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EMachine - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "EMachine - Win32 UFree" (based on "Win32 (x86) External Target")
!MESSAGE "EMachine - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE "EMachine - Win32 UDebug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "EMachine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Cmd_Line "NMAKE /f debug.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "emachine.exe"
# PROP BASE Bsc_Name "EMachine.bsc"
# PROP BASE Target_Dir "debug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "emachine.exe"
# PROP Bsc_Name ""
# PROP Target_Dir "debug"

!ELSEIF  "$(CFG)" == "EMachine - Win32 UFree"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UFree"
# PROP BASE Intermediate_Dir "UFree"
# PROP BASE Cmd_Line "nmake /f "debug.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "debug\EMachine.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UFree"
# PROP Intermediate_Dir "UFree"
# PROP Cmd_Line "nmake /f "ufree.mak""
# PROP Rebuild_Opt ""
# PROP Target_File "ufree\emachine.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "EMachine - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Cmd_Line "nmake /f "debug.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "debug\EMachine.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Free"
# PROP Intermediate_Dir "Free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt ""
# PROP Target_File "free\emachine.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "EMachine - Win32 UDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UDebug"
# PROP BASE Intermediate_Dir "UDebug"
# PROP BASE Cmd_Line "nmake /f "debug.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "debug\EMachine.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebug"
# PROP Intermediate_Dir "UDebug"
# PROP Cmd_Line "nmake /f "udebug.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "udebug\emachine.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "EMachine - Win32 Debug"
# Name "EMachine - Win32 UFree"
# Name "EMachine - Win32 Free"
# Name "EMachine - Win32 UDebug"

!IF  "$(CFG)" == "EMachine - Win32 Debug"

!ELSEIF  "$(CFG)" == "EMachine - Win32 UFree"

!ELSEIF  "$(CFG)" == "EMachine - Win32 Free"

!ELSEIF  "$(CFG)" == "EMachine - Win32 UDebug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\editdiff.cpp
# End Source File
# Begin Source File

SOURCE=.\EMACHINE.CPP
# End Source File
# Begin Source File

SOURCE=.\EMACHINE.RC
# End Source File
# Begin Source File

SOURCE=.\emachinec.rc
# End Source File
# Begin Source File

SOURCE=.\emachineg.rc
# End Source File
# Begin Source File

SOURCE=.\emachinej.rc
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

SOURCE=.\resrc2.h
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

SOURCE=.\digital_servo_amp.bmp
# End Source File
# Begin Source File

SOURCE=.\emachine.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\mvo_06596.bmp
# End Source File
# End Group
# End Target
# End Project
