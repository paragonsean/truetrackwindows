CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LIB32_FLAGS=/nologo /out:"..\ufreelib\fastrak.lib"

..\ufreelib\fastrak.lib : ufree\ftcmd.obj ufree\fthigh.obj ufree\ftlow.obj
 link -lib $(LIB32_FLAGS) ufree\ftcmd.obj ufree\fthigh.obj ufree\ftlow.obj

ufree\ftcmd.obj : ftcmd.cpp
    cl $(CPP_PROJ) ftcmd.cpp

ufree\fthigh.obj : fthigh.cpp
    cl $(CPP_PROJ) fthigh.cpp

ufree\ftlow.obj : ftlow.cpp
    cl $(CPP_PROJ) ftlow.cpp

