CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/postdata.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib htmlhelp.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\postdata.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/postdata.exe"

RSC_PROJ=/l 0x409 /fo"debug\postdata.res"

debug\postdata.exe : debug\postdata.obj debug\update.obj debug\postdata.res
    link $(LINK32_FLAGS) debug\postdata.obj debug\update.obj debug\postdata.res
    copy debug\postdata.exe \v5\exes

debug\postdata.obj : postdata.cpp
    cl $(CPP_PROJ) postdata.cpp

debug\update.obj : update.cpp
    cl $(CPP_PROJ) update.cpp

debug\postdata.res : postdata.rc
    rc $(RSC_PROJ) postdata.rc
