CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/template.exe"

RSC_PROJ=/l 0x409 /fo"free\template.res" /d "NDEBUG"

free\template.exe : free\template.obj free\template.res
    link $(LINK32_FLAGS) free\template.obj free\template.res

free\template.obj : template.cpp
    cl $(CPP_PROJ) template.cpp

free\template.res : template.rc
    rc $(RSC_PROJ) template.rc
