CPP_PROJ=/nologo /MTd /W3 /Od /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug\ftii.pch" /YX"windows.h"  /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib ..\debuglib\v5subs.lib /NODEFAULTLIB:LIBC.LIB /map /nologo /subsystem:windows /incremental:no /pdb:"debug\ft2.pdb" /debug /machine:I386 /out:"debug/ft2.exe"

RSC_PROJ=/l 0x409 /fo"debug\ft2.res"

debug\ft2.exe : debug\ft2.obj debug\board_monitor.obj debug\shotsave.obj debug\skip.obj debug\ft2.res
    link $(LINK32_FLAGS) debug\ft2.obj debug\board_monitor.obj debug\shotsave.obj debug\skip.obj debug\ft2.res
    copy debug\ft2.exe \v5\exes
    copy debug\ft2.exe \v5\exes\monall.exe

debug\ft2.obj : ft2.cpp ..\include\ftii_shot_data.h shotsave.h
    cl $(CPP_PROJ) ft2.cpp

debug\board_monitor.obj : board_monitor.cpp ..\include\ftii_shot_data.h shotsave.h
    cl $(CPP_PROJ) board_monitor.cpp

debug\shotsave.obj : shotsave.cpp shotsave.h
    cl $(CPP_PROJ) shotsave.cpp

debug\skip.obj : skip.cpp
    cl $(CPP_PROJ) skip.cpp

debug\ft2.res : ft2.rc
    rc $(RSC_PROJ) ft2.rc

