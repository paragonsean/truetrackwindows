#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\nameclas.h"
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

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
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
NAME_CLASS  s;

s.get_stparam_file_name( computer_name );

if ( !s.file_exists() )
    return FALSE;

if ( f.open_for_read(s) )
    {
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

    /*
    -----------------------------------------------
    I only have room in the file for 4 descriptions
    ----------------------------------------------- */
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

    /*
    -------------------------------
    Empty the last two descriptions
    ------------------------------- */
    for ( i=MAX_ST_LIMIT_SWITCHES; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        desc[i][0] = NullChar;

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
    }

return TRUE;
}

/***********************************************************************
*                                SAVE                                  *
***********************************************************************/
BOOLEAN SURETRAK_PARAM_CLASS::save( TCHAR * computer_name )
{
TCHAR buf[MAX_DOUBLE_LEN+1];
int32       i;
FILE_CLASS  f;
NAME_CLASS  s;

s.get_stparam_file_name( computer_name );
if ( !f.open_for_write(s) )
    return FALSE;

for ( i=0; i<NOF_INT_PARAMS; i++ )
    {
    int32toasc( buf, idata[i], DECIMAL_RADIX );
    f.writeline( buf );
    }

ultoascii( buf, (unsigned long) velocity_dividend_value, DECIMAL_RADIX );
f.writeline( buf );

for ( i=0; i<NOF_FLOAT_PARAMS; i++ )
    {
    ftoasc( buf, fdata[i] );
    f.writeline( buf );
    }

for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
    f.writeline( desc[i] );

int32toasc( buf, vacuum_limit_switch, DECIMAL_RADIX );
f.writeline( buf );

f.close();

return TRUE;
}

/***********************************************************************
*                          LIMIT_SWITCH_DESC                           *
***********************************************************************/
TCHAR * SURETRAK_PARAM_CLASS::limit_switch_desc( int32 i )
{
if ( i < 0 || i >= MAX_FTII_LIMIT_SWITCHES )
    i = 0;
return desc[i];
}

/***********************************************************************
*                        SET_LIMIT_SWITCH_DESC                         *
***********************************************************************/
void SURETRAK_PARAM_CLASS::set_limit_switch_desc( int32 i, TCHAR * sorc )
{
if ( i >= 0 && i < MAX_FTII_LIMIT_SWITCHES )
    {
    lstrcpyn( desc[i], sorc, DESC_LEN+1 );
    desc[i][DESC_LEN] = NullChar;
    }
}
