#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\fontclas.h"
#include "..\include\rectclas.h"
#include "..\include\subs.h"

const COLORREF RedColor   = RGB( 0xFF, 0x00, 0x00 );
const COLORREF WhiteColor = RGB( 0xFF, 0xFF, 0xFF );
const COLORREF YellowColor = RGB( 0xFF, 0xFF, 0x00 );
const LONG IsInputControl = 0x10;
const LONG OptoBitMask    = 0xF;

extern HINSTANCE MainInstance;

static FONT_LIST_CLASS FontList;
static HFONT           FontHandle = INVALID_FONT_HANDLE;
static HBRUSH          RedBrush = 0;
static HBRUSH          YellowBrush = 0;

static TCHAR IChar = TEXT( 'I' );
static TCHAR VtwwOptoClassName[] = TEXT( "VtwwOptoClass");

LRESULT CALLBACK vtww_opto_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void register_vtww_opto_contol();
void shutdown_vtww_opto_control();

/***********************************************************************
*                            CREATE_MY_FONT                            *
***********************************************************************/
static void create_my_font()
{
FONT_CLASS lf;
lf.lfHeight = -10;
lf.lfWeight = 400;
lf.lfEscapement = 1800;

lf.lfOutPrecision   = OUT_STROKE_PRECIS;
lf.lfClipPrecision  = CLIP_STROKE_PRECIS;
lf.lfQuality        = DEFAULT_QUALITY;
lf.lfPitchAndFamily = FF_DONTCARE;
copystring( lf.lfFaceName, TEXT("Arial") );
FontHandle = FontList.get_handle( lf );
}

/***********************************************************************
*                     SHUTDOWN_VTWW_OPTO_CONTROL                       *
***********************************************************************/
void shutdown_vtww_opto_control()
{
if ( FontHandle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( FontHandle );
    FontHandle = INVALID_FONT_HANDLE;
    }

if ( RedBrush )
    {
    DeleteObject( RedBrush );
    RedBrush = 0;
    }

if ( YellowBrush )
    {
    DeleteObject( YellowBrush );
    YellowBrush = 0;
    }
}

