CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"udebug\v5subs.pch" /YX"windows.h" /Fo"udebug/" /c

LIB32_FLAGS=/nologo /out:"..\udebuglib\v5subs.lib"

..\udebuglib\v5subs.lib : udebug\alarmsum.obj udebug\array.obj udebug\ascfloat.obj \
 udebug\asensor.obj udebug\bitclass.obj udebug\callfile.obj udebug\catclass.obj udebug\chaxis.obj \
 udebug\client.obj udebug\color.obj udebug\computer.obj udebug\converta.obj udebug\copymach.obj \
 udebug\copypart.obj udebug\current.obj udebug\dateclas.obj udebug\dbsubs.obj \
 udebug\dcurve.obj udebug\dow.obj udebug\downsubs.obj udebug\downtime.obj udebug\dstat.obj udebug\dtotchar.obj \
 udebug\dynamic_file_name.obj \
 udebug\error_ms.obj udebug\evclass.obj udebug\execwait.obj udebug\exists.obj udebug\extract.obj \
 udebug\fifo.obj \
 udebug\fileclas.obj udebug\fillpart.obj udebug\fillshot.obj udebug\fix.obj udebug\fontclas.obj udebug\ftanalog.obj \
 udebug\ftii_file.obj udebug\ftii_parameters.obj udebug\ftii_shot_data.obj udebug\ftiiclas.obj \
 udebug\ftime.obj udebug\ftoasc.obj udebug\genlist.obj udebug\getshot.obj udebug\get_ini.obj \
 udebug\get_tc.obj udebug\gexedir.obj udebug\gpvar.obj udebug\hex.obj udebug\hourglas.obj udebug\iniclass.obj \
 udebug\insalph.obj udebug\is_empty.obj udebug\itoasc.obj udebug\killdir.obj udebug\listbox.obj udebug\log_file_string.obj \
 udebug\machcomp.obj udebug\machine.obj udebug\machname.obj udebug\marks.obj udebug\maximize.obj \
 udebug\mem.obj udebug\menu.obj udebug\mexists.obj udebug\misc.obj udebug\monrun.obj udebug\msgclas.obj \
 udebug\namecopy.obj udebug\names.obj udebug\param.obj udebug\part.obj udebug\part_analog_sensor.obj  udebug\path.obj \
 udebug\pexists.obj udebug\plookup.obj udebug\plotclas.obj udebug\plotsubs.obj udebug\pointcl.obj udebug\postsec.obj \
 udebug\profiles.obj udebug\readline.obj udebug\rectclas.obj udebug\reverse.obj udebug\ringcl.obj udebug\rjust.obj \
 udebug\runlist.obj \
 udebug\semaphor.obj udebug\sensor.obj udebug\shift.obj udebug\shotclas.obj udebug\sockcli.obj udebug\startpar.obj \
 udebug\stddown.obj  udebug\stpres.obj udebug\strarray.obj \
 udebug\stparam.obj udebug\stringcl.obj udebug\structs.obj udebug\stsetup.obj udebug\textlen.obj \
 udebug\textlist.obj udebug\temp_message.obj udebug\time.obj \
 udebug\timeclas.obj udebug\unitadj.obj udebug\units.obj udebug\updown.obj udebug\visiplot.obj \
 udebug\warmupcl.obj udebug\wclass.obj udebug\winsubs.obj udebug\workclas.obj udebug\writelin.obj udebug\zero.obj
 link -lib $(LIB32_FLAGS) udebug\alarmsum.obj udebug\array.obj udebug\ascfloat.obj \
 udebug\asensor.obj udebug\bitclass.obj udebug\callfile.obj udebug\catclass.obj udebug\chaxis.obj \
 udebug\client.obj udebug\color.obj udebug\computer.obj udebug\converta.obj udebug\copymach.obj \
 udebug\copypart.obj udebug\current.obj udebug\dateclas.obj udebug\dbsubs.obj \
 udebug\dcurve.obj udebug\dow.obj udebug\downsubs.obj udebug\downtime.obj udebug\dstat.obj udebug\dtotchar.obj \
 udebug\dynamic_file_name.obj \
 udebug\error_ms.obj udebug\evclass.obj udebug\execwait.obj udebug\exists.obj udebug\extract.obj \
 udebug\fifo.obj \
 udebug\fileclas.obj udebug\fillpart.obj udebug\fillshot.obj udebug\fix.obj udebug\fontclas.obj udebug\ftanalog.obj \
 udebug\ftii_file.obj udebug\ftii_parameters.obj udebug\ftii_shot_data.obj udebug\ftiiclas.obj \
 udebug\ftime.obj udebug\ftoasc.obj udebug\genlist.obj udebug\getshot.obj udebug\get_ini.obj \
 udebug\get_tc.obj udebug\gexedir.obj udebug\gpvar.obj udebug\hex.obj udebug\hourglas.obj udebug\iniclass.obj \
 udebug\insalph.obj udebug\is_empty.obj udebug\itoasc.obj udebug\killdir.obj udebug\listbox.obj udebug\log_file_string.obj \
 udebug\machcomp.obj udebug\machine.obj udebug\machname.obj udebug\marks.obj udebug\maximize.obj \
 udebug\mem.obj udebug\menu.obj udebug\mexists.obj udebug\misc.obj udebug\monrun.obj udebug\msgclas.obj \
 udebug\namecopy.obj udebug\names.obj udebug\param.obj udebug\part.obj udebug\part_analog_sensor.obj udebug\path.obj \
 udebug\pexists.obj udebug\plookup.obj udebug\plotclas.obj udebug\plotsubs.obj udebug\pointcl.obj udebug\postsec.obj \
 udebug\profiles.obj udebug\readline.obj udebug\rectclas.obj udebug\reverse.obj udebug\ringcl.obj udebug\rjust.obj \
 udebug\runlist.obj \
 udebug\semaphor.obj udebug\sensor.obj udebug\shift.obj udebug\shotclas.obj udebug\sockcli.obj udebug\startpar.obj \
 udebug\stddown.obj udebug\stpres.obj udebug\strarray.obj \
 udebug\stparam.obj udebug\stringcl.obj udebug\structs.obj udebug\stsetup.obj udebug\textlen.obj \
 udebug\textlist.obj udebug\temp_message.obj  udebug\time.obj \
 udebug\timeclas.obj udebug\unitadj.obj udebug\units.obj udebug\updown.obj udebug\visiplot.obj \
 udebug\warmupcl.obj udebug\wclass.obj udebug\winsubs.obj udebug\workclas.obj udebug\writelin.obj udebug\zero.obj

