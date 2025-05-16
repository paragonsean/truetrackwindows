#include <windows.h>
#include <math.h>

#include "..\include\visiparm.h"
#include "..\include\genlist.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\plotclas.h"

static const TCHAR DecimalPointChar = TEXT( '.' );
static const TCHAR NullChar         = TEXT( '\0' );
static const TCHAR ZeroChar         = TEXT( '0' );

static COLORREF BackgroundColor = 0;
static int32    SymbolHeight    = 0;
static int32    SymbolWidth     = 0;

int32 draw_vertical_string( HDC dc, TCHAR * sorc, int32 x, int32 y );

/***********************************************************************
*                                 FINDAXIS                             *
***********************************************************************/
AXIS_ENTRY * PLOT_CLASS::findaxis( int32 id_to_find )
{
AXIS_ENTRY * ae;
 
axes.rewind();
while ( TRUE )
    {
    ae = (AXIS_ENTRY *) axes.next();
    if ( !ae )
        break;
    if ( ae->id == id_to_find )
        return ae;
    }

return 0;
}
 
/***********************************************************************
*                            GET_PIXEL_EQUATION                        *
*                                                                      *
*  Returns the equation such that y = int32( a + b*x ) is the correct  *
*  pixel position.                                                     *
***********************************************************************/
BOOLEAN PLOT_CLASS::get_pixel_equation( float & a, float & b, int32 axis_id )
{

AXIS_ENTRY * ae;

a = 0.0;
b = 1.0;

ae = findaxis( axis_id );
if ( ae )
    {
    pixel_equation( a, b, base[ae->base_axis_index].start_pos, base[ae->base_axis_index].end_pos, ae->min_value, ae->max_value );
    return TRUE;
    }
 
return FALSE;
}
 
/***********************************************************************
*                              WIDEST_CHAR                             *
***********************************************************************/
int32 PLOT_CLASS::widest_char( TCHAR * sorc )
{
SIZE  sorc_size;
int32 dx;
 
dx = 0;
 
while ( *sorc )
    {
    GetTextExtentPoint32( dc, sorc, 1, &sorc_size );
    if ( dx < sorc_size.cx )
        dx = sorc_size.cx;
    sorc++;
    }

return dx;
}
 
/***********************************************************************
*                             EMPTY_CURVES                             *
***********************************************************************/
void PLOT_CLASS::empty_curves( void )
{
CURVE_ENTRY * ce;
 
curves.rewind();
while ( TRUE )
    {
    ce = (CURVE_ENTRY *) curves.next();
    if ( !ce )
        break;
    if ( ce->data )
        delete[] ce->data;
    delete ce;
    }
 
curves.remove_all();
}
 
/***********************************************************************
*                             EMPTY_AXES                               *
***********************************************************************/
void PLOT_CLASS::empty_axes( void )
{
AXIS_ENTRY  * ae;
 
axes.rewind();

while ( TRUE )
    {
    ae = (AXIS_ENTRY *) axes.next();
    if ( !ae )
        break;
    delete ae;
    }

axes.remove_all();
}

/***********************************************************************
*                             EMPTY_LABELS                             *
***********************************************************************/
void PLOT_CLASS::empty_labels( void )
{
LABEL_ENTRY * le;
int32         i;

for ( i=0; i<NOF_BASE_AXES; i++ )
    {
    base[i].labels.rewind();
    while ( TRUE )
        {
        le = (LABEL_ENTRY *) base[i].labels.next();
        if ( !le )
            break;
        delete le;
        }
    base[i].labels.remove_all();
    }

}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void PLOT_CLASS::cleanup( void )
{
empty_axes();
empty_curves();
empty_labels();
}

/***********************************************************************
*                              PLOT_CLASS                              *
***********************************************************************/
PLOT_CLASS::PLOT_CLASS()
{
int i;

w = 0;
perimeter.top    = 0;
perimeter.bottom = 0;
perimeter.left   = 0;
perimeter.right  = 0;

need_to_show_symbols = TRUE;
have_new_label       = FALSE;
have_new_axis        = FALSE;
have_new_curve       = FALSE;

for ( i=0; i<NOF_BASE_AXES; i++ )
    {
    base[i].start_pos  = 0;
    base[i].end_pos    = 0;
    base[i].nof_tics   = 0;
    base[i].tic_length = 1;
    base[i].color      = RGB(0,0,0);
    }

}

