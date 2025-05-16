CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LIB32_FLAGS=/nologo /out:"..\ufreelib\v5subs.lib"

..\ufreelib\v5subs.lib : ufree\alarmsum.obj ufree\array.obj ufree\ascfloat.obj \
 ufree\asensor.obj ufree\setpoint.obj ufree\bitclass.obj ufree\callfile.obj ufree\catclass.obj ufree\chaxis.obj \
 ufree\client.obj ufree\color.obj ufree\colorcl.obj ufree\computer.obj ufree\converta.obj ufree\copymach.obj \
 ufree\copypart.obj ufree\current.obj ufree\dateclas.obj ufree\dbsubs.obj \
 ufree\dcurve.obj ufree\dow.obj ufree\downsubs.obj ufree\downtime.obj ufree\dstat.obj ufree\dtotchar.obj \
 ufree\dynamic_file_name.obj \
 ufree\error_ms.obj ufree\evclass.obj ufree\execwait.obj ufree\exists.obj ufree\extract.obj \
 ufree\fifo.obj ufree\fileclas.obj ufree\fillpart.obj \
 ufree\fillshot.obj ufree\fix.obj ufree\fontclas.obj ufree\ftanalog.obj \
 ufree\ftii_file.obj ufree\ftii_parameters.obj ufree\ftii_shot_data.obj ufree\ftiiclas.obj \
 ufree\ftime.obj ufree\ftoasc.obj ufree\genlist.obj ufree\getshot.obj ufree\get_ini.obj \
 ufree\get_tc.obj ufree\gexedir.obj ufree\gpvar.obj ufree\hex.obj ufree\hourglas.obj ufree\iniclass.obj \
 ufree\insalph.obj ufree\is_empty.obj ufree\itoasc.obj ufree\killdir.obj ufree\listbox.obj ufree\log_file_string.obj \
 ufree\machcomp.obj ufree\machine.obj ufree\machname.obj ufree\marks.obj ufree\maximize.obj \
 ufree\mem.obj ufree\menu.obj ufree\mexists.obj ufree\misc.obj ufree\monrun.obj ufree\msgclas.obj \
 ufree\namecopy.obj ufree\nameclas.obj ufree\names.obj ufree\optocontrol.obj ufree\param.obj ufree\param_index_class.obj ufree\part.obj ufree\part_analog_sensor.obj ufree\path.obj \
 ufree\pexists.obj ufree\plookup.obj ufree\plotclas.obj ufree\plotsubs.obj ufree\pointcl.obj ufree\postsec.obj \
 ufree\profiles.obj ufree\pw.obj \
 ufree\readline.obj ufree\rectclas.obj ufree\reverse.obj ufree\ringcl.obj ufree\rjust.obj \
 ufree\runlist.obj \
 ufree\semaphor.obj ufree\sensor.obj ufree\setpoint.obj ufree\shift.obj ufree\shotclas.obj ufree\sockcli.obj ufree\startpar.obj \
 ufree\stddown.obj ufree\stpres.obj ufree\strarray.obj \
 ufree\stparam.obj ufree\stringcl.obj ufree\structs.obj ufree\stsetup.obj ufree\textlen.obj \
 ufree\textlist.obj ufree\temp_message.obj ufree\time.obj \
 ufree\timeclas.obj ufree\unitadj.obj ufree\units.obj ufree\updown.obj ufree\visigrid.obj ufree\visiplot.obj \
 ufree\warmupcl.obj ufree\wclass.obj ufree\winsubs.obj ufree\workclas.obj ufree\writelin.obj ufree\zero.obj
 link -lib $(LIB32_FLAGS) ufree\alarmsum.obj ufree\array.obj ufree\ascfloat.obj \
 ufree\asensor.obj ufree\bitclass.obj ufree\callfile.obj ufree\catclass.obj ufree\chaxis.obj \
 ufree\client.obj ufree\color.obj ufree\colorcl.obj ufree\computer.obj ufree\converta.obj ufree\copymach.obj \
 ufree\copypart.obj ufree\current.obj ufree\dateclas.obj ufree\dbsubs.obj \
 ufree\dcurve.obj ufree\dow.obj ufree\downsubs.obj ufree\downtime.obj ufree\dstat.obj ufree\dtotchar.obj \
 ufree\dynamic_file_name.obj \
 ufree\error_ms.obj ufree\evclass.obj ufree\execwait.obj ufree\exists.obj ufree\extract.obj \
 ufree\fifo.obj ufree\fileclas.obj ufree\fillpart.obj \
 ufree\fillshot.obj ufree\fix.obj ufree\fontclas.obj ufree\ftanalog.obj \
 ufree\ftii_file.obj ufree\ftii_parameters.obj ufree\ftii_shot_data.obj ufree\ftiiclas.obj \
 ufree\ftime.obj ufree\ftoasc.obj ufree\genlist.obj ufree\getshot.obj ufree\get_ini.obj \
 ufree\get_tc.obj ufree\gexedir.obj ufree\gpvar.obj ufree\hex.obj ufree\hourglas.obj ufree\iniclass.obj \
 ufree\insalph.obj ufree\is_empty.obj ufree\itoasc.obj ufree\killdir.obj ufree\listbox.obj ufree\log_file_string.obj \
 ufree\machcomp.obj ufree\machine.obj ufree\machname.obj ufree\marks.obj ufree\maximize.obj \
 ufree\mem.obj ufree\menu.obj ufree\mexists.obj ufree\misc.obj ufree\monrun.obj ufree\msgclas.obj \
 ufree\namecopy.obj ufree\nameclas.obj ufree\names.obj ufree\optocontrol.obj ufree\param.obj ufree\param_index_class.obj ufree\part.obj ufree\part_analog_sensor.obj ufree\path.obj \
 ufree\pexists.obj ufree\plookup.obj ufree\plotclas.obj ufree\plotsubs.obj ufree\pointcl.obj ufree\postsec.obj \
 ufree\profiles.obj ufree\pw.obj \
 ufree\readline.obj ufree\rectclas.obj ufree\reverse.obj ufree\ringcl.obj ufree\rjust.obj \
 ufree\runlist.obj \
 ufree\semaphor.obj ufree\sensor.obj ufree/setpoint.obj ufree\shift.obj ufree\shotclas.obj ufree\sockcli.obj ufree\startpar.obj \
 ufree\stddown.obj ufree\stparam.obj ufree\stringcl.obj ufree\structs.obj ufree\stsetup.obj ufree\textlen.obj \
 ufree\stpres.obj ufree\strarray.obj\
 ufree\textlist.obj ufree\temp_message.obj ufree\time.obj \
 ufree\timeclas.obj ufree\unitadj.obj ufree\units.obj ufree\updown.obj ufree\visigrid.obj ufree\visiplot.obj \
 ufree\warmupcl.obj ufree\wclass.obj ufree\winsubs.obj ufree\workclas.obj ufree\writelin.obj ufree\zero.obj

