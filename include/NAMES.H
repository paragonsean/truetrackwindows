TCHAR * ascii_shot_number( int32 shot_number );
TCHAR * codir_name( TCHAR * computer_name, TCHAR * db_name );
TCHAR * exedir_name( const TCHAR * file_name );
BOOLEAN get_root_directory( TCHAR * dest );
TCHAR * madir_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * db_name );
TCHAR * mrdir_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * db_name );
TCHAR * padir_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * db_name );
TCHAR * rsdir_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * db_name );
TCHAR * master_name(    TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * master_name );
TCHAR * master_ii_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * master_name );

#ifdef _STRING_CLASS_
BOOLEAN get_machine_directory_file_name( STRING_CLASS & dest, TCHAR * computer_name, TCHAR * machine_name, TCHAR * file_name );
BOOLEAN get_part_results_file_name( STRING_CLASS & dest, TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * file_name );
BOOLEAN get_profile_ii_name( STRING_CLASS & dest, TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * shot_name );
BOOLEAN get_profile_ii_name( STRING_CLASS & dest, TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, int32 shot_number );
#endif

BOOLEAN names_startup( void );
BOOLEAN names_startup( TCHAR * exe_dir );
const TCHAR * computer_root_directory( TCHAR * computer_name );
const TCHAR * root_directory( void );
TCHAR * rootdir_name( TCHAR * db_name );
TCHAR * profile_ii_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * shot_name );
TCHAR * profile_ii_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, int32 shot_number );
TCHAR * profile_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, TCHAR * shot_name );
TCHAR * profile_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, int32 shot_number );
inline TCHAR * tc_parameter_list_filename( void ) { return exedir_name( TC_PARAMETERS_FILENAME ); }
inline TCHAR * tc_parameter_list_filename( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name ) { return padir_name( computer_name, machine_name, part_name,  TC_PARAMETERS_FILENAME ); }
inline TCHAR * status_screen_config_filename() { return exedir_name( STATUS_SCREEN_CONFIG_FILE_NAME ); }

TCHAR * computer_directory_name( TCHAR * computer_name );
inline TCHAR * machine_directory_name( TCHAR * computer_name, TCHAR * machine_name ) { return madir_name( computer_name, machine_name, 0 ); }
inline TCHAR * part_directory_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name ) { return padir_name( computer_name, machine_name, part_name, 0 ); }
inline TCHAR * result_directory_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name ) { return rsdir_name( computer_name, machine_name, part_name, 0 ); }
inline TCHAR * ini_file_name( const TCHAR * file_name ) { return exedir_name( file_name ); }
inline TCHAR * multipart_runlist_labels_file() { return exedir_name( TEXT("multipart_runlist_labels.txt") ); }
inline TCHAR * backup_list_dbname() { return exedir_name(BACKUP_LIST_DB); }

inline TCHAR * asensor_dbname( void )   { return rootdir_name( ASENSOR_DB );   }
inline TCHAR * color_dbname( void )     { return rootdir_name( COLOR_DB );     }
inline TCHAR * computer_dbname( void )  { return rootdir_name( COMPUTER_DB );  }
inline TCHAR * computer_csvname( void ) { return rootdir_name( COMPUTER_CSV );  }
inline TCHAR * density_dbname( void )   { return rootdir_name( DENSITY_DB );   }
inline TCHAR * diffcurve_dbname( void ) { return rootdir_name( DCURVE_DB );    }
inline TCHAR * downcat_dbname( void )   { return rootdir_name( DOWNCAT_DB );   }
inline TCHAR * downscat_dbname( void )  { return rootdir_name( DOWNSCAT_DB );  }
inline TCHAR * downscatc_dbname( void ) { return rootdir_name( DOWNSCATC_DB );  }
inline TCHAR * operator_dbname( void )  { return rootdir_name( OPERATOR_DB );  }
inline TCHAR * parmname_dbname( void )  { return rootdir_name( PARMNAME_DB );  }
inline TCHAR * new_password_dbname( void ) { return rootdir_name( NEW_PASSWORD_DB );  }
inline TCHAR * password_dbname( void )  { return rootdir_name( PASSWORD_DB );  }
inline TCHAR * polyeq_dbname( void )    { return rootdir_name( POLYEQ_DB );    }
inline TCHAR * psensor_dbname( void )   { return rootdir_name( PSENSOR_DB );   }
inline TCHAR * rodpitch_dbname( void )  { return rootdir_name( RODPITCH_DB );  }
inline TCHAR * scrapcat_dbname( void )  { return rootdir_name( SCRAPCAT_DB );  }
inline TCHAR * stddown_dbname( void )   { return rootdir_name( STDDOWN_DB );   }
inline TCHAR * shift_dbname( void )     { return rootdir_name( SHIFT_DB    );  }
inline TCHAR * softvers_dbname( void )  { return rootdir_name( SOFTVERS_DB );  }
inline TCHAR * units_dbname( void )     { return rootdir_name( UNITS_DB );     }
inline TCHAR * vartype_dbname( void )   { return rootdir_name( VARTYPE_DB );   }