/***********************************************************************
*                             ~PLOT_CLASS                              *
***********************************************************************/
PLOT_CLASS::~PLOT_CLASS()
{
cleanup();
}

/***********************************************************************
*                               ADD_LABEL                              *
***********************************************************************/
BOOLEAN PLOT_CLASS::add_label( int32 axis, TCHAR * sorc, COLORREF new_color )
{

LABEL_ENTRY * le;

if ( axis < 0 || axis >= NOF_BASE_AXES )
    return FALSE;

le = new LABEL_ENTRY;
if ( le )
    {
    le->t     = sorc;
    le->color = new_color;
    le->pos.x = 0;
    le->pos.y = 0;
    if ( base[axis].labels.append(le) )
        {
        have_new_label = TRUE;
        return TRUE;
        }
    else
        {
        delete le;
        }
    }

return FALSE;
}

/***********************************************************************
*                             DRAW_X_TICS                              *
***********************************************************************/
void PLOT_CLASS::draw_x_tics( void )
{
int32        dx;
int32        i;
int32        n;
int32        y1;
int32        y2;
int32        x;
int32        x0;

AXIS_ENTRY * ae;
float        dv;
float        v;
float        v0;
float        vn;
int32        left_places;
int32        right_places;

TCHAR      * cp;

n = base[X_AXIS_INDEX].nof_tics;
y1 = base[Y_AXIS_INDEX].start_pos;

y2 = y1 + base[Y_AXIS_INDEX].tic_length;

x0 = base[X_AXIS_INDEX].start_pos;
dx = base[X_AXIS_INDEX].end_pos - x0;

v0 = 0.0;
dv = 0.0;
vn = (float) n;
left_places  = 1;
right_places = 1;

axes.rewind();
while ( TRUE )
    {
    ae = (AXIS_ENTRY *) axes.next();
    if ( !ae )
        break;
    if ( ae->base_axis_index == X_AXIS_INDEX )
        {
        v0 = ae->min_value;
        dv = ae->max_value - v0;
        left_places = whole_places( ae->max_value );
        right_places = ae->decimal_places;
        break;
        }
    }

SetTextAlign( dc, TA_TOP | TA_CENTER );
SetTextColor( dc, base[X_AXIS_INDEX].color );
SetBkColor( dc, BackgroundColor );

for ( i=0; i<=n; i++ )
    {
    x = i * dx;
    x /= n;
    x += x0;

    v = dv * float( i );
    v /= vn;
    v += v0;

    MoveToEx( dc, x,  y1,   0 );
    LineTo(   dc, x,  y2 );

    cp = fixed_string( v, left_places, right_places );
    TextOut( dc, x, y2, cp, lstrlen(cp) );
    }

}

/***********************************************************************
*                             DRAW_Y_TICS                              *
***********************************************************************/
void PLOT_CLASS::draw_y_tics( void )
{
int32        dy;
int32        i;
int32        n;
int32        x1;
int32        x2;
int32        y;
int32        y0;

AXIS_ENTRY * ae;
float        dv;
float        v;
float        v0;
float        vn;
int32        left_places;
int32        right_places;

TCHAR      * cp;

n = base[Y_AXIS_INDEX].nof_tics;

x2 = base[X_AXIS_INDEX].start_pos;
x1 = x2 - base[X_AXIS_INDEX].tic_length;

y0 = base[Y_AXIS_INDEX].start_pos;
dy = base[Y_AXIS_INDEX].end_pos - y0;

v0 = 0.0;
dv = 0.0;
vn = (float) n;
left_places  = 1;
right_places = 1;

axes.rewind();
while ( TRUE )
    {
    ae = (AXIS_ENTRY *) axes.next();
    if ( !ae )
        break;
    if ( ae->base_axis_index == Y_AXIS_INDEX )
        {
        v0 = ae->min_value;
        dv = ae->max_value - v0;
        left_places = whole_places( ae->max_value );
        right_places = ae->decimal_places;
        break;
        }
    }

SetTextAlign( dc, TA_BASELINE | TA_RIGHT );
SetTextColor( dc, base[Y_AXIS_INDEX].color );
SetBkColor( dc, BackgroundColor );

for ( i=0; i<=n; i++ )
    {
    y = i * dy;
    y /= n;
    y += y0;

    v = dv * float( i );
    v /= vn;
    v += v0;

    MoveToEx( dc, x1,  y,   0 );
    LineTo(   dc, x2,  y );

    cp = fixed_string( v, left_places, right_places );
    TextOut( dc, x1, y, cp, lstrlen(cp) );
    }

}

