#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

static TCHAR CChar = TEXT( 'c');

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
/**************************
TCHAR  * cp;
while ( TRUE )
    {
    if ( lstrlen(cp) < 1 )
        break;
    }
***************************/
TCHAR c = '\xFC';
union { unsigned u; TCHAR ch; };
u = 0;
ch = c;
c = (TCHAR) CharUpper( LPTSTR(u) );
cprintf( "\n\r[ %c ]\n\r", c );
cprintf( "\n\rDone, press <Enter> to exit..." );
getline();
cprintf( "\n\r" );

return 0;
}

