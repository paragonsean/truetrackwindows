#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\rectclas.h"
#include "..\include\subs.h"

const TCHAR CommaChar = TEXT( ',' );
const TCHAR NullChar  = TEXT( '\0' );

struct CURSOR_ENTRY
    {
    int       cursor;
    BITSETYPE border;
    };
const CURSOR_ENTRY Cursors[] = {
    { (int) IDC_SIZENWSE, N_BORDER | W_BORDER },
    { (int) IDC_SIZENWSE, S_BORDER | E_BORDER },
    { (int) IDC_SIZENESW, N_BORDER | E_BORDER },
    { (int) IDC_SIZENESW, S_BORDER | W_BORDER },
    { (int) IDC_SIZEWE,   W_BORDER },
    { (int) IDC_SIZEWE,   E_BORDER },
    { (int) IDC_SIZENS,   S_BORDER },
    { (int) IDC_SIZENS,   N_BORDER }
    };

const int NofCursors = sizeof(Cursors)/sizeof(CURSOR_ENTRY);

/***********************************************************************
*                          RECTANGLE_CLASS                             *
***********************************************************************/
RECTANGLE_CLASS::RECTANGLE_CLASS()
{
left   = 0;
top    = 0;
right  = 0;
bottom = 0;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                          RECTANGLE_CLASS                             *
***********************************************************************/
RECTANGLE_CLASS::RECTANGLE_CLASS( const RECTANGLE_CLASS & sorc )
{
left   = sorc.left;
top    = sorc.top;
right  = sorc.right;
bottom = sorc.bottom;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                          RECTANGLE_CLASS                             *
***********************************************************************/
RECTANGLE_CLASS::RECTANGLE_CLASS( const RECT & sorc )
{
left   = sorc.left;
top    = sorc.top;
right  = sorc.right;
bottom = sorc.bottom;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                          RECTANGLE_CLASS                             *
***********************************************************************/
RECTANGLE_CLASS::RECTANGLE_CLASS( long x1, long y1, long x2, long y2 )
{
left   = x1;
top    = y1;
right  = x2;
bottom = y2;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                         ~RECTANGLE_CLASS                             *
***********************************************************************/
RECTANGLE_CLASS::~RECTANGLE_CLASS()
{
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                =                                     *
***********************************************************************/
void RECTANGLE_CLASS::operator=(const RECTANGLE_CLASS & sorc )
{
left   = sorc.left;
top    = sorc.top;
right  = sorc.right;
bottom = sorc.bottom;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                 =                                    *
***********************************************************************/
void RECTANGLE_CLASS::operator=(const RECT & sorc )
{
left   = sorc.left;
top    = sorc.top;
right  = sorc.right;
bottom = sorc.bottom;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                ==                                    *
***********************************************************************/
BOOLEAN RECTANGLE_CLASS::operator==(const RECTANGLE_CLASS & sorc )
{
if ( left != sorc.left || top != sorc.top || right != sorc.right || bottom != sorc.bottom )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                ==                                    *
***********************************************************************/
BOOLEAN RECTANGLE_CLASS::operator==(const RECT & sorc )
{
if ( left != sorc.left || top != sorc.top || right != sorc.right || bottom != sorc.bottom )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                              CONTAINS                                *
*Like PtInRect(this, &p) except I include the bottom of the rectangle. *
***********************************************************************/
BOOLEAN RECTANGLE_CLASS::contains( const POINT & p )
{
if ( p.x < left || p.x > right )
    return FALSE;

if ( p.y < top || p.y > bottom )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                             INTERSECTS                               *
***********************************************************************/
BOOLEAN RECTANGLE_CLASS::intersects( const RECT & r )
{
RECT dest;

if ( IntersectRect( &dest, this, &r ) != 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           RECTANGLE_CLASS                            *
*                               IS_EMTPY                               *
***********************************************************************/
BOOLEAN RECTANGLE_CLASS::is_empty( void )
{
if ( left == 0 && right == 0 && top == 0 && bottom == 0 )
    return TRUE;

if ( right == left && bottom == top )
    return TRUE;

if ( right < left || bottom < top )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           RECTANGLE_CLASS                            *
*                             NEAR_BORDERS                             *
***********************************************************************/
BITSETYPE RECTANGLE_CLASS::near_borders( const POINT & p )
{
const BITSETYPE N_BORDER  = 1;
const BITSETYPE E_BORDER  = 2;
const BITSETYPE S_BORDER  = 4;
const BITSETYPE W_BORDER  = 8;
BITSETYPE b;

b = 0;
if ( p.x >= left-1 && p.x <= right+1 )
    {
    if ( p.y >= top-1 && p.y <= bottom+1 )
        {
        if ( p.x <= left+2 )
            b |= W_BORDER;

        if ( p.x >= right-2 )
            b |= E_BORDER;

        if ( p.y <= top+2 )
            b |= N_BORDER;

        if ( p.y >= bottom-2 )
            b |= S_BORDER;
        }
    }

return b;
}

/***********************************************************************
*                           RECTANGLE_CLASS                            *
*                               GETCURSOR                              *
* Returns the proper cursor id for resizing the rectangle based        *
* on where the point is. If the point is not on the rectangle I        *
* return 0;                                                            *
***********************************************************************/
int  RECTANGLE_CLASS::getcursor( const POINT & p )
{
int       i;
BITSETYPE b;

b = near_borders( p );

if ( b )
    {
    for ( i=0; i<NofCursors; i++ )
        {
        if ( b == Cursors[i].border )
            return Cursors[i].cursor;
        }
    }

return 0;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                               MOVETO                                 *
***********************************************************************/
void RECTANGLE_CLASS::moveto( long x, long y )
{
bottom += y - top;
right  += x - left;
left    = x;
top     = y;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                               MOVETO                                 *
***********************************************************************/
void RECTANGLE_CLASS::moveto( POINT & p )
{
bottom += p.y - top;
right  += p.x - left;
left    = p.x;
top     = p.y;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                               OFFSET                                 *
***********************************************************************/
void RECTANGLE_CLASS::offset( long dx, long dy )
{
left   += dx;
right  += dx;
top    += dy;
bottom += dy;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                               SETSIZE                                *
***********************************************************************/
void RECTANGLE_CLASS::setsize( SIZE sz )
{
right  = left + sz.cx;
bottom = top  + sz.cy;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                               SETSIZE                                *
***********************************************************************/
void RECTANGLE_CLASS::setsize( long width, long height )
{
right  = left + width;
bottom = top  + height;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                DRAW                                  *
***********************************************************************/
void RECTANGLE_CLASS::draw( HDC dc )
{
MoveToEx( dc, left,  top, 0 );
LineTo(   dc, right, top    );
LineTo(   dc, right, bottom );
LineTo(   dc, left,  bottom );
LineTo(   dc, left,  top    );
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                DRAW                                  *
***********************************************************************/
void RECTANGLE_CLASS::draw( HDC dc, COLORREF color )
{
HPEN oldpen;
HPEN newpen;

newpen  = CreatePen( PS_SOLID, 1, color );

oldpen = (HPEN) SelectObject( dc, newpen );
draw( dc );
SelectObject( dc, oldpen );
DeleteObject( newpen );
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                FILL                                  *
***********************************************************************/
void RECTANGLE_CLASS::fill( HDC dc, HBRUSH brush )
{
FillRect( dc, this, brush );
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                FILL                                  *
***********************************************************************/
void RECTANGLE_CLASS::fill( HDC dc, COLORREF color )
{
HBRUSH       oldbrush;
HBRUSH       brush;

brush  = CreateSolidBrush( color );
oldbrush = (HBRUSH) SelectObject( dc, brush );
FillRect( dc, this, brush );
SelectObject( dc, oldbrush );
DeleteObject( brush );
}

/***********************************************************************
*                               REVERSE                                *
***********************************************************************/
static void reverse( TCHAR * s )
{

TCHAR  c;
TCHAR * cp1;
TCHAR * cp2;

cp1 = s;
cp2 = s + lstrlen(s) - 1;
while ( cp1 < cp2 )
    {
    c = *cp2;
    *cp2-- = *cp1;
    *cp1++ = c;
    }
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                PUT                                   *
*                       x     y     w     h                            *
*                     32000,32000,32000,32000                          *
***********************************************************************/
TCHAR * RECTANGLE_CLASS::put()
{
const int BUFLEN = 27;
static TCHAR buf[BUFLEN+1];
TCHAR * cp;
TCHAR s[MAX_INTEGER_LEN+1];

int32toasc( s, (int32) left, DECIMAL_RADIX );
cp = copy_w_char( buf, s, CommaChar );

int32toasc( s, (int32) top, DECIMAL_RADIX );
cp = copy_w_char( cp, s, CommaChar );

int32toasc( s, (int32) right-left, DECIMAL_RADIX );
cp = copy_w_char( cp, s, CommaChar );

int32toasc( s, (int32) bottom-top, DECIMAL_RADIX );
copystring( cp, s );

return buf;
}

/***********************************************************************
*                            NEXT_FIELD                                *
***********************************************************************/
static bool next_field( TCHAR * & sorc )
{
while ( *sorc != CommaChar && *sorc != NullChar )
    sorc++;

if ( *sorc == NullChar )
    return false;

sorc++;
return true;
}

/***********************************************************************
*                          RECTANGLE_CLASS                             *
*                                GET                                   *
*                       x     y     w     h                            *
*                     32000,32000,32000,32000                          *
***********************************************************************/
BOOLEAN RECTANGLE_CLASS::get( TCHAR * sorc )
{
if ( !sorc )
    return false;

left = (LONG) asctoint32( sorc );
next_field( sorc );

top = (LONG) asctoint32( sorc );
next_field( sorc );

right = left + (LONG) asctoint32( sorc );
next_field( sorc );

bottom = top + (LONG) asctoint32( sorc );

if ( *sorc == NullChar )
    {
    empty();
    return FALSE;
    }

return TRUE;
}
