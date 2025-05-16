#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"
#include "..\include\visiplot.h"

#define _VISI_PLOT_SOURCE_

TCHAR VisiPlotClassName[]  = TEXT( "VisiPlotClass" );
TCHAR VisiAxisClassName[]  = TEXT( "VisiAxisClass" );

/***********************************************************************
*                              PEN_ENTRY                               *
*                                  =                                   *
***********************************************************************/
void PEN_ENTRY::operator=( const PEN_ENTRY & sorc )
{
color = sorc.color;
style = sorc.style;
width = sorc.width;
}

/***********************************************************************
*                             CURVE_ENTRY                              *
*                             CURVE_ENTRY                              *
***********************************************************************/
CURVE_ENTRY::CURVE_ENTRY()
{
pa = 0;
n = 0;
max_n = 0;
delta_x = 1.0;
delta_y = 1.0;
}

/***********************************************************************
*                             CURVE_ENTRY                              *
*                                CLEAR                                 *
***********************************************************************/
void CURVE_ENTRY::clear( void )
{
if ( pa )
    {
    delete[] pa;
    pa = 0;
    }

n     = 0;
max_n = 0;
}

/***********************************************************************
*                             CURVE_ENTRY                              *
*                            ~CURVE_ENTRY                              *
***********************************************************************/
CURVE_ENTRY::~CURVE_ENTRY()
{
clear();
}

/***********************************************************************
*                             CURVE_ENTRY                              *
*                              transfer                                *
*                                                                      *
* This routine transfers a curve entry to a new location curve entry.  *
*                                                                      *
***********************************************************************/
void CURVE_ENTRY::transfer( CURVE_ENTRY & sorc )
{
pen = sorc.pen;
if ( pa )
    delete[] pa;

pa    = sorc.pa;
n     = sorc.n;
max_n = sorc.max_n;

/*
------------------------------------------------------------------------
I have taken the arrays from the source. Zero the sorc so he doesn't try
to delete the array.
------------------------------------------------------------------------ */
sorc.max_n = 0;
sorc.n     = 0;
sorc.pa    = 0;
extent     = sorc.extent;
delta_x    = sorc.delta_x;
delta_y    = sorc.delta_y;
}

/***********************************************************************
*                             CURVE_ENTRY                              *
*                             SET_EXTENTS                              *
***********************************************************************/
void CURVE_ENTRY::set_extents( double x1, double y1, double x2, double y2 )
{
extent.set( x1, y1, x2, y2 );
delta_x       = x2 - x1;
if ( delta_x == 0.0 )
    delta_x = 1.0;
delta_y       = y2 - y1;
if ( delta_y == 0.0 )
    delta_y = 1.0;
}

/***********************************************************************
*                             CURVE_ENTRY                              *
*                             SET_EXTENTS                              *
***********************************************************************/
void CURVE_ENTRY::set_extents( REAL_RECT & sorc )
{
set_extents( sorc.left, sorc.top, sorc.right, sorc.bottom );
}

/***********************************************************************
*                             CURVE_ENTRY                              *
*                                 ADD                                  *
***********************************************************************/
bool CURVE_ENTRY::add( HWND w, int number_of_new_points, REAL_POINT * p )
{
int i;
int first_index;
REAL_POINT * new_points;
bool    status;
HDC     dc;
RECT    r;

if ( !p )
    return false;

status = false;

if ( max_n < (n + number_of_new_points) )
    {
    if ( number_of_new_points == 1 )
        {
        if ( max_n > 0 )
            max_n *= 2;
        else
            max_n = 20;
        }
    else
        {
        max_n = n + number_of_new_points;
        }

    new_points = new REAL_POINT[max_n];
    if ( new_points )
        {
        if ( pa )
            {
            for ( i=0; i<n; i++ )
                new_points[i] = pa[i];
            delete[] pa;
            }
        }
    pa = new_points;
    }

if ( max_n >= (n+number_of_new_points) )
    {
    if ( n > 0 )
        first_index = n-1;
    else
        first_index = n;

    for ( i=0; i<number_of_new_points; i++ )
        {
        pa[n] = p[i];
        n++;
        }

    if ( w )
        {
        /*
        -------------------
        Plot the new points
        ------------------- */
        dc = GetDC( w );
        GetClientRect( w, &r );
        draw( dc, w, r, first_index, n-1 );
        ReleaseDC( w, dc );
        }

    status = true;
    }

delete[] p;
return status;
}

