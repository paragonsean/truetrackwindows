CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /Zi /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"/sources/projects/include/debug.pch" /Yc"windows.h" /Fo"debug/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"debug/empty.exe"

debug\empty.exe : debug\empty.obj
    link $(LINK32_FLAGS) debug\empty.obj

debug\empty.obj : empty.cpp
    cl $(CPP_PROJ) empty.cpp
