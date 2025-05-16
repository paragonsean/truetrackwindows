CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"udebug\monall.pch" /YX"windows.h" /Fo"udebug/" /Fd"udebug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\udebuglib\v5subs.lib ..\udebuglib\fastrak.lib /nologo /subsystem:windows /incremental:no /pdb:"udebug\monall.pdb" /NODEFAULTLIB:libcmt.lib /DEBUG /machine:I386 /out:"udebug/monall.exe"

RSC_PROJ=/l 0x409 /fo"udebug\monall.res"

udebug\monall.exe : udebug\bid.obj udebug\boards.obj udebug\control.obj udebug\down.obj udebug\findindx.obj \
    udebug\getdata.obj udebug\hardware.obj udebug\ioex4.obj udebug\monall.obj udebug\nextshot.obj \
    udebug\sensor.obj udebug\shotsave.obj udebug\shottime.obj udebug\skip.obj udebug\wparam.obj \
    udebug\wprofile.obj udebug\xdcrtest.obj udebug\monall.res
    link $(LINK32_FLAGS) udebug\bid.obj udebug\boards.obj udebug\control.obj udebug\down.obj udebug\findindx.obj \
    udebug\getdata.obj udebug\hardware.obj udebug\ioex4.obj udebug\monall.obj udebug\nextshot.obj \
    udebug\sensor.obj udebug\shotsave.obj udebug\shottime.obj udebug\skip.obj udebug\wparam.obj \
    udebug\wprofile.obj udebug\xdcrtest.obj udebug\monall.res
    copy udebug\monall.exe \v5wide\exes

udebug\bid.obj : bid.cpp
    cl $(CPP_PROJ) bid.cpp

udebug\boards.obj : boards.cpp
    cl $(CPP_PROJ) boards.cpp

udebug\control.obj : control.cpp
    cl $(CPP_PROJ) control.cpp

udebug\down.obj : down.cpp
    cl $(CPP_PROJ) down.cpp

udebug\findindx.obj : findindx.cpp
    cl $(CPP_PROJ) findindx.cpp

udebug\getdata.obj : getdata.cpp
    cl $(CPP_PROJ) getdata.cpp

udebug\hardware.obj : hardware.cpp
    cl $(CPP_PROJ) hardware.cpp

udebug\ioex4.obj : ioex4.cpp
    cl $(CPP_PROJ) ioex4.cpp

udebug\monall.obj : monall.cpp
    cl $(CPP_PROJ) monall.cpp

udebug\nextshot.obj : nextshot.cpp
    cl $(CPP_PROJ) nextshot.cpp

udebug\sensor.obj : sensor.cpp
    cl $(CPP_PROJ) sensor.cpp

udebug\shotsave.obj : shotsave.cpp
    cl $(CPP_PROJ) shotsave.cpp

udebug\shottime.obj : shottime.cpp
    cl $(CPP_PROJ) shottime.cpp

udebug\skip.obj : skip.cpp
    cl $(CPP_PROJ) skip.cpp

udebug\wparam.obj : wparam.cpp
    cl $(CPP_PROJ) wparam.cpp

udebug\wprofile.obj : wprofile.cpp
    cl $(CPP_PROJ) wprofile.cpp

udebug\xdcrtest.obj : xdcrtest.cpp
    cl $(CPP_PROJ) xdcrtest.cpp

udebug\monall.res : monall.rc
    rc $(RSC_PROJ) monall.rc

