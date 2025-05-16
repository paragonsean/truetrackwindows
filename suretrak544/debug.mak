CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug\suretrak.pch" /YX"windows.h" /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\debuglib\v5subs.lib ..\debuglib\ftclass.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\SureTrak.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/suretrak.exe"

RSC_PROJ=/l 0x409 /fo"debug\suretrak.res" /d "NDEBUG"

debug\suretrak.exe : debug\asensor.obj debug\cc.obj debug\curpart.obj \
    debug\profile.obj debug\realtime.obj debug\suretrak.obj debug\xdcrtest.obj debug\suretrak.res
    link $(LINK32_FLAGS) debug\asensor.obj debug\cc.obj debug\curpart.obj \
    debug\profile.obj debug\realtime.obj debug\suretrak.obj debug\xdcrtest.obj debug\suretrak.res
    copy debug\suretrak.exe \v5\exes

debug\asensor.obj : asensor.cpp
    cl $(CPP_PROJ) asensor.cpp

debug\cc.obj : cc.cpp
    cl $(CPP_PROJ) cc.cpp

debug\curpart.obj : curpart.cpp
    cl $(CPP_PROJ) curpart.cpp

debug\profile.obj : profile.cpp
    cl $(CPP_PROJ) profile.cpp

debug\realtime.obj : realtime.cpp
    cl $(CPP_PROJ) realtime.cpp

debug\suretrak.obj : suretrak.cpp
    cl $(CPP_PROJ) suretrak.cpp

debug\xdcrtest.obj : xdcrtest.cpp
    cl $(CPP_PROJ) xdcrtest.cpp

debug\suretrak.res : suretrak.rc
    rc $(RSC_PROJ) suretrak.rc
