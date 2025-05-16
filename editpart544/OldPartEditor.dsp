# Microsoft Developer Studio Project File - Name="OldPartEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=OldPartEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OldPartEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OldPartEditor.mak" CFG="OldPartEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OldPartEditor - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "OldPartEditor - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE "OldPartEditor - Win32 Ufree" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Cmd_Line "NMAKE /f OldPartEditor.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "editpart.exe"
# PROP BASE Bsc_Name "OldPartEditor.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "editpart.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "editpart.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Free"
# PROP Intermediate_Dir "Free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "editpart.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ufree"
# PROP BASE Intermediate_Dir "ufree"
# PROP BASE Cmd_Line "nmake /f "ufree.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ufree\editpart.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ufree"
# PROP Intermediate_Dir "ufree"
# PROP Cmd_Line "nmake /f "ufree.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "ufree\editpart.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "OldPartEditor - Win32 Debug"
# Name "OldPartEditor - Win32 Free"
# Name "OldPartEditor - Win32 Ufree"

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CALC.CPP

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\choose_wire.cpp
# End Source File
# Begin Source File

SOURCE=.\DRAWSTR.CPP

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EDITPART.CPP

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EditPartj.rc
# End Source File
# Begin Source File

SOURCE=.\EDPARAM.CPP

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PARTDATA.CPP

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SAVE_DOS.CPP

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\PARTDATA.H

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resource.h

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resrc1.h

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

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

SOURCE=.\editpart.ico

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EditPart.rc

!IF  "$(CFG)" == "OldPartEditor - Win32 Debug"

# PROP Intermediate_Dir "debug"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Free"

# PROP Intermediate_Dir "free"

!ELSEIF  "$(CFG)" == "OldPartEditor - Win32 Ufree"

# PROP BASE Intermediate_Dir "free"
# PROP Intermediate_Dir "free"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EditPartc.rc
# End Source File
# Begin Source File

SOURCE=.\EditPartg.rc
# End Source File
# End Group
# End Target
# End Project
