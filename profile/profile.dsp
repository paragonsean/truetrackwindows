# Microsoft Developer Studio Project File - Name="profile" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=profile - Win32 UDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "profile.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "profile.mak" CFG="profile - Win32 UDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "profile - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "profile - Win32 UDebug" (based on "Win32 (x86) External Target")
!MESSAGE "profile - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE "profile - Win32 UFree" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "profile - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f profile.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "profile.exe"
# PROP BASE Bsc_Name "profile.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "debug\profile.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "profile - Win32 UDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UDebug"
# PROP BASE Intermediate_Dir "UDebug"
# PROP BASE Cmd_Line "nmake /f "debug.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "debug\profile.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebug"
# PROP Intermediate_Dir "UDebug"
# PROP Cmd_Line "nmake /f "udebug.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "udebug\profile.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "profile - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "free\profile.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Free"
# PROP Intermediate_Dir "Free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "free\profile.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "profile - Win32 UFree"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "UFree"
# PROP BASE Intermediate_Dir "UFree"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "free\profile.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UFree"
# PROP Intermediate_Dir "UFree"
# PROP Cmd_Line "nmake /f "ufree.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "ufree\profile.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "profile - Win32 Debug"
# Name "profile - Win32 UDebug"
# Name "profile - Win32 Free"
# Name "profile - Win32 UFree"

!IF  "$(CFG)" == "profile - Win32 Debug"

!ELSEIF  "$(CFG)" == "profile - Win32 UDebug"

!ELSEIF  "$(CFG)" == "profile - Win32 Free"

!ELSEIF  "$(CFG)" == "profile - Win32 UFree"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\alarmexp.cpp
# End Source File
# Begin Source File

SOURCE=.\BITMAPCL.CPP
# End Source File
# Begin Source File

SOURCE=.\BOX.CPP
# End Source File
# Begin Source File

SOURCE=.\CHOSPART.CPP
# End Source File
# Begin Source File

SOURCE=.\CHOSSHOT.CPP
# End Source File
# Begin Source File

SOURCE=.\COLORDEF.CPP
# End Source File
# Begin Source File

SOURCE=.\COLORS.CPP
# End Source File
# Begin Source File

SOURCE=.\CORNER.CPP
# End Source File
# Begin Source File

SOURCE=.\ctrlimit.cpp
# End Source File
# Begin Source File

SOURCE=.\dbarcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\DCONFIG.CPP
# End Source File
# Begin Source File

SOURCE=.\DOWNBAR.CPP
# End Source File
# Begin Source File

SOURCE=.\editop.cpp
# End Source File
# Begin Source File

SOURCE=.\editshif.cpp
# End Source File
# Begin Source File

SOURCE=.\EXPORT.CPP
# End Source File
# Begin Source File

SOURCE=.\fixaxis.cpp
# End Source File
# Begin Source File

SOURCE=.\GRIDCLAS.CPP
# End Source File
# Begin Source File

SOURCE=.\ISALARM.CPP
# End Source File
# Begin Source File

SOURCE=.\MASTER.CPP
# End Source File
# Begin Source File

SOURCE=.\mplot.cpp
# End Source File
# Begin Source File

SOURCE=.\MULPRINT.CPP
# End Source File
# Begin Source File

SOURCE=.\PARAMHIS.CPP
# End Source File
# Begin Source File

SOURCE=.\paramwin.cpp
# End Source File
# Begin Source File

SOURCE=.\PASSWORD.CPP
# End Source File
# Begin Source File

SOURCE=.\PLOT.CPP
# End Source File
# Begin Source File

SOURCE=.\profile.cpp
# End Source File
# Begin Source File

SOURCE=.\profile.rc
# End Source File
# Begin Source File

SOURCE=.\profilec.rc
# End Source File
# Begin Source File

SOURCE=.\profileg.rc
# End Source File
# Begin Source File

SOURCE=.\profilej.rc
# End Source File
# Begin Source File

SOURCE=.\RTMLIST.CPP
# End Source File
# Begin Source File

SOURCE=.\SAVEPRO.CPP
# End Source File
# Begin Source File

SOURCE=.\status.cpp
# End Source File
# Begin Source File

SOURCE=.\TCCONFIG.CPP
# End Source File
# Begin Source File

SOURCE=.\TREND.CPP
# End Source File
# Begin Source File

SOURCE=.\ViewPara.cpp
# End Source File
# Begin Source File

SOURCE=.\vlbclass.cpp
# End Source File
# Begin Source File

SOURCE=.\XAXIS.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BITMAPCL.H
# End Source File
# Begin Source File

SOURCE=.\ctrlimit.h
# End Source File
# Begin Source File

SOURCE=.\dbarcfg.h
# End Source File
# Begin Source File

SOURCE=.\EXTERN.H
# End Source File
# Begin Source File

SOURCE=.\fixaxis.h
# End Source File
# Begin Source File

SOURCE=.\GRIDCLAS.H
# End Source File
# Begin Source File

SOURCE=.\mplot.h
# End Source File
# Begin Source File

SOURCE=..\NetSetup\msresc.h
# End Source File
# Begin Source File

SOURCE=.\password.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resrc1.h
# End Source File
# Begin Source File

SOURCE=.\resrcg.h
# End Source File
# Begin Source File

SOURCE=.\resrcj.h
# End Source File
# Begin Source File

SOURCE=.\rtmlist.h
# End Source File
# Begin Source File

SOURCE=.\tcconfig.h
# End Source File
# Begin Source File

SOURCE=.\textlen.h
# End Source File
# Begin Source File

SOURCE=.\vlbclass.h
# End Source File
# Begin Source File

SOURCE=.\xaxis.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\profile.ico
# End Source File
# Begin Source File

SOURCE=.\VisiTrakOval.bmp
# End Source File
# End Group
# End Target
# End Project
