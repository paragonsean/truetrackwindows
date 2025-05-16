CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_LIB" /Fp"debug\fastrak.pch" /YX  /Fo"debug/" /Fd"debug\\" /c

LIB32_FLAGS=/nologo /out:"..\debuglib\fastrak.lib"

..\debuglib\fastrak.lib : debug\ftcmd.obj debug\fthigh.obj debug\ftlow.obj
 link -lib $(LIB32_FLAGS) debug\ftcmd.obj debug\fthigh.obj debug\ftlow.obj

debug\ftcmd.obj : ftcmd.cpp
    cl $(CPP_PROJ) ftcmd.cpp

debug\fthigh.obj : fthigh.cpp
    cl $(CPP_PROJ) fthigh.cpp

debug\ftlow.obj : ftlow.cpp
    cl $(CPP_PROJ) ftlow.cpp
