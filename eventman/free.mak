CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/eventman.exe"

RSC_PROJ=/l 0x409 /fo"free\eventman.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=eventman.rc
!ENDIF

free\eventman.exe : free\eventman.obj free\mail.obj free\eventman.res
    link $(LINK32_FLAGS) free\eventman.obj free\mail.obj free\eventman.res
    copy free\eventman.exe \v5\exes

free\eventman.obj : eventman.cpp
    cl $(CPP_PROJ) eventman.cpp

free\mail.obj : mail.cpp
    cl $(CPP_PROJ) mail.cpp

free\eventman.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
