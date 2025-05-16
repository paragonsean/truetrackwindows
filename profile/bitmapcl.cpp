#include <windows.h>
 
#include "..\include\visiparm.h"
#include "bitmapcl.h"
 
/***********************************************************************
*                             BITMAP_CLASS                             *
*                               CLEANUP                                *
***********************************************************************/
void BITMAP_CLASS::cleanup( void )
{
if ( bh )
    {
    DeleteObject( bh );
    bh = 0;
    }
}
 
/***********************************************************************
*                             BITMAP_CLASS                             *
***********************************************************************/
BITMAP_CLASS::BITMAP_CLASS()
{
bh = 0;
h  = 0;
w  = 0;
x  = 0;
y  = 0;
}
 
/***********************************************************************
*                            ~BITMAP_CLASS                             *
***********************************************************************/
BITMAP_CLASS::~BITMAP_CLASS()
{
cleanup();
}
 
/***********************************************************************
*                             BITMAP_CLASS                             *
*                                 INIT                                 *
***********************************************************************/
BOOLEAN BITMAP_CLASS::init( HDC dc, int32 new_width, int32 new_height )
{
cleanup();
 
w = new_width;
h = new_height;
 
bh = CreateCompatibleBitmap( dc, w, h );
if ( bh )
    return TRUE;
 
h = 0;
w = 0;
return FALSE;
}
 
/***********************************************************************
*                             BITMAP_CLASS                             *
*                                 GET                                  *
***********************************************************************/
BOOLEAN BITMAP_CLASS::get( HDC dc, int32 xpos, int32 ypos )
{
HDC mdc;
 
if ( !bh )
    return FALSE;
 
mdc = CreateCompatibleDC( dc );
if ( !mdc )
    return FALSE;
 
x = xpos;
y = ypos;
 
SelectObject( mdc, bh );
BitBlt( mdc, 0, 0, w, h, dc, x, y, SRCCOPY );
DeleteDC( mdc );
 
return TRUE;
}
 
/***********************************************************************
*                             BITMAP_CLASS                             *
*                               RESTORE                                *
***********************************************************************/
BOOLEAN BITMAP_CLASS::restore( HDC dc )
{
HDC mdc;
 
if ( !bh )
    return FALSE;
 
mdc = CreateCompatibleDC( dc );
if ( !mdc )
    return FALSE;
 
SelectObject( mdc, bh );
BitBlt( dc, x, y, w, h, mdc, 0, 0, SRCCOPY );
DeleteDC( mdc );
 
return TRUE;
}
 
 
