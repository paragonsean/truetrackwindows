CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"udebug\dundas.pch" /YX /Fo"udebug/" /c

LIB32_FLAGS=/nologo /out:"..\udebuglib\dundas.lib"

..\udebuglib\dundas.lib : udebug\ugcell.obj udebug\ugceltyp.obj udebug\ugcnrbtn.obj \
 udebug\ugctarrw.obj udebug\ugctrl.obj udebug\ugdltype.obj \
 udebug\ugdrwhnt.obj udebug\ugdtasrc.obj udebug\ugedit.obj \
 udebug\ugformat.obj udebug\uggdinfo.obj udebug\uggrid.obj udebug\ughint.obj      \
 udebug\ughscrol.obj udebug\uglstbox.obj udebug\ugmemman.obj udebug\ugmultis.obj  \
 udebug\ugprint.obj udebug\ugptrlst.obj udebug\ugsidehd.obj udebug\ugstring.obj   \
 udebug\ugtab.obj udebug\ugtophdg.obj udebug\ugvscrol.obj
 link -lib $(LIB32_FLAGS) udebug\ugcell.obj udebug\ugceltyp.obj \
 udebug\ugcnrbtn.obj \
 udebug\ugctarrw.obj udebug\ugctrl.obj udebug\ugdltype.obj \
 udebug\ugdrwhnt.obj udebug\ugdtasrc.obj udebug\ugedit.obj \
 udebug\ugformat.obj udebug\uggdinfo.obj udebug\uggrid.obj udebug\ughint.obj      \
 udebug\ughscrol.obj udebug\uglstbox.obj udebug\ugmemman.obj udebug\ugmultis.obj  \
 udebug\ugprint.obj udebug\ugptrlst.obj udebug\ugsidehd.obj udebug\ugstring.obj   \
 udebug\ugtab.obj udebug\ugtophdg.obj udebug\ugvscrol.obj

udebug\ugcell.obj : ugcell.cpp
    cl $(CPP_PROJ) ugcell.cpp

udebug\ugceltyp.obj : ugceltyp.cpp
    cl $(CPP_PROJ) ugceltyp.cpp

udebug\ugcnrbtn.obj : ugcnrbtn.cpp
    cl $(CPP_PROJ) ugcnrbtn.cpp

udebug\ugctarrw.obj : ugctarrw.cpp
    cl $(CPP_PROJ) ugctarrw.cpp

udebug\ugctrl.obj : ugctrl.cpp
    cl $(CPP_PROJ) ugctrl.cpp

udebug\ugdltype.obj : ugdltype.cpp
    cl $(CPP_PROJ) ugdltype.cpp

udebug\ugdrgdrp.obj : ugdrgdrp.cpp
    cl $(CPP_PROJ) ugdrgdrp.cpp

udebug\ugdrwhnt.obj : ugdrwhnt.cpp
    cl $(CPP_PROJ) ugdrwhnt.cpp

udebug\ugdtasrc.obj : ugdtasrc.cpp
    cl $(CPP_PROJ) ugdtasrc.cpp

udebug\ugedit.obj : ugedit.cpp
    cl $(CPP_PROJ) ugedit.cpp

udebug\ugexcel.obj : ugexcel.cpp
    cl $(CPP_PROJ) ugexcel.cpp

udebug\ugformat.obj : ugformat.cpp
    cl $(CPP_PROJ) ugformat.cpp

udebug\uggdinfo.obj : uggdinfo.cpp
    cl $(CPP_PROJ) uggdinfo.cpp

udebug\uggrid.obj : uggrid.cpp
    cl $(CPP_PROJ) uggrid.cpp

udebug\ughint.obj : ughint.cpp
    cl $(CPP_PROJ) ughint.cpp

udebug\ughscrol.obj : ughscrol.cpp
    cl $(CPP_PROJ) ughscrol.cpp

udebug\uglstbox.obj : uglstbox.cpp
    cl $(CPP_PROJ) uglstbox.cpp

udebug\ugmemman.obj : ugmemman.cpp
    cl $(CPP_PROJ) ugmemman.cpp

udebug\ugmultis.obj : ugmultis.cpp
    cl $(CPP_PROJ) ugmultis.cpp

udebug\ugprint.obj : ugprint.cpp
    cl $(CPP_PROJ) ugprint.cpp

udebug\ugptrlst.obj : ugptrlst.cpp
    cl $(CPP_PROJ) ugptrlst.cpp

udebug\ugsidehd.obj : ugsidehd.cpp
    cl $(CPP_PROJ) ugsidehd.cpp

udebug\ugstring.obj : ugstring.cpp
    cl $(CPP_PROJ) ugstring.cpp

udebug\ugtab.obj : ugtab.cpp
    cl $(CPP_PROJ) ugtab.cpp

udebug\ugtophdg.obj : ugtophdg.cpp
    cl $(CPP_PROJ) ugtophdg.cpp

udebug\ugvscrol.obj : ugvscrol.cpp
    cl $(CPP_PROJ) ugvscrol.cpp

