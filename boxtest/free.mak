CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/boxtest.exe"

RSC_PROJ=/l 0x409 /fo"free\boxtest.res" /d "NDEBUG"

free\boxtest.exe : free\boxtest.obj free\boxtest.res
    link $(LINK32_FLAGS) free\boxtest.obj free\boxtest.res

free\boxtest.obj : boxtest.cpp
    cl $(CPP_PROJ) boxtest.cpp

free\boxtest.res : boxtest.rc
    rc $(RSC_PROJ) boxtest.rc
