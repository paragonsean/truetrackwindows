#include <windows.h>
#include <commctrl.h>
#include <winnetwk.h>   /* link to mpr.lib */
#include <shlobj.h>
#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\textlist.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"
#include "resrc1.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS ParameterListWindow;

static TCHAR MyClassName[]            = "Test";
static TCHAR ParameterListClassName[] = "ParameterList";

const TCHAR BackSlashChar     = TEXT( '\\' );
const TCHAR BackSlashString[] = TEXT( "\\" );
const TCHAR ColonChar         = TEXT( ':' );
const TCHAR TabChar           = TEXT( '\t' );
const TCHAR SpaceChar          = TEXT( ' ' );
const TCHAR NullChar          = TEXT( '\0' );
const TCHAR UnknownString[]   = UNKNOWN;
const TCHAR EmptyString[]     = TEXT("");

static FONT_LIST_CLASS FontList;
static HFONT FontHandle = INVALID_FONT_HANDLE;

COLORREF ParameterListBackgroundColor = RGB( 213, 213, 210 );
const int NofParameterListTitles = 5;
STRING_CLASS ParameterListTitle[NofParameterListTitles];

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
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
int             i;
int             id;
int             dx;
int             x;
int             additional_pixels;
HFONT           oldfont;

int  title_width[NofParameterListTitles] = { 26, 8, 8, 8, 8 };
int  title_position[NofParameterListTitles];

BeginPaint( w, &ps );
dc = ps.hdc;
GetClientRect( w, &r );

if ( FontHandle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( dc, FontHandle );
else
    oldfont = INVALID_FONT_HANDLE;

rc = r;
rc.bottom = rc.top + character_height(dc) + 3;

fill_rectangle( dc, rc, ParameterListBackgroundColor );

for ( i=0; i<NofParameterListTitles; i++ )
    title_width[i]= pixel_width( dc, ParameterListTitle[i].text() );

x = 0;
for ( i=1; i<NofParameterListTitles-1; i++ )
    maxint( x, title_width[i] );

for ( i=1; i<NofParameterListTitles-1; i++ )
    title_width[i] = x;

cw = average_character_width( dc );

for ( i=0; i<NofParameterListTitles; i++ )
    title_width[i] += cw;

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
for ( i=1; i<NofParameterListTitles; i++ )
    x += title_width[i];

if ( x > (r.width() - title_width[0]) )
    title_width[0] = r.width() - x;

x += title_width[0];

additional_pixels = r.width() - x;
additional_pixels /= NofParameterListTitles;

/*
----------------------------------------------------------------
If there isn't enough room this way, just make them all the same
---------------------------------------------------------------- */
if ( title_width[0] < title_width[1] )
    {
    i = NofParameterListTitles - 1;
    x = r.width() - cw - title_width[i];
    x /= i;
    for ( i=0; i<NofParameterListTitles-1; i++ )
        title_width[i] = x;
    }

x = cw;
for ( i=0; i<NofParameterListTitles; i++ )
    {
    dx = title_width[i] + additional_pixels;
    title_position[i] = x + dx/2;
    x += dx;
    }

SetTextAlign( dc, TA_TOP | TA_CENTER );
SetTextColor( dc, RGB(0, 0, 0) );
SetBkMode( dc, TRANSPARENT );
id = PARAMETER_TITLE_STRING;
for ( i=0; i<NofParameterListTitles; i++ )
    {
    TextOut( dc, title_position[i], 0, ParameterListTitle[i].text(), ParameterListTitle[i].len() );
    id++;
    }

s = EmptyString;
s += r.width();
TextOut( dc, r.width()/2, r.height()/2, s.text(), s.len() );

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( dc, oldfont );

EndPaint( w, &ps );
}

/***********************************************************************
*                             READ_TITLES                              *
***********************************************************************/
static void read_titles()
{
int i;
int id;

id = PARAMETER_TITLE_STRING;
for ( i=0; i<NofParameterListTitles; i++ )
    {
    ParameterListTitle[i] = resource_string( id );
    id++;
    }
}

