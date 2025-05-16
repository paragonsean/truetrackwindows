CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"/sources/projects/include/free.pch" /Yc"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/empty.exe"

free\empty.exe : free\empty.obj
    link $(LINK32_FLAGS) free\empty.obj

free\empty.obj : empty.cpp
    cl $(CPP_PROJ) empty.cpp
