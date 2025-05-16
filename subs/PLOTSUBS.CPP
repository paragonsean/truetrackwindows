#include <windows.h>
#include <math.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

int32 SmoothN      = 0;
float SmoothFactor = 0.0 ;

/***********************************************************************
*                            SET_SMOOTH                                *
***********************************************************************/
void set_smooth( int32 n, float factor )
{

SmoothN = 0;
if ( factor > 0.01 && factor <= 1.0 )
    {
    if ( n > 0 )
        {
        SmoothFactor = factor;
        SmoothN      = n;
        }
    }
}

/***********************************************************************
*                           RUNNING_AVERAGE                            *
***********************************************************************/
float running_average( float * raw, int32 true_index, int32 final_index )
{

int32 i;
int32 my_n;
int32 n;
float * dp;
float sum;
float divisor;
float multiplier;

/*
-----------------------------------------------------
If I'm close to the end, reduce the number to average
----------------------------------------------------- */
my_n = SmoothN;
if ( true_index < my_n )
    my_n = true_index;

n = final_index - true_index;
if ( n < my_n )
    my_n = n;

if ( my_n < 0 )
    my_n = 1;

divisor    = 1.0;
sum        = *raw;

/*
-----------------------------------
Add the points previous to this one
----------------------------------- */
i          = true_index;
dp         = raw;
n          = my_n;
multiplier = 1.0;

while ( n )
    {
    i--;
    if ( i >= 0 )
        {
        dp--;
        multiplier *= SmoothFactor;
        divisor    += multiplier;
        sum        += (*dp) * multiplier;
        }
    n--;
    }

i          = true_index;
dp         = raw;
n          = my_n;
multiplier = 1.0;

while ( n )
    {
    i--;
    if ( i <= final_index )
        {
        dp++;
        multiplier *= SmoothFactor;
        divisor    += multiplier;
        sum        += (*dp) * multiplier;
        }
    n--;
    }

return sum/divisor;
}

/***********************************************************************
*                                PIXEL_EQUATION                        *
*                                                                      *
*  Returns the equation such that y = int32( a + b*x ) is the correct  *
*  pixel position.                                                     *
***********************************************************************/
void pixel_equation( float & a, float & b, int32 min_pixel, int32 max_pixel, float min_value, float max_value )
{

float delta_value;
float ymax;
float ymin;

ymin = float( min_pixel );
ymax = float( max_pixel );

/*
------------------------------------------------------------------------
Adding .5 shifts the curve so truncated floats plot at the correct point
------------------------------------------------------------------------ */
ymin += .5;
ymax += .5;

delta_value = max_value - min_value;
if ( is_float_zero(delta_value) )
    delta_value = 1.0;

b = (ymax - ymin) / delta_value;
a = ymin - min_value*b;

}

/***********************************************************************
*                            WHOLE_PLACES                              *
*   Return the number of whole places needed to display a float.       *
***********************************************************************/
int32 whole_places( float x )
{
int32 n;

n = 1;
if ( x > 1.0 )
    n += (int32) floor( log10(x) );

return n;
}


/***********************************************************************
*                              PLOT_DATA                               *
***********************************************************************/
void plot_data( HDC dc, RECT & r, float min_x, float max_x, float min_y, float max_y, float * xp, float * yp, int32 n )
{

int    i;
int32  j;
int32  last_index;
float   x;
float   xa;
float   xb;

float   y;
float   ya;
float   yb;

BOOLEAN  is_first_point;
BOOLEAN  is_in_bounds;
POINT  * p;

if ( n < 2 )
    return;

pixel_equation( xa, xb, r.left,   r.right,  min_x, max_x );
pixel_equation( ya, yb, r.bottom, r.top,    min_y, max_y );

p = new POINT[n];
if ( !p )
    return;

last_index = n-1;

is_first_point = TRUE;
i = 0;

for ( j=0; j<n; j++ )
    {
    x = *xp;

    /*
    ---------------------------------------------------------
    I used to smooth here but now I do it on the actual data.
    if ( SmoothN > 0 )
        y = running_average( yp, j, last_index );
    else
    --------------------------------------------------------- */
        y = *yp;

    if ( min_x <= x && x <= max_x )
        {
        x *= xb;
        x += xa;

        if ( min_y <= y && y <= max_y )
            is_in_bounds = TRUE;
        else
            is_in_bounds = FALSE;

        y *= yb;
        y += ya;

        if ( is_in_bounds )
            {
            if ( is_first_point )
                {
                MoveToEx( dc, int(x), int(y), 0 );
                is_first_point = FALSE;
                }
            else
                {
                p[i].x = long( x );
                p[i].y = long( y );
                i++;
                }
            }
        else if ( i > 0 )
            {
            PolylineTo( dc, p, i );
            is_first_point = TRUE;
            i = 0;
            }

        }

    xp++;
    yp++;
    }

if ( i > 0 )
    PolylineTo( dc, p, i );

delete[] p;
}



