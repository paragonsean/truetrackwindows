#include <windows.h>

/***********************************************************************
*  Note: The ith entry is the one for the ith parameter.               *
*  There is room for all the parameters but, obviously, only the       *
*  ones that are ftanalog parameters are used for anything.            *
***********************************************************************/
#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\ftanalog.h"
#include "..\include\verrors.h"

static TCHAR CommaChar     = TEXT(',');
static TCHAR EmptyString[] = TEXT("");
static TCHAR CommaString[] = TEXT(",");

/***********************************************************************
*                         FTANALOG_CLASS::INIT                         *
***********************************************************************/
void FTANALOG_CLASS::init( short i )
{
array[i].channel      = NO_FT_CHANNEL;
array[i].ind_var_type = POSITION_VAR;
array[i].result_type  = SINGLE_POINT_RESULT_TYPE;
array[i].ind_value    = EmptyString;
array[i].end_value    = EmptyString;
}

/***********************************************************************
*                        FTANALOG_CLASS::REMOVE                        *
***********************************************************************/
BOOLEAN FTANALOG_CLASS::remove ( short i )
{
short n;

if ( i<0 || i>=MAX_PARMS )
    return FALSE;

n = MAX_PARMS - 1;
while ( i < n)
    {
    array[i] = array[i+1];
    i++;
    }

init( i );
return TRUE;
}

/***********************************************************************
*                                comma_count                           *
***********************************************************************/
static int comma_count( TCHAR * s )
{
int n;
n = 0;
while ( *s )
    {
    if ( *s == CommaChar )
        n++;
    s++;
    }

return n;
}

/***********************************************************************
*                           FTANALOG_CLASS::LOAD                       *
***********************************************************************/
BOOLEAN FTANALOG_CLASS::load( TCHAR * computer_to_find, TCHAR * machine_to_find, TCHAR * part_to_find )
{
int          i;
TCHAR        c;
TCHAR      * s;
STRING_CLASS fn;
FILE_CLASS   f;
DB_TABLE     t;

fn = ftanalog_csvname( computer_to_find, machine_to_find, part_to_find );
if ( fn.isempty() )
    return FALSE;

s = fn.text();
if ( file_exists(s) )
    {
    if ( f.open_for_read(s) )
        {
        while ( true )
            {
            s = f.readline();
            if ( !s )
                break;
            if ( comma_count(s) == 5 )
                {
                i = 0;
                replace_char_with_null( s, CommaChar );
                i = asctoint32( s );
                if ( i > 0 && i <= MAX_PARMS )
                    {
                    i--;
                    s = nextstring( s );
                    replace_char_with_null(s, CommaChar );
                    array[i].channel = (short) asctoint32( s );

                    s = nextstring( s );
                    replace_char_with_null(s, CommaChar );
                    array[i].result_type = (BITSETYPE) asctoul( s );

                    s = nextstring( s );
                    replace_char_with_null(s, CommaChar );
                    array[i].ind_var_type = (BITSETYPE) asctoul( s );

                    s = nextstring( s );
                    replace_char_with_null(s, CommaChar );
                    array[i].ind_value = s;

                    s = nextstring( s );
                    array[i].end_value = s;
                    }
                }
            }
        f.close();
        return TRUE;
        }
    }
else
    {
    fn = ftanalog_dbname( computer_to_find, machine_to_find, part_to_find );
    if ( file_exists(fn.text()) )
        {
        if ( t.open( fn.text(), FTANALOG_RECLEN, PFL) )
            {
            while( t.get_next_record(NO_LOCK) )
                {
                i = (int) t.get_short( FTANALOG_INPUT_NUMBER_OFFSET );
                if ( i > 0 && i <= MAX_PARMS )
                    {
                    i--;
                    array[i].channel = t.get_short( FTANALOG_CHANNEL_NUMBER_OFFSET );
                    c                = t.get_char(  FTANALOG_SAMPLE_TYPE_OFFSET );
                    if ( c == ASCII_POS_IND_VAR )
                        array[i].ind_var_type = POSITION_VAR;
                    else if ( c == ASCII_TIME_IND_VAR )
                        array[i].ind_var_type = TIME_VAR;
                    else if ( c == ASCII_GREATER_THAN_VAR )
                        array[i].ind_var_type = TRIGGER_WHEN_GREATER;
                    else if ( c == ASCII_SMALLER_THAN_VAR )
                        array[i].ind_var_type = TRIGGER_WHEN_SMALLER;
                    else
                        array[i].ind_var_type = NO_VARIABLE_TYPE;
                    array[i].result_type = SINGLE_POINT_RESULT_TYPE;
    
                    array[i].ind_value = ascii_float( t.get_float(FTANALOG_SAMPLE_VALUE_OFFSET) );
                    array[i].end_value = EmptyString;
                    }
                }
    
            t.close();
            if ( t.get_global_error() == VS_SUCCESS )
                return TRUE;
            }
        }
    }

return FALSE;
}

/***********************************************************************
*                          FTANALOG_CLASS::SAVE                        *
***********************************************************************/
BOOLEAN FTANALOG_CLASS::save( TCHAR * computer, TCHAR * machine, TCHAR * part )
{
int          i;
STRING_CLASS s;
FILE_CLASS   f;

s.upsize( 100 );
s = ftanalog_csvname( computer, machine, part );
if ( f.open_for_write(s.text()) )
    {
    for ( i=0; i<MAX_PARMS; i++ )
        {
        if ( array[i].channel == NO_FT_CHANNEL )
            continue;

        s = EmptyString;

        s += i+1;
        s += CommaString;

        s += (int) array[i].channel;
        s += CommaString;

        s += (unsigned) array[i].result_type;
        s += CommaString;

        s += (unsigned) array[i].ind_var_type;
        s += CommaString;

        s.cat_w_char( array[i].ind_value, CommaChar );
        s += array[i].end_value;
        f.writeline( s.text() );
        }
    f.close();
    return TRUE;
    }
return FALSE;
}
