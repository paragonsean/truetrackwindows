CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/CopyLog.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\CopyLog.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/CopyLog.exe"

RSC_PROJ=/l 0x409 /fo"debug\CopyLog.res"

debug\CopyLog.exe : debug\CopyLog.obj debug\CopyLog.res
    link $(LINK32_FLAGS) debug\CopyLog.obj debug\CopyLog.res
    copy debug\CopyLog.exe \v5\exes

debug\CopyLog.obj : CopyLog.cpp
    cl $(CPP_PROJ) CopyLog.cpp

debug\CopyLog.res : CopyLog.rc
    rc $(RSC_PROJ) CopyLog.rc
