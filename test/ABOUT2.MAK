CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /incremental:no /machine:I386

RSC_PROJ=/l 0x409 /fo"about2.res" /d "NDEBUG"

about2.exe : about2.obj about2.res
    link $(LINK32_FLAGS) about2.obj about2.res

about2.obj : about2.c
    cl $(CPP_PROJ) about2.c

about2.res : about2.rc
    rc $(RSC_PROJ) about2.rc
