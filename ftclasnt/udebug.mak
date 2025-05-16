CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"udebug\ftclasnt.pch" /YX /Fo"udebug/" /Fd"udebug\\" /c

LIB32_FLAGS=/nologo /out:"..\udebuglib\ftclass.lib"

..\udebuglib\ftclass.lib : udebug\ftclasnt.obj
 link -lib $(LIB32_FLAGS) udebug\ftclasnt.obj

udebug\ftclasnt.obj : ftclasnt.cpp
    cl $(CPP_PROJ) ftclasnt.cpp

