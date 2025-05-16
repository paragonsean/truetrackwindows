CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_LIB" /Fp"debug\ftclas95.pch" /YX /Fo"debug/" /Fd"debug\\" /c

LIB32_FLAGS=/nologo /out:"debug\ftclass.lib"

debug\ftclass.lib : debug\ftclas95.obj
 link -lib $(LIB32_FLAGS) debug\ftclas95.obj

debug\ftclas95.obj : ftclas95.cpp
    cl $(CPP_PROJ) ftclas95.cpp
