CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/su.exe"

RSC_PROJ=/l 0x409 /fo"ufree\su.res" /d "NDEBUG"

ufree\su.exe : ufree\su.obj ufree\su.res
    link $(LINK32_FLAGS) ufree\su.obj ufree\su.res

ufree\su.obj : su.cpp
    cl $(CPP_PROJ) su.cpp

ufree\su.res : su.rc
    rc $(RSC_PROJ) su.rc
