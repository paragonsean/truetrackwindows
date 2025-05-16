#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "msres.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS CopyLogWindow;
HWND         LogFileEbox;
HWND         LogFormatEbox;
HWND         LogRecordTbox;
LISTBOX_CLASS StatusListbox;

static STRING_CLASS LocalIniFile;
static STRING_CLASS RootDir;
static STRING_CLASS PasswordFile;
static STRING_CLASS PasswordFormat;;
static STRING_CLASS MonitorFile;
static STRING_CLASS MonitorFormat;

TCHAR MyClassName[] = TEXT("CopyLog");

const static TCHAR AsterixChar   = TEXT( '*' );
const static TCHAR BackSlashChar = TEXT( '\\' );
const static TCHAR CommaChar     = TEXT( ',' );
const static TCHAR NullChar      = TEXT( '\0' );
const static TCHAR SpaceChar     = TEXT( ' ' );
const static TCHAR TabChar       = TEXT( '\t' );

static TCHAR Cchar         = TEXT( 'c' );
static TCHAR Dchar         = TEXT( 'd' );
static TCHAR Ochar         = TEXT( 'o' );
static TCHAR Pchar         = TEXT( 'p' );
static TCHAR Mchar         = TEXT( 'm' );
static TCHAR Nchar         = TEXT( 'n' );
static TCHAR Tchar         = TEXT( 't' );
static TCHAR EmptyString[] = TEXT( "" );

static TCHAR ConfigSection[]      = TEXT( "Config" );
static TCHAR ThisComputerKey[]    = TEXT( "ThisComputer" );
static TCHAR RootDirKey[]         = TEXT( "RootDir" );

static TCHAR NetDirPrefixString[] = TEXT( "\\\\" );
static TCHAR NoComputerName[]     = NO_COMPUTER;
static TCHAR VisiTrakIniFile[]    = TEXT("\\visitrak.ini");
static TCHAR DataDirString[]      = TEXT("data\\");
static TCHAR ExesDirString[]      = TEXT("exes");
static TCHAR UnknownString[]      = UNKNOWN;

static TCHAR MonitorLogFileKey[]    = TEXT("MonitorLogFile");
static TCHAR MonitorLogFormatKey[]  = TEXT("MonitorLogFormat");
static TCHAR PasswordLogFileKey[]   = TEXT("PasswordLogFile");
static TCHAR PasswordLogFormatKey[] = TEXT("PasswordLogFormat");

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes()
{
TCHAR   buf[MAX_PATH+1];
TCHAR * cp;
COMPUTER_CLASS c;
STRING_CLASS file;
STRING_CLASS format;
TCHAR * filekey;
TCHAR * formatkey;
BOOLEAN is_password;
UINT    id;

is_password = is_checked( CopyLogWindow.control(PASSWORD_LOG_PB) );

if ( is_password )
    id = SAVING_LOCAL_PASSWORD_SETTINGS_STRING;
else
    id = SAVING_LOCAL_MONITOR_SETTINGS_STRING;

StatusListbox.add( resource_string(id) );
StatusListbox.add( LocalIniFile.text() );

file.get_text( LogFileEbox );
format.get_text( LogFormatEbox );

if ( is_password )
    {
    PasswordFile = file;
    PasswordFormat = format;
    filekey = PasswordLogFileKey;
    formatkey = PasswordLogFormatKey;
    }
else
    {
    MonitorFile = file;
    MonitorFormat = format;
    filekey = MonitorLogFileKey;
    formatkey = MonitorLogFormatKey;
    }

if ( file.isempty() )
    file = UnknownString;

if ( format.isempty() )
    format = UnknownString;

WritePrivateProfileString( ConfigSection, filekey,   file.text(),   LocalIniFile.text() );
WritePrivateProfileString( ConfigSection, formatkey, format.text(), LocalIniFile.text() );

if ( is_checked(CopyLogWindow.handle(), SAVE_TO_NETWORK_XBOX) )
    {
    if ( is_password )
        id = SAVING_NETWORK_PASSWORD_SETTINGS_STRING;
    else
        id = SAVING_NETWORK_MONITOR_SETTINGS_STRING;

    StatusListbox.add( resource_string(id) );
    c.rewind();
    while ( c.next() )
        {
        if ( !c.is_this_computer() )
            {
            copystring( buf, c.directory() );
            cp = findstring( DataDirString, buf );
            if ( cp )
                {
                *cp = NullChar;
                lstrcat( buf, ExesDirString );
                lstrcat( buf, VisiTrakIniFile );
                StatusListbox.add( buf );
                WritePrivateProfileString( ConfigSection, filekey,   file.text(),   buf );
                WritePrivateProfileString( ConfigSection, formatkey, format.text(), buf );
                }
            }
        }
    }

StatusListbox.add( resource_string(DONE_STRING) );
}

