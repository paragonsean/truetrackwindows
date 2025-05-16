#ifndef _NAME_CLASS_
#define _NAME_CLASS_

#ifndef _SUBS_H_
BOOLEAN create_directory( TCHAR * path );
BOOLEAN directory_exists( TCHAR * path );
BOOLEAN file_exists( TCHAR * path );
#endif

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

class NAME_CLASS : public STRING_CLASS {
public:
BOOLEAN get_root_dir_file_name( TCHAR * file_name );
BOOLEAN get_root_dir_file_name( TCHAR * computer_name, TCHAR * file_name );
BOOLEAN get_display_dir_file_name( TCHAR * computer, TCHAR * file_name );
BOOLEAN get_display_dir_file_name( TCHAR * computer, TCHAR * machine, TCHAR * file_name );
BOOLEAN get_computer_dir_file_name( TCHAR * computer_name, TCHAR * file_name );
BOOLEAN get_machine_dir_file_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * file_name );
BOOLEAN get_machine_list_file_name( TCHAR * co );
BOOLEAN get_part_dir_file_name( TCHAR * co, TCHAR * ma, TCHAR * pa, TCHAR * file_name );
BOOLEAN get_part_results_dir_file_name( TCHAR * co, TCHAR * ma, TCHAR * pa, TCHAR * file_name );
BOOLEAN get_profile_name( TCHAR * co, TCHAR * ma, TCHAR * pa, const TCHAR * shot_name, const TCHAR * suffix );
BOOLEAN get_profile_name( TCHAR * co, TCHAR * ma, TCHAR * pa, int32 shot_number, const TCHAR * suffix );
BOOLEAN get_machine_results_dir_file_name( TCHAR * computer_name, TCHAR * machine_name, TCHAR * file_name );
BOOLEAN get_exe_dir_file_name( TCHAR * file_name );
BOOLEAN get_exe_dir_file_name( TCHAR * computer_name, TCHAR * file_name );
BOOLEAN get_symphony_machine_directory( TCHAR * computer, TCHAR * machine );
BOOLEAN get_v5_directory( TCHAR * computer );