/***********************************************************************
*                              DRAW_CURVES                             *
***********************************************************************/
void PLOT_CLASS::draw_curves( void )
{
AXIS_ENTRY  * ae;
CURVE_ENTRY * ce;
float  ax;
float  ay;
float  bx;
float  by;
int32  delta;
int32  i;
int32  x;
int32  y;
HBRUSH mybrush;
HPEN   mypen;
HPEN   oldpen;
RECT   r;

SetBkColor( dc, BackgroundColor );

mybrush = GetSysColorBrush( COLOR_WINDOW );
if ( mybrush )
    SelectObject( dc, mybrush );

curves.rewind();
while ( TRUE )
    {
    ce = (CURVE_ENTRY *) curves.next();
    if ( !ce )
        break;

    if ( ce->n < 1 )
        continue;

    if ( !get_pixel_equation( ax, bx, ce->xid) )
        return;

    if ( !get_pixel_equation( ay, by, ce->yid) )
        return;

    ae = findaxis( ce->yid );
    if ( !ae )
        return;

    oldpen = 0;
    mypen  = CreatePen( PS_SOLID, 1, ae->color );
    if ( mypen )
        oldpen = (HPEN) SelectObject( dc, mypen );

    for ( i=0; i<ce->n; i++ )
        {
        x = (int32) (ax + bx*ce->data[i].x );
        y = (int32) (ay + by*ce->data[i].y );

        delta    = SymbolHeight / 2;
        r.top    = y - delta;
        r.bottom = y + delta;

        delta    = SymbolWidth / 2;
        r.left   = x - delta;
        r.right  = x + delta;

        if ( need_to_show_symbols )
            Ellipse( dc, r.left, r.top, r.right, r.bottom );

        if ( i == 0 )
            MoveToEx( dc, x, y, 0 );
        else
            LineTo(   dc, x, y );
        }

    if ( oldpen )
        SelectObject( dc, oldpen );

    if ( mypen )
        DeleteObject( mypen );
    }

have_new_curve = FALSE;
}

/***********************************************************************
*                               DRAW_AXES                              *
***********************************************************************/
void PLOT_CLASS::draw_axes( void )
{
HPEN         xpen;
HPEN         ypen;
HPEN         oldpen;
HPEN         p;

oldpen = 0;
p      = 0;

xpen = CreatePen( PS_SOLID, 1, base[X_AXIS_INDEX].color );
ypen = CreatePen( PS_SOLID, 1, base[Y_AXIS_INDEX].color );

SetBkColor( dc, BackgroundColor );

if ( ypen )
    {
    oldpen = (HPEN) SelectObject( dc, ypen );

    MoveToEx( dc, base[X_AXIS_INDEX].start_pos,  base[Y_AXIS_INDEX].start_pos,   0 );
    LineTo(   dc, base[X_AXIS_INDEX].start_pos,  base[Y_AXIS_INDEX].end_pos );

    draw_y_tics();
    }

if ( xpen )
    {
    p = (HPEN) SelectObject( dc, xpen );

    MoveToEx( dc, base[X_AXIS_INDEX].start_pos,  base[Y_AXIS_INDEX].start_pos,   0 );
    LineTo( dc,   base[X_AXIS_INDEX].end_pos,    base[Y_AXIS_INDEX].start_pos );

    draw_x_tics();
    }

if ( !oldpen )
    oldpen = p;

if ( oldpen )
    SelectObject( dc, oldpen );

if ( xpen )
    DeleteObject( xpen );

if ( ypen )
    DeleteObject( ypen );

have_new_axis = FALSE;
}

