#ifndef _SURETRAK_SETUP_CLASS_
#define _SURETRAK_SETUP_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif


const int32 MAX_ST_STEPS     = 6;
const int32 NOF_GAIN_BREAKS  = 3;

const int32 VELOCITY_LOOP_GAIN_INDEX = 0;
const int32 FT_STOP_POS_INDEX        = 1;
const int32 NOF_PART_PARAMS          = 2;

const int32 MIN_STROKE_LEN_INDEX     = 0;
const int32 VELOCITY_SETPOINT_INDEX  = 1;
const int32 PRESSURE_SETPOINT_INDEX  = 2;
const int32 NOF_PART_DATA            = 3;

struct SURETRAK_STEP_ENTRY
    {
    TCHAR accel[STSTEPS_FLOAT_LEN+1];
    TCHAR vel[STSTEPS_FLOAT_LEN+1];
    TCHAR end_pos[STSTEPS_FLOAT_LEN+1];
    TCHAR low_impact[STSTEPS_FLOAT_LEN+1];
    BOOLEAN vacuum_wait;
    BOOLEAN low_impact_from_input;
    BOOLEAN accel_percent;
    BOOLEAN vel_percent;
    };

struct GAIN_BREAK_ENTRY
    {
    float velocity;
    float gain;
    };

class SURETRAK_SETUP_CLASS
    {
    private:

    BOOLEAN             is_rewound;
    int32               current_index;
    int32               n;
    int32               pressure_sensor_number;
    SURETRAK_STEP_ENTRY step[MAX_ST_STEPS];
    TCHAR               limit_switch_pos[MAX_FTII_LIMIT_SWITCHES][STLIMITS_FLOAT_LEN];
    GAIN_BREAK_ENTRY    gainbreak[NOF_GAIN_BREAKS];
    float               fdata[NOF_PART_PARAMS];
    STRING_CLASS        partdata[NOF_PART_DATA];

    public:

    SURETRAK_SETUP_CLASS();
    ~SURETRAK_SETUP_CLASS();
    void    empty( void );
    void    clear_steps( void );
    BOOLEAN find( PART_NAME_ENTRY & p );
    BOOLEAN find( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name );
    BOOLEAN save( PART_NAME_ENTRY & p );
    BOOLEAN save( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name );
    BOOLEAN append( void );

    void    rewind( void ) { current_index = 0; is_rewound = TRUE; }
    BOOLEAN next( void );

    int32   nof_steps( void ) { return n; }
    int32   get_pressure_sensor_number( void ) { return pressure_sensor_number; }

    TCHAR * accel_string( void )      { return step[current_index].accel; }
    TCHAR * vel_string( void )        { return step[current_index].vel;   }
    TCHAR * end_pos_string( void )    { return step[current_index].end_pos; }
    TCHAR * low_impact_string( void ) { return step[current_index].low_impact; }
    TCHAR * limit_switch_pos_string( int32 i );
    TCHAR * min_stroke_len_string( void )    { return partdata[MIN_STROKE_LEN_INDEX].text();    }
    TCHAR * pressure_setpoint_string( void ) { return partdata[PRESSURE_SETPOINT_INDEX].text(); }
    TCHAR * velocity_setpoint_string( void ) { return partdata[VELOCITY_SETPOINT_INDEX].text(); }

    float   accel_value( void );
    float   vel_value( void );
    float   end_pos_value( void );
    float   ft_stop_pos( void ) { return fdata[FT_STOP_POS_INDEX]; }
    float   gain_break_gain( int32 i );
    float   gain_break_velocity( int32 i );
    float   low_impact_value( void );
    float   limit_switch_pos_value( int32 i );
    float   velocity_loop_gain( void ) { return fdata[VELOCITY_LOOP_GAIN_INDEX]; }
    float   min_stroke_len( void );
    float   pressure_setpoint( void );
    float   velocity_setpoint( void );

    BOOLEAN have_vacuum_wait( void )           { return step[current_index].vacuum_wait;           }
    BOOLEAN have_low_impact_from_input( void ) { return step[current_index].low_impact_from_input; }
    BOOLEAN accel_is_percent( void )           { return step[current_index].accel_percent;         }
    BOOLEAN vel_is_percent( void )             { return step[current_index].vel_percent;           }

    void    set_accel( TCHAR * sorc );
    void    set_vel( TCHAR * sorc );
    void    set_end_pos( TCHAR * sorc )    { lstrcpy( step[current_index].end_pos, sorc); }
    void    set_ft_stop_pos( float new_stop_pos ) { fdata[FT_STOP_POS_INDEX] = new_stop_pos; }
    void    set_gain_break_velocity( int32 i, float new_velocity );
    void    set_gain_break_gain( int32 i, float new_gain );
    void    set_low_impact( TCHAR * sorc ) { lstrcpy( step[current_index].low_impact, sorc); }
    void    set_limit_switch_pos( TCHAR * sorc, int32 i );
    void    set_min_stroke_len( TCHAR * sorc )    { partdata[MIN_STROKE_LEN_INDEX] = sorc; }
    void    set_pressure_setpoint( TCHAR * sorc ) { partdata[PRESSURE_SETPOINT_INDEX] = sorc; }
    void    set_velocity_setpoint( TCHAR * sorc ) { partdata[VELOCITY_SETPOINT_INDEX] = sorc; }
    void    set_pressure_sensor_number( int32 new_sensor_number) { pressure_sensor_number = new_sensor_number; }

    void    set_have_vacuum_wait( BOOLEAN sorc )           { step[current_index].vacuum_wait = sorc; }
    void    set_have_low_impact_from_input( BOOLEAN sorc ) { step[current_index].low_impact_from_input = sorc; }
    void    set_velocity_loop_gain( float new_value )     { fdata[VELOCITY_LOOP_GAIN_INDEX]     = new_value; }
    };

#endif

