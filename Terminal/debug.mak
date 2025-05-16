CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/terminal.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\terminal.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/terminal.exe"

RSC_PROJ=/l 0x409 /fo"debug\terminal.res"

debug\terminal.exe : debug\terminal.obj debug\socket_monitor.obj debug\socket_class.obj debug\terminal.res
    link $(LINK32_FLAGS) debug\terminal.obj debug\socket_monitor.obj debug\socket_class.obj debug\terminal.res

debug\terminal.obj : terminal.cpp
    cl $(CPP_PROJ) terminal.cpp

debug\socket_monitor.obj : socket_monitor.cpp
    cl $(CPP_PROJ) socket_monitor.cpp

debug\socket_class.obj : socket_class.cpp socket_class.h
    cl $(CPP_PROJ) socket_class.cpp

debug\terminal.res : terminal.rc
    rc $(RSC_PROJ) terminal.rc
