#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\colorcl.h"
#include "..\include\subs.h"

static TCHAR CommaChar = TEXT( ',' );
static TCHAR NullChar  = TEXT( '\0' );

/***********************************************************************
*                           GREEN_COMPONENT                            *
***********************************************************************/
int COLOR_CLASS::green_component()
{
int green;

green = color >> 8;
green &= 0xFF;
return green;
}

/***********************************************************************
*                            BLUE_COMPONENT                            *
***********************************************************************/
int COLOR_CLASS::blue_component()
{
int blue;

blue = color >> 16;
blue &= 0xFF;
return blue;
}

/***********************************************************************
*                             RED_COMPONENT                            *
***********************************************************************/
int COLOR_CLASS::red_component()
{
int red;

red = color & 0xFF;
return red;
}

/***********************************************************************
*                                 PUT                                  *
*                             0123456789_1                             *
*                             255,255,255n                             *
***********************************************************************/
TCHAR * COLOR_CLASS::put()
{
static TCHAR buf[12];
TCHAR * cp;
int c;

c = red_component();
cp = copy_w_char( buf, int32toasc((int32) c), CommaChar );

c = green_component();
cp = copy_w_char( cp, int32toasc((int32) c), CommaChar );

c = blue_component();
cp = copystring( cp, int32toasc((int32) c) );

return buf;
}

/***********************************************************************
*                                 GET                                  *
***********************************************************************/
void COLOR_CLASS::get( TCHAR * sorc )
{
TCHAR * cp;
TCHAR * ep;
int slen;

int b;
int g;
int r;

r = 0;
g = 0;
b = 0;

cp = sorc;
ep = findchar( CommaChar, cp );
if ( ep )
    {
    slen = ep-cp;
    r = (int) extlong( cp, slen );
    if ( r > 255 )
        r = 255;
    else if ( r < 0 )
        r = 0;
    cp = ep;
    cp++;
    ep = findchar( CommaChar, cp );
    if ( ep )
        {
        slen = ep-cp;
        g = (int) extlong( cp, slen );
        if ( g > 255 )
            g = 255;
        else if ( g < 0 )
            g = 0;

        cp = ep;
        cp++;
        b = (int) asctoint32( cp );
        if ( b > 255 )
            b = 255;
        else if ( b < 0 )
            b = 0;
        }
    }

set( r, g, b );
}