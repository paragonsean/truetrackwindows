CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_LIB" /D "UNICODE" /D "_UNICODE" /Fp"udebug\fastrak.pch" /YX"windows.h" /Fo"udebug/" /Fd"udebug\\" /c

LIB32_FLAGS=/nologo /out:"..\udebuglib\fastrak.lib"

..\udebuglib\fastrak.lib : udebug\ftcmd.obj udebug\fthigh.obj udebug\ftlow.obj
 link -lib $(LIB32_FLAGS) udebug\ftcmd.obj udebug\fthigh.obj udebug\ftlow.obj

udebug\ftcmd.obj : ftcmd.cpp
    cl $(CPP_PROJ) ftcmd.cpp

udebug\fthigh.obj : fthigh.cpp
    cl $(CPP_PROJ) fthigh.cpp

udebug\ftlow.obj : ftlow.cpp
    cl $(CPP_PROJ) ftlow.cpp

