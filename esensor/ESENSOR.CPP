#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\names.h"

#include "resource.h"

#define _MAIN_
#include "..\include\events.h"

HINSTANCE    MainInstance;
HWND         MainWindow;
HWND         SensorWindow;
STRING_CLASS MyWindowTitle;
LISTBOX_CLASS SensorNumberListbox;
LISTBOX_CLASS UnitsCbox;
LISTBOX_CLASS VartypeCbox;

TCHAR MyClassName[]   = TEXT( "ESensor" );

const TCHAR ZeroChar = TEXT( '0' );
const TCHAR NullChar = TEXT( '\0' );

struct VARTYPE_ENTRY
    {
    BITSETYPE value;
    TCHAR     name[VARTYPE_NAME_LEN+1];
    };

struct SENSOR_ENTRY
     {
     BOOLEAN has_changed;
     unsigned short number;
     TCHAR desc[ASENSOR_DESC_LEN+1];
     float high;
     float low;
     BITSETYPE var;
     TCHAR units[UNITS_LEN+1];
     unsigned short gain;
     float multiplier;
     float subtrahend;
     float divisor;
     float adder;
     float mconvert;
     unsigned short equ;
     };

SENSOR_ENTRY Sensor[MAX_ANALOG_SENSORS];
short        NofSensors     = 0;

VARTYPE_ENTRY * Vartypes    = 0;
short           NofVartypes = 0;

/*
---------------------
Current Sensor Number
--------------------- */
int32 sn = 0;

/***********************************************************************
*                            RESOURCE_STRING                           *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes( void )
{
DB_TABLE t;
short   i;
SENSOR_ENTRY * se;

if ( t.open(asensor_dbname(), ASENSOR_RECLEN, FL) )
    {
    t.empty();
    se = Sensor;
    for ( i=0; i<NofSensors; i++ )
        {
        t.put_short( ASENSOR_NUMBER_OFFSET,        se->number,          ASENSOR_NUMBER_LEN );
        t.put_alpha( ASENSOR_DESC_OFFSET,          se->desc,            ASENSOR_DESC_LEN );
        t.put_float( ASENSOR_HIGH_VALUE_OFFSET,    se->high,            ASENSOR_FLOAT_LEN );
        t.put_float( ASENSOR_LOW_VALUE_OFFSET,     se->low,             ASENSOR_FLOAT_LEN );
        t.put_long(  ASENSOR_VARIABLE_TYPE_OFFSET, long(se->var),       BITSETYPE_LEN );
        t.put_short( ASENSOR_UNITS_OFFSET,         units_id(se->units), UNITS_ID_LEN );
        t.put_short( ASENSOR_GAIN_OFFSET,          se->gain,            ASENSOR_GAIN_LEN );
        t.put_float( ASENSOR_MUL_OFFSET,           se->multiplier,      ASENSOR_FLOAT_LEN );
        t.put_float( ASENSOR_SUB_OFFSET,           se->subtrahend,      ASENSOR_FLOAT_LEN );
        t.put_float( ASENSOR_DIV_OFFSET,           se->divisor,         ASENSOR_FLOAT_LEN );
        t.put_float( ASENSOR_ADD_OFFSET,           se->adder,           ASENSOR_FLOAT_LEN );
        t.put_float( ASENSOR_MILLI_VOLT_OFFSET,    se->mconvert,        ASENSOR_FLOAT_LEN );
        t.put_short( ASENSOR_POLY_EQ_OFFSET,       se->equ,             ASENSOR_POLY_EQ_LEN );
        t.rec_append();
        se++;
        }
    t.close();
    }
}

/***********************************************************************
*                            SHOW_SENSOR                               *
***********************************************************************/
static void show_sensor( void )
{
short i;
SENSOR_ENTRY * se;

se = Sensor + sn;

set_text( SensorWindow, SENSOR_DESCRIPTION_EBOX, se->desc );
set_text( SensorWindow, MAX_VALUE_EBOX,          ascii_float(se->high) );
set_text( SensorWindow, MIN_VALUE_EBOX,          ascii_float(se->low)  );
set_text( SensorWindow, GAIN_EBOX,               int32toasc(se->gain)  );
set_text( SensorWindow, MUL_EBOX,                ascii_float(se->multiplier) );
set_text( SensorWindow, SUB_EBOX,                ascii_float(se->subtrahend) );
set_text( SensorWindow, DIV_EBOX,                ascii_float(se->divisor) );
set_text( SensorWindow, ADD_EBOX,                ascii_float(se->adder) );
UnitsCbox.setcursel( se->units );
if ( Vartypes )
    {
    for ( i=0; i<NofVartypes; i++ )
        {
        if ( Vartypes[i].value == se->var )
            {
            VartypeCbox.setcursel( Vartypes[i].name );
            break;
            }
        }
    }

}

