#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\strarray.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "vtwcontrol.h"
#include "windowlist.h"

#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;

static TCHAR NullChar                = TEXT( '\0' );
static TCHAR CommaChar               = TEXT( ',' );
static TCHAR EmptyString[]           = TEXT( "" );
static TCHAR MyClassName[]           = TEXT( "JeditRes" );

TCHAR FileName[MAX_PATH_NAME_LEN+1];

WINDOW_CLASS CurrentWindow;

int          CurrentTab = 0;

/*
------------
Shared lists
------------ */
FONT_LIST_CLASS   FontList;
WINDOW_LIST_CLASS ControlList;
WINDOW_LIST_CLASS MsControlList;
WINDOW_LIST_CLASS MoveList;

/***********************************************************************
*                      LPARAM_TO_POINT                                 *
***********************************************************************/
void lparam_to_point( POINT & p, LPARAM lParam )
{
short i;

i = LOWORD( lParam );
p.x = (long) i;

i = HIWORD( lParam );
p.y = (long) i;
}

/***********************************************************************
*                            TEXT_IS_MODIFIED                          *
***********************************************************************/
static bool text_is_modified( HWND w )
{
STRING_CLASS s;
MS_CONTROL_ENTRY * mce;

s.get_text(w);

mce = (MS_CONTROL_ENTRY *) GetWindowLong( w, GWL_USERDATA );
if ( mce )
    {
    if ( s == mce->s )
        return false;
    }
return true;
}

/***********************************************************************
*                            SAVE_CONTROLS                             *
***********************************************************************/
static bool save_controls()
{

FILE_CLASS destfile;
CONTROL_ENTRY * cep;
MS_CONTROL_ENTRY * mce;
WINDOW_CLASS w;

if ( lstrlen(FileName) < 1 )
    return false;

if ( destfile.open_for_write( FileName )  )
    {
    destfile.writeline( int32toasc(ControlList.count()) );
    if ( ControlList.count() )
        {
        ControlList.rewind();
        while ( ControlList.next() )
            {
            cep = (CONTROL_ENTRY *) GetWindowLong( ControlList.handle(), GWL_USERDATA );
            if ( cep )
                cep->write( destfile );
            }
        }
    destfile.writeline( int32toasc(MsControlList.count()) );
    if ( MsControlList.count() )
        {
        MsControlList.rewind();
        while ( MsControlList.next() )
            {
            mce = (MS_CONTROL_ENTRY *) GetWindowLong( MsControlList.handle(), GWL_USERDATA );
            if ( mce )
                mce->write( destfile );
            }
        }

    destfile.close();
    return true;
    }

return false;
}

/***********************************************************************
*                              LEFT_ALIGN                              *
***********************************************************************/
static void left_align( int id )
{
WINDOW_CLASS w;
RECTANGLE_CLASS r;
int x;
long width;
long height;

if ( !CurrentWindow.handle() )
    return;

if ( !MoveList.count() )
    return;

CurrentWindow.get_move_rect( r );
x      = r.left;
width  = r.width();
height = r.height();

MoveList.rewind();
while ( MoveList.next() )
    {
    w = MoveList.handle();
    w.get_move_rect( r );
    if ( id == LEFT_ALIGN_CHOICE )
        {
        w.move( x, r.top );
        }
    else
        {
        r.setsize( width, height );
        w.move( r );
        }
    }

}


/***********************************************************************
*                            HIGHEST_WINDOW                            *
*                                                                      *
* Find the highest window (y), remove it from the list, return handle  *
***********************************************************************/
HWND highest_window( WINDOW_LIST_CLASS & list )
{
WINDOW_CLASS low;
WINDOW_CLASS w;
RECTANGLE_CLASS r;
int y;

if ( list.count() < 1 )
    return 0;

list.rewind();
low = list.next();
low.get_move_rect( r );
y = r.top;
while ( list.next() )
    {
    w = list.handle();
    w.get_move_rect( r );
    if ( r.top < y )
        {
        low = w.handle();
        y   = r.top;
        }
    }

list.remove( low.handle() );
return low.handle();
}

