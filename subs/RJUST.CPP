#include <windows.h>

#include "../include/visiparm.h"
#include "../include/subs.h"

/***********************************************************************
*                                RJUST                                 *
*   "orig"  -> "   orig"                                               *
***********************************************************************/
void rjust( LPTSTR s, short n, TCHAR c )
{

short dif;
short slen;
size_t bytes_to_move;

slen = short( lstrlen(s) );
if ( slen < n )
    {
    dif = n - slen;
    bytes_to_move = size_t(slen);
    bytes_to_move++;
    bytes_to_move *= sizeof( TCHAR );
    MoveMemory( s+dif, s, bytes_to_move );
    fillchars( s, c, dif );
    }
}
