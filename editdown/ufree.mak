CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/editdown.exe"

RSC_PROJ=/l 0x409 /fo"ufree\editdown.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=editdown.rc
!ENDIF

ufree\editdown.exe : ufree\editdown.obj ufree\category.obj ufree\editdown.res
    link $(LINK32_FLAGS) ufree\editdown.obj ufree\category.obj ufree\editdown.res
    copy ufree\editdown.exe \v5wide\exes

ufree\editdown.obj : editdown.cpp
    cl $(CPP_PROJ) editdown.cpp

ufree\category.obj : category.cpp
    cl $(CPP_PROJ) category.cpp

ufree\editdown.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
