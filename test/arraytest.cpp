#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static const TCHAR NullChar = TEXT( '\0' );

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
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
TCHAR * cp;
unsigned int ** analog;
unsigned int *  x;
int i;

cprintf( "\n\rPress enter to start: " );
cp = getline();

analog = new unsigned int *[4];
x      = new unsigned int[4000];

for ( i=0; i<4000; i++ )
    x[i] = i;

//cprintf( "\n\rstart of shift = %s", t.text() );
analog[0] = x;
analog[1] = x+1000;
analog[2] = x+2000;
analog[3] = x+3000;

for ( i=0; i<1000; i++ )
    cprintf( "\n\r%4ld %4ld %4ld %4ld", analog[0][i], analog[1][i], analog[2][i], analog[3][i] );

delete[] analog;
analog = 0;
delete[] x;

cprintf( "\n\rPress enter to exit " );
cp = getline();

return 0;
}

