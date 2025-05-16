#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\textlist.h"
#include "..\include\warmupcl.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MainDialog;
LISTBOX_CLASS MachLb;

static TCHAR MyClassName[] = "MachlineList";
static TCHAR TabChar = TEXT( '\t' );
static TCHAR Computer[] = "C01";

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                          CREATE_IF_MISSING                           *
***********************************************************************/
static void create_if_missing( TCHAR * ma, TCHAR * pa, TCHAR * file_name )
{
NAME_CLASS s;
FILE_CLASS   f;

s.get_part_results_dir_file_name( Computer, ma, pa, file_name );
if ( !s.file_exists() )
    {
    f.open_for_write(s);
    f.close();
    }
}

/***********************************************************************
*                               FIXPART                                *
***********************************************************************/
void fixpart( TCHAR * ma, TCHAR * pa )
{
static short analog_sensor[MAX_FT2_CHANNELS] = { 0, 0, 24, 24, 24, 24, 25, 25, 24, 24, 24, 24, 24, 24, 24, 24 };
int          i;
DB_TABLE     t;
FILE_CLASS   f;
NAME_CLASS   s;
//unsigned     wire;
//WARMUP_CLASS w;

s.get_ftchan_file_name( Computer, ma, pa );
if ( !s.file_exists() )
    {
    t.create( s.text() );
    t.open( s.text(), FTCHAN_RECLEN, FL );
    t.empty();
    for ( i=0; i<MAX_FT2_CHANNELS; i++ )
        {
        t.put_short( FTCHAN_CHANNEL_NUMBER_OFFSET, i+1,              CHANNEL_NUMBER_LEN );
        t.put_short( FTCHAN_SENSOR_NUMBER_OFFSET,  analog_sensor[i], SENSOR_NUMBER_LEN );
        t.rec_append();
        }
    t.close();
    }

s.get_part_results_dir_file_name( Computer, ma, pa, 0 );
if ( !s.directory_exists() )
    s.create_directory();

create_if_missing( ma, pa, ALARMSUM_DB );
create_if_missing( ma, pa, GRAPHLST_DB );
create_if_missing( ma, pa, SHOTPARM_DB );

/*---------------------------------------------------------
s.get_ft2_limit_switch_wires_file_name( Computer, ma, pa );
if ( f.open_for_write(s) )
    {
    f.writeline( TEXT("29") );
    f.writeline( TEXT("30") );
    f.writeline( TEXT("31") );
    f.writeline( TEXT("32") );
    f.writeline( TEXT("0") );
    f.writeline( TEXT("0") );
    f.close();
    }

if ( w.get(Computer, ma, pa) )
    {
    s = w.wire;
    wire = s.uint_value();
    if ( wire > 0 && wire < 17 )
        {
        wire = mask_from_wire( wire );
        wire <<=16;
        wire = wire_from_mask( wire );
        s = wire;
        copystring( w.wire, s.text() );
        w.put( Computer, ma, pa );
        }
    }
-----------------------------------------------------------*/
}

/***********************************************************************
*                         MAKE_A_LIST_OF_PARTS                         *
***********************************************************************/
void make_a_list_of_parts( TEXT_LIST_CLASS & list, TCHAR * machine )
{
NAME_CLASS    s;
DB_TABLE      t;
TCHAR         name[PART_NAME_LEN+1];

list.empty();

s = parts_dbname( Computer, machine );
if ( !s.file_exists() )
    return;

t.open( s.text(), PARTS_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( name, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
    list.append( name );
    }

t.close();
}

/***********************************************************************
*                       MAKE_A_LIST_OF_MACHINES                        *
***********************************************************************/
static void make_a_list_of_machines()
{
COMPUTER_CLASS          c;
MACHINE_NAME_LIST_CLASS m;
STRING_CLASS            s;
TEXT_LIST_CLASS         t;

copystring( Computer, c.whoami() );
m.add_computer( Computer );

m.rewind();
while ( m.next() )
    {
    make_a_list_of_parts( t, m.name() );
    t.rewind();
    while ( t.next() )
        {
        s = m.name();
        s += TabChar;
        s += t.text();
        MachLb.add( s.text() );
        fixpart( m.name(), t.text() );
        }
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
        MachLb.set_tabs( PART_NAME_STATIC, PART_NAME_STATIC );
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
