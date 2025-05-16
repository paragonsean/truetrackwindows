#include <windows.h>
#include <ddeml.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\iniclass.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\part.h"
#include "..\include\subs.h"
#include "..\include\v5help.h"

#define MINIMIZE_MENU_CHOICE        103
#define EXIT_CHOICE                 108
#define MONITOR_CONFIG_CHOICE       502

#define _MAIN_
#include "extern.h"
#include "..\include\events.h"
#include "resource.h"

TCHAR   MyClassName[]   = TEXT( "MonEdit" );
TCHAR * MyWindowTitle   = 0;
TCHAR   NullChar        = TEXT( '\0' );

BOOL CALLBACK MonitorProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

void monitor_proc_cleanup( void );

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("v5help.hlp") );
    WinHelp( MainWindow, fname, helptype, context );
    }
}

/***********************************************************************
*                      NEW_MACHINE_SETUP_CALLBACK                      *
***********************************************************************/
void new_machine_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
const DWORD BYTES_TO_COPY = sizeof(TCHAR) * (MACHINE_NAME_LEN+1);
DWORD   bytes_copied;

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_copied = DdeGetData( edata, (LPBYTE) NewSetupMachine, BYTES_TO_COPY, 0 );
if ( bytes_copied > 0 )
    {
    *(NewSetupMachine+MACHINE_NAME_LEN) = NullChar;
    fix_dde_name( NewSetupMachine );
    SendMessage( MonitorWindow, WM_NEWSETUP, 0, 0L );
    }
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
void paint_me()
{
PAINTSTRUCT ps;

BeginPaint( MainWindow, &ps );
EndPaint( MainWindow, &ps );
}

/***********************************************************************
*                              PLOT_MIN_CALLBACK                       *
***********************************************************************/
void plot_min_callback( TCHAR * topic, short item, HDDEDATA edata )
{
if ( !IsIconic(MainWindow) )
    CloseWindow( MainWindow );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_DBINIT :
        client_dde_startup( hWnd );
        register_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX, new_machine_setup_callback );
        register_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX,   plot_min_callback          );
        return 0;

    case WM_EV_SHUTDOWN:
        SendMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, MONITOR_CONFIG_1_MACH_HELP );
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case MINIMIZE_MENU_CHOICE:
                CloseWindow( MainWindow );
                return 0;

            case MONITOR_CONFIG_CHOICE:
                return 0;

            case EXIT_CHOICE:
                SendMessage( hWnd, WM_CLOSE, 0, 0L );
                return 0;
            }

    case WM_PAINT:
        paint_me();
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static void init( int cmd_show )
{
WNDCLASS wc;
COMPUTER_CLASS c;

ShuttingDown = FALSE;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MAKEINTRESOURCE(IDI_MONEDIT) );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle,
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    515, 179,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );

if ( names_startup() )
    {
    c.startup();
    lstrcpy( ComputerName, c.whoami() );
    }

MonitorWindow  = CreateDialog(
    MainInstance,
    TEXT("MONITOR_LIST"),
    MainWindow,
    (DLGPROC) MonitorProc );

}

/***********************************************************************
*                            VOID SHUTDOWN                             *
***********************************************************************/
static void shutdown( void )
{
if ( MyWindowTitle )
    {
    delete [] MyWindowTitle;
    MyWindowTitle = 0;
    }

monitor_proc_cleanup();
unregister_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX );
unregister_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX );
client_dde_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{

MSG   msg;
BOOL  status;

MainInstance  = this_instance;
MyWindowTitle = maketext( resource_string( MainInstance, MONEDIT_TITLE_STRING) );

if ( is_previous_instance(MyClassName, MyWindowTitle) )
    return 0;

init( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MonitorWindow )
        status = IsDialogMessage( MonitorWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }


shutdown();
return(msg.wParam);
}
