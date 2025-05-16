CPP_PROJ=/nologo /MTd /W3 /Od /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_LIB" /Fp"debug\dundas.pch" /YX /Fo"debug/" /c

LIB32_FLAGS=/nologo /out:"..\debuglib\dundas.lib"

..\debuglib\dundas.lib : debug\ugcell.obj debug\ugceltyp.obj debug\ugcnrbtn.obj \
 debug\ugctarrw.obj debug\ugctrl.obj debug\ugdltype.obj \
 debug\ugdrwhnt.obj debug\ugdtasrc.obj debug\ugedit.obj \
 debug\ugformat.obj debug\uggdinfo.obj debug\uggrid.obj debug\ughint.obj      \
 debug\ughscrol.obj debug\uglstbox.obj debug\ugmemman.obj debug\ugmultis.obj  \
 debug\ugprint.obj debug\ugptrlst.obj debug\ugsidehd.obj debug\ugstring.obj   \
 debug\ugtab.obj debug\ugtophdg.obj debug\ugvscrol.obj
 link -lib $(LIB32_FLAGS) debug\ugcell.obj debug\ugceltyp.obj \
 debug\ugcnrbtn.obj \
 debug\ugctarrw.obj debug\ugctrl.obj debug\ugdltype.obj \
 debug\ugdrwhnt.obj debug\ugdtasrc.obj debug\ugedit.obj \
 debug\ugformat.obj debug\uggdinfo.obj debug\uggrid.obj debug\ughint.obj      \
 debug\ughscrol.obj debug\uglstbox.obj debug\ugmemman.obj debug\ugmultis.obj  \
 debug\ugprint.obj debug\ugptrlst.obj debug\ugsidehd.obj debug\ugstring.obj   \
 debug\ugtab.obj debug\ugtophdg.obj debug\ugvscrol.obj

debug\ugcell.obj : ugcell.cpp
    cl $(CPP_PROJ) ugcell.cpp

debug\ugceltyp.obj : ugceltyp.cpp
    cl $(CPP_PROJ) ugceltyp.cpp

debug\ugcnrbtn.obj : ugcnrbtn.cpp
    cl $(CPP_PROJ) ugcnrbtn.cpp

debug\ugctarrw.obj : ugctarrw.cpp
    cl $(CPP_PROJ) ugctarrw.cpp

debug\ugctrl.obj : ugctrl.cpp
    cl $(CPP_PROJ) ugctrl.cpp

debug\ugdltype.obj : ugdltype.cpp
    cl $(CPP_PROJ) ugdltype.cpp

debug\ugdrwhnt.obj : ugdrwhnt.cpp
    cl $(CPP_PROJ) ugdrwhnt.cpp

debug\ugdtasrc.obj : ugdtasrc.cpp
    cl $(CPP_PROJ) ugdtasrc.cpp

debug\ugedit.obj : ugedit.cpp
    cl $(CPP_PROJ) ugedit.cpp

debug\ugformat.obj : ugformat.cpp
    cl $(CPP_PROJ) ugformat.cpp

debug\uggdinfo.obj : uggdinfo.cpp
    cl $(CPP_PROJ) uggdinfo.cpp

debug\uggrid.obj : uggrid.cpp
    cl $(CPP_PROJ) uggrid.cpp

debug\ughint.obj : ughint.cpp
    cl $(CPP_PROJ) ughint.cpp

debug\ughscrol.obj : ughscrol.cpp
    cl $(CPP_PROJ) ughscrol.cpp

debug\uglstbox.obj : uglstbox.cpp
    cl $(CPP_PROJ) uglstbox.cpp

debug\ugmemman.obj : ugmemman.cpp
    cl $(CPP_PROJ) ugmemman.cpp

debug\ugmultis.obj : ugmultis.cpp
    cl $(CPP_PROJ) ugmultis.cpp

debug\ugprint.obj : ugprint.cpp
    cl $(CPP_PROJ) ugprint.cpp

debug\ugptrlst.obj : ugptrlst.cpp
    cl $(CPP_PROJ) ugptrlst.cpp

debug\ugsidehd.obj : ugsidehd.cpp
    cl $(CPP_PROJ) ugsidehd.cpp

debug\ugstring.obj : ugstring.cpp
    cl $(CPP_PROJ) ugstring.cpp

debug\ugtab.obj : ugtab.cpp
    cl $(CPP_PROJ) ugtab.cpp

debug\ugtophdg.obj : ugtophdg.cpp
    cl $(CPP_PROJ) ugtophdg.cpp

debug\ugvscrol.obj : ugvscrol.cpp
    cl $(CPP_PROJ) ugvscrol.cpp