ufree\alarmsum.obj : alarmsum.cpp
    cl $(CPP_PROJ) alarmsum.cpp

ufree\array.obj : array.cpp
    cl $(CPP_PROJ) array.cpp

ufree\ASCFLOAT.obj : ASCFLOAT.CPP
    cl $(CPP_PROJ) ASCFLOAT.CPP

ufree\ASENSOR.obj : ASENSOR.CPP
    cl $(CPP_PROJ) ASENSOR.CPP

ufree\bitclass.obj : bitclass.cpp
    cl $(CPP_PROJ) bitclass.cpp

ufree\CALLFILE.obj : CALLFILE.CPP
    cl $(CPP_PROJ) CALLFILE.CPP

ufree\catclass.obj : catclass.cpp
    cl $(CPP_PROJ) catclass.cpp

ufree\chaxis.obj : chaxis.cpp ..\include\chaxis.h
    cl $(CPP_PROJ) chaxis.cpp

ufree\client.obj : client.cpp
    cl $(CPP_PROJ) client.cpp

ufree\color.obj : color.cpp
    cl $(CPP_PROJ) color.cpp

ufree\colorcl.obj : colorcl.cpp
    cl $(CPP_PROJ) colorcl.cpp

ufree\COMPUTER.obj : COMPUTER.CPP
    cl $(CPP_PROJ) COMPUTER.CPP

ufree\CONVERTA.obj : CONVERTA.CPP
    cl $(CPP_PROJ) CONVERTA.CPP

ufree\COPYMACH.obj : COPYMACH.CPP
    cl $(CPP_PROJ) COPYMACH.CPP

