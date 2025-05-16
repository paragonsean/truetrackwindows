CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib ..\ufreelib\v5subs.lib /map /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/ft2.exe"

RSC_PROJ=/l 0x409 /fo"ufree\ft2.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=ft2.rc
!ENDIF

ufree\ft2.exe : ufree\ft2.obj ufree\board_monitor.obj ufree\shotsave.obj ufree\skip.obj ufree\ft2.res
    link $(LINK32_FLAGS) ufree\ft2.obj ufree\board_monitor.obj ufree\shotsave.obj ufree\skip.obj ufree\ft2.res
    copy ufree\ft2.exe \v5wide\exes\ft2.exe
    copy ufree\ft2.exe \v5wide\exes\monall.exe

ufree\ft2.obj : ft2.cpp ..\include\ftii.h resource.h extern.h
    cl $(CPP_PROJ) ft2.cpp

ufree\board_monitor.obj : board_monitor.cpp extern.h ..\include\ftii_shot_data.h ..\include\ftii.h shotsave.h
    cl $(CPP_PROJ) board_monitor.cpp

ufree\shotsave.obj : shotsave.cpp shotsave.h
    cl $(CPP_PROJ) shotsave.cpp

ufree\skip.obj : skip.cpp
    cl $(CPP_PROJ) skip.cpp

ufree\ft2.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
