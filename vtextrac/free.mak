CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/vtextrac.exe"

RSC_PROJ=/l 0x409 /fo"free\vtextrac.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=vtextrac.rc
!ENDIF

free\vtextrac.exe : free\vtextrac.obj free\vtextrac.res
    link $(LINK32_FLAGS) free\vtextrac.obj free\vtextrac.res
    copy free\vtextrac.exe \v5ds\exes

free\vtextrac.obj : vtextrac.cpp
    cl $(CPP_PROJ) vtextrac.cpp

free\vtextrac.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
