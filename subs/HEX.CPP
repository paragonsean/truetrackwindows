#include <windows.h>
#include <stdlib.h>

#include "../include/visiparm.h"
#include "../include/subs.h"

static TCHAR         NullChar = TEXT( '\0' );
static unsigned long Radix    = 16;

/***********************************************************************
*                             IS_HEX_CHAR                              *
***********************************************************************/
BOOLEAN is_hex_char( TCHAR c )
{
const static TCHAR s[] = TEXT( "0123456789ABCDEFabcdef" );
const TCHAR * cp;
cp = s;
while ( *cp )
    {
    if ( c == *cp )
        return TRUE;
    cp++;
    }
return FALSE;
}

/***********************************************************************
*                               ULTOHEX                                *
***********************************************************************/
void ultohex( TCHAR * dest, unsigned long n )
{
ultoascii( dest, n, (unsigned long)HEX_RADIX );
}

/***********************************************************************
*                               ULTOHEX                                *
***********************************************************************/
TCHAR * ultohex( unsigned long n )
{
return ultoascii( n, Radix );
}

/***********************************************************************
*                                HEXTOUL                               *
***********************************************************************/
unsigned long hextoul( const TCHAR * sorc )
{
TCHAR   sc[MAX_HEX_LEN+1];
char    save[MAX_HEX_LEN+1];
TCHAR * cp;

lstrcpyn( sc, sorc, MAX_HEX_LEN+1);
*(sc+MAX_HEX_LEN) = NullChar;

/*
---------------------------------------------------
Put a null at the first char that is not a hex char
--------------------------------------------------- */
cp = sc;
while ( is_hex_char(*cp) )
    cp++;
*cp = NullChar;

CharToOem( sc, save );

return strtoul( save, NULL, 16 );
}

