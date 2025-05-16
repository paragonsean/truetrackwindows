#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

static const TCHAR NullChar    = TEXT( '\0' );
static const TCHAR SpaceChar   = TEXT( ' '  );
static const TCHAR EqualsChar  = TEXT( '='  );
static const TCHAR HChar       = TEXT( 'H'  );
static const TCHAR UChar       = TEXT( 'U'  );
static const TCHAR VChar       = TEXT( 'V'  );
static const TCHAR PeriodChar  = TEXT( '.'  );
static const TCHAR PoundChar   = TEXT( '#'  );
static const TCHAR TabChar     = TEXT( '\t' );

/***********************************************************************
*                            REPLACE_VALUE                             *
***********************************************************************/
STRING_CLASS & replace_value( TCHAR * sorc, TCHAR * new_value )
{
static STRING_CLASS s;
TCHAR   c;
TCHAR * cp;

cp = findchar( EqualsChar, sorc );
if ( cp )
    {
    cp++;
    c = *cp;
    *cp = NullChar;
    s = sorc;
    s += new_value;
    *cp = c;
    while ( *cp != NullChar )
        {
        if ( *cp == SpaceChar || *cp == PoundChar || *cp == TabChar )
            break;
        cp++;
        }

    if ( *cp != NullChar )
        s += cp;
    }

return s;
}

/***********************************************************************
*                            REPLACE_VALUE                             *
***********************************************************************/
STRING_CLASS & replace_value( TCHAR * sorc, int new_value )
{
return replace_value( sorc, int32toasc((int32) new_value) );
}

/***********************************************************************
*                            REPLACE_VALUE                             *
***********************************************************************/
STRING_CLASS & replace_value( TCHAR * sorc, unsigned new_value )
{
return replace_value( sorc, ultoascii((unsigned long) new_value) );
}

/***********************************************************************
*                         REPLACE_PERCENT_VALUE                        *
***********************************************************************/
STRING_CLASS & replace_percent_value( TCHAR * sorc, double new_value, BOOLEAN need_bit_30 )
{
static TCHAR setbit30and31[] = TEXT( "|HC0000000" );
static TCHAR setbit31[] = TEXT( "|H80000000" );
const int SLEN = sizeof(setbit31)/sizeof(TCHAR);

TCHAR    s[MAX_INTEGER_LEN + SLEN + 1];
TCHAR  * cp;
unsigned u;

new_value *= 100.0;
u = (unsigned) round( new_value, 1.0 );

cp = copystring( s, ultoascii((unsigned long) u) );
if ( need_bit_30 )
    copystring( cp, setbit30and31 );
else
    copystring( cp, setbit31 );

return replace_value( sorc, s );
}

/***********************************************************************
*                         REPLACE_PERCENT_VALUE                        *
***********************************************************************/
STRING_CLASS & replace_percent_value( TCHAR * sorc, float new_value, BOOLEAN need_bit_30 )
{
return replace_percent_value( sorc, (double) new_value, need_bit_30 );
}

/***********************************************************************
*                         REPLACE_PERCENT_VALUE                        *
***********************************************************************/
STRING_CLASS & replace_percent_value( TCHAR * sorc, double new_value )
{
return replace_percent_value( sorc, new_value, (BOOLEAN) FALSE );
}

/***********************************************************************
*                         REPLACE_PERCENT_VALUE                        *
***********************************************************************/
STRING_CLASS & replace_percent_value( TCHAR * sorc, float new_value )
{
return replace_percent_value( sorc, (double) new_value, (BOOLEAN) FALSE );
}

/***********************************************************************
*                           VARIABLE_NUMBER                            *
***********************************************************************/
int variable_number( const TCHAR * sorc )
{
/*
-------------------------------------------------------------------------------
If this is a control variable, the line begins with .U before the V. Skip this.
------------------------------------------------------------------------------- */
if ( *sorc == PeriodChar )
    {
    sorc++;
    if ( *sorc == UChar )
        sorc++;
    else
        return -1;
    }

if ( *sorc == VChar )
    {
    sorc++;
    if ( is_numeric(*sorc) )
        return (int) asctoint32( sorc );
    }

return -1;
}

/***********************************************************************
*                           VARIABLE_VALUE                             *
***********************************************************************/
unsigned variable_value( const TCHAR * sorc )
{
const TCHAR * cp;

cp = findchar( EqualsChar, sorc );
if ( cp )
    {
    cp++;
    if ( *cp == HChar )
        {
        cp++;
        return (unsigned) hextoul( cp );
        }

    return (unsigned) asctoul( cp );
    }

return 0;
}

/***********************************************************************
*                        SIGNED_VARIABLE_VALUE                         *
***********************************************************************/
int signed_variable_value( TCHAR * sorc )
{
TCHAR * cp;

cp = findchar( EqualsChar, sorc );
if ( cp )
    {
    cp++;
    if ( *cp == HChar )
        {
        cp++;
        return (int) hextoul( cp );
        }

    return (int) asctoint32( cp );
    }

return 0;
}

