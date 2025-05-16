#ifndef _SUBS_H_
#define _SUBS_H_

/*
-----------------------
Args for copy_all_files
----------------------- */
#define WITHOUT_SUBS         FALSE
#define WITHOUT_COMPRESSION  FALSE

inline float absolute_value( float x )
{
if ( x < 0.0 )
    x = -x;
return x;
}

inline int absolute_value( int x )
{
if ( x < 0.0 )
    x = -x;
return x;
}

inline void swap( float & x, float & y )
{
float z;
z = y;
y = x;
x = z;
}

inline void swap( int & x, int & y )
{
int z;
z = y;
y = x;
x = z;
}

double    adjust_for_units( short dest_units_id, double x, short sorc_units_id );
float     adjust_for_units( short dest_units_id, float sorc, short sorc_units_id );
TCHAR   * alphadate( const SYSTEMTIME & sorc );
TCHAR   * alphatime( const SYSTEMTIME & sorc );
TCHAR *   ascii_double( double sorc );
TCHAR   * ascii_float( float sorc );
BOOLEAN   ascii_to_boolean( TCHAR * cp );
int32     asctoint32( const TCHAR * sorc );
unsigned long asctoul( const TCHAR * sorc );
bool      bool_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
void      bool_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, bool b );
BOOLEAN   boolean_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
TCHAR   * boolean_to_ascii( BOOLEAN b );
void      boolean_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, BOOLEAN b );
TCHAR   * category_name( TCHAR * number_to_match );
TCHAR   * catstring( TCHAR * dest, const TCHAR * sorc);
void      char_to_tchar( TCHAR * dest, char * sorc, int max_len );
short     charlen( const TCHAR * sorc, short max_len );
short     compare( const TCHAR * s1, const TCHAR * s2 );
short     compare( const TCHAR * s1, const TCHAR * s2, short max_len );
int       compare_text( const void * sorc1, const void * sorc2 );
#ifdef _STRUCTS_
float     convert_analog( BOARD_DATA raw_value, ANALOG_SENSOR_SETUP_DATA * ap );
#endif
void      copychars(   LPTSTR dest, const TCHAR * sorc, short n );
void      copyfromstr( LPTSTR dest, const TCHAR * sorc, short n );
TCHAR   * copystring( TCHAR * dest, const TCHAR * sorc );
TCHAR   * copystring( TCHAR * dest, const TCHAR * sorc, int maxlen );
BOOLEAN   copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, BOOLEAN with_subs, BOOLEAN with_compression, HWND w, int id );
BOOLEAN   copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, TCHAR * wildname, HWND w, int id );
TCHAR   * copy_w_char( TCHAR * dest, TCHAR * sorc, TCHAR c );
int32     countchars( TCHAR c, const TCHAR * sorc );
BOOLEAN   create_directory( TCHAR * path );
TCHAR   * current_part_name( TCHAR * computer, TCHAR * machine );
#ifdef _STRUCTS_
int32     current_shot_number( PART_NAME_ENTRY & p );
#endif
void      get_current_password_level( void );
long      current_sec_since1990( void );
int       day_of_the_week( int m, int d, int y );
WORD      days_in_month( WORD month, WORD year );
WORD      days_in_month( SYSTEMTIME & st );
void      dcurve_shutdown();
BOOLEAN   dcurve_startup();

#ifdef _STRUCTS_
ANALOG_SENSOR_SETUP_DATA * default_analog_sensor( void );
#endif

BOOLEAN   directory_exists( TCHAR * path );
WORD      dofyr( WORD month, WORD day, WORD year );
WORD      dofyr( const SYSTEMTIME & st );
short     dow_from_string( TCHAR * day );

bool      double_to_tchar( TCHAR * dest, double x, int max_decimal_places );

