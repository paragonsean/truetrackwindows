CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/terminal.exe"

RSC_PROJ=/l 0x409 /fo"free\terminal.res" /d "NDEBUG"

free\terminal.exe : free\terminal.obj free\socket_monitor.obj free\socket_class.obj free\terminal.res
    link /MAP $(LINK32_FLAGS) free\terminal.obj free\socket_monitor.obj free\socket_class.obj free\terminal.res
    copy free\terminal.exe \v5\exes

free\terminal.obj : terminal.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h socket_class.h
    cl $(CPP_PROJ) terminal.cpp

free\socket_monitor.obj : socket_monitor.cpp
    cl $(CPP_PROJ) socket_monitor.cpp

free\socket_class.obj : socket_class.cpp socket_class.h
    cl $(CPP_PROJ) socket_class.cpp

free\terminal.res : terminal.rc
    rc $(RSC_PROJ) terminal.rc
