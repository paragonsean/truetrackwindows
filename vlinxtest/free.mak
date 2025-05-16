CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/serialtest.exe"

RSC_PROJ=/l 0x409 /fo"free\serialtest.res" /d "NDEBUG"

free\serialtest.exe : free\serialtest.obj free\serialtest.res
    link $(LINK32_FLAGS) free\serialtest.obj free\serialtest.res

free\serialtest.obj : serialtest.cpp
    cl $(CPP_PROJ) serialtest.cpp

free\serialtest.res : serialtest.rc
    rc $(RSC_PROJ) serialtest.rc