inline void operator=( const TCHAR * sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( const NAME_CLASS & sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( const STRING_CLASS & sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( int sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( unsigned int sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( double sorc ) { STRING_CLASS::operator=( sorc ); }

inline void operator+=( const TCHAR   sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( const TCHAR * sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( const NAME_CLASS & sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( const STRING_CLASS & sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( int sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( unsigned int sorc ) { STRING_CLASS::operator+=( sorc ); }

inline BOOLEAN operator==(const NAME_CLASS & sorc ) { return STRING_CLASS::operator==( sorc ); }
inline BOOLEAN operator==(const STRING_CLASS & sorc ) { return STRING_CLASS::operator==( sorc ); }
inline BOOLEAN operator==(const TCHAR * sorc ) { return STRING_CLASS::operator==( sorc ); }

inline BOOLEAN operator!=(const NAME_CLASS & sorc ) { return STRING_CLASS::operator!=( sorc ); }
inline BOOLEAN operator!=(const STRING_CLASS & sorc ) { return STRING_CLASS::operator!=( sorc ); }
inline BOOLEAN operator!=(const TCHAR * sorc ) { return STRING_CLASS::operator!=( sorc ); }

inline BOOLEAN create_directory() { return ::create_directory(text());  }
inline BOOLEAN delete_file() { if (::DeleteFile(text())) return TRUE; else return FALSE; }
inline BOOLEAN directory_exists() { return ::directory_exists(text());  }
inline BOOLEAN file_exists() { return ::file_exists(text());  }
inline BOOLEAN get_asensor_dbname() { return get_root_dir_file_name( ASENSOR_DB ); }
inline BOOLEAN get_auto_save_all_file_name() { return get_exe_dir_file_name(AUTO_SAVE_ALL_FILE);  }
inline BOOLEAN get_backup_list_dbname() { return get_exe_dir_file_name(BACKUP_LIST_DB); }
inline BOOLEAN get_ft2_boards_file_name( TCHAR * co ) { return get_computer_dir_file_name( co, FTII_BOARDS_DB );  }
inline BOOLEAN get_ft2_ctrl_prog_steps_file_name( TCHAR * co, TCHAR * ma ) { return get_machine_dir_file_name( co, ma, FTII_CTRL_PROG_STEPS_FILE );  }
inline BOOLEAN get_ft2_editor_settings_file_name( TCHAR * co, TCHAR * ma ) { return get_machine_dir_file_name( co, ma, FTII_EDITOR_SETTINGS_FILE );  }
inline BOOLEAN get_ft2_global_settings_file_name( TCHAR * co, TCHAR * ma ) { return get_machine_dir_file_name( co, ma, FTII_GLOBAL_SETTINGS_FILE );  }
inline BOOLEAN get_ft2_limit_switch_wires_file_name( TCHAR * co, TCHAR * ma, TCHAR * pa )  { return get_part_dir_file_name( co, ma, pa, FTII_LIMIT_SWITCH_WIRES_FILE );  }
inline BOOLEAN get_ft2_machine_settings_file_name( TCHAR * co, TCHAR * ma )  { return get_machine_dir_file_name( co, ma, FTII_MACHINE_SETTINGS_FILE );  }
inline BOOLEAN get_ft2_part_settings_file_name( TCHAR * co, TCHAR * ma, TCHAR * pa )  { return get_part_dir_file_name( co, ma, pa, FTII_PART_SETTINGS_FILE );  }
inline BOOLEAN get_ftchan_file_name(  TCHAR * co, TCHAR * ma, TCHAR * pa ) { return get_part_dir_file_name( co, ma, pa, FTCHAN_DB ); }
inline BOOLEAN get_good_shot_setup_file_name( TCHAR * co, TCHAR * ma, TCHAR * pa ) { return get_part_dir_file_name( co, ma, pa, GOOD_SHOT_SETUP_FILE ); }
inline BOOLEAN get_graphlst_dbname( TCHAR * co, TCHAR * ma, TCHAR * pa ) { return get_part_results_dir_file_name( co, ma, pa, GRAPHLST_DB ); }
inline BOOLEAN get_local_ini_file_name( TCHAR * file_name ) { return get_exe_dir_file_name( file_name );  }
inline BOOLEAN get_machset_file_name( TCHAR * co ) { return get_computer_dir_file_name( co, MACHSET_DB ); }
inline BOOLEAN get_parameter_sort_order_file_name( TCHAR * co, TCHAR * ma, TCHAR * pa ) { return get_part_dir_file_name( co, ma, pa, PARAMETER_SORT_ORDER_FILE ); }
inline BOOLEAN get_parts_file_name( TCHAR * co, TCHAR * ma ) { return get_machine_dir_file_name( co, ma,  PARTS_DB ); }
inline BOOLEAN get_part_display_ini_name( TCHAR * co, TCHAR * ma, TCHAR * pa ) { return get_part_dir_file_name( co, ma, pa, PART_DISPLAY_INI_FILE ); }
inline BOOLEAN get_profile_name( TCHAR * co, TCHAR * ma, TCHAR * pa, TCHAR * shot_name ) { return get_profile_name(    co, ma, pa, shot_name,   PROFILE_NAME_SUFFIX ); }
inline BOOLEAN get_profile_name( TCHAR * co, TCHAR * ma, TCHAR * pa, int32 shot_number ) { return get_profile_name(    co, ma, pa, shot_number, PROFILE_NAME_SUFFIX ); }
inline BOOLEAN get_profile_ii_name( TCHAR * co, TCHAR * ma, TCHAR * pa, TCHAR * shot_name ) { return get_profile_name( co, ma, pa, shot_name,   PROFILE_II_SUFFIX ); }
inline BOOLEAN get_profile_ii_name( TCHAR * co, TCHAR * ma, TCHAR * pa, int32 shot_number ) { return get_profile_name( co, ma, pa, shot_number, PROFILE_II_SUFFIX ); }
inline BOOLEAN get_psensor_file_name( TCHAR * co ) { return get_root_dir_file_name( co, PSENSOR_DB );   }
inline BOOLEAN get_stlimits_dbname( TCHAR * co, TCHAR * ma, TCHAR * pa )  { return get_part_dir_file_name( co, ma, pa, STLIMITS_DB );  }
inline BOOLEAN get_stlimits_datname( TCHAR * co, TCHAR * ma, TCHAR * pa ) { return get_part_dir_file_name( co, ma, pa, STLIMITS_DATA );  }
inline BOOLEAN get_stparam_file_name( TCHAR * co ) { return get_computer_dir_file_name( co, STPARAM_FILE ); }
inline BOOLEAN get_ststeps_dbname(  TCHAR * co, TCHAR * ma, TCHAR * pa )  { return get_part_dir_file_name( co, ma, pa, STSTEPS_DB);  }
inline BOOLEAN get_valve_test_master_file_name( TCHAR * co ) { return get_computer_dir_file_name( co, VTMASTER_FILE ); }
inline BOOLEAN get_valve_test_master_file_name( TCHAR * co, TCHAR * ma ) { return get_machine_dir_file_name( co, ma, VTMASTER_FILE ); }

};

#endif