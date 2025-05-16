#include <windows.h>
#include <ddeml.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\evclass.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\stringcl.h"
#include "..\include\ringcl.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\ft.h"
#include "..\include\wclass.h"
#include "..\include\v5help.h"

#define MAX_MACHINES_TO_RESET        10

#include "boards.h"

#define _MAIN_
#include "extern.h"
#include "resource.h"
#include "shotsave.h"
#include "..\include\events.h"

static const int32  MAX_PENDING_FASTRAK_COMMANDS = 20;

TCHAR MyClassName[] = MONITOR_CLASS_NAME;

TCHAR NewSetupMachine[MACHINE_NAME_LEN+1];

static const TCHAR CommaChar = TEXT( ',' );
static const TCHAR NullChar  = TEXT( '\0' );
static const TCHAR TabChar   = TEXT('\t');
static const TCHAR YChar     = TEXT( 'Y' );

static const TCHAR ConfigSection[]   = TEXT( "Config" );
static const TCHAR MonallIniFile[]   = TEXT( "monall.ini" );
static const TCHAR VisiTrakIniFile[] = TEXT( "visitrak.ini" );
static const TCHAR LastNKey[]        = TEXT( "LastN" );
static const TCHAR SubtractTimeoutKey[] = TEXT( "SubtractTimeout" );

BOOL CALLBACK ManualControlProc(   HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK HardwareStatusProc(  HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK ioex4_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void gray_ioex4_controls( void );

void    hardware_shutdown( void );
void    sensor_list_shutdown( void );
BOOLEAN sensor_list_startup( void );
void    start_shotsave_thread( void );

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                        RESOURCE_MESSAGE_BOX                          *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("v5help.hlp") );
    WinHelp( MainWindow, fname, helptype, context );
    }
}

/***********************************************************************
*                             GRAY_CONTROLS                            *
***********************************************************************/
static void gray_controls( void )
{
UINT  menu_flags;
HMENU menu_handle;


menu_handle = GetMenu( MainWindow );

if ( menu_handle )
    {
    menu_flags = MF_ENABLED;
    if ( CurrentPasswordLevel < Ioex4MenuGrayLevel )
        menu_flags = MF_GRAYED;
    EnableMenuItem( menu_handle, IOEX4_MENU_CHOICE, menu_flags );

    menu_flags = MF_ENABLED;
    if ( CurrentPasswordLevel < CurrentFtValuesGrayLevel )
        menu_flags = MF_GRAYED;
    EnableMenuItem( menu_handle, SHOW_CURRENT_FT_VALUES_MENU_CHOICE, menu_flags );
    }

gray_ioex4_controls();
}

/***********************************************************************
*                          PASSWORD_LEVEL_CALLBACK                     *
***********************************************************************/
void password_level_callback( TCHAR * topic, short item, HDDEDATA edata )
{
TCHAR buf[MAX_INTEGER_LEN+3];
DWORD bytes_copied;
DWORD bufsize = sizeof( buf );

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_copied = DdeGetData( edata, (LPBYTE) buf, bufsize, 0 );
if ( bytes_copied > 0 )
    {
    *(buf+MAX_INTEGER_LEN) = NullChar;
    fix_dde_name( buf );
    CurrentPasswordLevel = asctoint32( buf );
    }

gray_controls();
}

/***********************************************************************
*                      NEW_MONITOR_SETUP_CALLBACK                      *
***********************************************************************/
void new_monitor_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
const DWORD BYTES_TO_COPY = sizeof(TCHAR)*(MACHINE_NAME_LEN + 1);
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
    SendMessage( MainWindow, WM_NEWSETUP, 0, 0L );
    }

}

/***********************************************************************
*                         NEW_DOWNTIME_CALLBACK                        *
***********************************************************************/
void new_downtime_callback( TCHAR * topic, short item, HDDEDATA edata )
{
DOWN_DATA_CLASS dd;
BOARD_CLASS     b;

if ( dd.set(edata) )
    b.refresh_downtime_state( dd );

}

/***********************************************************************
*                        POSITION_AUTO_SHOT_DISPLAY                    *
***********************************************************************/
void position_auto_shot_display( void )
{
RECT r;
INT  w;
INT  h;

GetWindowRect( AutoShotDisplayWindow, &r );

w = r.right - r.left;
h = r.bottom - r.top;

GetClientRect( GetDesktopWindow(), &r );

if ( r.top == 0 )
    r.top = -1;

r.left = r.right - w;
r.bottom = r.top + h;

SetWindowPos( AutoShotDisplayWindow, HWND_TOPMOST,
              r.left, r.top, r.right-r.left, r.bottom-r.top,
              SWP_HIDEWINDOW );
}

