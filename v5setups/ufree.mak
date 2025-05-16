CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= libcmt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/v5setups.exe"

RSC_PROJ=/l 0x409 /fo"ufree\v5setups.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=v5setups.rc
!ENDIF

ufree\v5setups.exe : ufree\v5setups.obj ufree\v5setups.res
    link $(LINK32_FLAGS) ufree\v5setups.obj ufree\v5setups.res
    copy ufree\v5setups.exe \v5wide\exes

ufree\v5setups.obj : v5setups.cpp
    cl $(CPP_PROJ) v5setups.cpp

ufree\v5setups.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
