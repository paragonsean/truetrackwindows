#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\listbox.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"
#include "resrc1.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS BrouseWindow;

static TCHAR MyClassName[] = "Test";
static TCHAR BackSlash[]   = "\\";

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                           CREATE_HEADER_CONTROL                      *
***********************************************************************/
void create_header_control( HWND parent )
{
TCHAR t[] = TEXT( "Part Name" );
TCHAR t1[] = TEXT( "Machine Name" );
const int height = 24;
HWND  hc;
RECT  r;
POINT p;
HDITEM hdi;
int    i;
/*
GetWindowRect( GetDlgItem(parent, TEST_LISTBOX), &r );
p.x = r.left;
p.y = r.top - height +2;

ScreenToClient( parent, &p );

hc = CreateWindowEx(
    0,
    WC_HEADER,
    0,
    WS_CHILD | WS_VISIBLE | WS_BORDER | HDS_BUTTONS | HDS_HORZ,
    p.x, p.y,                     // X, y
    r.right-r.left, height,       // W, h
    parent,
    (HMENU) BROUSE_LB_HEADER,
    MainInstance,
    0 );
*/

hc = GetDlgItem( parent, BROUSE_LB_HEADER );
GetWindowRect( hc, &r );

hdi.mask    = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
hdi.pszText = t;
hdi.cxy     = (r.right - r.left )/2;
hdi.cchTextMax = lstrlen(t);
hdi.fmt = HDF_LEFT | HDF_STRING;

i = 0;
i = SendMessage(hc, HDM_INSERTITEM, (WPARAM) i, (LPARAM) &hdi);

hdi.pszText = t1;
hdi.cchTextMax = lstrlen(t1);
i = SendMessage(hc, HDM_INSERTITEM, (WPARAM) i, (LPARAM) &hdi);
}

/***********************************************************************
*                             FILL_LISTBOX                             *
***********************************************************************/
static void fill_listbox( HWND parent )
{
LISTBOX_CLASS lb;
lb.init( parent, TEST_LISTBOX );
lb.add( "Hi you guys" );
lb.add( "What's up" );
}

/***********************************************************************
*                           BROUSE_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK brouse_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        BrouseWindow = hWnd;
        create_header_control( hWnd );
        MainWindow.shrinkwrap( hWnd );
        fill_listbox( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                ShellExecute( hWnd, "open", "c:\\program files\\windows nt\\accessories\\wordpad.exe", "d:\\apps\\worddata\\setup.wri", 0, SW_SHOWNORMAL );
                return TRUE;

            case IDCANCEL:
                MainWindow.close();
                return TRUE;
            }
        break;

    }

return FALSE;
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

    case WM_COMMAND:
        switch (id)
            {
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

//load_rich_edit_library();

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

CreateDialog(
    MainInstance,
    TEXT( "BROUSE_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) brouse_dialog_proc );

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

    if ( BrouseWindow.handle() )
        status = IsDialogMessage( BrouseWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