ufree\COPYPART.obj : COPYPART.CPP
    cl $(CPP_PROJ) COPYPART.CPP

ufree\CURRENT.obj : CURRENT.CPP
    cl $(CPP_PROJ) CURRENT.CPP

ufree\dateclas.obj : dateclas.cpp
    cl $(CPP_PROJ) dateclas.cpp

ufree\DBSUBS.obj : DBSUBS.CPP
    cl $(CPP_PROJ) DBSUBS.CPP

ufree\DCURVE.obj : DCURVE.CPP
    cl $(CPP_PROJ) DCURVE.CPP

ufree\dow.obj : dow.cpp
    cl $(CPP_PROJ) dow.cpp

ufree\downsubs.obj : downsubs.cpp
    cl $(CPP_PROJ) downsubs.cpp

ufree\downtime.obj : downtime.cpp
    cl $(CPP_PROJ) downtime.cpp

ufree\dstat.obj : dstat.cpp
    cl $(CPP_PROJ) dstat.cpp

ufree\dtotchar.obj : dtotchar.cpp
    cl $(CPP_PROJ) dtotchar.cpp

ufree\dynamic_file_name.obj : dynamic_file_name.cpp
    cl $(CPP_PROJ) dynamic_file_name.cpp

ufree\ERROR_MS.obj : ERROR_MS.CPP
    cl $(CPP_PROJ) ERROR_MS.CPP

ufree\evclass.obj : evclass.cpp
    cl $(CPP_PROJ) evclass.cpp

ufree\execwait.obj : execwait.cpp
    cl $(CPP_PROJ) execwait.cpp

ufree\exists.obj : exists.cpp
    cl $(CPP_PROJ) exists.cpp

ufree\EXTRACT.obj : EXTRACT.CPP
    cl $(CPP_PROJ) EXTRACT.CPP

ufree\fifo.obj : fifo.cpp
    cl $(CPP_PROJ) fifo.cpp

ufree\fileclas.obj : fileclas.cpp
    cl $(CPP_PROJ) fileclas.cpp

ufree\FILLPART.obj : FILLPART.CPP
    cl $(CPP_PROJ) FILLPART.CPP

ufree\fillshot.obj : fillshot.cpp
    cl $(CPP_PROJ) fillshot.cpp

ufree\fix.obj : fix.cpp
    cl $(CPP_PROJ) fix.cpp

ufree\fontclas.obj : fontclas.cpp
    cl $(CPP_PROJ) fontclas.cpp

ufree\FTANALOG.obj : FTANALOG.CPP
    cl $(CPP_PROJ) FTANALOG.CPP

ufree\ftii_file.obj : ftii_file.cpp ..\include\ftii_file.h
    cl $(CPP_PROJ) ftii_file.cpp

ufree\ftii_parameters.obj : ftii_parameters.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h ..\include\ftanalog.h
    cl $(CPP_PROJ) ftii_parameters.cpp

ufree\ftii_shot_data.obj : ftii_shot_data.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h
    cl $(CPP_PROJ) ftii_shot_data.cpp

ufree\ftiiclas.obj : ftiiclas.cpp ..\include\ftii.h ..\include\ftii_file.h
    cl $(CPP_PROJ) ftiiclas.cpp

ufree\ftime.obj : ftime.cpp
    cl $(CPP_PROJ) ftime.cpp

ufree\FTOASC.obj : FTOASC.CPP
    cl $(CPP_PROJ) FTOASC.CPP

ufree\GENLIST.obj : GENLIST.CPP
    cl $(CPP_PROJ) GENLIST.CPP

ufree\GETSHOT.obj : GETSHOT.CPP
    cl $(CPP_PROJ) GETSHOT.CPP

ufree\GET_INI.obj : GET_INI.CPP
    cl $(CPP_PROJ) GET_INI.CPP

ufree\get_tc.obj : get_tc.cpp
    cl $(CPP_PROJ) get_tc.cpp

ufree\GEXEDIR.obj : GEXEDIR.CPP ..\include\stringcl.h
    cl $(CPP_PROJ) GEXEDIR.CPP

ufree\GPVAR.obj : GPVAR.CPP
    cl $(CPP_PROJ) GPVAR.CPP

