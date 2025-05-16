CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/boxtest.pch" /YX"windows.h" /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\boxtest.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/boxtest.exe"

RSC_PROJ=/l 0x409 /fo"debug\boxtest.res"

debug\boxtest.exe : debug\boxtest.obj debug\boxtest.res
    link $(LINK32_FLAGS) debug\boxtest.obj debug\boxtest.res

debug\boxtest.obj : boxtest.cpp
    cl $(CPP_PROJ) boxtest.cpp

debug\boxtest.res : boxtest.rc
    rc $(RSC_PROJ) boxtest.rc
