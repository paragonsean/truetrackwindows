CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug\v5setups.pch" /YX"windows.h" /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\debuglib\v5subs.lib /NODEFAULTLIB:LIBC.LIB /nologo /subsystem:windows /incremental:no /pdb:"debug\profile.pdb" /debug /machine:I386 /out:"debug/v5setups.exe"

RSC_PROJ=/l 0x409 /fo"debug\v5setups.res" /d "NDEBUG"

debug\v5setups.exe : debug\v5setups.obj debug\v5setups.res
    link $(LINK32_FLAGS) debug\v5setups.obj debug\v5setups.res
    copy debug\v5setups.exe \v5\exes

debug\v5setups.obj : v5setups.cpp
    cl $(CPP_PROJ) v5setups.cpp

debug\v5setups.res : v5setups.rc
    rc $(RSC_PROJ) v5setups.rc