double    dups_to_vu( short velocity_units_id, short distance_units_id );
#ifdef _STRUCTS_
#ifdef _TIME_CLASS_
TCHAR *   dynamic_file_name( TIME_CLASS & tsorc, PART_NAME_ENTRY & pn, const TCHAR * sorc );
#endif
#endif
BOOLEAN   empty_directory( TCHAR * dirname );
BOOLEAN   empty_directory( TCHAR * dirname, TCHAR * fname );
void      error_message( short routine_number, short msg_number );
void      error_message( short routine_number, short msg_number, short n );
void      error_message( short routine_number, short msg_number, TCHAR * s );
void      error_message( TCHAR * s,            short msg_number );
void      error_message( TCHAR * s );

BOOLEAN   execute( TCHAR * command_line );
BOOLEAN   execute_and_wait( TCHAR * command_line, TCHAR * current_directory, DWORD ms_to_wait );
BOOLEAN   execute_and_wait( TCHAR * command_line, TCHAR * current_directory );
const TCHAR * exe_directory( void );

double    extdouble( const TCHAR * sorc, short nof_chars );
double    extdouble( const TCHAR * sorc );
TCHAR   * extfield( TCHAR * dest, TCHAR * sorc, TCHAR delimiter, int32 maxlen );
float     extfloat(  const TCHAR * sorc, short nof_chars );
float     extfloat( const TCHAR * sorc );
#ifdef _VISIPARM_H
BOOLEAN   exthms( SYSTEMTIME & t, const TCHAR * s );
#endif
long      extlong( const TCHAR * sorc, short nof_chars );
long      extlong( const TCHAR * sorc, short nof_chars, int radix );
#ifdef _VISIPARM_H
BOOLEAN   extmdy( SYSTEMTIME & t, const TCHAR * s );
BOOLEAN   extmmddyy( SYSTEMTIME & t, const TCHAR * s );
#endif
short     extshort( const TCHAR * sorc, short nof_chars );
BOOLEAN   file_exists( TCHAR * path );
int32     file_size( TCHAR * path );
BOOLEAN   get_file_write_time( SYSTEMTIME & t, TCHAR * fname );
BOOLEAN   get_shotsave_semaphore( TCHAR * computer, TCHAR * machine );
BOOLEAN   get_startup_part( TCHAR * machine, TCHAR * part );
#ifdef _TIME_CLASS_
#ifdef _DB_CLASS_
BOOLEAN   get_time_class_from_db( TIME_CLASS & tc, DB_TABLE & db, FIELDOFFSET offset );
#endif
#endif
void      fillchars( TCHAR * dest, TCHAR c, short n );
TCHAR   * findchar( TCHAR c, TCHAR * sorc, short maxlen );
TCHAR   * findchar( TCHAR c, TCHAR * sorc );
const TCHAR * findchar( TCHAR c, const TCHAR * sorc );
const TCHAR * findchar( TCHAR c, const TCHAR * sorc, short maxlen );
TCHAR   * find_last_char( TCHAR c, TCHAR * sorc );
TCHAR   * findstring( const TCHAR * s_to_find, TCHAR * s_to_search );
void      fill_part_listbox( HWND w, int box, TCHAR * computer, TCHAR * machine );
void      fill_parts_and_counts_listbox( HWND w, int box, TCHAR * computer, TCHAR * machine );
void      fill_parts_and_counts_listbox( HWND w, int box, TCHAR * computer, TCHAR * machine, TCHAR separator );
void      fill_parts_and_shot_count_listbox( HWND w, int box, TCHAR * computer, TCHAR * machine );
void      fill_shot_listbox( HWND w, int listbox_id, TCHAR * computer, TCHAR * machine, TCHAR * part );
void      fix_alphatime( TCHAR * sorc );
void      fix_dde_name( TCHAR * s );
void      fix_shotname( TCHAR * s );
TCHAR   * fixed_string( double x, int32 left_places, int32 right_places );
TCHAR   * fixed_string( double x, int32 left_places, int32 right_places );
float     float_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
void      float_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, float x );
double    double_from_ini( const TCHAR * fname, const TCHAR * section, TCHAR * key );
void      double_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, double x );

