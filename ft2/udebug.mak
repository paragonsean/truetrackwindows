CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"udebug\ft2.pch"    /YX"windows.h" /Fo"udebug/" /Fd"udebug\\" /c
LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib ..\udebuglib\v5subs.lib /map /nologo /subsystem:windows /incremental:no /pdb:"udebug\ft2.pdb"    /NODEFAULTLIB:libcmt.lib /DEBUG /machine:I386 /out:"udebug/ft2.exe"

RSC_PROJ=/l 0x409 /fo"udebug\ft2.res"

udebug\ft2.exe : udebug\ft2.obj udebug\board_monitor.obj udebug\shotsave.obj udebug\skip.obj udebug\ft2.res
    link $(LINK32_FLAGS) udebug\ft2.obj udebug\board_monitor.obj udebug\shotsave.obj udebug\skip.obj udebug\ft2.res
    copy udebug\ft2.exe \v5wide\exes\ft2.exe
    copy udebug\ft2.exe \v5wide\exes\monall.exe

udebug\ft2.obj : ft2.cpp ..\include\ftii.h resource.h extern.h
    cl $(CPP_PROJ) ft2.cpp

udebug\board_monitor.obj : board_monitor.cpp extern.h ..\include\ftii_shot_data.h ..\include\ftii.h shotsave.h
    cl $(CPP_PROJ) board_monitor.cpp

udebug\shotsave.obj : shotsave.cpp shotsave.h
    cl $(CPP_PROJ) shotsave.cpp

udebug\skip.obj : skip.cpp
    cl $(CPP_PROJ) skip.cpp

udebug\ft2.res : ft2.rc resource.h resrc1.h
    rc $(RSC_PROJ) ft2.rc
