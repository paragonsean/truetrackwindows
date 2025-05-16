CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib ..\freelib\v5subs.lib ..\freelib\ft95.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/fserver.exe"

RSC_PROJ=/l 0x409 /fo"free\fserver.res" /d "NDEBUG"

free\fserver.exe : free\fserver.obj free\sockserv.obj free\board.obj free\fserver.res
    link $(LINK32_FLAGS) free\fserver.obj free\sockserv.obj free\board.obj free\fserver.res

free\fserver.obj : fserver.cpp
    cl $(CPP_PROJ) fserver.cpp

free\sockserv.obj : sockserv.cpp
    cl $(CPP_PROJ) sockserv.cpp

free\board.obj : board.cpp
    cl $(CPP_PROJ) board.cpp

free\fserver.res : fserver.rc
    rc $(RSC_PROJ) fserver.rc
