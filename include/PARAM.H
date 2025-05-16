#ifndef _VISI_PARAM_H_

#define _VISI_PARAM_H_

/*
---------------------
Shot goodness returns
--------------------- */
#define GOOD_SHOT    0
#define WARNING_SHOT 1
#define ALARM_SHOT   2

#define NO_PARAMETER_NAME TEXT("NOPARAMETER")

struct WIRE_ENTRY
    {
    float value;
    short wire_number;
    };

struct INPUT_ENTRY
    {
    short type;
    short number;
    };

struct PARAMETER_ENTRY
    {
    TCHAR       name[PARAMETER_NAME_LEN+1];
    INPUT_ENTRY input;
    BITSETYPE   vartype;
    short       units;
    WIRE_ENTRY  limits[NOF_ALARM_LIMIT_TYPES];
    void        operator=( const PARAMETER_ENTRY & sorc );
    BOOLEAN     has_warning_limits( void );
    BOOLEAN     has_alarm_limits( void );
    BOOLEAN     has_any_limits( void ) { if ( has_warning_limits() || has_alarm_limits() ) return TRUE; else return FALSE; }
    BOOLEAN     is_alarm( float value );
    BOOLEAN     is_warning( float value );
    };

class PARAMETER_CLASS
    {
    public:

    TCHAR           computer[COMPUTER_NAME_LEN+1];
    TCHAR           machine[MACHINE_NAME_LEN+1];
    TCHAR           part[PART_NAME_LEN+1];
    PARAMETER_ENTRY parameter[MAX_PARMS];

    PARAMETER_CLASS( void );
    short      count( void );
    void       init( short i );
    BOOLEAN    remove( short i );
    BOOLEAN    find( TCHAR * computer, TCHAR * machine, TCHAR * part );
    BOOLEAN    read_param_db( TCHAR * filename );
    BOOLEAN    save( void );
    int        shot_goodness( DB_TABLE & db );
    };

#endif

