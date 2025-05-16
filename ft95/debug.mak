CPP_PROJ=/nologo /MTd /W3 /Zp1  /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_LIB" /Fp"debug\ft95.pch" /YX /Fo"debug/" /Fd"debug\\" /c

LIB32_FLAGS=/nologo /out:"debug\fastrak.lib"

debug\fastrak.lib : debug\ftcmd.obj debug\ftlow.obj debug\fthigh.obj
 link -lib $(LIB32_FLAGS) debug\ftcmd.obj debug\ftlow.obj debug\fthigh.obj

debug\ftcmd.obj : ftcmd.cpp
    cl $(CPP_PROJ) ftcmd.cpp
 
debug\ftlow.obj : ftlow.cpp
    cl $(CPP_PROJ) ftlow.cpp
 
debug\fthigh.obj : fthigh.cpp
    cl $(CPP_PROJ) fthigh.cpp
 
