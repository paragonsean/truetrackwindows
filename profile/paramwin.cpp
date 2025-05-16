#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\fontclas.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"

#include "..\include\subs.h"

#include "resource.h"
#include "extern.h"

static FONT_LIST_CLASS FontList;
static HFONT FontHandle = INVALID_FONT_HANDLE;
int LineHeight = 1;    /* This is used by paramhis */

static COLORREF ParameterListBackgroundColor = RGB( 192, 192, 192 );

static TCHAR EmptyString[] = TEXT("");

void    choose_corner_parameters();
BOOLEAN is_alarm( short parameter_index, float value );
BOOLEAN is_warning( short parameter_index, float value );
RECT  & my_desktop_client_rect();
void    position_main_plot_window();
TCHAR * resource_string( UINT resource_id );
void    toggle_big_param_display_type( void );

/***********************************************************************
*                      POSITION_PARAM_BOX_WINDOW                       *
***********************************************************************/
void position_parameter_window()
{
RECTANGLE_CLASS dr;
RECTANGLE_CLASS r;
HDC             dc;
HFONT           oldfont;
int             h;

if ( !ParameterWindow )
    return;

dc = GetDC( ParameterWindow );
if ( dc == 0 )
    return;

if ( FontHandle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( dc, FontHandle );
else
    oldfont = INVALID_FONT_HANDLE;

LineHeight = character_height( dc );

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( dc, oldfont );

ReleaseDC( ParameterWindow, dc );

h = NofCornerParameters;
if ( MyScreenType != ZABISH_SCREEN_TYPE )
    {
    h /= 2;
    if ( 2*h < NofCornerParameters )
        h++;
    }
h++;
h *= LineHeight;
dr = my_desktop_client_rect();

r = dr;
if ( MyScreenType == ZABISH_SCREEN_TYPE )
    r.right = r.left + dr.width()/2;

r.top = r.bottom - h - 1;
MoveWindow( ParameterWindow, r.left, r.top, r.width(), r.height(), TRUE );
}

/***********************************************************************
*                             CHOOSE_FONT                              *
***********************************************************************/
static bool choose_font()
{
FONT_CLASS lf;

if ( FontHandle != INVALID_FONT_HANDLE )
    FontList.get_font( lf, FontHandle );

if ( lf.choose(ParameterWindow) )
    {
    put_ini_string( DisplayIniFile, ConfigSection, ParameterWindowFontKey, lf.put() );
    if ( FontHandle != INVALID_FONT_HANDLE )
        FontList.free_handle( FontHandle );
    FontHandle = FontList.get_handle( lf );
    return true;
    }

return false;
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static void paint_me( HWND w )
{
PAINTSTRUCT     ps;
HDC             dc;
STRING_CLASS    s;
RECTANGLE_CLASS r;
RECTANGLE_CLASS rc;
int             cw;
int             ci;
int             i;
int             n;
int             dx;
int             x;
int             y;
int             additional_pixels;
HFONT           oldfont;
COLORREF        old_text_color;
COLORREF        my_text_color;
PARAMETER_ENTRY * p;
float             value;

int  title_width[NOF_PARAMETER_LIST_TITLES] = { 26, 8, 8, 8, 8 };
int  title_position[NOF_PARAMETER_LIST_TITLES];

BeginPaint( w, &ps );
dc = ps.hdc;
GetClientRect( w, &r );

if ( FontHandle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( dc, FontHandle );
else
    oldfont = INVALID_FONT_HANDLE;

rc = r;
rc.bottom = rc.top + LineHeight + 1;

fill_rectangle( dc, rc, ParameterListBackgroundColor );

for ( i=0; i<NOF_PARAMETER_LIST_TITLES; i++ )
    title_width[i]= pixel_width( dc, ParameterListTitle[i].text() );

x = 0;
for ( i=1; i<NOF_PARAMETER_LIST_TITLES-1; i++ )
    maxint( x, title_width[i] );

for ( i=1; i<NOF_PARAMETER_LIST_TITLES-1; i++ )
    title_width[i] = x;

cw = average_character_width( dc );

for ( i=0; i<NOF_PARAMETER_LIST_TITLES; i++ )
    title_width[i] += cw;

if ( MyScreenType != ZABISH_SCREEN_TYPE )
    {
    r.right = (r.left + r.right)/2;
    }
/*
------------------------------------
Get the width of the parameter field
------------------------------------ */
x = r.width();
x *= 26;
x /= 58;
maxint( title_width[0], x );

/*
-------------------------------------------
Sum up the widths of the rest of the titles
------------------------------------------- */
x = cw;
for ( i=1; i<NOF_PARAMETER_LIST_TITLES; i++ )
    x += title_width[i];

if ( x > (r.width() - title_width[0]) )
    title_width[0] = r.width() - x;

x += title_width[0];

additional_pixels = r.width() - x;
additional_pixels /= NOF_PARAMETER_LIST_TITLES;

/*
----------------------------------------------------------------
If there isn't enough room this way, just make them all the same
---------------------------------------------------------------- */
if ( title_width[0] < title_width[1] )
    {
    i = NOF_PARAMETER_LIST_TITLES - 1;
    x = r.width() - cw - title_width[i];
    x /= i;
    for ( i=0; i<NOF_PARAMETER_LIST_TITLES-1; i++ )
        title_width[i] = x;
    }

x = cw;
for ( i=0; i<NOF_PARAMETER_LIST_TITLES; i++ )
    {
    dx = title_width[i] + additional_pixels;
    title_position[i] = x;
    x += dx;
    }

SetTextAlign( dc, TA_TOP | TA_LEFT );
SetTextColor( dc, RGB(0, 0, 0) );
SetBkMode( dc, TRANSPARENT );

x = 0;
n = NofCornerParameters;
if ( MyScreenType != ZABISH_SCREEN_TYPE )
    {
    x = r.width();
    n /= 2;
    if ( 2*n < NofCornerParameters )
        n++;
    }

for ( i=0; i<NOF_PARAMETER_LIST_TITLES; i++ )
    {
    TextOut( dc, title_position[i], 0, ParameterListTitle[i].text(), ParameterListTitle[i].len() );
    if ( x > 0 )
        TextOut( dc, x + title_position[i], 0, ParameterListTitle[i].text(), ParameterListTitle[i].len() );
    }

if ( HaveHeader )
    {
    i = 0;
    x = 0;
    y = LineHeight;
    while ( i < NofCornerParameters )
        {
        if ( i == n )
            {
            y = LineHeight;
            x = r.width();
            }

        ci = CornerParameters[i] - 1;
        if ( ci >= 0 && ci < MAX_PARMS )
            {
            p = &CurrentParam.parameter[ci];
            if ( p->input.type != NO_PARAMETER_TYPE )
                {
                s = p->name;
                TextOut( dc, x+title_position[0], y, s.text(), s.len() );

                s = ascii_float( p->limits[LowLimitType].value );
                TextOut( dc, x+title_position[1], y, s.text(), s.len() );

                s = ascii_float( p->limits[HighLimitType].value );
                TextOut( dc, x+title_position[2], y, s.text(), s.len() );

                value = Parms[ci];
                if ( is_alarm(ci, value) )
                    my_text_color = AlarmColor;
                else if ( is_warning(ci, value) )
                    my_text_color = WarningColor;
                else
                    my_text_color = DialogTextColor;

                old_text_color = SetTextColor( dc, my_text_color );

                s = ascii_float( value );
                TextOut( dc, x+title_position[3], y, s.text(), s.len() );

                SetTextColor( dc, old_text_color );

                s = units_name( p->units );
                TextOut( dc, x+title_position[4], y, s.text(), s.len() );
                }
            }
        i++;
        y += LineHeight;
        }
    }

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( dc, oldfont );

EndPaint( w, &ps );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
if ( FontHandle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( FontHandle );
    FontHandle = INVALID_FONT_HANDLE;
    }
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static void init()
{
int        id[] = { LOW_ALARM_STRING, HIGH_ALARM_STRING, LOW_WARNING_STRING, HIGH_WARNING_STRING };

TCHAR    * cp;
FONT_CLASS lf;
int        i;

cp = get_ini_string( DisplayIniFile, ConfigSection, LowLimitTypeKey );
if ( !unknown(cp) )
    {
    i = (int) asctoint32( cp );
    if ( i >= 0 && i <= WARNING_MAX )
        LowLimitType = i;
    }

cp = get_ini_string( DisplayIniFile, ConfigSection, HighLimitTypeKey );
if ( !unknown(cp) )
    {
    i = (int) asctoint32( cp );
    if ( i >= 0 && i <= WARNING_MAX )
        HighLimitType = i;
    }

ParameterListTitle[0] = resource_string( PARAMETER_STRING );
ParameterListTitle[1] = resource_string( id[LowLimitType] );
ParameterListTitle[2] = resource_string( id[HighLimitType] );
ParameterListTitle[3] = resource_string( PARAMETER_VALUE_STRING );
ParameterListTitle[4] = resource_string( UNITS_TITLE_STRING );

cp = get_ini_string( DisplayIniFile, ConfigSection, ParameterWindowFontKey );
if ( !unknown(cp) )
    {
    lf.get( cp );
    FontHandle = FontList.get_handle( lf );
    }

position_parameter_window();
}

/***********************************************************************
*                       PARAMETER_WINDOW_PROC                          *
***********************************************************************/
BOOL CALLBACK parameter_window_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int ypos;

switch ( msg )
    {
    case WM_CREATE:
        ParameterWindow = hWnd;
        init();
        return 0;

//  case WM_LBUTTONUP:
//      toggle_big_param_display_type();
//      return 0;

    case WM_RBUTTONUP:
        ypos = HIWORD( lParam );

        if ( ypos > LineHeight )
            {
            choose_corner_parameters();
            }
        else
            {
            if ( choose_font() )
                {
                position_parameter_window();
                InvalidateRect( hWnd, 0, TRUE );
                position_main_plot_window();
                }
            }
        return 0;

    case WM_SETFOCUS:
        SetFocus( MainWindow );
        return 0;

    case WM_PAINT:
        paint_me(hWnd);
        return 0;

    case WM_NEW_DATA:
        return 0;

    case WM_DESTROY:
        shutdown();
        break;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}