CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/v5setups.exe"

RSC_PROJ=/l 0x409 /fo"free\v5setups.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=v5setups.rc
!ENDIF

free\v5setups.exe : free\v5setups.obj free\v5setups.res
    link $(LINK32_FLAGS) free\v5setups.obj free\v5setups.res
    copy free\v5setups.exe \v5\exes

free\v5setups.obj : v5setups.cpp
    cl $(CPP_PROJ) v5setups.cpp

free\v5setups.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
