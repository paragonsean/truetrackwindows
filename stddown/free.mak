CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"./free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /NODEFAULTLIB:LIBC.LIB ..\freelib\dundas.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/stddown.exe"

RSC_PROJ=/l 0x409 /fo"free\stddown.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=stddown.rc
!ENDIF

free\stddown.exe : free\stddown.obj free\stddown.res
    link $(LINK32_FLAGS) free\stddown.obj ..\editdown\free\category.obj free\stddown.res
    copy free\stddown.exe \v5\exes

free\stddown.obj : stddown.cpp
    cl $(CPP_PROJ) stddown.cpp

free\stddown.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)

