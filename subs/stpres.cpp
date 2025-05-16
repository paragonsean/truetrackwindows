#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\structs.h"
#include "..\include\stpres.h"
#include "..\include\subs.h"

static TCHAR IS_GOOSE_STRING[] = TEXT("goose");
static TCHAR IS_OPEN_STRING[]  = TEXT("open");
static TCHAR NullChar = TEXT( '\0' );
static TCHAR NChar = TEXT( 'N' );
static TCHAR PercentChar = TEXT( '%' );
static TCHAR YChar = TEXT( 'Y' );

/***********************************************************************
*                      PRESSURE_CONTROL_CLASS                          *
*                              INIT                                    *
***********************************************************************/
void PRESSURE_CONTROL_CLASS::init()
{
int i;

is_enabled                 = FALSE;
use_only_positive_voltages = FALSE;
retctl_is_enabled          = FALSE;
proportional_valve         = FALSE;

nof_steps                  = 0;
sensor_number              = 0;
voltage_output_range       = 1; /* 0-5 V */
arm_intensifier_ls_number  = 1;
pressure_loop_gain         = TEXT( "25" );
integral_gain.null();
derivative_gain.null();
retract_volts              = TEXT( "500" );
park_volts                 = TEXT( "50" );

retctl_arm_pres            = TEXT( "0" );
retctl_pres                = TEXT( "0.0" );

for ( i=0; i<MAX_PRESSURE_CONTROL_STEPS; i++ )
    {
    step[i].is_goose_step    = FALSE;
    step[i].is_goose_percent = FALSE;
    step[i].is_open_loop     = FALSE;
    step[i].is_percent       = FALSE;
    }
}

/***********************************************************************
*                      PRESSURE_CONTROL_CLASS                          *
*                      PRESSURE_CONTROL_CLASS                          *
***********************************************************************/
PRESSURE_CONTROL_CLASS::PRESSURE_CONTROL_CLASS()
{
init();
}

/***********************************************************************
*                      PRESSURE_CONTROL_CLASS                          *
*                      ~PRESSURE_CONTROL_CLASS                         *
***********************************************************************/
PRESSURE_CONTROL_CLASS::~PRESSURE_CONTROL_CLASS()
{
}

/***********************************************************************
*                      PRESSURE_CONTROL_CLASS                          *
*                              =                                       *
***********************************************************************/
void PRESSURE_CONTROL_CLASS::operator=( const PRESSURE_CONTROL_CLASS & sorc )
{
int i;

is_enabled                 = sorc.is_enabled;
use_only_positive_voltages = sorc.use_only_positive_voltages;
retctl_is_enabled          = sorc.retctl_is_enabled;
proportional_valve         = sorc.proportional_valve;
nof_steps                  = sorc.nof_steps;
sensor_number              = sorc.sensor_number;
voltage_output_range       = sorc.voltage_output_range;
arm_intensifier_ls_number  = sorc.arm_intensifier_ls_number;
retctl_arm_pres            = sorc.retctl_arm_pres;
pressure_loop_gain         = sorc.pressure_loop_gain;
integral_gain              = sorc.integral_gain;
derivative_gain            = sorc.derivative_gain;
retctl_pres                = sorc.retctl_pres;
retract_volts              = sorc.retract_volts;
park_volts                 = sorc.park_volts;

for ( i=0; i<MAX_PRESSURE_CONTROL_STEPS; i++ )
    {
    step[i].is_goose_step    = sorc.step[i].is_goose_step;
    step[i].is_goose_percent = sorc.step[i].is_goose_percent;
    step[i].is_open_loop     = sorc.step[i].is_open_loop;
    step[i].is_percent       = sorc.step[i].is_percent;
    step[i].ramp             = sorc.step[i].ramp;
    step[i].pressure         = sorc.step[i].pressure;
    step[i].holdtime         = sorc.step[i].holdtime;
    }
}

