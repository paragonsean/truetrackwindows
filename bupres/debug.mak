CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/bupres.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\bupres.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/bupres.exe"

RSC_PROJ=/l 0x409 /fo"debug\bupres.res"

debug/bupres.exe : debug/backup.obj debug/bupres.obj debug/dd.obj debug/delete.obj \
    debug/fill.obj debug/loadzip.obj debug/purge.obj debug/renum.obj debug/restore.obj \
    debug/save.obj debug/tb.obj debug/tr.obj debug/bupres.res
    link $(LINK32_FLAGS) debug/backup.obj debug/bupres.obj debug/dd.obj debug/delete.obj \
    debug/fill.obj debug/loadzip.obj debug/purge.obj debug/renum.obj debug/restore.obj \
    debug/save.obj debug/tb.obj debug/tr.obj debug/bupres.res
    copy debug\bupres.exe \v5\exes

debug/backup.obj : backup.cpp
    cl $(CPP_PROJ) backup.cpp

debug/bupres.obj : bupres.cpp
    cl $(CPP_PROJ) bupres.cpp

debug/dd.obj : dd.cpp
    cl $(CPP_PROJ) dd.cpp

debug/delete.obj : delete.cpp
    cl $(CPP_PROJ) delete.cpp

debug/fill.obj : fill.cpp
    cl $(CPP_PROJ) fill.cpp

debug/loadzip.obj : loadzip.cpp
    cl $(CPP_PROJ) loadzip.cpp

debug/purge.obj : purge.cpp
    cl $(CPP_PROJ) purge.cpp

debug/renum.obj : renum.cpp
    cl $(CPP_PROJ) renum.cpp

debug/restore.obj : restore.cpp
    cl $(CPP_PROJ) restore.cpp

debug/save.obj : save.cpp
    cl $(CPP_PROJ) save.cpp

debug/tb.obj : tb.cpp
    cl $(CPP_PROJ) tb.cpp

debug/tr.obj : tr.cpp
    cl $(CPP_PROJ) tr.cpp

debug/bupres.res : bupres.rc
    rc $(RSC_PROJ) bupres.rc

