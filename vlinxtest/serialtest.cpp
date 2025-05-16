#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\iniclass.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MainDialog;

static TCHAR MyClassName[]         = TEXT( "SerialTest" );
static TCHAR MonallIniFile[]       = TEXT( "monall.ini" );
static TCHAR SerialtestIniFile[]   = TEXT( "serialtest.ini" );
static TCHAR ConfigSection[]       = TEXT( "Config"     );
static TCHAR EosComDelimitCharKey[]= TEXT( "EosComDelimitChar" );
static TCHAR EosComEolKey[]        = TEXT( "EosComEol"  );
static TCHAR EosComPortKey[]       = TEXT( "EosComPort" );
static TCHAR EosComSetupKey[]      = TEXT( "EosComSetup" );
static TCHAR EosComSpeedKey[]      = TEXT( "EosComSpeed" );
static TCHAR EosComStopKey[]       = TEXT( "EosComStop" );
static TCHAR EosFixedWidthKey[]    = TEXT( "EosFixedWidth" );
static TCHAR EosNeedsStxEtxKey[]   = TEXT( "EosNeedsStxEtx" );
static TCHAR TimerMsKey[]          = TEXT( "TimerMs" );

static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR CommaChar     = TEXT( ',' );
static TCHAR CrChar        = TEXT( '\r' );
static TCHAR CChar         = TEXT( 'C' );
static TCHAR DChar         = TEXT( 'D' );
static TCHAR LfChar        = TEXT( '\n' );
static TCHAR LowerNChar    = TEXT( 'n' );
static TCHAR LowerRChar    = TEXT( 'r' );
static TCHAR LowerTChar    = TEXT( 't' );
static TCHAR MChar         = TEXT( 'M' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR PChar         = TEXT( 'P' );
static TCHAR PeriodChar    = TEXT( '.' );
static TCHAR SChar         = TEXT( 'S' );
static TCHAR SpaceChar     = TEXT( ' ' );
static TCHAR TabChar       = TEXT( '\t' );
static TCHAR TChar         = TEXT( 'T' );
static TCHAR ZeroChar      = TEXT( '0' );

static TCHAR EtxString[]   = TEXT( "\003" );
static TCHAR StxString[]   = TEXT( "\002" );

STRING_CLASS EosComDelimitChar;
STRING_CLASS EosComEol;
STRING_CLASS EosComPort;
STRING_CLASS EosComSetup;
STRING_CLASS EosComSpeed;
STRING_CLASS EosFixedWidth;

BOOLEAN EosNeedsStxEtx = FALSE;

static UINT TimerId = 1;
static UINT TimerMs = 0;
static bool TimerIsActive = false;
static int  MessageCount  = 0;
static BOOLEAN     Wsa_Is_Started = FALSE;
static SOCKET      MySocket = INVALID_SOCKET;
static sockaddr_in MyAddress;

char * CBuf = 0;
int    CBufLen = 0;

/***********************************************************************
*                            SEND_UDP_MESSAGE                          *
***********************************************************************/
void send_udp_message( sockaddr_in * dest, TCHAR * message )
{
int  slen;
char * cp;

slen = lstrlen( message );
if ( slen > CBufLen )
    {
    if ( CBuf )
        {
        delete[] CBuf;
        CBuf    = 0;
        CBufLen = 0;
        }
    cp = new char[slen+1];
    if ( cp )
        {
        CBuf = cp;
        CBufLen = slen;
        }
    }

if ( CBuf )
    {
    unicode_to_char( CBuf, message );
    sendto( MySocket, CBuf, slen, 0, (sockaddr *) dest, sizeof(sockaddr_in) );
    }
}

/***********************************************************************
*                          UDP_SOCKET_STARTUP                          *
***********************************************************************/
void udp_socket_startup()
{
const u_short my_port    = 20001;
const WORD SocketVersion = MAKEWORD(2,0);

char      buf[257];
WSADATA   wsadata;
int       status;
HOSTENT * hostname;
in_addr * ap;

if ( WSAStartup(SocketVersion, &wsadata) )
    {
    MessageBox( 0, TEXT("WSAStartup"), int32toasc(WSAGetLastError()), MB_OK );
    return;
    }

Wsa_Is_Started = TRUE;
MySocket = socket( PF_INET, SOCK_DGRAM, 0 );
if ( MySocket == INVALID_SOCKET )
    {
    MessageBox( 0, TEXT("socket"), int32toasc(WSAGetLastError()), MB_OK );
    return;
    }

MyAddress.sin_family           = PF_INET;
MyAddress.sin_port             = htons( my_port );
MyAddress.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
status = bind( MySocket, (sockaddr *) &MyAddress, sizeof(MyAddress) );
if ( status == SOCKET_ERROR )
    MessageBox( 0, TEXT("bind"), int32toasc(WSAGetLastError()), MB_OK );

status = gethostname( buf, 256 );
if ( status != SOCKET_ERROR )
    {
    hostname = gethostbyname( buf );
    ap = (in_addr *)hostname->h_addr_list[0];
    if ( ap )
        MyAddress.sin_addr = *ap;
    }
}

/***********************************************************************
*                         UDP_SOCKET_SHUTDOWN                          *
***********************************************************************/
void udp_socket_shutdown()
{
if ( MySocket != INVALID_SOCKET )
    {
    closesocket( MySocket );
    MySocket = INVALID_SOCKET;
    }

if ( Wsa_Is_Started )
    {
    Wsa_Is_Started = FALSE;
    WSACleanup();
    }

if ( CBuf )
    {
    CBufLen = 0;
    delete[] CBuf;
    CBuf = 0;
    }
}

/***********************************************************************
*                         GET_EOS_UDP_ADDRESS                          *
***********************************************************************/
BOOLEAN get_eos_udp_address( sockaddr_in & dest )
{
STRING_CLASS s;
BOOLEAN      status;
char         cbuf[TCP_ADDRESS_LEN+1];

status = FALSE;

dest.sin_family           = PF_INET;

if ( countchars(PeriodChar, EosComPort.text()) == 3 )
    {
    unicode_to_char( cbuf, EosComPort.text() );
    dest.sin_addr.S_un.S_addr = inet_addr( cbuf );
    if ( !EosComSpeed.isempty() )
        {
        dest.sin_port = htons( (u_short) EosComSpeed.uint_value() );
        status = TRUE;
        }
    }

return status;
}

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                          CONVERT_CONTROL_CHARS                       *
*                This is save to use on STRING_CLASS vars.             *
***********************************************************************/
static void convert_control_chars( TCHAR * sorc )
{
TCHAR * cp;

if ( !sorc )
    return;

while ( *sorc != NullChar )
    {
    if ( *sorc == BackslashChar )
        {
        cp = sorc;
        cp++;
        if ( *cp == LowerRChar )
            {
            *sorc = CrChar;
            remove_char( cp );
            }
        else if ( *cp == LowerNChar )
            {
            *sorc = LfChar;
            remove_char( cp );
            }
        else if ( *cp == LowerTChar )
            {
            *sorc = TabChar;
            remove_char( cp );
            }
        }
    sorc++;
    }
}

/***********************************************************************
*                     SEND_MESSAGE_TO_SERIAL_PORT                      *
***********************************************************************/
static void send_message_to_serial_port()
{
STRING_CLASS s;
STRING_CLASS msg;
STRING_CLASS eol;
sockaddr_in  sa;

eol = EosComEol;
convert_control_chars( eol.text() );

msg.get_text( MainDialog.control(MESSAGE_TBOX) );
if ( msg.isempty() )
    return;

if ( EosNeedsStxEtx )
    s = StxString;

s += msg;
if ( !eol.isempty() )
    s += eol;

if ( EosNeedsStxEtx )
    s += EtxString;

if ( get_eos_udp_address( sa ) )
    send_udp_message( &sa, s.text() );

}

/***********************************************************************
*                             TOGGLE_TIMER                             *
***********************************************************************/
static void toggle_timer()
{
if ( TimerIsActive )
    {
    TimerIsActive = false;
    KillTimer( MainDialog.handle(), TimerId );
    set_text( MainDialog.control(TIMED_MESSAGES_PB), TEXT("Begin") );
    }
else if ( TimerMs > 0 )
    {
    TimerIsActive = true;
    MessageCount = 0;
    set_text( MainDialog.control(MESSAGE_COUNT_TBOX), TEXT("0") );

    SetTimer( MainDialog.handle(), TimerId, TimerMs, 0 );
    set_text( MainDialog.control(TIMED_MESSAGES_PB), TEXT("Stop") );
    }
else
    {
    MessageBox( MainDialog.handle(), TEXT("Seconds must be > 0"), TEXT("Timer Error"), MB_OK );
    }
}

/***********************************************************************
*                             MAKE_MESSAGE                             *
***********************************************************************/
static void make_message()
{
static int shot_number = 0;
TCHAR buf[100];
STRING_CLASS s;
STRING_CLASS delimiter;
TCHAR * bp;
TCHAR * cp;
int     fixed_width;
int     n;
bool    firstfield;
SYSTEMTIME st;

GetLocalTime( &st );

fixed_width = EosFixedWidth.int_value();
firstfield  = true;

delimiter = EosComDelimitChar;
convert_control_chars( delimiter.text() );
if ( delimiter.isempty() )
    delimiter = TEXT( "," );

cp = EosComSetup.text();
while ( *cp != NullChar )
    {
    if ( firstfield )
        firstfield = false;
    else
        s += delimiter;

    if ( is_numeric(*cp) )
        {
        n = asctoint32( cp );
        rounded_double_to_tchar( buf, (double) n );
        if ( fixed_width > 0 )
            rjust( buf, (short) fixed_width, SpaceChar );
        s += buf;
        }
    else if ( *cp == CChar )
        {
        /*
        --------------------
        This is a cmi robot.
                  10        20
        0123456789_123456789_
        mm/dd/yy,hh,mm,ss\015
        -------------------- */
        bp = buf;
        lstrcpy( bp, alphadate(st) );
        bp[6] = bp[8];
        bp[7] = bp[9];
        bp[8] = CommaChar;
        bp += 9;
        lstrcpy( bp, alphatime(st) );
        bp[2] = CommaChar;
        bp[5] = CommaChar;
        bp[8] = CrChar;
        bp[9] = NullChar;
        s = buf;
        }
    else if ( *cp == MChar )
        {
        s += TEXT( "M01" );
        }
    else if ( *cp == PChar )
        {
        s += TEXT( "VISITRAK" );
        }
    else if ( *cp == SChar )
        {
        shot_number++;
        n = 0;
        if ( is_numeric(*(cp+1)) )
            {
            n = asctoint32(cp+1);
            if ( n > 0 )
                {
                insalph( buf, shot_number, n, ZeroChar, DECIMAL_RADIX );
                buf[n] = NullChar;
                }
            }
        if ( n > 0 )
            s += buf;
        else
            s += shot_number;
        }
    else if ( *cp == DChar )
        {
        s += alphadate( st );
        }
    else if ( *cp == TChar )
        {
        s += alphatime( st );
        }

    while ( *cp != NullChar )
        {
        if ( *cp == CommaChar )
            {
            cp++;
            break;
            }
        cp++;
        }
    }

s.set_text( MainDialog.control(MESSAGE_TBOX) );
}

/***********************************************************************
*                            SAVE_SETTINGS                             *
***********************************************************************/
static void save_settings()
{
INI_CLASS ini;
STRING_CLASS s;

s = exe_directory();
s.cat_path( MonallIniFile );

if ( !file_exists(s.text()) )
    {
    s = exe_directory();
    s.cat_path( SerialtestIniFile );
    }

ini.set_file( s.text() );
ini.set_section( ConfigSection );

ini.put_string( EosComDelimitCharKey, EosComDelimitChar.text() );
ini.put_string( EosComEolKey,         EosComEol.text()         );
ini.put_string( EosComPortKey,        EosComPort.text()        );
ini.put_string( EosComSetupKey,       EosComSetup.text()       );
ini.put_string( EosComSpeedKey,       EosComSpeed.text()       );
ini.put_string( EosFixedWidthKey,     EosFixedWidth.text()     );

ini.put_boolean( EosNeedsStxEtxKey, EosNeedsStxEtx );

s = exe_directory();
s.cat_path( SerialtestIniFile );
ini.set_file( s.text() );
ini.put_ul( TimerMsKey, TimerMs );
}

/***********************************************************************
*                       GET_SETTINGS_FROM_SCREEN                       *
***********************************************************************/
static void get_settings_from_screen()
{
STRING_CLASS s;

EosComEol.get_text(         MainDialog.control(COM_EOL_EBOX)     );
EosComPort.get_text(        MainDialog.control(COM_PORT_EBOX)    );
EosFixedWidth.get_text(     MainDialog.control(FIXED_WIDTH_EBOX) );
EosComDelimitChar.get_text( MainDialog.control(DELIMITER_EBOX)   );
EosComSetup.get_text(       MainDialog.control(SETUP_EBOX)       );
EosComSetup.uppercase();
EosComSpeed.get_text(       MainDialog.control(SPEED_EBOX)       );

s.get_text( MainDialog.control(SECONDS_EBOX) );
TimerMs = s.uint_value() * 1000;
}

/***********************************************************************
*                           INIT_MAIN_DIALOG                           *
***********************************************************************/
void init_main_dialog()
{
INI_CLASS ini;
STRING_CLASS s;
int          i;

s = exe_directory();
s.cat_path( MonallIniFile );

if ( !file_exists(s.text()) )
    {
    s = exe_directory();
    s.cat_path( SerialtestIniFile );
    }

ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( ini.find(EosComEolKey) )
    EosComEol = ini.get_string();

if ( ini.find(EosComPortKey) )
    EosComPort = ini.get_string();

if ( ini.find(EosComSetupKey) )
    EosComSetup = ini.get_string();

if ( ini.find(EosComSpeedKey) )
    EosComSpeed = ini.get_string();

if ( ini.find(EosFixedWidthKey) )
    EosFixedWidth = ini.get_string();

if ( ini.find(EosComDelimitCharKey) )
    EosComDelimitChar = ini.get_string( EosComDelimitCharKey );

EosNeedsStxEtx   = ini.get_boolean( EosNeedsStxEtxKey );

EosComEol.set_text(         MainDialog.control(COM_EOL_EBOX)     );
EosComPort.set_text(        MainDialog.control(COM_PORT_EBOX)    );
EosFixedWidth.set_text(     MainDialog.control(FIXED_WIDTH_EBOX) );
EosComDelimitChar.set_text( MainDialog.control(DELIMITER_EBOX)   );
EosComSetup.set_text(       MainDialog.control(SETUP_EBOX)       );
EosComSpeed.set_text(       MainDialog.control(SPEED_EBOX)       );

s = exe_directory();
s.cat_path( SerialtestIniFile );
ini.set_file( s.text() );

if ( ini.find(TimerMsKey) )
    TimerMs = ini.get_ul();
else
    TimerMs = 0;
s.empty();
s += unsigned( TimerMs/1000 );
s.set_text( MainDialog.control(SECONDS_EBOX) );
}

/***********************************************************************
*                             MAIN_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static HWND countwindow;
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_DBINIT:
        init_main_dialog();
        countwindow = MainDialog.control( MESSAGE_COUNT_TBOX );
        set_text( countwindow, int32toasc( (int32) MessageCount) );
        return TRUE;

    case WM_INITDIALOG:
        MainDialog = hWnd;
        MainWindow.shrinkwrap( hWnd );
        MainDialog.post( WM_DBINIT );
        return TRUE;

    case WM_TIMER:
        MessageCount++;
        set_text( countwindow, int32toasc( (int32) MessageCount) );
        make_message();
        send_message_to_serial_port();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_SETUP_PB:
                get_settings_from_screen();
                save_settings();
                return TRUE;

            case TIMED_MESSAGES_PB:
                get_settings_from_screen();
                toggle_timer();
                return TRUE;

            case SEND_ONE_MESSAGE_PB:
                get_settings_from_screen();
                make_message();
                MessageCount++;
                set_text( countwindow, int32toasc( (int32) MessageCount) );
                send_message_to_serial_port();
                return TRUE;

            case MAKE_MESSAGE_PB:
                get_settings_from_screen();
                make_message();
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
switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        break;

    case WM_DESTROY:
        if ( TimerIsActive )
            {
            TimerIsActive = false;
            KillTimer( MainDialog.handle(), TimerId );
            }
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

udp_socket_startup();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
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
udp_socket_shutdown();
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
