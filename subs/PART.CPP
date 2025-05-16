#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\setpoint.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"
#include "..\include\part.h"
#include "..\include\verrors.h"

static TCHAR YChar = TEXT( 'Y' );
static TCHAR BackSlashString[] = TEXT( "\\" );
static TCHAR CommaString[]     = TEXT( "," );

/***********************************************************************
*                         PART_CLASS::INIT                          *
***********************************************************************/
void PART_CLASS::init( void )
{

short i;

lstrcpy( name,     NO_PART );
lstrcpy( computer, NO_COMPUTER );
lstrcpy( machine,  NO_MACHINE );
distance_units       = NO_UNITS;
velocity_units       = NO_UNITS;
pressure_units       = NO_UNITS;
total_stroke_length  = 0.0;
min_stroke_length    = 0.0;
biscuit_time_delay   = 0;
eos_velocity         = 0.0;
sleeve_fill_distance = 0.0;
runner_fill_distance = 0.0;
csfs_min_velocity    = 0.0;
csfs_rise_velocity   = 0.0;
csfs_min_position    = 0.0;

for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
    limit_switch_position[i] = 0.0;

for ( i=0; i<USER_VEL_COUNT; i++ )
    user_velocity_position[i] = 0.0;

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    user_avg_velocity_start[i] = 0.0;
    user_avg_velocity_end[i] = 0.0;
    }

plunger_diameter       = 0.0;
time_for_intens_pres   = 0;
pres_for_response_time = 0.0;
ms_per_time_sample     = 0;
nof_time_samples       = 0;
production_enabled     = FALSE;
head_pressure_channel  = 0;
rod_pressure_channel   = 0;

for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    analog_sensor[i] = NO_ANALOG_SENSOR_NUMBER;

x4_divisor          = 80.0;
velocity_multiplier =  1.0;
good_shot_wire      = NO_WIRE;
good_shot_requires_no_warnings = FALSE;
}

/***********************************************************************
*                             PART_CLASS                               *
***********************************************************************/
PART_CLASS::PART_CLASS( void )
{
init();
}

