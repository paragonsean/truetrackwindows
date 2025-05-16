#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static const TCHAR NineChar  = TEXT( '9' );
static const TCHAR SpaceChar = TEXT( ' ' );
static const TCHAR ZeroChar  = TEXT( '0' );
static const TCHAR DecimalPointChar  = TEXT( '.' );


/***********************************************************************
*                         POST_IMPACT_SECONDS                          *
***********************************************************************/
void post_impact_seconds( TCHAR * dest, short n, short ms_per_sample )
{

long seconds;
long fraction;

fraction = long(n) * long(ms_per_sample);
seconds  = fraction/1000L;

fraction -= seconds*1000L;
fraction /= 10L;

insalph( dest, seconds, 2, SpaceChar, DECIMAL_RADIX );
dest += 2;
*dest = '.';
dest++;
insalph( dest, fraction, 2, ZeroChar, DECIMAL_RADIX );
}

/***********************************************************************
*                        LONG ASCII_TO_TOTAL_MS                        *
***********************************************************************/
static long ascii_to_total_ms( TCHAR * sorc )
{

TCHAR * dp;
long   fraction;
long   seconds;

seconds  = extlong( sorc, lstrlen(sorc) );
fraction = 0L;

dp =  findchar( DecimalPointChar, sorc );
if ( dp )
    {
    dp++;
    if ( *dp )
        {
        fraction = 10L * extlong( dp, 2 );
        dp++;
        if ( *dp < ZeroChar || *dp > NineChar )
            fraction *= 10L;
        }
    }

return seconds * 1000L + fraction;

}

/***********************************************************************
*                           POST_IMPACT_N_MS                           *
***********************************************************************/
void post_impact_n_ms( short * dest_n, short * dest_ms, TCHAR * sorc )
{

long n;
long ms;
long total_ms;

*dest_n = 1;
*dest_ms = 1.0;

total_ms = ascii_to_total_ms( sorc );

if ( total_ms <= 1L )
    return;

ms = 1;
while ( MAX_TIME_SAMPLES * ms < total_ms )
    ms++;

n = total_ms / ms;

while ( n*ms < total_ms )
    n++;

*dest_n  = short( n );
*dest_ms = short( ms );

}


/***********************************************************************
*                            POST_IMPACT_N                             *
*   This is used when the operator selects a number of seconds to      *
*   display. It returns the number of samples.                         *
***********************************************************************/
short post_impact_n( short true_ms, TCHAR * sorc )
{

long n;
long ms;
long total_ms;

total_ms = ascii_to_total_ms( sorc );
ms       = long( true_ms );

if ( total_ms <= 1L )
    return 1;

n = total_ms /ms;

while ( n*ms < total_ms )
    n++;

return short(n);
}

