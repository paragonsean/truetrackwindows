CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

..\freelib\ftclasnt.lib : free\ftclasnt.obj
 link -lib /nologo /out:"..\freelib\ftclasnt.lib" free\ftclasnt.obj

free\ftclasnt.obj : ftclasnt.cpp
    cl $(CPP_PROJ) ftclasnt.cpp
