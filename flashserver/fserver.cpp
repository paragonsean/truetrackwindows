#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\bitclass.h"
#include "..\include\fifo.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\stringcl.h"
#include "..\include\textlist.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"

HINSTANCE     MainInstance;
WINDOW_CLASS  MainWindow;
LISTBOX_CLASS MainListBox;
const INT     MainListBoxId = 1001;

u_short TcpIpPort        = 50007;
char    TcpIpAddress[16] = "127.0.0.1";
char    BoardAddress[]   = "A340";

const  TCHAR NChar          = TEXT( 'N' );
const  TCHAR YChar          = TEXT( 'Y' );
const  TCHAR WChar          = TEXT( 'W' );
const  TCHAR NullChar       = TEXT( '\0' );
static TCHAR MyClassName[]  = TEXT("JaysFlashServer");
static TCHAR EmptyString[]  = TEXT("");

static TCHAR IniFileName[]     = TEXT("flashserver.ini" );
static TCHAR BoardAddressKey[] = TEXT("BoardAddress" );
static TCHAR ConfigSection[]   = TEXT("Config" );
static TCHAR TcpIpAddressKey[] = TEXT("TcpIpAddress" );
static TCHAR TcpIpPortKey[]    = TEXT("TcpIpPort" );

extern BOOLEAN ShuttingDown;
extern HANDLE  SendEvent;
BOOLEAN sockets_startup( char * tcp_address, u_short port );
void    sockets_shutdown();
void    send( TCHAR * sorc );
INI_CLASS Ini;

const int NofMotors = 8;
static int Motors[NofMotors] = { 0, 0, 0, 0, 0, 0, 0, 0 };

unsigned short Input_Bits;

/*
------------------------------------
Linked with the menu item check mark
------------------------------------ */
static BOOLEAN EnableListing   = FALSE;
static BOOLEAN Ft_Is_Enabled   = FALSE;

void send_command_to_board( const TCHAR * cmd );
bool start_board_thread();

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                          SHOW_TCP_LOCATION                           *
***********************************************************************/
static void show_tcp_location()
{
STRING_CLASS s;

s = TEXT( "Address: " );
s += TcpIpAddress;
s += TEXT( " Port: " );
s += (unsigned ) TcpIpPort;

MainListBox.add( s.text() );
}

