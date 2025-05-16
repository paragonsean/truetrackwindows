CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib ..\ufreelib\v5subs.lib ..\ufreelib\fastrak.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/monall.exe"

RSC_PROJ=/l 0x409 /fo"ufree\monall.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=monall.rc
!ENDIF

ufree\monall.exe : ufree\bid.obj ufree\boards.obj ufree\control.obj ufree\down.obj ufree\findindx.obj \
    ufree\getdata.obj ufree\hardware.obj ufree\ioex4.obj ufree\monall.obj ufree\nextshot.obj \
    ufree\sensor.obj ufree\shotsave.obj ufree\shottime.obj ufree\skip.obj ufree\udp_socket.obj ufree\wparam.obj \
    ufree\wprofile.obj ufree\xdcrtest.obj ufree\monall.res
    link $(LINK32_FLAGS) ufree\bid.obj ufree\boards.obj ufree\control.obj ufree\down.obj ufree\findindx.obj \
    ufree\getdata.obj ufree\hardware.obj ufree\ioex4.obj ufree\monall.obj ufree\nextshot.obj \
    ufree\sensor.obj ufree\shotsave.obj ufree\shottime.obj ufree\skip.obj ufree\udp_socket.obj ufree\wparam.obj \
    ufree\wprofile.obj ufree\xdcrtest.obj ufree\monall.res
    copy ufree\monall.exe \v5wide\exes

ufree\bid.obj : bid.cpp
    cl $(CPP_PROJ) bid.cpp

ufree\boards.obj : boards.cpp
    cl $(CPP_PROJ) boards.cpp

ufree\control.obj : control.cpp
    cl $(CPP_PROJ) control.cpp

ufree\down.obj : down.cpp
    cl $(CPP_PROJ) down.cpp

ufree\findindx.obj : findindx.cpp
    cl $(CPP_PROJ) findindx.cpp

ufree\getdata.obj : getdata.cpp
    cl $(CPP_PROJ) getdata.cpp

ufree\hardware.obj : hardware.cpp
    cl $(CPP_PROJ) hardware.cpp

ufree\ioex4.obj : ioex4.cpp
    cl $(CPP_PROJ) ioex4.cpp

ufree\monall.obj : monall.cpp
    cl $(CPP_PROJ) monall.cpp

ufree\nextshot.obj : nextshot.cpp
    cl $(CPP_PROJ) nextshot.cpp

ufree\sensor.obj : sensor.cpp
    cl $(CPP_PROJ) sensor.cpp

ufree\shotsave.obj : shotsave.cpp
    cl $(CPP_PROJ) shotsave.cpp

ufree\shottime.obj : shottime.cpp
    cl $(CPP_PROJ) shottime.cpp

ufree\skip.obj : skip.cpp
    cl $(CPP_PROJ) skip.cpp

ufree\udp_socket.obj : udp_socket.cpp
    cl $(CPP_PROJ) udp_socket.cpp

ufree\wparam.obj : wparam.cpp
    cl $(CPP_PROJ) wparam.cpp

ufree\wprofile.obj : wprofile.cpp
    cl $(CPP_PROJ) wprofile.cpp

ufree\xdcrtest.obj : xdcrtest.cpp
    cl $(CPP_PROJ) xdcrtest.cpp

ufree\monall.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
