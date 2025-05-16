#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\ftii.h"
#include "..\include\iniclass.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\stparam.h"

static const TCHAR NullChar = TEXT( '\0' );

/***********************************************************************
*                         SURETRAK_PARAM_CLASS                         *
***********************************************************************/
SURETRAK_PARAM_CLASS::SURETRAK_PARAM_CLASS()
{
int32 i;


idata[LVDT_DAC_OFFSET_INDEX]        = 0;
idata[NULL_DAC_OFFSET_INDEX]        = 0;

velocity_dividend_value             = 4294967295UL;

fdata[ZERO_SPEED_CHECK_POS_INDEX]   = 22;
fdata[COUNTS_PER_UNIT_INDEX]        = 80;
fdata[JOG_VALVE_PERCENT_INDEX]      = 9.0;
fdata[RETRACT_VALVE_PERCENT_INDEX]  = -100.0;
fdata[FT_VALVE_PERCENT_INDEX]       = 12.0;
fdata[VEL_MULTIPLIER_INDEX]         = 2.86;
fdata[MIN_LOW_IMPACT_PERCENT_INDEX] = 5.0;
fdata[MAX_VELOCITY_INDEX]           = 100.0;

for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
    desc[i][0] = NullChar;

vacuum_limit_switch = 0;
}

/***********************************************************************
*                         ~SURETRAK_PARAM_CLASS                        *
***********************************************************************/
SURETRAK_PARAM_CLASS::~SURETRAK_PARAM_CLASS()
{

}

/***********************************************************************
*                                FIND                                  *
***********************************************************************/
BOOLEAN SURETRAK_PARAM_CLASS::find( TCHAR * computer_name )
{
int32       i;
FILE_CLASS  f;
TCHAR     * cp;
TCHAR     * fname;
DWORD   status;

fname = stparam_filename( computer_name );

if ( !file_exists(fname) )
    return FALSE;

if ( !f.open_for_read(fname) )
    {
    if ( file_exists(fname) )
        {
        status = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            f.error(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &cp,
            0,
            NULL
            );

        if ( status )
            {
            MessageBox( NULL, cp, fname, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL );
            LocalFree( cp );
            }
        }

    return FALSE;
    }

for ( i=0; i<NOF_INT_PARAMS; i++ )
    {
    cp = f.readline();
    if ( cp )
        idata[i] = asctoint32( cp );
    }

cp = f.readline();
if ( cp )
    velocity_dividend_value = asctoul( cp );

for ( i=0; i<NOF_FLOAT_PARAMS; i++ )
    {
    cp = f.readline();
    if ( cp )
        fdata[i] = extfloat( cp );
    }

for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
    {
    cp = f.readline();
    if ( cp )
        {
        if ( lstrlen(cp) > DESC_LEN )
            *(cp+DESC_LEN) = NullChar;
        lstrcpy( desc[i], cp );
        }
    }

cp = f.readline();
if ( cp )
    {
    vacuum_limit_switch = asctoint32( cp );
    }
else
    {
    /*
    -----------------------------------------------------------
    This is probably an old file with only four limit switches.
    Try the name for limit switch 5
    ----------------------------------------------------------- */
    i = MAX_ST_LIMIT_SWITCHES - 1;
    if ( is_numeric(desc[i][0]) )
        {
        i = asctoint32( desc[i] );
        if ( i >= 0 && i < MAX_ST_LIMIT_SWITCHES )
            vacuum_limit_switch = i;
        }
    }

f.close();

return TRUE;
}

/***********************************************************************
*                                FIND                                  *
*                Get the settings from the ftii files                  *
***********************************************************************/
BOOLEAN SURETRAK_PARAM_CLASS::find( TCHAR * computer_name, TCHAR * machine_name )
{
int32       i;
FILE_CLASS  f;
INI_CLASS   ini;
TCHAR     * cp;
TCHAR     * fname;
DWORD   status;
FTII_VAR    va;

fname = ftii_editor_settings_name( computer_name, machine_name );
if ( !file_exists(fname) )
    return FALSE;

ini.set_file( fname );
ini.set_section( ConfigSection );

lstrcpy( desc[0], ini.get_string( LS1DescriptionKey) );
lstrcpy( desc[1], ini.get_string( LS2DescriptionKey) );
lstrcpy( desc[2], ini.get_string( LS3DescriptionKey) );
lstrcpy( desc[3], ini.get_string( LS4DescriptionKey) );

vacuum_limit_switch                 = ini.get_int( VacuumLimitSwitchKey );
fdata[MIN_LOW_IMPACT_PERCENT_INDEX] = (float) ini.get_double( MinLowImpactPercentKey) );
fdata[MAX_VELOCITY_INDEX]           = (float) ini.get_double( MaxVelocityKey)         );


fname = ftii_global_settings_name( computer_name, machine_name );
if ( file_exists(fname) )
    {
    if ( f.open_for_read(fname) )
        {
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;
            va.get( cp );

            switch ( va.vn )
                {
                case


                }


        }
    }


#define NULL_DAC_OFFSET_VN  int(350)
const int32 NULL_DAC_OFFSET_INDEX   = 0;

#define LVDT_DAC_OFFSET_VN  int(351)
const int32 LVDT_DAC_OFFSET_INDEX   = 1;

#define ZERO_SPEED_CHECK_VN int(349)
const int32 ZERO_SPEED_CHECK_POS_INDEX   = 0;

const int32 COUNTS_PER_UNIT_INDEX        = 1;