inline TCHAR * main_valve_types_name()  { return rootdir_name( TEXT("main_valve_types.txt" ) ); }
inline TCHAR * pilot_valve_types_name() { return rootdir_name( TEXT("pilot_valve_types.txt" ) ); }

inline TCHAR * boards_dbname( TCHAR * computer_name ) { return codir_name( computer_name, BOARDS_DB );  }
inline TCHAR * ftii_boards_dbname( TCHAR * computer_name ) { return codir_name( computer_name, FTII_BOARDS_DB );  }
inline TCHAR * control_message_name( TCHAR * computer_name ) { return codir_name( computer_name, CONTROL_MESSAGE_FILE );   }
inline TCHAR * control_program_name( TCHAR * computer_name ) { return codir_name( computer_name, CONTROL_PROGRAM_FILE );   }
inline TCHAR * machset_dbname( TCHAR * computer_name ) { return codir_name( computer_name, MACHSET_DB ); }
inline TCHAR * stparam_filename( TCHAR * computer_name ) { return codir_name( computer_name, STPARAM_FILE ); }
inline TCHAR * valve_test_master_filename( TCHAR * computer_name) { return codir_name( computer_name, VTMASTER_FILE ); }
inline TCHAR * valve_test_master_filename( TCHAR * computer_name, TCHAR * machine_name ) { return madir_name( computer_name, machine_name, VTMASTER_FILE );    }

inline TCHAR * parts_dbname( TCHAR * computer_name, TCHAR * machine_name )    { return madir_name( computer_name, machine_name, PARTS_DB );    }
inline TCHAR * plookup_dbname( TCHAR * computer_name, TCHAR * machine_name )  { return madir_name( computer_name, machine_name, PLOOKUP_DB );  }
inline TCHAR * dstat_dbname(   TCHAR * computer_name, TCHAR * machine_name )  { return madir_name( computer_name, machine_name, DSTAT_DB );  }
inline TCHAR * eos_udp_address_name( TCHAR * computer_name, TCHAR * machine_name )  { return madir_name( computer_name, machine_name, EOS_UDP_ADDRESS_FILE );  }
inline TCHAR * runlist_dbname(   TCHAR * computer_name, TCHAR * machine_name )  { return madir_name( computer_name, machine_name, RUNLIST_DB );  }
inline TCHAR * downtime_dbname(TCHAR * computer_name, TCHAR * machine_name )  { return mrdir_name( computer_name, machine_name, DOWNTIME_DB );  }
inline TCHAR * smooth_filename(TCHAR * computer_name, TCHAR * machine_name )  { return mrdir_name( computer_name, machine_name, SMOOTH_FILE );  }

