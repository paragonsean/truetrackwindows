CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/machlist.exe"

RSC_PROJ=/l 0x409 /fo"free\machlist.res" /d "NDEBUG"

free\machlist.exe : free\machlist.obj free\machlist.res
    link $(LINK32_FLAGS) free\machlist.obj free\machlist.res
    copy free\machlist.exe \v5\exes

free\machlist.obj : machlist.cpp
    cl $(CPP_PROJ) machlist.cpp

free\machlist.res : machlist.rc
    rc $(RSC_PROJ) machlist.rc