udebug\alarmsum.obj : alarmsum.cpp
    cl $(CPP_PROJ) alarmsum.cpp

udebug\array.obj : array.cpp
    cl $(CPP_PROJ) array.cpp

udebug\ASCFLOAT.obj : ASCFLOAT.CPP
    cl $(CPP_PROJ) ASCFLOAT.CPP

udebug\ASENSOR.obj : ASENSOR.CPP
    cl $(CPP_PROJ) ASENSOR.CPP

udebug\bitclass.obj : bitclass.cpp
    cl $(CPP_PROJ) bitclass.cpp

udebug\CALLFILE.obj : CALLFILE.CPP
    cl $(CPP_PROJ) CALLFILE.CPP

udebug\CATCLASS.obj : CATCLASS.CPP
    cl $(CPP_PROJ) CATCLASS.CPP

udebug\chaxis.obj : chaxis.cpp  ..\include\chaxis.h
    cl $(CPP_PROJ) chaxis.cpp

udebug\CLIENT.obj : CLIENT.CPP
    cl $(CPP_PROJ) CLIENT.CPP

udebug\COLOR.obj : COLOR.CPP
    cl $(CPP_PROJ) COLOR.CPP

udebug\COMPUTER.obj : COMPUTER.CPP
    cl $(CPP_PROJ) COMPUTER.CPP

udebug\CONVERTA.obj : CONVERTA.CPP
    cl $(CPP_PROJ) CONVERTA.CPP