ufree\hex.obj : hex.cpp
    cl $(CPP_PROJ) hex.cpp

ufree\hourglas.obj : hourglas.cpp
    cl $(CPP_PROJ) hourglas.cpp

ufree\iniclass.obj : iniclass.cpp ..\include\iniclass.h
    cl $(CPP_PROJ) iniclass.cpp

ufree\INSALPH.obj : INSALPH.CPP
    cl $(CPP_PROJ) INSALPH.CPP

ufree\IS_EMPTY.obj : IS_EMPTY.CPP
    cl $(CPP_PROJ) IS_EMPTY.CPP

ufree\ITOASC.obj : ITOASC.CPP
    cl $(CPP_PROJ) ITOASC.CPP

ufree\KILLDIR.obj : KILLDIR.CPP
    cl $(CPP_PROJ) KILLDIR.CPP

ufree\listbox.obj : listbox.cpp
    cl $(CPP_PROJ) listbox.cpp

ufree\log_file_string.obj : log_file_string.cpp
    cl $(CPP_PROJ) log_file_string.cpp

ufree\MACHCOMP.obj : MACHCOMP.CPP
    cl $(CPP_PROJ) MACHCOMP.CPP

ufree\MACHINE.obj : MACHINE.CPP
    cl $(CPP_PROJ) MACHINE.CPP

ufree\MACHNAME.obj : MACHNAME.CPP ..\include\machname.h
    cl $(CPP_PROJ) MACHNAME.CPP

ufree\MARKS.obj : MARKS.CPP
    cl $(CPP_PROJ) MARKS.CPP

ufree\MAXIMIZE.obj : MAXIMIZE.CPP
    cl $(CPP_PROJ) MAXIMIZE.CPP

ufree\MEM.obj : MEM.CPP
    cl $(CPP_PROJ) MEM.CPP

ufree\menu.obj : menu.cpp
    cl $(CPP_PROJ) menu.cpp

ufree\MEXISTS.obj : MEXISTS.CPP
    cl $(CPP_PROJ) MEXISTS.CPP

ufree\misc.obj : misc.cpp
    cl $(CPP_PROJ) misc.cpp

ufree\monrun.obj : monrun.cpp
    cl $(CPP_PROJ) monrun.cpp

ufree\msgclas.obj : msgclas.cpp
    cl $(CPP_PROJ) msgclas.cpp

ufree\namecopy.obj : namecopy.cpp
    cl $(CPP_PROJ) namecopy.cpp

ufree\nameclas.obj : nameclas.cpp ..\include\computer.h ..\include\stringcl.h ..\include\nameclas.h
    cl $(CPP_PROJ) nameclas.cpp

ufree\NAMES.obj : NAMES.CPP
    cl $(CPP_PROJ) NAMES.CPP

ufree\optocontrol.obj : optocontrol.cpp
    cl $(CPP_PROJ) optocontrol.cpp

ufree\param.obj : param.cpp
    cl $(CPP_PROJ) param.cpp

ufree\param_index_class.obj : param_index_class.cpp ..\include\param_index_class.h
    cl $(CPP_PROJ) param_index_class.cpp

ufree\PART.obj : PART.CPP
    cl $(CPP_PROJ) PART.CPP

ufree\part_analog_sensor.obj : part_analog_sensor.cpp ..\include\part_analog_sensor.h
    cl $(CPP_PROJ) part_analog_sensor.cpp

ufree\path.obj : path.cpp
    cl $(CPP_PROJ) path.cpp

ufree\PEXISTS.obj : PEXISTS.CPP
    cl $(CPP_PROJ) PEXISTS.CPP

ufree\plookup.obj : plookup.cpp
    cl $(CPP_PROJ) plookup.cpp

ufree\plotclas.obj : plotclas.cpp
    cl $(CPP_PROJ) plotclas.cpp

ufree\plotsubs.obj : plotsubs.cpp
    cl $(CPP_PROJ) plotsubs.cpp

ufree\pointcl.obj : pointcl.cpp ..\include\pointcl.h
    cl $(CPP_PROJ) pointcl.cpp

ufree\POSTSEC.obj : POSTSEC.CPP
    cl $(CPP_PROJ) POSTSEC.CPP

ufree\profiles.obj : profiles.cpp
    cl $(CPP_PROJ) profiles.cpp

