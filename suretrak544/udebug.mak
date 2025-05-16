CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"udebug\suretrak.pch" /YX"windows.h" /Fo"udebug/" /Fd"udebug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\udebuglib\v5subs.lib ..\udebuglib\ftclass.lib /nologo /subsystem:windows /incremental:no /pdb:"udebug\SureTrak.pdb" /NODEFAULTLIB:LIBCMT.LIB /debug /machine:I386 /out:"udebug/suretrak.exe"

RSC_PROJ=/l 0x409 /fo"udebug\suretrak.res" /d "NDEBUG"

udebug\suretrak.exe : udebug\asensor.obj udebug\cc.obj udebug\curpart.obj \
    udebug\profile.obj udebug\realtime.obj udebug\suretrak.obj udebug\xdcrtest.obj udebug\suretrak.res
    link $(LINK32_FLAGS) udebug\asensor.obj udebug\cc.obj udebug\curpart.obj \
    udebug\profile.obj udebug\realtime.obj udebug\suretrak.obj udebug\xdcrtest.obj udebug\suretrak.res
    copy udebug\suretrak.exe \v5wide\exes

udebug\asensor.obj : asensor.cpp
    cl $(CPP_PROJ) asensor.cpp

udebug\cc.obj : cc.cpp
    cl $(CPP_PROJ) cc.cpp

udebug\curpart.obj : curpart.cpp
    cl $(CPP_PROJ) curpart.cpp

udebug\profile.obj : profile.cpp
    cl $(CPP_PROJ) profile.cpp

udebug\realtime.obj : realtime.cpp
    cl $(CPP_PROJ) realtime.cpp

udebug\suretrak.obj : suretrak.cpp
    cl $(CPP_PROJ) suretrak.cpp

udebug\xdcrtest.obj : xdcrtest.cpp
    cl $(CPP_PROJ) xdcrtest.cpp

udebug\suretrak.res : suretrak.rc
    rc $(RSC_PROJ) suretrak.rc

