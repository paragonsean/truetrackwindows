#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\stringcl.h"
#include "..\include\fileclas.h"
#include "..\include\stsetup.h"

static const TCHAR NullChar      = TEXT( '\0' );
static const TCHAR PercentChar   = TEXT( '%' );
static       TCHAR EmptyString[] = TEXT( "" );

/***********************************************************************
*                              IS_PERCENT                              *
***********************************************************************/
static BOOLEAN is_percent( TCHAR * sorc )
{
if ( findchar(PercentChar, sorc ) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                             FLOAT_VALUE                              *
***********************************************************************/
static float float_value( TCHAR * sorc )
{
static TCHAR s[STSTEPS_FLOAT_LEN+1];
TCHAR * cp;
int32   i;

cp = s;
for ( i=0; i<STSTEPS_FLOAT_LEN; i++ )
    {
    if ( *sorc == PercentChar || *sorc == NullChar )
        break;

    *cp = *sorc;
    cp++;
    sorc++;
    }

*cp = NullChar;

/*
-------------------------------------------
If the string is empty of numbers, return 0
------------------------------------------- */
if ( cp == s )
    return 0.0;

return extfloat( s );
}

/***********************************************************************
*                            CLEAR_STEPS                               *
***********************************************************************/
void SURETRAK_SETUP_CLASS::clear_steps( void )
{
int32 i;

n             = 0;
current_index = 0;
is_rewound    = TRUE;

for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    *step[i].accel      = NullChar;
    *step[i].vel        = NullChar;
    *step[i].end_pos    = NullChar;
    *step[i].low_impact = NullChar;

    step[i].vacuum_wait           = FALSE;
    step[i].low_impact_from_input = FALSE;
    step[i].accel_percent         = FALSE;
    step[i].vel_percent           = FALSE;
    }

}

/***********************************************************************
*                              EMPTY                                   *
***********************************************************************/
void SURETRAK_SETUP_CLASS::empty( void )
{
int32 i;

clear_steps();

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    *limit_switch_pos[i] = NullChar;

fdata[VELOCITY_LOOP_GAIN_INDEX]     = 300.0;

for ( i=0; i<NOF_GAIN_BREAKS; i++ )
    {
    gainbreak[i].velocity = 0.0;
    gainbreak[i].gain     = 1.0;
    }

for ( i=0; i<NOF_PART_DATA; i++ )
    partdata[i].empty();

pressure_sensor_number = DEFAULT_ANALOG_SENSOR;
}

/***********************************************************************
*                         SURETRAK_SETUP_CLASS                         *
***********************************************************************/
SURETRAK_SETUP_CLASS::SURETRAK_SETUP_CLASS()
{
empty();
}

/***********************************************************************
*                         ~SURETRAK_SETUP_CLASS                        *
***********************************************************************/
SURETRAK_SETUP_CLASS::~SURETRAK_SETUP_CLASS()
{

}

/***********************************************************************
*                                FIND                                  *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::find( PART_NAME_ENTRY & p )
{
const int32  OLD_MAX_ST_LIMIT_SWITCHES = 4;
int32        i;
DB_TABLE     t;
FIELDOFFSET  fo;
DWORD        status;
TCHAR      * cp;
NAME_CLASS   s;
FILE_CLASS   f;
double       x;

s.get_ststeps_dbname( p.computer, p.machine, p.part );
if ( !t.open(s.text(), STSTEPS_RECLEN, PFL) )
    {
    status = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        t.get_global_error(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &cp,
        0,
        NULL
        );

    if ( status )
        {
        MessageBox( NULL, cp, ultohex(t.get_global_error()), MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL );
        LocalFree( cp );
        }
    return FALSE;
    }

n = 0;
while ( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( step[n].accel,      STSTEPS_ACCEL_OFFSET,      STSTEPS_FLOAT_LEN );
    strip( step[n].accel );
    t.get_alpha( step[n].vel,        STSTEPS_VEL_OFFSET,        STSTEPS_FLOAT_LEN );
    strip( step[n].vel );
    t.get_alpha( step[n].end_pos,    STSTEPS_END_POS_OFFSET,    STSTEPS_FLOAT_LEN );
    strip( step[n].end_pos );
    t.get_alpha( step[n].low_impact, STSTEPS_LOW_IMPACT_OFFSET, STSTEPS_FLOAT_LEN );
    strip( step[n].low_impact );

    step[n].vacuum_wait           = t.get_boolean( STSTEPS_VACUUM_WAIT_OFFSET );
    step[n].low_impact_from_input = t.get_boolean( STSTEPS_LOW_IMPACT_FROM_INPUT_OFFSET );
    step[n].accel_percent         = is_percent( step[n].accel );
    step[n].vel_percent           = is_percent( step[n].vel );

    n++;
    }

t.close();

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    *limit_switch_pos[i] = NullChar;

s.get_stlimits_datname( p.computer, p.machine, p.part );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
        {
        for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
            {
            cp = f.readline();
            if ( cp )
                lstrcpy( limit_switch_pos[i], cp );
            }

        for ( i=0; i<NOF_GAIN_BREAKS; i++ )
            {
            cp = f.readline();
            if ( cp )
                gainbreak[i].velocity = extfloat( cp );
            }

        for ( i=0; i<NOF_GAIN_BREAKS; i++ )
            {
            cp = f.readline();
            if ( cp )
                gainbreak[i].gain = extfloat( cp );
            }

        for ( i=0; i<NOF_PART_PARAMS; i++ )
            {
            cp = f.readline();
            if ( cp )
                fdata[i] = extfloat( cp );
            }

        f.close();
        }
    }
else
    {
    /*
    ----------------------------------------------------------------
    This is probably an old part with the old db format for stlimits
    ---------------------------------------------------------------- */
    s.get_stlimits_dbname( p.computer, p.machine, p.part );
    if ( s.file_exists() )
        {
        if ( t.open(s.text(), STLIMITS_RECLEN, PFL) )
            {
            fo = 0;
            if ( t.get_next_record(NO_LOCK) )
                {
                for ( i=0; i<OLD_MAX_ST_LIMIT_SWITCHES; i++ )
                    {
                    t.get_alpha( limit_switch_pos[i], fo, STLIMITS_FLOAT_LEN );
                    fo += STLIMITS_FLOAT_LEN + ALPHA_ADDED_CHARS;
                    }

                for ( i=0; i<NOF_GAIN_BREAKS; i++ )
                    {
                    gainbreak[i].velocity = t.get_float( fo );
                    fo += STLIMITS_FLOAT_LEN + NUMBER_ADDED_CHARS;

                    gainbreak[i].gain = t.get_float( fo );
                    fo += STLIMITS_FLOAT_LEN + NUMBER_ADDED_CHARS;
                    }

                for ( i=0; i<NOF_PART_PARAMS; i++ )
                    {
                    fdata[i] = t.get_float( fo );
                    fo += STLIMITS_FLOAT_LEN + NUMBER_ADDED_CHARS;
                    }
                }

            t.close();
            }
        }
    }