/***********************************************************************
*                             PART_CLASS                               *
*                            READ_PARTS_DB                             *
***********************************************************************/
BOOLEAN PART_CLASS::read_parts_db( TCHAR * fname, TCHAR * part_name )
{
int32    i;
DB_TABLE t;
BOOLEAN status;
FIELDOFFSET offset;
FIELDOFFSET end_offset;

status = FALSE;

t.open( fname, PARTS_RECLEN, PFL );
t.put_alpha( PARTS_PART_NAME_OFFSET, part_name, PART_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    {

    distance_units       = t.get_short( PARTS_DISTANCE_UNITS_OFFSET );
    velocity_units       = t.get_short( PARTS_VELOCITY_UNITS_OFFSET );
    pressure_units       = t.get_short( PARTS_PRESSURE_UNITS_OFFSET );
    total_stroke_length  = t.get_float( PARTS_TOTAL_STROKE_LEN_OFFSET );
    min_stroke_length    = t.get_float( PARTS_MIN_STROKE_LEN_OFFSET );
    biscuit_time_delay   = t.get_long( PARTS_BISCUIT_TIME_DELAY_OFFSET );
    eos_velocity         = t.get_float( PARTS_EOS_VEL_OFFSET );
    sleeve_fill_distance = t.get_float( PARTS_SLEEVE_FILL_DIST_OFFSET );
    runner_fill_distance = t.get_float( PARTS_RUNNER_FILL_DIST_OFFSET );
    csfs_min_velocity    = t.get_float( PARTS_MIN_CSFS_VEL_OFFSET );
    csfs_rise_velocity   = t.get_float( PARTS_RISE_CSFS_VEL_OFFSET );
    csfs_min_position    = t.get_float( PARTS_MIN_CSFS_POS_OFFSET );

    offset = PARTS_LIMIT_SW_1_OFFSET;
    for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
        {
        limit_switch_position[i]= t.get_float( offset );
        offset += PARTS_DB_FLOAT_LEN + 1;
        }

    offset = PARTS_USER_VEL_1_OFFSET;
    for ( i=0; i<USER_VEL_COUNT; i++ )
        {
        user_velocity_position[i]= t.get_float( offset );
        offset += PARTS_DB_FLOAT_LEN + 1;
        }

    offset     = PARTS_AVG_START_1_OFFSET;
    end_offset = PARTS_AVG_END_1_OFFSET;
    for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
        {
        user_avg_velocity_start[i] = t.get_float( offset );
        user_avg_velocity_end[i]   = t.get_float( end_offset );
        offset     += PARTS_DB_FLOAT_LEN + 1;
        end_offset += PARTS_DB_FLOAT_LEN + 1;
        }

    plunger_diameter       = t.get_float( PARTS_PLUNGER_DIA_OFFSET );
    time_for_intens_pres   = t.get_long( PARTS_TIME_FOR_INTENS_PRES_OFFSET );
    pres_for_response_time = t.get_float( PARTS_PRES_FOR_RESP_TIME_OFFSET );
    ms_per_time_sample     = t.get_short( PARTS_MS_PER_TIME_SAMPLE_OFFSET );
    nof_time_samples       = t.get_short( PARTS_N_OF_TIME_SAMPLES_OFFSET );
    production_enabled     = t.get_boolean( PARTS_PRODUCTION_ENABLED );
    head_pressure_channel  = t.get_short( PARTS_HEAD_CHANNEL_OFFSET );
    rod_pressure_channel   = t.get_short( PARTS_ROD_CHANNEL_OFFSET );

    status = TRUE;
    }

t.close();

/*
---------------------------------------------------------------------------------------------
I don't use the setpoint_name() function here because I may be getting the information from a
different place
s = fname;
if ( dir_from_filename(s.text()) )
    {
    s += BackSlashString;
    s += part_name;
    s += BackSlashString;
    s += SETPOINT_CSV_FILE;
    setpoint.get( s.text() );
    }
--------------------------------------------------------------------------------------------- */

return status;
}

/***********************************************************************
*                             PART_CLASS                               *
*                         REFRESH_TIME_SAMPLE                          *
*                                                                      *
* This routine refreshes the ms_per_time_sample and the number         *
* of time samples. It is called by the part editor so that if the      *
* monitor setup has been changed, saving the part doesn't unchange it. *
*                                                                      *
***********************************************************************/
BOOLEAN PART_CLASS::refresh_time_sample()
{
BOOLEAN    status;
NAME_CLASS s;
DB_TABLE   t;

status = FALSE;

s.get_parts_file_name( computer, machine );
if ( file_exists(s.text()) )
    {
    t.open( s.text(), PARTS_RECLEN, PFL );
    t.put_alpha( PARTS_PART_NAME_OFFSET, name, PART_NAME_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        {
        ms_per_time_sample = t.get_short( PARTS_MS_PER_TIME_SAMPLE_OFFSET );
        nof_time_samples   = t.get_short( PARTS_N_OF_TIME_SAMPLES_OFFSET );
        status = TRUE;
        }

    t.close();
    }

return status;
}

/***********************************************************************
*                             PART_CLASS                               *
*                           READ_FTCHAN_DB                             *
***********************************************************************/
BOOLEAN PART_CLASS::read_ftchan_db( TCHAR * fname )
{
short    i;
DB_TABLE t;
BOOLEAN status;

status = FALSE;

if ( file_exists(fname) )
    {
    if ( t.open(fname, FTCHAN_RECLEN, PFL) )
        {
        /*
        -------------------------------------------------
        Clear all the sensors before loading the new ones
        ------------------------------------------------- */
        for ( i=0; i<MAX_FT2_CHANNELS; i++ )
            analog_sensor[i] = NO_ANALOG_SENSOR_NUMBER;

        while( t.get_next_record(NO_LOCK) )
            {
            i = t.get_short( FTCHAN_CHANNEL_NUMBER_OFFSET );
            if ( i > 0 && i <= MAX_FT2_CHANNELS )
                {
                i--;
                analog_sensor[i] = t.get_short( FTCHAN_SENSOR_NUMBER_OFFSET );
                }
            }
        status = TRUE;
        t.close();
        }
    }

return status;
}

/***********************************************************************
*                           READ_X4_DIVISOR                            *
***********************************************************************/
BOOLEAN PART_CLASS::read_x4_divisor()
{
COMPUTER_CLASS c;
BOOLEAN status;
DB_TABLE t;
TCHAR rodpitch[RODPITCH_NAME_LEN+1];
NAME_CLASS s;

status = FALSE;
s.get_machset_file_name( computer );
t.open( s.text(), MACHSET_RECLEN, PFL );
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, machine, MACHINE_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    {
    t.get_alpha( rodpitch, MACHSET_RODPITCH_OFFSET, RODPITCH_NAME_LEN );
    status = TRUE;
    }
t.close();

if ( !status )
    return status;

status = FALSE;
s.get_psensor_file_name( computer );

/*
----------------------------------
Backups don't have a psensor table
---------------------------------- */
if ( !s.file_exists() )
    s.get_psensor_file_name( c.whoami() );

if ( t.open(s.text(), PSENSOR_RECLEN, PFL) )
    {
    t.put_alpha( PSENSOR_RODPITCH_OFFSET,   rodpitch,       RODPITCH_NAME_LEN );
    t.put_short( PSENSOR_DIST_UNITS_OFFSET, distance_units, UNITS_ID_LEN      );
    t.reset_search_mode();

    if ( t.get_next_key_match(2, NO_LOCK) )
        {
        x4_divisor = t.get_double( PSENSOR_X4_POS_DIVISOR_OFFSET );
        status = TRUE;
        }
    t.close();
    }

return status;
}

/***********************************************************************
*                           PART_CLASS::FIND                           *
***********************************************************************/
BOOLEAN PART_CLASS::find( TCHAR * computer_to_find, TCHAR * machine_to_find, TCHAR * name_to_find )
{
BOOLEAN status;
SETPOINT_CLASS sp;
NAME_CLASS s;

s.get_parts_file_name( computer_to_find, machine_to_find );
if ( !file_exists(s.text()) )
    return FALSE;

status = read_parts_db( s.text(), name_to_find );

if ( status )
    {
    /*
    -------------------------------------------
    It's possible to call using my own strings.
    Don't copy from myself.
    ------------------------------------------- */
    if ( computer != computer_to_find )
        lstrcpy( computer, computer_to_find );
    if ( machine != machine_to_find )
        lstrcpy( machine, machine_to_find );
    if ( name != name_to_find )
        lstrcpy( name, name_to_find );

    s.get_ftchan_file_name( computer, machine, name );
    read_ftchan_db( s.text() );
    read_x4_divisor();
    velocity_multiplier = dups_to_vu( velocity_units, distance_units );
    good_shot_wire = NO_WIRE;
    good_shot_requires_no_warnings = FALSE;
    s.get_good_shot_setup_file_name( computer, machine, name );
    if ( sp.get(s.text()) > 1 )
        {
        good_shot_wire = sp[0].value.uint_value();
        if ( good_shot_wire > HIGH_FT2_OUTPUT_WIRE )
            good_shot_wire = NO_WIRE;
        good_shot_requires_no_warnings = ascii_to_boolean( sp[1].value.text() );
        }
    }

return status;
}

/***********************************************************************
*                          PART_CLASS::SAVE                            *
***********************************************************************/
BOOLEAN PART_CLASS::save( void )
{
TCHAR       this_part[PART_NAME_LEN+1];
short       i;
BOOLEAN     is_new_part;
BOOLEAN     need_to_insert;
BOOLEAN     status;
FIELDOFFSET offset;
FIELDOFFSET end_offset;
DB_TABLE    t;
FILE_CLASS  f;
NAME_CLASS  s;
SETPOINT_CLASS sp;

is_new_part = TRUE;
need_to_insert = FALSE;
status         = FALSE;

s.get_parts_file_name( computer, machine );
if ( s.file_exists() )
    {
    if ( t.open( s.text(), PARTS_RECLEN, PWL) )
        {
        while( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( this_part, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
            if ( strings_are_equal(name, this_part, PART_NAME_LEN) )
                {
                t.lock_record();
                is_new_part = FALSE;
                break;
                }
            else if ( compare(name, this_part, PART_NAME_LEN) < 0 )
                {
                need_to_insert = TRUE;
                break;
                }
            }

        t.put_alpha( PARTS_PART_NAME_OFFSET, name, PART_NAME_LEN );

        t.put_short( PARTS_DISTANCE_UNITS_OFFSET,     distance_units,       UNITS_ID_LEN );
        t.put_short( PARTS_VELOCITY_UNITS_OFFSET,     velocity_units,       UNITS_ID_LEN );
        t.put_short( PARTS_PRESSURE_UNITS_OFFSET,     pressure_units,       UNITS_ID_LEN );
        t.put_float( PARTS_TOTAL_STROKE_LEN_OFFSET,   total_stroke_length,  PARTS_DB_FLOAT_LEN );
        t.put_float( PARTS_MIN_STROKE_LEN_OFFSET,     min_stroke_length,    PARTS_DB_FLOAT_LEN );
        t.put_long(  PARTS_BISCUIT_TIME_DELAY_OFFSET, biscuit_time_delay,   PARTS_DB_INT32_LEN );
        t.put_float( PARTS_EOS_VEL_OFFSET,            eos_velocity,         PARTS_DB_FLOAT_LEN );
        t.put_float( PARTS_SLEEVE_FILL_DIST_OFFSET,   sleeve_fill_distance, PARTS_DB_FLOAT_LEN );
        t.put_float( PARTS_RUNNER_FILL_DIST_OFFSET,   runner_fill_distance, PARTS_DB_FLOAT_LEN );
        t.put_float( PARTS_MIN_CSFS_VEL_OFFSET,       csfs_min_velocity,    PARTS_DB_FLOAT_LEN );
        t.put_float( PARTS_RISE_CSFS_VEL_OFFSET,      csfs_rise_velocity,   PARTS_DB_FLOAT_LEN );
        t.put_float( PARTS_MIN_CSFS_POS_OFFSET,       csfs_min_position,    PARTS_DB_FLOAT_LEN );

        offset = PARTS_LIMIT_SW_1_OFFSET;
        for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
            {
            t.put_float( offset, limit_switch_position[i], PARTS_DB_FLOAT_LEN );
            offset += PARTS_DB_FLOAT_LEN + 1;
            }

        offset = PARTS_USER_VEL_1_OFFSET;
        for ( i=0; i<USER_VEL_COUNT; i++ )
            {
            t.put_float( offset, user_velocity_position[i], PARTS_DB_FLOAT_LEN );
            offset += PARTS_DB_FLOAT_LEN + 1;
            }

        offset     = PARTS_AVG_START_1_OFFSET;
        end_offset = PARTS_AVG_END_1_OFFSET;
        for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
            {
            t.put_float( offset,     user_avg_velocity_start[i], PARTS_DB_FLOAT_LEN );
            t.put_float( end_offset, user_avg_velocity_end[i],   PARTS_DB_FLOAT_LEN );
            offset     += PARTS_DB_FLOAT_LEN + 1;
            end_offset += PARTS_DB_FLOAT_LEN + 1;
            }

        t.put_float(   PARTS_PLUNGER_DIA_OFFSET,          plunger_diameter,       PARTS_DB_FLOAT_LEN );
        t.put_long(    PARTS_TIME_FOR_INTENS_PRES_OFFSET, time_for_intens_pres,   PARTS_DB_INT32_LEN );
        t.put_float(   PARTS_PRES_FOR_RESP_TIME_OFFSET,   pres_for_response_time, PARTS_DB_FLOAT_LEN );
        t.put_short(   PARTS_MS_PER_TIME_SAMPLE_OFFSET,   ms_per_time_sample,     PARTS_DB_SHORT_LEN );
        t.put_short(   PARTS_N_OF_TIME_SAMPLES_OFFSET,    nof_time_samples,       PARTS_DB_SHORT_LEN );
        t.put_boolean( PARTS_PRODUCTION_ENABLED,          production_enabled );
        t.put_short(   PARTS_HEAD_CHANNEL_OFFSET,           head_pressure_channel,  CHANNEL_NUMBER_LEN );
        t.put_short(   PARTS_ROD_CHANNEL_OFFSET,            rod_pressure_channel,   CHANNEL_NUMBER_LEN );

        if ( is_new_part )
            {
            if ( need_to_insert )
                status = t.rec_insert();
            else
                status = t.rec_append();
            }
        else
            {
            status = t.rec_update();
            t.unlock_record();
            }

        t.close();

        if ( status )
            {
            s.get_ftchan_file_name( computer, machine, name );
            if ( !s.file_exists() )
                t.create( s.text() );

            t.open( s.text(), FTCHAN_RECLEN, FL );
            t.empty();
            for ( i=0; i<MAX_FT2_CHANNELS; i++ )
                {
                t.put_short( FTCHAN_CHANNEL_NUMBER_OFFSET, i+1,              CHANNEL_NUMBER_LEN );
                t.put_short( FTCHAN_SENSOR_NUMBER_OFFSET,  analog_sensor[i], SENSOR_NUMBER_LEN );
                t.rec_append();
                }
            t.close();

            s.get_good_shot_setup_file_name( computer, machine, name );
            sp.setsize( 2 );
            sp[0].desc  = GOOD_SHOT_WIRE_DESC;
            sp[0].value = good_shot_wire;
            sp[1].desc  = GOOD_SHOT_IF_NO_WARNINGS_DESC;
            sp[1].value = boolean_to_ascii( good_shot_requires_no_warnings );
            sp.put( s.text() );
            }
        }
    }

return status;
}

/***********************************************************************
*                             PART_CLASS                               *
*                            plunger_area                              *
***********************************************************************/
double PART_CLASS::plunger_area()
{
double x;

x = double( plunger_diameter );
if ( not_zero(x) )
    {
    x *= x;
    x *= .78540;
    }
else
    x = 1.0;

return x;
}
