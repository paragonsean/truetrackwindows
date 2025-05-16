CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib ..\ufreelib\dundas.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/emachine.exe"

RSC_PROJ=/l 0x409 /fo"ufree\emachine.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=emachine.rc
!ENDIF

ufree\emachine.exe : ufree\editdiff.obj ufree\emachine.obj ufree\emachine.res
    link $(LINK32_FLAGS) ufree\editdiff.obj ufree\emachine.obj ufree\emachine.res
    copy ufree\emachine.exe \v5wide\exes

ufree\editdiff.obj : editdiff.cpp
    cl $(CPP_PROJ) editdiff.cpp

ufree\emachine.obj : emachine.cpp
    cl $(CPP_PROJ) emachine.cpp

ufree\emachine.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
