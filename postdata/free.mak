CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib htmlhelp.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/postdata.exe"

RSC_PROJ=/l 0x409 /fo"free\postdata.res" /d "NDEBUG"

free\postdata.exe : free\postdata.obj free\update.obj free\postdata.res
    link $(LINK32_FLAGS) free\postdata.obj free\update.obj free\postdata.res
    copy free\postdata.exe \v5\exes

free\postdata.obj : postdata.cpp
    cl $(CPP_PROJ) postdata.cpp

free\update.obj : update.cpp
    cl $(CPP_PROJ) update.cpp

free\postdata.res : postdata.rc
    rc $(RSC_PROJ) postdata.rc