#define JOG_SHOT_SETTING_VN int(352)
const int32 JOG_VALVE_PERCENT_INDEX      = 2;

#define RETRACT_SETTING_VN  int(355)
const int32 RETRACT_VALVE_PERCENT_INDEX  = 3;

#define FT_SETTING_VN       int(353)
const int32 FT_VALVE_PERCENT_INDEX       = 4;

const int32 VEL_MULTIPLIER_INDEX         = 5;




set_text( w, NULL_DAC_EDITBOX,       int32toasc((int32) GlobalParameters[NULL_DAC_INDEX].ival()) );
set_text( w, LVDT_DAC_EDITBOX,       int32toasc((int32) GlobalParameters[LVDT_DAC_INDEX].ival()) );

set_text( w, JOG_SHOT_VALVE_EDITBOX, percent_from_mv(GlobalParameters[JOG_SHOT_VALVE_INDEX].ival()) );
set_text( w, FT_VALVE_EDITBOX,       percent_from_mv(GlobalParameters[FT_VALVE_INDEX].ival())       );
set_text( w, RETRACT_VALVE_EDITBOX,  percent_from_mv(GlobalParameters[RETRACT_VALVE_INDEX].ival())  );

set_text( w, ZERO_SPEED_CHECK_EDITBOX, ascii_double( ThisPart.dist_from_x4(GlobalParameters[ZERO_SPEED_CHECK_INDEX].realval())) );
set_text( w, GLOBAL_DIST_UNITS_XBOX, units_name(ThisPart.distance_units) );


const int32 NOF_INT_PARAMS   = 2;
const int32 NOF_FLOAT_PARAMS = 8;

const int32 NULL_DAC_OFFSET_INDEX   = 0;
const int32 LVDT_DAC_OFFSET_INDEX   = 1;
const int32 ZERO_SPEED_CHECK_POS_INDEX   = 0;
const int32 JOG_VALVE_PERCENT_INDEX      = 2;
const int32 RETRACT_VALVE_PERCENT_INDEX  = 3;
const int32 FT_VALVE_PERCENT_INDEX       = 4;


const int32 VEL_MULTIPLIER_INDEX         = 5;
const int32 COUNTS_PER_UNIT_INDEX        = 1;
const int32 MIN_LOW_IMPACT_PERCENT_INDEX = 6;
const int32 MAX_VELOCITY_INDEX           = 7;








if ( !f.open_for_read(fname) )
    {
    if ( file_exists(fname) )
        {
        status = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            f.error(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &cp,
            0,
            NULL
            );

        if ( status )
            {
            MessageBox( NULL, cp, fname, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL );
            LocalFree( cp );
            }
        }

    return FALSE;
    }

for ( i=0; i<NOF_INT_PARAMS; i++ )
    {
    cp = f.readline();
    if ( cp )
        idata[i] = asctoint32( cp );
    }

cp = f.readline();
if ( cp )
    velocity_dividend_value = asctoul( cp );

for ( i=0; i<NOF_FLOAT_PARAMS; i++ )
    {
    cp = f.readline();
    if ( cp )
        fdata[i] = extfloat( cp );
    }

for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
    {
    cp = f.readline();
    if ( cp )
        {
        if ( lstrlen(cp) > DESC_LEN )
            *(cp+DESC_LEN) = NullChar;
        lstrcpy( desc[i], cp );
        }
    }

cp = f.readline();
if ( cp )
    {
    vacuum_limit_switch = asctoint32( cp );
    }
else
    {
    /*
    -----------------------------------------------------------
    This is probably an old file with only four limit switches.
    Try the name for limit switch 5
    ----------------------------------------------------------- */
    i = MAX_ST_LIMIT_SWITCHES - 1;
    if ( is_numeric(desc[i][0]) )
        {
        i = asctoint32( desc[i] );
        if ( i >= 0 && i < MAX_ST_LIMIT_SWITCHES )
            vacuum_limit_switch = i;
        }
    }

f.close();

return TRUE;
}

/***********************************************************************
*                                SAVE                                  *
***********************************************************************/
BOOLEAN SURETRAK_PARAM_CLASS::save( TCHAR * computer_name )
{
int32       i;
FILE_CLASS  f;

if ( !f.open_for_write(stparam_filename(computer_name)) )
    return FALSE;

for ( i=0; i<NOF_INT_PARAMS; i++ )
    f.writeline( int32toasc(idata[i]) );

f.writeline( ultoascii(velocity_dividend_value) );

for ( i=0; i<NOF_FLOAT_PARAMS; i++ )
    f.writeline( ascii_float(fdata[i]) );

for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
    f.writeline( desc[i] );

f.writeline( int32toasc(vacuum_limit_switch) );

f.close();

return TRUE;
}

/***********************************************************************
*                          LIMIT_SWITCH_DESC                           *
***********************************************************************/
TCHAR * SURETRAK_PARAM_CLASS::limit_switch_desc( int32 i )
{
if ( i < 0 || i >= MAX_ST_LIMIT_SWITCHES )
    i = 0;
return desc[i];
}

/***********************************************************************
*                        SET_LIMIT_SWITCH_DESC                         *
***********************************************************************/
void SURETRAK_PARAM_CLASS::set_limit_switch_desc( int32 i, TCHAR * sorc )
{
if ( i >= 0 && i < MAX_ST_LIMIT_SWITCHES )
    {
    lstrcpyn( desc[i], sorc, DESC_LEN+1 );
    desc[i][DESC_LEN] = NullChar;
    }
}

