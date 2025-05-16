#ifndef _MACHINE_CLASS_
#define _MACHINE_CLASS_

#ifndef _DB_CLASS_
#include "..\include\dbclass.h"
#endif

#define PRE_IMPACT_INDEX    0
#define POST_IMPACT_INDEX   1
#define MAX_SHOT_REGION     2

struct SAVE_COUNTS
    {
    int32     skip_count;
    int32     acquire_count;
    int32     max_saved;
    short     skip_seconds;
    BITSETYPE flags;
    };

struct SMOOTH_ENTRY
    {
    double n;
    double factor;
    SMOOTH_ENTRY() { n = 0.0; factor=0.0; }
    };

class MACHINE_CLASS
    {
    private:

    BOOLEAN MACHINE_CLASS::open_readonly( DB_TABLE & t, TCHAR * tname );

    public:

    TCHAR         name[MACHINE_NAME_LEN+1];
    TCHAR         computer[COMPUTER_NAME_LEN+1];
    TCHAR         current_part[PART_NAME_LEN+1];
    TCHAR         rodpitch[RODPITCH_NAME_LEN+1];
    TCHAR         opnumber[OPERATOR_NUMBER_LEN+1];
    BITSETYPE     type;
    short         ft_board_number;
    short         muxchan;
    BOOLEAN       is_ftii;
    BOOLEAN       suretrak_controlled;
    BOOLEAN       is_cyclone;
    SAVE_COUNTS   profile;
    SAVE_COUNTS   params;
    BITSETYPE     monitor_flags;
    short         autoshot_timeout_seconds;
    short         cycle_timeout_seconds;
    short         diff_curve_number;
    BITSETYPE     impact_pres_type[MAX_SHOT_REGION];
    short         impact_points[MAX_SHOT_REGION];
    SMOOTH_ENTRY  smooth;

    MACHINE_CLASS();
    void       init();
    BOOLEAN    isempty();
    BOOLEAN    find( TCHAR * computer, TCHAR * name );
    void       operator=( const MACHINE_CLASS & sorc );
    BOOLEAN    read_machset_db( TCHAR * fname );
    BOOLEAN    refresh_monitor_flags( void );
    BOOLEAN    save();
    };

#endif