/***********************************************************************
*                          AUTOSHOTDISPLAYPROC                         *
***********************************************************************/
BOOL CALLBACK AutoShotDisplayProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static const COLORREF GreenColor     = RGB( 0, 255, 0 );
static HBRUSH InputOnBackgroundBrush = 0;
HWND ws;
int  id;

switch (msg)
    {
    case WM_WINDOWPOSCHANGING:
        return TRUE;

    case WM_INITDIALOG:
        InputOnBackgroundBrush = CreateSolidBrush ( GreenColor );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DESTROY:
        if ( InputOnBackgroundBrush )
            {
            DeleteObject( InputOnBackgroundBrush );
            InputOnBackgroundBrush = 0;
            }
        break;

    case WM_CTLCOLORSTATIC:
        ws = (HWND) lParam;
        id = GetDlgCtrlID( ws );
        if ( id == AUTO_SHOT_MACHINE_NUMBER_TEXTBOX || id == AUTO_SHOT_LABEL_TBOX )
            {
            if ( IsWindowEnabled(ws) )
                {
                SetBkColor ((HDC) wParam, GreenColor );
                return (int) InputOnBackgroundBrush;
                }
            }
        break;

    case WM_DBINIT:
        position_auto_shot_display();
        SetFocus( MainWindow );
        return TRUE;
    }

return FALSE;
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
*                    SEND_DATA_SERVER_SHOT_NOTIFICATION                *
*                                                                      *
*                           M01,VISITRAK,NS,1395                       *
***********************************************************************/
static void send_data_server_shot_notification( TCHAR * mach_part, TCHAR * shot )
{
static TCHAR s[] = TEXT( ",NS," );
TCHAR buf[MACHINE_NAME_LEN+1+PART_NAME_LEN+4+SHOT_LEN+1];

lstrcpy( buf, mach_part );
lstrcat( buf, s );
lstrcat( buf, shot );

poke_data( DDE_MONITOR_TOPIC, ItemList[DS_NOTIFY_INDEX].name, buf );
}

/***********************************************************************
*                    SEND_DATA_SERVER_DOWN_NOTIFICATION                *
*                                                                      *
*                            M01,VISITRAK,DT                           *
***********************************************************************/
static void send_data_server_down_notification( TCHAR * sorc )
{
static TCHAR s[] = TEXT( ",DT" );
TCHAR * cp;

cp = findchar( TabChar, sorc );
if ( cp )
    {
    *cp = CommaChar;
    cp++;
    cp = findchar( TabChar, cp );
    }

if ( cp )
    {
    lstrcpy( cp, s );
    poke_data( DDE_MONITOR_TOPIC, ItemList[DS_NOTIFY_INDEX].name, sorc );
    }

}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

