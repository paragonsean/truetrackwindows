CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LIB32_FLAGS=/nologo /out:"ufree\ftclass.lib"

ufree\ftclass.lib : ufree\ftclasnt.obj
 link -lib $(LIB32_FLAGS) ufree\ftclasnt.obj

ufree\ftclasnt.obj : ftclasnt.cpp
    cl $(CPP_PROJ) ftclasnt.cpp
