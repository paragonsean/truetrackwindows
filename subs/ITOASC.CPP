#include <windows.h>
#ifdef __BORLANDC__
#include <systypes.h>
#endif

#define MAX_INTEGER_LEN 18

#include "../include/visiparm.h"
#include "../include/subs.h"

static const TCHAR NullChar  = TEXT( '\0' );
static const TCHAR MinusChar = TEXT( '-' );

/***********************************************************************
*                               INT32TOASC                             *
***********************************************************************/
void int32toasc( LPTSTR dest, int32 n, int32 radix )
{

LPTSTR cp;
int32 sign;
int32 i;

sign = n;
if ( n < 0 )
    n = -n;

cp = dest;
do  {
    i = n % radix;
    if ( i <= 9 )
        *cp = TCHAR(i+'0');
    else
        *cp = TCHAR(i+'7');
    cp++;
    n /= radix;
    } while ( n > 0 );

if ( sign < 0 )
    *cp++ = MinusChar;

*cp = NullChar;

reverse(dest);

}

/***********************************************************************
*                               INT32TOASC                             *
***********************************************************************/
TCHAR * int32toasc( int32 n )
{
static TCHAR s[MAX_INTEGER_LEN+1];

int32toasc( s, n, DECIMAL_RADIX );

return s;
}

/***********************************************************************
*                               ULTOASCII                              *
***********************************************************************/
void ultoascii( TCHAR * dest, unsigned long n, unsigned long radix )
{
TCHAR  * cp;
unsigned long i;

cp = dest;
do  {
    i = n % radix;
    if ( i <= 9 )
        *cp = TCHAR(i+'0');
    else
        *cp = TCHAR(i+'7');
    cp++;
    n /= radix;
    } while ( n > 0 );

*cp = NullChar;

reverse( dest );
}

/***********************************************************************
*                               ULTOASCII                              *
***********************************************************************/
TCHAR * ultoascii( unsigned long n, unsigned long radix )
{
static TCHAR s[MAX_INTEGER_LEN+1];
ultoascii( s, n, radix );
return s;
}

/***********************************************************************
*                               ULTOASCII                              *
***********************************************************************/
TCHAR * ultoascii( unsigned long n )
{
return ultoascii( n, (unsigned long) DECIMAL_RADIX );
}
