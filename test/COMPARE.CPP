#include <windows.h>
#include <winioctl.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\genlist.h"
#include "..\include\subs.h"

/***********************************************************************
*                            COMPARE_TEXT                              *
*                         -1 => sorc1 <  sorc2                         *
*                          0 => sorc1 == sorc2                         *
*                          1 => sorc1 >  sorc2                         *
***********************************************************************/
int compare_text( const void * sorc1, const void * sorc2 )
{
int           result;
const TCHAR * s1;
const TCHAR * s2;

if ( !sorc1 )
    return -1;

if ( !sorc2 )
    return 1;

s1 = (const TCHAR *) sorc1;
s2 = (const TCHAR *) sorc2;

result = CompareString( LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, s1, -1, s2, -1 );

if ( result == CSTR_LESS_THAN )
    return -1;

else if ( result == CSTR_GREATER_THAN )
    return 1;

return 0;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
//TCHAR buf[23];
static TCHAR s1[] = TEXT( "Hi" );
static TCHAR s2[] = TEXT( "You" );
static TCHAR s3[] = TEXT( "Guys" );
static TCHAR s4[] = TEXT( "ZipUt" );
static TCHAR s5[] = TEXT( "ZipIt" );

GENERIC_LIST_CLASS g;
TCHAR * s;

g.append( s1 );
g.append( s2 );
g.append( s3 );
g.append( s4 );
g.append( s1 );
g.append( s5 );

g.rewind();
while ( TRUE )
    {
    s = (TCHAR *) g.next();
    if ( !s )
        break;
    cprintf( "\n\r [%s]", s );
    }

cprintf( "\n\rSorting..." );
g.sort( compare_text );

g.rewind();
while ( TRUE )
    {
    s = (TCHAR *) g.next();
    if ( !s )
        break;
    cprintf( "\n\r [%s]", s );
    }

//buf[0] = 10;
//cgets( buf );

return 0;
}