#ifdef _STRUCTS_
ANALOG_SENSOR_SETUP_DATA * find_analog_sensor( short analog_number );
#endif
int32     free_kilobytes( TCHAR * root );
void      free_shotsave_semaphore( void );
void      ftoasc( TCHAR * dest, double x );
void      ftoasc( TCHAR * dest, float x  );
void      getday( TCHAR * dest, short day );
BOOLEAN   get_dcurve_coefficients( float & dest_head_coef, float & dest_rod_coef, short curve_number );
BOOLEAN   get_dcurve_coefficients( double & dest_head_coef, double & dest_rod_coef, int curve_number );
BOOLEAN   get_exe_directory( TCHAR * dest );
TCHAR   * get_ini_string( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
TCHAR   * get_long_ini_string( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
void      put_ini_string( const TCHAR * fname, const TCHAR * section, const TCHAR * key, const TCHAR * sorc );
unsigned long hextoul( const TCHAR * sorc );
TCHAR   * hhmm( int32 hours, int32 minutes );
void      init_systemtime_struct( SYSTEMTIME & t );
void      insalph( LPTSTR dest, int32 n,  int32 slen, TCHAR c, int32 radix );
void      insalph( LPTSTR dest, double x, int32 slen, TCHAR c );
void      insalph( LPTSTR dest, float x,  int32 slen, TCHAR c );
void      insert_char( TCHAR * sorc, TCHAR c );
void      int32toasc(  LPTSTR dest, int32 n, int32 radix );
TCHAR   * int32toasc( int32 n );
int32     int32_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
void      int32_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, int32 i );
BOOLEAN   is_numeric( TCHAR c );
BOOLEAN   is_hex_char( TCHAR c );
BOOLEAN   is_real_char( TCHAR c );
BOOLEAN   is_removable( TCHAR * path );
BOOLEAN   is_time( TCHAR * s );
BOOLEAN   kill_directory( TCHAR * dirname );
void      killtext( TCHAR * s );
void      lower_case( TCHAR & c );
void      lower_case( TCHAR * s );
#ifdef _STRING_CLASS_
BOOLEAN   log_file_string( STRING_CLASS & dest, TCHAR * format, TCHAR * owner, TCHAR * machine, TCHAR * old_part, TCHAR * new_part );
#endif
TCHAR   * maketext( int32 n );
TCHAR   * maketext( const TCHAR * sorc, int32 chars_to_add );
TCHAR   * maketext( const TCHAR * sorc );
TCHAR   * maketext_from_char( char * sorc );
TCHAR   * maketext_from_char( char * sorc, int len );
TCHAR   * machine_computer_name( TCHAR * machine_name );
BOOLEAN   machine_exists( TCHAR * computer_to_find, TCHAR * name_to_find );
void      matchcase( TCHAR * dest, const TCHAR * sorc );
BOOLEAN   monitor_is_running( void );
BOOLEAN   part_exists(    TCHAR * computer_to_find, TCHAR * machine_to_find, TCHAR * name_to_find );
#ifdef _STRUCTS_
BOOLEAN   part_exists( PART_NAME_ENTRY & p );
#endif

TCHAR   * make_downtime_event_string( TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat, TCHAR * worker );

#ifdef _DB_CLASS_
BOOLEAN   make_downtime_record( DB_TABLE & t, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat, TCHAR * op, TCHAR * part );
#endif

void      maxdouble( double & x, const double y );
void      maxfloat( float & x, const float y );
void      maxint(     int & x, const   int y );
void      maxunsigned( unsigned & x, const unsigned y );
void      maxlong(   long & x, const  long y );
void      maxshort( short & x, const short y );

void      mindouble( double & x, const double y );
void      minfloat( float & x, const float y );
void      minint(     int & x, const   int y );
void      minunsigned( unsigned & x, const unsigned y );
void      minlong(   long & x, const  long y );
void      minshort( short & x, const short y );

#ifdef _VISIPARM_H
void      maxint32( int32 & x, const int32 y );
void      minint32( int32 & x, const int32 y );
#endif

void      move_string_forward( TCHAR * s, int n );
TCHAR   * nextstring( TCHAR * s );

BOOLEAN   is_empty( const TCHAR * s );
bool      is_zero( double x );
BOOLEAN   is_float_zero( float x );
BOOLEAN   not_a_part( TCHAR * pa );
BOOLEAN   not_float_zero( float x );
bool      not_zero( double x );
bool      double_are_equal( double x1, double x2 );

BOOLEAN   parmdata_startup( void );
#ifdef _VISIPARM_H
BITSETYPE parmdata_type( short parm_number );
#endif
TCHAR   * part_name_copy( TCHAR * sorc );
short     password_level( TCHAR * pw );
void      pixel_equation( float & a, float & b, int32 min_pixel, int32 max_pixel, float min_value, float max_value );
void      plot_data( HDC dc, RECT & r, float min_x, float max_x, float min_y, float max_y, float * xp, float * yp, int32 n );
void      post_impact_seconds( TCHAR * dest, short n, short ms_per_sample );
void      post_impact_n_ms( short * dest_n, short * dest_ms, TCHAR * sorc );
short     post_impact_n( short true_ms, TCHAR * sorc );
short     readline( TCHAR * dest, HANDLE fh, short buflen );
void      remove_char( TCHAR * sorc );
BOOLEAN   replace_char_with_null( TCHAR * cp, TCHAR c );
BOOLEAN   replace_char_with_null( TCHAR * cp, const TCHAR * cset );
BOOLEAN   replace_tab_with_null( TCHAR * cp );
void      reverse( LPTSTR s );
void      rjust( LPTSTR s, short n, TCHAR c );
double    round( double sorc, double smallest_increment );
bool      rounded_double_to_tchar( TCHAR * dest, double x );
BOOLEAN   samedate( const SYSTEMTIME & s1, const SYSTEMTIME & s2 );
int32     seconds_difference( SYSTEMTIME & t1, SYSTEMTIME & t2 );
int32     seconds_difference( FILETIME & t1,   FILETIME & t2 );
float     sec_per_vel_unit( short uid );
long      sec_since1990( SYSTEMTIME & t );
BOOLEAN   sensor_list_startup( void );
void      sensor_list_shutdown( void );
BOOLEAN   set_current_part_name( TCHAR * co, TCHAR * ma, TCHAR * pa );

#ifdef _STRUCTS_
BOOLEAN   set_current_part_name( PART_NAME_ENTRY & p );
BOOLEAN   set_last_shot_number( PART_NAME_ENTRY & p, int32 new_shot_number );
#endif
BOOLEAN   set_last_shot_number( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, int32 new_shot_number );

void      get_shift_number_and_date( short * shift, SYSTEMTIME * shift_date, const SYSTEMTIME & true_date );
short     shift_number( short * shift_dow, const SYSTEMTIME & now );
void      shifts_shutdown( void );
BOOLEAN   shifts_startup( void );
SYSTEMTIME & start_of_shift( const SYSTEMTIME & now );

int32     max_parameters_saved( TCHAR * machine );
int32     max_profiles_saved( TCHAR * machine );

#ifdef _VISIPARM_H
BITSETYPE skip_new_shot_notify( TCHAR * machine, FILETIME time_of_shot );
#endif
BOOLEAN   skip_reset_machine( TCHAR * machine );
void      skip_shutdown( void );
BOOLEAN   skip_startup( void );
BOOLEAN   strings_are_equal( const TCHAR * s1, const TCHAR * s2, short max_len );
BOOLEAN   strings_are_equal( const TCHAR * s1, const TCHAR * s2 );
void      strip( TCHAR * s );
TCHAR   * subcategory_name( TCHAR * cat_to_match, TCHAR * subcat_to_match );
void      trim( TCHAR * s );

void      ultoascii( TCHAR * dest, unsigned long n, unsigned long radix );
TCHAR   * ultoascii( unsigned long n, unsigned long radix );
TCHAR   * ultoascii( unsigned long n );
void      ultohex( TCHAR * dest, unsigned long n );
TCHAR   * ultohex( unsigned long n );
char      ul_to_char( unsigned long sorc );
void      ul_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, unsigned long i );
unsigned long ul_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );

