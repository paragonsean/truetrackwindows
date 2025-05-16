CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LIB32_FLAGS=/nologo /out:"..\freelib\v5subs.lib"

..\freelib\v5subs.lib : free\alarmsum.obj free\array.obj free\ascfloat.obj \
 free\asensor.obj free\bitclass.obj free\callfile.obj free\catclass.obj free\chaxis.obj \
 free\client.obj free\color.obj free\colorcl.obj free\computer.obj free\converta.obj free\copymach.obj \
 free\copypart.obj free\current.obj free\dateclas.obj free\dbsubs.obj \
 free\dcurve.obj free\dow.obj free\downsubs.obj free\downtime.obj free\dstat.obj free\dtotchar.obj \
 free\dynamic_file_name.obj \
 free\error_ms.obj free\evclass.obj free\execwait.obj free\exists.obj free\extract.obj \
 free\fifo.obj free\fileclas.obj free\fillpart.obj \
 free\fillshot.obj free\fix.obj free\fontclas.obj free\ftanalog.obj \
 free\ftii_file.obj  free\ftii_parameters.obj free\ftii_shot_data.obj free\ftiiclas.obj \
 free\ftime.obj free\ftoasc.obj free\genlist.obj free\getshot.obj free\get_ini.obj \
 free\get_tc.obj free\gexedir.obj free\gpvar.obj free\hex.obj free\hourglas.obj free\iniclass.obj \
 free\insalph.obj free\is_empty.obj free\itoasc.obj free\killdir.obj free\listbox.obj free\log_file_string.obj \
 free\machcomp.obj free\machine.obj free\machname.obj free\marks.obj free\maximize.obj \
 free\mem.obj free\menu.obj free\mexists.obj free\misc.obj free\monrun.obj free\msgclas.obj \
 free\namecopy.obj free\nameclas.obj free\names.obj free\optocontrol.obj free\param.obj free\param_index_class.obj free\part.obj free\part_analog_sensor.obj free\path.obj \
 free\pexists.obj free\plookup.obj free\plotclas.obj free\plotsubs.obj free\pointcl.obj free\postsec.obj \
 free\profiles.obj free\pw.obj \
 free\readline.obj free\rectclas.obj free\reverse.obj free\ringcl.obj free\rjust.obj \
 free\runlist.obj \
 free\semaphor.obj free\sensor.obj free\setpoint.obj free\shift.obj free\shotclas.obj free\sockcli.obj free\startpar.obj \
 free\stddown.obj free\stpres.obj free\strarray.obj \
 free\stparam.obj free\stringcl.obj free\structs.obj free\stsetup.obj free\textlen.obj \
 free\textlist.obj free\temp_message.obj free\time.obj \
 free\timeclas.obj free\unitadj.obj free\units.obj free\updown.obj free\visigrid.obj free\visiplot.obj \
 free\warmupcl.obj free\wclass.obj free\winsubs.obj free\workclas.obj free\writelin.obj free\zero.obj
 link -lib $(LIB32_FLAGS) free\alarmsum.obj free\array.obj free\ascfloat.obj \
 free\asensor.obj free\bitclass.obj free\callfile.obj free\catclass.obj free\chaxis.obj \
 free\client.obj free\color.obj free\colorcl.obj free\computer.obj free\converta.obj free\copymach.obj \
 free\copypart.obj free\current.obj free\dateclas.obj free\dbsubs.obj \
 free\dcurve.obj free\dow.obj free\downsubs.obj free\downtime.obj free\dstat.obj free\dtotchar.obj \
 free\dynamic_file_name.obj \
 free\error_ms.obj free\evclass.obj free\execwait.obj free\exists.obj free\extract.obj \
 free\fifo.obj free\fileclas.obj free\fillpart.obj \
 free\fillshot.obj free\fix.obj free\fontclas.obj free\ftanalog.obj \
 free\ftii_file.obj free\ftii_parameters.obj free\ftii_shot_data.obj free\ftiiclas.obj \
 free\ftime.obj free\ftoasc.obj free\genlist.obj free\getshot.obj free\get_ini.obj \
 free\get_tc.obj free\gexedir.obj free\gpvar.obj free\hex.obj free\hourglas.obj free\iniclass.obj \
 free\insalph.obj free\is_empty.obj free\itoasc.obj free\killdir.obj free\listbox.obj free\log_file_string.obj \
 free\machcomp.obj free\machine.obj free\machname.obj free\marks.obj free\maximize.obj \
 free\mem.obj free\menu.obj free\mexists.obj free\misc.obj free\monrun.obj free\msgclas.obj \
 free\namecopy.obj free\nameclas.obj free\names.obj free\optocontrol.obj free\param.obj free\param_index_class.obj free\part.obj free\part_analog_sensor.obj free\path.obj \
 free\pexists.obj free\plookup.obj free\plotclas.obj free\plotsubs.obj free\pointcl.obj free\postsec.obj \
 free\profiles.obj free\pw.obj \
 free\readline.obj free\rectclas.obj free\reverse.obj free\ringcl.obj free\rjust.obj \
 free\runlist.obj \
 free\semaphor.obj free\sensor.obj free\setpoint.obj free\shift.obj free\shotclas.obj free\sockcli.obj free\startpar.obj \
 free\stddown.obj free\stpres.obj free\strarray.obj \
 free\stparam.obj free\stringcl.obj free\structs.obj free\stsetup.obj free\textlen.obj \
 free\textlist.obj free\temp_message.obj free\time.obj \
 free\timeclas.obj free\unitadj.obj free\units.obj free\updown.obj free\visigrid.obj free\visiplot.obj \
 free\warmupcl.obj free\wclass.obj free\winsubs.obj free\workclas.obj free\writelin.obj free\zero.obj