/***********************************************************************
*                            STACK_WINDOWS                             *
***********************************************************************/
static void stack_windows()
{
WINDOW_CLASS low;
WINDOW_LIST_CLASS mylist;
WINDOW_CLASS w;
RECTANGLE_CLASS r;
int y;

if ( !MoveList.count() )
    return;

mylist.append( MoveList );

low = highest_window( mylist );
while ( true )
    {
    w = highest_window( mylist );
    if ( !w.handle()  )
        break;
    low.get_move_rect( r );
    y = r.bottom + 1;
    w.get_move_rect( r );
    w.move( r.left, y );
    low = w;
    }

}

/***********************************************************************
*                           NEXT_CONTROL_ID                            *
***********************************************************************/
static int next_control_id( int cid )
{
CONTROL_ENTRY * cep;
MS_CONTROL_ENTRY * mce;
bool found;

/*
---------------------
Don't do a next on -1
--------------------- */
if ( cid > 0 )
    {
    while ( true )
        {
        cid++;
        found = false;
        if ( ControlList.count() )
            {
            ControlList.rewind();
            while ( !found && ControlList.next() )
                {
                cep = (CONTROL_ENTRY *) GetWindowLong( ControlList.handle(), GWL_USERDATA );
                if ( cep )
                    found = ( cid == cep->id );
                }
            }

        if ( MsControlList.count() )
            {
            MsControlList.rewind();
            while ( !found && MsControlList.next() )
                {
                mce = (MS_CONTROL_ENTRY *) GetWindowLong( MsControlList.handle(), GWL_USERDATA );
                if ( mce )
                    found = ( cid == mce->id );
                }
            }

        if ( !found )
            break;
        }
    }

return cid;
}

/***********************************************************************
*                           ADD_ONE_VTW_CONTROL                        *
***********************************************************************/
static void add_one_vtw_control( void )
{
CONTROL_ENTRY * cep;
CONTROL_ENTRY * copy;
HWND w;

copy = (CONTROL_ENTRY *) GetWindowLong( CurrentWindow.handle(), GWL_USERDATA );
if ( copy )
    {
    cep = new CONTROL_ENTRY;
    if ( cep )
        {
        *cep = *copy;
        cep->r.offset( 0, cep->r.height()+1 );
        cep->id = next_control_id( cep->id );
        w = cep->create( MainWindow, MainInstance );
        if ( w )
            {
            ControlList.append( w );
            CurrentWindow = w;
            ShowWindow( w, SW_SHOWNA );
            }
        else
            {
            delete cep;
            }
        }
    }
}

/***********************************************************************
*                            ADD_ONE_MS_CONTROL                        *
***********************************************************************/
static void add_one_ms_control( void )
{
MS_CONTROL_ENTRY * mse;
MS_CONTROL_ENTRY * copy;
HWND w;

copy = (MS_CONTROL_ENTRY *) GetWindowLong( CurrentWindow.handle(), GWL_USERDATA );
if ( copy )
    {
    mse = new MS_CONTROL_ENTRY;
    if ( mse )
        {
        *mse = *copy;
        mse->r.offset( 0, mse->r.height()+1 );
        mse->id = next_control_id( mse->id );
        w = mse->create( MainWindow, MainInstance );
        if ( w )
            {
            MsControlList.append( w );
            CurrentWindow = w;
            ShowWindow( w, SW_SHOWNORMAL );
            }
        else
            {
            delete mse;
            }
        }
    }
}

/***********************************************************************
*                           ADD_ONE_CONTROL                            *
***********************************************************************/
static void add_one_control( void )
{

if ( ControlList.contains(CurrentWindow.handle()) )
    add_one_vtw_control();
else
    add_one_ms_control();

}

/***********************************************************************
*                          REMOVE_ONE_CONTROL                          *
***********************************************************************/
static void remove_one_control()
{
if ( CurrentWindow.handle() )
    {
    ControlList.remove( CurrentWindow.handle() );
    MsControlList.remove( CurrentWindow.handle() );
    CurrentWindow.destroy();
    MainWindow.refresh();
    }
}

