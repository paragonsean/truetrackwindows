CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"udebug/esensor.pch" /YX"windows.h" /Fo"udebug/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\udebuglib\v5subs.lib /nologo /debug /subsystem:windows /incremental:no /machine:I386 /out:"udebug/esensor.exe"

RSC_PROJ=/l 0x409 /fo"udebug\esensor.res"

udebug\esensor.exe : udebug\esensor.obj udebug\esensor.res
    link $(LINK32_FLAGS) udebug\esensor.obj udebug\esensor.res
    copy udebug\esensor.exe \v5\exes

udebug\esensor.obj : esensor.cpp
    cl $(CPP_PROJ) esensor.cpp

udebug\esensor.res : esensor.rc
    rc $(RSC_PROJ) esensor.rc
