#ifndef _SURETRAK_PARAM_CLASS_
#define _SURETRAK_PARAM_CLASS_

const int32 NOF_INT_PARAMS   = 2;
const int32 NOF_FLOAT_PARAMS = 8;

const int32 NULL_DAC_OFFSET_INDEX   = 0;
const int32 LVDT_DAC_OFFSET_INDEX   = 1;

const int32 ZERO_SPEED_CHECK_POS_INDEX   = 0;
const int32 COUNTS_PER_UNIT_INDEX        = 1;
const int32 JOG_VALVE_PERCENT_INDEX      = 2;
const int32 RETRACT_VALVE_PERCENT_INDEX  = 3;
const int32 FT_VALVE_PERCENT_INDEX       = 4;
const int32 VEL_MULTIPLIER_INDEX         = 5;
const int32 MIN_LOW_IMPACT_PERCENT_INDEX = 6;
const int32 MAX_VELOCITY_INDEX           = 7;

class SURETRAK_PARAM_CLASS
    {
    private:

    int32   idata[NOF_INT_PARAMS];
    float   fdata[NOF_FLOAT_PARAMS];
    uint32  velocity_dividend_value;
    TCHAR   desc[MAX_ST_LIMIT_SWITCHES][DESC_LEN];

    int32   vacuum_limit_switch;

    public:

    SURETRAK_PARAM_CLASS();
    ~SURETRAK_PARAM_CLASS();
    BOOLEAN find( TCHAR * computer_name, TCHAR * machine_name );
    BOOLEAN find( TCHAR * computer_name );
    BOOLEAN save( TCHAR * computer_name );

    uint32 velocity_dividend( void ) { return velocity_dividend_value; }

    int32 null_dac_offset( void )  { return idata[NULL_DAC_OFFSET_INDEX];  }
    int32 lvdt_dac_offset( void )  { return idata[LVDT_DAC_OFFSET_INDEX];  }
    int32 vacuum_limit_switch_number( void ) { return vacuum_limit_switch; }

    float zero_speed_check_pos( void )   { return fdata[ZERO_SPEED_CHECK_POS_INDEX];   }
    float counts_per_unit( void )        { return fdata[COUNTS_PER_UNIT_INDEX];        }
    float jog_valve_percent( void )      { return fdata[JOG_VALVE_PERCENT_INDEX];      }
    float retract_valve_percent( void )  { return fdata[RETRACT_VALVE_PERCENT_INDEX];  }
    float ft_valve_percent( void )       { return fdata[FT_VALVE_PERCENT_INDEX];       }
    float vel_multiplier( void )         { return fdata[VEL_MULTIPLIER_INDEX];         }
    float min_low_impact_percent( void ) { return fdata[MIN_LOW_IMPACT_PERCENT_INDEX]; }
    float max_velocity( void )           { return fdata[MAX_VELOCITY_INDEX];           }

    TCHAR * limit_switch_desc( int32 i );

    void set_lvdt_dac_offset(  int32 new_value ) { idata[LVDT_DAC_OFFSET_INDEX]    = new_value; }
    void set_null_dac_offset(  int32 new_value ) { idata[NULL_DAC_OFFSET_INDEX]    = new_value; }
    void set_velocity_dividend( uint32 new_value ) { velocity_dividend_value = new_value; }

    void set_zero_speed_check_pos( float new_value )   { fdata[ZERO_SPEED_CHECK_POS_INDEX]   = new_value; }
    void set_counts_per_unit( float new_value )        { fdata[COUNTS_PER_UNIT_INDEX]        = new_value; }
    void set_jog_valve_percent( float new_value )      { fdata[JOG_VALVE_PERCENT_INDEX]      = new_value; }
    void set_retract_valve_percent( float new_value )  { fdata[RETRACT_VALVE_PERCENT_INDEX]  = new_value; }
    void set_ft_valve_percent( float new_value )       { fdata[FT_VALVE_PERCENT_INDEX]       = new_value; }
    void set_vel_multiplier( float new_value )         { fdata[VEL_MULTIPLIER_INDEX]         = new_value; }
    void set_min_low_impact_percent( float new_value ) { fdata[MIN_LOW_IMPACT_PERCENT_INDEX] = new_value; }
    void set_max_velocity( float new_value )           { fdata[MAX_VELOCITY_INDEX]           = new_value; }

    void set_limit_switch_desc( int32 i, TCHAR * sorc );
    void set_vacuum_limit_switch_number( int32 n ) { vacuum_limit_switch = n; }

    };

#endif

