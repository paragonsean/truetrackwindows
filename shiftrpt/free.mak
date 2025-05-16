CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/shiftrpt.exe"

RSC_PROJ=/l 0x409 /fo"free\shiftrpt.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=shiftrpt.rc
!ENDIF

free\shiftrpt.exe : free\shiftrpt.obj free\shiftrpt.res
    link $(LINK32_FLAGS) free\shiftrpt.obj free\shiftrpt.res
    copy free\shiftrpt.exe \v5\exes

free\shiftrpt.obj : shiftrpt.cpp resource.h
    cl $(CPP_PROJ) shiftrpt.cpp

free\shiftrpt.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
