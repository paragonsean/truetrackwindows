CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/monall.exe"

RSC_PROJ=/l 0x409 /fo"free\monall.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=monall.rc
!ENDIF

free\monall.exe : free\bid.obj free\boards.obj free\control.obj free\down.obj free\findindx.obj \
    free\getdata.obj free\hardware.obj free\ioex4.obj free\monall.obj free\nextshot.obj \
    free\sensor.obj free\shotsave.obj free\shottime.obj free\skip.obj free\udp_socket.obj free\wparam.obj \
    free\wprofile.obj free\xdcrtest.obj free\monall.res
    link $(LINK32_FLAGS) free\bid.obj free\boards.obj free\control.obj free\down.obj free\findindx.obj \
    free\getdata.obj free\hardware.obj free\ioex4.obj free\monall.obj free\nextshot.obj \
    free\sensor.obj free\shotsave.obj free\shottime.obj free\skip.obj free\udp_socket.obj free\wparam.obj \
    ..\freelib\fastrak.lib free\wprofile.obj free\xdcrtest.obj free\monall.res
    copy free\monall.exe \v5\exes\monallnt.exe
    copy free\monall.exe \v5\exes\monall.exe
    link $(LINK32_FLAGS) free\bid.obj free\boards.obj free\control.obj free\down.obj free\findindx.obj \
    free\getdata.obj free\hardware.obj free\ioex4.obj free\monall.obj free\nextshot.obj \
    free\sensor.obj free\shotsave.obj free\shottime.obj free\skip.obj free\udp_socket.obj free\wparam.obj \
    ..\freelib\ft95.lib free\wprofile.obj free\xdcrtest.obj free\monall.res
    copy free\monall.exe \v5\exes\monall95.exe

free\bid.obj : bid.cpp
    cl $(CPP_PROJ) bid.cpp

free\boards.obj : boards.cpp
    cl $(CPP_PROJ) boards.cpp

free\control.obj : control.cpp
    cl $(CPP_PROJ) control.cpp

free\down.obj : down.cpp
    cl $(CPP_PROJ) down.cpp

free\findindx.obj : findindx.cpp
    cl $(CPP_PROJ) findindx.cpp

free\getdata.obj : getdata.cpp
    cl $(CPP_PROJ) getdata.cpp

free\hardware.obj : hardware.cpp
    cl $(CPP_PROJ) hardware.cpp

free\ioex4.obj : ioex4.cpp
    cl $(CPP_PROJ) ioex4.cpp

free\monall.obj : monall.cpp
    cl $(CPP_PROJ) monall.cpp

free\nextshot.obj : nextshot.cpp
    cl $(CPP_PROJ) nextshot.cpp

free\sensor.obj : sensor.cpp
    cl $(CPP_PROJ) sensor.cpp

free\shotsave.obj : shotsave.cpp
    cl $(CPP_PROJ) shotsave.cpp

free\shottime.obj : shottime.cpp
    cl $(CPP_PROJ) shottime.cpp

free\skip.obj : skip.cpp
    cl $(CPP_PROJ) skip.cpp

free\udp_socket.obj : udp_socket.cpp
    cl $(CPP_PROJ) udp_socket.cpp

free\wparam.obj : wparam.cpp
    cl $(CPP_PROJ) wparam.cpp

free\wprofile.obj : wprofile.cpp
    cl $(CPP_PROJ) wprofile.cpp

free\xdcrtest.obj : xdcrtest.cpp
    cl $(CPP_PROJ) xdcrtest.cpp

free\monall.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
