CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/CopyLog.exe"

RSC_PROJ=/l 0x409 /fo"ufree\CopyLog.res" /d "NDEBUG"

ufree\CopyLog.exe : ufree\CopyLog.obj ufree\CopyLog.res
    link $(LINK32_FLAGS) ufree\CopyLog.obj ufree\CopyLog.res
    copy ufree\copylog.exe \v5wide\exes

ufree\CopyLog.obj : CopyLog.cpp
    cl $(CPP_PROJ) CopyLog.cpp

ufree\CopyLog.res : CopyLog.rc
    rc $(RSC_PROJ) CopyLog.rc
