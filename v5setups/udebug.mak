CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"udebug/v5setups.pch" /YX"windows.h" /Fo"udebug/" /Fd"udebug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\udebuglib\v5subs.lib /nologo /pdb:"udebug\v5setups.pdb" /NODEFAULTLIB:libcmt.lib /debug /subsystem:windows /incremental:no /machine:I386 /out:"udebug/v5setups.exe"

RSC_PROJ=/l 0x409 /fo"udebug\v5setups.res"

udebug\v5setups.exe : udebug\v5setups.obj udebug\v5setups.res
    link $(LINK32_FLAGS) udebug\v5setups.obj udebug\v5setups.res
    copy udebug\v5setups.exe \v5wide\exes

udebug\v5setups.obj : v5setups.cpp
    cl $(CPP_PROJ) v5setups.cpp

udebug\v5setups.res : v5setups.rc
    rc $(RSC_PROJ) v5setups.rc
