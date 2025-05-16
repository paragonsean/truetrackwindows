CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /NODEFAULTLIB:LIBC.LIB ..\freelib\dundas.lib /MAP /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/profile.exe"

RSC_PROJ=/l 0x409 /fo"free\profile.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=profile.rc
!ENDIF

free\profile.exe : free\profile.obj free\alarmexp.obj free\bitmapcl.obj \
    free\box.obj free\chospart.obj free\chosshot.obj free\colordef.obj \
    free\colors.obj free\corner.obj free\ctrlimit.obj free\dbarcfg.obj \
    free\dconfig.obj free\downbar.obj free\editop.obj free\editshif.obj \
    free\export.obj free\fixaxis.obj \
    free\gridclas.obj free\isalarm.obj free\master.obj \
    free\mplot.obj free\mulprint.obj free\olmsted.obj free\paramhis.obj free\paramwin.obj free\password.obj \
    free\plot.obj free\rtmlist.obj free\savepro.obj \
    free\status.obj free\tcconfig.obj \
    free\trend.obj free\viewpara.obj free\vlbclass.obj free\xaxis.obj free\xdcrplot.obj \
    free\profile.res
    link $(LINK32_FLAGS) free\profile.obj free\alarmexp.obj free\bitmapcl.obj \
    free\box.obj free\chospart.obj free\chosshot.obj free\colordef.obj \
    free\colors.obj free\corner.obj free\ctrlimit.obj free\dbarcfg.obj \
    free\dconfig.obj free\downbar.obj free\editop.obj free\editshif.obj \
    free\export.obj free\fixaxis.obj \
    free\gridclas.obj free\isalarm.obj free\master.obj \
    free\mplot.obj free\mulprint.obj free\olmsted.obj free\paramhis.obj free\paramwin.obj free\password.obj \
    free\plot.obj free\rtmlist.obj free\savepro.obj \
    free\status.obj free\tcconfig.obj \
    free\trend.obj free\viewpara.obj free\vlbclass.obj free\xaxis.obj free\xdcrplot.obj \
    free\profile.res
    copy free\profile.exe \v5\exes

free\alarmexp.obj : alarmexp.cpp
    cl $(CPP_PROJ) alarmexp.cpp

free\bitmapcl.obj : bitmapcl.cpp
    cl $(CPP_PROJ) bitmapcl.cpp

free\box.obj : box.cpp
    cl $(CPP_PROJ) box.cpp

free\chospart.obj : chospart.cpp
    cl $(CPP_PROJ) chospart.cpp

free\chosshot.obj : chosshot.cpp
    cl $(CPP_PROJ) chosshot.cpp

free\colordef.obj : colordef.cpp
    cl $(CPP_PROJ) colordef.cpp

free\colors.obj : colors.cpp
    cl $(CPP_PROJ) colors.cpp

free\corner.obj : corner.cpp
    cl $(CPP_PROJ) corner.cpp

free\ctrlimit.obj : ctrlimit.cpp
    cl $(CPP_PROJ) ctrlimit.cpp

free\dbarcfg.obj : dbarcfg.cpp ..\include\catclass.h
    cl $(CPP_PROJ) dbarcfg.cpp

free\dconfig.obj : dconfig.cpp
    cl $(CPP_PROJ) dconfig.cpp

free\downbar.obj : downbar.cpp
    cl $(CPP_PROJ) downbar.cpp

free\editop.obj : editop.cpp
    cl $(CPP_PROJ) editop.cpp

free\editshif.obj : editshif.cpp
    cl $(CPP_PROJ) editshif.cpp

free\export.obj : export.cpp
    cl $(CPP_PROJ) export.cpp

free\fixaxis.obj : fixaxis.cpp
    cl $(CPP_PROJ) fixaxis.cpp

free\gridclas.obj : gridclas.cpp
    cl $(CPP_PROJ) gridclas.cpp

free\isalarm.obj : isalarm.cpp
    cl $(CPP_PROJ) isalarm.cpp

free\master.obj : master.cpp
    cl $(CPP_PROJ) master.cpp

free\mplot.obj : mplot.cpp
    cl $(CPP_PROJ) mplot.cpp

free\mulprint.obj : mulprint.cpp
    cl $(CPP_PROJ) mulprint.cpp

free\olmsted.obj : olmsted.cpp
    cl $(CPP_PROJ) olmsted.cpp

free\paramhis.obj : paramhis.cpp
    cl $(CPP_PROJ) paramhis.cpp

free\paramwin.obj : paramwin.cpp
    cl $(CPP_PROJ) paramwin.cpp

free\password.obj : password.cpp
    cl $(CPP_PROJ) password.cpp

free\plot.obj : plot.cpp ..\include\part.h
    cl $(CPP_PROJ) plot.cpp

free\profile.obj : profile.cpp ..\include\catclass.h extern.h
    cl $(CPP_PROJ) profile.cpp

free\rtmlist.obj : rtmlist.cpp
    cl $(CPP_PROJ) rtmlist.cpp

free\savepro.obj : savepro.cpp
    cl $(CPP_PROJ) savepro.cpp

free\status.obj : status.cpp
    cl $(CPP_PROJ) status.cpp

free\tcconfig.obj : tcconfig.cpp
    cl $(CPP_PROJ) tcconfig.cpp

free\trend.obj : trend.cpp
    cl $(CPP_PROJ) trend.cpp

free\viewpara.obj : viewpara.cpp
    cl $(CPP_PROJ) viewpara.cpp

free\vlbclass.obj : vlbclass.cpp
    cl $(CPP_PROJ) vlbclass.cpp

free\xaxis.obj : xaxis.cpp
    cl $(CPP_PROJ) xaxis.cpp

free\xdcrplot.obj : xdcrplot.cpp
    cl $(CPP_PROJ) xdcrplot.cpp

free\profile.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)

