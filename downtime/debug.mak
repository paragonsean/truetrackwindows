CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/downtime.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib                      ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\downtime.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/downtime.exe"

RSC_PROJ=/l 0x409 /fo"debug\downtime.res" /d "NDEBUG"

debug\downtime.exe : debug\downtime.obj debug\category.obj debug\downtime.res
    link $(LINK32_FLAGS) debug\downtime.obj debug\category.obj debug\downtime.res
    copy debug\downtime.exe \v5\exes

debug\downtime.obj : downtime.cpp
    cl $(CPP_PROJ) downtime.cpp

debug\category.obj : category.cpp
    cl $(CPP_PROJ) category.cpp

debug\downtime.res : downtime.rc
    rc $(RSC_PROJ) downtime.rc
