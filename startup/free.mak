CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/visiedit.exe"
LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/su.exe"

RSC_PROJ=/l 0x409 /fo"free\su.res" /d "NDEBUG"

free\su.exe : free\su.obj free\su.res
    link $(LINK32_FLAGS) free\su.obj free\su.res

free\su.obj : su.cpp
    cl $(CPP_PROJ) su.cpp

free\su.res : su.rc
    rc $(RSC_PROJ) su.rc

