#include <windows.h>
#include <htmlhelp.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\asensor.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\ftanalog.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\param.h"
#include "..\include\rectclas.h"
#include "..\include\part.h"
#include "..\include\stringcl.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#define _MAIN_
#include "extern.h"
#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MainDialog;

static LISTBOX_CLASS MainListbox;
static       TCHAR EmptyString[]        = TEXT( "" );
static const TCHAR MyClassName[]        = TEXT( "PostData" );

static TCHAR MachineMessage[]       = TEXT( "<param id=\"Name\" desc=\"Machine Name\" textValue=\"" );
static TCHAR PartMessage[]          = TEXT( "<param id=\"PartName\" desc=\"Part Name\" textValue=\"" );
static TCHAR EndParameterMessage[]  = TEXT( "\"/>" );
static TCHAR BeginMessage[]         = TEXT( "<param id=\"" );
static TCHAR UnitsMessage[]         = TEXT( "\" units=\""  );
static TCHAR EndMessage[]           = TEXT( "\" curValType=\"0\" curValue=\"0\"/>" );

static const TCHAR BackslashChar     = TEXT( '\\' );
static const TCHAR DotChar           = TEXT( '.' );
static const TCHAR GTChar            = TEXT( '>' );
static const TCHAR LTChar            = TEXT( '<' );
static const TCHAR NullChar          = TEXT( '\0' );
static const TCHAR QuoteChar         = TEXT( '\"' );
static const TCHAR SpaceChar         = TEXT( ' ' );
static const TCHAR TabChar           = TEXT( '\t' );
static const TCHAR YChar             = TEXT( 'Y' );
static const TCHAR UnderscoreChar    = TEXT( '_' );
static const TCHAR ZeroChar          = TEXT( '0' );

static const TCHAR CrLf[]            = TEXT( "\r\n" );
static       TCHAR FalseString[]     = TEXT( "False" );
static       TCHAR TrueString[]      = TEXT( "True" );

/*
-------------------
Current value types
------------------- */
static TCHAR ShotDataCVT[]          = TEXT( "0" );
static TCHAR SetupDataCVT[]         = TEXT( "1" );
static TCHAR MeasuredParameterCVT[] = TEXT( "1" ); /* This used to be a 2 */

int               NofMachines = 0;
MACHINE_CLASS   * Machine     = 0;
PART_CLASS      * Part        = 0;
PARAMETER_CLASS * Param       = 0;
FTANALOG_CLASS  * FtAnalog    = 0;

void create_shot_data_files( TCHAR * profilename, int shotnumber, int mi );

/***********************************************************************
*                            RESOURCE_STRING                           *
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
append_filename_to_path(  s, TEXT("postdata.chm") );

HtmlHelp( w, s, HH_DISPLAY_TOPIC, NULL );
}

/***********************************************************************
*                           CLEANUP_GLOBALS                            *
***********************************************************************/
static void cleanup_globals()
{
NofMachines = 0;

if ( Machine )
    {
    delete[] Machine;
    Machine = 0;
    }

if ( Part )
    {
    delete[] Part;
    Part = 0;
    }

if ( Param )
    {
    delete[] Param;
    Param = 0;
    }

if ( FtAnalog )
    {
    delete[] FtAnalog;
    FtAnalog = 0;
    }
}