udebug\COPYMACH.obj : COPYMACH.CPP
    cl $(CPP_PROJ) COPYMACH.CPP

udebug\COPYPART.obj : COPYPART.CPP
    cl $(CPP_PROJ) COPYPART.CPP

udebug\CURRENT.obj : CURRENT.CPP
    cl $(CPP_PROJ) CURRENT.CPP

udebug\dateclas.obj : dateclas.cpp
    cl $(CPP_PROJ) dateclas.cpp

udebug\DBSUBS.obj : DBSUBS.CPP
    cl $(CPP_PROJ) DBSUBS.CPP

udebug\DCURVE.obj : DCURVE.CPP
    cl $(CPP_PROJ) DCURVE.CPP

udebug\dow.obj : dow.cpp
    cl $(CPP_PROJ) dow.cpp

udebug\downsubs.obj : downsubs.cpp
    cl $(CPP_PROJ) downsubs.cpp

udebug\downtime.obj : downtime.cpp
    cl $(CPP_PROJ) downtime.cpp

udebug\dstat.obj : dstat.cpp
    cl $(CPP_PROJ) dstat.cpp

udebug\dtotchar.obj : dtotchar.cpp
    cl $(CPP_PROJ) dtotchar.cpp

udebug\dynamic_file_name.obj : dynamic_file_name.cpp
    cl $(CPP_PROJ) dynamic_file_name.cpp

udebug\ERROR_MS.obj : ERROR_MS.CPP
    cl $(CPP_PROJ) ERROR_MS.CPP

udebug\evclass.obj : evclass.cpp
    cl $(CPP_PROJ) evclass.cpp

udebug\execwait.obj : execwait.cpp
    cl $(CPP_PROJ) execwait.cpp

udebug\exists.obj : exists.cpp
    cl $(CPP_PROJ) exists.cpp

udebug\EXTRACT.obj : EXTRACT.CPP
    cl $(CPP_PROJ) EXTRACT.CPP

udebug\fifo.obj : fifo.cpp
    cl $(CPP_PROJ) fifo.cpp

udebug\fileclas.obj : fileclas.cpp
    cl $(CPP_PROJ) fileclas.cpp

udebug\FILLPART.obj : FILLPART.CPP
    cl $(CPP_PROJ) FILLPART.CPP

udebug\fillshot.obj : fillshot.cpp
    cl $(CPP_PROJ) fillshot.cpp

udebug\fix.obj : fix.cpp
    cl $(CPP_PROJ) fix.cpp

udebug\fontclas.obj : fontclas.cpp
    cl $(CPP_PROJ) fontclas.cpp

udebug\FTANALOG.obj : FTANALOG.CPP
    cl $(CPP_PROJ) FTANALOG.CPP

udebug\ftii_file.obj : ftii_file.cpp ..\include\ftii_file.h
    cl $(CPP_PROJ) ftii_file.cpp

udebug\ftii_parameters.obj : ftii_parameters.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h ..\include\ftanalog.h
    cl $(CPP_PROJ) ftii_parameters.cpp

udebug\ftii_shot_data.obj : ftii_shot_data.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h
    cl $(CPP_PROJ) ftii_shot_data.cpp

udebug\ftiiclas.obj : ftiiclas.cpp ..\include\ftii.h ..\include\ftii_file.h
    cl $(CPP_PROJ) ftiiclas.cpp

udebug\ftime.obj : ftime.cpp
    cl $(CPP_PROJ) ftime.cpp

udebug\FTOASC.obj : FTOASC.CPP
    cl $(CPP_PROJ) FTOASC.CPP

udebug\GENLIST.obj : GENLIST.CPP
    cl $(CPP_PROJ) GENLIST.CPP

udebug\GETSHOT.obj : GETSHOT.CPP
    cl $(CPP_PROJ) GETSHOT.CPP

udebug\GET_INI.obj : GET_INI.CPP
    cl $(CPP_PROJ) GET_INI.CPP