char      unicode_to_char( TCHAR sorc );
void      unicode_to_char( char * dest, TCHAR * sorc );

void      units_shutdown( void );
#ifdef _VISIPARM_H
BOOLEAN   units_startup( void );
#endif
TCHAR *   units_name( short uid );
short     units_id( TCHAR * sorc );
#ifdef _VISIPARM_H
BITSETYPE units_type( short uid );
#endif

BOOLEAN   unknown( TCHAR * cp );
TCHAR   * unknown_string();
void      upper_case( TCHAR & c );
void      upper_case( TCHAR * s );
BOOLEAN   waitfor_shotsave_semaphor( TCHAR * computer, TCHAR * machine );
int32     whole_places( float x );
unsigned  mask_from_wire( unsigned wire );
unsigned  wire_from_mask( unsigned mask );
BOOL      writeline( HANDLE fh, TCHAR * sorc );

/*
-------------
Path routines
------------- */
void    append_backslash_to_path( TCHAR * s );
void    append_directory_to_path( TCHAR * dest, const TCHAR * sorc );
void    append_filename_to_path(  TCHAR * dest, const TCHAR * sorc );
TCHAR * catpath( TCHAR * dest, const TCHAR * sorc );
void    remove_backslash_from_path( TCHAR * s );
BOOLEAN drive_exists( TCHAR * path );
BOOLEAN disk_is_in_drive( TCHAR * path );
BOOLEAN path_is_drive_only( TCHAR * s );
BOOLEAN dir_from_filename( TCHAR * s );
TCHAR * filename_from_path( TCHAR * path );

