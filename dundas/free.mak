CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LIB32_FLAGS=/nologo /out:"..\freelib\dundas.lib"

..\freelib\dundas.lib : free\ugcell.obj free\ugceltyp.obj free\ugcnrbtn.obj \
 free\ugctarrw.obj free\ugctrl.obj free\ugdltype.obj \
 free\ugdrwhnt.obj free\ugdtasrc.obj free\ugedit.obj \
 free\ugformat.obj free\uggdinfo.obj free\uggrid.obj free\ughint.obj      \
 free\ughscrol.obj free\uglstbox.obj free\ugmemman.obj free\ugmultis.obj  \
 free\ugprint.obj free\ugptrlst.obj free\ugsidehd.obj free\ugstring.obj   \
 free\ugtab.obj free\ugtophdg.obj free\ugvscrol.obj
 link -lib $(LIB32_FLAGS) free\ugcell.obj free\ugceltyp.obj \
 free\ugcnrbtn.obj \
 free\ugctarrw.obj free\ugctrl.obj free\ugdltype.obj \
 free\ugdrwhnt.obj free\ugdtasrc.obj free\ugedit.obj \
 free\ugformat.obj free\uggdinfo.obj free\uggrid.obj free\ughint.obj      \
 free\ughscrol.obj free\uglstbox.obj free\ugmemman.obj free\ugmultis.obj  \
 free\ugprint.obj free\ugptrlst.obj free\ugsidehd.obj free\ugstring.obj   \
 free\ugtab.obj free\ugtophdg.obj free\ugvscrol.obj

free\ugcell.obj : ugcell.cpp
    cl $(CPP_PROJ) ugcell.cpp

free\ugceltyp.obj : ugceltyp.cpp
    cl $(CPP_PROJ) ugceltyp.cpp

free\ugcnrbtn.obj : ugcnrbtn.cpp
    cl $(CPP_PROJ) ugcnrbtn.cpp

free\ugctarrw.obj : ugctarrw.cpp
    cl $(CPP_PROJ) ugctarrw.cpp

free\ugctrl.obj : ugctrl.cpp
    cl $(CPP_PROJ) ugctrl.cpp

free\ugdltype.obj : ugdltype.cpp
    cl $(CPP_PROJ) ugdltype.cpp

free\ugdrwhnt.obj : ugdrwhnt.cpp
    cl $(CPP_PROJ) ugdrwhnt.cpp

free\ugdtasrc.obj : ugdtasrc.cpp
    cl $(CPP_PROJ) ugdtasrc.cpp

free\ugedit.obj : ugedit.cpp
    cl $(CPP_PROJ) ugedit.cpp

free\ugformat.obj : ugformat.cpp
    cl $(CPP_PROJ) ugformat.cpp

free\uggdinfo.obj : uggdinfo.cpp
    cl $(CPP_PROJ) uggdinfo.cpp

free\uggrid.obj : uggrid.cpp
    cl $(CPP_PROJ) uggrid.cpp

free\ughint.obj : ughint.cpp
    cl $(CPP_PROJ) ughint.cpp

free\ughscrol.obj : ughscrol.cpp
    cl $(CPP_PROJ) ughscrol.cpp

free\uglstbox.obj : uglstbox.cpp
    cl $(CPP_PROJ) uglstbox.cpp

free\ugmemman.obj : ugmemman.cpp
    cl $(CPP_PROJ) ugmemman.cpp

free\ugmultis.obj : ugmultis.cpp
    cl $(CPP_PROJ) ugmultis.cpp

free\ugprint.obj : ugprint.cpp
    cl $(CPP_PROJ) ugprint.cpp

free\ugptrlst.obj : ugptrlst.cpp
    cl $(CPP_PROJ) ugptrlst.cpp

free\ugsidehd.obj : ugsidehd.cpp
    cl $(CPP_PROJ) ugsidehd.cpp

free\ugstring.obj : ugstring.cpp
    cl $(CPP_PROJ) ugstring.cpp

free\ugtab.obj : ugtab.cpp
    cl $(CPP_PROJ) ugtab.cpp

free\ugtophdg.obj : ugtophdg.cpp
    cl $(CPP_PROJ) ugtophdg.cpp

free\ugvscrol.obj : ugvscrol.cpp
    cl $(CPP_PROJ) ugvscrol.cpp