/***********************************************************************
*                            SET_BOARD_WIRES                           *
***********************************************************************/
void set_board_wires()
{
const BOARD_DATA bitarray[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
static char buf[10];

int        i;
BOARD_DATA bits;
char     * cp;

bits = 0;
for ( i=0; i<NofMotors; i++ )
    {
    if ( Motors[i] > 0 )
        bits |= bitarray[i];
    }

cp  = buf;
*cp = WChar;
cp++;
copystring( cp, ultohex((unsigned long) bits) );
send_command_to_board( buf );

if ( EnableListing )
    MainListBox.add( cp );
}

/***********************************************************************
*                          DO_NEW_INPUT_BITS                           *
***********************************************************************/
static void do_new_input_bits( unsigned short new_input_bits )
{
unsigned short x;
unsigned short bit;
int i;
char    buf[4];
char  * np;
char  * cp;

x   = new_input_bits ^ Input_Bits;
bit = 1;
copystring( buf, "1Y" );
np = buf;
cp = buf + 1;

for ( i=0; i<8; i++ )
    {
    if ( x & bit )
        {
        if ( new_input_bits & bit )
            *cp = YChar;
        else
            *cp = NChar;

        send( buf );
        if ( EnableListing )
            MainListBox.add( buf );
        }
    (*np)++;
    bit <<= 1;
    }

Input_Bits = new_input_bits;
}

/***********************************************************************
*                          SHUTDOWN_CALLBACK                           *
***********************************************************************/
void shutdown_callback()
{
MainListBox.add( TEXT( "Shutting down...") );
MainWindow.close();
}

/***********************************************************************
*                             UPDATE_TITLE                             *
***********************************************************************/
void update_title()
{
const TCHAR OffString[] = TEXT( " 0" );
const TCHAR OnString[]  = TEXT( " 1" );

STRING_CLASS s;
int i;

for ( i=0; i<NofMotors; i++ )
    {
    if ( Motors[i] )
        s += OnString;
    else
        s += OffString;
    }

MainWindow.set_title( s.text() );
}

/***********************************************************************
*                       SOCKET_MESSAGE_CALLBACK                        *
***********************************************************************/
void socket_message_callback( char * sorc )
{
TCHAR * cp;
int     motor_number;
int     motor_value;

if ( EnableListing )
    MainListBox.add( sorc );

cp = sorc;
if ( cp )
    {
    motor_number = asctoint32( cp );
    motor_value = 0;
    cp++;
    if ( *cp == YChar )
        motor_value = 1;
    if ( motor_number > 0 && motor_number < 9 )
        {
        motor_number--;
        Motors[motor_number] = motor_value;
        }
    update_title();
    if ( Ft_Is_Enabled )
        set_board_wires();
    }
}

/***********************************************************************
*                          INIT_TCPIP_ADDRESS                          *
***********************************************************************/
static void init_tcpip_address( void )
{
if ( Ini.find(TcpIpAddressKey) )
    lstrcpy( TcpIpAddress, Ini.get_string() );

if ( Ini.find(TcpIpPortKey) )
    TcpIpPort = (u_short) Ini.get_ul();
}

/***********************************************************************
*                          TCPIP_SETUP_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK tcpip_setup_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;
static int  cmd;

switch (msg)
    {
    case WM_INITDIALOG:
        set_text ( hWnd, TCP_IP_ADDRESS_EBOX, TcpIpAddress );
        set_text ( hWnd, TCP_IP_PORT_EBOX, ultoascii( (unsigned long) TcpIpPort) );
        return FALSE;

    case WM_COMMAND:
        id  = LOWORD( wParam );
        cmd = HIWORD( wParam );
        switch (id )
            {
            case TCPIP_CONNECT_BUTTON:
                if ( cmd == BN_CLICKED )
                    {
                    STRING_CLASS s;
                    s.get_text( hWnd, TCP_IP_ADDRESS_EBOX );
                    if ( s.len() > 6 && s.len() < 16 )
                        {
                        lstrcpy( TcpIpAddress, s.text() );
                        Ini.put_string( TcpIpAddressKey, s.text() );
                        sockets_startup( TcpIpAddress, TcpIpPort );
                        MainListBox.add( TEXT( "SocketServer started") );
                        show_tcp_location();
                        EndDialog( hWnd, 0 );
                        }
                    else
                        {
                        MessageBox( hWnd, s.text(), TEXT( "Address length between 7..15" ), MB_OK );
                        }
                    s.get_text( hWnd, TCP_IP_PORT_EBOX );
                    Ini.put_string( TcpIpPortKey, s.text() );
                    TcpIpPort = (u_short) s.uint_value();
                    }
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, 0 );
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
        MainListBox.init( CreateWindow(TEXT ("listbox"),
                          NULL,
                          WS_CHILD | WS_THICKFRAME | WS_VSCROLL | WS_VISIBLE | LBS_NOSEL,
                          0, 0, 450, 390,                         // x, y, w, h
                          hWnd,
                          (HMENU) MainListBoxId,
                          MainInstance,
                          NULL)
                        );

        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        init_tcpip_address();
        sockets_startup( TcpIpAddress, TcpIpPort );
        MainListBox.add( "SocketServer started" );
        show_tcp_location();
        start_board_thread();
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case ENABLE_LISTING_CHOICE:
                toggle_menu_view_state( EnableListing, hWnd, ENABLE_LISTING_CHOICE );
                return 0;

            case CLEAR_LISTBOX_CHOICE:
                MainListBox.empty();
                return 0;

            case EXIT_CHOICE:
                MainListBox.add( "Shutting Down..." );
                sockets_shutdown();
                send_command_to_board( "Exit" );
                return 0;


            case TEST_MESSAGE_CHOICE:
                send( "Hi you guys" );
                MainListBox.add( "Hi you guys" );
                return 0;

            case EDIT_ADDRESS_CHOICE:
                DialogBox(
                    MainInstance,
                    TEXT( "TCPIP_SETUP_DIALOG" ),
                    MainWindow.handle(),
                    (DLGPROC) tcpip_setup_dialog_proc
                     );
                return 0;

            case ENABLE_BOARD_CHOICE:
                toggle_menu_view_state( Ft_Is_Enabled, hWnd, ENABLE_BOARD_CHOICE );
                if ( Ft_Is_Enabled )
                    {
                    send_command_to_board( "EnableBoard" );
                    send_command_to_board( BoardAddress  );
                    if ( EnableListing )
                        MainListBox.add( BoardAddress );
                    send_command_to_board( "InitBoard"   );
                    }
                return 0;
            }

        break;

    case WM_NEW_DATA:
        do_new_input_bits( (unsigned short) wParam );
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
static void startup( void )
{
STRING_CLASS title;
WNDCLASS wc;

title = exe_directory();
title.cat_path( IniFileName );
Ini.set_file( title.text() );
Ini.set_section( ConfigSection );
if ( Ini.find(BoardAddressKey) )
    lstrcpy( BoardAddress, Ini.get_string() );

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = 0;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
    0, 0, 470, 400,                         // x, y, w, h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainWindow.show();
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;

InitCommonControls();

MainInstance = this_instance;

startup();

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage(  &msg );
    }

return( msg.wParam );
}