/***********************************************************************
*                             CURVE_ENTRY                              *
*                                DRAW                                  *
***********************************************************************/
bool CURVE_ENTRY::draw( HDC dc, HWND w, RECT & r, int first_index, int last_index )
{
HPEN     newpen;
HPEN     oldpen;
int      i;
int      ix;
int      iy;
double   x;
double   y;
bool     was_out_of_bounds;
bool     is_out_of_bounds;

if ( first_index >= n )
    return false;

if ( last_index >= n )
    last_index = n-1;

if ( (last_index - first_index) < 1 )
    return false;

newpen = CreatePen( pen.style, pen.width, pen.color );
if ( newpen != NULL )
    {
    oldpen = (HPEN) SelectObject( dc, newpen );

    is_out_of_bounds  = false;

    i = first_index;

    x = pa[i].x - extent.left;
    if ( x < 0 )
        {
        x = 0;
        is_out_of_bounds = true;
        }

    x *= r.right;
    x /= delta_x;

    y = pa[i].y - extent.top;
    if ( y < 0 )
        {
        y = 0;
        is_out_of_bounds = true;
        }

    y *= r.bottom;
    y /= delta_y;
    y = r.bottom - y;

    if ( y >= r.bottom )
        {
        y = r.bottom -1;
        is_out_of_bounds = true;
        }

    MoveToEx( dc, x, y, 0 );
    i++;

    while ( i <= last_index )
        {
        was_out_of_bounds = is_out_of_bounds;
        is_out_of_bounds  = false;

        x = pa[i].x - extent.left;
        if ( x < 0 )
            {
            x = 0;
            is_out_of_bounds = true;
            }

        x *= r.right;
        x /= delta_x;

        if ( x >= r.right )
            {
            x = r.right;
            is_out_of_bounds = TRUE;
            }

        y = pa[i].y - extent.top;
        if ( y < 0 )
            {
            y = 0;
            is_out_of_bounds = true;
            }

        y *= r.bottom;
        y /= delta_y;

        if ( y >= r.bottom )
            {
            y = r.bottom -1;
            is_out_of_bounds = true;
            }
        y = r.bottom - y;

        ix = round( x, 1.0 );
        iy = round( y, 1.0 );

        if ( is_out_of_bounds && was_out_of_bounds )
            MoveToEx( dc, ix, iy, 0 );
        else
            LineTo( dc, ix, iy );
        i++;
        }

    SelectObject( dc, oldpen );
    DeleteObject( newpen );
    }

return true;
}

/***********************************************************************
*                             PLOT_CURVES                              *
***********************************************************************/
static void plot_curves( HWND w )
{
HDC         dc;
PAINTSTRUCT ps;
RECT        r;
int         i;
VISI_PLOT_ENTRY * vp;

vp = (VISI_PLOT_ENTRY *) GetWindowLong( w, GWL_USERDATA );
if ( !vp )
    return;

GetClientRect( w, &r );

dc = BeginPaint( w, &ps );

for ( i=0; i<vp->nof_curves; i++ )
    vp->curve[i].draw( dc, w, r );

EndPaint( w, &ps );
}

