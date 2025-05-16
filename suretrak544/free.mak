CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/suretrak.exe"

RSC_PROJ=/l 0x409 /fo"free\suretrak.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=suretrak.rc
!ENDIF

free\suretrak.exe : free\asensor.obj free\cc.obj free\curpart.obj \
    free\profile.obj free\realtime.obj free\suretrak.obj free\xdcrtest.obj free\suretrak.res
    link $(LINK32_FLAGS) free\asensor.obj free\cc.obj free\curpart.obj \
    free\profile.obj free\realtime.obj free\suretrak.obj free\xdcrtest.obj \
    ..\freelib\ftclasnt.lib free\suretrak.res
    copy free\suretrak.exe \v5\exes\suretrnt.exe
    copy free\suretrak.exe \v5\exes\suretrak.exe
    link $(LINK32_FLAGS) free\asensor.obj free\cc.obj free\curpart.obj \
    free\profile.obj free\realtime.obj free\suretrak.obj free\xdcrtest.obj \
    ..\freelib\ftclas95.lib free\suretrak.res
    copy free\suretrak.exe \v5\exes\suretr95.exe

free\asensor.obj : asensor.cpp
    cl $(CPP_PROJ) asensor.cpp

free\cc.obj : cc.cpp
    cl $(CPP_PROJ) cc.cpp

free\curpart.obj : curpart.cpp
    cl $(CPP_PROJ) curpart.cpp

free\profile.obj : profile.cpp
    cl $(CPP_PROJ) profile.cpp

free\realtime.obj : realtime.cpp
    cl $(CPP_PROJ) realtime.cpp

free\suretrak.obj : suretrak.cpp
    cl $(CPP_PROJ) suretrak.cpp

free\xdcrtest.obj : xdcrtest.cpp
    cl $(CPP_PROJ) xdcrtest.cpp

free\suretrak.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)

