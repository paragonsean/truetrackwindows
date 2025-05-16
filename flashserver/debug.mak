CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/fserver.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\fserver.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/fserver.exe"

RSC_PROJ=/l 0x409 /fo"debug\fserver.res"

debug\fserver.exe : debug\fserver.obj debug\sockserv.obj debug\bclass.obj debug\fserver.res
    link $(LINK32_FLAGS) debug\fserver.obj debug\sockserv.obj debug\bclass.obj debug\fserver.res

debug\fserver.obj : fserver.cpp
    cl $(CPP_PROJ) fserver.cpp

debug\bclass.obj : bclass.cpp bclass.h
    cl $(CPP_PROJ) bclass.cpp

debug\sockserv.obj : sockserv.cpp
    cl $(CPP_PROJ) sockserv.cpp

debug\fserver.res : fserver.rc
    rc $(RSC_PROJ) fserver.rc