/*
----------------
Profile routines
---------------- */
BOOLEAN change_shot_number_in_profile( TCHAR * fname, int32 new_shot_number );
#ifdef _STRUCTS_
BOOLEAN copy_profile( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc, int32 shot_number, int32 shot_increment );
BOOLEAN copy_profile( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc, int32 shot_number, int32 shot_increment, BOOLEAN need_to_check_for_symphony );
#endif
short   read_parameters_from_profile(  float * parameters, TCHAR * fname );
int32   read_parameters_from_parmlist( float * dest, TCHAR * filename );

/*
--------------
Copypart flags
-------------- */
#define COPY_PART_PARAMETERS      0x0001
#define COPY_PART_CHANNELS        0x0002
#define COPY_PART_MARKS           0x0004
#define COPY_SUBPART_NAMES        0x0008
#define COPY_PART_BASIC           0x0010
#define COPY_PART_ADVANCED        0x0020
#define COPY_PART_USER_POSITIONS  0x0040
#define COPY_PART_REFERENCE_TRACE 0x0080
#define COPY_PART_SURETRAK_SETUP  0x0100
#define COPY_PART_PARAM_NAMES     0x0200
#define COPY_PART_WARMUP_PART     0x0400
#define COPY_PART_DISPLAY_SETUP   0x0800
#define COPY_PART_CHAXIS_LABEL    0x1000
#define COPY_PART_PARAM_SORT      0x2000
#define COPY_PART_PRES_CONTROL    0x4000
#define COPY_PART_ALL             0x7FFF

#ifdef _VISIPARM_H
BOOLEAN copypart( TCHAR * dest_computer, TCHAR * dest_machine, TCHAR * dest_part, TCHAR * sorc_computer, TCHAR * sorc_machine, TCHAR * sorc_part, BITSETYPE flags );
#endif

#ifdef _MACHINE_CLASS_
BOOLEAN copymachine( TCHAR * computer, TCHAR * machine, MACHINE_CLASS & sorc );
#endif

/*
----------------
Subs for windows
---------------- */
int32   average_character_width( HDC dc );
int32   character_height( HDC dc );
HFONT   create_font( HDC dc, long width, long height, TCHAR * name );
RECT    desktop_client_rect( void );
void    draw_rectangle( HDC dc, RECT & r, COLORREF color );

void    eb_select_all( HWND w );
void    eb_select_all( HWND w, int id );

