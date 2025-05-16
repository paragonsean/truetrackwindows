#include <windows.h>
#ifdef __BORLANDC__
#include <systypes.h>
#endif

#include "../include/visiparm.h"
#include "../include/subs.h"

/***********************************************************************
*                               INSALPH                                *
***********************************************************************/
void insalph( LPTSTR dest, int32 n, int32 slen, TCHAR c, int32 radix )
{

TCHAR t[12];

int32toasc( t, n, radix );
rjust( t, short(slen), c );
copychars( dest, t, short(slen) );
}

/***********************************************************************
*                               INSALPH                                *
***********************************************************************/
void insalph( LPTSTR dest, double x, int32 slen, TCHAR c )
{

TCHAR t[31];

ftoasc( t, x );
rjust( t, short(slen), c );
copychars( dest, t, short(slen) );

}

/***********************************************************************
*                               INSALPH                                *
***********************************************************************/
void insalph( LPTSTR dest, float x, int32 slen, TCHAR c )
{

TCHAR t[31];

ftoasc( t, x );
rjust( t, short(slen), c );
copychars( dest, t, short(slen) );

}