/*
---------------------------------------------------------------------------------------
Get limit switches 5 and 6 from the part file. If this is not a ft2 these are not used.
--------------------------------------------------------------------------------------- */
s.get_parts_file_name( p.computer, p.machine );
if ( t.open(s.text(), PARTS_RECLEN, PFL) )
    {
    t.put_alpha( PARTS_PART_NAME_OFFSET, p.part, PART_NAME_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        {
        fo = PARTS_LIMIT_SW_1_OFFSET;
        for ( i=MAX_ST_LIMIT_SWITCHES; i<MAX_FTII_LIMIT_SWITCHES; i++ )
            {
            x = t.get_double( fo );
            if ( is_zero(x) )
                {
                s.null();
                }
            else
                {
                s = x;
                lstrcpy( limit_switch_pos[i], s.text() );
                }
            fo += PARTS_DB_FLOAT_LEN + 1;
            }
        }
    t.close();
    }

/*
------------------------------------------------------------------------
New part data is contained in the STPART.DAT file, one line per variable
------------------------------------------------------------------------ */
pressure_sensor_number = DEFAULT_ANALOG_SENSOR;
for ( i=0; i<NOF_PART_DATA; i++ )
    partdata[i].empty();

s = stpart_datname( p.computer, p.machine, p.part );
if ( f.open_for_read(s.text()) )
    {
    cp = f.readline();
    if ( cp )
        pressure_sensor_number = asctoint32( cp );
    for ( i=0; i<NOF_PART_DATA; i++ )
        partdata[i] = f.readline();
    f.close();
    }

if ( t.get_global_error() == VS_SUCCESS )
    return TRUE;

current_index = 0;
is_rewound    = TRUE;

return TRUE;
}

