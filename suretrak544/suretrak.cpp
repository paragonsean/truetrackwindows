#include <windows.h>
#include <math.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\computer.h"
#include "..\include\genlist.h"
#include "..\include\listbox.h"
#include "..\include\msgclas.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\structs.h"
#include "..\include\stringcl.h"
#include "..\include\stparam.h"
#include "..\include\stsetup.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"
#include "..\include\ringcl.h"
#include "..\include\runlist.h"
#include "..\include\plotclas.h"
#include "..\include\fileclas.h"

#include "resource.h"
#include "..\include\ftclass.h"

#define _MAIN_
#include "extern.h"
#include "..\include\events.h"

static const int32    MAX_PENDING_SURETRAK_COMMANDS = 100;
static const int32    MAX_STEP_LINE_LEN             = 80;
static const COLORREF BlackColor                    = RGB( 0, 0, 0 );
static const COLORREF RedColor                      = RGB( 255, 0, 0 );
static const COLORREF GreenColor                    = RGB( 0, 255, 0 );
static const int32    OPEN_FILE_TYPE = 1;
static const int32    SAVE_FILE_TYPE = 2;

WINDOW_CLASS ParamWindow;
static PLOT_CLASS           SureTrakPlot;

COMPUTER_CLASS  MyComputer;

TCHAR MyName[]              = TEXT( "Test" );
TCHAR CommaString[]         = TEXT( "," );
TCHAR EmptyString[]         = TEXT( "" );
TCHAR SureTrakIniFile[]     = TEXT( "suretrak.ini" );
TCHAR VisiTrakIniFile[]     = TEXT( "visitrak.ini" );
TCHAR ConfigSectionName[]   = TEXT( "Config" );
TCHAR ButtonLevelsSection[] = TEXT( "ButtonLevels" );
TCHAR InitialMoveStepName[] = TEXT( "InitialMoveStep" );
TCHAR RetractStepName[]     = TEXT( "RetractStep" );
TCHAR CommentIntroducer[]   = TEXT( "<-- " );
TCHAR CommentDelimiter[]    = TEXT( " ;" );
static TCHAR ZeroVoltsString[]     = TEXT( "0" );

const TCHAR BChar         = TEXT( 'B' );
const TCHAR CChar         = TEXT( 'C' );
const TCHAR EChar         = TEXT( 'E' );
const TCHAR MinusChar     = TEXT( '-' );
const TCHAR NChar         = TEXT( 'N' );
const TCHAR NullChar      = TEXT( '\0' );
const TCHAR PChar         = TEXT( 'P' );
const TCHAR PeriodChar    = TEXT( '.' );
const TCHAR SpaceChar     = TEXT( ' ' );
const TCHAR SemiColonChar = TEXT( ';' );
const TCHAR TabChar       = TEXT( '\t' );
const TCHAR TTChar        = TEXT( 'T' );
const TCHAR VChar         = TEXT( 'V' );
const TCHAR ZeroChar      = TEXT( '0' );

STRING_CLASS MyTitle;

static HBRUSH GreenBackgroundBrush = 0;
static HBRUSH RedBackgroundBrush   = 0;

/*
------------------
First Step Globals
------------------ */
static int32   FirstStepNumber = 44;
static TCHAR   NormalFirstStep[MAX_STEP_LINE_LEN+1];
static TCHAR   AlternateFirstStep[MAX_STEP_LINE_LEN+1];
static TCHAR   AlternateLineEnd[MAX_STEP_LINE_LEN+1];

static TCHAR   DefaultFirstStep[]     = TEXT( "T-1 B1.2"     );
static TCHAR   DefaultAlternateStep[] = TEXT( "V4AC5P2 E1.2" );

/*
--------------------
Retract Step Globals
-------------------- */
static int32 RetractStepNumber = 4;
static TCHAR RetractLineEnd[MAX_STEP_LINE_LEN+1];

/*
-------------------
Valve Start Globals
------------------- */
static int32   ValveStartStepNumber = 42;
static TCHAR   NormalValveStartStep[MAX_STEP_LINE_LEN+1];
static TCHAR   AlternateValveStartStep[MAX_STEP_LINE_LEN+1];
static TCHAR   ValveStartComment[MAX_STEP_LINE_LEN+1];

static TCHAR   DefaultValveStartStep[]          = TEXT( "T-4LD0.25 N"     );
static TCHAR   DefaultAlternateValveStartStep[] = TEXT( "T6800 N" );
static TCHAR   DefaultValveStartComment[]       = TEXT( "Set valve to center pos + lvdt and null offsets" );

/*
------------------
Valve Test Globals
------------------ */
const float VALVE_MS_MAX    = 1000.0;
const float VALVE_VOLTS_MAX = 5.0;
const float VALVE_MS_MIN    = 0.0;
const float VALVE_VOLTS_MIN = 0.0;
const int32 VALVE_MS_AXIS_ID    = 1;
const int32 VALVE_VOLTS_AXIS_ID = 2;
const int32 VALVE_REF_VOLTS_AXIS_ID = 3;

static BOOLEAN ProgramIsUpdating = FALSE;
static BOOLEAN HaveValveTestMaster = FALSE;

void start_suretrak_monitor_thread( void );
BOOLEAN get_suretrak_part_name( void );
BOOL CALLBACK transducer_test_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam );

/***********************************************************************
*                            RESOURCE_STRING                           *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                              GRAY_MENU                               *
***********************************************************************/
static void gray_menu()
{
UINT  menu_flags;
HMENU menu_handle;

if ( CurrentPasswordLevel >= ExitEnableLevel )
    menu_flags = MF_ENABLED;
else
    menu_flags = MF_GRAYED;

menu_handle = GetMenu( MainWindow.handle() );

if ( menu_handle )
    EnableMenuItem( menu_handle, EXIT_TASK_MENU, menu_flags );
}

/***********************************************************************
*                             GRAY_CONTROLS                            *
***********************************************************************/
static void gray_controls( HWND w, int * id, int n )
 {

int  i;
BOOL is_enabled;

if ( CurrentPasswordLevel >= SYSTEM_PASSWORD_LEVEL )
    is_enabled = TRUE;
else
    is_enabled = FALSE;

for ( i=0; i<n; i++ )
    EnableWindow( GetDlgItem(w,id[i]), is_enabled );

}

/***********************************************************************
*                             GRAY_CONTROLS                            *
***********************************************************************/
static void gray_controls( void )
{
static int id[] = {
    EDIT_CONTROL_PGM_BUTTON,
    PARAMETERS_BUTTON,
    SERVICE_TOOLS_BUTTON
    };

const int32 n = sizeof(id)/sizeof(int);

gray_controls( MainScreenWindow.handle(), id, n );



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
    WinHelp( MainWindow.handle(), fname, helptype, context );
    }
}

/***********************************************************************
*                         DRAW_VERTICAL_STRING                         *
*                                                                      *
* The return value is the rightmost pixel location I have written to.  *
***********************************************************************/
int32 draw_vertical_string( HDC dc, TCHAR * sorc, int32 x, int32 y )
{
int32      dx;
int32      h;

TEXTMETRIC tm;
SIZE       sorc_size;

if ( !sorc )
    return x;

GetTextMetrics( dc, &tm );

h = tm.tmHeight;

SetTextAlign( dc, TA_TOP | TA_CENTER );

dx = 0;
while ( *sorc )
    {
    GetTextExtentPoint32( dc, sorc, 1, &sorc_size );
    if ( dx < sorc_size.cx )
        dx = sorc_size.cx;
    TextOut( dc, x, y, sorc, 1 );
    y += h;
    sorc++;
    }

return x + dx;
}

/***********************************************************************
*                UPDATE_MAIN_SCREEN_LIMIT_SWITCH_NAMES                 *
***********************************************************************/
static void update_main_screen_limit_switch_names( void )
{
HWND w;

w = MainScreenWindow.handle();

set_text( w, LS1_RADIO, SureTrakParameters.limit_switch_desc(0) );
set_text( w, LS2_RADIO, SureTrakParameters.limit_switch_desc(1) );
set_text( w, LS3_RADIO, SureTrakParameters.limit_switch_desc(2) );
set_text( w, LS4_RADIO, SureTrakParameters.limit_switch_desc(3) );
}

/***********************************************************************
*                  UPDATE_MAIN_SCREEN_DISTANCE_UNITS                   *
***********************************************************************/
static void update_main_screen_distance_units( void )
{
HWND w;

w = MainScreenWindow.handle(),

set_text( w, MAIN_POSITION_UNITS_TBOX, units_name( DistanceUnits) );
}

/***********************************************************************
*                           FILL_PARAMETERS                            *
***********************************************************************/
static void fill_parameters( void )
{
HWND w;

w = ParamWindow.handle();

if ( !w )
    return;

set_text( w, VELOCITY_DIV_TBOX,   ultoascii(SureTrakParameters.velocity_dividend())  );
set_text( w, NULL_DAC_EDITBOX,       int32toasc(SureTrakParameters.null_dac_offset())   );
set_text( w, LVDT_DAC_EDITBOX,       int32toasc(SureTrakParameters.lvdt_dac_offset())   );

set_text( w, ZERO_SPEED_CHECK_EDITBOX, ascii_float(SureTrakParameters.zero_speed_check_pos())   );
set_text( w, COUNTS_PER_UNIT_TBOX,     ascii_float(SureTrakParameters.counts_per_unit())        );
set_text( w, JOG_SHOT_VALVE_EDITBOX,   ascii_float(SureTrakParameters.jog_valve_percent())      );
set_text( w, RETRACT_VALVE_EDITBOX,    ascii_float(SureTrakParameters.retract_valve_percent())  );
set_text( w, FT_VALVE_EDITBOX,         ascii_float(SureTrakParameters.ft_valve_percent())       );
set_text( w, VELOCITY_MULTIPLIER_TBOX, ascii_float(SureTrakParameters.vel_multiplier())         );
set_text( w, MIN_LI_EDITBOX,           ascii_float(SureTrakParameters.min_low_impact_percent()) );

set_text( w, LS1_DESCRIPTION_EDITBOX, SureTrakParameters.limit_switch_desc(0) );
set_text( w, LS2_DESCRIPTION_EDITBOX, SureTrakParameters.limit_switch_desc(1) );
set_text( w, LS3_DESCRIPTION_EDITBOX, SureTrakParameters.limit_switch_desc(2) );
set_text( w, LS4_DESCRIPTION_EDITBOX, SureTrakParameters.limit_switch_desc(3) );
set_text( w, MAX_VELOCITY_EDITBOX, ascii_float(SureTrakParameters.max_velocity()) );

CheckRadioButton( w, VACUUM_1_RADIO, VACUUM_4_RADIO, VACUUM_1_RADIO + SureTrakParameters.vacuum_limit_switch_number() );
}

