#ifndef _PART_CLASS_
#define _PART_CLASS_

class PART_CLASS
    {
    public:

    TCHAR          name[PART_NAME_LEN+1];
    TCHAR          computer[COMPUTER_NAME_LEN+1];
    TCHAR          machine[MACHINE_NAME_LEN+1];
    short          velocity_units;
    short          distance_units;
    short          pressure_units;
    float          total_stroke_length;
    float          min_stroke_length;
    int32          biscuit_time_delay;
    float          eos_velocity;
    float          sleeve_fill_distance;
    float          runner_fill_distance;
    float          csfs_min_velocity;
    float          csfs_rise_velocity;
    float          csfs_min_position;
    float          limit_switch_position[LIMIT_SWITCH_COUNT];
    float          user_velocity_position[USER_VEL_COUNT];
    float          user_avg_velocity_start[USER_AVG_VEL_COUNT];
    float          user_avg_velocity_end[USER_AVG_VEL_COUNT];
    float          plunger_diameter;
    int32          time_for_intens_pres;
    float          pres_for_response_time;
    short          ms_per_time_sample;
    short          nof_time_samples;
    BOOLEAN        production_enabled;
    short          head_pressure_channel;
    short          rod_pressure_channel;
    short          analog_sensor[MAX_FT2_CHANNELS];
    double         x4_divisor;
    double         velocity_multiplier;
    unsigned       good_shot_wire;
    BOOLEAN        good_shot_requires_no_warnings;

    PART_CLASS( void );
    void    init( void );
    double  plunger_area();
    BOOLEAN read_parts_db( TCHAR * fname, TCHAR * part_name );
    BOOLEAN read_ftchan_db( TCHAR * fname );
    BOOLEAN read_x4_divisor();
    BOOLEAN refresh_time_sample();
    BOOLEAN find( TCHAR * computer, TCHAR * machine, TCHAR * name );
    BOOLEAN save( void );
    double  dist_from_x4(     double x4_value ){ return x4_value/x4_divisor; }
    double  velocity_from_x4( double x4_value ){ return x4_value * velocity_multiplier / x4_divisor; }
    double  x4_from_dist(     double dist_value     ){ return dist_value*x4_divisor; }
    double  x4_from_velocity( double velocity_value ){ return velocity_value * x4_divisor / velocity_multiplier;}
    };

#endif

