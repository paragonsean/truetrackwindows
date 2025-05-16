#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\rectclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

extern HINSTANCE MainInstance;

const TCHAR NullChar          = TEXT( '\0' );


static TCHAR   TempMessageClassName[] = TEXT( "TempMessageClass");
static HWND    TempMessageWindow      = 0;
static TCHAR * TempMessage            = 0;
static int     TempMessageLen         = 0;
static int     TempMessageMargin      = 4;
static UINT    TempMessageTimeoutMs   = 0;
static BOOLEAN CenterTempMessage      = FALSE;

/***********************************************************************
*                          PAINT_TEMP_MESSAGE                          *
***********************************************************************/
static void paint_temp_message()
{
static COLORREF mycolor = RGB( 255, 0, 0 );
PAINTSTRUCT ps;
HDC dc;
BOOL need_update;

need_update = GetUpdateRect( TempMessageWindow, NULL, FALSE );

dc = BeginPaint( TempMessageWindow, &ps );
if ( need_update )
    {
    SetBkMode( dc, TRANSPARENT );
    SetTextColor( dc, mycolor );
    SetTextAlign( dc, TA_TOP | TA_LEFT );
    if ( TempMessage && TempMessageLen )
        TextOut( dc, TempMessageMargin, 0, TempMessage, TempMessageLen );
    }

EndPaint( TempMessageWindow, &ps );
}

/***********************************************************************
*                        POSITION_TEMP_MESSAGE                         *
***********************************************************************/
static void position_temp_message( HWND w )
{
WINDOW_CLASS wc;
HDC dc;
SIZE sz;
RECTANGLE_CLASS r;

/*
----------------------
Put the box mid screen
---------------------- */
if ( CenterTempMessage )
    {
    CenterTempMessage = FALSE;
    GetClientRect( GetDesktopWindow(), &r );
    }
else
    {
    wc = GetFocus();
    if ( wc.style() & WS_CHILD )
        wc = GetParent( wc.handle() );
    wc.getrect( r );
    }

r.left = (r.left + r.right)/2;
r.top  = (r.top + r.bottom)/2;

wc = w;
dc = GetDC( w );
GetTextExtentPoint32( dc, TempMessage, TempMessageLen, &sz );

sz.cx += 8 + 2*TempMessageMargin;
sz.cy += 8;
r.setsize( sz );

sz.cx /= 2;
sz.cy /= 2;

r.offset( -sz.cx, -sz.cy );

wc.move( r );
}

/***********************************************************************
*                          TEMP_MESSAGE_PROC                           *
***********************************************************************/
LRESULT CALLBACK temp_message_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static UINT timer_id;

int    id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_DESTROY:
        if ( TempMessage )
            {
            delete[] TempMessage;
            TempMessage = 0;
            TempMessageLen = 0;
            }
        TempMessageWindow = 0;

        if ( timer_id )
            {
            KillTimer(hWnd, timer_id );
            timer_id = 0;
            }
        break;

    case WM_LBUTTONDOWN:
        DestroyWindow( hWnd );
        return 0;

    case WM_CREATE:
        /*
        ---------------------------------------------------
        This assumes TempMessage already has the text in it
        --------------------------------------------------- */
        position_temp_message( hWnd );

        if ( TempMessageTimeoutMs > 0 )
            timer_id = SetTimer (hWnd, 0, TempMessageTimeoutMs, NULL) ;
        break;

    case WM_TIMER:
        if ( timer_id )
            {
            KillTimer(hWnd, timer_id );
            timer_id = 0;
            }
        DestroyWindow( hWnd );
        return 0;

    case WM_PAINT:
        paint_temp_message();
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                        REGISTER_TEMP_MESSAGE                         *
***********************************************************************/
void register_temp_message( void )
{
static bool firstime = true;
WNDCLASS wc;

if ( !firstime )
    return;
firstime = false;

wc.lpszClassName = TempMessageClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) temp_message_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = NULL;
wc.lpszMenuName  = NULL;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );
}

/***********************************************************************
*                             TEMP_MESSAGE                             *
***********************************************************************/
void temp_message( TCHAR * message, int ms )
{
HWND w;
w = GetFocus();

if ( TempMessageWindow )
    return;

if ( !message )
    return;

if ( *message == NullChar )
    return;

register_temp_message();

if ( TempMessage )
    delete[] TempMessage;

TempMessage = maketext( message );
if ( !TempMessage )
    return;

TempMessageLen = lstrlen(TempMessage);
TempMessageTimeoutMs = ms;

TempMessageWindow = CreateWindowEx(
    WS_EX_TOOLWINDOW | WS_EX_TOPMOST,  // | WS_EX_TRANSPARENT,
    TempMessageClassName,              // window class name
    0,                                 // window title
    WS_POPUP | WS_DLGFRAME,
    100, 100,                          // X,y
    300, 50,                           // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow(   TempMessageWindow, SW_SHOW );
UpdateWindow( TempMessageWindow );

SetFocus( w );
}

/***********************************************************************
*                             TEMP_MESSAGE                             *
***********************************************************************/
void temp_message( TCHAR * message )
{
temp_message( message, 500 );
}

/***********************************************************************
*                          KILL_TEMP_MESSAGE                           *
***********************************************************************/
void kill_temp_message()
{
if ( TempMessageWindow )
    DestroyWindow( TempMessageWindow );
}

/***********************************************************************
*                        CENTER_TEMP_MESSAGE                           *
***********************************************************************/
void center_temp_message()
{
CenterTempMessage = TRUE;
}

