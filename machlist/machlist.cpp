#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MainDialog;
LISTBOX_CLASS MachLb;

static TCHAR MyClassName[] = "MachlineList";
static TCHAR TabChar = TEXT( '\t' );

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                       MAKE_A_LIST_OF_MACHINES                        *
***********************************************************************/
static void make_a_list_of_machines()
{
COMPUTER_CLASS          c;
MACHINE_NAME_LIST_CLASS m;
STRING_CLASS            s;

c.rewind();
while ( c.next() )
    {
    if ( c.is_present() )
        m.add_computer( c.name() );
    }

s.upsize( MAX_PATH );
m.rewind();
while ( m.next() )
    {
    c.find( m.computer_name() );
    s = m.name();
    s += TabChar;
    s += c.name();
    s += TabChar;
    s += c.directory();
    MachLb.add( s.text() );
    }
}

/***********************************************************************
*                        POSITION_MAIN_LISTBOX                         *
***********************************************************************/
static void position_main_listbox()
{
RECTANGLE_CLASS r;
RECTANGLE_CLASS rc;
WINDOW_CLASS w;

w = MachLb.handle();

MainDialog.get_client_rect( r );
w.get_move_rect( rc );
rc.left = r.left + 4;
rc.right = r.right - 4;
rc.bottom = r.bottom - 4;
w.move(rc);
}

/***********************************************************************
*                         POSITION_MAIN_DIALOG                         *
***********************************************************************/
static void position_main_dialog()
{
RECTANGLE_CLASS r;
if ( MainWindow.handle() && MainDialog.handle() )
    {
    MainWindow.get_client_rect( r );
    MainDialog.move(r);
    position_main_listbox();
    }
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MainDialog = hWnd;
        MachLb.init( hWnd, MACHINE_LB );
        MachLb.set_tabs( COMPUTER_NAME_STATIC, COMPUTER_DIR_STATIC );
        MainDialog.post( WM_DBINIT );
        return TRUE;

    case WM_DBINIT:
        make_a_list_of_machines();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
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

    case WM_SIZE:
        position_main_dialog();
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
COMPUTER_CLASS c;
STRING_CLASS   title;
WNDCLASS       wc;

names_startup();
c.startup();

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

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 410,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT( "MAIN_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) main_dialog_proc );

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

    if ( MainDialog.handle() )
        status = IsDialogMessage( MainDialog.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
