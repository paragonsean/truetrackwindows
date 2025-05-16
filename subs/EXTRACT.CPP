#include <windows.h>
#include <stdlib.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static TCHAR NullChar     = TEXT( '\0' );
static TCHAR MinusChar    = TEXT( '-' );
static TCHAR PlusChar     = TEXT( '+' );
static TCHAR DotChar      = TEXT( '.' );
static TCHAR SpaceChar    = TEXT( ' ' );
static TCHAR ZeroChar     = TEXT( '0' );
static TCHAR NineChar     = TEXT( '9' );

/***********************************************************************
*                             IS_REAL_CHAR                             *
***********************************************************************/
BOOLEAN is_real_char( TCHAR c )
{
if ( ZeroChar <= c && c <= NineChar )
    return TRUE;

if ( c == MinusChar || c == PlusChar || c == DotChar )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              NUMBERLEN                               *
***********************************************************************/
static short numberlen( const TCHAR * sorc )
{
const TCHAR * cp;
short slen;

slen = 0;
cp = sorc;

/*
---------------------------------------------------------
Skip leading spaces (sometimes these come after the sign)
--------------------------------------------------------- */
while ( *cp )
    {
    if ( (*cp != MinusChar) && (*cp != PlusChar) && (*cp != SpaceChar) )
        break;
    slen++;
    cp++;
    }

while ( *cp )
    {
    if ( slen >= MAX_DOUBLE_LEN )
        break;

    if ( !is_numeric(*cp) && (*cp != MinusChar) && (*cp != PlusChar) && (*cp != DotChar) )
        break;

    slen++;
    cp++;
    }

return slen;
}

/***********************************************************************
*                              EXTDOUBLE                               *
***********************************************************************/
double extdouble( const TCHAR * sorc, short nofchars )
{

TCHAR sc[30];
char  save[60];
double x;

if ( nofchars > 29 )
    nofchars = 29;

x = 0.0;

if ( nofchars > 0 )
    {
    lstrcpyn( sc, sorc, nofchars+1 );
    CharToOem( sc, save );
    x = strtod( save, NULL );
    }

return x;
}

/***********************************************************************
*                              EXTDOUBLE                               *
***********************************************************************/
double extdouble( const TCHAR * sorc )
{
return extdouble( sorc, (short) lstrlen(sorc) );
}

/***********************************************************************
*                               EXTFLOAT                               *
***********************************************************************/
float extfloat( const TCHAR * sorc, short nofchars )
{
return (float) extdouble( sorc, nofchars );
}

/***********************************************************************
*                               EXTFLOAT                               *
***********************************************************************/
float extfloat( const TCHAR * sorc )
{
return (float) extdouble( sorc, numberlen(sorc) );
}

/***********************************************************************
*                               EXTLONG                                *
***********************************************************************/
long extlong( const TCHAR * sorc, short nofchars, int radix )
{
TCHAR sc[MAX_INTEGER_LEN+1];
char  save[MAX_INTEGER_LEN+1];
long  x;

x = 0;
if ( nofchars > MAX_INTEGER_LEN )
    nofchars = MAX_INTEGER_LEN;

if ( nofchars > 0 )
    {
    lstrcpyn( sc, sorc, nofchars+1 );
    CharToOem( sc, save );
    x = strtol( save, NULL, radix );
    }

return x;
}

/***********************************************************************
*                               EXTUL                                  *
***********************************************************************/
unsigned long extul( const TCHAR * sorc, short nofchars, int radix )
{
TCHAR sc[MAX_INTEGER_LEN+1];
char  save[MAX_INTEGER_LEN+1];
unsigned long x;

x = 0;
if ( nofchars > MAX_INTEGER_LEN )
    nofchars = MAX_INTEGER_LEN;

if ( nofchars > 0 )
    {
    lstrcpyn( sc, sorc, nofchars+1 );
    CharToOem( sc, save );
    x = strtoul( save, NULL, radix );
    }

return x;
}

/***********************************************************************
*                              ASCTOUL                                 *
***********************************************************************/
unsigned long asctoul( const TCHAR * sorc )
{
return extul( sorc, numberlen(sorc), DECIMAL_RADIX );
}

/***********************************************************************
*                              ASCTOINT32                              *
***********************************************************************/
int32 asctoint32( const TCHAR * sorc )
{
return (int32) extlong( sorc, numberlen(sorc), DECIMAL_RADIX );
}

/***********************************************************************
*                               EXTLONG                                *
***********************************************************************/
long extlong( const TCHAR * sorc, short nofchars )
{
return extlong( sorc, nofchars, DECIMAL_RADIX );
}

/***********************************************************************
*                               EXTSHORT                               *
***********************************************************************/
short extshort( const TCHAR * sorc, short nofchars )
{
return (short) extlong( sorc, nofchars, DECIMAL_RADIX  );
}

/***********************************************************************
*                               EXTFIELD                               *
*                                                                      *
*  maxlen is not counting the null.                                    *
*  return a pointer to the next field.                                 *
***********************************************************************/
TCHAR * extfield( TCHAR * dest, TCHAR * sorc, TCHAR delimiter, int32 maxlen )
{

while ( TRUE )
    {
    if ( *sorc == delimiter || *sorc == NullChar )
        break;

    if ( maxlen > 0 )
        {
        *dest = *sorc;
        dest++;
        }

    sorc++;
    maxlen--;
    }

*dest = NullChar;

/*
-----------------------------------------------
Return a pointer to the start of the next field
----------------------------------------------- */
if ( *sorc != NullChar )
    sorc++;

return sorc;
}

