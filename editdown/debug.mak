CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/EditDown.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib ..\debuglib\dundas.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\editdown.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/editdown.exe"

RSC_PROJ=/l 0x409 /fo"debug\editdown.res" /d "NDEBUG"

debug\editdown.exe : debug\editdown.obj debug\category.obj debug\editdown.res
    link $(LINK32_FLAGS) debug\editdown.obj debug\category.obj debug\editdown.res
    copy debug\editdown.exe \v5\exes

debug\editdown.obj : editdown.cpp
    cl $(CPP_PROJ) editdown.cpp

debug\category.obj : category.cpp
    cl $(CPP_PROJ) category.cpp

debug\editdown.res : editdown.rc
    rc $(RSC_PROJ) editdown.rc
