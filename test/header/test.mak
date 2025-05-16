CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"test.exe"

RSC_PROJ=/l 0x409 /fo"test.res" /d "NDEBUG"

test.exe : test.obj test.res
    link $(LINK32_FLAGS) test.obj test.res

test.obj : test.cpp
    cl $(CPP_PROJ) test.cpp

test.res : test.rc
    rc $(RSC_PROJ) test.rc