/***********************************************************************
*                                FIND                                  *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::find( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )
{
PART_NAME_ENTRY p;

lstrcpy( p.computer, computer_name );
lstrcpy( p.machine,  machine_name );
lstrcpy( p.part,     part_name );

return find( p );
}

/***********************************************************************
*                                SAVE                                  *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::save( PART_NAME_ENTRY & p )
{
int32        i;
FIELDOFFSET  offset;
TCHAR        this_part[PART_NAME_LEN+1];
NAME_CLASS   s;
DB_TABLE     t;
FILE_CLASS   f;

s.get_ststeps_dbname( p.computer, p.machine, p.part );

if ( !s.file_exists() )
    {
    if ( !t.create(s.text()) )
        return FALSE;
    }

if ( !t.open(s.text(), STSTEPS_RECLEN, FL) )
    return FALSE;

t.empty();
for ( i=0; i<n; i++ )
    {
    t.put_alpha(   STSTEPS_ACCEL_OFFSET,          step[i].accel,      STSTEPS_FLOAT_LEN );
    t.put_alpha(   STSTEPS_VEL_OFFSET,            step[i].vel,        STSTEPS_FLOAT_LEN );
    t.put_alpha(   STSTEPS_END_POS_OFFSET,        step[i].end_pos,    STSTEPS_FLOAT_LEN );
    t.put_alpha(   STSTEPS_LOW_IMPACT_OFFSET,     step[i].low_impact, STSTEPS_FLOAT_LEN );
    t.put_boolean( STSTEPS_VACUUM_WAIT_OFFSET,    step[i].vacuum_wait );
    t.put_boolean( STSTEPS_LOW_IMPACT_FROM_INPUT_OFFSET, step[i].low_impact_from_input );
    t.rec_append();
    }

t.close();

/*
--------------------------------------------------------------
Save the limits in a data file (this replaces the stlimits.txt
database that was used in 5.43.
-------------------------------------------------------------- */
s.get_stlimits_datname( p.computer, p.machine, p.part );
if ( f.open_for_write(s) )
    {
    for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
        f.writeline( limit_switch_pos[i] );

    for ( i=0; i<NOF_GAIN_BREAKS; i++ )
        f.writeline( ascii_float(gainbreak[i].velocity) );

    for ( i=0; i<NOF_GAIN_BREAKS; i++ )
        f.writeline( ascii_float(gainbreak[i].gain) );

    for ( i=0; i<NOF_PART_PARAMS; i++ )
        f.writeline( ascii_float(fdata[i]) );

    f.close();
    }