int id;
TCHAR * buf;
TCHAR * cp;
TCHAR   shot[SHOT_LEN+1];

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        MainMenu = LoadMenu( MainInstance, TEXT("MAINMENU") );
        SetMenu( hWnd, MainMenu );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        register_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX, new_monitor_setup_callback );
        register_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX,     new_downtime_callback );
        register_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX,   password_level_callback );
        register_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX,   plot_min_callback          );
        get_current_password_level();
        return 0;

    case WM_NEWSETUP:
        MachinesToReset.push( NewSetupMachine );
        return 0;

    case WM_EV_SHUTDOWN:
        PostMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;


    case WM_SETFOCUS:
        if ( Ioex4IsActive )
            {
            Ioex4Window.set_focus();
            return 0;
            }
        break;

    case WM_NEWSHOT:
        if ( lParam )
            {
            /*
            --------------------------------------------------------
            The machine name and part name are pointed to by lParam.
                                  "M01,VISITRAK"
            -------------------------------------------------------- */
            buf = (TCHAR *) lParam;

            /*
            ---------------------------------------
            The shot number is pointed to by wParam
            --------------------------------------- */
            int32toasc( shot, (int32) wParam, DECIMAL_RADIX );

            send_data_server_shot_notification( buf, shot );

            /*
            -------------------------------------
            Need machine name only for topic name
            ------------------------------------- */
            cp = findchar( CommaChar, buf );
            if ( cp )
                *cp = NullChar;

            poke_data( buf, ItemList[SHOT_NUMBER_INDEX].name, shot );

            /*
            -------------------------------------------------------------
            I have to clean up the memory used by the machine name string
            ------------------------------------------------------------- */
            delete[] buf;
            }
        return 0;

    case WM_POKEMON:
        /*
        -----------------------------------------------------------
        wParam is the index to the item name, lParam is a pointer
        to a string allocated for this purpose (which I must free).
        ----------------------------------------------------------- */
        if ( lParam )
            {
            cp = (TCHAR *) lParam;
            poke_data( DDE_MONITOR_TOPIC, ItemList[wParam].name, cp );

            if ( wParam == (WPARAM) DOWN_DATA_INDEX )
                send_data_server_down_notification( cp );

            /*
            -------------------------------------------------------------
            I have to clean up the memory used by the machine name string
            ------------------------------------------------------------- */
            delete[] cp;
            }
        return 0;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, BOARD_MONITOR_MENU_HELP );
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case MINIMIZE_MENU_CHOICE:
                CloseWindow( MainWindow );
                return 0;

            case SHOW_CURRENT_FT_VALUES_MENU_CHOICE:
            case IOEX4_MENU_CHOICE:
                if ( !Ioex4Window.is_visible() )
                    {
                    SetMenu( MainWindow, 0 );
                    if ( id == SHOW_CURRENT_FT_VALUES_MENU_CHOICE )
                        ShowCurrentFtValuesOnly = TRUE;
                    ShowWindow( HardwareStatusWindow, SW_HIDE );
                    Ioex4IsActive = TRUE;
                    Ioex4Window.show();
                    }
                return 0;

            case HELP_CONTENTS_MENU_CHOICE:
                gethelp( HELP_CONTEXT, TABLE_OF_CONTENTS_HELP );
                return 0;

            case EXIT_MENU_CHOICE:
                SendMessage( hWnd, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    case WM_CLOSE:
        if ( !ShuttingDown )
            {
            ShuttingDown = TRUE;
            if ( BoardMonitorEvent )
                SetEvent( BoardMonitorEvent );
            if ( ShotSaveEvent )
                SetEvent( ShotSaveEvent );
            }
        else if ( !BoardMonitorIsRunning && !ShotSaveIsRunning )
            {
            /*
            --------------------------------------------------
            My threads have shut down, let the os shut me down
            -------------------------------------------------- */
            break;
            }
        return 0;

    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                          GET_HISTORY_LENGTH                          *
***********************************************************************/
static void get_history_length( void )
{
TCHAR  * cp;
short slen;

cp = get_ini_string( VisiTrakIniFile, ConfigSection, LastNKey );
slen = lstrlen( cp );
if ( slen > 0 )
    HistoryShotCount = extshort( cp, slen );

}

/***********************************************************************
*                        CHECK_DISK_FREE_SPACE                         *
***********************************************************************/
static void check_disk_free_space( void )
{
TCHAR root[MAX_PATH];

if ( get_root_directory(root) )
    {
    if ( free_kilobytes(root) < MIN_FREE_DISK_KB )
        resource_message_box( MainInstance, LOW_DISK_SPACE_STRING, NO_SHOTS_SAVED_STRING, MB_OK | MB_SYSTEMMODAL );
    }
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOLEAN init( int cmd_show )
{
WNDCLASS       wc;
COMPUTER_CLASS c;
TCHAR *        cp;
int            width;
int            height;
STRING_CLASS   s;

if ( is_previous_instance(MyClassName, 0) )
    return FALSE;

/*
---------------------------------------
Create a ring buffer for reset messages
--------------------------------------- */
MachinesToReset.init( MAX_MACHINES_TO_RESET );

FasTrakCommand.init( MAX_PENDING_FASTRAK_COMMANDS );

ShuttingDown = FALSE;

if ( names_startup() )
    {
    c.startup();
    lstrcpy( ComputerName, c.whoami() );
    get_history_length();
    skip_startup();
    sensor_list_startup();
    shifts_startup();
    dcurve_startup();
    ft_startup();
    check_disk_free_space();
    units_startup();
    }

if ( *get_ini_string( VisiTrakIniFile, ConfigSection, TEXT("HaveSureTrak") ) == YChar )
    HaveSureTrakControl = TRUE;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MyClassName );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

if ( PRIMARYLANGID(GetUserDefaultLangID()) == LANG_JAPANESE )
    {
    width  = 375;
    height = 352;
    }
else
    {
    width  = 274;
    height = 334;
    }

s = resource_string( MainInstance, MONITOR_WINDOW_TITLE_STRING );

MainWindow = CreateWindow(
    MyClassName,
    s.text(),
    WS_POPUP | WS_CAPTION,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    width, height,                    // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );

if ( *get_ini_string(MonallIniFile, ConfigSection, SubtractTimeoutKey) == YChar )
    SubtractTimeoutFromAutoDown = TRUE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("FtClearCmdWait") );
if ( !unknown(cp) )
    ft_set_clear_cmd_wait( (int) asctoint32(cp) );

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EffPeakSkipCount") );
if ( !unknown(cp) )
    EpiSkipCount = (short) asctoint32( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("CurrentFtValuesGrayLevel") );
if ( !unknown(cp) )
    CurrentFtValuesGrayLevel = (short) asctoint32( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("Ioex4MenuGrayLevel") );
if ( !unknown(cp) )
    Ioex4MenuGrayLevel = (short) asctoint32( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("Ioex4GrayLevel") );
if ( !unknown(cp) )
    Ioex4GrayLevel = (short) asctoint32(cp);

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("LeftVelPlaces") );
if ( !unknown(cp) )
    LeftVelPlaces = asctoint32(cp);

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("RightVelPlaces") );
if ( !unknown(cp) )
    RightVelPlaces = asctoint32(cp);

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("LeftPosPlaces") );
if ( !unknown(cp) )
    LeftPosPlaces = asctoint32(cp);

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("RightPosPlaces") );
if ( !unknown(cp) )
    RightPosPlaces = asctoint32(cp);

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("LeftAnalogPlaces") );
if ( !unknown(cp) )
    LeftAnalogPlaces = asctoint32(cp);

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("RightAnalogPlaces") );
if ( !unknown(cp) )
    RightAnalogPlaces = asctoint32(cp);