inline TCHAR * calc_file_name(  TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name ) { return padir_name( computer_name, machine_name, part_name,  CALC_FILENAME   ); }
inline TCHAR * config_ini_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name ) { return padir_name( computer_name, machine_name, part_name,  CONFIG_INI_FILE ); }
inline TCHAR * ctrlimit_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name ) { return padir_name( computer_name, machine_name, part_name,  CTRLIMIT_DB ); }
inline TCHAR * parmlist_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name ) { return padir_name( computer_name, machine_name, part_name,  PARMLIST_DB ); }
inline TCHAR * ftanalog_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name ) { return padir_name( computer_name, machine_name, part_name,  FTANALOG_DB ); }
inline TCHAR * ftanalog_csvname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name ) { return padir_name( computer_name, machine_name, part_name,  FTANALOG_CSV ); }
inline TCHAR * ftchan_dbname(   TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )   { return padir_name( computer_name, machine_name, part_name,  FTCHAN_DB );   }
inline TCHAR * marklist_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, MARKLIST_DB );  }
inline TCHAR * part_display_ini_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, PART_DISPLAY_INI_FILE );  }

inline TCHAR * ftii_editor_settings_name( TCHAR * computer_name, TCHAR * machine_name )  { return madir_name( computer_name, machine_name, FTII_EDITOR_SETTINGS_FILE );  }
inline TCHAR * ftii_global_settings_name( TCHAR * computer_name, TCHAR * machine_name )  { return madir_name( computer_name, machine_name, FTII_GLOBAL_SETTINGS_FILE );  }
inline TCHAR * ftii_ctrl_prog_steps_name( TCHAR * computer_name, TCHAR * machine_name )  { return madir_name( computer_name, machine_name, FTII_CTRL_PROG_STEPS_FILE );  }
#ifdef _MACHINE_CLASS_
       TCHAR * ft2_control_program_name( int flash_version, MACHINE_CLASS & m );
#endif
inline TCHAR * ftii_limit_switch_wires_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, FTII_LIMIT_SWITCH_WIRES_FILE );  }
inline TCHAR * ftii_machine_settings_name( TCHAR * computer_name, TCHAR * machine_name )  { return madir_name( computer_name, machine_name, FTII_MACHINE_SETTINGS_FILE );  }
inline TCHAR * ftii_part_settings_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, FTII_PART_SETTINGS_FILE );  }
inline TCHAR * default_ftii_control_message_name() { return exedir_name( CONTROL_MESSAGE_FILE );  }

inline TCHAR * chaxis_label_datname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, CHAXIS_LABEL_DATA_FILE );  }
inline TCHAR * presure_control_datname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, STPRES_DATA );  }
inline TCHAR * setpoint_name(  TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, SETPOINT_CSV_FILE );  }
inline TCHAR * stpart_datname(  TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, STPART_DATA );  }
inline TCHAR * setup_sheet_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, SETUP_SHEET_FILE );  }

inline TCHAR * ststeps_dbname(  TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, STSTEPS_DB  );  }
inline TCHAR * stlimits_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, STLIMITS_DB );  }
inline TCHAR * stlimits_datname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, STLIMITS_DATA );  }
inline TCHAR * gpvar_dbname(    TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, GPVAR_DB    );  }
inline TCHAR * subparts_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, SUBPARTS_DB );  }
inline TCHAR * warmup_dbname(   TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return padir_name( computer_name, machine_name, part_name, WARMUP_DB );    }

inline TCHAR * alarmsum_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return rsdir_name( computer_name, machine_name, part_name, ALARMSUM_DB );  }
inline TCHAR * graphlst_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return rsdir_name( computer_name, machine_name, part_name, GRAPHLST_DB );  }
inline TCHAR * shotparm_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return rsdir_name( computer_name, machine_name, part_name, SHOTPARM_DB );  }

inline TCHAR * temp_computer_dbname( void )  { return rootdir_name( TEMP_COMPUTER_DB );  }
inline TCHAR * temp_graphlst_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return rsdir_name( computer_name, machine_name, part_name, TEMP_GRAPHLST_DB );  }
inline TCHAR * temp_shotparm_dbname( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )  { return rsdir_name( computer_name, machine_name, part_name, TEMP_SHOTPARM_DB );  }
inline TCHAR * temp_control_program_name( TCHAR * computer_name ) { return codir_name( computer_name, TEMP_CONTROL_PROGRAM_FILE );   }

