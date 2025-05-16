#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

/***********************************************************************
*                              FIND_INDEX                              *
***********************************************************************/
int find_index( float * points, float target, int start_index, int nof_points )
{

int     mindex;
int     increment;
int     i;
float   min_delta;
float   delta;

i         = start_index;
mindex    = start_index;
min_delta = absolute_value( target - points[i] );
increment = 1;
if ( points[i] > target )
    increment = -1;

while ( TRUE )
    {
    i += increment;
    if ( i < 0 || i >= nof_points )
        break;

    delta = absolute_value( target - points[i] );
    if ( delta <= min_delta )
        {
        min_delta = delta;
        mindex    = i;
        }
    else
        break;
    }

return mindex;
}