/***********************************************************************
*                     REGISTER_VTWW_OPTO_CONTROL                       *
***********************************************************************/
void register_vtww_opto_control()
{
static bool firstime = true;
WNDCLASS wc;

if ( !firstime )
    return;
firstime = false;

create_my_font();
RedBrush = CreateSolidBrush( RedColor );
YellowBrush = CreateSolidBrush( YellowColor );

wc.lpszClassName = VtwwOptoClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) vtww_opto_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = NULL;
wc.lpszMenuName  = NULL;
wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static void paint_me( HWND hWnd, LONG bits )
{
const LONG bit1 = 0x1;
const LONG bit2 = 0x2;
const LONG bit3 = 0x4;
const LONG bit4 = 0x8;

PAINTSTRUCT ps;
RECTANGLE_CLASS r;
RECTANGLE_CLASS cr;

INT     orbkmode;
UINT    ori_align;
HBRUSH  background_brush;
HBRUSH  ori_brush;
HBRUSH  b;
HPEN    ori_pen;
HFONT   oldfont;
HDC     dc;
int     x;
int     y;
int     ch;

BeginPaint( hWnd, &ps );
GetClientRect( hWnd, &r );

dc = ps.hdc;
ch = character_height( dc );

oldfont = INVALID_FONT_HANDLE;
if ( FontHandle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( dc, FontHandle );

orbkmode = GetBkMode( dc );
SetBkMode( dc, TRANSPARENT );
ori_align = SetTextAlign( dc, TA_CENTER );
ori_pen   = (HPEN) SelectObject( dc, (HPEN)GetStockObject(NULL_PEN) );

if ( bits & IsInputControl )
    {
    background_brush = YellowBrush;
    }
else
    {
    SetTextColor( dc, WhiteColor );
    background_brush = (HBRUSH) GetStockObject( BLACK_BRUSH );
    }
x = r.width()/4;
y = ch + x/4;

cr.left = x - x/2;
cr.right = cr.left + x;
cr.top = ch + x/2;
cr.bottom = x + cr.top;

TextOut( dc, x, y, TEXT("4"), 1 );
if ( bits & bit4 )
    b = RedBrush;
else
    b = background_brush;
ori_brush = (HBRUSH) SelectObject( dc, b );
Ellipse( dc, cr.left, cr.top, cr.right, cr.bottom );

TextOut( dc, x*3, y, TEXT("2"), 1 );
if ( bits & bit2 )
    b = RedBrush;
else
    b = background_brush;

SelectObject( dc, b );
Ellipse( dc, cr.left+x*2, cr.top, cr.right+x*2, cr.bottom );

y = cr.bottom + x/8 + ch;
cr.top = y + x/8;
cr.bottom = cr.top + x;

TextOut( dc, x, y, TEXT("3"), 1 );
if ( bits & bit3 )
    b = RedBrush;
else
    b = background_brush;;

SelectObject( dc, b );
Ellipse( dc, cr.left, cr.top, cr.right, cr.bottom );

TextOut( dc, x*3, y, TEXT("1"), 1 );
if ( bits & bit1 )
    b = RedBrush;
else
    b = background_brush;

SelectObject( dc, b );
Ellipse( dc, cr.left+x*2, cr.top, cr.right+x*2, cr.bottom );

SetBkMode( dc, orbkmode );
SetTextColor( dc, 0 );
SetTextAlign( dc, ori_align );

if ( ori_brush )
    SelectObject( dc, ori_brush );

SelectObject( dc, ori_pen );

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( dc, oldfont );

EndPaint( hWnd, &ps );
}

/***********************************************************************
*                           PAINT_BACKGROUND                           *
***********************************************************************/
static void paint_background( HWND hWnd, HDC dc )
{
RECTANGLE_CLASS r;

GetClientRect( hWnd, &r );
r.fill( dc, YellowBrush );
}

/***********************************************************************
*                     SEND_CLICK_MESSAGE_TO_PARENT                     *
* This sends a WM_OPTO_CLICK message to the parent telling which  4  2 *
* quadrant was clicked [1,2,3,4] and the window handle.           3  1 *
***********************************************************************/
static void send_click_message_to_parent( HWND hWnd, LPARAM lParam )
{
RECTANGLE_CLASS r;
RECTANGLE_CLASS q;
POINT p;

int quadrant;
quadrant = 0;

p.x = LOWORD( lParam );
p.y = HIWORD( lParam );

GetClientRect( hWnd, &r );
q = r;

/*
-----------------------
Upper left = quadrant 4
----------------------- */
q.right = (r.left + r.right) / 2;
q.bottom = (r.top + r.bottom)/2;
if ( q.contains(p) )
    quadrant = 4;

/*
-----------------------
Lower left = quadrant 3
----------------------- */
if ( !quadrant )
    {
    q.top    = q.bottom;
    q.bottom = r.bottom;
    if ( q.contains(p) )
        quadrant = 3;
    }

/*
------------------------
Lower right = quadrant 1
------------------------ */
if ( !quadrant )
    {
    q.left  = q.right;
    q.right = r.right;
    if ( q.contains(p) )
        quadrant = 1;
    }

if ( !quadrant )
    {
    quadrant = 2;
    }

PostMessage( GetParent(hWnd), (UINT)WM_OPTO_CLICK, (WPARAM) quadrant, (LPARAM) hWnd );
}

/***********************************************************************
*                          VTWW_OPTO_PROC                              *
***********************************************************************/
LRESULT CALLBACK vtww_opto_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static HDC dc;
static PAINTSTRUCT ps;
LONG   current_value;
LONG   new_opto_bits;
RECT   r;
TCHAR  buf[21];

if ( msg == WM_CREATE )
    {
    get_text( buf, hWnd, 20 );
    current_value = 0;
    if ( *buf == IChar )
        current_value = IsInputControl;
    SetWindowLong( hWnd, GWL_USERDATA, current_value );
    return 0;
    }

current_value = GetWindowLong( hWnd, GWL_USERDATA );
switch ( msg )
    {
    case WM_NEW_DATA:
        new_opto_bits = (LONG) wParam;
        if ( new_opto_bits != (current_value & OptoBitMask) )
            {
            current_value &= ~OptoBitMask;
            current_value |= new_opto_bits;
            SetWindowLong( hWnd, GWL_USERDATA, current_value );
            GetClientRect( hWnd, &r );
            InvalidateRect( hWnd, &r, FALSE );
            }
        return 0;

    case WM_LBUTTONUP:
        send_click_message_to_parent( hWnd, lParam );
        return 0;

    case WM_ERASEBKGND:
        if ( current_value & IsInputControl )
            {
            paint_background( hWnd, (HDC) wParam );
            return 1;  /* 1 tells windows I have erased the window */
            }
        break;

    case WM_PAINT:
        paint_me( hWnd, current_value );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}