/***********************************************************************
*                          LOAD_VTW_CONTROLS                           *
* This assumes you are loading a list for the first time, and hence    *
* sets the array size.                                                 *
***********************************************************************/
bool load_vtw_controls( WINDOW_LIST_CLASS & clist, FILE_CLASS & sorcfile )
{
CONTROL_ENTRY * cep;
HWND w;
int n;

/*
----------------------------------------------------------------------
The first line of the file is the number of controls contained therein
---------------------------------------------------------------------- */
n = (int) asctoint32( sorcfile.readline() );

if ( n > 0 )
    {
    if ( clist.setsize(n) )
        {
        while ( n )
            {
            cep = new CONTROL_ENTRY;
            if ( !cep )
                break;
            if ( !cep->read(sorcfile) )
                {
                delete cep;
                break;
                }
            w = cep->create( MainWindow, MainInstance );
            if ( w )
                clist.append( w );
            n--;
            }
        }
    }

if ( clist.count() > 0 )
    return true;

return false;
}

/***********************************************************************
*                          LOAD_MS_CONTROLS                           *
***********************************************************************/
bool load_ms_controls( WINDOW_LIST_CLASS & clist, FILE_CLASS & sorcfile )
{
MS_CONTROL_ENTRY * mce;
HWND w;
int n;

/*
----------------------------------------------------------------------
The first line of the file is the number of controls contained therein
---------------------------------------------------------------------- */
n = (int) asctoint32( sorcfile.readline() );

if ( n > 0 )
    {
    if ( clist.setsize(n) )
        {
        while ( n )
            {
            mce = new MS_CONTROL_ENTRY;
            if ( !mce )
                break;
            if ( !mce->read(sorcfile) )
                {
                delete mce;
                break;
                }
            w = mce->create( MainWindow, MainInstance );
            if ( w )
                clist.append( w );
            n--;
            }
        }
    }

if ( clist.count() > 0 )
    return true;

return false;
}

/***********************************************************************
*                         CHOOSE_FILE_TO_LOAD                          *
***********************************************************************/
static bool choose_file_to_load( void )
{
static TCHAR Filter[] = TEXT( "Control Files\0*.TXT;\0" );

OPENFILENAME fh;

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow.handle();
fh.hInstance         = 0;
fh.lpstrFilter       = Filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = FileName;
fh.nMaxFile          = MAX_PATH_NAME_LEN;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = 0;                     // TEXT( "d:\\sources\\projects\\visitrak\\free");
fh.lpstrTitle        = TEXT( "Choose file to load" );
fh.Flags             = OFN_EXPLORER;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*FileName = NullChar;

if ( GetOpenFileName(&fh) )
    return true;

return false;
}