/***********************************************************************
*                          VISI_PLOT_ENTRY                             *
*                            CURVE_EXISTS                              *
*                                                                      *
* Check to see that the curve exists. If not, try to create it.        *
*                                                                      *
***********************************************************************/
bool VISI_PLOT_ENTRY::curve_exists( int curve_index )
{
if ( nof_curves <= curve_index )
    {
    CURVE_ENTRY * new_curves;
    int n;
    int i;

    n = curve_index + 1;
    new_curves = new CURVE_ENTRY[n];
    if ( new_curves )
        {
        if ( curve )
            {
            for ( i=0; i<nof_curves; i++ )
                new_curves[i].transfer( curve[i] );

            delete[] curve;
            }
        curve      = new_curves;
        nof_curves = n;
        }
    }

if ( nof_curves > curve_index )
    return true;

return false;
}

/***********************************************************************
*                          VISI_PLOT_ENTRY                             *
*                         ~VISI_PLOT_ENTRY                             *
***********************************************************************/
VISI_PLOT_ENTRY::~VISI_PLOT_ENTRY()
{
if ( curve )
    {
    delete[] curve;
    curve = 0;
    nof_curves = 0;
    }
}

/***********************************************************************
*                       VISI_AXIS_ENTRY                             *
*                       VISI_AXIS_ENTRY                             *
***********************************************************************/
VISI_AXIS_ENTRY::VISI_AXIS_ENTRY()
{
type               = X_AXIS_TYPE;
plot_window_handle = 0;
real_minimum       = 0.0;
real_maximum       = 100.0;
left_places        = 3;
right_places       = 2;
nof_major_tics     = 3;
nof_minor_tics     = 3;
};

/***********************************************************************
*                         VISI_AXIS_ENTRY                              *
*                         SET_PLOT_WINDOW                              *
***********************************************************************/
void VISI_AXIS_ENTRY::set_plot_window( HWND my_handle, HWND new_plot_window_handle )
{
RECT  r;
POINT p;

plot_window_handle = new_plot_window_handle;

if ( plot_window_handle )
    {
    GetClientRect( plot_window_handle, &r );
    p.x = r.left;
    p.y = r.bottom;
    ClientToScreen( plot_window_handle, &p );
    ScreenToClient( my_handle, &p );

    if ( type & X_AXIS_TYPE )
        minimum = p.x;
    else if ( type & LEFT_AXIS_TYPE || type & RIGHT_AXIS_TYPE )
        minimum = p.y;

    p.x = r.right;
    p.y = r.top;
    ClientToScreen( plot_window_handle, &p );
    ScreenToClient( my_handle, &p );
    if ( type & X_AXIS_TYPE )
        maximum = p.x;
    else if ( type & LEFT_AXIS_TYPE || type & RIGHT_AXIS_TYPE )
        maximum = p.y;

    GetClientRect( my_handle, &r );
    if ( type & X_AXIS_TYPE )
        {
        if ( minimum < 0 )
            minimum = 0;
        if ( maximum >= r.right )
            maximum = r.right - 1;
        }
    else if ( type & LEFT_AXIS_TYPE || type & RIGHT_AXIS_TYPE )
        {
        if ( maximum < 0 )
            maximum = 0;
        if ( minimum >= r.bottom )
            minimum = r.bottom - 1;
        }
    }

}

/***********************************************************************
*                         VISI_AXIS_ENTRY                              *
*                         DRAW_LEFT_AXIS                               *
***********************************************************************/
void VISI_AXIS_ENTRY::draw_left_axis( HDC dc, HWND w, RECT & r )
{
UINT   align_options;
int    cw;
int    i;
int    n;
int    range;
int    y;
double realvalue;
double realrange;
TCHAR  s[MAX_DOUBLE_LEN+1];

if ( PRIMARYLANGID(GetUserDefaultLangID())==LANG_JAPANESE )
    align_options = VTA_BASELINE | TA_RIGHT;
else
    align_options = TA_BASELINE | TA_RIGHT;

SetTextAlign( dc, align_options );

r.right--;
r.top++;
r.bottom--;

if ( plot_window_handle )
    {
    r.top    = maximum;
    r.bottom = minimum;
    }

MoveToEx( dc, r.right, r.top, 0 );
LineTo( dc, r.right, r.bottom );

cw = average_character_width( dc );

n = nof_major_tics - 1;
if ( n <= 0 )
    n = 1;

range = r.bottom - r.top;
realrange = real_maximum - real_minimum;

for ( i=0; i<=n; i++ )
    {
    y = i * range;
    y /= n;
    y += r.top;
    MoveToEx( dc, r.right, y, 0 );
    LineTo( dc, r.right-cw, y );

    realvalue = real_maximum - double(i)*realrange / double(n);

    lstrcpy( s, fixed_string( realvalue, left_places, right_places) );
    TextOut( dc, r.right-cw, y, s, lstrlen(s) );
    }
}

