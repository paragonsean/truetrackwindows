CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /MAP /subsystem:windows /incremental:no /machine:I386 /out:"ufree/monedit.exe"

RSC_PROJ=/l 0x409 /fo"ufree\monedit.res" /d "NDEBUG"
!IFNDEF RCFILE
RCFILE=monedit.rc
!ENDIF
ufree\monedit.exe : ufree\monedit.obj ufree\monitor.obj ufree\monedit.res
    link $(LINK32_FLAGS) ufree\monedit.obj ufree\monitor.obj ufree\monedit.res
    copy ufree\monedit.exe \v5wide\exes

ufree\monedit.obj : monedit.cpp
    cl $(CPP_PROJ) monedit.cpp

ufree\monitor.obj : monitor.cpp
    cl $(CPP_PROJ) monitor.cpp

ufree\monedit.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
