#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\textlen.h"

/***********************************************************************
*                           TEXT_LEN_CLASS                             *
*                               check                                  *
***********************************************************************/
void TEXT_LEN_CLASS::check( TCHAR * s )
{
SIZE sz;
int32 i;

if ( dc )
    {
    GetTextExtentPoint32( dc, s, lstrlen(s), &sz );
    i = sz.cx;
    if ( i > w )
        w = i;

    i = sz.cy;
    if ( i > h )
        h = i;
    }

};

