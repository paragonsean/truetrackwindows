CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

..\freelib\fastrak.lib : free\ftcmd.obj free\fthigh.obj free\ftlow.obj
    link -lib /nologo /out:"..\freelib\fastrak.lib" free\ftcmd.obj free\fthigh.obj free\ftlow.obj

free\ftcmd.obj : ftcmd.cpp
    cl $(CPP_PROJ) ftcmd.cpp

free\fthigh.obj : fthigh.cpp
    cl $(CPP_PROJ) fthigh.cpp

free\ftlow.obj : ftlow.cpp
    cl $(CPP_PROJ) ftlow.cpp
