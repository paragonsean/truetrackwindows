CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib netapi32.lib ole32.lib uuid.lib wsock32.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/test.exe"

RSC_PROJ=/l 0x409 /fo"free\test.res" /d "NDEBUG"

free\test.exe : free\test.obj free\test.res
    link $(LINK32_FLAGS) free\test.obj free\test.res

free\test.obj : test.cpp
    cl $(CPP_PROJ) test.cpp

free\test.res : test.rc
    rc $(RSC_PROJ) test.rc