/***********************************************************************
*                           GET_CONNECT_LIST                           *
***********************************************************************/
static void get_connect_list()
{
TCHAR buf[MAX_PATH+1];

GetPrivateProfileString( ConfigSection, RootDirKey, UnknownString, buf, MAX_PATH+1, LocalIniFile.text() );
if ( !unknown(buf) )
    RootDir = buf;

GetPrivateProfileString( ConfigSection, PasswordLogFileKey, UnknownString, buf, MAX_PATH+1, LocalIniFile.text() );
if ( unknown(buf) )
    buf[0] = NullChar;
PasswordFile = buf;

GetPrivateProfileString( ConfigSection, PasswordLogFormatKey, UnknownString, buf, MAX_PATH+1, LocalIniFile.text() );
if ( unknown(buf) )
    buf[0] = NullChar;
PasswordFormat = buf;

GetPrivateProfileString( ConfigSection, MonitorLogFileKey, UnknownString, buf, MAX_PATH+1, LocalIniFile.text() );
if ( unknown(buf) )
    buf[0] = NullChar;
MonitorFile = buf;

GetPrivateProfileString( ConfigSection, MonitorLogFormatKey, UnknownString, buf, MAX_PATH+1, LocalIniFile.text() );
if ( unknown(buf) )
    buf[0] = NullChar;
MonitorFormat = buf;
}

/***********************************************************************
*                         UPDATE_SAMPLE_RECORD                         *
***********************************************************************/
static void update_sample_record()
{
static TCHAR m[] = TEXT( "M01" );
static TCHAR old_part[] = TEXT( "OLD_PART" );
static TCHAR new_part[] = TEXT( "NEW_PART" );
static TCHAR op[]       = TEXT( "John Doe" );

STRING_CLASS format;
STRING_CLASS record;
format.get_text( LogFormatEbox );
log_file_string( record, format.text(), op, m, old_part, new_part );
record.set_text( LogRecordTbox );
}

/***********************************************************************
*                        COPYLOG_DIALOG_PROC                           *
***********************************************************************/
BOOL CALLBACK copylog_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static WINDOW_CLASS wc;
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        StatusListbox.init( w, STATUS_LISTBOX );
        CopyLogWindow = w;
        LogFileEbox   = GetDlgItem( w, LOG_FILE_NAME_EBOX );
        LogFormatEbox = GetDlgItem( w, LOG_FORMAT_EBOX );
        LogRecordTbox = GetDlgItem( w, LOG_RECORD_TBOX );
        MainWindow.shrinkwrap( w );
        CopyLogWindow.post( WM_DBINIT );
        return TRUE;

    case WM_DBINIT:
        get_connect_list();
        wc = GetDlgItem( w, PASSWORD_LOG_PB );
        wc.post( BM_CLICK );
        return 0;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_CHANGES_BUTTON:
                save_changes();
                return TRUE;

            case LOG_FORMAT_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    update_sample_record();
                    return TRUE;
                    }
                break;

            case PASSWORD_LOG_PB:
                if ( cmd == BN_CLICKED )
                    {
                    wc = GetDlgItem( w, RECORD_HELP_STATIC );
                    wc.hide();
                    set_text( w, LOG_FILE_TYPE_HELP_STATIC, resource_string(PASSWORD_LOG_STRING) );
                    set_text( w, FORMAT_EXAMPLE_TBOX,       resource_string(PASSWORD_FORMAT_EXAMPLE_STRING) );
                    set_text( w, RECORD_EXAMPLE_TBOX,       resource_string(PASSWORD_RECORD_EXAMPLE_STRING) );
                    PasswordFile.set_text( w, LOG_FILE_NAME_EBOX );
                    PasswordFormat.set_text( LogFormatEbox );
                    return TRUE;
                    }
                break;

            case MONITOR_LOG_PB:
                if ( cmd == BN_CLICKED )
                    {
                    wc = GetDlgItem( w, RECORD_HELP_STATIC );
                    wc.show();
                    set_text( w, LOG_FILE_TYPE_HELP_STATIC, resource_string(MONITOR_LOG_STRING) );
                    set_text( w, FORMAT_EXAMPLE_TBOX,       resource_string(MONITOR_FORMAT_EXAMPLE_STRING) );
                    set_text( w, RECORD_EXAMPLE_TBOX,       resource_string(MONITOR_RECORD_EXAMPLE_STRING) );
                    MonitorFile.set_text( w, LOG_FILE_NAME_EBOX );
                    MonitorFormat.set_text( LogFormatEbox );
                    return TRUE;
                    }
                break;

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

switch (msg)
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        return 0;

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
STRING_CLASS title;
WNDCLASS wc;

if ( names_startup() )
    c.startup();

LocalIniFile = exe_directory();
LocalIniFile += VisiTrakIniFile;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("NETSETUP_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title   = resource_string( MAIN_WINDOW_TITLE_STRING );

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
    TEXT("COPYLOG_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) copylog_dialog_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( CopyLogWindow.handle() )
        status = IsDialogMessage( CopyLogWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