void    fill_rectangle( HDC dc, RECT & r, COLORREF color );

BOOLEAN get_cb_select( HWND w, TCHAR * dest );
BOOLEAN get_cb_select( HWND w, int box, TCHAR * dest );

short   get_lb_count( HWND w );
short   get_lb_count( HWND w, int box );

BOOLEAN get_lb_item_text( HWND w, TCHAR * dest, int i, int max_len );
BOOLEAN get_lb_item_text( HWND w, int box, TCHAR * dest, int i, int max_len );

BOOLEAN get_lb_select( HWND w, TCHAR * dest );
BOOLEAN get_lb_select( HWND w, int box, TCHAR * dest );

short   get_lb_select_count( HWND w );
short   get_lb_select_count( HWND w, int box );

short   get_lb_select_list( INT ** dest, HWND w );
short   get_lb_select_list( INT ** dest, HWND w, int box );

int32   pixel_width( HDC dc, TCHAR * s );
int32   pixel_width( HDC dc, TCHAR * s, int32 nof_chars );

void    hourglass_cursor( void );

void    lb_select_all( HWND w );
void    lb_select_all( HWND w, int box );

BOOLEAN get_text( TCHAR * dest, HWND w, int max_len );
BOOLEAN get_text( TCHAR * dest, HWND w, int box, int max_len );

double get_double_from_ebox( HWND w );
double get_double_from_ebox( HWND w, int box );

void    limit_text( HWND w, int nof_chars );
void    limit_text( HWND w, int box, int nof_chars );

void    restore_cursor( void );

BOOLEAN set_text( HWND w, const TCHAR * sorc );
BOOLEAN set_text( HWND w, int box, const TCHAR * sorc );

BOOLEAN set_current_cb_item( HWND w, TCHAR * item_to_find );
BOOLEAN set_current_cb_item( HWND w, int box, TCHAR * item_to_find );

BOOLEAN set_current_lb_item( HWND w, TCHAR * item_to_find );
BOOLEAN set_current_lb_item( HWND w, int box, TCHAR * item_to_find );

BOOLEAN is_a_listbox( HWND w );
BOOLEAN is_an_editbox( HWND w );
BOOLEAN is_checked( HWND w );
BOOLEAN is_checked( HWND w, int button_id );
BOOLEAN is_previous_instance( TCHAR * class_name, TCHAR * window_title );
BOOLEAN is_windows_nt( void );
void    add_to_menu( UINT id, TCHAR * name );
BOOL    load_menu( HWND w, TCHAR * menuname, short current_password_level, const UINT * bad_ids, int nof_bad_ids );
BOOL    load_menu( HWND w, TCHAR * menuname, short current_password_level );
void    modify_menu_string( UINT id, TCHAR * newname );
void    remove_from_menu( UINT id );
int     resource_message_box( HWND w, HINSTANCE instance, UINT msg_id, UINT title_id, UINT boxtype );
int     resource_message_box( HINSTANCE instance, UINT msg_id, UINT title_id, UINT boxtype );
void    resource_message_box( HINSTANCE instance, UINT msg_id, UINT title_id );
TCHAR * resource_string( HINSTANCE instance, UINT resource_id );
BOOLEAN is_changed( HWND w, int id ); /* Edit Box */
BOOLEAN is_changed( HWND w );
void    set_checkbox( HWND w, BOOLEAN need_check );
void    set_checkbox( HWND w, int id, BOOLEAN need_check );
void    set_listbox_tabs_from_title_positions( HWND parent_window, UINT listbox_id, int * id, int n );
void    show_window( HWND w, BOOLEAN is_visible );
void    show_window( HWND w, int id, BOOLEAN is_visible );
void    shrinkwrap( HWND main_window, HWND dialog_window );
void    set_menu_view_state( BOOLEAN & state, HWND w, UINT menu_choice_id );
void    toggle_menu_view_state( BOOLEAN & state, HWND w, UINT menu_choice_id );

void center_temp_message();
void kill_temp_message();
void temp_message( TCHAR * message, int ms );
void temp_message( TCHAR * message );

#endif

