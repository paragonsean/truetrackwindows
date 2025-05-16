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
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
static TCHAR USACountryCode[] = TEXT( "1" );
UINT   x;
TCHAR * cp;
bool   waset;

waset = false;
cprintf( "\n\rInput the number of milliseconds between blinks: " );
cp = getline();
if ( cp )
    {
    if ( lstrlen(cp) > 0 )
        {
        x = asctoul( cp );
        SetCaretBlinkTime( x );
        cprintf( "\n\rBlink Time has been set to %u ms\n\r", x );
        waset = true;
        }
    }

if ( !waset )
    cprintf( "\n\rNothing done." );
return 0;
}

