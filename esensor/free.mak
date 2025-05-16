CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/esensor.exe"

RSC_PROJ=/l 0x409 /fo"free\esensor.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=esensor.rc
!ENDIF

free\esensor.exe : free\esensor.obj free\esensor.res
    link $(LINK32_FLAGS) free\esensor.obj free\esensor.res
    copy free\esensor.exe \v5\exes

free\esensor.obj : esensor.cpp
    cl $(CPP_PROJ) esensor.cpp

free\esensor.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
