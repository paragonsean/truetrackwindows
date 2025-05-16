CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"./ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /NODEFAULTLIB:LIBC.LIB ..\ufreelib\dundas.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/stddown.exe"

RSC_PROJ=/l 0x409 /fo"ufree\stddown.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=stddown.rc
!ENDIF

ufree\stddown.exe : ufree\stddown.obj ufree\stddown.res
    link $(LINK32_FLAGS) ufree\stddown.obj ..\editdown\ufree\category.obj ufree\stddown.res
    copy ufree\stddown.exe \v5wide\exes

ufree\stddown.obj : stddown.cpp
    cl $(CPP_PROJ) stddown.cpp

ufree\stddown.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)

