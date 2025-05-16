CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/shiftrpt.exe"

RSC_PROJ=/l 0x409 /fo"ufree\shiftrpt.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=shiftrpt.rc
!ENDIF

ufree\shiftrpt.exe : ufree\shiftrpt.obj ufree\shiftrpt.res
    link $(LINK32_FLAGS) ufree\shiftrpt.obj ufree\shiftrpt.res
    copy ufree\shiftrpt.exe \v5wide\exes

ufree\shiftrpt.obj : shiftrpt.cpp resource.h
    cl $(CPP_PROJ) shiftrpt.cpp

ufree\shiftrpt.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
