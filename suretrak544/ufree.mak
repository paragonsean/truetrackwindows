CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib ..\ufreelib\ftclass.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/suretrak.exe"

RSC_PROJ=/l 0x409 /fo"ufree\suretrak.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=suretrak.rc
!ENDIF

ufree\suretrak.exe : ufree\asensor.obj ufree\cc.obj ufree\curpart.obj \
    ufree\profile.obj ufree\realtime.obj ufree\suretrak.obj ufree\xdcrtest.obj ufree\suretrak.res
    link $(LINK32_FLAGS) ufree\asensor.obj ufree\cc.obj ufree\curpart.obj \
    ufree\profile.obj ufree\realtime.obj ufree\suretrak.obj ufree\xdcrtest.obj ufree\suretrak.res
    copy ufree\suretrak.exe \v5wide\exes

ufree\asensor.obj : asensor.cpp
    cl $(CPP_PROJ) asensor.cpp

ufree\cc.obj : cc.cpp
    cl $(CPP_PROJ) cc.cpp

ufree\curpart.obj : curpart.cpp
    cl $(CPP_PROJ) curpart.cpp

ufree\profile.obj : profile.cpp
    cl $(CPP_PROJ) profile.cpp

ufree\realtime.obj : realtime.cpp
    cl $(CPP_PROJ) realtime.cpp

ufree\suretrak.obj : suretrak.cpp
    cl $(CPP_PROJ) suretrak.cpp

ufree\xdcrtest.obj : xdcrtest.cpp
    cl $(CPP_PROJ) xdcrtest.cpp

ufree\suretrak.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)