/***********************************************************************
*                         VISI_AXIS_ENTRY                              *
*                         DRAW_RIGHT_AXIS                              *
***********************************************************************/
void VISI_AXIS_ENTRY::draw_right_axis( HDC dc, HWND w, RECT & r )
{

int    cw;
int    i;
int    n;
int    range;
int    y;
double realvalue;
double realrange;
TCHAR  s[MAX_DOUBLE_LEN+1];

SetTextAlign( dc, TA_BASELINE | TA_LEFT );

r.right--;
r.top++;
r.bottom--;

if ( plot_window_handle )
    {
    r.top    = maximum;
    r.bottom = minimum;
    }

MoveToEx( dc, r.left, r.top, 0 );
LineTo( dc, r.left, r.bottom );

cw = average_character_width( dc );

n = nof_major_tics - 1;
if ( n <= 0 )
    n = 1;

range = r.bottom - r.top;
realrange = real_maximum - real_minimum;

for ( i=0; i<=n; i++ )
    {
    y = i * range;
    y /= n;
    y += r.top;
    MoveToEx( dc, r.left, y, 0 );
    LineTo( dc, r.left+cw, y );

    realvalue = real_maximum - double(i)*realrange / double(n);

    lstrcpy( s, fixed_string( realvalue, left_places, right_places) );
    TextOut( dc, r.left+cw, y, s, lstrlen(s) );
    }
}

/***********************************************************************
*                     VISI_AXIS_ENTRY::DRAW_X_AXIS                     *
***********************************************************************/
void VISI_AXIS_ENTRY::draw_x_axis( HDC dc, HWND w, RECT & r )
{
int    h;
int    i;
int    j;
int    n;
int    range;
int    x;
double realvalue;
double realrange;
TCHAR  s[MAX_DOUBLE_LEN+1];

SetTextAlign( dc, TA_TOP | TA_CENTER );

r.right--;
r.bottom--;

if ( plot_window_handle )
    {
    r.left  = minimum;
    r.right = maximum;
    }

h = character_height( dc );
h /= 2;

MoveToEx( dc, r.left, r.top, 0 );
LineTo( dc, r.right, r.top );

n = nof_major_tics - 1;
if ( n <= 0 )
    n = 1;

range     = r.right - r.left;
realrange = real_maximum - real_minimum;

for ( i=0; i<=n; i++ )
    {
    x = i * range;
    x /= n;
    x += r.left;
    MoveToEx( dc, x, r.top, 0 );
    LineTo( dc, x, r.top+h );

    realvalue = real_minimum + double(i)*realrange / double(n);

    lstrcpy( s, fixed_string( realvalue, left_places, right_places) );
    TextOut( dc, x, r.top+h, s, lstrlen(s) );
    }

/*
-------------------
Draw the minor tics
------------------- */
h /= 2;
n += n * nof_minor_tics;
i = 1;
while ( i <= n )
    {
    for ( j=0; j<nof_minor_tics; j++ )
        {
        x = i * range;
        x /= n;
        x += r.left;
        MoveToEx( dc, x, r.top, 0 );
        LineTo( dc, x, r.top+h );
        i++;
        }
    /*
    ------------------
    Skip the major tic
    ------------------ */
    i++;
    }

}

