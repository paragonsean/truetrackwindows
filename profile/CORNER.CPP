#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\iniclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "extern.h"

static TCHAR CommaChar = TEXT( ',' );
static TCHAR NullChar  = TEXT( '\0' );
static const TCHAR CornerParamsKey[] = TEXT( "CornerParams" );

/***********************************************************************
*                        READ_CORNER_PARAMETERS                        *
***********************************************************************/
void read_corner_parameters( int * dest, int & n )
{
INI_CLASS inifile;
TCHAR * cp;
bool  finished;

finished = false;
cp = 0;

if ( PartBasedDisplay )
    {
    inifile.set_file( part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name) );
    if ( inifile.find(ConfigSection, CornerParamsKey) )
        cp = inifile.get_string();
    }

if ( !cp )
    {
    cp = get_ini_string( DisplayIniFile, ConfigSection, CornerParamsKey );
    }

n = 0;
while ( n < (MAX_PARMS - 1) )
    {
    if ( *cp == NullChar )
        break;

    if ( !replace_char_with_null(cp, CommaChar) )
        finished = true;

    *dest = (int) asctoint32( cp );
    n++;

    if ( finished )
        break;

    cp = nextstring( cp );

    dest++;
    }

}

/***********************************************************************
*                        SAVE_CORNER_PARAMETERS                        *
***********************************************************************/
void save_corner_parameters( int * sorc, int n )
{
INI_CLASS inifile;
TCHAR buf[MAX_LONG_INI_STRING_LEN+1];
TCHAR * cp;
int   i;

cp = buf;
for ( i=0; i<n; i++ )
    {
    cp = copystring( cp, int32toasc((int32) sorc[i]) );
    *cp++ = CommaChar;
    }

cp--;
*cp = NullChar;

if ( PartBasedDisplay )
    {
    inifile.set_file( part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name) );
    inifile.put_string( ConfigSection, CornerParamsKey, buf );
    }

/*
-----------------------------------
Save it in the default file as well
----------------------------------- */
put_ini_string( DisplayIniFile, ConfigSection, CornerParamsKey, buf );
}
