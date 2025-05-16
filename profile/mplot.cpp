#define NOMINMAX
#include <windows.h>
#include <math.h>

#include "..\include\visiparm.h"
#include "..\include\array.h"
#include "..\include\color.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

#include "extern.h"
#include "mplot.h"
#include "xaxis.h"

extern STRING_CLASS ArialFontName;
extern X_AXIS_CLASS XAxis; /* Kept in Trend.cpp */

static const COLORREF DEFAULT_COLOR = RGB( 255, 255, 255 );
static const COLORREF LIGHT_GRAY_COLOR = RGB( 200, 200, 200 );
static SINGLE_PLOT_CLASS EmptyPlot;

/***********************************************************************
*                          DATA_POINT_ENTRY                            *
*                                ADD                                   *
***********************************************************************/
void DATA_POINT_ENTRY::add( float x )
{

sum  += x;
sum2 += x*x;
if ( n == 0 )
    {
    min = x;
    max = x;
    }
else if ( x < min )
    {
    min = x;
    }
else if ( x > max )
    {
    max = x;
    }

n++;
}

/***********************************************************************
*                          DATA_POINT_ENTRY                            *
*                                AVG                                   *
***********************************************************************/
float DATA_POINT_ENTRY::avg( void )
{
float x;

x = 0.0;
if ( n > 0 )
    x = sum / float(n);

return x;
}

/***********************************************************************
*                          DATA_POINT_ENTRY                            *
*                                 SD                                   *
***********************************************************************/
float DATA_POINT_ENTRY::sd( void )
{
float x;
x = 0.0;
if ( n > 0 )
    x = sqrt( sum2 - sum*sum / float(n) );

return x;
}

/***********************************************************************
*                          DATA_ARRAY_CLASS                            *
*                               CLEANUP                                *
***********************************************************************/
void DATA_ARRAY_CLASS::cleanup( void )
{
if ( p )
    delete[] p;
p    = 0;
n    = 0;
nset = 0;
x    = 0;
rewound = FALSE;
}

/***********************************************************************
*                          DATA_ARRAY_CLASS                            *
*                                INIT                                  *
***********************************************************************/
BOOLEAN DATA_ARRAY_CLASS::init( int32 nof_points )
{
cleanup();

if ( nof_points > 0 )
    {
    n = nof_points;
    p = new DATA_POINT_ENTRY[n];
    if ( p )
        {
        rewound = TRUE;
        return TRUE;
        }
    else
        {
        n = 0;
        cleanup();
        }
    }

return FALSE;
}

/***********************************************************************
*                          DATA_ARRAY_CLASS                            *
*                            LEFT_SHIFT                                *
***********************************************************************/
int32 DATA_ARRAY_CLASS::left_shift( DATA_POINT_ENTRY & sorc )
{
int32 i;

if ( !p )
    return 0;

if ( n <= 0 )
    return 0;

if ( nset < n )
    {
    i = nset;
    p[nset] = sorc;
    nset++;
    }
else
    {
    for ( i=0; i<(n-1); i++ )
        p[i] = p[i+1];

    p[n-1] = sorc;
    i = n-1;
    }

return i;
}

/***********************************************************************
*                          DATA_ARRAY_CLASS                            *
*                            LEFT_SHIFT                                *
***********************************************************************/
int32 DATA_ARRAY_CLASS::left_shift( float value )
{
DATA_POINT_ENTRY d;

d.add ( value );
return left_shift( d );
}

/***********************************************************************
*                          DATA_ARRAY_CLASS                            *
*                            NEXT_SET                                  *
***********************************************************************/
BOOLEAN DATA_ARRAY_CLASS::next_set( void )
{

if ( rewound )
    {
    rewound = FALSE;
    }
else
    {
    x++;
    if ( x >= nset )
        {
        x--;
        return FALSE;
        }
    }

return TRUE;
}

/***********************************************************************
*                          DATA_ARRAY_CLASS                            *
*                               NEXT                                   *
***********************************************************************/
BOOLEAN DATA_ARRAY_CLASS::next( void )
{

if ( rewound )
    {
    rewound = FALSE;
    }
else
    {
    x++;
    if ( x >= n )
        {
        x--;
        return FALSE;
        }
    }

return TRUE;
}