/*
---------------------------------------------------------------------------------------
Put limit switches 5 and 6 into the part file. If this is not a ft2 these are not used.
--------------------------------------------------------------------------------------- */
s.get_parts_file_name( p.computer, p.machine );
if ( s.file_exists() )
    {
    if ( t.open(s.text(), PARTS_RECLEN, PWL) )
        {
        while( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( this_part, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
            if ( strings_are_equal(p.part, this_part, PART_NAME_LEN) )
                {
                t.lock_record();
                offset = PARTS_LIMIT_SW_1_OFFSET;
                for ( i=MAX_ST_LIMIT_SWITCHES; i<MAX_FTII_LIMIT_SWITCHES; i++ )
                    {
                    t.put_float( offset, limit_switch_pos_value(i), PARTS_DB_FLOAT_LEN );
                    offset += PARTS_DB_FLOAT_LEN + 1;
                    }
                t.rec_update();
                t.unlock_record();
                }
            }
        t.close();
        }
    }

/*
-------------------------------------------------------------
I have just saved the limits using the new stlimits.dat file.
If there is an old database, delete it.
------------------------------------------------------------- */
s.get_stlimits_dbname( p.computer, p.machine, p.part );
if ( s.file_exists() )
    s.delete_file();

s = stpart_datname( p.computer, p.machine, p.part );
if ( f.open_for_write(s.text()) )
    {
    f.writeline( int32toasc(pressure_sensor_number) );
    for ( i=0; i<NOF_PART_DATA; i++ )
        f.writeline( partdata[i].text() );
    f.close();
    }

if ( t.get_global_error() == VS_SUCCESS )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                                SAVE                                  *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::save( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )
{
PART_NAME_ENTRY p;

lstrcpy( p.computer, computer_name );
lstrcpy( p.machine,  machine_name );
lstrcpy( p.part,     part_name );

return save( p );
}

/***********************************************************************
*                               APPEND                                 *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::append( void )
{
if ( n < MAX_ST_STEPS )
    {
    n++;
    current_index = n - 1;
    is_rewound    = FALSE;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                                NEXT                                  *
*  If I have just rewound, point to the 0th record and return true     *
*  if there is one. Otherwise, if there is another record, point to    *
*  it and return TRUE.                                                 *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::next( void )
{

if ( is_rewound )
    {
    is_rewound = FALSE;
    if ( n > 0 )
        return TRUE;
    }
else if ( current_index < (n-1) )
    {
    current_index++;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             ACCEL_VALUE                              *
***********************************************************************/
float SURETRAK_SETUP_CLASS::accel_value( void )
{
return float_value( step[current_index].accel );
}

/***********************************************************************
*                               VEL_VALUE                              *
***********************************************************************/
float SURETRAK_SETUP_CLASS::vel_value( void )
{
return float_value( step[current_index].vel );
}

/***********************************************************************
*                           END_POS_VALUE                              *
***********************************************************************/
float SURETRAK_SETUP_CLASS::end_pos_value( void )
{
return float_value( step[current_index].end_pos );
}

/***********************************************************************
*                         GAIN_BREAK_VELOCITY                          *
***********************************************************************/
float SURETRAK_SETUP_CLASS::gain_break_velocity( int32 i )
{
if ( i < 0 || i >= NOF_GAIN_BREAKS )
    return 0.0;

return gainbreak[i].velocity;
}

/***********************************************************************
*                           GAIN_BREAK_GAIN                            *
***********************************************************************/
float SURETRAK_SETUP_CLASS::gain_break_gain( int32 i )
{

if ( i < 0 || i >= NOF_GAIN_BREAKS )
    return 0.0;

return gainbreak[i].gain;
}

/***********************************************************************
*                          LOW_IMPACT_VALUE                            *
***********************************************************************/
float SURETRAK_SETUP_CLASS::low_impact_value( void )
{
return float_value( step[current_index].low_impact );
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_STRING                      *
***********************************************************************/
TCHAR * SURETRAK_SETUP_CLASS::limit_switch_pos_string( int32 i )
{
if ( i >= 0 && i < MAX_FTII_LIMIT_SWITCHES )
    return limit_switch_pos[i];

return EmptyString;
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_VALUE                       *
***********************************************************************/
float SURETRAK_SETUP_CLASS::limit_switch_pos_value( int32 i )
{
if ( i >= 0 && i < MAX_FTII_LIMIT_SWITCHES )
    return float_value( limit_switch_pos[i] );

return 0.0;
}

/***********************************************************************
*                               SET_ACCEL                              *
***********************************************************************/
void SURETRAK_SETUP_CLASS::set_accel( TCHAR * sorc )
{
lstrcpy( step[current_index].accel, sorc);
step[current_index].accel_percent = is_percent( sorc );
}

/***********************************************************************
*                       SET_GAIN_BREAK_VELOCITY                        *
***********************************************************************/
void SURETRAK_SETUP_CLASS::set_gain_break_velocity( int32 i, float new_velocity )
{
if ( i >= 0 && i < NOF_GAIN_BREAKS )
    gainbreak[i].velocity = new_velocity;
}

/***********************************************************************
*                         SET_GAIN_BREAK_GAIN                          *
***********************************************************************/
void SURETRAK_SETUP_CLASS::set_gain_break_gain( int32 i, float new_gain )
{
if ( i >= 0 && i < NOF_GAIN_BREAKS )
    gainbreak[i].gain = new_gain;
}

/***********************************************************************
*                                 SET_VEL                              *
***********************************************************************/
void SURETRAK_SETUP_CLASS::set_vel( TCHAR * sorc )
{
lstrcpy( step[current_index].vel, sorc);
step[current_index].vel_percent = is_percent( sorc );
}

/***********************************************************************
*                           SET_LIMIT_SWITCH_POS                       *
***********************************************************************/
void SURETRAK_SETUP_CLASS::set_limit_switch_pos( TCHAR * sorc, int32 i )
{
if ( i >= 0 && i < MAX_FTII_LIMIT_SWITCHES )
    lstrcpy( limit_switch_pos[i], sorc);
}

/***********************************************************************
*                              MIN_STROKE_LEN                          *
***********************************************************************/
float SURETRAK_SETUP_CLASS::min_stroke_len( void )
{
int32 slen;
float x;

x = 0.0;
slen = partdata[MIN_STROKE_LEN_INDEX].len();
if ( slen > 0 )
    x = extfloat( partdata[MIN_STROKE_LEN_INDEX].text(), slen );

return x;
}

/***********************************************************************
*                             VELOCITY_SETPOINT                        *
***********************************************************************/
float SURETRAK_SETUP_CLASS::velocity_setpoint( void )
{
int32 slen;
float x;

x = 0.0;
slen = partdata[VELOCITY_SETPOINT_INDEX].len();
if ( slen > 0 )
    x = extfloat( partdata[VELOCITY_SETPOINT_INDEX].text(), slen );

return x;
}

/***********************************************************************
*                             PRESSURE_SETPOINT                        *
***********************************************************************/
float SURETRAK_SETUP_CLASS::pressure_setpoint( void )
{
int32 slen;
float x;

x = 0.0;
slen = partdata[PRESSURE_SETPOINT_INDEX].len();
if ( slen > 0 )
    x = extfloat( partdata[PRESSURE_SETPOINT_INDEX].text(), slen );

return x;
}
