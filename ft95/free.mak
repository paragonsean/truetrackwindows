CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

..\freelib\fastrak.lib : free\ftcmd.obj free\ftlow.obj free\fthigh.obj
    link -lib /nologo /out:"..\freelib\ft95.lib" free\ftcmd.obj free\ftlow.obj free\fthigh.obj

free\ftclas95.obj : ftclas95.cpp
    cl $(CPP_PROJ) ftclas95.cpp
 
free\ftcmd.obj : ftcmd.cpp
    cl $(CPP_PROJ) ftcmd.cpp
 
free\ftlow.obj : ftlow.cpp
    cl $(CPP_PROJ) ftlow.cpp
 
free\fthigh.obj : fthigh.cpp
    cl $(CPP_PROJ) fthigh.cpp
 
