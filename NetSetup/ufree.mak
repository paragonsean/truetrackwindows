CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib mpr.lib htmlhelp.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/netsetup.exe"

RSC_PROJ=/l 0x409 /fo"ufree/netsetup.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=netsetup.rc
!ENDIF

ufree\netsetup.exe : ufree\netsetup.obj ufree\netsetup.res
    link $(LINK32_FLAGS) ufree\netsetup.obj ufree\netsetup.res

ufree\netsetup.obj : netsetup.cpp
    cl $(CPP_PROJ) netsetup.cpp

ufree\netsetup.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
