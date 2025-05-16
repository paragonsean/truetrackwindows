CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/jeditres.exe"

RSC_PROJ=/l 0x409 /fo"free\jeditres.res" /d "NDEBUG"

free\jeditres.exe : free\jeditres.obj free\windowlist.obj free\vtwcontrol.obj free\jeditres.res
    link $(LINK32_FLAGS) free\jeditres.obj free\windowlist.obj free\vtwcontrol.obj free\jeditres.res

free\jeditres.obj : jeditres.cpp windowlist.h vtwcontrol.h
    cl $(CPP_PROJ) jeditres.cpp

free\windowlist.obj : windowlist.cpp windowlist.h
    cl $(CPP_PROJ) windowlist.cpp

free\vtwcontrol.obj : vtwcontrol.cpp vtwcontrol.h
    cl $(CPP_PROJ) vtwcontrol.cpp

free\jeditres.res : jeditres.rc
    rc $(RSC_PROJ) jeditres.rc
