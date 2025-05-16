CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/partfix.exe"

RSC_PROJ=/l 0x409 /fo"free\partfix.res" /d "NDEBUG"

free\partfix.exe : free\partfix.obj free\partfix.res
    link $(LINK32_FLAGS) free\partfix.obj free\partfix.res
    copy free\partfix.exe \v5\exes

free\partfix.obj : partfix.cpp
    cl $(CPP_PROJ) partfix.cpp

free\partfix.res : partfix.rc
    rc $(RSC_PROJ) partfix.rc
