CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/downtime.exe"

RSC_PROJ=/l 0x409 /fo"ufree\downtime.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=downtime.rc
!ENDIF

ufree\downtime.exe : ufree\downtime.obj ufree\category.obj ufree\downtime.res
    link $(LINK32_FLAGS) ufree\downtime.obj ufree\category.obj ufree\downtime.res
    copy ufree\downtime.exe \v5wide\exes

ufree\downtime.obj : downtime.cpp
    cl $(CPP_PROJ) downtime.cpp

ufree\category.obj : category.cpp
    cl $(CPP_PROJ) category.cpp

ufree\downtime.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
