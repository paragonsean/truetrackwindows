CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/template.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\template.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/template.exe"

RSC_PROJ=/l 0x409 /fo"debug\template.res"

debug\template.exe : debug\template.obj debug\template.res
    link $(LINK32_FLAGS) debug\template.obj debug\template.res
    copy debug\template.exe \v5\exes

debug\template.obj : template.cpp
    cl $(CPP_PROJ) template.cpp

debug\template.res : template.rc
    rc $(RSC_PROJ) template.rc
