CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/demo.exe"

RSC_PROJ=/l 0x409 /fo"free\demo.res" /d "NDEBUG"

free\demo.exe : free\demo.obj free\monitor.obj free\demo.res
    link $(LINK32_FLAGS) free\demo.obj free\monitor.obj free\demo.res
    copy free\demo.exe \v5\exes

free\demo.obj : demo.cpp
    cl $(CPP_PROJ) demo.cpp

free\monitor.obj : monitor.cpp
    cl $(CPP_PROJ) monitor.cpp

free\demo.res : demo.rc
    rc $(RSC_PROJ) demo.rc
