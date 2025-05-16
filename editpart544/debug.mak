CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/editpart.pch" /YX"windows.h" /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\debuglib\v5subs.lib ..\debuglib\dundas.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\editpart.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/editpart.exe"

RSC_PROJ=/l 0x409 /fo"debug\editpart.res"

debug\editpart.exe : debug\editpart.obj debug\choose_wire.obj debug\partdata.obj debug\edparam.obj  debug\drawstr.obj \
    debug\save_dos.obj debug\calc.obj debug\editpart.res
    link $(LINK32_FLAGS) debug\editpart.obj debug\choose_wire.obj debug\partdata.obj debug\edparam.obj \
    debug\drawstr.obj debug\save_dos.obj debug\calc.obj debug\editpart.res
    copy debug\editpart.exe \v5\exes

debug\editpart.obj : editpart.cpp
    cl $(CPP_PROJ) editpart.cpp

debug\choose_wire.obj : choose_wire.cpp
    cl $(CPP_PROJ) choose_wire.cpp

debug\partdata.obj : partdata.cpp
    cl $(CPP_PROJ) partdata.cpp

debug\edparam.obj  : edparam.cpp
    cl $(CPP_PROJ) edparam.cpp

debug\drawstr.obj  : drawstr.obj
    cl $(CPP_PROJ) drawstr.cpp

debug\save_dos.obj : save_dos.cpp
    cl $(CPP_PROJ) save_dos.cpp

debug\calc.obj : calc.cpp
    cl $(CPP_PROJ) calc.cpp

debug\editpart.res : editpart.rc
    rc $(RSC_PROJ) editpart.rc