/***********************************************************************
*                          VISI_AXIS_ENTRY                             *
*                               DRAW                                   *
***********************************************************************/
bool VISI_AXIS_ENTRY::draw( HDC dc, HWND w )
{
RECT        r;
HPEN        newpen;
HPEN        oldpen;

GetClientRect( w, &r );

SetTextColor( dc, pen.color );
SetBkMode(    dc, TRANSPARENT );

newpen = CreatePen( pen.style, pen.width, pen.color );
if ( newpen != NULL )
    {
    oldpen = (HPEN) SelectObject( dc, newpen );

    if ( type & X_AXIS_TYPE )
        draw_x_axis( dc, w, r );
    else if ( type & LEFT_AXIS_TYPE )
        draw_left_axis( dc, w, r );
    else if ( type & RIGHT_AXIS_TYPE )
        draw_right_axis( dc, w, r );

    SelectObject( dc, oldpen );
    DeleteObject( newpen );

    return true;
    }

return false;
}

/***********************************************************************
*                        VISIAXIS_WINDOW_CLASS                         *
***********************************************************************/
VISIAXIS_WINDOW_CLASS::VISIAXIS_WINDOW_CLASS()
{
}

/***********************************************************************
*                        VISIAXIS_WINDOW_CLASS                         *
***********************************************************************/
VISIAXIS_WINDOW_CLASS::~VISIAXIS_WINDOW_CLASS()
{
}

/***********************************************************************
*                        VISIAXIS_WINDOW_CLASS                         *
*                           SET_PLOT_WINDOW                            *
***********************************************************************/
void VISIAXIS_WINDOW_CLASS::set_plot_window( HWND w )
{
post( WM_SET_AXIS_PLOT_WINDOW, (WPARAM) w, 0L );
}

/***********************************************************************
*                        VISIAXIS_WINDOW_CLASS                         *
*                            SET_PEN_COLOR                             *
***********************************************************************/
void VISIAXIS_WINDOW_CLASS::set_pen_color( COLORREF pencolor )
{
post( WM_SET_AXIS_PEN_COLOR, (WPARAM) pencolor, 0L );
}

/***********************************************************************
*                        VISIAXIS_WINDOW_CLASS                         *
*                            SET_TIC_COUNT                             *
***********************************************************************/
void VISIAXIS_WINDOW_CLASS::set_tic_count( int nof_major_tics, int nof_minor_tics )
{
post( WM_SET_NOF_MAJOR_TICS,   (WPARAM) nof_major_tics,      0L );
post( WM_SET_NOF_MINOR_TICS,   (WPARAM) nof_minor_tics,      0L );
}

/***********************************************************************
*                        VISIAXIS_WINDOW_CLASS                         *
*                           SET_AXIS_LIMITS                            *
***********************************************************************/
void VISIAXIS_WINDOW_CLASS::set_axis_limits( double minimum, double maximum )
{
double * dp;
dp = new double[2];
if ( dp )
    {
    dp[0] = minimum;
    dp[1] = maximum;
    post( WM_SET_REAL_AXIS_LIMITS, (WPARAM) dp, 0L );
    }
}

/***********************************************************************
*                        VISIAXIS_WINDOW_CLASS                         *
*                            SET_AXIS_TYPE                             *
***********************************************************************/
void VISIAXIS_WINDOW_CLASS::set_axis_type( BITSETYPE new_type )
{
post( WM_SET_AXIS_TYPE, (WPARAM) new_type, 0L );
}

/***********************************************************************
*                        VISIPLOT_WINDOW_CLASS                         *
***********************************************************************/
VISIPLOT_WINDOW_CLASS::VISIPLOT_WINDOW_CLASS()
{
}

/***********************************************************************
*                        VISIPLOT_WINDOW_CLASS                         *
***********************************************************************/
VISIPLOT_WINDOW_CLASS::~VISIPLOT_WINDOW_CLASS()
{
}

/***********************************************************************
*                        VISIPLOT_WINDOW_CLASS                         *
*                             CLEAR_CURVE                              *
***********************************************************************/
void VISIPLOT_WINDOW_CLASS::clear_curve( int curve_number )
{
post( WM_CLEAR_VISIPLOT_CURVE, curve_number, 0L );
}


