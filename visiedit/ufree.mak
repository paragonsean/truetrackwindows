CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/visiedit.exe"

RSC_PROJ=/l 0x409 /fo"ufree\visiedit.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=visiedit.rc
!ENDIF

ufree\visiedit.exe : ufree\visiedit.obj ufree\visiedit.res
    link $(LINK32_FLAGS) ufree\visiedit.obj ufree\visiedit.res
    copy ufree\visiedit.exe \v5wide\exes

ufree\visiedit.obj : visiedit.cpp
    cl $(CPP_PROJ) visiedit.cpp

ufree\visiedit.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
