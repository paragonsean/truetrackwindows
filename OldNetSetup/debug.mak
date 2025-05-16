CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/netsetup.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\netsetup.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/netsetup.exe"

RSC_PROJ=/l 0x409 /fo"debug\netsetup.res"

debug\netsetup.exe : debug\netsetup.obj debug\netsetup.res
    link $(LINK32_FLAGS) debug\netsetup.obj debug\netsetup.res

debug\netsetup.obj : netsetup.cpp
    cl $(CPP_PROJ) netsetup.cpp

debug\netsetup.res : netsetup.rc
    rc $(RSC_PROJ) netsetup.rc
