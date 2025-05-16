CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"debug/setup.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib                      ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\netsetup.pdb" /NODEFAULTLIB:libcmt.lib /debug  /machine:I386 /out:"debug/setup.exe"

RSC_PROJ=/l 0x409 /fo"debug\setup.res" /d "NDEBUG"

debug\setup.exe : debug\setup.obj debug\menupdat.obj debug\setup.res
    link $(LINK32_FLAGS) debug\setup.obj debug\menupdat.obj debug\setup.res

debug\setup.obj : setup.cpp
    cl $(CPP_PROJ) setup.cpp

debug\menupdat.obj : menupdat.cpp
    cl $(CPP_PROJ) menupdat.cpp

debug\setup.res : setup.rc
    rc $(RSC_PROJ) setup.rc
