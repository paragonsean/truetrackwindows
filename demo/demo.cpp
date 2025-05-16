#include <windows.h>
#include <ddeml.h>
#include <htmlhelp.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\evclass.h"
#include "..\include\names.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\structs.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#define _MAIN_
#include "extern.h"
#include "resource.h"

#include "..\include\events.h"

static TCHAR MyClassName[] = TEXT( "Demo" );

static const COLORREF RedColor   = RGB( 255, 0, 0 );

static const TCHAR MonallIniFile[]      = TEXT( "monall.ini" );
static const TCHAR MyIniFile[]          = TEXT( "demo.ini" );
static const TCHAR ConfigSection[]      = TEXT( "Config" );
static const TCHAR SorcDirKey[]         = TEXT( "SorcDir" );
static const TCHAR CycleTimeKey[]       = TEXT( "CycleTimeSeconds" );
static const TCHAR IsContinuousKey[]    = TEXT( "IsContinuous" );
static const TCHAR SubtractTimeoutKey[] = TEXT( "SubtractTimeout" );
static const TCHAR NullChar             = TEXT( '\0' );
static const TCHAR BackSlashChar        = TEXT( '\\' );
static const TCHAR YChar                = TEXT( 'Y' );

void start_aquiring_data_thread( void );

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                                GETHELP                               *
***********************************************************************/
void gethelp( HWND w )
{
TCHAR s[MAX_PATH+1];

get_exe_directory( s );
append_filename_to_path(  s, TEXT("Demo.chm") );

HtmlHelp( w, s, HH_DISPLAY_TOPIC, NULL );
}

/***********************************************************************
*                              PLOT_MIN_CALLBACK                       *
***********************************************************************/
void plot_min_callback( TCHAR * topic, short item, HDDEDATA edata )
{
if ( !MainWindow.is_minimized() )
    MainWindow.minimize();
}

/***********************************************************************
*                        POSITION_AUTO_SHOT_DISPLAY                    *
***********************************************************************/
void position_auto_shot_display( void )
{
RECT r;
INT  w;
INT  h;

AutoShotDisplayWindow.getrect( r );

w = r.right - r.left;
h = r.bottom - r.top;

GetClientRect( GetDesktopWindow(), &r );

if ( r.top == 0 )
    r.top = -1;

r.left = r.right - w;
r.bottom = r.top + h;

SetWindowPos( AutoShotDisplayWindow.handle(), HWND_TOPMOST,
              r.left, r.top, r.right-r.left, r.bottom-r.top,
              SWP_HIDEWINDOW );
}

/***********************************************************************
*                         AUTO_SHOT_DISPLAY_PROC                       *
***********************************************************************/
BOOL CALLBACK auto_shot_display_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
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
        AutoShotDisplayWindow = hWnd;
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
        if ( id == AUTO_SHOT_MACH_NAME_TBOX || id == AUTO_SHOT_LABEL_TBOX )
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
        BrouseWindow.set_focus();
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          SHOW_CURRENT_PART                           *
***********************************************************************/
static void show_current_part()
{
set_text( BrouseWindow.control(CURRENT_COMPUTER_TBOX), CurrentPart.computer );
set_text( BrouseWindow.control(CURRENT_MACHINE_TBOX),  CurrentPart.machine  );
set_text( BrouseWindow.control(CURRENT_PART_TBOX),     CurrentPart.part  );
}

/***********************************************************************
*                         FIND_PART_TO_MONITOR                         *
***********************************************************************/
static BOOLEAN find_part_to_monitor()
{
DB_TABLE t;
BITSETYPE flags;
BOOLEAN   status;

status = FALSE;
t.open( machset_dbname(CurrentPart.computer), MACHSET_RECLEN, PFL );
while ( t.get_next_record(FALSE) )
    {
    flags = BITSETYPE( t.get_long(MACHSET_MONITOR_FLAGS_OFFSET) );
    if ( flags & MA_MONITORING_ON )
        {
        t.get_alpha( CurrentPart.machine, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
        t.get_alpha( CurrentPart.part, MACHSET_CURRENT_PART_OFFSET, PART_NAME_LEN );
        status = TRUE;
        break;
        }
    }
t.close();

if ( status )
    {
    show_current_part();

    /*
    -------------------------------------------------------
    Tell the monitor thread that there is a new destination
    ------------------------------------------------------- */
    HaveNewPart = TRUE;
    }

return status;
}

/***********************************************************************
*                      NEW_MONITOR_SETUP_CALLBACK                      *
*                                                                      *
* This message has data but I just reload the current part from        *
* scratch and don't even look at the data.                             *
***********************************************************************/
void new_monitor_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
find_part_to_monitor();
}