free\alarmsum.obj : alarmsum.cpp
    cl $(CPP_PROJ) alarmsum.cpp

free\array.obj : array.cpp ..\include\stringcl.h ..\include\fileclas.h ..\include\array.h
    cl $(CPP_PROJ) array.cpp

free\ascfloat.obj : ascfloat.cpp
    cl $(CPP_PROJ) ascfloat.cpp

free\asensor.obj : asensor.cpp
    cl $(CPP_PROJ) asensor.cpp

free\bitclass.obj : bitclass.cpp ..\include\bitclass.h
    cl $(CPP_PROJ) bitclass.cpp

free\CALLFILE.obj : CALLFILE.CPP
    cl $(CPP_PROJ) CALLFILE.CPP

free\CATCLASS.obj : CATCLASS.CPP
    cl $(CPP_PROJ) CATCLASS.CPP

free\chaxis.obj : chaxis.cpp ..\include\chaxis.h ..\include\fileclas.h
    cl $(CPP_PROJ) chaxis.cpp

free\CLIENT.obj : CLIENT.CPP ..\include\events.h
    cl $(CPP_PROJ) CLIENT.CPP

free\color.obj : color.cpp ..\include\color.h
    cl $(CPP_PROJ) color.cpp

free\colorcl.obj : colorcl.cpp ..\include\colorcl.h
    cl $(CPP_PROJ) colorcl.cpp

free\COMPUTER.obj : COMPUTER.CPP ..\include\computer.h
    cl $(CPP_PROJ) COMPUTER.CPP

free\CONVERTA.obj : CONVERTA.CPP
    cl $(CPP_PROJ) CONVERTA.CPP

free\COPYMACH.obj : copymach.cpp ..\include\computer.h
    cl $(CPP_PROJ) copymach.cpp

free\copypart.obj : copypart.cpp ..\include\computer.h ..\include\ftii.h ..\include\part.h
    cl $(CPP_PROJ) copypart.cpp

free\CURRENT.obj : CURRENT.CPP
    cl $(CPP_PROJ) CURRENT.CPP

free\dateclas.obj : dateclas.cpp
    cl $(CPP_PROJ) dateclas.cpp

free\DBSUBS.obj : DBSUBS.CPP
    cl $(CPP_PROJ) DBSUBS.CPP

