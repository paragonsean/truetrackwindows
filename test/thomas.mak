CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"thomas.exe"

RSC_PROJ=/l 0x409 /fo"thomas.res" /d "NDEBUG"

thomas.exe : thomas.obj thomas.res
    link $(LINK32_FLAGS) thomas.obj thomas.res

thomas.obj : thomas.cpp
    cl $(CPP_PROJ) thomas.cpp

thomas.res : thomas.rc
    rc $(RSC_PROJ) thomas.rc