/*
-------------------------------------------------------------
Create a mutex for controlling access to the fastrak routines
FasTrakMutex = CreateMutex(NULL, FALSE, NULL);
------------------------------------------------------------- */
FasTrakMutex = 0;

/*
--------------------------------------------
Create an event for the board monitor thread
-------------------------------------------- */
BoardMonitorEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !BoardMonitorEvent )
    resource_message_box( MainInstance,  UNABLE_TO_CONTINUE_STRING, BOARD_MONITOR_EVENT_FAIL_STRING, MB_OK | MB_SYSTEMMODAL );

/*
---------------------------------------
Create an event for the shotsave thread
--------------------------------------- */
ShotSaveEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !ShotSaveEvent )
    resource_message_box( MainInstance,  UNABLE_TO_CONTINUE_STRING, SHOTSAVE_EVENT_FAIL_STRING, MB_OK | MB_SYSTEMMODAL );

start_shotsave_thread();

HardwareStatusWindow  = CreateDialog(
    MainInstance,
   TEXT("HARDWARE_STATUS"),
    MainWindow,
    (DLGPROC) HardwareStatusProc );

Ioex4Window = CreateDialog(
    MainInstance,
   TEXT("IOEX4_DIALOG"),
    MainWindow,
    (DLGPROC) ioex4_dialog_proc );

AutoShotDisplayWindow  = CreateDialog(
    MainInstance,
   TEXT("AUTO_SHOT"),
    MainWindow,
    (DLGPROC) AutoShotDisplayProc );

Ioex4Window.hide();
ShowWindow( AutoShotDisplayWindow, SW_HIDE );

SetFocus( MainWindow );

return TRUE;
}

/***********************************************************************
*                            VOID SHUTDOWN                             *
***********************************************************************/
static void shutdown( void )
{
unregister_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX  );
unregister_for_event( DDE_CONFIG_TOPIC,  PW_LEVEL_INDEX );
unregister_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX     );
unregister_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX );
client_dde_shutdown();

if ( FasTrakMutex )
    {
    CloseHandle( FasTrakMutex );
    FasTrakMutex = 0;
    }

if ( BoardMonitorEvent )
    {
    CloseHandle( BoardMonitorEvent );
    BoardMonitorEvent = 0;
    }

if ( ShotSaveEvent )
    {
    CloseHandle( ShotSaveEvent );
    ShotSaveEvent = 0;
    }

dcurve_shutdown();
skip_shutdown();
ft_shutdown();
sensor_list_shutdown();
shifts_shutdown();
units_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG     msg;
BOOL    status;

InitCommonControls();

MainInstance = this_instance;

if ( !init(cmd_show) )
    return 0;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status && HardwareStatusWindow )
        status = IsDialogMessage( HardwareStatusWindow, &msg );

    if ( !status && Ioex4Window.handle() )
        status = IsDialogMessage( Ioex4Window.handle(), &msg );

    if ( !status && AutoShotDisplayWindow )
        status = IsDialogMessage( AutoShotDisplayWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