/***********************************************************************
*                                 SIZE                                 *
*                                                                      *
*               I assume that all the labels have been set.            *
*               r is the bounding rectangle for the plot.              *
***********************************************************************/
void PLOT_CLASS::size( HWND hWnd, RECT r )
{
int32 i;
int32 n;
int32 x;
int32 y;
int32 char_height;
int32 xpos;
int32 ypos;
int32 width;
int32 dx;
int32 left_places;
int32 right_places;
TEXTMETRIC tm;
TCHAR number[MAX_FLOAT_LEN+1];
AXIS_ENTRY  * ae;
LABEL_ENTRY * le;
SIZE  sorc_size;

w         = hWnd;
perimeter = r;
dc        = GetDC( w );

GetTextMetrics( dc, &tm );

char_height = tm.tmHeight;

base[X_AXIS_INDEX].tic_length = tm.tmAveCharWidth;
base[Y_AXIS_INDEX].tic_length = tm.tmHeight/2;

/*
-------------------------
Make a square for symbols
------------------------- */
SymbolWidth = tm.tmAveCharWidth * 3;
SymbolWidth /= 2;
SymbolHeight = tm.tmAscent - tm.tmInternalLeading;

y = r.bottom - base[X_AXIS_INDEX].labels.count() * char_height;

base[Y_AXIS_INDEX].start_pos = y - tm.tmExternalLeading - base[Y_AXIS_INDEX].tic_length - char_height;
base[Y_AXIS_INDEX].end_pos   = r.top + char_height;

y  = ( base[Y_AXIS_INDEX].start_pos + base[Y_AXIS_INDEX].end_pos ) / 2;
dx = tm.tmAveCharWidth/4;
x  = r.left + dx;

base[Y_AXIS_INDEX].labels.rewind();
while ( TRUE )
    {
    le = (LABEL_ENTRY *) base[Y_AXIS_INDEX].labels.next();
    if ( !le )
        break;

    ypos = le->t.len() * char_height;
    ypos /= 2;
    ypos = y - ypos;
    if ( ypos < r.top )
        ypos = r.top;

    le->pos.y = ypos;

    width = widest_char( le->t.text() );
    le->pos.x = x + width/2;
    x += width;
    x += dx;
    }

/*
------------------------------------------------------------------
The current x pos is where I will put the tic value. I need to add
the length of the longest value that is going to go here.
------------------------------------------------------------------ */
number[0] = DecimalPointChar;
left_places  = 1;
right_places = 0;

axes.rewind();
while ( TRUE )
    {
    ae = (AXIS_ENTRY *) axes.next();
    if ( !ae )
        break;

    if ( ae->base_axis_index == Y_AXIS_INDEX )
        {
        if ( ae->decimal_places > right_places )
            right_places = ae->decimal_places;

        n = whole_places( ae->max_value );
        if ( n > left_places )
            left_places = n;
        }
    }

n = left_places + right_places;
for ( i=1; i<=n; i++ )
    number[i] = ZeroChar;
n++;
number[n] = NullChar;

GetTextExtentPoint32( dc, number, lstrlen(number), &sorc_size );

base[X_AXIS_INDEX].start_pos = x + sorc_size.cx + base[X_AXIS_INDEX].tic_length;
base[X_AXIS_INDEX].end_pos   = r.right - 3 * tm.tmAveCharWidth;

xpos = ( base[X_AXIS_INDEX].start_pos + base[X_AXIS_INDEX].end_pos ) / 2;
y    = r.bottom - base[X_AXIS_INDEX].labels.count() * char_height;

base[X_AXIS_INDEX].labels.rewind();
while ( TRUE )
    {
    le = (LABEL_ENTRY *) base[X_AXIS_INDEX].labels.next();
    if ( !le )
        break;
    le->pos.x = xpos;
    le->pos.y = y;
    y += char_height;
    }

ReleaseDC( w, dc );
dc = 0;
}

/***********************************************************************
*                                 SIZE                                 *
***********************************************************************/
void PLOT_CLASS::size( HWND hWnd )
{
RECT r;

GetClientRect( hWnd, &r );
size( hWnd, r );
}

