CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/monall.pch" /YX"windows.h"  /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib ..\debuglib\v5subs.lib ..\debuglib\fastrak.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\monall.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/monall.exe"

RSC_PROJ=/l 0x409 /fo"debug\monall.res"

debug\monall.exe : debug\bid.obj debug\boards.obj debug\control.obj debug\down.obj debug\findindx.obj \
    debug\getdata.obj debug\hardware.obj debug\ioex4.obj debug\monall.obj debug\nextshot.obj \
    debug\sensor.obj debug\shotsave.obj debug\shottime.obj debug\skip.obj debug\wparam.obj \
    debug\wprofile.obj debug\xdcrtest.obj debug\monall.res
    link $(LINK32_FLAGS) debug\bid.obj debug\boards.obj debug\control.obj debug\down.obj debug\findindx.obj \
    debug\getdata.obj debug\hardware.obj debug\ioex4.obj debug\monall.obj debug\nextshot.obj \
    debug\sensor.obj debug\shotsave.obj debug\shottime.obj debug\skip.obj debug\wparam.obj \
    debug\wprofile.obj debug\xdcrtest.obj debug\monall.res
    copy debug\monall.exe \v5\exes

debug\bid.obj : bid.cpp
    cl $(CPP_PROJ) bid.cpp

debug\boards.obj : boards.cpp
    cl $(CPP_PROJ) boards.cpp

debug\control.obj : control.cpp
    cl $(CPP_PROJ) control.cpp

debug\down.obj : down.cpp
    cl $(CPP_PROJ) down.cpp

debug\findindx.obj : findindx.cpp
    cl $(CPP_PROJ) findindx.cpp

debug\getdata.obj : getdata.cpp
    cl $(CPP_PROJ) getdata.cpp

debug\hardware.obj : hardware.cpp
    cl $(CPP_PROJ) hardware.cpp

debug\ioex4.obj : ioex4.cpp
    cl $(CPP_PROJ) ioex4.cpp

debug\monall.obj : monall.cpp
    cl $(CPP_PROJ) monall.cpp

debug\nextshot.obj : nextshot.cpp
    cl $(CPP_PROJ) nextshot.cpp

debug\sensor.obj : sensor.cpp
    cl $(CPP_PROJ) sensor.cpp

debug\shotsave.obj : shotsave.cpp
    cl $(CPP_PROJ) shotsave.cpp

debug\shottime.obj : shottime.cpp
    cl $(CPP_PROJ) shottime.cpp

debug\skip.obj : skip.cpp
    cl $(CPP_PROJ) skip.cpp

debug\wparam.obj : wparam.cpp
    cl $(CPP_PROJ) wparam.cpp

debug\wprofile.obj : wprofile.cpp
    cl $(CPP_PROJ) wprofile.cpp

debug\xdcrtest.obj : xdcrtest.cpp
    cl $(CPP_PROJ) xdcrtest.cpp

debug\monall.res : monall.rc
    rc $(RSC_PROJ) monall.rc