udebug\get_tc.obj : get_tc.cpp
    cl $(CPP_PROJ) get_tc.cpp

udebug\GEXEDIR.obj : GEXEDIR.CPP
    cl $(CPP_PROJ) GEXEDIR.CPP

udebug\GPVAR.obj : GPVAR.CPP
    cl $(CPP_PROJ) GPVAR.CPP

udebug\hex.obj : hex.cpp
    cl $(CPP_PROJ) hex.cpp

udebug\hourglas.obj : hourglas.cpp
    cl $(CPP_PROJ) hourglas.cpp

udebug\iniclass.obj : iniclass.cpp
    cl $(CPP_PROJ) iniclass.cpp

udebug\INSALPH.obj : INSALPH.CPP
    cl $(CPP_PROJ) INSALPH.CPP

udebug\IS_EMPTY.obj : IS_EMPTY.CPP
    cl $(CPP_PROJ) IS_EMPTY.CPP

udebug\ITOASC.obj : ITOASC.CPP
    cl $(CPP_PROJ) ITOASC.CPP

udebug\KILLDIR.obj : KILLDIR.CPP
    cl $(CPP_PROJ) KILLDIR.CPP

udebug\listbox.obj : listbox.cpp
    cl $(CPP_PROJ) listbox.cpp

udebug\log_file_string.obj : log_file_string.cpp
    cl $(CPP_PROJ) log_file_string.cpp

udebug\MACHCOMP.obj : MACHCOMP.CPP
    cl $(CPP_PROJ) MACHCOMP.CPP

udebug\MACHINE.obj : MACHINE.CPP
    cl $(CPP_PROJ) MACHINE.CPP

udebug\MACHNAME.obj : MACHNAME.CPP ..\include\machname.h
    cl $(CPP_PROJ) MACHNAME.CPP

udebug\MARKS.obj : MARKS.CPP
    cl $(CPP_PROJ) MARKS.CPP

udebug\MAXIMIZE.obj : MAXIMIZE.CPP
    cl $(CPP_PROJ) MAXIMIZE.CPP

udebug\MEM.obj : MEM.CPP
    cl $(CPP_PROJ) MEM.CPP

udebug\menu.obj : menu.cpp
    cl $(CPP_PROJ) menu.cpp

udebug\MEXISTS.obj : MEXISTS.CPP
    cl $(CPP_PROJ) MEXISTS.CPP

udebug\misc.obj : misc.cpp
    cl $(CPP_PROJ) misc.cpp

udebug\monrun.obj : monrun.cpp
    cl $(CPP_PROJ) monrun.cpp

udebug\msgclas.obj : msgclas.cpp
    cl $(CPP_PROJ) msgclas.cpp

udebug\namecopy.obj : namecopy.cpp
    cl $(CPP_PROJ) namecopy.cpp

udebug\NAMES.obj : NAMES.CPP
    cl $(CPP_PROJ) NAMES.CPP

udebug\PARAM.obj : PARAM.CPP
    cl $(CPP_PROJ) PARAM.CPP

udebug\PART.obj : PART.CPP
    cl $(CPP_PROJ) PART.CPP

udebug\part_analog_sensor.obj : part_analog_sensor.cpp
    cl $(CPP_PROJ) part_analog_sensor.cpp

udebug\path.obj : path.cpp
    cl $(CPP_PROJ) path.cpp

udebug\PEXISTS.obj : PEXISTS.CPP
    cl $(CPP_PROJ) PEXISTS.CPP

udebug\plookup.obj : plookup.cpp
    cl $(CPP_PROJ) plookup.cpp

udebug\plotclas.obj : plotclas.cpp
    cl $(CPP_PROJ) plotclas.cpp

udebug\plotsubs.obj : plotsubs.cpp
    cl $(CPP_PROJ) plotsubs.cpp

udebug\pointcl.obj : pointcl.cpp ..\include\pointcl.h
    cl $(CPP_PROJ) pointcl.cpp

udebug\POSTSEC.obj : POSTSEC.CPP
    cl $(CPP_PROJ) POSTSEC.CPP

