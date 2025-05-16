CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/optotest.exe"

free\optotest.exe : free\optotest.obj
    link $(LINK32_FLAGS) free\optotest.obj ..\freelib\ft95.lib

free\optotest.obj : optotest.cpp
    cl $(CPP_PROJ) optotest.cpp