/***********************************************************************
*                         NEW_DOWNTIME_CALLBACK                        *
***********************************************************************/
void new_downtime_callback( TCHAR * topic, short item, HDDEDATA edata )
{
DOWN_DATA_CLASS dd;
DSTAT_CLASS     d;

if ( dd.set(edata) )
    {
    d.set_cat( dd.category() );
    d.set_subcat( dd.subcategory() );
    CurrentDownState = d.down_state();
    }
}

/***********************************************************************
*                             LOAD_DEFAULTS                            *
***********************************************************************/
static void load_defaults()
{
TCHAR * cp;

cp = get_ini_string( MyIniFile, ConfigSection, SorcDirKey );
if ( !unknown(cp) )
    SorcDir = cp;

cp = get_ini_string( MyIniFile, ConfigSection, CycleTimeKey );
if ( !unknown(cp) )
    CycleTime = cp;

IsContinuous = boolean_from_ini( MyIniFile, ConfigSection, IsContinuousKey  );

find_part_to_monitor();
}

/***********************************************************************
*                          BROUSE_FOR_SORCDIR                          *
***********************************************************************/
static void brouse_for_sorcdir( void )
{
OPENFILENAME fh;
STRING_CLASS startdir;
STRING_CLASS title;
TCHAR        fname[MAX_PATH+1];
TCHAR      * cp;
int          slen;
static DWORD current_filter_index = 1;
TCHAR        myfilter[] = TEXT( "Shot Profiles\0*pr?\0\0\0" );

title = resource_string( BROUSE_DIR_TITLE_STRING );
startdir.get_text( BrouseWindow.control(SOURCE_DIR_EBOX) );

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = BrouseWindow.handle();
fh.hInstance         = 0;
fh.lpstrFilter       = myfilter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = current_filter_index;
fh.lpstrFile         = fname;
fh.nMaxFile          = MAX_PATH;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = startdir.text();
fh.lpstrTitle        = title.text();
fh.Flags             = OFN_EXPLORER;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*fname = NullChar;

if ( GetOpenFileName( &fh ) )
    {
    slen = lstrlen( fname );
    if ( slen )
        {
        cp = fname + slen;
        while ( cp > fname )
            {
            cp--;
            if ( *cp == BackSlashChar )
                {
                cp++;
                *cp = NullChar;
                break;
                }
            }
        set_text( BrouseWindow.control(SOURCE_DIR_EBOX), fname );
        HaveNewSorc = TRUE;
        }
    }
}

/***********************************************************************
*                        COPY_DIALOG_TO_GLOBALS                        *
***********************************************************************/
static void copy_dialog_to_globals()
{
SorcDir.get_text(   BrouseWindow.control(SOURCE_DIR_EBOX) );
CycleTime.get_text( BrouseWindow.control(CYCLE_TIME_EBOX) );
SecondsBetweenShots = CycleTime.int_value();
IsContinuous        = is_checked( BrouseWindow.control(CONTINUOUS_XBOX) );
}

/***********************************************************************
*                             SAVE_DEFAULTS                            *
***********************************************************************/
static void save_defaults()
{
copy_dialog_to_globals();
put_ini_string( MyIniFile, ConfigSection, SorcDirKey,      SorcDir.text()   );
put_ini_string( MyIniFile, ConfigSection, CycleTimeKey,    CycleTime.text() );
boolean_to_ini( MyIniFile, ConfigSection, IsContinuousKey, IsContinuous     );
}

