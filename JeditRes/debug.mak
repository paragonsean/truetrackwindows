CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/jeditres.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\jeditres.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/jeditres.exe"

RSC_PROJ=/l 0x409 /fo"debug\jeditres.res"

debug\jeditres.exe : debug\jeditres.obj debug\windowlist.obj debug\vtwcontrol.obj debug\jeditres.res
    link $(LINK32_FLAGS) debug\jeditres.obj debug\windowlist.obj debug\vtwcontrol.obj debug\jeditres.res

debug\jeditres.obj : jeditres.cpp windowlist.h vtwcontrol.h
    cl $(CPP_PROJ) jeditres.cpp

debug\windowlist.obj : windowlist.cpp windowlist.h
    cl $(CPP_PROJ) windowlist.cpp

debug\vtwcontrol.obj : vtwcontrol.cpp vtwcontrol.h
    cl $(CPP_PROJ) vtwcontrol.cpp

debug\jeditres.res : jeditres.rc
    rc $(RSC_PROJ) jeditres.rc
