CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"./" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /NODEFAULTLIB:LIBC.LIB ..\freelib\dundas.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ugtest.exe"

RSC_PROJ=/l 0x409 /fo"ugtest.res" /d "NDEBUG"

ugtest.exe : ugtest.obj ugtest.res
    link $(LINK32_FLAGS) ugtest.obj ugtest.res

ugtest.obj : ugtest.cpp
    cl $(CPP_PROJ) ugtest.cpp

ugtest.res : ugtest.rc
    rc $(RSC_PROJ) ugtest.rc

