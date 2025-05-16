CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/dsrestor.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\dsrestor.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/dsrestor.exe"

RSC_PROJ=/l 0x409 /fo"debug\dsrestor.res"

debug\dsrestor.exe : debug\dsrestor.obj debug\proclass.obj debug\dsrestor.res
    link $(LINK32_FLAGS) debug\dsrestor.obj debug\proclass.obj debug\dsrestor.res
    copy debug\dsrestor.exe \v5\exes

debug\dsrestor.obj : dsrestor.cpp
    cl $(CPP_PROJ) dsrestor.cpp

debug\proclass.obj : proclass.cpp
    cl $(CPP_PROJ) proclass.cpp

debug\dsrestor.res : dsrestor.rc
    rc $(RSC_PROJ) dsrestor.rc
