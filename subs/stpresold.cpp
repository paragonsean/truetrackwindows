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
static TCHAR YChar = TEXT( 'Y' );

/***********************************************************************
*                      PRESSURE_CONTROL_CLASS                          *
*                      PRESSURE_CONTROL_CLASS                          *
***********************************************************************/
PRESSURE_CONTROL_CLASS::PRESSURE_CONTROL_CLASS()
{
int i;

is_enabled                = FALSE;
nof_steps                 = 0;
sensor_number             = 0;
voltage_output_range      = 1; /* 0-5 V */
arm_intensifier_ls_number = 1;
arm_intensifier_pressure  = TEXT( "2500" );
pressure_loop_gain        = TEXT( "25" );
initial_pressure          = TEXT( "3000.0" );
retract_volts             = TEXT( "500" );
park_volts                = TEXT( "50" );
for ( i=0; i<MAX_PRESSURE_CONTROL_STEPS; i++ )
    {
    step[i].is_goose_step = FALSE;
    step[i].is_open_loop  = FALSE;
    }
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
is_enabled                 = sorc.is_enabled;
use_only_positive_voltages = FALSE;
nof_steps                  = sorc.nof_steps;
sensor_number              = sorc.sensor_number;
voltage_output_range       = sorc.voltage_output_range;
arm_intensifier_ls_number  = sorc.arm_intensifier_ls_number;
arm_intensifier_pressure   = sorc.arm_intensifier_pressure;
pressure_loop_gain         = sorc.pressure_loop_gain;
initial_pressure           = sorc.initial_pressure;
retract_volts              = sorc.retract_volts;
park_volts                 = sorc.park_volts;
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

tn = presure_control_datname( computer_name, machine_name, part_name );

if ( file_exists(tn.text()) )
    {
    if ( f.open_for_read(tn.text()) )
        {
        is_enabled = FALSE;
        use_only_positive_voltages = FALSE;
        cp = f.readline();
        if ( cp )
            {
            if ( *cp == TEXT('Y') )
                is_enabled = TRUE;
            cp++;
            if ( *cp == TEXT('Y') )
                use_only_positive_voltages = TRUE;
            }

        cp = f.readline();
        if ( cp )
            nof_steps = asctoint32( cp );

        for ( i=0; i<nof_steps; i++ )
            {
            step[i].is_goose_step = FALSE;
            step[i].ramp    = f.readline();
            cp = findstring( IS_GOOSE_STRING, step[i].ramp.text() );
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

        arm_intensifier_pressure  = f.readline();
        pressure_loop_gain        = f.readline();
        initial_pressure          = f.readline();
        retract_volts             = f.readline();
        park_volts                = f.readline();

        f.close();
        return TRUE;
        }
    }
else if ( nof_steps > 0 )
    {
    for ( i=0; i<nof_steps; i++ )
        {
        step[i].ramp.empty();
        step[i].is_goose_step = FALSE;
        step[i].pressure.empty();
        step[i].is_open_loop = FALSE;
        step[i].holdtime.empty();
        }
    nof_steps = 0;
    }
return FALSE;
}

/***********************************************************************
*                      PRESSURE_CONTROL_CLASS                          *
*                              SAVE                                    *
***********************************************************************/
BOOLEAN PRESSURE_CONTROL_CLASS::save( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )
{
TCHAR bs[3];

FILE_CLASS   f;
STRING_CLASS tn;
int32        i;

tn = presure_control_datname( computer_name, machine_name, part_name );
if ( f.open_for_write(tn.text()) )
    {
    if ( is_enabled )
        bs[0] = YChar;
    else
        bs[0] = NChar;
    if ( use_only_positive_voltages )
        bs[1] = YChar;
    else
        bs[1] = NChar;
    bs[2] = NullChar;
    f.writeline( bs );
    f.writeline( int32toasc(nof_steps) );
    for ( i=0; i<nof_steps; i++ )
        {
        if ( step[i].is_goose_step )
            step[i].ramp += IS_GOOSE_STRING;
        f.writeline( step[i].ramp.text() );
        if ( step[i].is_open_loop )
            step[i].pressure += IS_OPEN_STRING;
        f.writeline( step[i].pressure.text() );
        f.writeline( step[i].holdtime.text() );
        }
    f.writeline( int32toasc(sensor_number) );
    f.writeline( int32toasc(voltage_output_range) );
    f.writeline( int32toasc(arm_intensifier_ls_number) );

    f.writeline( arm_intensifier_pressure.text() );
    f.writeline( pressure_loop_gain.text() );
    f.writeline( initial_pressure.text() );
    f.writeline( retract_volts.text() );
    f.writeline( park_volts.text() );
    f.close();
    return TRUE;
    }

return FALSE;
}