/***********************************************************************
*                             GET_BOARD_DATA                           *
***********************************************************************/
static BOOLEAN get_board_data( HWND w, UINT id, BOARD_DATA & dest )
{

TCHAR buf[MAX_INTEGER_LEN+1];

if ( get_text(buf, w, id, MAX_INTEGER_LEN) )
    {
    dest = (BOARD_DATA) asctoul( buf );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           SAVE_PARAMETERS                            *
***********************************************************************/
static void save_parameters( void )
{

HWND  w;
int32 i;
TCHAR buf[MAX_NUMBER_LEN+1];
TCHAR cbuf[DESC_LEN+1];

w = ParamWindow.handle();

/*
--------------------------------------------------------------
if ( get_text(buf, w, VELOCITY_DIV_TBOX, MAX_INTEGER_LEN) )
    SureTrakParameters.set_velocity_dividend( asctoul(buf) );
-------------------------------------------------------------- */

get_text( buf, w, NULL_DAC_EDITBOX, MAX_INTEGER_LEN );
SureTrakParameters.set_null_dac_offset( asctoint32(buf) );

get_text( buf, w, LVDT_DAC_EDITBOX, MAX_INTEGER_LEN );
SureTrakParameters.set_lvdt_dac_offset( asctoint32(buf) );

get_text( buf, w, ZERO_SPEED_CHECK_EDITBOX, MAX_FLOAT_LEN );
SureTrakParameters.set_zero_speed_check_pos( extfloat(buf) );

/*
--------------------------------------------------------
get_text( buf, w, COUNTS_PER_UNIT_TBOX, MAX_FLOAT_LEN );
SureTrakParameters.set_counts_per_unit( extfloat(buf) );
-------------------------------------------------------- */

get_text( buf, w, JOG_SHOT_VALVE_EDITBOX, MAX_FLOAT_LEN );
SureTrakParameters.set_jog_valve_percent( extfloat(buf) );

get_text( buf, w, RETRACT_VALVE_EDITBOX, MAX_FLOAT_LEN );
SureTrakParameters.set_retract_valve_percent( extfloat(buf) );

get_text( buf, w, FT_VALVE_EDITBOX, MAX_FLOAT_LEN );
SureTrakParameters.set_ft_valve_percent( extfloat(buf) );

/*
------------------------------------------------------------
get_text( buf, w, VELOCITY_MULTIPLIER_TBOX, MAX_FLOAT_LEN );
SureTrakParameters.set_vel_multiplier( extfloat(buf) );
------------------------------------------------------------ */

get_text( buf, w, MIN_LI_EDITBOX, MAX_FLOAT_LEN );
SureTrakParameters.set_min_low_impact_percent( extfloat(buf) );

get_text( cbuf, w, LS1_DESCRIPTION_EDITBOX, DESC_LEN );
SureTrakParameters.set_limit_switch_desc( 0, cbuf );

get_text( cbuf, w, LS2_DESCRIPTION_EDITBOX, DESC_LEN );
SureTrakParameters.set_limit_switch_desc( 1, cbuf );

get_text( cbuf, w, LS3_DESCRIPTION_EDITBOX, DESC_LEN );
SureTrakParameters.set_limit_switch_desc( 2, cbuf );

get_text( cbuf, w, LS4_DESCRIPTION_EDITBOX, DESC_LEN );
SureTrakParameters.set_limit_switch_desc( 3, cbuf );

get_text( buf, w, MAX_VELOCITY_EDITBOX, MAX_FLOAT_LEN );
SureTrakParameters.set_max_velocity( extfloat(buf) );
/*
---------------------------------------------------
See which limit switch is used for the vacuum check
--------------------------------------------------- */
for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
    {
    if ( is_checked(w, VACUUM_1_RADIO + i) )
        {
        SureTrakParameters.set_vacuum_limit_switch_number( i );
        break;
        }
    }

SureTrakParameters.save( MyComputer.whoami() );
}

/***********************************************************************
*                        SEND_COMMAND_TO_BOARD                         *
***********************************************************************/
void send_command_to_board( BOARD_DATA command )
{
TCHAR hexbuf[MAX_HEX_LEN+1];

int32toasc( hexbuf, command, HEX_RADIX );

if ( !SureTrakCommand.push( hexbuf) )
    MessageBox( 0, hexbuf, resource_string(RING_BUFFER_FULL_STRING), MB_OK | MB_SYSTEMMODAL );

}

/***********************************************************************
*                          SEND_REQUEST_PLUS_DATA                      *
***********************************************************************/
static void send_request_plus_data( BOARD_DATA request, BOARD_DATA data )
{
STRING_CLASS s;

s =  ultohex( request );
s += CommaString;
s += ultohex( data );

if ( !SureTrakCommand.push(s.text()) )
    MessageBox( 0, s.text(), resource_string(RING_BUFFER_FULL_STRING), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                          SEND_REQUEST_PLUS_DATA                      *
***********************************************************************/
static void send_request_plus_data( BOARD_DATA request, BOARD_DATA command, BOARD_DATA data )
{
STRING_CLASS s;

s =  ultohex( request );
s += CommaString;
s += ultohex( command );
s += CommaString;
s += ultohex( data );

if ( !SureTrakCommand.push(s.text()) )
    MessageBox( 0, s.text(), resource_string(RING_BUFFER_FULL_STRING), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                        SEND_PARAMETER_TO_BOARD                         *
***********************************************************************/
static void send_parameter_to_board( BOARD_DATA command, BOARD_DATA data )
{
send_request_plus_data( SET_PARAMETER_REQUEST, command, data );
}

/***********************************************************************
*                       SEND_VEL_DIVIDEND_TO_BOARD                     *
***********************************************************************/
static void send_vel_dividend_to_board( void )
{
STRING_CLASS s;

s =  ultohex( SET_VEL_DIVIDEND_REQUEST );
s += CommaString;
s += ultohex( SureTrakParameters.velocity_dividend() );

if ( !SureTrakCommand.push(s.text()) )
    MessageBox( 0, s.text(), resource_string(RING_BUFFER_FULL_STRING), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                     SEND_ZERO_SPEED_CHECK_POS                        *
***********************************************************************/
static void send_zero_speed_check_pos( void )
{
BOARD_DATA x;

x = (BOARD_DATA) ( SureTrakParameters.counts_per_unit() * SureTrakParameters.zero_speed_check_pos() );
send_parameter_to_board( SET_P_CHECK_ZSPEED,  x );
}

/***********************************************************************
*                         SEND_COUNTS_PER_UNIT                         *
***********************************************************************/
static void send_counts_per_unit( void )
{
BOARD_DATA x;

x = (BOARD_DATA) ( SureTrakParameters.counts_per_unit() * 100.0 );

send_parameter_to_board( SET_COUNTS_PER_UNIT,  x );
}

/***********************************************************************
*                             DAC_COMMAND                              *
***********************************************************************/
static BOARD_DATA dac_command( float percentage, BOOLEAN need_min_check )
{
const float real2000 = (float) 0x2000;
const float real100  = 100.0;

int32 command;
float y;

/*
-------------------------------------
Convert percentage to the dac command
------------------------------------- */
y = percentage;
y *= real2000;
y /= real100;
y += real2000;

command = (int32) y;

/*
------------
Check bounds
------------ */
if ( command > 0x3fff )
    command = 0x3fff;

if ( need_min_check && command < 0x2000 )
    command = 0x2200; /* Force to a positive voltage */

return (BOARD_DATA) command;
}

/***********************************************************************
*                     DOWNLOAD_PARAMETERS_TO_BOARD                     *
***********************************************************************/
static void download_parameters_to_board( void )
{
BOARD_DATA x;
BOARD_DATA gain;
BOARD_DATA last_gain;
BOARD_DATA last_velocity;
BOARD_DATA velocity;

send_vel_dividend_to_board();
send_counts_per_unit();
send_zero_speed_check_pos();
send_parameter_to_board( SET_NULL,          SureTrakParameters.null_dac_offset() );
send_parameter_to_board( SET_LVDT_OFFSET,   SureTrakParameters.lvdt_dac_offset() );
send_parameter_to_board( SET_JS_SETTING,    dac_command(SureTrakParameters.jog_valve_percent(), TRUE) );
send_parameter_to_board( SET_RETR_SETTING,  dac_command(SureTrakParameters.retract_valve_percent(), FALSE) );
send_parameter_to_board( SET_FT_SETTING,    dac_command(SureTrakParameters.ft_valve_percent(), TRUE) );
send_parameter_to_board( SET_VEL_MUL,       SureTrakParameters.vel_multiplier() * 100.0 );
send_parameter_to_board( SET_V_LOOP_GAIN,   (BOARD_DATA) SureTrakSetup.velocity_loop_gain() );

/*
---------------------------
Add followthru stop postion
--------------------------- */
last_gain     = 0;
last_velocity = 0;

for ( x=0; x<NOF_GAIN_BREAKS; x++ )
    {
    gain     = (BOARD_DATA) (SureTrakSetup.gain_break_gain(x) * 1000.0);
    velocity = (BOARD_DATA) (SureTrakSetup.gain_break_velocity(x) * 100.0 );

    if ( x > 0 && velocity == BOARD_DATA(0) )
        {
        gain     = last_gain;
        velocity = last_velocity;
        }
    else
        {
        last_gain     = gain;
        last_velocity = velocity;
        }

    if ( gain == BOARD_DATA(0) )
        gain = 1000;

    send_parameter_to_board( SET_BKGN1 + x,   gain     );
    send_parameter_to_board( SET_BK_VEL1 + x, velocity );
    }

}

/***********************************************************************
*                          PARAM_DIALOG_PROC                           *
***********************************************************************/
BOOL CALLBACK param_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ParamWindow = w;
        ParamWindow.post( WM_DBINIT );
        return TRUE;

    case WM_DBINIT:
        fill_parameters();
        return TRUE;

    case WM_CLOSE:
        ParamWindow = 0;
        EndDialog( w, 0 );
        return TRUE;

    case WM_HELP:
        gethelp( HELP_CONTEXT, GLOBAL_SURETRAK_PARAMETERS_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_CHANGES_BUTTON:
                temp_message( resource_string(SAVING_STRING) );
                save_parameters();
                update_main_screen_limit_switch_names();
                download_parameters_to_board();
                ParamWindow.post( WM_CLOSE );
                return TRUE;

            case CANCEL_CHANGES_BUTTON:
                SureTrakParameters.find( MyComputer.whoami() );
                fill_parameters();

            case IDCANCEL:
                ParamWindow.post( WM_CLOSE );
                return TRUE;

            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                           VALVE_TEST_PAINT                           *
***********************************************************************/
void valve_test_paint( HWND w )
{

PAINTSTRUCT  ps;

if ( !GetUpdateRect(w, NULL, FALSE) )
    return;

BeginPaint( w, &ps );

SureTrakPlot.paint( ps.hdc );

EndPaint( w, &ps );
}

/***********************************************************************
*                         SIZE_VALVE_TEST_PLOT                         *
***********************************************************************/
void size_valve_test_plot( HWND w )
{
HWND  bw;
RECT  r;
int32 h;

bw = GetDlgItem( w, SAVE_VALVE_TEST_MASTER_BUTTON );
if ( GetWindowRect(bw, &r) )
    {
    h = r.bottom - r.top;
    if ( GetClientRect(w, &r) )
        {
        r.bottom -= h;
        SureTrakPlot.size( w, r );
        }
    }

}

/***********************************************************************
*                         WRITE_MASTER_TRACE                           *
***********************************************************************/
void write_master_trace( void )
{
int32      i;
FILE_CLASS f;

if ( f.open_for_write(valve_test_master_filename(MyComputer.whoami())) )
    {
    for ( i=0; i<NofValveTestPoints; i++ )
        f.writeline( ascii_float(ValveTestData[i].y) );
    f.close();
    }

}

/***********************************************************************
*                          READ_MASTER_TRACE                           *
***********************************************************************/
void read_master_trace( void )
{
TCHAR    * cp;
int32      i;
FILE_CLASS f;
float      x;

if ( !f.open_for_read(valve_test_master_filename(MyComputer.whoami())) )
    {
    HaveValveTestMaster = FALSE;
    return;
    }

x = 0.0;
for ( i=0; i<NofValveTestPoints; i++ )
    {
    cp = f.readline();
    if ( cp )
        {
        MasterData[i].y = extfloat( cp );
        MasterData[i].x = x;
        }

    x += 2.0;
    }

f.close();
HaveValveTestMaster = TRUE;
}

/***********************************************************************
*                          INIT_VALVE_TEST_DATA                        *
***********************************************************************/
static void init_valve_test_data( void )
{
int32      i;
float      x;

x = 0.0;
for ( i=0; i<NofValveTestPoints; i++ )
    {
    ValveTestData[i].y = 0.8;
    ValveTestData[i].x = x;
    x += 2.0;
    }
}

/***********************************************************************
*                       GET_NEW_VALVE_TEST_DATA                        *
***********************************************************************/
static void get_new_valve_test_data( void )
{
SureTrakPlot.empty_curves();

if ( HaveValveTestMaster )
    SureTrakPlot.add_curve( VALVE_MS_AXIS_ID, VALVE_REF_VOLTS_AXIS_ID, NofValveTestPoints, MasterData );

SureTrakPlot.add_curve( VALVE_MS_AXIS_ID, VALVE_VOLTS_AXIS_ID, NofValveTestPoints, ValveTestData );
SureTrakPlot.refresh();
}

/***********************************************************************
*                        VALVE_TEST_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK valve_test_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_VALVE_TEST_MASTER_BUTTON:
                write_master_trace();
                read_master_trace();
                return TRUE;

            case IDCANCEL:
                ValveTestWindow.close();
                return TRUE;
            }
        break;

    case WM_NEW_DATA:
        get_new_valve_test_data();
        return TRUE;

    case WM_PAINT:
        valve_test_paint(w);
        return TRUE;

    case WM_INITDIALOG:
        NeedMonitorUpdate = FALSE;
        ValveTestWindow = w;
        SureTrakPlot.empty_labels();
        SureTrakPlot.add_label( X_AXIS_INDEX, resource_string(TIME_IN_MILLISECONDS_STRING),    BlackColor );
        SureTrakPlot.add_label( Y_AXIS_INDEX, resource_string(VOLTS_STRING),                   BlackColor );

        SureTrakPlot.empty_curves();
        SureTrakPlot.empty_axes();
        SureTrakPlot.dont_show_symbols();

        SureTrakPlot.set_tic_count( X_AXIS_INDEX, 10 );
        SureTrakPlot.add_axis( VALVE_MS_AXIS_ID,    X_AXIS_INDEX, VALVE_MS_MIN,    VALVE_MS_MAX,    BlackColor );
        SureTrakPlot.set_decimal_places( VALVE_MS_AXIS_ID, 0 );

        SureTrakPlot.add_axis( VALVE_VOLTS_AXIS_ID,     Y_AXIS_INDEX, VALVE_VOLTS_MIN, VALVE_VOLTS_MAX, BlackColor );
        SureTrakPlot.add_axis( VALVE_REF_VOLTS_AXIS_ID, Y_AXIS_INDEX, VALVE_VOLTS_MIN, VALVE_VOLTS_MAX, RedColor );
        SureTrakPlot.set_decimal_places( VALVE_VOLTS_AXIS_ID, 1 );
        SureTrakPlot.set_tic_count( Y_AXIS_INDEX, 10 );

        size_valve_test_plot( w );
        read_master_trace();
        init_valve_test_data();
        get_new_valve_test_data();
        send_command_to_board( VALVE_TEST_REQUEST );
        return TRUE;

    case WM_CLOSE:
        ValveTestWindow   = 0;
        NeedMonitorUpdate = TRUE;
        EndDialog( w, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        KILL_NULL_VALVE_DIALOG                        *
***********************************************************************/
static void kill_null_valve_dialog( BOOLEAN board_was_running )
{
HWND w;

send_command_to_board( NULL_VALVE_FINISH_REQUEST );

if ( board_was_running && !(SureTrakStatus & CVIS_CONTROL_ACTIVE) )
    {
    send_command_to_board( ACTIVATE_CONTROL_REQUEST );
    }

send_parameter_to_board( SET_NULL, SureTrakParameters.null_dac_offset() );
w = NullValveWindow;
NullValveWindow = 0;
EndDialog( w, 0 );
}

/***********************************************************************
*                        NULL_VALVE_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK null_valve_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOOLEAN board_was_running = FALSE;
static int32   current_null_offset = 0;
static bool    need_green          = false;
static bool    have_value          = false;
static COLORREF mycolor;
static int      mybrush;
HWND ws;

int   id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case NULL_VALVE_SAVE_BUTTON:
                SureTrakParameters.save( MyComputer.whoami() );
                kill_null_valve_dialog( board_was_running );
                return TRUE;

            case IDCANCEL:
                SureTrakParameters.set_null_dac_offset( current_null_offset );
                kill_null_valve_dialog( board_was_running );
                return TRUE;

            case MANUAL_NULL_BUTTON:
                send_parameter_to_board( SET_NULL, 0 );
                send_request_plus_data( SEND_COMMAND_BUSY_REQUEST, ZERO_DAC );
                set_text( w, NULL_STATUS_TBOX, resource_string(MANUAL_NULLING_STRING) );
                return TRUE;

            case AUTO_NULL_BUTTON:
                current_null_offset = SureTrakParameters.null_dac_offset();
                set_text( w, NULL_STATUS_TBOX, resource_string(AUTO_NULL_BEGIN_STRING) );
                send_command_to_board( AUTO_NULL_REQUEST );
                return TRUE;

            }
        break;

    case WM_CTLCOLORSTATIC:
        ws = (HWND) lParam;
        id = GetDlgCtrlID( ws );
        if ( id == NULL_VOLTAGE_TBOX && have_value )
            {
            if ( IsWindowEnabled(ws) )
                {
                if ( need_green )
                    {
                    mycolor = GreenColor;
                    mybrush = (int) GreenBackgroundBrush;
                    }
                else
                    {
                    mycolor = RedColor;
                    mybrush = (int) RedBackgroundBrush;
                    }

                SetBkColor ((HDC) wParam, mycolor );
                return mybrush;
                }
            }
        break;

    case WM_NEW_DATA:
        double y;
        have_value = true;
        y = (double) (short) SureTrakAnalogValue[0];
        y *= 5.0;
        y /= 2047.0;
        if ( y >= -0.5 && y <= 0.5 )
            need_green = true;
        else
            need_green = false;
        set_text( w, NULL_VOLTAGE_TBOX, fixed_string(y, 3, 2) );
        return TRUE;

    case WM_INITDIALOG:
        if ( SureTrakStatus & CVIS_CONTROL_ACTIVE )
            {
            board_was_running = TRUE;
            send_command_to_board( DEACTIVATE_CONTROL_REQUEST );
            }
        else
            {
            board_was_running = FALSE;
            }
        NullValveWindow = w;
        need_green = true;  // Start out green
        have_value = false;
        set_text( w, NULL_VOLTAGE_TBOX, EmptyString );
        current_null_offset = SureTrakParameters.null_dac_offset();
        send_command_to_board( NULL_VALVE_INIT_REQUEST );
        return TRUE;

    case WM_CLOSE:
        kill_null_valve_dialog( board_was_running );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      SEND_DAC_VOLTAGE_TO_BOARD                       *
***********************************************************************/
void send_dac_voltage_to_board( HWND w )
{
BOARD_DATA x;
float      voltage;
TCHAR      buf[MAX_FLOAT_LEN+1];

if ( get_text(buf, w, DAC_VOLTAGE_EBOX, MAX_FLOAT_LEN) )
    {
    voltage = extfloat( buf );
    voltage += 10.0;

    if ( voltage < 0.0 )
        voltage = 0.0;

    if ( voltage > 20.0 )
        voltage = 20.0;

    voltage *= 16383.0;
    voltage /= 20.0;

    x = (BOARD_DATA) voltage;

    send_request_plus_data( SEND_DATA_COMMAND_BUSY_REQUEST, COUT_TO_DAC, x );
    }

}

/***********************************************************************
*                          DAC_PORT_DIALOG_PROC                        *
*                            Modal Dialog                              *
***********************************************************************/
BOOL CALLBACK dac_port_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;
static BOOLEAN board_was_running = FALSE;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        if ( SureTrakStatus & CVIS_CONTROL_ACTIVE )
            {
            board_was_running = TRUE;
            send_command_to_board( DEACTIVATE_CONTROL_REQUEST );
            }
        else
            {
            board_was_running = FALSE;
            }
        NullValveWindow = hWnd;
        SetFocus( GetDlgItem(hWnd, DAC_VOLTAGE_EBOX) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case SET_DAC_VOLTAGE_BUTTON:
                send_dac_voltage_to_board( hWnd );
                SendMessage( GetDlgItem(hWnd, DAC_VOLTAGE_EBOX), EM_SETSEL, 0, -1 );
                return TRUE;

            case IDCANCEL:
                SendMessage( hWnd, WM_CLOSE, 0, 0 );
                return TRUE;
            }
        break;

    case WM_NEW_DATA:
        double y;
        y = (double) (short) SureTrakAnalogValue[0];
        y *= 5.0;
        y /= 2047.0;
        set_text( hWnd, NULL_VOLTAGE_TBOX, fixed_string(y, 3, 2) );
        return TRUE;

    case WM_CLOSE:
        if ( board_was_running && !(SureTrakStatus & CVIS_CONTROL_ACTIVE) )
            {
            send_command_to_board( ACTIVATE_CONTROL_REQUEST );
            }

        send_parameter_to_board( SET_NULL, SureTrakParameters.null_dac_offset() );
        NullValveWindow = 0;
        EndDialog( hWnd, 0 );
        return TRUE;

    }

return FALSE;
}

/***********************************************************************
*                          DO_SEND_DATA_PORT                           *
***********************************************************************/
static void do_send_data_port( HWND w )
{
TCHAR buf[MAX_HEX_LEN+1];
BOARD_DATA x;

if ( get_text(buf, w, MAX_HEX_LEN) )
    {
    x = (BOARD_DATA) hextoul( buf );
    send_request_plus_data( SEND_DATA_REQUEST, x );
    }

}

/***********************************************************************
*                        COMMAND_PORT_DIALOG_PROC                      *
*                            Modal Dialog                              *
***********************************************************************/
BOOL CALLBACK command_port_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int          id;
TCHAR        buf[MAX_HEX_LEN+1];
BOARD_DATA   x;
HWND         w;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case READ_DATA_PORT_BUTTON:
                w = GetDlgItem( hWnd, DATA_PORT_VALUE_EBOX );
                set_text( w, EmptyString );
                DataPortWindow = w;
                return TRUE;

            case SEND_DATA_PORT_BUTTON:
                w = GetDlgItem( hWnd, DATA_PORT_VALUE_EBOX );
                do_send_data_port( w );
                return TRUE;

            case CLEAR_COMMAND_PORT_BUTTON:
                send_request_plus_data( SEND_COMMAND_REQUEST, 0 );
                return TRUE;

            case SEND_COMMAND_BUTTON:
                w = GetDlgItem( hWnd, COMMAND_VALUE_EBOX );
                if ( get_text(buf, w, MAX_HEX_LEN) )
                    {
                    x = (BOARD_DATA) hextoul( buf );
                    send_request_plus_data( SEND_COMMAND_REQUEST, x );
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
*                        VARIABLES_DIALOG_PROC                         *
*                            Modal Dialog                              *
***********************************************************************/
BOOL CALLBACK variables_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;
BOARD_DATA command;
BOARD_DATA data;
HWND       w;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        VariablesWindow = hWnd;
        SetFocus( hWnd );
        return TRUE;

    case WM_CLOSE:
        VariablesWindow = 0;
        EndDialog( hWnd, 0 );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case READ_VARIABLE_BUTTON:
                if ( get_board_data( hWnd, VARIABLE_NUMBER_EBOX, command ) )
                    {
                    set_text( hWnd, VARIABLE_VALUE_EBOX, EmptyString );
                    send_request_plus_data( READ_VARIABLE_REQUEST, command );
                    w = GetDlgItem( hWnd, VARIABLE_NUMBER_EBOX );
                    SetFocus( w );
                    SendMessage( w, EM_SETSEL, 0, -1 );
                    }
                return TRUE;

            case SET_VARIABLE_BUTTON:
                if ( get_board_data( hWnd, VARIABLE_NUMBER_EBOX, command ) )
                    {
                    if ( get_board_data( hWnd, VARIABLE_VALUE_EBOX, data ) )
                        {
                        send_request_plus_data( SET_VARIABLE_REQUEST, command, data );
                        }
                    }
                return TRUE;

            case IDCANCEL:
                VariablesWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                       LOAD_STATUS_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK load_status_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int   id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        LoadStatusWindow = hWnd;
        return TRUE;

    case WM_CLOSE:
        LoadStatusWindow = 0;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                RamBlockNumber   = -1;
                LoadStatusWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                       UPDATE_RAM_BLOCK_LISTBOX                       *
***********************************************************************/
void update_ram_block_listbox( HWND w, uint32 * sorc )
{
const int32   NUMBER_LEN = 5;
const int32   LINE_LEN = 11 * (NUMBER_LEN + 3);
LISTBOX_CLASS lb;
TCHAR         buf[LINE_LEN+1];
TCHAR       * cp;
int32         i;
int32         j;
uint32      * sp;

lb.init( w, BLOCK_RAM_LISTBOX );
lb.redraw_off();
lb.empty();
sp = sorc;
for ( i=0; i<200; i++ )
    {
    fillchars( buf, SpaceChar, LINE_LEN );
    cp = buf;
    insalph( cp, i*10, 4, SpaceChar, DECIMAL_RADIX );
    cp += 4;
    for ( j=0; j<10; j++ )
        {
        cp += 1;
        insalph( cp, *sp, NUMBER_LEN, SpaceChar, DECIMAL_RADIX );
        sp++;
        cp += NUMBER_LEN;
        }
    *cp = NullChar;
    lb.add( buf );
    }

if ( LoadStatusWindow )
    PostMessage( LoadStatusWindow,  WM_CLOSE, 0, 0L );

lb.redraw_on();
delete[] sorc;
}

/***********************************************************************
*                      BLOCK_MODE_RAM_DIALOG_PROC                      *
*                            Modal Dialog                              *
***********************************************************************/
BOOL CALLBACK block_mode_ram_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        SetFocus( hWnd );
        RamBlockWindow = hWnd;
        set_checkbox( hWnd, LOW_RAM_BUTTON, TRUE );
        return TRUE;

    case WM_NEW_DATA:
        update_ram_block_listbox( hWnd, (uint32 *) lParam );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                RamBlockWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;

            case BLOCK_9000_BUTTON:
            case BLOCK_A000_BUTTON:
            case BLOCK_B000_BUTTON:
            case BLOCK_C000_BUTTON:
            case BLOCK_D000_BUTTON:
            case BLOCK_E000_BUTTON:
            case BLOCK_F000_BUTTON:
                RamBlockNumber = 1 + id - BLOCK_9000_BUTTON;
                DialogBox(
                    MainInstance,
                    TEXT("LOAD_STATUS_DIALOG"),
                    hWnd,
                    (DLGPROC) load_status_dialog_proc );
                return TRUE;
            }
        break;
    }

return FALSE;
}


/***********************************************************************
*                      POSITION_CURRENT_ANALOG_WINDOW                  *
***********************************************************************/
void position_current_analog_window( void )
{
RECT r;
long x;
long y;
long height;
long width;
WINDOW_CLASS wc;

if ( GetWindowRect( CurrentAnalogWindow, &r) )
    {
    height = r.bottom - r.top;
    width  = r.right  - r.left;
    if ( GetWindowRect(MainWindow.handle(), &r) )
        {
        x = r.right - width;
        y = 0;
        wc = CurrentAnalogWindow;
        wc.move( x, y, width, height );
        }
    }

}

/***********************************************************************
*                    CURRENT_VARIABLES_DIALOG_PROC                     *
*                            Modal Dialog                              *
***********************************************************************/
BOOL CALLBACK current_variables_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
const UINT TBOX[MAX_ST_CHANNELS] = { CURRENT_NULL_VOLTAGE_TEXTBOX, CURRENT_ANALOG_2_TEXTBOX, CURRENT_ANALOG_3_TEXTBOX, CURRENT_ANALOG_4_TEXTBOX };
int   id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        CurrentAnalogWindow = hWnd;
        position_current_analog_window();
        set_text( hWnd, VELOCITY_UNITS_TEXTBOX, units_name(VelocityUnits) );
        SetFocus( hWnd );
        return TRUE;

    case WM_NEW_DATA:
        int i;
        double y;
        for ( i=0; i<MAX_ST_CHANNELS; i++ )
            {
            y = (double) (short) SureTrakAnalogValue[i];
            y *= 5.0;
            y /= 2047.0;
            set_text( hWnd, TBOX[i], fixed_string(y, LeftAnalogPlaces, RightAnalogPlaces) );
            }

        y = 0.0;
        if ( SureTrakVelocity != 0 && SureTrakVelocity != 0xFFFF )
            y = PosSensorVelocityDividend / (double) (short) SureTrakVelocity;
        set_text( hWnd, CURRENT_VELOCITY_TEXTBOX, fixed_string(y, LeftVelPlaces, RightVelPlaces) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                CurrentAnalogWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                     SAVE_EDIT_CONTROL_TO_FILE                        *
***********************************************************************/
static void save_edit_control_to_file( TCHAR * file_name, HWND hWnd, UINT id )
{

HANDLE  fh;
TCHAR * s;
DWORD   slen;
DWORD   bytes_to_write;
DWORD   bytes_written;
HWND    w;

const DWORD access_mode  = GENERIC_WRITE;
const DWORD share_mode   = 0;
const DWORD create_flags = CREATE_ALWAYS;

w = GetDlgItem(hWnd, id);
slen = GetWindowTextLength( w );
s    = maketext( slen+1 );
if ( s )
    {
    if ( get_text(s, w, slen) )
        {
        fh = CreateFile( file_name, access_mode, share_mode, NULL, create_flags, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
        if ( fh != INVALID_HANDLE_VALUE )
            {
            *(s+slen) = NullChar;
            slen = lstrlen( s );
            bytes_to_write = slen * sizeof(TCHAR);
            WriteFile( fh, s, bytes_to_write, &bytes_written, 0 );
            delete[] s;
            CloseHandle( fh );
            }
        }
    }
}

/***********************************************************************
*                     LOAD_EDIT_CONTROL_FROM_FILE                      *
***********************************************************************/
static void load_edit_control_from_file( HWND w, UINT id, TCHAR * file_name )
{

HANDLE  fh;
TCHAR * s;
DWORD   slen;
DWORD   bytes_to_read;
DWORD   bytes_read;
BOOL    status;

const DWORD access_mode  = GENERIC_READ;
const DWORD share_mode   = FILE_SHARE_READ | FILE_SHARE_WRITE;
const DWORD create_flags = OPEN_EXISTING;

fh = CreateFile( file_name, access_mode, share_mode, NULL, create_flags, FILE_FLAG_SEQUENTIAL_SCAN, 0 );

if ( fh != INVALID_HANDLE_VALUE )
    {
    bytes_to_read = GetFileSize( fh, 0 );
    slen = bytes_to_read/sizeof(TCHAR);

    s = maketext( slen+1 );
    if ( s )
        {
        status = ReadFile( fh, s, bytes_to_read, &bytes_read, 0 );
        if ( status && bytes_read > 0 )
            {
            *(s+slen) = NullChar;
            set_text( w, id, s );
            }
        delete[] s;
        }
    CloseHandle( fh );
    }
}

/***********************************************************************
*                     LOAD_OTHER_CONTROL_PROGRAM                       *
***********************************************************************/
void load_other_control_file( HWND w )
{
static TCHAR Filter[] = TEXT( "Control Programs\0*.PRG\0" );

OPENFILENAME fh;
TCHAR   dir_name[MAX_PATH+1];
TCHAR   file_name[MAX_PATH+1];
TCHAR * cp;

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow.handle();
fh.hInstance         = 0;
fh.lpstrFilter       = Filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = file_name;
fh.nMaxFile          = MAX_PATH;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = dir_name;
fh.lpstrTitle        = resource_string( CHOOSE_CONTROL_PROGRAM_STRING );
fh.Flags             = OFN_EXPLORER;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*file_name = NullChar;

lstrcpy( dir_name, computer_directory_name(MyComputer.whoami()) );
remove_backslash_from_path( dir_name );

if ( GetOpenFileName(&fh) )
    {
    load_edit_control_from_file( w, CONTROL_PROGRAM_EBOX, file_name );
    cp = file_name + lstrlen(file_name) - 3;
    lstrcpy( cp, MESSAGE_FILE_SUFFIX );
    if ( file_exists(file_name) )
        load_edit_control_from_file( w, CONTROL_MESSAGE_EBOX, file_name );
    }

}
/***********************************************************************
*                         SAVE_CONTROL_PROGRAM                         *
***********************************************************************/
static void save_control_program( HWND w )
{
save_edit_control_to_file( control_program_name(MyComputer.whoami()), w, CONTROL_PROGRAM_EBOX );
save_edit_control_to_file( control_message_name(MyComputer.whoami()), w, CONTROL_MESSAGE_EBOX );
SureTrakBlockMessage.load( control_message_name(MyComputer.whoami()) );
}

/***********************************************************************
*                         SAVE_CONTROL_PROGRAM                         *
***********************************************************************/
static void save_control_program( HWND w, TCHAR * dest_file_name )
{
TCHAR * cp;
TCHAR   fname[MAX_PATH+1];
int32   slen;

lstrcpy( fname, dest_file_name );
save_edit_control_to_file( fname, w, CONTROL_PROGRAM_EBOX );

slen = lstrlen( fname );
if ( slen < 4 )
    return;
cp = fname + slen - 3;
lstrcpy( cp, MESSAGE_FILE_SUFFIX );
save_edit_control_to_file( fname, w, CONTROL_MESSAGE_EBOX );
}

/***********************************************************************
*                     SAVE_OTHER_CONTROL_PROGRAM                       *
***********************************************************************/
void save_other_control_file( HWND w )
{
static TCHAR Filter[] = TEXT( "Control Programs\0*.PRG\0" );

OPENFILENAME fh;
TCHAR   dir_name[MAX_PATH+1];
TCHAR   file_name[MAX_PATH+1];

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow.handle();
fh.hInstance         = 0;
fh.lpstrFilter       = Filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = file_name;
fh.nMaxFile          = MAX_PATH;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = dir_name;
fh.lpstrTitle        = resource_string( SAVE_CONTROL_PROGRAM_STRING );
fh.Flags             = OFN_EXPLORER;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*file_name = NullChar;

lstrcpy( dir_name, computer_directory_name(MyComputer.whoami()) );
remove_backslash_from_path( dir_name );

if ( GetSaveFileName(&fh) )
    save_control_program( w, file_name );
}

/***********************************************************************
*                         LOAD_CONTROL_PROGRAM                         *
***********************************************************************/
static void load_control_program( HWND w )
{
load_edit_control_from_file( w, CONTROL_PROGRAM_EBOX, control_program_name(MyComputer.whoami()) );
load_edit_control_from_file( w, CONTROL_MESSAGE_EBOX, control_message_name(MyComputer.whoami()) );
}

/***********************************************************************
*                      CONTROL_PROGRAM_DIALOG_PROC                     *
*                            Modal Dialog                              *
_**********************************************************************/
BOOL CALLBACK control_program_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        load_control_program( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CP_OPEN_OTHER_MENU:
                load_other_control_file( hWnd );
                return TRUE;

            case CP_SAVE_AS_MENU:
                save_other_control_file( hWnd );
                return TRUE;

            case SAVE_CONTROL_PROGRAM_BUTTON:
                save_control_program( hWnd );
                send_command_to_board( PROGRAM_DOWNLOAD_REQUEST );
                PostMessage( ServiceToolsWindow, WM_DBCLOSE, 0, 0L );

            case CP_EXIT_MENU:
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        EPROM_NUMBER_DIALOG_PROC                      *
*                            Modal Dialog                              *
***********************************************************************/
BOOL CALLBACK eprom_number_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        PromNumberWindow = GetDlgItem( hWnd, EPROM_NUMBER_TBOX );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        POSITION_SERVICE_TOOLS                        *
***********************************************************************/
static void position_service_tools( HWND w )
{
RECT r;
long x;
long y;
long height;
long width;
WINDOW_CLASS wc;

if ( GetWindowRect( w, &r) )
    {
    height = r.bottom - r.top;
    width  = r.right  - r.left;
    if ( GetWindowRect(MainWindow.handle(), &r) )
        {
        x = r.right - width;
        y = r.bottom - height;
        wc = w;
        wc.move( x, y, width, height );
        }
    }

}

/***********************************************************************
*                   SET_CURRENT_PARAMETERS_TAB_STOPS                   *
***********************************************************************/
static void set_current_parameters_tab_stops( void )
{
int tabs[1];
int x;

x = LOWORD( GetDialogBaseUnits() );
x /= 2;

tabs[0] = 22 * x;

SendDlgItemMessage( CurrentParametersWindow, CURRENT_PARAMETER_VALUE_LISTBOX, LB_SETTABSTOPS, (WPARAM) 1, (LPARAM) tabs );
}

/***********************************************************************
*                   CURRENT_PARAMETERS_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK current_parameters_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentParametersWindow = w;
        set_current_parameters_tab_stops();
        send_command_to_board( READ_CONTROL_PARAMETERS_REQUEST );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                CurrentParametersWindow = 0;
                EndDialog( w, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                             SKIP_WHITESPACE                          *
***********************************************************************/
static TCHAR * skip_whitespace( TCHAR * s )
{
while ( *s == SpaceChar || *s == TabChar )
    s++;
return s;
}

/***********************************************************************
*                             STEP_NUMBER                              *
***********************************************************************/
static int32 step_number( TCHAR * s )
{
if ( *s != PeriodChar )
    return NO_STEP_NUMBER;

s++;
return asctoint32( s );
}

/***********************************************************************
*                          ASCII_STEP_NUMBER                           *
*                              ".042 "                                 *
***********************************************************************/
static TCHAR * ascii_step_number( int32 step_number )
{
static TCHAR buf[1+ST_STEP_NUMBER_LEN+2];
TCHAR * s;

s = buf;

*s = PeriodChar;
s++;
insalph( s, step_number, ST_STEP_NUMBER_LEN, ZeroChar, DECIMAL_RADIX );
s += ST_STEP_NUMBER_LEN;

*s = SpaceChar;
s++;

*s = NullChar;
return buf;
}

/***********************************************************************
*                         COPY_VALUE_TO_EBOX                           *
***********************************************************************/
void copy_value_to_ebox( HWND w, UINT id, TCHAR * s )
{
TCHAR * bp;
TCHAR   buf[MAX_FLOAT_LEN+1];

bp = buf;
while ( *s != NullChar && *s != SpaceChar && !IsCharAlpha(*s) )
    {
    *bp = *s;
    bp++;
    s++;
    }

*bp = NullChar;

set_text( w, id, buf );
}

/***********************************************************************
*                          PARSE_ALTERNATE_STEP                        *
*                         .044 V4AC5P.2 E1.2                           *
***********************************************************************/
void parse_alternate_step( HWND w )
{
TCHAR * p;
TCHAR * cp;

cp  = findchar( VChar, AlternateFirstStep );
if ( cp )
    {
    cp++;
    copy_value_to_ebox( w, START_VEL_EBOX, cp );
    }

cp  = findchar( CChar, AlternateFirstStep );
if ( cp )
    {
    cp++;
    copy_value_to_ebox( w, START_ACCEL_EBOX, cp );
    }

/*
-------------------------------------------------------
Point P to the global where I store the end of the step
------------------------------------------------------- */
p = AlternateLineEnd;
cp  = findchar( PChar, AlternateFirstStep );
if ( cp )
    {
    cp++;
    copy_value_to_ebox( w, START_POS_EBOX, cp );

    /*
    ----------------------------------------------------------
    Make a copy of the end of the line to be used to construct
    the alternate step.
    ---------------------------------------------------------- */
    cp  = findchar( SpaceChar, cp );
    if ( cp )
        {
        while ( TRUE )
            {
            *p = *cp;
            p++;
            cp++;
            if ( *cp == NullChar || *cp == SpaceChar )
                break;
            }
        }
    }

*p = NullChar;
}

/***********************************************************************
*                           VOLTS_FROM_STEP                            *
***********************************************************************/
static double volts_from_step( TCHAR * sorc )
{
double  x;

x = 0.0;

/*
-----------------------------------
Convert [0,16383] to [-10,10] volts
----------------------------------- */
sorc  = findchar( TTChar, sorc );
if ( sorc )
    {
    sorc++;
    x = (double) asctoint32( sorc );
    x += 1;
    x *= 0.001220763;
    x -= 0.000732434;
    x *= 100;
    x += 0.5;
    x = (double) floor( x );
    x /= 100;
    x -= 10.0;
    }

return x;
}

/***********************************************************************
*                          COUNTS_FROM_VOLTS                           *
***********************************************************************/
static int counts_from_volts( TCHAR * sorc )
{
double x;

/*
-----------------------------------
Convert [-10,10] volts to [0,16383]
----------------------------------- */
x = extdouble( sorc );
x += 10.0;
x *= 16383.0;
x /= 20.0;
x += 0.5;

return (int) x;
}

/***********************************************************************
*                          PARSE_RETRACT_STEP                          *
*                         .004 T4096 N                                 *
***********************************************************************/
void parse_retract_step( HWND w, TCHAR * sorc )
{
double  x;

*RetractLineEnd = NullChar;

x = volts_from_step( sorc );

/*
---------------------------------------------
Save the end of the line so I can put it back
--------------------------------------------- */
sorc  = findchar( TTChar, sorc );
if ( sorc )
    sorc  = findchar( SpaceChar, sorc );
if ( sorc )
    lstrcpy( RetractLineEnd, sorc );

ProgramIsUpdating = TRUE;
copy_value_to_ebox( w, RETR_VOLTS_EBOX, ascii_double(x) );
ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                          LOAD_RETRACT_STEP                           *
***********************************************************************/
static BOOLEAN load_retract_step( HWND w )
{
BOOLEAN    have_step;
TCHAR    * cp;
FILE_CLASS f;

have_step = FALSE;

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, RetractStepName );
if ( !unknown(cp) )
    RetractStepNumber = asctoint32( cp );

if ( f.open_for_read(control_program_name(MyComputer.whoami())) )
    {
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;
        if ( step_number(cp) == RetractStepNumber )
            {
            parse_retract_step( w, cp );
            set_text( w, RETR_STEP_TBOX, cp );
            have_step = TRUE;
            }
        }

    f.close();
    }

return have_step;
}

/***********************************************************************
*                             IS_FLOAT_CHAR                            *
***********************************************************************/
static BOOLEAN is_float_char( TCHAR c )
{
if ( is_numeric(c) )
    return TRUE;

if ( c == PeriodChar )
    return TRUE;

if ( c == MinusChar )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                       IS_NORMAL_VALVE_START_STEP                     *
*                                                                      *
*      Return TRUE if this is a valve start step like T-4LD0.25 N      *
*                                                                      *
***********************************************************************/
static BOOLEAN is_normal_valve_start_step( TCHAR * s )
{
static TCHAR ld[] = TEXT( "LD" );

s = findchar( TTChar, s );
if ( s )
    s = findstring( ld, s );

if ( s )
    s = findchar( NChar, s );

if ( s )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                      IS_ALTERNATE_VALVE_START_STEP                   *
*                                                                      *
*      Return TRUE if this is a valve start step like T6800 N          *
*                                                                      *
***********************************************************************/
static BOOLEAN is_alternate_valve_start_step( TCHAR * s )
{
static TCHAR ld[] = TEXT( "LD" );

s = findchar( TTChar, s );

if ( s )
    {
    if ( findstring(ld, s) )
        return FALSE;
    s = findchar( NChar, s );
    }

if ( s )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                       UPDATE_VALVE_START_TBOX                        *
***********************************************************************/
static void update_valve_start_tbox( HWND w )
{
static TCHAR tstring[] = TEXT( "T" );
static TCHAR spacen[]  = TEXT( " N" );

int32   i;
TCHAR   s[80];
TCHAR * cp;
TCHAR * sp; /* Points to the step w/o the step number */

if ( ProgramIsUpdating )
    return;

if ( !get_text(s, w, VALVE_START_VOLTAGE_EBOX, MAX_FLOAT_LEN) )
    copystring( s, ZeroVoltsString );

i = counts_from_volts( s );

/*
--------------------------------
Make new step and put in textbox
-------------------------------- */
sp = copystring( s, ascii_step_number(ValveStartStepNumber) );
cp = copystring( sp, tstring );
cp = copystring( cp, int32toasc(i) );
copystring( cp, spacen );
set_text( w, VALVE_START_STEP_TBOX, s );
copystring( AlternateValveStartStep, sp );
}

/***********************************************************************
*                  PARSE_ALTERNATE_VALVE_START_STEP                    *
***********************************************************************/
void parse_alternate_valve_start_step( HWND w )
{
double  x;
x = volts_from_step( AlternateValveStartStep );

ProgramIsUpdating = TRUE;
copy_value_to_ebox( w, VALVE_START_VOLTAGE_EBOX, ascii_double(x) );
ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                        LOAD_VALVE_START_STEP                         *
***********************************************************************/
static BOOLEAN load_valve_start_step( HWND w )
{
BOOLEAN    have_step;
int        cmd;
UINT       id;
TCHAR      s[MAX_STEP_LINE_LEN+1];
TCHAR    * cp;
TCHAR    * cp2;
TCHAR    * cp3;
FILE_CLASS f;

have_step = FALSE;
if ( f.open_for_read(control_program_name(MyComputer.whoami())) )
    {
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;

        if ( step_number(cp) == ValveStartStepNumber )
            {
            lstrcpyn( s, cp, MAX_STEP_LINE_LEN+1 );
            cp  = findchar( SpaceChar,     cp );
            cp2 = findchar( SemiColonChar, cp );

            if ( cp2 )
                {
                *cp2 = NullChar;
                cp2++;

                cp3 = findstring( CommentIntroducer, cp2 );

                if ( cp3 )
                    {
                    *cp3 = NullChar;
                    cp3 += lstrlen( CommentIntroducer );
                    if ( *cp3 != NullChar )
                        lstrcpy( ValveStartComment, cp3 );
                    }

                /*
                ------------------------------------------------
                See if there is a valid step after the semicolon
                ------------------------------------------------ */
                cp2 = skip_whitespace( cp2 );
                if ( *cp2 != TTChar || ( !is_normal_valve_start_step(cp2) && !is_alternate_valve_start_step(cp2)) )
                    {
                    /*
                    -----------------------------------------------------------------
                    This step has probably never been edited before, copy the comment
                    ----------------------------------------------------------------- */
                    if ( !cp3 )
                        {
                        if ( lstrlen(cp2) > 1 )
                            lstrcpy( ValveStartComment, cp2 );
                        else
                            lstrcpy( ValveStartComment, DefaultValveStartComment );
                        }
                    cp2 = 0;
                    }
                }

            if ( cp )
                {
                cp = skip_whitespace( cp );
                if ( is_normal_valve_start_step(cp) )
                    {
                    lstrcpy( NormalValveStartStep, cp );
                    if ( !cp2 )
                        cp2 = DefaultAlternateValveStartStep;
                    lstrcpy( AlternateValveStartStep, cp2 );
                    id = NORMAL_VALVE_START_RADIO;
                    have_step = TRUE;
                    }
                else if ( is_alternate_valve_start_step(cp) )
                    {
                    lstrcpy( AlternateValveStartStep, cp );
                    if ( !cp2 )
                        cp2 = DefaultValveStartStep;
                    lstrcpy( NormalValveStartStep, cp2 );
                    id = ALT_VALVE_START_RADIO;
                    have_step = TRUE;
                    }
                else
                    {
                    set_text( w, START_STEP_TBOX, s );
                    }

                if ( have_step )
                    {
                    trim( NormalValveStartStep );
                    trim( AlternateValveStartStep );
                    parse_alternate_valve_start_step( w );
                    CheckRadioButton( w, NORMAL_VALVE_START_RADIO, ALT_VALVE_START_RADIO, id );

                    cp = copystring( s, ascii_step_number(ValveStartStepNumber) );
                    if ( id == NORMAL_VALVE_START_RADIO )
                        {
                        cmd = SW_HIDE;
                        copystring( cp, NormalValveStartStep );
                        }
                    else
                        {
                        cmd = SW_SHOW;
                        copystring( cp, AlternateValveStartStep );
                        }

                    set_text( w, VALVE_START_STEP_TBOX, s );
                    ShowWindow( GetDlgItem(w, VALVE_START_VOLTAGE_TBOX), cmd );
                    ShowWindow( GetDlgItem(w, VALVE_START_VOLTAGE_EBOX), cmd );
                    }
                else
                    {
                    set_text( w, VALVE_START_STEP_TBOX, resource_string(NO_STEP_FOUND_STRING) );
                    }
                }

            break;
            }
        }
    f.close();
    }

if ( !have_step )
    {
    *NormalValveStartStep    = NullChar;
    *AlternateValveStartStep = NullChar;

    CheckDlgButton( w, NORMAL_VALVE_START_RADIO, BST_UNCHECKED );
    CheckDlgButton( w, ALT_VALVE_START_RADIO,    BST_UNCHECKED );
    }

return have_step;
}


/***********************************************************************
*                         IS_ALTERNATE_FIRST_STEP                      *
*                                                                      *
*          Return TRUE if this is a first step like V4AC5P2 E1.2       *
*                                                                      *
***********************************************************************/
static BOOLEAN is_alternate_first_step( TCHAR * s )
{
const static TCHAR ac[] = TEXT( "AC" );

s = findchar( VChar, s );
if ( s )
    {
    s++;
    if ( !is_float_char(*s) )
        s = 0;
    }

if ( s )
    s = findstring( ac, s );

if ( s )
    {
    s += lstrlen( ac );
    if ( !is_float_char(*s) )
        s = 0;
    }

if ( s )
    s = findchar( PChar, s );

if ( s )
    {
    s++;
    if ( !is_float_char(*s) )
        s = 0;
    }

if ( s )
    s = findchar( EChar, s );

if ( s )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                         IS_NORMAL_FIRST_STEP                         *
*                                                                      *
*          Return TRUE if this is a first step like T-1 B1.2           *
*                                                                      *
***********************************************************************/
static BOOLEAN is_normal_first_step( TCHAR * s )
{
s = findchar( TTChar, s );
if ( s )
    s = findchar( BChar, s );

if ( s )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           LOAD_FIRST_STEP                            *
***********************************************************************/
static BOOLEAN load_first_step( HWND w )
{
BOOLEAN    have_step;
UINT       id;
TCHAR      s[MAX_STEP_LINE_LEN+1];
TCHAR    * cp;
TCHAR    * cp2;
FILE_CLASS f;

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, InitialMoveStepName );
if ( !unknown(cp) )
    FirstStepNumber = asctoint32( cp );

have_step = FALSE;
if ( f.open_for_read(control_program_name(MyComputer.whoami())) )
    {
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;

        if ( step_number(cp) == FirstStepNumber )
            {
            lstrcpyn( s, cp, MAX_STEP_LINE_LEN+1 );
            cp  = findchar( SpaceChar,     cp );
            cp2 = findchar( SemiColonChar, cp );

            if ( cp2 )
                {
                *cp2 = NullChar;
                cp2++;
                /*
                ------------------------------------------------
                See if there is a valid step after the semicolon
                ------------------------------------------------ */
                cp2 = skip_whitespace( cp2 );
                if ( !is_normal_first_step(cp2) && !is_alternate_first_step(cp2) )
                    cp2 = 0;
                }

            if ( cp )
                {
                cp = skip_whitespace( cp );
                if ( is_normal_first_step(cp) )
                    {
                    lstrcpy( NormalFirstStep, cp );
                    if ( !cp2 )
                        cp2 = DefaultAlternateStep;
                    lstrcpy( AlternateFirstStep, cp2 );
                    id = TIME_STEP_RADIO;
                    have_step = TRUE;
                    }
                else if ( is_alternate_first_step(cp) )
                    {
                    lstrcpy( AlternateFirstStep, cp );
                    if ( !cp2 )
                        cp2 = DefaultFirstStep;
                    lstrcpy( NormalFirstStep, cp2 );
                    id = POS_STEP_RADIO;
                    have_step = TRUE;
                    }
                else
                    {
                    set_text( w, START_STEP_TBOX, s );
                    }

                if ( have_step )
                    {
                    trim( NormalFirstStep );
                    trim( AlternateFirstStep );
                    parse_alternate_step( w );
                    CheckRadioButton( w, POS_STEP_RADIO, TIME_STEP_RADIO, id );
                    }
                }

            break;
            }
        }
    f.close();
    }

if ( !have_step )
    {
    *AlternateLineEnd   = NullChar;
    *NormalFirstStep    = NullChar;
    *AlternateFirstStep = NullChar;
    CheckDlgButton( w, POS_STEP_RADIO,  BST_UNCHECKED );
    CheckDlgButton( w, TIME_STEP_RADIO, BST_UNCHECKED );
    }

return have_step;
}

/***********************************************************************
*                       UPDATE_RETRACT_STEP_TBOX                       *
***********************************************************************/
static void update_retract_step_tbox( HWND  w )
{
static TCHAR tcmd[] = TEXT( "T" );

int32   i;
TCHAR   s[80];
TCHAR * cp;

if ( ProgramIsUpdating )
    return;

if ( !get_text(s, w, RETR_VOLTS_EBOX, MAX_FLOAT_LEN) )
    copystring( s, ZeroVoltsString );

i = counts_from_volts( s );

/*
--------------------------------
Make new step and put in textbox
-------------------------------- */
cp = copystring( s, ascii_step_number(RetractStepNumber) );
cp = copystring( cp, tcmd );
cp = copystring( cp, int32toasc(i) );
copystring( cp, RetractLineEnd );
set_text( w, RETR_STEP_TBOX, s );
}

/***********************************************************************
*                       MAKE_START_OF_FIRST_STEP                       *
*                              ".044 "                                 *
*                     Return a pointer to the null char                *
***********************************************************************/
TCHAR * make_start_of_first_step( TCHAR * s )
{
const int SLEN = 1 + ST_STEP_NUMBER_LEN + 1;

copystring( s, ascii_step_number(FirstStepNumber) );
s += SLEN;

return s;
}

/***********************************************************************
*                        MAKE_FIRST_TIME_STEP                          *
*                           .044 T-1 B1.2                              *
***********************************************************************/
static void make_first_time_step( HWND w )
{
TCHAR   s[MAX_STEP_LINE_LEN+1];
TCHAR * cp;

cp = make_start_of_first_step( s );
lstrcpy( cp, NormalFirstStep );
set_text( w, START_STEP_TBOX, s );
}

/***********************************************************************
*                        MAKE_FIRST_POS_STEP                           *
*                         .044 V4AC5P.2 E1.2                           *
***********************************************************************/
static void make_first_pos_step( HWND w )
{
static TCHAR accel_cmd[] = TEXT( "AC" );
static TCHAR pos_cmd[]   = TEXT( "P" );

TCHAR   s[MAX_STEP_LINE_LEN+1];
TCHAR * cp;

cp = AlternateFirstStep;

/*
----------------
Velocity Command
---------------- */
*cp = VChar;
cp++;

if ( get_text( cp, w, START_VEL_EBOX, MAX_FLOAT_LEN) )
    cp += lstrlen(cp);

/*
-------------
Accel Command
------------- */
cp = copystring( cp, accel_cmd );

if ( get_text( cp, w, START_ACCEL_EBOX, MAX_FLOAT_LEN) )
    cp += lstrlen(cp);

/*
----------------
Position Command
---------------- */
cp = copystring( cp, pos_cmd );

if ( get_text( cp, w, START_POS_EBOX, MAX_FLOAT_LEN) )
    cp += lstrlen(cp);


copystring( cp, AlternateLineEnd );

/*
-----------
Step Number
----------- */
cp = make_start_of_first_step( s );
copystring( cp, AlternateFirstStep );

set_text( w, START_STEP_TBOX, s );
}

/***********************************************************************
*                       UPDATE_FIRST_STEP_TBOX                         *
***********************************************************************/
static void update_first_step_tbox( HWND w )
{

if ( is_checked(w, POS_STEP_RADIO) )
    {
    make_first_pos_step( w );
    }
else
    {
    make_first_time_step( w );
    }
}

/***********************************************************************
*                           DO_STEP_TYPE_RADIOS                        *
***********************************************************************/
static void do_step_type_radios( HWND parent )
{
BOOLEAN have_step;
int  cmd;
UINT id[] = {
    START_VEL_NAME_TBOX, START_ACCEL_NAME_TBOX, START_POS_NAME_TBOX,
    START_VEL_EBOX, START_ACCEL_EBOX, START_POS_EBOX,
    START_VEL_UNITS_TBOX, START_ACCEL_UNITS_TBOX, START_POS_UNITS_TBOX
    };

const int32 NOF_IDS = sizeof(id)/sizeof(UINT);
int32  i;
HWND   w;

have_step = TRUE;
if ( is_checked(parent, POS_STEP_RADIO) )
    cmd = SW_SHOW;
else
    {
    cmd = SW_HIDE;
    if ( !is_checked(parent, TIME_STEP_RADIO) )
        have_step = FALSE;
    }

for ( i=0; i<NOF_IDS; i++ )
    ShowWindow( GetDlgItem(parent, id[i]), cmd );

if ( have_step )
    {
    update_first_step_tbox( parent );
    }
else
    {
    /*
    -------------------------------------------------
    The step is not the first step, disable the radio
    buttons and show an error message
    ------------------------------------------------- */
    EnableWindow( GetDlgItem(parent, POS_STEP_RADIO),  FALSE );
    EnableWindow( GetDlgItem(parent, TIME_STEP_RADIO), FALSE );

    w = GetDlgItem( parent, START_VEL_NAME_TBOX );
    set_text( w, resource_string(INVALID_FIRST_STEP_STRING) );
    ShowWindow( w, SW_SHOW );

    w = GetDlgItem( parent, START_ACCEL_NAME_TBOX );
    set_text( w, resource_string(INVALID_FIRST_STEP_STRING2) );
    ShowWindow( w, SW_SHOW );
    }
}

/***********************************************************************
*                           DO_VALVE_START_RADIOS                      *
***********************************************************************/
static void do_valve_start_radios( HWND parent )
{
bool    have_step;
int     cmd;
int     i;
STRING_CLASS s;

UINT id[] = { VALVE_START_VOLTAGE_TBOX, VALVE_START_VOLTAGE_EBOX };
int   n = 2;

have_step = true;

s = ascii_step_number( ValveStartStepNumber );

if ( is_checked(parent, ALT_VALVE_START_RADIO) )
    {
    cmd = SW_SHOW;
    s += AlternateValveStartStep;
    }
else if ( is_checked(parent, NORMAL_VALVE_START_RADIO) )
    {
    cmd = SW_HIDE;
    s += NormalValveStartStep;
    }
else
    return;

for ( i=0; i<n; i++ )
    ShowWindow( GetDlgItem(parent, id[i]), cmd );

s.set_text( parent, VALVE_START_STEP_TBOX );
}

/***********************************************************************
*                       SAVE_CONTROL_MOD_CHANGES                       *
***********************************************************************/
BOOLEAN save_control_mod_changes( HWND w )
{
FILE_CLASS sorc;
FILE_CLASS dest;
TCHAR    * cp;
TCHAR    * cp1;
STRING_CLASS valvestartstep;
TCHAR      retractstep[MAX_STEP_LINE_LEN+1];
TCHAR      firstep[MAX_STEP_LINE_LEN+1];
TCHAR      midline[] = TEXT( "  ;" );
BOOLEAN    have_valve_start;
BOOLEAN    have_tempfile;
TCHAR    * control_file_name;
TCHAR    * temp_file_name;
int32      this_step;
BOOLEAN    have_first_step;

have_valve_start = TRUE;
valvestartstep = TEXT( ".042 " );

if ( is_checked(w, ALT_VALVE_START_RADIO) )
    {
    cp  = AlternateValveStartStep;
    cp1 = NormalValveStartStep;
    }
else if ( is_checked(w, NORMAL_VALVE_START_RADIO) )
    {
    cp  = NormalValveStartStep;
    cp1 = AlternateValveStartStep;
    }
else
    have_valve_start = FALSE;

if ( have_valve_start )
    {
    valvestartstep += cp;
    valvestartstep += CommentDelimiter;
    valvestartstep += cp1;
    valvestartstep += CommentIntroducer;
    valvestartstep += ValveStartComment;
    }

/*
------------------------------------
Save the changes to the retract step
------------------------------------ */
get_text( retractstep, w, RETR_STEP_TBOX, MAX_STEP_LINE_LEN );

if ( IsWindowEnabled(GetDlgItem(w, POS_STEP_RADIO)) )
    have_first_step = TRUE;
else
    have_first_step = FALSE;

if ( have_first_step )
{
    /*
    ----------------------------------
    Save the changes to the first step
    ---------------------------------- */
    get_text( firstep, w, START_STEP_TBOX, MAX_STEP_LINE_LEN );
    lstrcat( firstep, midline );

    /*
    -----------------------------------
    Copy the other setup as the comment
    ----------------------------------- */
    if ( is_checked(w, POS_STEP_RADIO) )
        cp = NormalFirstStep;
    else
        cp = AlternateFirstStep;

    lstrcat( firstep, cp );
    }

have_tempfile = FALSE;

temp_file_name    = maketext( temp_control_program_name(MyComputer.whoami()) );
control_file_name = maketext( control_program_name(MyComputer.whoami())      );

if ( temp_file_name && control_file_name )
    {
    if ( dest.open_for_write(temp_file_name) )
        {
        if ( sorc.open_for_read(control_file_name) )
            {
            while ( TRUE )
                {
                cp = sorc.readline();
                if ( !cp )
                    {
                    have_tempfile = TRUE;
                    break;
                    }

                this_step = step_number( cp );
                if ( this_step == FirstStepNumber )
                    {
                    if ( have_first_step )
                        cp = firstep;
                    }
                else if ( this_step == RetractStepNumber )
                    {
                    cp = retractstep;
                    }
                else if ( this_step == ValveStartStepNumber )
                    {
                    cp = valvestartstep.text();
                    }
                dest.writeline( cp );
                }
            sorc.close();
            }
        dest.close();
        }
    }

if ( have_tempfile )
    {
    DeleteFile( control_file_name );
    MoveFile( temp_file_name, control_file_name );
    }

if ( temp_file_name )
    delete[] temp_file_name;

if ( control_file_name )
    delete[] control_file_name;

return have_tempfile;
}

/***********************************************************************
*                         CONTROL_MODS_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK control_mods_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOOLEAN program_mods_in_progress = FALSE;
int   id;
int   cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        program_mods_in_progress = TRUE;
        set_text( w, START_POS_UNITS_TBOX, units_name(DistanceUnits) );
        set_text( w, START_VEL_UNITS_TBOX, units_name(VelocityUnits) );
        load_first_step( w );
        do_step_type_radios( w );
        load_retract_step( w );
        load_valve_start_step( w );
        program_mods_in_progress = FALSE;
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case POS_STEP_RADIO:
            case TIME_STEP_RADIO:
                if ( !program_mods_in_progress )
                    do_step_type_radios( w );
                return TRUE;

            case NORMAL_VALVE_START_RADIO:
            case ALT_VALVE_START_RADIO:
                if ( !program_mods_in_progress )
                    do_valve_start_radios( w );
                return TRUE;

            case VALVE_START_VOLTAGE_EBOX:
                if ( cmd == EN_CHANGE && !program_mods_in_progress )
                    update_valve_start_tbox( w );
                return TRUE;

            case START_VEL_EBOX:
            case START_ACCEL_EBOX:
            case START_POS_EBOX:
                if ( cmd == EN_CHANGE )
                     update_first_step_tbox( w );
                return TRUE;

            case RETR_VOLTS_EBOX:
                if ( cmd == EN_CHANGE && !program_mods_in_progress )
                    update_retract_step_tbox( w );
                return TRUE;

            case SAVE_CONTROL_MODS_BUTTON:
                save_control_mod_changes(w);
                send_command_to_board( PROGRAM_DOWNLOAD_REQUEST );
                PostMessage( ServiceToolsWindow, WM_DBCLOSE, 0, 0L );

            case IDCANCEL:
                EndDialog( w, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         CLOSE_SERVICE_TOOLS                          *
***********************************************************************/
static void close_service_tools( void )
{
HWND w;

if ( ServiceToolsWindow )
    {
    w = ServiceToolsWindow;
    NeedMonitorUpdate = TRUE;
    MainScreenWindow.post( WM_STATUS_CHANGE );
    ServiceToolsWindow = 0;
    EndDialog( w, 0 );
    }
}

/***********************************************************************
*                       ENABLE_EDIT_CONTROL_BUTTON                     *
***********************************************************************/
static void enable_edit_control_button( HWND w )
{
TCHAR * cp;
int32   level_needed;
BOOL    is_enabled;

cp = get_ini_string( VisiTrakIniFile, ButtonLevelsSection, TEXT("EditControlButton") );
if ( !unknown(cp) )
    {
    level_needed = asctoint32( cp );
    if ( CurrentPasswordLevel >= level_needed )
        EnableWindow( GetDlgItem(w,EDIT_CONTROL_PGM_BUTTON), TRUE );
    }

if ( CurrentPasswordLevel >= ServiceToolsGrayLevel )
    is_enabled = TRUE;
else
    is_enabled = FALSE;

EnableWindow( GetDlgItem(w,OPTO_PORT_TEST_BUTTON), is_enabled );
}

/***********************************************************************
*                        SERVICE_TOOLS_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK service_tools_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
BOOLEAN ok_to_test;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ServiceToolsWindow = w;
        NeedMonitorUpdate = FALSE;
        enable_edit_control_button( w );
        position_service_tools( w );
        return TRUE;

    case WM_DBCLOSE:
        close_service_tools();
        return TRUE;

    case WM_HELP:
        gethelp( HELP_CONTEXT, SURETRAK_SERVICE_TOOLS_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case READ_OR_SET_VARIABLES_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("VARIABLES_READ_AND_SET_DIALOG"),
                    w,
                    (DLGPROC) variables_dialog_proc );
                return TRUE;

            case EDIT_CONTROL_PGM_BUTTON:
                    DialogBox(
                        MainInstance,
                        TEXT("CONTROL_PROGRAM_DIALOG"),
                        w,
                        (DLGPROC) control_program_dialog_proc );

                return TRUE;

            case CURRENT_PARAMETERS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("CURRENT_PARAMETERS_DIALOG"),
                    w,
                    (DLGPROC) current_parameters_dialog_proc );
                return TRUE;

            case CONTROL_MODS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("CONTROL_MODS_DIALOG"),
                    w,
                    (DLGPROC) control_mods_dialog_proc );
                return TRUE;

            case SEND_CLEAR_COMMAND_WORD_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("COMMAND_PORT_DIALOG"),
                    w,
                    (DLGPROC) command_port_dialog_proc );
                return TRUE;

            case SEND_DAC_WORD_BUTTON:
                if ( resource_message_box( MainInstance, IS_THIS_OK_STRING, WILL_TURN_OFF_CONTROL_STRING, MB_YESNO | MB_SYSTEMMODAL) == IDYES )
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("DAC_PORT_DIALOG"),
                        w,
                        (DLGPROC) dac_port_dialog_proc );
                    }
                return TRUE;

            case SET_CONTROL_ACTIVE_BUTTON:
                set_text( w, ACTION_STATUS_TEXTBOX, resource_string(CONTROL_ACTIVATED_STRING) );
                send_command_to_board( ACTIVATE_CONTROL_REQUEST );
                return TRUE;

            case CLEAR_CONTROL_MODE_BUTTON:
                set_text( w, ACTION_STATUS_TEXTBOX, resource_string(CONTROL_DEACTIVATED_STRING) );
                send_command_to_board( DEACTIVATE_CONTROL_REQUEST );
                return TRUE;

            case GET_PROM_VERSION_NUMBER_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("EPROM_NUMBER_DIALOG"),
                    w,
                    (DLGPROC) eprom_number_dialog_proc );
                return TRUE;

            case RAM_BLOCK_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("BLOCK_MODE_RAM_DIALOG"),
                    w,
                    (DLGPROC) block_mode_ram_dialog_proc );
                return TRUE;

            case CLEAR_ERROR_BUTTON:
                send_request_plus_data( SEND_COMMAND_BUSY_REQUEST, CCLR_ERR );
                return TRUE;

            case RAM_TEST_BUTTON:
                ok_to_test = TRUE;
                if ( SureTrakStatus & CVIS_CONTROL_ACTIVE )
                    {
                    if ( resource_message_box( MainInstance, IS_THIS_OK_STRING, WILL_TURN_OFF_CONTROL_STRING, MB_YESNO | MB_SYSTEMMODAL) == IDYES )
                        send_command_to_board( DEACTIVATE_CONTROL_REQUEST );
                    else
                        ok_to_test = FALSE;
                    }
                if ( ok_to_test )
                    {
                    set_text( w, ACTION_STATUS_TEXTBOX, resource_string(TESTING_RAM_STRING) );
                    send_command_to_board( RAM_TEST_REQUEST );
                    }
                return TRUE;

            case OPTO_PORT_TEST_BUTTON:
                set_text( w, ACTION_STATUS_TEXTBOX, resource_string(TESTING_OPTO_PORT_STRING) );
                send_command_to_board( OPTO_PORT_TEST_REQUEST );
                return TRUE;

            case IO_PORT_TEST_BUTTON:
                ok_to_test = TRUE;
                if ( SureTrakStatus & CVIS_CONTROL_ACTIVE )
                    {
                    if ( resource_message_box( MainInstance, IS_THIS_OK_STRING, WILL_TURN_OFF_CONTROL_STRING, MB_YESNO | MB_SYSTEMMODAL) != IDYES )
                        ok_to_test = FALSE;
                    }

                if ( ok_to_test )
                    {
                    set_text( w, ACTION_STATUS_TEXTBOX, resource_string(TESTING_IO_PORT_STRING) );
                    send_command_to_board( IO_PORT_TEST_REQUEST );
                    }
                return TRUE;

            case IDCANCEL:
                close_service_tools();
                return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                             SHOW_CONTROL                             *
***********************************************************************/
void show_control( HWND w, UINT id, BOARD_DATA bits, BOARD_DATA changed_bit )
{
int cmd;

if ( changed_bit == 0 )
    return;

cmd = SW_HIDE;

if ( bits )
    cmd = SW_SHOW;

w = GetDlgItem( w, id );
if ( w )
    ShowWindow( w, cmd );

}

/***********************************************************************
*                             SHOW_RADIO                               *
*    These used to be radio buttons but are now text buttons.          *
***********************************************************************/
static void show_radio( HWND w, UINT id, BOARD_DATA bits, BOARD_DATA changed_bit )
{
BOOLEAN state;

if ( changed_bit == 0 )
    return;

if ( bits != 0 )
    state = TRUE;
else
    state = FALSE;

//set_checkbox( w, id, state );

w = GetDlgItem( w, id );
EnableWindow( w, state );
}

/***********************************************************************
*                         SHOW_PROGRAM_STATUS                          *
***********************************************************************/
static void show_program_status( void )
{
UINT       id;

id = NO_PROGRAM_STATUS_STRING;

if ( SureTrakHasProgram )
    {
    if ( SureTrakStatus & CVIS_CONTROL_ACTIVE )
        id = RUNNING_STATUS_STRING;
    else
        id = NOT_RUNNING_STATUS_STRING;
    }

set_text( MainScreenWindow.handle(), MAIN_PROGRAM_STATUS_TEXTBOX, resource_string(id) );
}

/***********************************************************************
*                        REFRESH_STATUS_DISPLAY                        *
***********************************************************************/
void refresh_status_display( void )
{
static BOARD_DATA old_pos          = 0;
static BOARD_DATA old_status       = 0;
static BOARD_DATA old_dac_word     = 0;
static BOARD_DATA old_input_word   = 0;
static BOARD_DATA old_output_word  = 0;
static BOARD_DATA old_block_number = 0;
static BOOLEAN    old_has_program  = FALSE;
static BOOLEAN first_time          = TRUE;

double     fx;
BOARD_DATA changed_bits;
BOARD_DATA x;
HWND       w;
BOOLEAN    program_status_has_changed;

program_status_has_changed = FALSE;

w = MainScreenWindow.handle();

if ( first_time || SureTrakHasProgram != old_has_program )
    {
    program_status_has_changed = TRUE;
    show_program_status();
    }

changed_bits = 0xFFFF;

if ( !first_time )
    changed_bits = old_status ^ SureTrakStatus;

show_control( w, BUSY_STATUS_TEXTBOX,             SureTrakStatus & VIS_BUSY,               changed_bits & VIS_BUSY              );

if ( changed_bits & CVIS_CONTROL_ACTIVE )
    {
    x = SureTrakStatus & CVIS_CONTROL_ACTIVE;
    show_control( w, CONTROL_ACTIVE_TEXTBOX, x, changed_bits & CVIS_CONTROL_ACTIVE   );

    x ^= CVIS_CONTROL_ACTIVE;
    show_control( w, CONTROL_DISABLED_STATUS_TEXTBOX, x, changed_bits & CVIS_CONTROL_ACTIVE   );

    program_status_has_changed = TRUE;
    show_program_status();
    }

/*
------------------------------------------------------------
If this is a new fatal error I need to show the error number
------------------------------------------------------------ */
if ( changed_bits & CVIS_CONTROL_ERR )
    {
    TCHAR * cp;
    cp = EmptyString;
    if ( SureTrakStatus & CVIS_CONTROL_ERR )
        cp = int32toasc( (int32) SureTrakFatalErrorNumber );
    set_text( w, FATAL_ERROR_NUMBER_TBOX, cp );
    }

show_control( w, FATAL_ERROR_STATUS_TEXTBOX,      SureTrakStatus & CVIS_CONTROL_ERR,       changed_bits & CVIS_CONTROL_ERR      );
show_control( w, WARNING_STATUS_TEXTBOX,          SureTrakStatus & VIS_SPEED_ZERO,         changed_bits & VIS_SPEED_ZERO        );
show_control( w, SHOT_IN_PROGRESS_STATUS_TEXTBOX, SureTrakStatus & CVIS_SHOT_IN_PROGRESS,  changed_bits & CVIS_SHOT_IN_PROGRESS );
show_control( w, BIT_11_STATUS_TEXTBOX,           SureTrakStatus & 0x0400,                 changed_bits & 0x0400                );
show_control( w, POWER_UP_STATUS_TEXTBOX,         SureTrakStatus & VIS_POWER_UP,           changed_bits & VIS_POWER_UP          );
show_control( w, INT_DISABLED_STATUS_TEXTBOX,     SureTrakStatus & VIS_INT_DISABLED,       changed_bits & VIS_INT_DISABLED      );
show_control( w, BIT_8_STATUS_TEXTBOX,            SureTrakStatus & 0x0080,                 changed_bits & 0x0080                );
show_control( w, POS_XDCR_WARNING_STATUS_TEXTBOX, SureTrakStatus & VIS_POS_TRANS_ERROR,    changed_bits & VIS_POS_TRANS_ERROR   );
show_control( w, MEMORY_ERROR_STATUS_TEXTBOX,     SureTrakStatus & CVIS_MEMORY_ERROR,      changed_bits & CVIS_MEMORY_ERROR     );
show_control( w, INVALID_COMMAND_STATUS_TEXTBOX,  SureTrakStatus & VIS_INVALID_COMMAND,    changed_bits & VIS_INVALID_COMMAND   );
show_control( w, INTERNAL_ERROR_STATUS_TEXTBOX,   SureTrakStatus & VIS_INTERNAL_ERROR,     changed_bits & VIS_INTERNAL_ERROR    );
show_control( w, BIT_2_STATUS_TEXTBOX,            SureTrakStatus & 0x0002,                 changed_bits & 0x0002                );
show_control( w, BIT_1_STATUS_TEXTBOX,            SureTrakStatus & 0x0001,                 changed_bits & 0x0001                );

if ( !first_time )
    changed_bits = old_input_word ^ SureTrakInputWord;

show_radio( w, PUMP_ON_RADIO,         SureTrakInputWord & 0x0001,  changed_bits & 0x0001 );
show_radio( w, START_SHOT_RADIO,      SureTrakInputWord & 0x0002,  changed_bits & 0x0002 );
show_radio( w, FOLLOW_THROUGH_RADIO,  SureTrakInputWord & 0x0004,  changed_bits & 0x0004 );
show_radio( w, RETRACT_RADIO,         SureTrakInputWord & 0x0008,  changed_bits & 0x0008 );
show_radio( w, JOG_SHOT_RADIO,        SureTrakInputWord & 0x0010,  changed_bits & 0x0010 );
show_radio( w, MANUAL_RADIO,          SureTrakInputWord & 0x0020,  changed_bits & 0x0020 );
show_radio( w, AT_HOME_RADIO,         SureTrakInputWord & 0x0040,  changed_bits & 0x0040 );
show_radio( w, OK_FOR_VAC_FAST_RADIO, SureTrakInputWord & 0x0200,  changed_bits & 0x0200 );

show_radio( w, VACUUM_ON_RADIO,       SureTrakInputWord & 0x0400,  changed_bits & 0x0400 );
show_radio( w, LOW_IMPACT_RADIO,      SureTrakInputWord & 0x0800,  changed_bits & 0x0800 );
show_radio( w, FILL_TEST_RADIO,       SureTrakInputWord & 0x1000,  changed_bits & 0x1000 );
show_radio( w, E_STOP_RADIO,          SureTrakInputWord & 0x8000,  changed_bits & 0x8000 );

if ( !first_time )
    changed_bits = old_output_word ^ SureTrakOutputWord;

show_radio( w, SLOW_SHOT_OUTPUT_RADIO,SureTrakOutputWord & 0x0001, changed_bits & 0x0001 );
show_radio( w, ACCUMULATOR_RADIO,     SureTrakOutputWord & 0x0002, changed_bits & 0x0002 );
show_radio( w, LS1_RADIO,             SureTrakOutputWord & 0x1000, changed_bits & 0x1000 );
show_radio( w, LS2_RADIO,             SureTrakOutputWord & 0x2000, changed_bits & 0x2000 );
show_radio( w, LS3_RADIO,             SureTrakOutputWord & 0x4000, changed_bits & 0x4000 );
show_radio( w, LS4_RADIO,             SureTrakOutputWord & 0x8000, changed_bits & 0x8000 );

if ( program_status_has_changed || old_block_number != SureTrakBlockNumber )
    {
    set_text( w, BLOCK_NUMBER_TEXTBOX, int32toasc((int32) SureTrakBlockNumber) );
    old_block_number = SureTrakBlockNumber;
    if ( SureTrakStatus & CVIS_CONTROL_ACTIVE )
        {
        set_text( w, WAITING_FOR_TEXTBOX, SureTrakBlockMessage.message(SureTrakBlockNumber) );
        }
    else if ( !Downloading )
        {
        UINT id;
        if ( SureTrakStatus & CVIS_CONTROL_ERR )
            id = SURETRAK_FATAL_STRING;
        else if ( NullValveWindow )
            id = SURETRAK_NULL_VALVE_STRING;
        else
            id = SURETRAK_NOT_ACTIVE_STRING;


        set_text( w, WAITING_FOR_TEXTBOX, resource_string(id) );
        }
    }

if ( first_time || old_pos != SureTrakRawPosition )
    {
    double divisor;
    divisor = (double) SureTrakParameters.counts_per_unit();
    if ( divisor < 1.0 )
        divisor = 1.0;
    fx  = (double) (short) SureTrakRawPosition;
    fx /= divisor;
    set_text( w, CURRENT_MAIN_POSITION_TEXTBOX, fixed_string(fx, LeftPosPlaces, RightPosPlaces) );
    old_pos = SureTrakRawPosition;
    }

if ( first_time || old_dac_word != SureTrakDacWord )
    {
    fx = (double) SureTrakDacWord;
    fx -= (double) 8192.0;
    fx /= (double) 819.2;
    set_text( w, CURRENT_VALVE_COMMAND_TEXTBOX, fixed_string(fx, 3, 2) );
/*  set_text( w, CURRENT_VALVE_COMMAND_TEXTBOX, ascii_double(fx) ); */
    old_dac_word = SureTrakDacWord;
    }

old_status       = SureTrakStatus;
old_input_word   = SureTrakInputWord;
old_output_word  = SureTrakOutputWord;
old_has_program  = SureTrakHasProgram;
first_time       = FALSE;
}

/***********************************************************************
*                     DOWNLOAD_ALL_SETUPS_TO_BOARD                     *
***********************************************************************/
void download_all_setups_to_board( void )
{
download_parameters_to_board();
send_command_to_board( PROFILE_DOWNLOAD_REQUEST );
}

/***********************************************************************
*                           RELOAD_SURETRAK                            *
***********************************************************************/
void reload_suretrak( void )
{
int i;

get_suretrak_part_name();
update_main_screen_distance_units();

HaveRunlist = Runlist.get( SureTrakPart.computer, SureTrakPart.machine );
NeedRunlistCheck = HaveRunlist;

if ( HaveRunlist )
    {
    for ( i=0; i<Runlist.count(); i++ )
        {
        if ( !part_exists( SureTrakPart.computer, SureTrakPart.machine, Runlist[i].partname) )
            {
            MessageBox( 0, resource_string(NO_WARMUP_PART_STRING), Runlist[i].partname, MB_OK | MB_SYSTEMMODAL );
            }
        }
    }

if ( !SureTrakParameters.find(SureTrakPart.computer) )
    MessageBox( 0, TEXT("Can't Find"), TEXT("Suretrak Parameters"), MB_OK | MB_SYSTEMMODAL );

if ( !SureTrakSetup.find(SureTrakPart.computer, SureTrakPart.machine, SureTrakPart.part) )
    MessageBox( 0, TEXT("Can't Find"), TEXT("Suretrak Steps"), MB_OK | MB_SYSTEMMODAL );

download_all_setups_to_board();
}

/***********************************************************************
*                          MAIN_SCREEN_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK main_screen_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static HBRUSH InputOnBackgroundBrush = 0;
static HBRUSH ErrorBackgroundBrush   = 0;

HWND ws;
int  id;


id  = LOWORD( wParam );

switch (msg)
    {

    case WM_DBINIT:
        /*
        ---------------------------------
        Load the name of the current part
        --------------------------------- */
        if ( !get_suretrak_part_name() )
            {
            resource_message_box( MainInstance, NO_CURRENT_PART_STRING, UNABLE_TO_CONTINUE_STRING );
            MainWindow.close();
            return TRUE;
            }

        start_suretrak_monitor_thread();
        reload_suretrak();
        send_command_to_board( ACTIVATE_CONTROL_REQUEST );
        update_main_screen_limit_switch_names();
        SetFocus( GetDlgItem(w, PARAMETERS_BUTTON) );
        return TRUE;

    case WM_INITDIALOG:
        MainScreenWindow = w;
        InputOnBackgroundBrush = CreateSolidBrush ( GreenColor );
        ErrorBackgroundBrush   = CreateSolidBrush ( RedColor );
        MainWindow.shrinkwrap( w );
        MainScreenWindow.post( WM_DBINIT );
        return TRUE;

    case WM_DESTROY:
        if ( InputOnBackgroundBrush )
            {
            DeleteObject( InputOnBackgroundBrush );
            InputOnBackgroundBrush = 0;
            }

        if ( ErrorBackgroundBrush )
            {
            DeleteObject( ErrorBackgroundBrush );
            ErrorBackgroundBrush = 0;
            }
        break;

    case WM_STATUS_CHANGE:
        refresh_status_display();
        return TRUE;

    case WM_CTLCOLORSTATIC:
        ws = (HWND) lParam;
        id = GetDlgCtrlID( ws );
        if ( id >= PUMP_ON_RADIO && id <= LS4_RADIO )
            {
            if ( IsWindowEnabled(ws) )
                {
                SetBkColor ((HDC) wParam, GreenColor );
                return (int) InputOnBackgroundBrush;
                }
            }
        else if ( !NullValveWindow && !Downloading && !(SureTrakStatus & CVIS_CONTROL_ACTIVE) && id == WAITING_FOR_TEXTBOX )
            {
            SetBkColor ((HDC) wParam, RedColor );
            return (int) ErrorBackgroundBrush;
            }
        break;

    case WM_HELP:
        if ( SureTrakStatus & CVIS_CONTROL_ERR )
            gethelp( HELP_CONTEXT, SURETRAK_PROBLEMS_HELP );
        else
            gethelp( HELP_CONTEXT, SURETRAK_MAIN_SCREEN_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case PARAMETERS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("SURETRAK_PARAMETER_SETUP"),
                    MainWindow.handle(),
                    (DLGPROC) param_dialog_proc );
                return TRUE;

            case RESTART_BUTTON:
                send_command_to_board( CONTROLLER_RESET_REQUEST );
                return TRUE;

            case CURRENT_VALUES_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("CURRENT_POS_AND_ANALOG_DIALOG"),
                    w,
                    (DLGPROC) current_variables_dialog_proc );
                return TRUE;

            case VALVE_TEST_BUTTON:
                if ( resource_message_box( MainInstance, IS_THIS_OK_STRING, WILL_TURN_OFF_CONTROL_STRING, MB_YESNO | MB_SYSTEMMODAL) == IDYES )
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("VALVE_TEST_DIALOG"),
                        MainWindow.handle(),
                        (DLGPROC) valve_test_dialog_proc );
                    }
                return TRUE;

            case NULL_VALVE_BUTTON:
                if ( SureTrakHasProgram )
                    {
                    if ( resource_message_box( MainInstance, IS_THIS_OK_STRING, WILL_TURN_OFF_CONTROL_STRING, MB_YESNO | MB_SYSTEMMODAL) == IDYES )
                        {
                        DialogBox(
                            MainInstance,
                            TEXT("NULL_VALVE_DIALOG"),
                            MainWindow.handle(),
                            (DLGPROC) null_valve_dialog_proc );
                        }
                    }
                else
                    resource_message_box( MainInstance, NO_PROGRAM_STATUS_STRING, CANT_SET_NULL_STRING );
                return TRUE;

            case TRANSDUCER_TEST_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("TRANSDUCER_TEST_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) transducer_test_dialog_proc );
                return TRUE;


            case SERVICE_TOOLS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("SERVICE_TOOLS_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) service_tools_dialog_proc );
                return TRUE;

            case FORCE_DOWNLOAD_BUTTON:
                download_parameters_to_board();
                send_command_to_board( PROFILE_DOWNLOAD_REQUEST );
                send_command_to_board( PROGRAM_DOWNLOAD_REQUEST );
                return TRUE;

            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                      NEW_MACHINE_SETUP_CALLBACK                      *
***********************************************************************/
void new_machine_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
const DWORD BYTES_TO_COPY = sizeof(TCHAR) * (MACHINE_NAME_LEN+1);
DWORD bytes_copied;
TCHAR machine_name[MACHINE_NAME_LEN+1];

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_copied = DdeGetData( edata, (LPBYTE) machine_name, BYTES_TO_COPY, 0 );
if ( bytes_copied > 0 )
    {
    *(machine_name+MACHINE_NAME_LEN) = NullChar;
    fix_dde_name( machine_name );
    if ( strings_are_equal(machine_name, SureTrakPart.machine) )
        reload_suretrak();
    }
}

/***********************************************************************
*                      NEW_MONITOR_SETUP_CALLBACK                      *
***********************************************************************/
void new_monitor_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
const DWORD BYTES_TO_COPY = sizeof(TCHAR) * (MACHINE_NAME_LEN+1);
DWORD bytes_copied;
TCHAR machine_name[MACHINE_NAME_LEN+1];

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_copied = DdeGetData( edata, (LPBYTE) machine_name, BYTES_TO_COPY, 0 );
if ( bytes_copied > 0 )
    {
    *(machine_name+MACHINE_NAME_LEN) = NullChar;
    fix_dde_name( machine_name );
    reload_suretrak();
    }

}

/***********************************************************************
*                           MONITOR_NEW_PART                           *
***********************************************************************/
static void monitor_new_part( LPARAM lParam )
{
TCHAR * part_name;

if ( !lParam )
    return;

/*
-------------------------------------------------------------------------------
lParam is a pointer to a string allocated for this purpose (which I must free).
------------------------------------------------------------------------------- */
part_name = (TCHAR *) lParam;

set_current_part_name( SureTrakPart.computer, SureTrakPart.machine, part_name );

delete[] part_name;

poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, SureTrakPart.machine );
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
*                              PLOT_MIN_CALLBACK                       *
***********************************************************************/
void plot_min_callback( TCHAR * topic, short item, HDDEDATA edata )
{
if ( !MainWindow.is_minimized() )
    MainWindow.minimize();
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        register_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX, new_machine_setup_callback );
        register_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX, new_monitor_setup_callback );
        register_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX,   password_level_callback );
        register_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX,   plot_min_callback       );
        get_current_password_level();
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    case WM_POKEMON:
        monitor_new_part( lParam );
        return 0;

    case WM_EV_SHUTDOWN:
        MainWindow.close();
        return 0;

    case WM_CLOSE:
        if ( !ShuttingDown )
            {
            ShuttingDown = TRUE;
            if ( SureTrakMonitorEvent )
                SetEvent( SureTrakMonitorEvent );
            }
        else if ( !SureTrakMonitorIsRunning )
            {
            /*
            --------------------------------------------------
            My threads have shut down, let the OS shut me down
            -------------------------------------------------- */
            break;
            }
        return 0;

    case WM_SETFOCUS:
        MainScreenWindow.set_focus();
        return TRUE;

    case WM_COMMAND:
        switch (id)
            {
            case MINIMIZE_SCREEN_MENU:
                MainWindow.minimize();
                return 0;

            case EXIT_TASK_MENU:
                MainWindow.close();
                return 0;
            }
        break;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static BOOLEAN startup( int cmd_show )
{
WNDCLASS wc;
TCHAR * cp;

ShuttingDown = FALSE;

SureTrakCommand.init( MAX_PENDING_SURETRAK_COMMANDS );

names_startup();
MyComputer.startup();
lstrcpy( SureTrakPart.computer, MyComputer.whoami() );
SureTrakBlockMessage.load( control_message_name(MyComputer.whoami()) );
MyTitle = resource_string( MAIN_WINDOW_TITLE_STRING );

GreenBackgroundBrush = CreateSolidBrush ( GreenColor );
RedBackgroundBrush   = CreateSolidBrush ( RedColor );

if ( is_previous_instance(MyName, 0) )
    return FALSE;

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, TEXT("LeftVelPlaces") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    LeftVelPlaces = asctoint32(cp);

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, TEXT("RightVelPlaces") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    RightVelPlaces = asctoint32(cp);

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, TEXT("LeftPosPlaces") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    LeftPosPlaces = asctoint32(cp);

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, TEXT("RightPosPlaces") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    RightPosPlaces = asctoint32(cp);

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, TEXT("LeftAnalogPlaces") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    LeftAnalogPlaces = asctoint32(cp);

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, TEXT("RightAnalogPlaces") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    RightAnalogPlaces = asctoint32(cp);

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, TEXT("ServiceToolsGrayLevel") );
if ( !unknown(cp) )
    ServiceToolsGrayLevel = asctoint32(cp);

cp = get_ini_string( SureTrakIniFile, ConfigSectionName, TEXT("ExitEnableLevel") );
if ( !unknown(cp) )
    ExitEnableLevel = asctoint32(cp);

units_startup();

wc.lpszClassName = MyName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MAKEINTRESOURCE(SURETRAK_ICON) );
wc.lpszMenuName  = TEXT("MainMenu");
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

CreateWindow(
    MyName,
    MyTitle.text(),
    WS_POPUP | WS_CAPTION | WS_MINIMIZEBOX,
    0, 0,                             // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT("MAIN_SCREEN"),
    MainWindow.handle(),
    (DLGPROC) main_screen_dialog_proc );

SureTrakMonitorEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !SureTrakMonitorEvent )
    resource_message_box( MainInstance,  CREATE_FAILED_STRING, SURETRAK_EVENT_FAIL_STRING );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );

return TRUE;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
shifts_shutdown();

unregister_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX );
unregister_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX );
unregister_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX );
unregister_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX );
client_dde_shutdown();
units_shutdown();

/*
-------------------------------------------------------------------------------
If I am running the warmup part, restore the main part name as the current part
------------------------------------------------------------------------------- */
if ( HaveRunlist )
    {
    if ( Runlist.count() > 1 )
        {
        if ( !strings_are_equal(Runlist[0].partname, SureTrakPart.part) )
            set_current_part_name( SureTrakPart.computer, SureTrakPart.machine, Runlist[0].partname );
        }
    }

if ( GreenBackgroundBrush )
    {
    DeleteObject( GreenBackgroundBrush );
    GreenBackgroundBrush = 0;
    }

if ( RedBackgroundBrush )
    {
    DeleteObject( RedBackgroundBrush );
    RedBackgroundBrush = 0;
    }
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

if ( !startup( cmd_show ) )
    return 0;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainScreenWindow.handle() && !status )
        status = IsDialogMessage( MainScreenWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
