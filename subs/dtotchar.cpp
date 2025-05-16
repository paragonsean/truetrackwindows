#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

const TCHAR NullChar = '\0';
const TCHAR PunctChar = '.';
const TCHAR MinusChar = '-';
const TCHAR ZeroChar  = '0';

const TCHAR ZeroPointZero[] = TEXT( "0.0" );

/***********************************************************************
*                            DOUBLE_TO_TCHAR                           *
***********************************************************************/
bool double_to_tchar( TCHAR * dest, double x, int max_decimal_places )
{
const double POINT_5 = 0.5;
const double ZERO    = 0.0;

int32 left;
int32 right;
int32 place;
TCHAR * cp;
TCHAR * dp;

if ( !dest )
    return false;

double increment = 1.0;
for ( place=0; place<max_decimal_places; place++ )
    increment /= 10.0;

dp = dest;

if ( x < ZERO )
    {
    *dp++ = MinusChar;
    x = -x;
    }

x = round( x, increment );

/*
-----------------------
Get the integer portion
----------------------- */
left = (int32) x;
if ( left > 0 )
    x -= (double) left;

for ( place=0; place<max_decimal_places; place++ )
    x *= 10.0;

right = (int32) x;

if ( left == 0 && right == 0 )
    {
    copystring( dest, ZeroPointZero );
    return true;
    }

int32toasc( dp, left, DECIMAL_RADIX );
if ( max_decimal_places > 0 )
    {
    dp += lstrlen( dp );
    *dp++ = PunctChar;

    int32toasc( dp, right, DECIMAL_RADIX );
    rjust( dp, short(max_decimal_places), ZeroChar );

    cp = dp + lstrlen(dp) - 1;
    while ( cp > dp )
        {
        if ( *cp != ZeroChar )
            break;
        *cp-- = NullChar;
        }
    }

return true;
}

/***********************************************************************
*                       ROUNDED_DOUBLE_TO_TCHAR                        *
***********************************************************************/
bool rounded_double_to_tchar( TCHAR * dest, double x )
{
int i;

i = (int) (x + 0.5);
if ( i < 0 )
    i = -i;
if ( i < 1 )
    i = 5;
else if ( i < 10 )
    i = 4;
else if ( i < 100 )
    i = 3;
else if ( i < 1000 )
    i = 2;
else if ( i < 10000 )
    i = 1;
else
    i = 0;

return double_to_tchar( dest, x, i );
}
