#ifndef _PRESSURE_CONTROL_CLASS_
#define _PRESSURE_CONTROL_CLASS_

const int32 MAX_PRESSURE_CONTROL_STEPS   = 3;
const int32 NO_CONTROL_PRESSURE          = 32767;

struct PRES_CONTROL_STEP_ENTRY
    {
    STRING_CLASS ramp;
    BOOLEAN      is_goose_step;
    STRING_CLASS pressure;
    BOOLEAN      is_open_loop;
    STRING_CLASS holdtime;
    };

class PRESSURE_CONTROL_CLASS
    {
    public:
    BOOLEAN      is_enabled;
    BOOLEAN      use_only_positive_voltages;
    BOOLEAN      enable_retract;
    int32        nof_steps;
    int32        sensor_number;
    int32        voltage_output_range;    /* 0=0-2.5, 1=0-5v, 2=0-10v */
    int32        arm_intensifier_ls_number;
    STRING_CLASS retctrl_arm_volts;
    STRING_CLASS pressure_loop_gain;
    STRING_CLASS retctrl_pressure;
    STRING_CLASS retract_volts;
    STRING_CLASS park_volts;
    PRES_CONTROL_STEP_ENTRY step[MAX_PRESSURE_CONTROL_STEPS];

    PRESSURE_CONTROL_CLASS();
    ~PRESSURE_CONTROL_CLASS();

    void    operator=( const PRESSURE_CONTROL_CLASS & sorc );
    BOOLEAN find( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name );
    BOOLEAN find( PART_NAME_ENTRY & p ) { return find( p.computer, p.machine, p.part ); }
    BOOLEAN save( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name );
    BOOLEAN save( PART_NAME_ENTRY & p ) { return save( p.computer, p.machine, p.part ); }
    };

#endif

