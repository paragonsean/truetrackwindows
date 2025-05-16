CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/serialtest.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\serialtest.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/serialtest.exe"

RSC_PROJ=/l 0x409 /fo"debug\serialtest.res"

debug\serialtest.exe : debug\serialtest.obj debug\serialtest.res
    link $(LINK32_FLAGS) debug\serialtest.obj debug\serialtest.res
    copy debug\serialtest.exe \v5\exes

debug\serialtest.obj : serialtest.cpp
    cl $(CPP_PROJ) serialtest.cpp

debug\serialtest.res : serialtest.rc
    rc $(RSC_PROJ) serialtest.rc
