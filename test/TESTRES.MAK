CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"testres.exe"

RSC_PROJ=/l 0x409 /fo"testres.res" /d "NDEBUG"

testres.exe : testres.obj testres.res
    link $(LINK32_FLAGS) testres.obj testres.res

testres.obj : testres.cpp testres.h
    cl $(CPP_PROJ) testres.cpp

testres.res : testres.rc testres.h
    rc $(RSC_PROJ) testres.rc