/***********************************************************************
*                            UPDATE_UNITS                              *
***********************************************************************/
static void update_units( void )
{
lstrcpy( Sensor[sn].units, UnitsCbox.selected_text() );
Sensor[sn].has_changed = TRUE;
}

/***********************************************************************
*                            UPDATE_VARTYPE                            *
***********************************************************************/
static void update_vartype( void )
{
int i;
STRING_CLASS s;

Sensor[sn].var = NO_VARIABLE_TYPE;
s = VartypeCbox.selected_text();
for ( i=0; i<NofVartypes; i++ )
    {
    if ( s == Vartypes[i].name )
        {
        Sensor[sn].var = Vartypes[i].value;
        Sensor[sn].has_changed = TRUE;
        break;
        }
    }
}

/***********************************************************************
*                            UPDATE_DESC                               *
***********************************************************************/
static void update_desc ( void )
{
get_text( Sensor[sn].desc, SensorWindow, SENSOR_DESCRIPTION_EBOX, ASENSOR_DESC_LEN+1 );
Sensor[sn].has_changed = TRUE;
}

/***********************************************************************
*                            UPDATE_GAIN                               *
***********************************************************************/
static void update_gain ( void )
{
Sensor[sn].gain = (short) GetDlgItemInt( SensorWindow, GAIN_EBOX, NULL, TRUE );
Sensor[sn].has_changed = TRUE;
}

/***********************************************************************
*                          FLOAT_FROM_TEXTBOX                          *
***********************************************************************/
static float float_from_textbox( HWND w, INT box )
{
TCHAR buf[MAX_FLOAT_LEN+1];
float x;

if ( get_text(buf, w, box,  MAX_FLOAT_LEN+1) )
    x = extfloat( buf );
else
    x = 0.0;

return x;
}

/***********************************************************************
*                          FLOAT_FROM_TEXTBOX                          *
***********************************************************************/
static float float_from_textbox( INT box )
{
return float_from_textbox( SensorWindow, box );
}

/***********************************************************************
*                            UPDATE_HIGH                               *
***********************************************************************/
static void update_high( void )
{
Sensor[sn].high = float_from_textbox( MAX_VALUE_EBOX );
Sensor[sn].has_changed = TRUE;
}

/***********************************************************************
*                            UPDATE_LOW                                *
***********************************************************************/
static void update_low( void )
{
Sensor[sn].low         = float_from_textbox( MIN_VALUE_EBOX );
Sensor[sn].has_changed = TRUE;
}

/***********************************************************************
*                            UPDATE_MULTIPLIER                         *
***********************************************************************/
static void update_multiplier( void )
{
Sensor[sn].multiplier  = float_from_textbox( MUL_EBOX );
Sensor[sn].has_changed = TRUE;
}

/***********************************************************************
*                            UPDATE_SUBTRAHEND                         *
***********************************************************************/
static void update_subtrahend ( void )
{
Sensor[sn].subtrahend  = float_from_textbox( SUB_EBOX );
Sensor[sn].has_changed = TRUE;
}

/***********************************************************************
*                            UPDATE_DIVISOR                            *
***********************************************************************/
static void update_divisor( void )
{
Sensor[sn].divisor     = float_from_textbox( DIV_EBOX );
Sensor[sn].has_changed = TRUE;
}

/***********************************************************************
*                            UPDATE_ADDER                             *
***********************************************************************/
static void update_adder ( void )
{
Sensor[sn].adder       = float_from_textbox( ADD_EBOX );
Sensor[sn].has_changed = TRUE;
}