/***********************************************************************
*                               SHOW_LABELS                            *
***********************************************************************/
void PLOT_CLASS::show_labels( void )
{
LABEL_ENTRY * le;

SetTextAlign( dc, TA_TOP | TA_CENTER );
SetBkColor(   dc, BackgroundColor );

base[X_AXIS_INDEX].labels.rewind();
while ( TRUE )
    {
    le = (LABEL_ENTRY *) base[X_AXIS_INDEX].labels.next();
    if ( !le )
        break;
    SetTextColor( dc, le->color );
    TextOut( dc, le->pos.x, le->pos.y, le->t.text(), le->t.len() );
    }

base[Y_AXIS_INDEX].labels.rewind();
while ( TRUE )
    {
    le = (LABEL_ENTRY *) base[Y_AXIS_INDEX].labels.next();
    if ( !le )
        break;

    draw_vertical_string( dc, le->t.text(), le->pos.x, le->pos.y );
    }

have_new_label = FALSE;
}

/***********************************************************************
*                                  PAINT                               *
***********************************************************************/
void PLOT_CLASS::paint( HDC dest_dc )
{
int original_mode;
dc = dest_dc;

original_mode = SetBkMode( dc, TRANSPARENT );
BackgroundColor = GetSysColor( COLOR_WINDOW );
show_labels();
draw_axes();
draw_curves();

if ( original_mode != 0 )
    SetBkMode( dc, original_mode );

dc = 0;
}

/***********************************************************************
*                                ADD_AXIS                              *
***********************************************************************/
BOOLEAN PLOT_CLASS::add_axis( int32 new_id, int32 base_axis, float new_min, float new_max, COLORREF new_color )
{
AXIS_ENTRY  * ae;
int32         n;

if ( base_axis < 0 || base_axis >= NOF_BASE_AXES )
    return FALSE;

ae = new AXIS_ENTRY;
if ( ae )
    {
    ae->id              = new_id;
    ae->base_axis_index = base_axis;
    ae->color           = new_color;
    ae->min_value       = new_min;
    ae->max_value       = new_max;

    ae->decimal_places  = 4;

    if ( ae->max_value > 1.0 )
        {
        ae->decimal_places = 0;
        n = (int32) floor( log10(ae->max_value) );
        if ( n < 5 )
            ae->decimal_places = 5-n;
        }

    if ( axes.append(ae) )
        {
        have_new_axis = TRUE;
        return TRUE;
        }
    else
        {
        delete ae;
        }
    }

return FALSE;
}

/***********************************************************************
*                              SET_TIC_COUNT                           *
***********************************************************************/
void PLOT_CLASS::set_tic_count( int32 axis, int32 count )
{

if ( axis < 0 || axis >= NOF_BASE_AXES )
    return;

base[axis].nof_tics = count;
}

/***********************************************************************
*                            SET_DECIMAL_PLACES                        *
***********************************************************************/
void PLOT_CLASS::set_decimal_places( int32 id_to_set, int32 new_count )
{
AXIS_ENTRY * ae;

ae = findaxis( id_to_set );
if ( ae )
    ae->decimal_places = new_count;
}

/***********************************************************************
*                                ADD_CURVE                             *
***********************************************************************/
BOOLEAN PLOT_CLASS::add_curve( int32 x_axis_id, int32 y_axis_id, int32 nof_points, FLOAT_POINT * fp )
{
CURVE_ENTRY * ce;
int32         i;

if ( nof_points < 0 )
    return FALSE;

if ( !findaxis(x_axis_id) )
    return FALSE;

if ( !findaxis(y_axis_id) )
    return FALSE;

ce = new CURVE_ENTRY;
if ( !ce )
    return FALSE;

ce->xid  = x_axis_id;
ce->yid  = y_axis_id;
ce->n    = nof_points;
ce->data = 0;
if ( nof_points > 0 )
    {
    ce->data = new FLOAT_POINT[nof_points];
    if ( !ce->data )
        {
        delete ce;
        return FALSE;
        }
    }

for ( i=0; i<nof_points; i++ )
    ce->data[i] = fp[i];

if ( curves.append(ce) )
    {
    have_new_curve = TRUE;
    return TRUE;
    }

if ( ce->data )
    delete[] ce->data;

delete ce;
return FALSE;
}

/***********************************************************************
*                               REFRESH                                *
***********************************************************************/
void PLOT_CLASS::refresh( void )
{
RECT r;

r = perimeter;

if ( !have_new_label && !have_new_axis )
    {
    r.left   = base[X_AXIS_INDEX].start_pos + 1;
    r.bottom = base[Y_AXIS_INDEX].start_pos - 1;
    }

InvalidateRect( w, &r, TRUE );
}
