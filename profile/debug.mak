CPP_PROJ=/nologo /MTd /W3 /Od /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug\profile.pch" /YX"windows.h" /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\debuglib\v5subs.lib /NODEFAULTLIB:LIBC.LIB ..\debuglib\dundas.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\profile.pdb" /debug /machine:I386 /out:"debug/profile.exe"

RSC_PROJ=/l 0x409 /fo"debug\profile.res" /d "NDEBUG"

debug\profile.exe : debug\profile.obj debug\alarmexp.obj debug\bitmapcl.obj \
    debug\box.obj debug\chospart.obj debug\chosshot.obj debug\colordef.obj \
    debug\colors.obj debug\corner.obj debug\ctrlimit.obj debug\dbarcfg.obj \
    debug\dconfig.obj debug\downbar.obj debug\editop.obj debug\editshif.obj \
    debug\export.obj \
    debug\fixaxis.obj debug\gridclas.obj debug\isalarm.obj debug\master.obj \
    debug\mplot.obj debug\mulprint.obj debug\olmsted.obj debug\paramhis.obj debug\paramwin.obj debug\password.obj \
    debug\plot.obj debug\rtmlist.obj debug\savepro.obj debug\status.obj debug\tcconfig.obj \
    debug\trend.obj debug\viewpara.obj debug\vlbclass.obj debug\xaxis.obj debug\xdcrplot.obj \
    debug\profile.res
    link $(LINK32_FLAGS) debug\profile.obj debug\alarmexp.obj debug\bitmapcl.obj \
    debug\box.obj debug\chospart.obj debug\chosshot.obj debug\colordef.obj \
    debug\colors.obj debug\corner.obj debug\ctrlimit.obj debug\dbarcfg.obj \
    debug\dconfig.obj debug\downbar.obj debug\editop.obj debug\editshif.obj \
    debug\export.obj \
    debug\fixaxis.obj debug\gridclas.obj debug\isalarm.obj debug\master.obj \
    debug\mplot.obj debug\mulprint.obj debug\olmsted.obj debug\paramhis.obj debug\paramwin.obj debug\password.obj \
    debug\plot.obj debug\rtmlist.obj debug\savepro.obj debug\status.obj debug\tcconfig.obj \
    debug\trend.obj debug\viewpara.obj debug\vlbclass.obj debug\xaxis.obj debug\xdcrplot.obj \
    debug\profile.res
    copy debug\profile.exe \v5\exes

debug\alarmexp.obj : alarmexp.cpp
    cl $(CPP_PROJ) alarmexp.cpp

debug\bitmapcl.obj : bitmapcl.cpp
    cl $(CPP_PROJ) bitmapcl.cpp

debug\box.obj : box.cpp
    cl $(CPP_PROJ) box.cpp

debug\chospart.obj : chospart.cpp
    cl $(CPP_PROJ) chospart.cpp

debug\chosshot.obj : chosshot.cpp
    cl $(CPP_PROJ) chosshot.cpp

debug\colordef.obj : colordef.cpp
    cl $(CPP_PROJ) colordef.cpp

debug\colors.obj : colors.cpp
    cl $(CPP_PROJ) colors.cpp

debug\corner.obj : corner.cpp
    cl $(CPP_PROJ) corner.cpp

debug\ctrlimit.obj : ctrlimit.cpp
    cl $(CPP_PROJ) ctrlimit.cpp

debug\dbarcfg.obj : dbarcfg.cpp
    cl $(CPP_PROJ) dbarcfg.cpp

debug\dconfig.obj : dconfig.cpp
    cl $(CPP_PROJ) dconfig.cpp

debug\downbar.obj : downbar.cpp
    cl $(CPP_PROJ) downbar.cpp

debug\editop.obj : editop.cpp
    cl $(CPP_PROJ) editop.cpp

debug\editshif.obj : editshif.cpp
    cl $(CPP_PROJ) editshif.cpp

debug\export.obj : export.cpp
    cl $(CPP_PROJ) export.cpp

debug\fixaxis.obj : fixaxis.cpp
    cl $(CPP_PROJ) fixaxis.cpp

debug\gridclas.obj : gridclas.cpp
    cl $(CPP_PROJ) gridclas.cpp

debug\isalarm.obj : isalarm.cpp
    cl $(CPP_PROJ) isalarm.cpp

debug\master.obj : master.cpp
    cl $(CPP_PROJ) master.cpp

debug\mplot.obj : mplot.cpp
    cl $(CPP_PROJ) mplot.cpp

debug\mulprint.obj : mulprint.cpp
    cl $(CPP_PROJ) mulprint.cpp

debug\olmsted.obj : olmsted.cpp
    cl $(CPP_PROJ) olmsted.cpp

debug\paramhis.obj : paramhis.cpp
    cl $(CPP_PROJ) paramhis.cpp

debug\paramwin.obj : paramwin.cpp
    cl $(CPP_PROJ) paramwin.cpp

debug\password.obj : password.cpp
    cl $(CPP_PROJ) password.cpp

debug\plot.obj : plot.cpp
    cl $(CPP_PROJ) plot.cpp

debug\profile.obj : profile.cpp
    cl $(CPP_PROJ) profile.cpp

debug\rtmlist.obj : rtmlist.cpp
    cl $(CPP_PROJ) rtmlist.cpp

debug\savepro.obj : savepro.cpp
    cl $(CPP_PROJ) savepro.cpp

debug\status.obj : status.cpp
    cl $(CPP_PROJ) status.cpp

debug\tcconfig.obj : tcconfig.cpp
    cl $(CPP_PROJ) tcconfig.cpp

debug\trend.obj : trend.cpp
    cl $(CPP_PROJ) trend.cpp

debug\viewpara.obj : viewpara.cpp
    cl $(CPP_PROJ) viewpara.cpp

debug\vlbclass.obj : vlbclass.cpp
    cl $(CPP_PROJ) vlbclass.cpp

debug\xaxis.obj : xaxis.cpp
    cl $(CPP_PROJ) xaxis.cpp

debug\xdcrplot.obj : xdcrplot.cpp
    cl $(CPP_PROJ) xdcrplot.cpp

debug\profile.res : profile.rc
    rc $(RSC_PROJ) profile.rc