free\DCURVE.obj : DCURVE.CPP
    cl $(CPP_PROJ) DCURVE.CPP

free\dow.obj : dow.cpp
    cl $(CPP_PROJ) dow.cpp

free\downsubs.obj : downsubs.cpp
    cl $(CPP_PROJ) downsubs.cpp

free\downtime.obj : downtime.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) downtime.cpp

free\dstat.obj : dstat.cpp
    cl $(CPP_PROJ) dstat.cpp

free\dtotchar.obj : dtotchar.cpp
    cl $(CPP_PROJ) dtotchar.cpp

free\dynamic_file_name.obj : dynamic_file_name.cpp ..\include\stringcl.h ..\include\computer.h ..\include\timeclas.h
    cl $(CPP_PROJ) dynamic_file_name.cpp

free\ERROR_MS.obj : ERROR_MS.CPP
    cl $(CPP_PROJ) ERROR_MS.CPP

free\evclass.obj : evclass.cpp ..\include\evclass.h
    cl $(CPP_PROJ) evclass.cpp

free\execwait.obj : execwait.cpp
    cl $(CPP_PROJ) execwait.cpp

free\exists.obj : exists.cpp
    cl $(CPP_PROJ) exists.cpp

free\EXTRACT.obj : EXTRACT.CPP
    cl $(CPP_PROJ) EXTRACT.CPP

free\fifo.obj : fifo.cpp ..\include\fifo.h
    cl $(CPP_PROJ) fifo.cpp

free\fileclas.obj : fileclas.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) fileclas.cpp

free\fillpart.obj : fillpart.cpp
    cl $(CPP_PROJ) fillpart.cpp

free\fillshot.obj : fillshot.cpp
    cl $(CPP_PROJ) fillshot.cpp

free\fix.obj : fix.cpp
    cl $(CPP_PROJ) fix.cpp

free\fontclas.obj : fontclas.cpp ..\include\fontclas.h
    cl $(CPP_PROJ) fontclas.cpp

free\FTANALOG.obj : FTANALOG.CPP ..\include\ftanalog.h
    cl $(CPP_PROJ) FTANALOG.CPP

free\ftii_file.obj : ftii_file.cpp ..\include\ftii_file.h
    cl $(CPP_PROJ) ftii_file.cpp

free\ftii_parameters.obj : ftii_parameters.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h ..\include\ftanalog.h
    cl $(CPP_PROJ) ftii_parameters.cpp

free\ftii_shot_data.obj : ftii_shot_data.cpp ..\include\ftii.h ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h
    cl $(CPP_PROJ) ftii_shot_data.cpp

free\ftiiclas.obj : ftiiclas.cpp ..\include\ftii.h ..\include\ftii_file.h
    cl $(CPP_PROJ) ftiiclas.cpp

free\ftime.obj : ftime.cpp
    cl $(CPP_PROJ) ftime.cpp

free\FTOASC.obj : FTOASC.CPP
    cl $(CPP_PROJ) FTOASC.CPP

free\GENLIST.obj : GENLIST.CPP
    cl $(CPP_PROJ) GENLIST.CPP

free\GETSHOT.obj : GETSHOT.CPP
    cl $(CPP_PROJ) GETSHOT.CPP

free\GET_INI.obj : GET_INI.CPP
    cl $(CPP_PROJ) GET_INI.CPP

free\get_tc.obj : get_tc.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) get_tc.cpp

free\GEXEDIR.obj : GEXEDIR.CPP ..\include\stringcl.h
    cl $(CPP_PROJ) GEXEDIR.CPP

free\GPVAR.obj : GPVAR.CPP ..\include\fileclas.h
    cl $(CPP_PROJ) GPVAR.CPP

free\hex.obj : hex.cpp
    cl $(CPP_PROJ) hex.cpp

free\hourglas.obj : hourglas.cpp
    cl $(CPP_PROJ) hourglas.cpp

free\iniclass.obj : iniclass.cpp ..\include\iniclass.h
    cl $(CPP_PROJ) iniclass.cpp