/***********************************************************************
*                            LOAD_MACHINES                             *
***********************************************************************/
static void load_machines()
{
COMPUTER_CLASS c;
STRING_CLASS   s;
DB_TABLE t;
int      i;
int      n;

cleanup_globals();

s.upsize( MACHINE_NAME_LEN );

/*
-------------------------------------------------------------------
Count the local machines. Only the local ones need to be counted as
this is assumed to be running on the Data Archiver.
-------------------------------------------------------------------*/
n = 0;
if ( t.open( machset_dbname(c.whoami()), MACHSET_RECLEN, PFL) )
    {
    n += t.nof_recs();
    t.close();
    }

if ( n > 0 )
    {
    Machine  = new MACHINE_CLASS[n];
    Part     = new PART_CLASS[n];
    Param    = new PARAMETER_CLASS[n];
    FtAnalog = new FTANALOG_CLASS[n];
    }

i = 0;

if ( t.open( machset_dbname(c.whoami()), MACHSET_RECLEN, PFL) )
    {
    while ( t.get_next_record(FALSE) && i < n )
        {
        t.get_alpha( s.text(), MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
        if ( Machine[i].find(c.whoami(), s.text()) )
            i++;
        }
    t.close();
    }

if ( i > 0 )
    {
    NofMachines = i;
    for ( i=0; i<NofMachines; i++ )
        {
        s = Machine[i].computer;
        s += TabChar;
        s += Machine[i].name;
        s += TabChar;
        s += Machine[i].current_part;
        MainListbox.add( s.text() );
        if ( Part[i].find(Machine[i].computer, Machine[i].name, Machine[i].current_part) )
            {
            Param[i].find( Machine[i].computer, Machine[i].name, Machine[i].current_part );
            FtAnalog[i].load( Machine[i].computer, Machine[i].name, Machine[i].current_part );
            }
        }
    }
}

/***********************************************************************
*                               EOFSTRING                              *
***********************************************************************/
TCHAR * eofstring( TCHAR * sorc )
{
while ( *sorc != NullChar )
    sorc++;

return sorc;
}

/***********************************************************************
*                     CONVERT_PARAMETER_NAME_TO_ID                     *
***********************************************************************/
void convert_parameter_name_to_id( TCHAR * sorc )
{
const TCHAR CYCLE_TIME_NAME[] = TEXT("Cycle Time");
const TCHAR CYCLE_TIME_ID[]   = TEXT("CYCLE_TIME");
if ( strings_are_equal(sorc, CYCLE_TIME_NAME) )
    {
    copystring( sorc, CYCLE_TIME_ID );
    }

while ( *sorc != NullChar )
    {
    if ( *sorc == SpaceChar )
        remove_char( sorc );
    else
        sorc++;
    }
}

/***********************************************************************
*                             ENTRY_STRING                             *
*                         [ halEnabled="True"]                         *
***********************************************************************/
static TCHAR * entry_string( TCHAR * entryname, TCHAR * entryvalue )
{
static TCHAR buf[100];
TCHAR * cp;

cp = buf;
*cp++ = SpaceChar;
cp = copystring( cp, entryname );
cp = copystring( cp, TEXT("=\"") );
cp = copystring( cp, entryvalue );
*cp++ = QuoteChar;
*cp = NullChar;

return buf;
}

/***********************************************************************
*                             ENTRY_STRING                             *
***********************************************************************/
static TCHAR * entry_string( TCHAR * entryname, float value )
{
return entry_string( entryname, ascii_float(value) );
}

/***********************************************************************
*                             ENTRY_STRING                             *
***********************************************************************/
static TCHAR * entry_string( TCHAR * entryname, int value )
{
return entry_string( entryname, int32toasc((int32) value) );
}

/***********************************************************************
*                             ENTRY_STRING                             *
***********************************************************************/
static TCHAR * entry_string( TCHAR * entryname, BOOLEAN sorc )
{
TCHAR * cp;
cp = FalseString;
if ( sorc )
    cp = TrueString;
return entry_string( entryname, cp );
}

/***********************************************************************
*                             BOOLEAN_STRING                           *
***********************************************************************/
static TCHAR * boolean_string( BOOLEAN sorc )
{
TCHAR * cp;

if ( sorc )
    cp = TrueString;
else
    cp = FalseString;

return cp;
}

/***********************************************************************
*                      GET_SORC_MACHINE_DIRECTORY                      *
***********************************************************************/
static BOOLEAN get_sorc_machine_directory( STRING_CLASS & dest, TCHAR * machine_name )
{
int n;

n = RootSorcDirectory.len();
if ( machine_name )
    n += lstrlen( machine_name );
n++;

dest.upsize( n );
dest = RootSorcDirectory;

if ( machine_name )
    dest.cat_w_char( machine_name, BackslashChar );

return TRUE;
}

/***********************************************************************
*                      SYMPHONY_MACHINE_DIRECTORY                      *
*                         c:\v5\symphony\M03\                          *
*     The returned buffer is MAX_PATH+1. You can append to it.         *
***********************************************************************/
static TCHAR * symphony_machine_directory( TCHAR * machine_name )
{
static TCHAR path[MAX_PATH+1];

TCHAR * cp;

cp = copystring( path, RootSymphonyDirectory.text() );
cp = copystring( cp, machine_name );
*cp++ = BackslashChar;
*cp = NullChar;

return path;
}

/***********************************************************************
*                       PARAMETER_INIT_MESSAGE                         *
***********************************************************************/
static TCHAR * parameter_init_message( int machine_index, int parameter_index )
{

static TCHAR DefaultValue[] = TEXT( "1.0" );
static TCHAR buf[256];

PARAMETER_ENTRY * p;
TCHAR * cp;

p = &Param[machine_index].parameter[parameter_index];

/*
--------
Param Id
-------- */
cp = copystring( buf, TEXT("<param id=\"") );
copystring( cp, p->name );
convert_parameter_name_to_id( cp );
cp = eofstring( cp );
*cp = QuoteChar;
cp++;

/*
----
Desc
---- */
cp = copystring( cp, entry_string(TEXT("desc"), p->name) );

/*
------------
Alarm values
------------ */
cp = copystring( cp, entry_string(TEXT("halEnabled"), p->has_alarm_limits())      );
cp = copystring( cp, entry_string(TEXT("halValue"),   p->limits[ALARM_MAX].value) );

cp = copystring( cp, entry_string(TEXT("hwlEnabled"), p->has_warning_limits())      );
cp = copystring( cp, entry_string(TEXT("hwlValue"),   p->limits[WARNING_MAX].value) );

cp = copystring( cp, entry_string(TEXT("lalEnabled"), p->has_alarm_limits())      );
cp = copystring( cp, entry_string(TEXT("lalValue"),   p->limits[ALARM_MIN].value) );

cp = copystring( cp, entry_string(TEXT("lwlEnabled"), p->has_warning_limits())      );
cp = copystring( cp, entry_string(TEXT("lwlValue"),   p->limits[WARNING_MIN].value) );

/*
-------
Current
------- */
cp = copystring( cp, entry_string(TEXT("curValType"), MeasuredParameterCVT) );
cp = copystring( cp, entry_string(TEXT("curValue"), DefaultValue) );

/*
----
Misc
---- */
cp = copystring( cp, entry_string(TEXT("precision"), 3) );
cp = copystring( cp, entry_string(TEXT("units"), units_name(p->units)) );

copystring( cp, TEXT(" />") );

return buf;
}

/***********************************************************************
*                         SHOT_PARAMETER_MESSAGE                       *
***********************************************************************/
TCHAR * shot_parameter_message( TCHAR * name, short units_id )
{
static STRING_CLASS s;

s = BeginMessage;
s += name;
s += UnitsMessage;
s += units_name( units_id );
s += EndMessage;

return s.text();
}

/***********************************************************************
*                        WRITE_PARAMS_XML_FILE                         *
***********************************************************************/
static void write_params_xml_file( int mi )
{

int         i;
int         pi;
double      dx;

STRING_CLASS  s;
FILE_CLASS    f;
ANALOG_SENSOR_SETUP_DATA * my_analog_sensor;
short analog_sensor_units[4];

s = symphony_machine_directory( Machine[mi].name );
if ( !directory_exists(s.text()) )
    create_directory( s.text() );

s += ParamsXmlFileName;

for ( i=0; i<4; i++ )
    {
    my_analog_sensor = find_analog_sensor( Part[mi].analog_sensor[i] );
    if ( !my_analog_sensor )
        {
        Part[mi].analog_sensor[i] = DEFAULT_ANALOG_SENSOR;
        my_analog_sensor = find_analog_sensor( Part[mi].analog_sensor[i] );
        }
    analog_sensor_units[i] = my_analog_sensor->units;
    }

f.open_for_write( s.text() );

f.writeline( TEXT("<?xml version=\"1.0\" encoding=\"utf-8\" ?>") );
f.writeline( TEXT("<params>") );

/*
----------------------
Machine name parameter
---------------------- */
s = MachineMessage;
s += Machine[mi].name;
s += EndParameterMessage;
f.writeline( s.text() );

/*
-------------------
Part name parameter
------------------- */
s = PartMessage;
s += Machine[mi].current_part;
s += EndParameterMessage;
f.writeline( s.text() );

f.writeline( shot_parameter_message( TEXT("Velocity_Pos"),  Part[mi].distance_units) );
f.writeline( shot_parameter_message( TEXT("Velocity_Time"), Part[mi].velocity_units) );
f.writeline( shot_parameter_message( TEXT("Analog0_Pos"),   analog_sensor_units[0])  );
f.writeline( shot_parameter_message( TEXT("Analog0_Time"),  analog_sensor_units[0])  );
f.writeline( shot_parameter_message( TEXT("Analog1_Pos"),   analog_sensor_units[1])  );
f.writeline( shot_parameter_message( TEXT("Analog1_Time"),  analog_sensor_units[1])  );
f.writeline( shot_parameter_message( TEXT("Analog2_Pos"),   analog_sensor_units[2])  );
f.writeline( shot_parameter_message( TEXT("Analog2_Time"),  analog_sensor_units[2])  );
f.writeline( shot_parameter_message( TEXT("Analog3_Pos"),   analog_sensor_units[3])  );
f.writeline( shot_parameter_message( TEXT("Analog3_Time"),  analog_sensor_units[3])  );
f.writeline( shot_parameter_message( TEXT("Time_Pos"),      MS_UNITS)                );
f.writeline( shot_parameter_message( TEXT("Position_Time"), MS_UNITS)                );

s = BeginMessage;
s += TEXT( "PosInc" );
s += UnitsMessage;
s += units_name( Part[mi].distance_units );
s += TEXT( "\" curValType=\"1\" curValue=\"" );
if ( Machine[mi].is_ftii )
    dx = 1.0;
else
    dx = 4.0;
s += ascii_double( Part[mi].dist_from_x4(dx) );
s += TEXT( "\" Precision=\"3\"/>" );
f.writeline( s.text() );

s = BeginMessage;
s += TEXT( "TimeInc" );
s += UnitsMessage;
s += units_name( MS_UNITS );
s += TEXT( "\" curValType=\"1\" curValue=\"" );
s += (int) Part[mi].ms_per_time_sample;
s += TEXT( "\" Precision=\"3\"/>" );
f.writeline( s.text() );

for ( pi=0; pi<Param[mi].count(); pi++ )
    f.writeline( parameter_init_message(mi, pi) );

f.writeline( TEXT("</params>") );

f.close();
}

/***********************************************************************
*                      SHOT_NUMBER_FROM_FILE_NAME                      *
***********************************************************************/
static int shot_number_from_file_name( TCHAR * filename )
{
static STRING_CLASS s;
TCHAR * cp;

s = filename;
cp = s.text();
while ( *cp != NullChar )
    {
    if ( *cp == DotChar )
        {
        *cp = NullChar;
        break;
        }
    cp++;
    }
return s.int_value();
}

/***********************************************************************
*                         SEND_UPDATE_MESSAGES                         *
***********************************************************************/
static void send_update_messages()
{
int         mi;
int         max_shot_number;
int         shot_number;

FILE_CLASS      f;
STRING_CLASS    basepath;
STRING_CLASS    path;
STRING_CLASS    s;
TEXT_LIST_CLASS t;
TIME_CLASS      ti;
BOOLEAN         have_listbox_entry;

for ( mi=0; mi<NofMachines; mi++ )
    {
    get_sorc_machine_directory( basepath,  Machine[mi].name );
    t.empty();
    t.get_file_list( basepath.text(), "*.pr?" );
    t.rewind();
    max_shot_number = 0;
    while( t.next() )
        {
        shot_number = shot_number_from_file_name( t.text() );
        if ( shot_number >= max_shot_number )
            {
            max_shot_number = shot_number;
            s = t.text();
            }
        }

    if ( max_shot_number > 0 )
        {
        path = basepath;
        path += s;
        create_shot_data_files( path.text(), max_shot_number, mi );

        /*
        ---------------------------------------------
        Test: rename the file to the next shot number
        s = path;

        shot_number = max_shot_number;
        copystring( oldname, int32toasc(shot_number) );
        fix_shotname( oldname );
        if ( shot_number == 999999 )
            shot_number = 1;
        else
            shot_number++;
        copystring( newname, int32toasc(shot_number) );
        fix_shotname( newname );

        s.replace( oldname, newname );
        MoveFile( path.text(), s.text() );
        --------------------------------------------- */

        /*
        -------------------------
        Delete all the shot files
        ------------------------- */
        t.rewind();
        while ( t.next() )
            {
            s = basepath;
            s += t.text();
            DeleteFile( s.text() );
            }

        /*
        -----------------------------
        Create the symphony stat file
        ----------------------------- */
        path = symphony_machine_directory( Machine[mi].name );
        path += StatFileName;
        f.open_for_write( path.text() );

        /*
        ------------
        Machine Name
        ------------ */
        s = Machine[mi].name;
        f.writeline( s.text() );

        /*
        ----------
        Shot Count
        ---------- */
        s = int32toasc( (int32)max_shot_number );
        f.writeline( s.text() );

        /*
        ---------
        Part Name
        --------- */
        s = Part[mi].name;
        f.writeline( s.text() );

        f.close();

        s = Machine[mi].computer;
        s += TabChar;
        s += Machine[mi].name;
        s += TabChar;
        have_listbox_entry = MainListbox.findprefix( s.text() );
        s += Part[mi].name;
        s += TabChar;
        s += int32toasc( (int32)max_shot_number );
        s += TabChar;
        ti.get_local_time();
        s += ti.text();
        if ( have_listbox_entry )
            MainListbox.replace( s.text() );
        else
            MainListbox.add( s.text() );
        }
    }
}

/***********************************************************************
*                          send_init_messages                          *
***********************************************************************/
static void send_init_messages()
{
int         mi;

for ( mi=0; mi<NofMachines; mi++ )
    write_params_xml_file( mi );
}

/***********************************************************************
*                        POSITION_MAIN_LISTBOX                         *
***********************************************************************/
static void position_main_listbox()
{
static int ids[] = { MACHINE_STATIC, PART_STATIC, SHOT_NUMBER_STATIC, SHOT_TIME_STATIC };
static int nof_ids = sizeof(ids)/sizeof(int);

RECTANGLE_CLASS r;
RECTANGLE_CLASS rc;
WINDOW_CLASS w;

w = MainListbox.handle();

MainDialog.get_client_rect( r );
w.get_move_rect( rc );
rc.left = r.left + 4;
rc.right = r.right - 4;
rc.bottom = r.bottom - 4;
w.move(rc);
set_listbox_tabs_from_title_positions( MainDialog.handle(), (UINT)MAIN_LISTBOX, ids, nof_ids );
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
static UINT MyTimerId = 0;
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_DBINIT:
        MainListbox.init( MainDialog.control(MAIN_LISTBOX) );
        position_main_dialog();
        load_machines();
        send_init_messages();
        MyTimerId = SetTimer( hWnd, 0, PollMs, NULL );
        return TRUE;

    case WM_TIMER:
        KillTimer( hWnd, MyTimerId );
        MyTimerId = 0;
        send_update_messages();
        MyTimerId = SetTimer( hWnd, 0, PollMs, NULL );
        return 0;

    case WM_INITDIALOG:
        MainDialog = hWnd;
        MainDialog.post( WM_DBINIT );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                MainWindow.close();
                return TRUE;
            }
        break;

    case WM_DESTROY:
        if ( MyTimerId )
            {
            KillTimer( hWnd, MyTimerId );
            MyTimerId = 0;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         SAVE_WINDOW_POSITION                         *
***********************************************************************/
static void save_window_position()
{
INI_CLASS       ini;
RECTANGLE_CLASS r;
STRING_CLASS    s;

s = exe_directory();
s.cat_path( PostDataIniFileName );

MainWindow.getrect( r );

ini.put_string( s.text(), ConfigSection, WindowRectangleKey, r.put() );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        break;

    case WM_HELP:
        gethelp( hWnd );
        return 0;

    case WM_SIZE:
        position_main_dialog();
        break;

    case WM_DBINIT:
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case SAVE_WINDOW_POSITION_CHOICE:
                save_window_position();
                return 0;

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
*                            READ_INI_FILE                             *
***********************************************************************/
static void read_ini_file( RECTANGLE_CLASS & dest )
{
static TCHAR data[] = TEXT( "data" );
static TCHAR symphony[] = TEXT( "symphony" );
INI_CLASS    ini;
STRING_CLASS s;

s = exe_directory();
s.cat_path( PostDataIniFileName );

ini.set_file( s.text() );
ini.set_section( ConfigSection );

RootSymphonyDirectory = ini.get_string( RootSymphonyDirectoryKey );
ParamsXmlFileName     = ini.get_string( ParamsXmlFileNameKey     );
StatFileName          = ini.get_string( StatFileNameKey          );
DataFileName          = ini.get_string( DataFileNameKey          );
ShotDataFileName      = ini.get_string( ShotDataFileNameKey      );
PollMs                = ini.get_int( PollMsKey );
if ( PollMs < 1000 )
    PollMs = 10000;

/*
------------------------------------------------------------------------------
The root sorc directory doesn't have to be read. It's like the data directory.
------------------------------------------------------------------------------ */
RootSorcDirectory = root_directory();
RootSorcDirectory.replace( data, symphony );

if ( ini.find(WindowRectangleKey) )
    dest.get( ini.get_string() );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
STRING_CLASS    title;
WNDCLASS        wc;
COMPUTER_CLASS  c;
RECTANGLE_CLASS r;

names_startup();
c.startup();
units_startup();
analog_sensor_startup();

r.top = 0;
r.left = 0;
r.right = 700;
r.bottom = 500;
read_ini_file( r );

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
    r.left, r.top,     // X,y
    r.width(), r.height(),                         // W,h
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
MainDialog.show();
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{

cleanup_globals();
analog_sensor_shutdown();
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
