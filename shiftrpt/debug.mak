CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/shiftrpt.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\shiftrpt.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/shiftrpt.exe"

RSC_PROJ=/l 0x409 /fo"debug\shiftrpt.res"

debug\shiftrpt.exe : debug\shiftrpt.obj debug\shiftrpt.res
    link $(LINK32_FLAGS) debug\shiftrpt.obj debug\shiftrpt.res
    copy debug\shiftrpt.exe \v5\exes

debug\shiftrpt.obj : shiftrpt.cpp resource.h
    cl $(CPP_PROJ) shiftrpt.cpp

debug\shiftrpt.res : shiftrpt.rc
    rc $(RSC_PROJ) shiftrpt.rc
