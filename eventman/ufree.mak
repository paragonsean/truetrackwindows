CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/eventman.exe"

RSC_PROJ=/l 0x409 /fo"ufree\eventman.res" /d "NDEBUG"

eventman.exe : ufree\eventman.obj ufree\mail.obj ufree\eventman.res
    link $(LINK32_FLAGS) ufree\eventman.obj ufree\mail.obj ufree\eventman.res
    copy ufree\eventman.exe \v5wide\exes

ufree\eventman.obj : eventman.cpp
    cl $(CPP_PROJ) eventman.cpp

ufree\mail.obj : mail.cpp
    cl $(CPP_PROJ) mail.cpp

ufree\eventman.res : eventman.rc
    rc $(RSC_PROJ) eventman.rc