/***********************************************************************
*                        VISIPLOT_WINDOW_CLASS                         *
*                           SET_PEN_COLOR                              *
***********************************************************************/
void VISIPLOT_WINDOW_CLASS::set_pen_color( int curve_number, COLORREF pencolor )
{
post( WM_SET_VISIPLOT_PEN_COLOR, (WPARAM) curve_number, (LPARAM) pencolor );
}

/***********************************************************************
*                        VISIPLOT_WINDOW_CLASS                         *
*                           SET_PEN_WIDTH                              *
***********************************************************************/
void VISIPLOT_WINDOW_CLASS::set_pen_width( int curve_number, int width )
{
post( WM_SET_VISIPLOT_PEN_WIDTH, (WPARAM) curve_number, (LPARAM) width );
}

/***********************************************************************
*                        VISIPLOT_WINDOW_CLASS                         *
*                          set_plot_limits                             *
***********************************************************************/
void VISIPLOT_WINDOW_CLASS::set_plot_limits( int curve_number, double x_min, double x_max, double y_min, double y_max )
{
REAL_RECT * rp;

rp = new REAL_RECT;
if ( rp )
    {
    rp->set( x_min, y_min, x_max, y_max );
    post( WM_SET_VISIPLOT_LIMITS, (WPARAM) curve_number, (LPARAM) rp );
    }
}

/***********************************************************************
*                        VISIPLOT_WINDOW_CLASS                         *
*                                add                                   *
***********************************************************************/
void VISIPLOT_WINDOW_CLASS::add( int curve_number, int nof_points, REAL_POINT * p )
{
WPARAM wParam;

wParam = MAKEWPARAM( (WORD) curve_number, (WORD) nof_points );
post( WM_ADD_VISIPLOT_DATA, wParam, (LPARAM) p );
}

/***********************************************************************
*                           PAINT_AXIS                                 *
***********************************************************************/
static void paint_axis( HWND w )
{
HDC         dc;
PAINTSTRUCT ps;
VISI_AXIS_ENTRY * xp;

HFONT       oldfont;
LOGFONT    lf;
POINT      p;
static TCHAR ArialFontName[] = TEXT( "Arial" );

dc = BeginPaint( w, &ps );

p.y = 13;
p.x = 0;

DPtoLP( dc, &p, 1 );

lf.lfHeight          = p.y;
lf.lfWidth           = 0;
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
lstrcpy( lf.lfFaceName, ArialFontName );

oldfont = (HFONT) SelectObject( dc, CreateFontIndirect(&lf) );

xp = (VISI_AXIS_ENTRY *) GetWindowLong( w, GWL_USERDATA );
if ( xp )
    xp->draw( dc, w );

DeleteObject( SelectObject(dc, oldfont) );
EndPaint( w, &ps );
}

/***********************************************************************
*                          VISI_AXIS_PROC                              *
***********************************************************************/
LRESULT CALLBACK visi_axis_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

VISI_AXIS_ENTRY * xp;

