CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"sttest.exe"

RSC_PROJ=/l 0x409 /fo"sttest.res" /d "NDEBUG"

sttest.exe : sttest.obj sttest.res
    link $(LINK32_FLAGS) sttest.obj sttest.res

sttest.obj : sttest.cpp sttest.h
    cl $(CPP_PROJ) sttest.cpp

sttest.res : sttest.rc sttest.h
    rc $(RSC_PROJ) sttest.rc
