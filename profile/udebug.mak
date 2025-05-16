CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"udebug\profile.pch" /YX"windows.h" /Fo"udebug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\udebuglib\v5subs.lib ..\udebuglib\dundas.lib /nologo /debug /subsystem:windows /incremental:no /pdb:"udebug\profile.pdb"  /NODEFAULTLIB:LIBCMT.LIB  /machine:I386 /out:"udebug/profile.exe"

RSC_PROJ=/l 0x409 /fo"udebug\profile.res"

udebug\profile.exe : udebug\profile.obj udebug\alarmexp.obj udebug\bitmapcl.obj \
    udebug\box.obj udebug\chospart.obj udebug\chosshot.obj udebug\colordef.obj \
    udebug\colors.obj udebug\corner.obj udebug\ctrlimit.obj udebug\dbarcfg.obj \
    udebug\dconfig.obj udebug\downbar.obj udebug\editop.obj udebug\editshif.obj \
    udebug\export.obj \
    udebug\fixaxis.obj udebug\gridclas.obj udebug\isalarm.obj udebug\master.obj \
    udebug\mplot.obj udebug\mulprint.obj udebug\paramhis.obj udebug\password.obj \
    udebug\plot.obj udebug\rtmlist.obj udebug\savepro.obj udebug\tcconfig.obj \
    udebug\trend.obj udebug\viewpara.obj udebug\vlbclass.obj udebug\xaxis.obj \
    udebug\profile.res
    link $(LINK32_FLAGS) udebug\profile.obj udebug\alarmexp.obj udebug\bitmapcl.obj \
    udebug\box.obj udebug\chospart.obj udebug\chosshot.obj udebug\colordef.obj \
    udebug\colors.obj udebug\corner.obj udebug\ctrlimit.obj udebug\dbarcfg.obj \
    udebug\dconfig.obj udebug\downbar.obj udebug\editop.obj udebug\editshif.obj \
    udebug\export.obj \
    udebug\fixaxis.obj udebug\gridclas.obj udebug\isalarm.obj udebug\master.obj \
    udebug\mplot.obj udebug\mulprint.obj udebug\paramhis.obj udebug\password.obj \
    udebug\plot.obj udebug\rtmlist.obj udebug\savepro.obj udebug\tcconfig.obj \
    udebug\trend.obj udebug\viewpara.obj udebug\vlbclass.obj udebug\xaxis.obj \
    udebug\profile.res
    copy udebug\profile.exe \v5wide\exes

udebug\alarmexp.obj : alarmexp.cpp
    cl $(CPP_PROJ) alarmexp.cpp

udebug\bitmapcl.obj : bitmapcl.cpp
    cl $(CPP_PROJ) bitmapcl.cpp

udebug\box.obj : box.cpp
    cl $(CPP_PROJ) box.cpp

udebug\chospart.obj : chospart.cpp
    cl $(CPP_PROJ) chospart.cpp

udebug\chosshot.obj : chosshot.cpp
    cl $(CPP_PROJ) chosshot.cpp

udebug\colordef.obj : colordef.cpp
    cl $(CPP_PROJ) colordef.cpp

udebug\colors.obj : colors.cpp
    cl $(CPP_PROJ) colors.cpp

udebug\corner.obj : corner.cpp
    cl $(CPP_PROJ) corner.cpp

udebug\ctrlimit.obj : ctrlimit.cpp
    cl $(CPP_PROJ) ctrlimit.cpp

udebug\dbarcfg.obj : dbarcfg.cpp
    cl $(CPP_PROJ) dbarcfg.cpp

udebug\dconfig.obj : dconfig.cpp
    cl $(CPP_PROJ) dconfig.cpp

udebug\downbar.obj : downbar.cpp
    cl $(CPP_PROJ) downbar.cpp

udebug\editop.obj : editop.cpp
    cl $(CPP_PROJ) editop.cpp

udebug\editshif.obj : editshif.cpp
    cl $(CPP_PROJ) editshif.cpp

udebug\export.obj : export.cpp
    cl $(CPP_PROJ) export.cpp

udebug\fixaxis.obj : fixaxis.cpp
    cl $(CPP_PROJ) fixaxis.cpp

udebug\gridclas.obj : gridclas.cpp
    cl $(CPP_PROJ) gridclas.cpp

udebug\isalarm.obj : isalarm.cpp
    cl $(CPP_PROJ) isalarm.cpp

udebug\master.obj : master.cpp
    cl $(CPP_PROJ) master.cpp

udebug\mplot.obj : mplot.cpp
    cl $(CPP_PROJ) mplot.cpp

udebug\mulprint.obj : mulprint.cpp
    cl $(CPP_PROJ) mulprint.cpp

udebug\paramhis.obj : paramhis.cpp
    cl $(CPP_PROJ) paramhis.cpp

udebug\password.obj : password.cpp
    cl $(CPP_PROJ) password.cpp

udebug\plot.obj : plot.cpp
    cl $(CPP_PROJ) plot.cpp

udebug\profile.obj : profile.cpp
    cl $(CPP_PROJ) profile.cpp

udebug\rtmlist.obj : rtmlist.cpp
    cl $(CPP_PROJ) rtmlist.cpp

udebug\savepro.obj : savepro.cpp
    cl $(CPP_PROJ) savepro.cpp

udebug\tcconfig.obj : tcconfig.cpp
    cl $(CPP_PROJ) tcconfig.cpp

udebug\trend.obj : trend.cpp
    cl $(CPP_PROJ) trend.cpp

udebug\viewpara.obj : viewpara.cpp
    cl $(CPP_PROJ) viewpara.cpp

udebug\vlbclass.obj : vlbclass.cpp
    cl $(CPP_PROJ) vlbclass.cpp

udebug\xaxis.obj : xaxis.cpp
    cl $(CPP_PROJ) xaxis.cpp

udebug\profile.res : profile.rc
    rc $(RSC_PROJ) profile.rc


