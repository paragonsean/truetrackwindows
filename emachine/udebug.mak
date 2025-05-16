CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"udebug\emachine.pch" /YX"windows.h" /Fo"udebug/" /Fd"udebug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\udebuglib\v5subs.lib ..\udebuglib\dundas.lib /nologo /subsystem:windows  /incremental:no /pdb:"udebug\emachine.pdb" /NODEFAULTLIB:LIBCMT.LIB /debug /machine:I386 /out:"udebug/emachine.exe"

RSC_PROJ=/l 0x409 /fo"udebug\emachine.res" /d "NDEBUG"

udebug\emachine.exe : udebug\editdiff.obj udebug\emachine.obj udebug\emachine.res
    link $(LINK32_FLAGS) udebug\editdiff.obj udebug\emachine.obj udebug\emachine.res
    copy udebug\emachine.exe \v5wide\exes

udebug\editdiff.obj : editdiff.cpp
    cl $(CPP_PROJ) editdiff.cpp

udebug\emachine.obj : emachine.cpp
    cl $(CPP_PROJ) emachine.cpp

udebug\emachine.res : emachine.rc
    rc $(RSC_PROJ) emachine.rc