/***********************************************************************
*                      PRESSURE_CONTROL_CLASS                          *
*                              FIND                                    *
***********************************************************************/
BOOLEAN PRESSURE_CONTROL_CLASS::find( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )
{
FILE_CLASS   f;
STRING_CLASS tn;
TCHAR      * cp;
int32        i;
BOOLEAN      is_old_format;

init();
is_old_format = FALSE;

tn = presure_control_datname( computer_name, machine_name, part_name );

if ( file_exists(tn.text()) )
    {
    if ( f.open_for_read(tn.text()) )
        {
        is_enabled = FALSE;
        use_only_positive_voltages = FALSE;
        retctl_is_enabled  = FALSE;
        proportional_valve = FALSE;

        cp = f.readline();
        if ( cp )
            {
            if ( *cp == YChar )
                is_enabled = TRUE;
            cp++;
            if ( *cp == YChar )
                use_only_positive_voltages = TRUE;
            cp++;
            if ( *cp == NullChar )
                {
                is_old_format = TRUE;
                }
            else
                {
                if ( *cp == YChar )
                    retctl_is_enabled = TRUE;
                cp++;
                if ( *cp == YChar )
                    proportional_valve = TRUE;
                }
            }

        cp = f.readline();
        if ( cp )
            nof_steps = asctoint32( cp );

        for ( i=0; i<nof_steps; i++ )
            {
            step[i].is_goose_step = FALSE;
            step[i].ramp    = f.readline();
            if ( step[i].ramp.contains(PercentChar) )
                step[i].is_goose_percent = TRUE;
            cp = step[i].ramp.find( IS_GOOSE_STRING );
            if ( cp )
                {
                step[i].is_goose_step = TRUE;
                *cp = NullChar;
                }
            step[i].is_open_loop = FALSE;
            step[i].pressure = f.readline();
            cp = findstring( IS_OPEN_STRING, step[i].pressure.text() );
            if ( cp )
                {
                step[i].is_open_loop = TRUE;
                *cp = NullChar;
                }

            step[i].holdtime  = f.readline();
            }

        cp = f.readline();
        if ( cp )
            sensor_number = asctoint32( cp );

        cp = f.readline();
        if ( cp )
            voltage_output_range = asctoint32( cp );

        cp = f.readline();
        if ( cp )
            arm_intensifier_ls_number = asctoint32( cp );

        retctl_arm_pres = f.readline();
        if ( is_old_format )
            retctl_arm_pres.null();
        pressure_loop_gain        = f.readline();
        retctl_pres = f.readline();
        if ( is_old_format )
            retctl_pres.null();
        retract_volts             = f.readline();
        park_volts                = f.readline();
        integral_gain             = f.readline();
        derivative_gain           = f.readline();

        f.close();
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                      PRESSURE_CONTROL_CLASS                          *
*                              SAVE                                    *
***********************************************************************/
BOOLEAN PRESSURE_CONTROL_CLASS::save( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )
{
TCHAR bs[5];

FILE_CLASS   f;
STRING_CLASS s;
int32        i;

s = presure_control_datname( computer_name, machine_name, part_name );
if ( f.open_for_write(s) )
    {
    if ( is_enabled )
        bs[0] = YChar;
    else
        bs[0] = NChar;
    if ( use_only_positive_voltages )
        bs[1] = YChar;
    else
        bs[1] = NChar;
    if ( retctl_is_enabled )
        bs[2] = YChar;
    else
        bs[2] = NChar;
    if ( proportional_valve )
        bs[3] = YChar;
    else
        bs[3] = NChar;

    bs[4] = NullChar;
    f.writeline( bs );
    f.writeline( int32toasc(nof_steps) );
    for ( i=0; i<nof_steps; i++ )
        {
        s = step[i].ramp;
        if ( step[i].is_goose_step )
            s += IS_GOOSE_STRING;
        if ( step[i].is_goose_percent && !step[i].ramp.contains(PercentChar) )
            s += PercentChar;
        f.writeline( s );

        s = step[i].pressure;
        if ( step[i].is_open_loop )
            {
            s += IS_OPEN_STRING;
            if ( step[i].is_percent && !step[i].pressure.contains(PercentChar) )
                s += PercentChar;
            }
        f.writeline( s );
        f.writeline( step[i].holdtime );
        }
    f.writeline( int32toasc(sensor_number) );
    f.writeline( int32toasc(voltage_output_range) );
    f.writeline( int32toasc(arm_intensifier_ls_number) );

    f.writeline( retctl_arm_pres );
    f.writeline( pressure_loop_gain );
    f.writeline( retctl_pres );
    f.writeline( retract_volts );
    f.writeline( park_volts );
    f.writeline( integral_gain );
    f.writeline( derivative_gain );
    f.close();
    return TRUE;
    }

return FALSE;
}