/***********************************************************************
*                               FILL_UNITS                             *
***********************************************************************/
static void fill_units ( void )
{
DB_TABLE t;
TCHAR buf[UNITS_LEN+1];


UnitsCbox.redraw_off();
UnitsCbox.empty();

if ( t.open( units_dbname(), UNITS_RECLEN, PFL ) )
    {
    while ( t.get_next_record(FALSE) )
        {
        if ( t.get_alpha(buf, UNITS_NAME_OFFSET, UNITS_LEN) )
            UnitsCbox.add( buf );
        }
    t.close();
    }

UnitsCbox.set_current_index( 0 );
UnitsCbox.redraw_on();
}

/***********************************************************************
*                       FILL_SENSOR_NUMBER_LISTBOX                     *
***********************************************************************/
static void fill_sensor_number_listbox( void )
{
int i;

SensorNumberListbox.redraw_off();
SensorNumberListbox.empty();

for ( i=0; i<NofSensors; i++ )
    SensorNumberListbox.add( int32toasc(Sensor[i].number) );

if ( sn < 0 || sn >= NofSensors )
    sn = 0;

SensorNumberListbox.set_current_index( sn );
SensorNumberListbox.redraw_on();

show_sensor();
}

/***********************************************************************
*                            LOAD_SENSORS                              *
***********************************************************************/
static void load_sensors( void )
{
DB_TABLE t;
short   i;

NofSensors = 0;

if ( t.open( asensor_dbname(), ASENSOR_RECLEN, PFL ) )
     {
     while ( t.get_next_record(FALSE) )
          {
          Sensor[NofSensors].number = t.get_short(ASENSOR_NUMBER_OFFSET);
          t.get_alpha( Sensor[NofSensors].desc, ASENSOR_DESC_OFFSET, ASENSOR_DESC_LEN );

          Sensor[NofSensors].high = t.get_float (ASENSOR_HIGH_VALUE_OFFSET);
          Sensor[NofSensors].low = t.get_float (ASENSOR_LOW_VALUE_OFFSET);

          Sensor[NofSensors].var = (BITSETYPE) t.get_long( ASENSOR_VARIABLE_TYPE_OFFSET);

          i = t.get_short( ASENSOR_UNITS_OFFSET );
          lstrcpy( Sensor[NofSensors].units, units_name(i) );

          Sensor[NofSensors].gain       = t.get_short( ASENSOR_GAIN_OFFSET       );
          Sensor[NofSensors].multiplier = t.get_float( ASENSOR_MUL_OFFSET        );
          Sensor[NofSensors].subtrahend = t.get_float( ASENSOR_SUB_OFFSET        );
          Sensor[NofSensors].divisor    = t.get_float( ASENSOR_DIV_OFFSET        );
          Sensor[NofSensors].adder      = t.get_float( ASENSOR_ADD_OFFSET        );
          Sensor[NofSensors].mconvert   = t.get_float( ASENSOR_MILLI_VOLT_OFFSET );
          Sensor[NofSensors].equ        = t.get_short( ASENSOR_POLY_EQ_OFFSET    );
          NofSensors++;
          }
     t.close();
     }

fill_units();

sn = 0;
fill_sensor_number_listbox();
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static void paint_me( void )
{

PAINTSTRUCT ps;

BeginPaint( MainWindow, &ps );
EndPaint( MainWindow, &ps );

}

/***********************************************************************
*                             ADD_SENSOR                               *
***********************************************************************/
static void add_sensor( void )
{
SENSOR_ENTRY s;
int32 i;

if ( NofSensors >= MAX_ANALOG_SENSORS )
    {
    resource_message_box( CANNOT_COMPLY_STRING, SENSOR_LIST_FULL_STRING );
    return;
    }

/*
-----------------------
Copy the current sensor
----------------------- */
s = Sensor[sn];

/*
------------------------
Find the first open slot
------------------------ */
sn = NofSensors;
for ( i=0; i<NofSensors; i++ )
    {
    if ( Sensor[i].number > i )
        {
        sn = i;
        break;
        }
    }

s.number = sn;

/*
------------------------------
Move any higher numbers up one
------------------------------ */
i = NofSensors;
while ( i > sn )
    {
    Sensor[i] = Sensor[i-1];
    i--;
    }

/*
--------------
Copy mine here
-------------- */
Sensor[sn] = s;

NofSensors++;
fill_sensor_number_listbox();
}

/***********************************************************************
*                            DELETE_SENSOR                             *
***********************************************************************/
static void delete_sensor( void )
{
int i;

if ( NofSensors <= 1 )
    return;

if ( sn >= 0 && sn < NofSensors )
    {
    i = sn;
    NofSensors--;
    while ( i < NofSensors )
        {
        Sensor[i] = Sensor[i+1];
        i++;
        }
    if ( sn >= NofSensors )
        sn--;
    fill_sensor_number_listbox();
    }
}

/***********************************************************************
*                            FILL_VARTYPES                             *
***********************************************************************/
static void fill_vartypes( void )
{
short i;

if ( !Vartypes )
    return;

VartypeCbox.empty();
for ( i=0; i<NofVartypes; i++ )
    VartypeCbox.add( Vartypes[i].name );

VartypeCbox.set_current_index( 0 );
VartypeCbox.redraw_on();
}

/***********************************************************************
*                            LOAD_VARTYPES                             *
***********************************************************************/
static void load_vartypes( void )
{

int n;
DB_TABLE t;
VARTYPE_ENTRY * v;

if ( Vartypes )
    {
    delete[] Vartypes;
    Vartypes = 0;
    }

if ( t.open(vartype_dbname(), VARTYPE_RECLEN, PFL) )
    {
    n = t.nof_recs();
    NofVartypes = 0;
    if ( n > 0 )
        {
        Vartypes = new VARTYPE_ENTRY[n+1];
        if ( Vartypes )
            {
            v = Vartypes;
            v->value = 0;
            lstrcpy( v->name, resource_string(MainInstance, NONE_STRING) );
            NofVartypes++;
            v++;
            while ( NofVartypes < n && t.get_next_record(FALSE) )
                {
                v->value = (BITSETYPE) t.get_long( VARTYPE_VALUE_OFFSET );
                t.get_alpha( v->name, VARTYPE_NAME_OFFSET, VARTYPE_NAME_LEN );
                v++;
                NofVartypes++;
                }
            }
        else
            {
            NofVartypes = 0;
            }
        }
    t.close();
    fill_vartypes();
    }
else
    {
    resource_message_box( UNABLE_TO_CONTINUE_STRING, NO_VARIABLE_TYPES_STRING );
    }

}

/***********************************************************************
*                      EDIT_ANALOG_SENSOR_DIALOG_PROC                  *
***********************************************************************/
BOOL CALLBACK edit_analog_sensor_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SensorWindow = hWnd;
        SensorNumberListbox.init( SensorWindow, SENSOR_NUMBER_LISTBOX );
        UnitsCbox.init(           SensorWindow, UNITS_CBOX            );
        VartypeCbox.init(         SensorWindow, VARTYPE_CBOX          );
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        load_vartypes();
        if ( !Vartypes )
            SendMessage( MainWindow, WM_CLOSE, 0, 0L );
        load_sensors();
        limit_text( SensorWindow, SENSOR_DESCRIPTION_EBOX, ASENSOR_DESC_LEN );
        limit_text( SensorWindow, GAIN_EBOX,               ASENSOR_GAIN_LEN );
        SetFocus( GetDlgItem(SensorWindow, SENSOR_NUMBER_LISTBOX) );
        return TRUE;

    case WM_COMMAND:
       switch ( id )
            {
            case SENSOR_NUMBER_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    sn = SensorNumberListbox.current_index();
                    show_sensor();
                    }
                break;

            case UNITS_CBOX:
                if ( cmd == LBN_SELCHANGE )
                    update_units();
                break;

            case VARTYPE_CBOX:
                if ( cmd == LBN_SELCHANGE )
                    update_vartype();
                break;

            case SENSOR_DESCRIPTION_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_desc();
                break;

            case MAX_VALUE_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_high();
                break;

            case MIN_VALUE_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_low();
                break;

            case GAIN_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_gain();
                break;

            case MUL_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_multiplier();
                break;

            case SUB_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_subtrahend();
                break;

            case DIV_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_divisor();
                break;

            case ADD_EBOX:
                if ( cmd == EN_KILLFOCUS )
                    update_adder();
                break;

            case SAVE_CHANGES_BUTTON:
                save_changes();
            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                break;
            }
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                       CALCULATE_LINEAR_EQUATION                      *
***********************************************************************/
static void calculate_linear_equation( HWND w )
{
float low_volts;
float high_volts;

float low_atod_counts;
float high_atod_counts;

low_volts  = float_from_textbox( w, MIN_VOLTS_EBOX );
high_volts = float_from_textbox( w, MAX_VOLTS_EBOX );

low_volts  *= (float) Sensor[sn].gain;
high_volts *= (float) Sensor[sn].gain;

low_atod_counts  = low_volts * 409.5;
high_atod_counts = high_volts * 409.5;

Sensor[sn].divisor    = high_atod_counts - low_atod_counts;
if ( is_float_zero(Sensor[sn].divisor) )
    Sensor[sn].divisor = 1.0;

Sensor[sn].subtrahend = low_atod_counts;
Sensor[sn].multiplier = Sensor[sn].high - Sensor[sn].low;
Sensor[sn].adder      = Sensor[sn].low;

Sensor[sn].has_changed = TRUE;
set_text( SensorWindow, MUL_EBOX, ascii_float(Sensor[sn].multiplier) );
set_text( SensorWindow, DIV_EBOX, ascii_float(Sensor[sn].divisor)    );
set_text( SensorWindow, SUB_EBOX, ascii_float(Sensor[sn].subtrahend) );
set_text( SensorWindow, ADD_EBOX, ascii_float(Sensor[sn].adder)      );
}

