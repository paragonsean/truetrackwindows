CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/esensor.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\esensor.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/esensor.exe"

RSC_PROJ=/l 0x409 /fo"debug\esensor.res"

debug\esensor.exe : debug\esensor.obj debug\esensor.res
    link $(LINK32_FLAGS) debug\esensor.obj debug\esensor.res
    copy debug\esensor.exe \v5\exes

debug\esensor.obj : esensor.cpp
    cl $(CPP_PROJ) esensor.cpp

debug\esensor.res : esensor.rc
    rc $(RSC_PROJ) esensor.rc
