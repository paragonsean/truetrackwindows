CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib mpr.lib htmlhelp.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/netsetup.exe"

RSC_PROJ=/l 0x409 /fo"free\netsetup.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=netsetup.rc
!ENDIF

free\netsetup.exe : free\netsetup.obj free\netsetup.res
    link $(LINK32_FLAGS) free\netsetup.obj free\netsetup.res

free\netsetup.obj : netsetup.cpp
    cl $(CPP_PROJ) netsetup.cpp

free\netsetup.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
