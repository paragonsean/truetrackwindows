CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

..\freelib\ftclas95.lib : free\ftclas95.obj
 link -lib /nologo /out:"..\freelib\ftclas95.lib" free\ftclas95.obj

free\ftclas95.obj : ftclas95.cpp
    cl $(CPP_PROJ) ftclas95.cpp