/***********************************************************************
*                         PARAMETER_LIST_PROC                          *
***********************************************************************/
long CALLBACK parameter_list_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch ( msg )
    {
    case WM_CREATE:
        read_titles();
        break;

    case WM_PAINT:
        paint_me(hWnd);
        return 0;

    case WM_NEW_DATA:
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                             CHOOSE_FONT                              *
***********************************************************************/
static void choose_font( HWND w )
{
FONT_CLASS lf;
INI_CLASS  ini;
STRING_CLASS s;

if ( FontHandle != INVALID_FONT_HANDLE )
    FontList.get_font( lf, FontHandle );

if ( lf.choose(w) )
    {
    s = lf.put();
    ini.put_string( ".\\free\\test.ini", "Config", "ParameterWindowFont", s.text() );
    if ( FontHandle != INVALID_FONT_HANDLE )
        FontList.free_handle( FontHandle );
    FontHandle = FontList.get_handle( lf );
    }
}

/***********************************************************************
*                     CALLBACK BROWSECALLBACKPROC                      *
***********************************************************************/
int CALLBACK browse_callback_proc( HWND w, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
ITEMIDLIST * p;
TCHAR buf[MAX_PATH];

if ( uMsg == BFFM_SELCHANGED )
    {
    p = (ITEMIDLIST *) lParam;
    if ( p != NULL )
        {
        if ( SHGetPathFromIDList(p, buf) )
            MessageBox( w, buf, "new select", MB_OK );
        }
    }
return 0;
}

/***********************************************************************
*                             CHOOSE_DIRS                              *
***********************************************************************/
void choose_dirs( HWND w )
{
TCHAR buf[MAX_PATH];
BROWSEINFO bi;
ITEMIDLIST * pidlBrowse;
LPMALLOC     ip;
bool         status;

bi.hwndOwner      = w;
bi.pidlRoot       = NULL;
bi.pszDisplayName = buf;
bi.lpszTitle      = "Click on the V5 or V5DS (Data Archiver) folder";
bi.ulFlags        = 0;
bi.lpfn           = browse_callback_proc;
bi.lParam         = 0;
bi.iImage         = 0;

if ( SUCCEEDED(SHGetMalloc(&ip)) )
    {
    status = false;
    pidlBrowse = SHBrowseForFolder( &bi );
    if ( pidlBrowse != NULL )
        {
        if ( SHGetPathFromIDList(pidlBrowse, buf) )
            status = true;
        ip->Free(pidlBrowse);
        }

    ip->Release();
    if ( status )
        MessageBox( w, buf, "pidl_folder", MB_OK );
    else
        MessageBox( w, buf, "folder", MB_OK );
    }
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        return 0;

    case WM_NEW_DATA:
        MessageBox( hWnd, "Hi you guys", "New Data", MB_OK );
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case CHOOSE_DIRS_CHOICE:
                choose_dirs(hWnd);
                return 0;

            case ADV_MACHINE_SETUP_CHOICE:
                temp_message( TEXT("Hi you guys...") );
                return 0;

            case CREATE_CHOICE:
                choose_font(hWnd);
                return 0;

            case DELETE_CHOICE:
                kill_temp_message();
                return 0;

            case EXIT_CHOICE:
                MainWindow.close();
                return 0;
            }

        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
STRING_CLASS title;
WNDCLASS wc;
RECTANGLE_CLASS r;
FONT_CLASS      lf;
INI_CLASS       ini;

if ( CoInitialize(0) != S_OK )
    MessageBox( 0, "Opps", "CoInitialize Failed", MB_OK );

ini.set_file( ".\\free\\test.ini" );
ini.set_section( "Config" );

if ( ini.find( "ParameterWindowFont" ) )
    {
    title = ini.get_string();
    lf.get( title.text() );
    FontHandle = FontList.get_handle( lf );
    }

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

wc.lpszClassName = ParameterListClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) parameter_list_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainWindow.get_client_rect( r );

ParameterListWindow = CreateWindow(
    ParameterListClassName,
    0,
    WS_CHILD | WS_THICKFRAME,
    0, 0,                             // X,y
    r.width(), r.height(),            // W,h
    MainWindow.handle(),
    NULL,
    MainInstance,
    NULL
    );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );

ParameterListWindow.show( cmd_show );
UpdateWindow( ParameterListWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
if ( FontHandle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( FontHandle );
    FontHandle = INVALID_FONT_HANDLE;
    }

shifts_shutdown();
CoUninitialize();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