udebug\profiles.obj : profiles.cpp
    cl $(CPP_PROJ) profiles.cpp

udebug\READLINE.obj : READLINE.CPP
    cl $(CPP_PROJ) READLINE.CPP

udebug\rectclas.obj : rectclas.CPP  ..\include\rectclas.h
    cl $(CPP_PROJ) rectclas.CPP

udebug\REVERSE.obj : REVERSE.CPP
    cl $(CPP_PROJ) REVERSE.CPP

udebug\ringcl.obj : ringcl.cpp
    cl $(CPP_PROJ) ringcl.cpp

udebug\RJUST.obj : RJUST.CPP
    cl $(CPP_PROJ) RJUST.CPP

udebug\runlist.obj : runlist.cpp  ..\include\runlist.h
    cl $(CPP_PROJ) runlist.cpp

udebug\SEMAPHOR.obj : SEMAPHOR.CPP
    cl $(CPP_PROJ) SEMAPHOR.CPP

udebug\SENSOR.obj : SENSOR.CPP
    cl $(CPP_PROJ) SENSOR.CPP

udebug\SHIFT.obj : SHIFT.CPP
    cl $(CPP_PROJ) SHIFT.CPP

udebug\shotclas.obj : shotclas.cpp
    cl $(CPP_PROJ) shotclas.cpp

udebug\sockcli.obj : sockcli.cpp
    cl $(CPP_PROJ) sockcli.cpp

udebug\startpar.obj : startpar.cpp
    cl $(CPP_PROJ) startpar.cpp

udebug\stddown.obj : stddown.cpp ..\include\stddown.h
    cl $(CPP_PROJ) stddown.cpp

udebug\stparam.obj : stparam.cpp
    cl $(CPP_PROJ) stparam.cpp

udebug\stpres.obj : stpres.cpp
    cl $(CPP_PROJ) stpres.cpp

udebug\strarray.obj : strarray.cpp ..\include\strarray.h
    cl $(CPP_PROJ) strarray.cpp

udebug\stringcl.obj : stringcl.cpp
    cl $(CPP_PROJ) stringcl.cpp

udebug\STRUCTS.obj : STRUCTS.CPP
    cl $(CPP_PROJ) STRUCTS.CPP

udebug\stsetup.obj : stsetup.cpp
    cl $(CPP_PROJ) stsetup.cpp

udebug\textlen.obj : textlen.CPP
    cl $(CPP_PROJ) textlen.CPP

udebug\TEXTLIST.obj : TEXTLIST.CPP
    cl $(CPP_PROJ) TEXTLIST.CPP

udebug\temp_message.obj : temp_message.cpp
    cl $(CPP_PROJ) temp_message.cpp

udebug\TIME.obj : TIME.CPP
    cl $(CPP_PROJ) TIME.CPP

udebug\timeclas.obj : timeclas.cpp
    cl $(CPP_PROJ) timeclas.cpp

udebug\UNITADJ.obj : UNITADJ.CPP
    cl $(CPP_PROJ) UNITADJ.CPP

udebug\UNITS.obj : UNITS.CPP
    cl $(CPP_PROJ) UNITS.CPP

udebug\updown.obj : updown.cpp
    cl $(CPP_PROJ) updown.cpp

udebug\visiplot.obj : visiplot.cpp
    cl $(CPP_PROJ) visiplot.cpp

udebug\wclass.obj : wclass.cpp
    cl $(CPP_PROJ) wclass.cpp

udebug\warmupcl.obj : warmupcl.cpp
    cl $(CPP_PROJ) warmupcl.cpp

udebug\WINSUBS.obj : WINSUBS.CPP
    cl $(CPP_PROJ) WINSUBS.CPP

udebug\workclas.obj : workclas.cpp
    cl $(CPP_PROJ) workclas.cpp

udebug\writelin.obj : writelin.cpp
    cl $(CPP_PROJ) writelin.cpp

udebug\ZERO.obj : ZERO.CPP
    cl $(CPP_PROJ) ZERO.CPP