free\INSALPH.obj : INSALPH.CPP
    cl $(CPP_PROJ) INSALPH.CPP

free\IS_EMPTY.obj : IS_EMPTY.CPP
    cl $(CPP_PROJ) IS_EMPTY.CPP

free\ITOASC.obj : ITOASC.CPP
    cl $(CPP_PROJ) ITOASC.CPP

free\KILLDIR.obj : KILLDIR.CPP
    cl $(CPP_PROJ) KILLDIR.CPP

free\listbox.obj : listbox.cpp
    cl $(CPP_PROJ) listbox.cpp

free\log_file_string.obj : log_file_string.cpp
    cl $(CPP_PROJ) log_file_string.cpp

free\MACHCOMP.obj : MACHCOMP.CPP ..\include\computer.h
    cl $(CPP_PROJ) MACHCOMP.CPP

free\MACHINE.obj : MACHINE.CPP
    cl $(CPP_PROJ) MACHINE.CPP

free\MACHNAME.obj : MACHNAME.CPP ..\include\machname.h
    cl $(CPP_PROJ) MACHNAME.CPP

free\MARKS.obj : MARKS.CPP
    cl $(CPP_PROJ) MARKS.CPP

free\MAXIMIZE.obj : MAXIMIZE.CPP
    cl $(CPP_PROJ) MAXIMIZE.CPP

free\MEM.obj : MEM.CPP
    cl $(CPP_PROJ) MEM.CPP

free\menu.obj : menu.cpp
    cl $(CPP_PROJ) menu.cpp

free\mexists.obj : mexists.cpp
    cl $(CPP_PROJ) mexists.cpp

free\misc.obj : misc.cpp
    cl $(CPP_PROJ) misc.cpp

free\monrun.obj : monrun.cpp
    cl $(CPP_PROJ) monrun.cpp

free\msgclas.obj : msgclas.cpp ..\include\msgclas.h
    cl $(CPP_PROJ) msgclas.cpp

free\namecopy.obj : namecopy.cpp
    cl $(CPP_PROJ) namecopy.cpp

free\nameclas.obj : nameclas.cpp ..\include\computer.h ..\include\stringcl.h ..\include\nameclas.h
    cl $(CPP_PROJ) nameclas.cpp

free\names.obj : names.cpp ..\include\computer.h
    cl $(CPP_PROJ) names.cpp

free\optocontrol.obj : optocontrol.cpp
    cl $(CPP_PROJ) optocontrol.cpp

free\param.obj : param.cpp
    cl $(CPP_PROJ) param.cpp

free\param_index_class.obj : param_index_class.cpp ..\include\param_index_class.h
    cl $(CPP_PROJ) param_index_class.cpp

free\part.obj : part.cpp ..\include\part.h ..\include\stringcl.h ..\include\nameclas.h
    cl $(CPP_PROJ) part.cpp

free\part_analog_sensor.obj : part_analog_sensor.cpp ..\include\part_analog_sensor.h
    cl $(CPP_PROJ) part_analog_sensor.cpp

free\path.obj : path.cpp
    cl $(CPP_PROJ) path.cpp

free\pexists.obj : pexists.cpp
    cl $(CPP_PROJ) pexists.cpp

free\plookup.obj : plookup.cpp
    cl $(CPP_PROJ) plookup.cpp

free\plotclas.obj : plotclas.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) plotclas.cpp

free\plotsubs.obj : plotsubs.cpp
    cl $(CPP_PROJ) plotsubs.cpp

free\pointcl.obj : pointcl.cpp ..\include\pointcl.h
    cl $(CPP_PROJ) pointcl.cpp

free\POSTSEC.obj : POSTSEC.CPP
    cl $(CPP_PROJ) POSTSEC.CPP

free\profiles.obj : profiles.cpp ..\include\stringcl.h ..\include\nameclas.h
    cl $(CPP_PROJ) profiles.cpp

free\pw.obj : pw.cpp ..\include\computer.h
    cl $(CPP_PROJ) pw.cpp

free\READLINE.obj : READLINE.CPP
    cl $(CPP_PROJ) READLINE.CPP

