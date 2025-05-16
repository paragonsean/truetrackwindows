CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/esensor.exe"

RSC_PROJ=/l 0x409 /fo"ufree\esensor.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=esensor.rc
!ENDIF

ufree\esensor.exe : ufree\esensor.obj ufree\esensor.res
    link $(LINK32_FLAGS) ufree\esensor.obj ufree\esensor.res
    copy ufree\esensor.exe \v5wide\exes

ufree\esensor.obj : esensor.cpp
    cl $(CPP_PROJ) esensor.cpp

ufree\esensor.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
