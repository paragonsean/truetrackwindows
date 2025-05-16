CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LIB32_FLAGS=/nologo /out:"..\ufreelib\dundas.lib"

..\ufreelib\dundas.lib : ufree\ugcell.obj ufree\ugceltyp.obj ufree\ugcnrbtn.obj \
 ufree\ugctarrw.obj ufree\ugctrl.obj ufree\ugdltype.obj \
 ufree\ugdrwhnt.obj ufree\ugdtasrc.obj ufree\ugedit.obj \
 ufree\ugformat.obj ufree\uggdinfo.obj ufree\uggrid.obj ufree\ughint.obj      \
 ufree\ughscrol.obj ufree\uglstbox.obj ufree\ugmemman.obj ufree\ugmultis.obj  \
 ufree\ugprint.obj ufree\ugptrlst.obj ufree\ugsidehd.obj ufree\ugstring.obj   \
 ufree\ugtab.obj ufree\ugtophdg.obj ufree\ugvscrol.obj
 link -lib $(LIB32_FLAGS) ufree\ugcell.obj ufree\ugceltyp.obj \
 ufree\ugcnrbtn.obj \
 ufree\ugctarrw.obj ufree\ugctrl.obj ufree\ugdltype.obj \
 ufree\ugdrwhnt.obj ufree\ugdtasrc.obj ufree\ugedit.obj \
 ufree\ugformat.obj ufree\uggdinfo.obj ufree\uggrid.obj ufree\ughint.obj      \
 ufree\ughscrol.obj ufree\uglstbox.obj ufree\ugmemman.obj ufree\ugmultis.obj  \
 ufree\ugprint.obj ufree\ugptrlst.obj ufree\ugsidehd.obj ufree\ugstring.obj   \
 ufree\ugtab.obj ufree\ugtophdg.obj ufree\ugvscrol.obj

ufree\ugcell.obj : ugcell.cpp
    cl $(CPP_PROJ) ugcell.cpp

ufree\ugceltyp.obj : ugceltyp.cpp
    cl $(CPP_PROJ) ugceltyp.cpp

ufree\ugcnrbtn.obj : ugcnrbtn.cpp
    cl $(CPP_PROJ) ugcnrbtn.cpp

ufree\ugctarrw.obj : ugctarrw.cpp
    cl $(CPP_PROJ) ugctarrw.cpp

ufree\ugctrl.obj : ugctrl.cpp
    cl $(CPP_PROJ) ugctrl.cpp

ufree\ugdltype.obj : ugdltype.cpp
    cl $(CPP_PROJ) ugdltype.cpp

ufree\ugdrwhnt.obj : ugdrwhnt.cpp
    cl $(CPP_PROJ) ugdrwhnt.cpp

ufree\ugdtasrc.obj : ugdtasrc.cpp
    cl $(CPP_PROJ) ugdtasrc.cpp

ufree\ugedit.obj : ugedit.cpp
    cl $(CPP_PROJ) ugedit.cpp

ufree\ugformat.obj : ugformat.cpp
    cl $(CPP_PROJ) ugformat.cpp

ufree\uggdinfo.obj : uggdinfo.cpp
    cl $(CPP_PROJ) uggdinfo.cpp

ufree\uggrid.obj : uggrid.cpp
    cl $(CPP_PROJ) uggrid.cpp

ufree\ughint.obj : ughint.cpp
    cl $(CPP_PROJ) ughint.cpp

ufree\ughscrol.obj : ughscrol.cpp
    cl $(CPP_PROJ) ughscrol.cpp

ufree\uglstbox.obj : uglstbox.cpp
    cl $(CPP_PROJ) uglstbox.cpp

ufree\ugmemman.obj : ugmemman.cpp
    cl $(CPP_PROJ) ugmemman.cpp

ufree\ugmultis.obj : ugmultis.cpp
    cl $(CPP_PROJ) ugmultis.cpp

ufree\ugprint.obj : ugprint.cpp
    cl $(CPP_PROJ) ugprint.cpp

ufree\ugptrlst.obj : ugptrlst.cpp
    cl $(CPP_PROJ) ugptrlst.cpp

ufree\ugsidehd.obj : ugsidehd.cpp
    cl $(CPP_PROJ) ugsidehd.cpp

ufree\ugstring.obj : ugstring.cpp
    cl $(CPP_PROJ) ugstring.cpp

ufree\ugtab.obj : ugtab.cpp
    cl $(CPP_PROJ) ugtab.cpp

ufree\ugtophdg.obj : ugtophdg.cpp
    cl $(CPP_PROJ) ugtophdg.cpp

ufree\ugvscrol.obj : ugvscrol.cpp
    cl $(CPP_PROJ) ugvscrol.cpp

