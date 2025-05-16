#include <windows.h>
#include <math.h>
#include <stdlib.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static const TCHAR NullChar         = TEXT( '\0' );
static const TCHAR DecimalPointChar = TEXT( '.' );
static const TCHAR ZeroChar         = TEXT( '0' );
static const TCHAR NineChar         = TEXT( '9' );
static const TCHAR SpaceChar        = TEXT( ' ' );
static const TCHAR MinusChar        = TEXT( '-' );
static const TCHAR ZeroPointZero[]  = TEXT( "0.0" );

/***********************************************************************
*                              FIXED_STRING                            *
***********************************************************************/
TCHAR * fixed_string( double x, int32 left_places, int32 right_places )
{
static TCHAR s[MAX_DOUBLE_LEN+1];
int32 i;
int32 n;
int32 dn;
TCHAR * cp;
int32 nmax;

cp = s;
if ( x < 0.0 )
    {
    *cp++ = MinusChar;
    x = -x;
    }

n  = (int32) floor( x );
x -= (double) n;

nmax = 1;
for ( i=0; i<right_places; i++ )
    {
    x *= 10.0;
    nmax *= 10;
    }

x = round( x, 1.0 );

dn = (int32) x;
if ( dn >= nmax )
    {
    n++;
    dn -= nmax;
    }

insalph( cp, n, left_places, SpaceChar, DECIMAL_RADIX );
cp += left_places;
*cp = DecimalPointChar;
cp++;

insalph( cp, dn, right_places, ZeroChar, DECIMAL_RADIX );
cp += right_places;
*cp = NullChar;

/*
-----------------------------------------------------
Move the minus sign up to just before the first digit
----------------------------------------------------- */
cp = s;
if ( *cp == MinusChar )
    {
    *cp = SpaceChar;
    cp++;
    while ( *cp == SpaceChar )
        cp++;
    cp--;
    *cp = MinusChar;
    }

return s;
}

/***********************************************************************
*                            CHAR_TO_TCHAR                             *
***********************************************************************/
void char_to_tchar( TCHAR * dest, char * sorc, int max_len )
{

if ( !dest )
    return;

if ( sorc )
    {
    while ( *sorc )
        {
        *dest = (TCHAR) (*sorc);
        dest++;
        sorc++;

        max_len--;
        if ( !max_len )
            break;
        }
    }

*dest = NullChar;
}

/***********************************************************************
*                                FTOASC                                *
***********************************************************************/
static void ftoasc( TCHAR * dest, double x, int width )
{

char  * p;
TCHAR * sorc;
TCHAR   buf[MAX_DOUBLE_LEN+1];

int pos;
int sign;

if ( x < 0.00001 && x > -0.00001 )
    {
    lstrcpy( dest, ZeroPointZero );
    return;
    }

p = ecvt( x, width, &pos, &sign );
char_to_tchar( buf, p, MAX_DOUBLE_LEN );

sorc = buf;

if ( sign )
    *dest++ = MinusChar;

if ( pos <= 0 )
    {
    *dest++ = DecimalPointChar;
    while ( pos < 0 )
        {
        *dest++ = ZeroChar;
        pos++;
        }
    }

while ( width )
    {
    if ( !(*sorc) )
        break;
    *dest++ = *sorc++;
    pos--;
    if ( pos == 0 )
        *dest++ = DecimalPointChar;
    width--;
    }

if ( pos > 0 )
    {
    while ( pos > 0 )
        {
        *dest++ = ZeroChar;
        pos--;
        }
    *dest++ = DecimalPointChar;
    }
*dest = NullChar;
}

/***********************************************************************
*                                FTOASC                                *
***********************************************************************/
void ftoasc( TCHAR * dest, float x )
{
ftoasc( dest, double(x), 7 );
}

/***********************************************************************
*                                FTOASC                                *
***********************************************************************/
void ftoasc( TCHAR * dest, double x )
{
ftoasc( dest, x, 15 );
}
