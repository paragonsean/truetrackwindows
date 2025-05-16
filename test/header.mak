CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"header.exe"

RSC_PROJ=/l 0x409 /fo"header.res" /d "NDEBUG"

header.exe : header.obj header.res
    link $(LINK32_FLAGS) header.obj header.res

header.obj : header.cpp
    cl $(CPP_PROJ) header.cpp

header.res : header.rc
    rc $(RSC_PROJ) header.rc
