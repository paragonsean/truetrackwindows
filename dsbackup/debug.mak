CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/dsbackup.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\dsbackup.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/dsbackup.exe"

RSC_PROJ=/l 0x409 /fo"debug\dsbackup.res"

debug\dsbackup.exe : debug\global_file_settings.obj debug\dsbackup.obj debug\backupcl.obj debug\dsbackup.res
    link $(LINK32_FLAGS) debug\global_file_settings.obj debug\dsbackup.obj debug\backupcl.obj debug\dsbackup.res
    copy debug\dsbackup.exe \v5ds\exes

debug\dsbackup.obj : dsbackup.cpp
    cl $(CPP_PROJ) dsbackup.cpp

debug\backupcl.obj : backupcl.cpp
    cl $(CPP_PROJ) backupcl.cpp

debug\global_file_settings.obj : global_file_settings.cpp
    cl $(CPP_PROJ) global_file_settings.cpp

debug\dsbackup.res : dsbackup.rc
    rc $(RSC_PROJ) dsbackup.rc
