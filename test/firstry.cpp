#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
TCHAR * getline( void )
{
static TCHAR buf[103];
buf[0] = 100;

cgets( buf );

return buf+2;
}


/***********************************************************************
*                             GET_SAMPLE_TIMES                         *
***********************************************************************/
double * get_sample_times( int & nof_points, double start_time, double end_time, int n_wanted )
{
const  double Ck = 100000.0;
const  int    Ick = 100000;
double * p;
double   dt;
double   multiplier;
int      d;
int      n;
int      t;
int      t2;

/*
--------------------------------------------------------------------------
Calculate the multiplier to convert delta time in seconds to 10^-5 seconds
-------------------------------------------------------------------------- */
multiplier = double( Ick/n_wanted )
dt = end_time - start_time;
n  = 0;
d = int( dt * multiplier + 0.5 );
while ( d >= 10 )
    {
    n++;
    d /= 10;
    }

if ( d >= 5 )
    d = 5;
else if ( d >= 2 )
    d = 2;
else
    d = 1;

while ( n )
    {
    d *= 10;
    n--;
    }

t  = int( start_time * Ck );
n  = t/d;
t2 = n * d;
while ( t2 < t )
    t2 += d;
t = t2;

/*
-----------------------------------------------------
t is now the first sample greater than the start time
----------------------------------------------------- */
t2 = int( end_time * Ck );
n  = (t2 - t )/d;

nof_points = 0;

p = 0;
if ( n <= 0 )
    return p;

p = new double[n];
if ( !p )
    return p;

for ( i=0; i<n; i++ )
    {
    p[i] = double(t) * Ck;
    t += d;
    }

nof_points = n;
return p;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
static TCHAR NullChar = TEXT( '\0' );
const  double Ck = 100000.0;
int    d;
int    n;
double dt;
double t1;
double t2;
TCHAR * cp;

while ( true )
    {
    cprintf( "\n\rInput the start time in seconds: " );
    cp = getline();
    cprintf( "\n\r" );
    if ( *cp == NullChar )
        break;

    t1 = extdouble( cp );

    cprintf( "\n\rInput the end time in seconds: " );
    cp = getline();
    cprintf( "\n\r" );
    if ( *cp == NullChar )
        break;

    t2 = extdouble( cp );


    dt = t2 - t1;
    n  = 0;
    d = int( dt * 200.0 + 0.5 );
    while ( d >= 10 )
        {
        n++;
        d /= 10;
        }

    if ( d >= 5 )
        d = 5;
    else if ( d >= 2 )
        d = 2;
    else
        d = 1;

    while ( n )
        {
        d *= 10;
        n--;
        }

    dt = double( d );
    dt /= Ck;
    cprintf( "\n\rThe increment is [%s]", ascii_double( dt) );
    }

cprintf( "\n\r*** Done ***\n\r" );

return 0;
}

