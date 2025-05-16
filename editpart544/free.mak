CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib ..\freelib\dundas.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/editpart.exe"

RSC_PROJ=/l 0x409 /fo"free\editpart.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=editpart.rc
!ENDIF

free\editpart.exe : free\editpart.obj free\choose_wire.obj free\partdata.obj free\edparam.obj  free\drawstr.obj \
    free\save_dos.obj free\calc.obj free\editpart.res
    link $(LINK32_FLAGS) free\editpart.obj free\choose_wire.obj free\partdata.obj free\edparam.obj \
    free\drawstr.obj free\save_dos.obj free\calc.obj free\editpart.res
    copy free\editpart.exe \v5\exes\editpart.exe

free\editpart.obj : editpart.cpp
    cl $(CPP_PROJ) editpart.cpp

free\choose_wire.obj : choose_wire.cpp
    cl $(CPP_PROJ) choose_wire.cpp

free\partdata.obj : partdata.cpp
    cl $(CPP_PROJ) partdata.cpp

free\edparam.obj  : edparam.cpp
    cl $(CPP_PROJ) edparam.cpp

free\drawstr.obj  : drawstr.obj
    cl $(CPP_PROJ) drawstr.cpp

free\save_dos.obj : save_dos.cpp
    cl $(CPP_PROJ) save_dos.cpp

free\calc.obj : calc.cpp
    cl $(CPP_PROJ) calc.cpp

free\editpart.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