/***********************************************************************
*                            LOAD_CONTROLS                             *
***********************************************************************/
static void load_controls()
{
FILE_CLASS f;
CONTROL_ENTRY * cep;
HWND w;

if ( choose_file_to_load() )
    {
    ControlList.destroy();
    MsControlList.destroy();

    if ( f.open_for_read(FileName) )
        {
        load_vtw_controls( ControlList, f );
        ControlList.show();

        load_ms_controls( MsControlList, f );
        MsControlList.show();

        f.close();

        ControlList.rewind();
        while ( (w=ControlList.next()) )
            {
            cep = (CONTROL_ENTRY *) GetWindowLong( w, GWL_USERDATA );
            if ( cep )
                {
                if ( cep->type & BACKGROUND_TYPE )
                    SetWindowPos( w, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE );
                }
            }

        if ( MsControlList.count() > 0 )
            SetFocus( MsControlList[0] );
        }
    }

}

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                             PAINT_TITLE                              *
***********************************************************************/
static void paint_title()
{
static TCHAR myfont[] = TEXT( "-19,0,0,0,400,0,0,0,0,3,2,1,18,Times New Roman" );
static HFONT font_handle = INVALID_FONT_HANDLE;
const COLORREF font_color = RGB( 228, 64, 64 );
STRING_CLASS s;

FONT_CLASS lf;
RECTANGLE_CLASS r;
RECTANGLE_CLASS cr;
int x;
int y;

if ( font_handle == INVALID_FONT_HANDLE )
    {
    lf.get( myfont );
    font_handle = FontList.get_handle( lf );
    }

HDC dc = GetWindowDC( MainWindow.handle() );

GetWindowRect( MainWindow.handle(), &r );

x = (r.right - r.left) / 2;
y = 0;

SetBkMode( dc, TRANSPARENT );
SetTextColor( dc, font_color );
SetTextAlign( dc, TA_TOP | TA_CENTER );

HFONT oldfont = INVALID_FONT_HANDLE;
if ( font_handle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( dc, font_handle );

if ( *FileName != NullChar )
    TextOut( dc, x, y, FileName, lstrlen(FileName) );

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( dc, oldfont );

x = r.right - r.left;
GetClientRect( MainWindow.handle(), &cr );

TCHAR   * int32toasc( int32 n );
s = TEXT( "Client: " );
s += int32toasc( (int32) cr.width() );
s += TEXT( " x " );
s += int32toasc( (int32) cr.height() );

SetTextAlign( dc, TA_TOP | TA_RIGHT );
SetTextColor( dc, (COLORREF) 0 );
TextOut( dc, r.width()-3, y, s.text(), s.len() );

y = r.bottom - cr.height() - 5;

MoveToEx( dc, 0,         y, 0 );
LineTo(   dc, r.width(), y    );
}

/***********************************************************************
*                               DRAW_XOR                               *
***********************************************************************/
static void draw_xor( HWND w, RECTANGLE_CLASS & r )
{
HDC dc;
int orimode;

dc = GetDC( w );

if ( w )
    {
    orimode = SetROP2( dc, R2_NOT );
    r.draw( dc );
    SetROP2( dc, orimode );
    }

ReleaseDC( w, dc );
}

/***********************************************************************
*                            MAKE_MOVE_LIST                            *
*  Make a list of all windows that intersect the rectangle.            *
***********************************************************************/
static void make_move_list( RECTANGLE_CLASS & r )
{
RECTANGLE_CLASS cr;
WINDOW_CLASS w;
MoveList.remove_all();

ControlList.rewind();
while ( ControlList.next() )
    {
    w = ControlList.handle();
    w.get_move_rect( cr );
    if ( cr.intersects(r) )
        MoveList.append( w.handle() );
    }

MsControlList.rewind();
while ( MsControlList.next() )
    {
    w = MsControlList.handle();
    w.get_move_rect( cr );
    if ( cr.intersects(r) )
        MoveList.append( w.handle() );
    }

}

/***********************************************************************
*                              NEW_CORNER                              *
***********************************************************************/
static void new_corner( RECTANGLE_CLASS & r, POINT & p , POINT & lp )
{
if ( p.x != lp.x )
    {
    if ( p.x < r.left )
        r.left = p.x;
    else if ( p.x > r.right )
        r.right = p.x;
    else if ( lp.x == r.left )
        r.left = p.x;
    else
        r.right = p.x;

    }

if ( p.y != lp.y )
    {
    if ( p.y < r.top )
        r.top = p.y;
    else if ( p.y > r.bottom )
        r.bottom = p.y;
    else if ( lp.y == r.top )
        r.top = p.y;
    else
        r.bottom = p.y;
    }
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
LRESULT CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static const COLORREF RedColor     = RGB( 255, 0, 0 );
static HBRUSH EboxBackgroundBrush = 0;

static HWND w;
static int  dest_tbox_id;
static RECTANGLE_CLASS r;
/*
-----------------
Drawing Variables
----------------- */
static bool drawing = false;
static RECTANGLE_CLASS dr;
static POINT dp;

int id;
int cmd;
id  = LOWORD( wParam );
cmd = HIWORD( wParam );
static WINDOW_CLASS hadfocus;

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        EboxBackgroundBrush = CreateSolidBrush ( (COLORREF) GetSysColor(COLOR_WINDOW) );
        load_controls();
        break;

    case WM_ACTIVATE:
        if ( id == WA_INACTIVE )
            {
            hadfocus = GetFocus();
            return 0;
            }
        else
            {
            if ( !(BOOL) cmd )
                {
                /*
                -------------------------------------------
                I am being activated and I am not minimized
                ------------------------------------------- */
                if ( hadfocus.handle() )
                    {
                    hadfocus.set_focus();
                    /*
                    --------------------------------------------
                    Return 0 so windows does not set focus to me
                    -------------------------------------------- */
                    return 0;
                    }
                }
            }
        break;

    case WM_LBUTTONDOWN:
        lparam_to_point( dp, lParam );
        SetCapture( hWnd );
        dr.set( dp.x, dp.y, dp.x, dp.y );
        drawing = true;
        return 0;

    case WM_MOUSEMOVE:
        if ( drawing )
            {
            POINT p;
            lparam_to_point( p, lParam );
            if ( p.x != dp.x || p.y != dp.y )
                {
                if ( !dr.is_empty() )
                    draw_xor( hWnd, dr );
                new_corner( dr, p, dp );
                dp.x = p.x;
                dp.y = p.y;
                if ( !dr.is_empty() )
                    draw_xor( hWnd, dr );
                }
            }
        return 0;

    case WM_LBUTTONUP:
        if ( drawing )
            {
            if ( !dr.is_empty() )
                draw_xor( hWnd, dr );
            ReleaseCapture();
            drawing = false;
            make_move_list( dr );
            }
        return 0;

    case WM_CTLCOLOREDIT:
        w = (HWND) lParam;
        if ( text_is_modified(w) )
            {
            SetTextColor ((HDC) wParam, RedColor );
            SetBkColor ((HDC) wParam, (COLORREF) GetSysColor(COLOR_WINDOW) );
            return (int) EboxBackgroundBrush;
            }
        break;

    case WM_KEYDOWN:
        if ( CurrentWindow.handle() && (wParam == VK_UP || wParam == VK_DOWN || wParam == VK_LEFT || wParam == VK_RIGHT) )
            {
            CurrentWindow.get_move_rect(r);
            POINT p;
            p.x = 0;
            p.y = 0;
            switch ( wParam )
                {
                case VK_UP:
                    if ( r.top > 0 )
                        p.y = -1;
                    break;

                case VK_DOWN:
                    p.y = 1;
                    break;

                case VK_LEFT:
                    if ( r.left > 0 )
                        p.x = -1;
                    break;

                case VK_RIGHT:
                    p.x = 1;
                    break;

                }
            if ( (p.x != 0) || (p.y != 0) )
                {
                if ( MoveList.count() > 0 )
                    MoveList.offset( p );
                else
                    CurrentWindow.offset( p );
                }
            }
        else if ( wParam == VK_INSERT )
            {
            add_one_control();
            }
        else if ( wParam == VK_DELETE )
            {
            remove_one_control();
            }
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case EXIT_CHOICE:
                MainWindow.close();
                return 0;

            case REFRESH_CHOICE:
                MainWindow.refresh();
                return 0;

            case SAVE_CONTROLS_CHOICE:
                save_controls();
                return 0;

            case OPEN_CONTROLS_CHOICE:
                load_controls();
                return 0;

            case DELETE_CHOICE:
                remove_one_control();
                return 0;

            case CREATE_CHOICE:
                add_one_control();
                return 0;

            case LEFT_ALIGN_CHOICE:
            case SAME_SIZE_CHOICE:
                left_align( id );
                return 0;

            case STACK_WINDOWS_CHOICE:
                stack_windows();
                return 0;
            }

        break;

    case WM_NCPAINT:
        DefWindowProc( hWnd, msg, wParam, lParam );
        paint_title();
        return 0;

    case WM_DESTROY:
        if ( EboxBackgroundBrush )
            {
            DeleteObject( EboxBackgroundBrush );
            EboxBackgroundBrush = 0;
            }

        PostQuitMessage( 0 );
        return 0;

    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{

STRING_CLASS title;
WNDCLASS wc;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);  // GetStockObject(BLACK_BRUSH) );
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );
*FileName = NullChar;

CreateWindow(
    MyClassName,
    title.text(),
    WS_POPUP | WS_VISIBLE | WS_THICKFRAME,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    800, 600,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
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