free\rectclas.obj : rectclas.cpp ..\include\rectclas.h
    cl $(CPP_PROJ) rectclas.cpp

free\REVERSE.obj : REVERSE.CPP
    cl $(CPP_PROJ) REVERSE.CPP

free\ringcl.obj : ringcl.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) ringcl.cpp

free\RJUST.obj : RJUST.CPP
    cl $(CPP_PROJ) RJUST.CPP

free\runlist.obj : runlist.cpp ..\include\runlist.h
    cl $(CPP_PROJ) runlist.cpp

free\SEMAPHOR.obj : SEMAPHOR.CPP
    cl $(CPP_PROJ) SEMAPHOR.CPP

free\SENSOR.obj : SENSOR.CPP
    cl $(CPP_PROJ) SENSOR.CPP

free\setpoint.obj : setpoint.CPP ..\include\setpoint.h ..\include\strarray.h
    cl $(CPP_PROJ) setpoint.CPP

free\SHIFT.obj : SHIFT.CPP ..\include\timeclas.h
    cl $(CPP_PROJ) SHIFT.CPP

free\shotclas.obj : shotclas.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) shotclas.cpp

free\sockcli.obj : sockcli.cpp ..\include\sockcli.h
    cl $(CPP_PROJ) sockcli.cpp

free\startpar.obj : startpar.cpp
    cl $(CPP_PROJ) startpar.cpp

free\stddown.obj : stddown.cpp ..\include\stddown.h
    cl $(CPP_PROJ) stddown.cpp

free\stparam.obj : stparam.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) stparam.cpp

free\stpres.obj : stpres.cpp ..\include\stringcl.h ..\include\fileclas.h
    cl $(CPP_PROJ) stpres.cpp

free\strarray.obj : strarray.cpp ..\include\strarray.h
    cl $(CPP_PROJ) strarray.cpp

free\stringcl.obj : stringcl.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) stringcl.cpp

free\STRUCTS.obj : STRUCTS.CPP
    cl $(CPP_PROJ) STRUCTS.CPP

free\stsetup.obj : stsetup.cpp ..\include\stringcl.h ..\include\fileclas.h
    cl $(CPP_PROJ) stsetup.cpp

free\textlen.obj : textlen.CPP
    cl $(CPP_PROJ) textlen.CPP

free\textlist.obj : textlist.CPP
    cl $(CPP_PROJ) textlist.CPP

free\temp_message.obj : temp_message.cpp ..\include\wclass.h
    cl $(CPP_PROJ) temp_message.cpp

free\time.obj : TIME.CPP
    cl $(CPP_PROJ) TIME.CPP

free\timeclas.obj : timeclas.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) timeclas.cpp

free\UNITADJ.obj : UNITADJ.CPP
    cl $(CPP_PROJ) UNITADJ.CPP

free\UNITS.obj : UNITS.CPP
    cl $(CPP_PROJ) UNITS.CPP

free\updown.obj : updown.cpp
    cl $(CPP_PROJ) updown.cpp

free\visigrid.obj : visigrid.cpp ..\include\visigrid.h
    cl $(CPP_PROJ) visigrid.cpp

free\visiplot.obj : visiplot.cpp ..\include\visiplot.h ..\include\stringcl.h ..\include\wclass.h
    cl $(CPP_PROJ) visiplot.cpp

free\warmupcl.obj : warmupcl.cpp ..\include\warmupcl.h ..\include\iniclass.h
    cl $(CPP_PROJ) warmupcl.cpp

free\wclass.obj : wclass.cpp ..\include\wclass.h
    cl $(CPP_PROJ) wclass.cpp

free\WINSUBS.obj : WINSUBS.CPP
    cl $(CPP_PROJ) WINSUBS.CPP

free\workclas.obj : workclas.CPP
    cl $(CPP_PROJ) workclas.CPP

free\writelin.obj : writelin.cpp
    cl $(CPP_PROJ) writelin.cpp

free\ZERO.obj : ZERO.CPP
    cl $(CPP_PROJ) ZERO.CPP

