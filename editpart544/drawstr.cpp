#include <windows.h>

#include "..\include\visiparm.h"

/***********************************************************************
*                         DRAW_VERTICAL_STRING                         *
*                                                                      *
* The return value is the rightmost pixel location I have written to.  *
***********************************************************************/
int32 draw_vertical_string( HDC dc, TCHAR * sorc, int32 x, int32 y )
{
int32      dx;
int32      h;

TEXTMETRIC tm;
SIZE       sorc_size;

if ( !sorc )
    return x;

GetTextMetrics( dc, &tm );

h = tm.tmHeight;

SetTextAlign( dc, TA_TOP | TA_CENTER );

dx = 0;
while ( *sorc )
    {
    GetTextExtentPoint32( dc, sorc, 1, &sorc_size );
    if ( dx < sorc_size.cx )
        dx = sorc_size.cx;
    TextOut( dc, x, y, sorc, 1 );
    y += h;
    sorc++;
    }

return x + dx;
}
