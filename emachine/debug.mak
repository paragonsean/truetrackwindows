CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/emachine.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\debuglib\v5subs.lib ..\debuglib\dundas.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\emachine.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/emachine.exe"

RSC_PROJ=/l 0x409 /fo"debug\emachine.res"

debug\emachine.exe : debug\editdiff.obj debug\emachine.obj debug\emachine.res
    link $(LINK32_FLAGS) debug\editdiff.obj debug\emachine.obj debug\emachine.res
    copy debug\emachine.exe \v5\exes

debug\emachine.obj : emachine.cpp
    cl $(CPP_PROJ) emachine.cpp

debug\editdiff.obj : editdiff.cpp
    cl $(CPP_PROJ) editdiff.cpp

debug\emachine.res : emachine.rc
    rc $(RSC_PROJ) emachine.rc