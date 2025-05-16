CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_LIB" /Fp"debug\ftclasnt.pch" /YX /Fo"debug/" /Fd"debug\\" /c

LIB32_FLAGS=/nologo /out:"debug\ftclass.lib"

debug\ftclass.lib : debug\ftclasnt.obj
 link -lib $(LIB32_FLAGS) debug\ftclasnt.obj

debug\ftclasnt.obj : ftclasnt.cpp
    cl $(CPP_PROJ) ftclasnt.cpp