/***********************************************************************
*                     CALC_LINEAR_CONVERT_DIALOG_PROC                  *
***********************************************************************/
BOOL CALLBACK calc_linear_convert_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CALC_LINEAR_EQUATION_BUTTON:
                calculate_linear_equation( hWnd );

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
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

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        break;

    case WM_COMMAND:
        switch (id)
            {
            case MINIMIZE_MENU:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_CHOICE:
                SendMessage( hWnd, WM_CLOSE, 0, 0L );
                break;

            case CREATE_CHOICE:
                add_sensor();
                return 0;

            case DELETE_CHOICE:
                delete_sensor();
                return 0;

            case CALC_LINEAR_CONVERT_CHOICE:
                DialogBox( MainInstance, TEXT("CALC_LINEAR_CONVERT_DIALOG"), MainWindow, (DLGPROC) calc_linear_convert_dialog_proc );
                return 0;
            }

        break;

    case WM_PAINT:
        MainWindow = hWnd;
        paint_me();
        return 0;

    case WM_EV_SHUTDOWN:
        SendMessage( hWnd, WM_CLOSE, 0, 0L );
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

SensorWindow = 0;
names_startup();
c.startup();
units_startup();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MyClassName );
wc.lpszMenuName  = TEXT("GeneralMenu");
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    0, 0,                             // X,y
    460, 320,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

SensorWindow  = CreateDialog(
     MainInstance,
     TEXT("EDIT_ANALOG_SENSOR"),
     MainWindow,
    (DLGPROC) edit_analog_sensor_dialog_proc );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
}

/***********************************************************************
*                            VOID SHUTDOWN                             *
***********************************************************************/
static void shutdown( void )
{
COMPUTER_CLASS c;

client_dde_shutdown();
units_shutdown();
c.shutdown();

if ( Vartypes )
    delete[] Vartypes;
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{

MSG   msg;

MainInstance = this_instance;
MyWindowTitle = resource_string( MAIN_WINDOW_TITLE_STRING );

if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return 0;

init( cmd_show);

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    if ( (SensorWindow == 0) || !IsDialogMessage(SensorWindow,&msg) )
        {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }
    }

shutdown();
return(msg.wParam);
}