ufree\pw.obj : pw.cpp
    cl $(CPP_PROJ) pw.cpp

ufree\READLINE.obj : READLINE.CPP
    cl $(CPP_PROJ) READLINE.CPP

ufree\rectclas.obj : rectclas.cpp  ..\include\rectclas.h
    cl $(CPP_PROJ) rectclas.cpp

ufree\REVERSE.obj : REVERSE.CPP
    cl $(CPP_PROJ) REVERSE.CPP

ufree\ringcl.obj : ringcl.cpp
    cl $(CPP_PROJ) ringcl.cpp

ufree\RJUST.obj : RJUST.CPP
    cl $(CPP_PROJ) RJUST.CPP

ufree\runlist.obj : runlist.cpp  ..\include\runlist.h
    cl $(CPP_PROJ) runlist.cpp

ufree\SEMAPHOR.obj : SEMAPHOR.CPP
    cl $(CPP_PROJ) SEMAPHOR.CPP

ufree\SENSOR.obj : SENSOR.CPP
    cl $(CPP_PROJ) SENSOR.CPP

ufree\setpoint.obj : setpoint.CPP ..\include\setpoint.h ..\include\strarray.h
    cl $(CPP_PROJ) setpoint.CPP

ufree\SHIFT.obj : SHIFT.CPP
    cl $(CPP_PROJ) SHIFT.CPP

ufree\shotclas.obj : shotclas.cpp
    cl $(CPP_PROJ) shotclas.cpp

ufree\sockcli.obj : sockcli.cpp
    cl $(CPP_PROJ) sockcli.cpp

ufree\startpar.obj : startpar.cpp
    cl $(CPP_PROJ) startpar.cpp

ufree\stddown.obj : stddown.cpp ..\include\stddown.h
    cl $(CPP_PROJ) stddown.cpp

ufree\stparam.obj : stparam.cpp
    cl $(CPP_PROJ) stparam.cpp

ufree\stpres.obj : stpres.cpp
    cl $(CPP_PROJ) stpres.cpp

ufree\strarray.obj : strarray.cpp ..\include\strarray.h
    cl $(CPP_PROJ) strarray.cpp

ufree\stringcl.obj : stringcl.cpp
    cl $(CPP_PROJ) stringcl.cpp

ufree\STRUCTS.obj : STRUCTS.CPP
    cl $(CPP_PROJ) STRUCTS.CPP

ufree\stsetup.obj : stsetup.cpp
    cl $(CPP_PROJ) stsetup.cpp

ufree\textlen.obj : textlen.cpp
    cl $(CPP_PROJ) textlen.cpp

ufree\textlist.obj : textlist.cpp
    cl $(CPP_PROJ) textlist.cpp

ufree\temp_message.obj : temp_message.cpp
    cl $(CPP_PROJ) temp_message.cpp

ufree\TIME.obj : TIME.CPP
    cl $(CPP_PROJ) TIME.CPP

ufree\timeclas.obj : timeclas.cpp
    cl $(CPP_PROJ) timeclas.cpp

ufree\UNITADJ.obj : UNITADJ.CPP
    cl $(CPP_PROJ) UNITADJ.CPP

ufree\UNITS.obj : UNITS.CPP
    cl $(CPP_PROJ) UNITS.CPP

ufree\updown.obj : updown.cpp
    cl $(CPP_PROJ) updown.cpp

ufree\visigrid.obj : visigrid.cpp ..\include\visigrid.h
    cl $(CPP_PROJ) visigrid.cpp

ufree\visiplot.obj : visiplot.cpp
    cl $(CPP_PROJ) visiplot.cpp

ufree\warmupcl.obj : warmupcl.cpp
    cl $(CPP_PROJ) warmupcl.cpp

ufree\wclass.obj : wclass.cpp
    cl $(CPP_PROJ) wclass.cpp

ufree\WINSUBS.obj : WINSUBS.CPP
    cl $(CPP_PROJ) WINSUBS.CPP

ufree\workclas.obj : workclas.cpp
    cl $(CPP_PROJ) workclas.cpp

ufree\writelin.obj : writelin.cpp
    cl $(CPP_PROJ) writelin.cpp

ufree\ZERO.obj : ZERO.CPP
    cl $(CPP_PROJ) ZERO.CPP

