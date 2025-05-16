CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib ..\ufreelib\dundas.lib /nologo /MAP /subsystem:windows /incremental:no /machine:I386 /out:"ufree/editpart.exe"

RSC_PROJ=/l 0x409 /fo"ufree\editpart.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=editpartc.rc
!ENDIF

ufree\editpart.exe : ufree\editpart.obj ufree\choose_wire.obj ufree\partdata.obj ufree\edparam.obj ufree\drawstr.obj \
    ufree\save_dos.obj ufree\calc.obj ufree\editpart.res
    link $(LINK32_FLAGS) ufree\editpart.obj ufree\choose_wire.obj ufree\partdata.obj ufree\edparam.obj \
    ufree\drawstr.obj ufree\save_dos.obj ufree\calc.obj ufree\editpart.res
    copy ufree\editpart.exe \v5wide\exes

ufree\editpart.obj : editpart.cpp
    cl $(CPP_PROJ) editpart.cpp

ufree\choose_wire.obj : choose_wire.cpp
    cl $(CPP_PROJ) choose_wire.cpp

ufree\partdata.obj : partdata.cpp
    cl $(CPP_PROJ) partdata.cpp

ufree\edparam.obj  : edparam.cpp
    cl $(CPP_PROJ) edparam.cpp

ufree\drawstr.obj  : drawstr.obj
    cl $(CPP_PROJ) drawstr.cpp

ufree\save_dos.obj : save_dos.cpp
    cl $(CPP_PROJ) save_dos.cpp

ufree\calc.obj : calc.cpp
    cl $(CPP_PROJ) calc.cpp

ufree\editpart.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
