CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/vtextrac.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\vtextrac.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/vtextrac.exe"

RSC_PROJ=/l 0x409 /fo"debug\vtextrac.res"

debug\vtextrac.exe : debug\vtextrac.obj debug\vtextrac.res
    link $(LINK32_FLAGS) debug\vtextrac.obj debug\vtextrac.res
    copy debug\vtextrac.exe \v5\exes

debug\vtextrac.obj : vtextrac.cpp
    cl $(CPP_PROJ) vtextrac.cpp

debug\vtextrac.res : vtextrac.rc
    rc $(RSC_PROJ) vtextrac.rc