/***********************************************************************
*                           BROUSE_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK brouse_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static COLORREF EboxBackgroundColor = 0;
static HBRUSH   EboxBackgroundBrush = 0;

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        BrouseWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        load_defaults();

        EboxBackgroundColor = (COLORREF) GetSysColor( COLOR_WINDOW );
        EboxBackgroundBrush = CreateSolidBrush( EboxBackgroundColor );

        SorcDirEbox = GetDlgItem( hWnd, SOURCE_DIR_EBOX );
        SorcDir.set_text( SorcDirEbox );
        CycleTime.set_text( hWnd, CYCLE_TIME_EBOX );
        set_checkbox( hWnd, CONTINUOUS_XBOX, IsContinuous );
        return TRUE;

    case WM_HELP:
        gethelp( hWnd );
        return TRUE;

    case WM_DESTROY:
        if ( EboxBackgroundBrush )
            {
            DeleteObject( EboxBackgroundBrush );
            EboxBackgroundBrush = 0;
            }
        break;

    case WM_CTLCOLOREDIT:
        if ( HaveNewSorc && (SorcDirEbox == (HWND) lParam) )
            {
            SetTextColor ((HDC) wParam, RedColor );
            SetBkColor ((HDC) wParam, EboxBackgroundColor );
            return (int) EboxBackgroundBrush;
            }
        break;

    case WM_NO_DATA:
        resource_message_box( MainInstance, SELECT_SORC_AGAIN_STRING, NO_PROFILES_IN_SORC_STRING );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case BROUSE_BUTTON:
                brouse_for_sorcdir();
                return TRUE;

            case SOURCE_DIR_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    HaveNewSorc = TRUE;
                    }
                return TRUE;

            case CYCLE_TIME_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    CycleTime.get_text( BrouseWindow.control(CYCLE_TIME_EBOX) );
                    SecondsBetweenShots = CycleTime.int_value();
                    }
                return TRUE;

            case MANUAL_MODE_XBOX:
                if ( cmd == BN_CLICKED )
                    {
                    ManualMode = is_checked( (HWND) lParam );
                    if ( ManualMode )
                        ManualShotWindow.show();
                    else
                        ManualShotWindow.hide();
                    }
                return TRUE;

            case IDOK:
                 save_defaults();
                 BoardMonitorCommand = START_MONITORING;
                 return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                     POSITION_MANUAL_SHOT_DIALOG                      *
***********************************************************************/
static void position_manual_shot_dialog()
{
HWND desktop_window;
int x;
int y;
RECTANGLE_CLASS dr;
RECTANGLE_CLASS r;

x = 0;
y = 0;

AutoShotDisplayWindow.getrect( r );
y = r.height();

ManualShotWindow.getrect( r );

desktop_window = GetDesktopWindow();
if ( desktop_window )
    {
    GetClientRect( desktop_window, &dr );
    x = dr.right - r.width();
    }

SetWindowPos(
    ManualShotWindow.handle(),
    HWND_TOPMOST,
    x, y,
    r.width(), r.height(),
    0
   );

}

/***********************************************************************
*                           MANUAL_SHOT_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK manual_shot_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ManualShotWindow = hWnd;
        position_manual_shot_dialog();
        return TRUE;

    case WM_ACTIVATE:
        if ( id == WA_ACTIVE )
            {
            BrouseWindow.set_focus();
            return TRUE;
            }
        break;
    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                BoardMonitorCommand = MANUAL_SHOT_REQ;
                SetEvent( BoardMonitorEvent );
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
static TCHAR * cp;
static TCHAR   shot[SHOT_LEN+1];

int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        register_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX, new_monitor_setup_callback );
        register_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX,     new_downtime_callback );
        register_for_event( DDE_ACTION_TOPIC,  PLOT_MIN_INDEX,      plot_min_callback          );
        start_aquiring_data_thread();
        return 0;

    case WM_CLOSE:
        if ( !ShuttingDown )
            {
            save_defaults();
            ShuttingDown = TRUE;
            if ( BoardMonitorEvent && BoardMonitorIsRunning )
                SetEvent( BoardMonitorEvent );
            else
                break;
            }
        else if ( !BoardMonitorIsRunning )
            {
            /*
            --------------------------------------------------
            My threads have shut down, let the os shut me down
            -------------------------------------------------- */
            break;
            }
        return 0;

    case WM_NEWSHOT:
        /*
        ---------------------------------------
        The shot number is pointed to by wParam
        --------------------------------------- */
        int32toasc( shot, (int32) lParam, DECIMAL_RADIX );
        poke_data( CurrentPart.machine, ItemList[SHOT_NUMBER_INDEX].name, shot );
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

            /*
            -------------------------------------------------------------
            I have to clean up the memory used by the machine name string
            ------------------------------------------------------------- */
            delete[] cp;
            cp = 0;
            }
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
STRING_CLASS   title;
WNDCLASS       wc;
COMPUTER_CLASS c;

if ( names_startup() )
    {
    c.startup();
    CurrentPart.set( c.whoami(), 0, 0 );
    shifts_startup();
    units_startup();
    }

if ( *get_ini_string(MonallIniFile, ConfigSection, SubtractTimeoutKey) == YChar )
    SubtractTimeoutFromAutoDown = TRUE;

/*
--------------------------------------
Create an event for the monitor thread
-------------------------------------- */
BoardMonitorEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0;                          /*   TEXT( "MAINMENU" );   */
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

CreateDialog(
    MainInstance,
    TEXT( "BROUSE_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) brouse_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("AUTO_SHOT"),
    MainWindow.handle(),
    (DLGPROC) auto_shot_display_proc );

CreateDialog(
    MainInstance,
    TEXT("MANUAL_SHOT_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) manual_shot_dialog_proc );

AutoShotDisplayWindow.hide();
ManualShotWindow.hide();

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{

unregister_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX  );
unregister_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX );
unregister_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX );
client_dde_shutdown();

if ( BoardMonitorEvent )
    {
    CloseHandle( BoardMonitorEvent );
    BoardMonitorEvent = 0;
    }

shifts_shutdown();
units_shutdown();
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

    if ( !status && ManualShotWindow.handle() )
        status = IsDialogMessage( ManualShotWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
