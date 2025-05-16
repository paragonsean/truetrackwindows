#include <windows.h>

/********************************************************************
*                           DAY_OF_THE_WEEK                         *
* This function returns the number of the day of the week           *
* for the date given to it.  Year is like 1986 or 2001.             *
* 1=Jan, 12=Dec, days = [1,31];                                     *
* Sun = 0  Mon = 1  Tue = 2  Wed = 3  Thu = 4  Fri = 5  Sat = 6     *
********************************************************************/
int day_of_the_week( int m, int d, int y )
{

int i1;
int i2;
int i3;
int i4;
int n;

if ( m <= 2 )
    {
    m = m + 12;
    y = y - 1;
    }

i1 = ( 6 * (m+1) )/10;
i2 = y/4;
i3 = y/100;
i4 = y/400;

n = d + 2*m + i1 + y + i2 - i3 + i4 + 1;

return (  n%7 );
}
