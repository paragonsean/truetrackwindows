CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/demo.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib  ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\demo.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/demo.exe"

RSC_PROJ=/l 0x409 /fo"debug\demo.res"

debug\demo.exe : debug\demo.obj debug\monitor.obj debug\demo.res
    link $(LINK32_FLAGS) debug\demo.obj debug\monitor.obj debug\demo.res
    copy debug\demo.exe \v5\exes

debug\demo.obj : demo.cpp
    cl $(CPP_PROJ) demo.cpp

debug\monitor.obj : monitor.cpp
    cl $(CPP_PROJ) monitor.cpp

debug\demo.res : demo.rc
    rc $(RSC_PROJ) demo.rc
