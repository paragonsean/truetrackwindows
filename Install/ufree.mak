CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/setup.exe"

RSC_PROJ=/l 0x409 /fo"ufree\setup.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=setup.rc
!ENDIF

ufree\setup.exe : ufree\setup.obj ufree\menupdat.obj ufree\setup.res
    link $(LINK32_FLAGS) ufree\setup.obj ufree\menupdat.obj ufree\setup.res

ufree\setup.obj : setup.cpp
    cl $(CPP_PROJ) setup.cpp

ufree\menupdat.obj : menupdat.cpp
    cl $(CPP_PROJ) menupdat.cpp

ufree\setup.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