if ( msg==WM_DESTROY || (msg >= WM_LOW_AXIS_MSG_VALUE && msg <= WM_HIGH_AXIS_MSG_VALUE) )
    {
    xp = (VISI_AXIS_ENTRY *) GetWindowLong( hWnd, GWL_USERDATA );
    if ( msg != WM_DESTROY && !xp )
        return 0;
    }

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        xp = new VISI_AXIS_ENTRY;
        if ( !xp )
            return -1;
        SetWindowLong( hWnd, GWL_USERDATA, (LONG) xp );
        return 0;

    case WM_DESTROY:
        if ( xp )
            {
            delete xp;
            xp = 0;
            SetWindowLong( hWnd, GWL_USERDATA, (LONG) xp );
            }
        return 0;

    case WM_SET_REAL_AXIS_LIMITS:
        double * dp;
        dp = (double *) wParam;
        if ( dp )
            {
            xp->real_minimum = dp[0];
            xp->real_maximum = dp[1];
            delete[] dp;

            if ( xp->real_maximum >= 10000.0 )
                {
                xp->left_places = 5;
                xp->right_places = 0;
                }
            else if ( xp->real_maximum >= 1000.0 )
                {
                xp->left_places = 4;
                xp->right_places = 1;
                }
            else
                {
                xp->left_places = 3;
                xp->right_places = 2;
                }
            }
        return 0;

    case WM_SET_NOF_MAJOR_TICS:
        xp->nof_major_tics = (int) wParam;
        return 0;

    case WM_SET_NOF_MINOR_TICS:
        xp->nof_minor_tics = (int) wParam;
        return 0;

    case WM_SET_AXIS_TYPE:
        xp->type = (BITSETYPE) wParam;
        return 0;

    case WM_SET_AXIS_PEN_COLOR:
        xp->pen.color = (int) wParam;
        return 0;

    case WM_SET_AXIS_PLOT_WINDOW:
        xp->set_plot_window( hWnd, (HWND) wParam );
        InvalidateRect( hWnd, 0, TRUE );
        return 0;

    case WM_PAINT:
        paint_axis( hWnd );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                          VISI_PLOT_PROC                              *
***********************************************************************/
LRESULT CALLBACK visi_plot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int id;
static int cmd;

VISI_PLOT_ENTRY * vp;

if ( msg==WM_DESTROY || (msg >= WM_LOW_VISIPLOT_MSG_VALUE && msg <= WM_HIGH_VISIPLOT_MSG_VALUE) )
    {
    vp = (VISI_PLOT_ENTRY *) GetWindowLong( hWnd, GWL_USERDATA );
    if ( msg != WM_DESTROY && !vp )
        return 0;
    }

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        vp = new VISI_PLOT_ENTRY;
        if ( !vp )
            return -1;
        SetWindowLong( hWnd, GWL_USERDATA, (LONG) vp );
        return 0;

    case WM_DESTROY:
        if ( vp )
            {
            delete vp;
            vp = 0;
            SetWindowLong( hWnd, GWL_USERDATA, (LONG) vp );
            }
        return 0;

    case WM_ADD_VISIPLOT_DATA:
        if ( vp->curve_exists(id) )
            vp->curve[id].add( hWnd, cmd, (REAL_POINT *) lParam );
        return 0;

    case WM_SET_VISIPLOT_PEN_WIDTH:
        if ( vp->curve_exists(id) )
            vp->curve[id].pen.width = (int) lParam;
        return 0;

    case WM_SET_VISIPLOT_PEN_COLOR:
        if ( vp->curve_exists(id) )
            vp->curve[id].pen.color = (int) lParam;
        return 0;

    case WM_SET_VISIPLOT_PEN_STYLE:
        if ( vp->curve_exists(id) )
            vp->curve[id].pen.style = (int) lParam;
        return 0;

    case WM_SET_VISIPLOT_LIMITS:
        if ( vp->curve_exists(id) )
            {
            REAL_RECT * rp;
            rp = (REAL_RECT *) lParam;
            if ( rp )
                {
                vp->curve[id].set_extents( *rp );
                delete rp;
                }
            }
        return 0;

    case WM_CLEAR_VISIPLOT_CURVE:
        if ( vp->curve_exists(id) )
            vp->curve[id].clear();
        return 0;

    case WM_PAINT:
        plot_curves( hWnd );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                          REGISTER_VISI_PLOT                          *
***********************************************************************/
void register_visi_plot( HINSTANCE  main_instance )
{
static bool firstime = true;
WNDCLASS wc;

if ( !firstime )
    return;
firstime = false;

wc.lpszClassName = VisiPlotClassName;
wc.hInstance     = main_instance;
wc.lpfnWndProc   = (WNDPROC) visi_plot_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = NULL;
wc.lpszMenuName  = NULL;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );


wc.lpszClassName = VisiAxisClassName;
wc.lpfnWndProc   = (WNDPROC) visi_axis_proc;

RegisterClass( &wc );
}