/***********************************************************************
*                          DATA_ARRAY_CLASS                            *
*                                ADD                                   *
***********************************************************************/
BOOLEAN DATA_ARRAY_CLASS::add( float value )
{
if ( p )
    {
    p[x].add( value );
    if ( x >= nset )
        nset = x + 1;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          DATA_ARRAY_CLASS                            *
*                                ADD                                   *
***********************************************************************/
BOOLEAN DATA_ARRAY_CLASS::add( int32 i, float value )
{
if ( !p )
    return FALSE;

if ( i < 0 || i >= n )
    return FALSE;

x = i;
return add( value );
}

static HFONT OldFont = 0;
static HFONT BigFont = 0;

/***********************************************************************
*                               LOADFONT                               *
***********************************************************************/
static BOOLEAN loadfont( HDC dc, TCHAR * fontname, int32 height, int32 width )
{
LOGFONT    lf;

/*
-------------------------------------------
Go home if I have been called and not freed
------------------------------------------- */
if ( OldFont )
    return FALSE;

lf.lfWidth           = width;
lf.lfHeight          = height;
lf.lfEscapement      = 0;
lf.lfOrientation     = 0;
lf.lfWeight          = FW_DONTCARE;
lf.lfItalic          = FALSE;
lf.lfUnderline       = FALSE;
lf.lfStrikeOut       = FALSE;
lf.lfCharSet         = ANSI_CHARSET;
lf.lfOutPrecision    = 0;
lf.lfClipPrecision   = 0;
lf.lfQuality         = DEFAULT_QUALITY;
lf.lfPitchAndFamily  = 0;
lstrcpy( lf.lfFaceName, fontname );

if (PRIMARYLANGID(GetUserDefaultLangID())==LANG_JAPANESE)
{
	lf.lfCharSet = SHIFTJIS_CHARSET;
	lstrcpy(lf.lfFaceName,TEXT("MS –¾’©"));
}

BigFont = CreateFontIndirect( &lf );
if ( !BigFont )
    return FALSE;

OldFont = (HFONT) SelectObject( dc, BigFont );
return TRUE;
}

/***********************************************************************
*                             GET_TEXT_SIZE                            *
***********************************************************************/
void get_text_size( int32 & dest_width, int32 & dest_height, HDC dc, const TCHAR * sorc )
{
int32 slen;
SIZE  textsize;

slen = lstrlen( sorc );
GetTextExtentPoint32( dc, sorc, slen, &textsize );
dest_width  = (int32) textsize.cx;
dest_height = (int32) textsize.cy;
}

/***********************************************************************
*                             LOAD_BIGFONT                             *
***********************************************************************/
static BOOLEAN load_bigfont( HDC dc, TCHAR * fontname, RECT & r, TCHAR * s )
{

int32 n;
int32 navg;
int32 slen;
int32 height;
int32 width;

SIZE       textsize;
TEXTMETRIC tm;

slen = lstrlen( s );
GetTextExtentPoint32( dc, s, slen, &textsize );
slen = textsize.cx;

/*
----------------------------------------------------------
Calculate the number of average characters this represents
---------------------------------------------------------- */
GetTextMetrics( dc, &tm );
navg = slen / tm.tmAveCharWidth;

/*
--------
Round up
-------- */
navg++;
navg++;

/*
----------------------------------------------------------
Now, calculate the size of the largest average char to fit
---------------------------------------------------------- */
width  = (r.right - r.left ) / navg;
height = r.bottom - r.top;

n = 3*width;

if ( height > n )
    height = n;

n = height/2;
if ( width > n )
    width = n;

return loadfont( dc, fontname, height, width );
}

/***********************************************************************
*                             UNLOADFONT                               *
***********************************************************************/
static void unloadfont( HDC dc )
{

if ( OldFont )
    {
    SelectObject( dc, OldFont );
    OldFont = 0;
    if ( BigFont )
        {
        DeleteObject( BigFont );
        BigFont = 0;
        }
    }
}

/***********************************************************************
*                              DRAW_LINE                               *
***********************************************************************/
static void draw_line( HDC dc, int x1, int y1, int x2, int y2 )
{
MoveToEx( dc, x1, y1, 0 );
LineTo( dc, x2, y2 );
}

/***********************************************************************
*                            DECIMAL_PLACES                            *
***********************************************************************/
static int32 decimal_places( float value )
{
int32 n;

n = 0;

if ( value < 10000.0 )
    n = 1;

if ( value < 1000.0 )
    n = 2;

if ( value < 100.0 )
    n = 3;

if ( value < 10.0 )
    n = 4;

return n;
}

/***********************************************************************
*                               ROUND_UP                               *
*             Round up to a nice round number for plotting.            *
***********************************************************************/
static float round_up( float value, float min_value )
{
BOOLEAN is_negative;
int32 n;
float rounded_value;

/*
--------------------------
Don't round if less than 1
-------------------------- */
rounded_value = value - min_value;

if ( rounded_value > -1.0 && rounded_value < 1.0 )
    return value;

rounded_value = value;

n = (int32) value;
if ( value > float(n) )
    n++;
is_negative = FALSE;
if ( n < 0 )
    {
    is_negative = TRUE;
    n = -n;
    }

if ( n > 0 && n < 10 )
    {
    if ( float(n) < value )
        n++;
    }
else if ( n >= 10 && n < 100 )
    {
    while ( n % 5 )
        n++;
    }
else if ( n >= 100 && n < 1000 )
    {
    while ( n % 10 )
        n++;
    }
else
    {
    while ( n % 100 )
        n++;
    }

if ( n != 0 )
    {
    if ( is_negative )
        n = -n;
    rounded_value = n;
    }

if ( rounded_value > value && value > min_value )
    {
    while ( rounded_value - value > value - min_value )
        rounded_value -= (rounded_value - value) / 2.0;

    }
return rounded_value;
}

/***********************************************************************
*                           SINGLE_PLOT_CLASS                          *
***********************************************************************/
SINGLE_PLOT_CLASS::SINGLE_PLOT_CLASS()
{
alarm_max     = 0.0;
alarm_min     = 0.0;
warning_max   = 0.0;
warning_min   = 0.0;
}

/***********************************************************************
*                          ~SINGLE_PLOT_CLASS                          *
***********************************************************************/
SINGLE_PLOT_CLASS::~SINGLE_PLOT_CLASS()
{
}

/***********************************************************************
*                          SINGLE_PLOT_CLASS                           *
*                             SHOW_UNITS                               *
***********************************************************************/
void SINGLE_PLOT_CLASS::show_units( HDC dc )
{
int32 ch;
int32 x;
int32 y;
RECT  mr;
SIZE  sz;
TCHAR * cp;

if ( !units.len() )
    return;

y = character_height( dc );

/*
------------------------------------
Calculate the size of each character
------------------------------------ */
ch = (r.bottom - r.top) / units.len();
if ( ch > y )
    ch = y;

if ( !loadfont(dc, ArialFontName.text(), ch, 0) )
    return;

ch = character_height( dc );
y = ( r.bottom + r.top - ch*units.len() ) / 2;

/*
-------------------------------
Get the maximum character width
------------------------------- */
x  = 0;
cp = units.text();
while ( *cp )
    {
    GetTextExtentPoint32( dc, cp, 1, &sz );
    if ( sz.cx > x )
        x = sz.cx;
    cp++;
    };

GetWindowRect( MainPlotWindow, &mr );

/*
-----------------
X is the midpoint
----------------- */
x++;
x /= 2;
x += mr.left;

SetTextAlign( dc, TA_TOP | TA_CENTER );
SetTextColor( dc, CurrentColor[TEXT_COLOR] );
SetBkMode( dc, TRANSPARENT );

cp = units.text();
while ( *cp )
    {
    TextOut( dc, x, y, cp, 1 );
    y += ch;
    cp++;
    }

unloadfont( dc );
}

/***********************************************************************
*                         SINGLE_PLOT_CLASS                            *
*                               PAINT                                  *
***********************************************************************/
void SINGLE_PLOT_CLASS::paint( HDC dc )
{
int32 ch;
int32 cw;
int32 right_places;
int32 left_places;
int32 n;
int32 x;
int32 y;
float a;
float b;
float delta;
float max_value;
float min_value;
float ry;
HPEN mypen;
HPEN oldpen;
STRING_CLASS s;

HBRUSH       oldbrush;
HBRUSH       mybrush;

mybrush  = CreateSolidBrush( CurrentColor[TREND_COLOR] );
oldbrush = (HBRUSH) SelectObject( dc, mybrush );
FillRect( dc, &r, mybrush );
SelectObject( dc, oldbrush );
DeleteObject( mybrush );

if ( title.len() > 0 )
    {
    if ( load_bigfont(dc, ArialFontName.text(), r, title.text()) )
        {
        SetTextColor( dc, LIGHT_GRAY_COLOR );
        SetBkMode( dc, TRANSPARENT );
        SetTextAlign( dc, TA_BOTTOM | TA_CENTER );

        x = (r.left + r.right) / 2;
        y = r.top + character_height(dc);
        y += r.bottom;
        y /= 2;
        TextOut( dc, x, y, title.text(), title.len() );

        unloadfont( dc );
        }
    }

/*
--------------------------
Get the max and min values
-------------------------- */
rewind();
max_value = average();
min_value = average();
while ( next_set() )
    {
    maxfloat( max_value, average() );
    minfloat( min_value, average() );
    }

if ( alarm_max != alarm_min )
    {
    maxfloat( max_value, alarm_max );
    minfloat( min_value, alarm_min );
    }

if ( warning_max != warning_min )
    {
    maxfloat( max_value, warning_max );
    minfloat( min_value, warning_min );
    }

max_value = round_up( max_value, min_value );

if ( fixed_max != fixed_min )
    {
    max_value = fixed_max;
    min_value = fixed_min;
    }

pixel_equation( a, b, r.bottom, r.top, min_value, max_value );

if ( alarm_max != alarm_min )
    {
    mypen = CreatePen( PS_DASH, 1, AlarmColor );
    if ( mypen )
        {
        oldpen = (HPEN) SelectObject( dc, mypen );
        y = (int32) (a + b*alarm_max);
        draw_line( dc, r.left, y, r.right, y );

        y = (int32) (a + b*alarm_min);
        draw_line( dc, r.left, y, r.right, y );
        SelectObject( dc, oldpen );
        DeleteObject( mypen );
        }
    }

if ( warning_max != warning_min )
    {
    mypen = CreatePen( PS_DASH, 1, WarningColor );
    if ( mypen )
        {
        oldpen = (HPEN) SelectObject( dc, mypen );
        y = (int32) (a + b*warning_max);
        draw_line( dc, r.left, y, r.right, y );

        y = (int32) (a + b*warning_min);
        draw_line( dc, r.left, y, r.right, y );
        SelectObject( dc, oldpen );
        DeleteObject( mypen );
        }
    }

mypen = CreatePen( PS_SOLID, 2, CurrentColor[FIRST_ANALOG_AXIS] );
if ( mypen )
    {
    oldpen = (HPEN) SelectObject( dc, mypen );

    rewind();
    XAxis.rewind();

    next();
    XAxis.next();

    x = XAxis.shot_number();
    x = XAxis.x_pos( x );

    y = (int32) ( a + b*average() );

    MoveToEx( dc, x, y, 0 );

    while ( next() )
        {
        if ( !is_set() )
            break;
        XAxis.next();
        x = XAxis.shot_number();
        x = XAxis.x_pos( x );
        y = (int32) ( a + b*average() );
        if ( y < r.top )
            MoveToEx( dc, x, r.top, 0 );
        else if ( y > r.bottom )
            MoveToEx( dc, x, r.bottom, 0 );
        else
            LineTo( dc, x, y );
        }

    SelectObject( dc, oldpen );
    DeleteObject( mypen );
    }

mypen = CreatePen( PS_SOLID, 1, CurrentColor[TEXT_COLOR] );
if ( mypen )
    {
    oldpen = (HPEN) SelectObject( dc, mypen );

    SetTextColor( dc, CurrentColor[TEXT_COLOR] );
    SetBkMode( dc, TRANSPARENT );
    SetTextAlign( dc, TA_BOTTOM | TA_RIGHT );

    ch = character_height( dc );
    cw = average_character_width( dc );
    x  = r.left - cw/2;

    right_places = decimal_places( max_value );
    left_places = 5 - right_places;

    y  = r.top;
    ry = max_value;
    draw_line( dc, x, y, r.left, y );

    y += (ch*3)/4;
    s = fixed_string( ry, left_places, right_places );
    TextOut( dc, x, y, s.text(), s.len() );

    y  = r.bottom;
    draw_line( dc, x, y, r.left, y );

    y += ch/4;
    s = fixed_string( min_value, left_places, right_places );
    TextOut( dc, x, y, s.text(), s.len() );

    n = ((r.bottom - ch) - (r.top + ch)) / ( ch + ch/2 );
    if ( n > 0 )
        {
        ry = min_value;
        delta = (max_value - min_value) / float(n+1);
        while ( n > 0 )
            {
            ry += delta;
            y = (int32) ( a + b*ry );
            draw_line( dc, x, y, r.left, y );
            s = fixed_string( ry, left_places, right_places );
            TextOut( dc, x, y+ch/2, s.text(), s.len() );
            n--;
            }
        }

    SelectObject( dc, oldpen );
    DeleteObject( mypen );
    }

show_units( dc );
}

/***********************************************************************
*                         SINGLE_PLOT_CLASS                            *
*                        INFO_BOX_X_POSITION                           *
***********************************************************************/
int32 SINGLE_PLOT_CLASS::info_box_x_position( int32 x )
{
int32 midpoint;

midpoint = r.right - 2 - bitmap.width();

if ( x < midpoint )
    x += 2;
else
    x -= bitmap.width() + 2;

return x;
}

/***********************************************************************
*                         SINGLE_PLOT_CLASS                            *
*                          RESTORE_INFO_BOX                            *
***********************************************************************/
BOOLEAN SINGLE_PLOT_CLASS::restore_info_box( HDC dc )
{
return bitmap.restore( dc );
}

/***********************************************************************
*                         SINGLE_PLOT_CLASS                            *
*                           SHOW_INFO_BOX                              *
***********************************************************************/
BOOLEAN SINGLE_PLOT_CLASS::show_info_box( HDC dc, int32 x, int32 i, BOOLEAN need_bitmap )
{
const COLORREF black = RGB( 0, 0, 0 );
const COLORREF white = RGB( 255, 255, 255 );

int32 y;
float value;
HFONT old_font;
RECT  nr;
STRING_CLASS s;

x = info_box_x_position( x );
y = r.top + 2;

/*
-----------------------------------------
Copy the rectangle I am going to cover up
----------------------------------------- */
if ( need_bitmap )
    bitmap.get( dc, x, y );

/*
-----------------
Paint a rectangle
----------------- */
nr.left = x;
nr.top  = y;
nr.right  = x + bitmap.width() - 1;
nr.bottom = y + bitmap.height() - 1;

fill_rectangle( dc, nr, white );
draw_rectangle( dc, nr, black );

x += 2;
y += 2;

old_font = (HFONT) SelectObject( dc, GetStockObject(SYSTEM_FONT) );

SetTextAlign( dc, TA_TOP | TA_LEFT );
SetTextColor( dc, black );
SetBkMode( dc, TRANSPARENT );
value = 0.0;
if ( p && i >= 0 && i < n )
    value = p[i].avg();
s = ascii_float( value );
TextOut( dc, x, y, s.text(), s.len() );

SelectObject( dc, old_font );
return TRUE;
}

/***********************************************************************
*                         SINGLE_PLOT_CLASS                            *
*                               INIT                                   *
***********************************************************************/
BOOLEAN SINGLE_PLOT_CLASS::init( int32 nof_points )
{
const TCHAR dummy_string[] = TEXT( "000.000" );

HFONT old_font;
HDC   dc;
int32 h;
int32 w;


alarm_max   = 0.0;
alarm_min   = 0.0;
warning_max = 0.0;
warning_min = 0.0;
title.empty();

/*
-----------------
Create the bitmap
----------------- */
dc  = GetDC( MainPlotWindow );

old_font = (HFONT) SelectObject( dc, GetStockObject(SYSTEM_FONT) );
get_text_size( w, h, dc, dummy_string );
SelectObject( dc, old_font );

w  += 4;
h  += 4;

bitmap.init( dc, w, h );

ReleaseDC( MainPlotWindow, dc );

return DATA_ARRAY_CLASS::init( nof_points );
}

/***********************************************************************
*                          MULTI_PLOT_CLASS                            *
*                               CLEANUP                                *
***********************************************************************/
void MULTI_PLOT_CLASS::cleanup( void )
{
if ( sp )
    delete[] sp;

n  = 0;
sp = 0;
}

/***********************************************************************
*                          MULTI_PLOT_CLASS                            *
*                                INIT                                  *
***********************************************************************/
BOOLEAN MULTI_PLOT_CLASS::init( int32 nof_plots )
{
cleanup();

if ( nof_plots > 0 )
    {
    n = nof_plots;
    sp = new SINGLE_PLOT_CLASS[n];
    if ( sp )
        {
        return TRUE;
        }
    else
        {
        n = 0;
        cleanup();
        }
    }

return FALSE;
}

/***********************************************************************
*                          MULTI_PLOT_CLASS                            *
*                           SET_NOF_POINTS                             *
***********************************************************************/
void MULTI_PLOT_CLASS::set_nof_points( int32 nof_points )
{
int32 i;
for ( i=0; i<n; i++ )
    sp[i].init( nof_points );

}

/***********************************************************************
*                          MULTI_PLOT_CLASS                            *
*                                 []                                   *
***********************************************************************/
SINGLE_PLOT_CLASS & MULTI_PLOT_CLASS::operator[]( int32 i )
{
if ( i >= 0 && i < n )
    return sp[i];

return EmptyPlot;
}
