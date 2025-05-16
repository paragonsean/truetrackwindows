CPP_PROJ=/nologo /MT /W3 /O1 /Zp1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /NODEFAULTLIB:LIBC.LIB ..\ufreelib\dundas.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"ufree/profile.exe"

RSC_PROJ=/l 0x409 /fo"ufree\profile.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=profile.rc
!ENDIF

ufree\profile.exe : ufree\profile.obj ufree\alarmexp.obj ufree\bitmapcl.obj \
    ufree\box.obj ufree\chospart.obj ufree\chosshot.obj ufree\colordef.obj \
    ufree\colors.obj ufree\corner.obj ufree\ctrlimit.obj ufree\dbarcfg.obj \
    ufree\dconfig.obj ufree\downbar.obj ufree\editop.obj ufree\editshif.obj \
    ufree\export.obj \
    ufree\fixaxis.obj ufree\gridclas.obj ufree\isalarm.obj ufree\master.obj \
    ufree\mplot.obj ufree\mulprint.obj ufree\olmsted.obj  ufree\paramhis.obj ufree\paramwin.obj ufree\password.obj \
    ufree\plot.obj ufree\rtmlist.obj ufree\savepro.obj \
    ufree\status.obj ufree\tcconfig.obj \
    ufree\trend.obj ufree\viewpara.obj ufree\vlbclass.obj ufree\xaxis.obj ufree\xdcrplot.obj \
    ufree\profile.res
    link $(LINK32_FLAGS) ufree\profile.obj ufree\alarmexp.obj ufree\bitmapcl.obj \
    ufree\box.obj ufree\chospart.obj ufree\chosshot.obj ufree\colordef.obj \
    ufree\colors.obj ufree\corner.obj ufree\ctrlimit.obj ufree\dbarcfg.obj \
    ufree\dconfig.obj ufree\downbar.obj ufree\editop.obj ufree\editshif.obj \
    ufree\export.obj \
    ufree\fixaxis.obj ufree\gridclas.obj ufree\isalarm.obj ufree\master.obj \
    ufree\mplot.obj ufree\mulprint.obj ufree\olmsted.obj  ufree\paramhis.obj ufree\paramwin.obj ufree\password.obj \
    ufree\plot.obj ufree\rtmlist.obj ufree\savepro.obj \
    ufree\status.obj ufree\tcconfig.obj \
    ufree\trend.obj ufree\viewpara.obj ufree\vlbclass.obj ufree\xaxis.obj ufree\xdcrplot.obj \
    ufree\profile.res
    copy ufree\profile.exe \v5wide\exes

ufree\alarmexp.obj : alarmexp.cpp
    cl $(CPP_PROJ) alarmexp.cpp

ufree\bitmapcl.obj : bitmapcl.cpp
    cl $(CPP_PROJ) bitmapcl.cpp

ufree\box.obj : box.cpp
    cl $(CPP_PROJ) box.cpp

ufree\chospart.obj : chospart.cpp
    cl $(CPP_PROJ) chospart.cpp

ufree\chosshot.obj : chosshot.cpp
    cl $(CPP_PROJ) chosshot.cpp

ufree\colordef.obj : colordef.cpp
    cl $(CPP_PROJ) colordef.cpp

ufree\colors.obj : colors.cpp
    cl $(CPP_PROJ) colors.cpp

ufree\corner.obj : corner.cpp
    cl $(CPP_PROJ) corner.cpp

ufree\ctrlimit.obj : ctrlimit.cpp
    cl $(CPP_PROJ) ctrlimit.cpp

ufree\dbarcfg.obj : dbarcfg.cpp
    cl $(CPP_PROJ) dbarcfg.cpp

ufree\dconfig.obj : dconfig.cpp
    cl $(CPP_PROJ) dconfig.cpp

ufree\downbar.obj : downbar.cpp
    cl $(CPP_PROJ) downbar.cpp

ufree\editop.obj : editop.cpp
    cl $(CPP_PROJ) editop.cpp

ufree\editshif.obj : editshif.cpp
    cl $(CPP_PROJ) editshif.cpp

ufree\export.obj : export.cpp
    cl $(CPP_PROJ) export.cpp

ufree\fixaxis.obj : fixaxis.cpp
    cl $(CPP_PROJ) fixaxis.cpp

ufree\gridclas.obj : gridclas.cpp
    cl $(CPP_PROJ) gridclas.cpp

ufree\isalarm.obj : isalarm.cpp
    cl $(CPP_PROJ) isalarm.cpp

ufree\master.obj : master.cpp
    cl $(CPP_PROJ) master.cpp

ufree\mplot.obj : mplot.cpp
    cl $(CPP_PROJ) mplot.cpp

ufree\mulprint.obj : mulprint.cpp
    cl $(CPP_PROJ) mulprint.cpp

ufree\olmsted.obj : olmsted.cpp
    cl $(CPP_PROJ) olmsted.cpp

ufree\paramhis.obj : paramhis.cpp
    cl $(CPP_PROJ) paramhis.cpp

ufree\paramwin.obj : paramwin.cpp
    cl $(CPP_PROJ) paramwin.cpp

ufree\password.obj : password.cpp
    cl $(CPP_PROJ) password.cpp

ufree\plot.obj : plot.cpp
    cl $(CPP_PROJ) plot.cpp

ufree\profile.obj : profile.cpp
    cl $(CPP_PROJ) profile.cpp

ufree\rtmlist.obj : rtmlist.cpp
    cl $(CPP_PROJ) rtmlist.cpp

ufree\savepro.obj : savepro.cpp
    cl $(CPP_PROJ) savepro.cpp

ufree\status.obj : status.cpp
    cl $(CPP_PROJ) status.cpp

ufree\tcconfig.obj : tcconfig.cpp
    cl $(CPP_PROJ) tcconfig.cpp

ufree\trend.obj : trend.cpp
    cl $(CPP_PROJ) trend.cpp

ufree\viewpara.obj : viewpara.cpp
    cl $(CPP_PROJ) viewpara.cpp

ufree\vlbclass.obj : vlbclass.cpp
    cl $(CPP_PROJ) vlbclass.cpp

ufree\xaxis.obj : xaxis.cpp
    cl $(CPP_PROJ) xaxis.cpp

ufree\xdcrplot.obj : xdcrplot.cpp
    cl $(CPP_PROJ) xdcrplot.cpp

ufree\profile.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)

