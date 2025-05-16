CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/terminal.exe"

RSC_PROJ=/l 0x409 /fo"ufree\terminal.res" /d "NDEBUG"

ufree\terminal.exe : ufree\terminal.obj ufree\socket_monitor.obj ufree\socket_class.obj ufree\terminal.res
    link /MAP $(LINK32_FLAGS) ufree\terminal.obj ufree\socket_monitor.obj ufree\socket_class.obj ufree\terminal.res
    copy ufree\terminal.exe \v5\exes

ufree\terminal.obj : terminal.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h socket_class.h
    cl $(CPP_PROJ) terminal.cpp

ufree\socket_monitor.obj : socket_monitor.cpp
    cl $(CPP_PROJ) socket_monitor.cpp

ufree\socket_class.obj : socket_class.cpp socket_class.h
    cl $(CPP_PROJ) socket_class.cpp

ufree\terminal.res : terminal.rc
    rc $(RSC_PROJ) terminal.rc
