#include <windows.h>
#define _CRTDBG_MAP_ALLOC
#include <ddeml.h>
#include <commctrl.h>
#include <crtdbg.h>

#include "..\include\visiparm.h"
#include "..\include\array.h"
#include "..\include\dbclass.h"
#include "..\include\evclass.h"
#include "..\include\fileclas.h"
#include "..\include\fifo.h"
#include "..\include\computer.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\msgclas.h"
#include "..\include\optocontrol.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\part_analog_sensor.h"
#include "..\include\rectclas.h"
#include "..\include\setpoint.h"
#include "..\include\stparam.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"

#include "..\include\ftii.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_file.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

#define _MAIN_
#include "extern.h"
#include "resource.h"
#include "shotsave.h"
#include "..\include\events.h"

/*
--------------------------------------------------------------------------
The following indexes can be used with current_lvdt_feedback_index(), etc.
-------------------------------------------------------------------------- */
static const int DAC_1 = 0;
static const int DAC_2 = 1;

TCHAR MyClassName[] = TEXT( "FASTRAK2" );

HACCEL  AccelHandle;
TCHAR   AccelName[] = TEXT("HOT_KEYS");

LISTBOX_CLASS ShutdownStatusListbox;
WINDOW_CLASS  ShutdownStatusDialog;

LISTBOX_CLASS OutputListbox;
WINDOW_CLASS OutputListDialog;
WINDOW_CLASS NullStatusMessage;
WINDOW_CLASS ManualInfoTbox;
static const int NOF_OPTOS = 4;
static WINDOW_CLASS InputOpto[NOF_OPTOS];
static WINDOW_CLASS OutputOpto[NOF_OPTOS];

static const int NOF_TEST_ALARM_OPTOS = 8;
static WINDOW_CLASS TestAlarmOpto[NOF_TEST_ALARM_OPTOS];

bool         ManualNullInProgress = false;

HWND       * Dnu = 0;
static HWND   ToolTipWindow  = 0;

static TCHAR CommaString[]   = TEXT( "," );
static TCHAR CrString[]      = TEXT( "\r" );
static TCHAR EqualsString[]  = TEXT( "=" );
static TCHAR EmptyString[]   = TEXT( "" );
static TCHAR OffString[]     = TEXT( " OFF" );
static TCHAR OnString[]      = TEXT( " ON" );
static TCHAR PercentString[] = TEXT( "%" );
static TCHAR PoundString[]   = TEXT( "#" );
static TCHAR ResetString[]   = TEXT( "RESET" );
static TCHAR SpaceString[]   = TEXT( " " );
static TCHAR TString[]       = TEXT( "T" );
static TCHAR TwoSpacesString[] = TEXT( "  " );
static TCHAR VString[]       = TEXT( "V" );
static TCHAR ZeroString[]    = TEXT( "0" );
static TCHAR ZeroPointZero[] = TEXT( "0.0" );
static TCHAR DotTxt[]        = TEXT( ".TXT" );
static TCHAR DotMsg[]        = TEXT( ".MSG" );

static TCHAR ColonChar      = TEXT( ':' );
static TCHAR CommaChar      = TEXT( ',' );
static TCHAR DecimalChar    = TEXT( '.' );
static TCHAR EqualsChar     = TEXT( '=' );
static TCHAR HChar          = TEXT( 'H' );
static TCHAR MinusChar      = TEXT( '-' );
static TCHAR NineChar       = TEXT( '9' );
static TCHAR NullChar       = TEXT( '\0' );
static TCHAR OneChar        = TEXT( '1' );
static TCHAR PercentChar    = TEXT( '%' );
static TCHAR PeriodChar     = TEXT( '.' );
static TCHAR PoundChar      = TEXT( '#' );
static TCHAR SpaceChar      = TEXT( ' ' );
static TCHAR TabChar        = TEXT( '\t' );
static TCHAR UnderscoreChar = TEXT( '_' );
static TCHAR VChar          = TEXT( 'V' );
static TCHAR ZeroChar       = TEXT( '0' );

static TCHAR ChannelModeSetString[] = TEXT( "V429=" );
static TCHAR ConfigWordMsg[]  = TEXT( "V313_" );
static TCHAR ConfigWord2Msg[] = TEXT( "V314_" );
static int   ConfigWordMsgLen = 5;
static TCHAR AnalogCSVChannelKey[] = TEXT( "AnalogCSVChannel" );
static TCHAR AnalogCSVFileKey[]    = TEXT( "AnalogCSVFile" );

static TCHAR OpenValve_1_Loop[]  = TEXT( "OC10=1" );
static TCHAR CloseValve_1_Loop[] = TEXT( "OC10=0" );
static TCHAR OpenValve_2_Loop[]  = TEXT( "OC11=1" );
static TCHAR CloseValve_2_Loop[] = TEXT( "OC11=0" );
static TCHAR ZeroPositionCmd[]   = TEXT( ".UA1=0" );
static TCHAR CycleString[]  = TEXT( "Cycle" );
static INT   CycleStringLen = 5;
static TCHAR FatalString[]  = TEXT( "Fatal" );
static INT   FatalStringLen = 5;
static TCHAR WarningString[]  = TEXT( "Warning" );
static INT   WarningStringLen = 7;
static TCHAR NoFileString[]  = TEXT( "No Upload File!" );
static INT   NoFileStringLen = 15;

FTII_VAR_ARRAY FtiiAutoNull;
FTII_VAR_ARRAY CurrentMachineVars;
HPEN           MasterPen;

static FIFO_CLASS  ErrorLog;

static bool    HaveValveMasterData = false;
static int     ValveMasterData[2000];
static int     ValveMasterMax = 1;
static int     ValveMasterMin = 0;

static int     ValveTestData[2000];
static int     ValveTestMax = 1;
static int     ValveTestMin = 0;

const UINT          HIRES_SENSOR_TEST_TIMER_ID = 4;
static const int NO_SHOT_PERIOD     = 0;
static const int START_SHOT_PERIOD  = 1;
static const int FOLLOW_THRU_PERIOD = 2;
static const int RETRACT_PERIOD     = 3;
static const int END_OF_SHOT_PERIOD = 4;
static const int NOF_PERIODS        = 5;

static int          CurrentPasswordLevel     = 0;
static int          CurrentHiresSensorPeriod = -1;
static int          HiresSensorPeriodMs[NOF_PERIODS] = { 0, 0, 0, 0, 0 };  /* The zeroth is not used */
static int          HiresSensorStartShotWire  = NO_WIRE;
static int          HiresSensorFollowThruWire = NO_WIRE;
static int          HiresSensorRetractWire    = NO_WIRE;
static int          ServiceToolsGrayLevel     = 0;

const UINT          REMOTE_MONITOR_TIMER_ID = 3;
const UINT          CLOSING_TIMER_ID = 2;
const UINT          RESUME_TIMER_ID  = 1;
static bool         WaitingToResume  = false;
static bool         ClosingAfterRemote = false;
static bool         ShutdownAnyway     = false;

static BOOLEAN      LoggingAnalogs = FALSE;
static int          AnalogCSVChannel = 1;
static int          AnalogCSVSensor  = 0;
static NAME_CLASS   AnalogCSVFile;
static DWORD        AnalogCSVStartMs      = 0;
static STRING_CLASS AnalogCSVTitle;

static WINDOW_CLASS ResumeDialog;
static WINDOW_CLASS Ft2TestDialog;
static HWND         SwitchChannelsXbox     = 0;
static HWND         AbortedMachineNameTbox = 0;

/*
------------------------------------------------------------------------------------
When I send a MONITOR_SETUP_INDEX dde message I need to ignore it. This is done only
if I am a remote monitor.
------------------------------------------------------------------------------------ */
static int MyMonitorSetupCount = 0;

/*
------------------------------------------------------------------------
This is to tell set_current_window to send a wm_close to the main window
------------------------------------------------------------------------ */
static bool NeedToCloseMainWindow = false;

/*
-----------------------------------------------------------------------------
This is to tell the current values dialog that the analogs all need refreshed
----------------------------------------------------------------------------- */
static bool NeedToRefreshAnalogs = true;

/*
------
Colors
------ */
static HBRUSH GreenBackgroundBrush  = 0;
static HBRUSH OrangeBackgroundBrush = 0;
static HBRUSH RedBackgroundBrush    = 0;

/*
------------------------------------------
List of machines for UPLOAD_ABORTED_DIALOG
------------------------------------------ */
TEXT_LIST_CLASS UploadAbortedList;

/*
----------------------
Current Values Globals
---------------------- */
static BOOLEAN  NeedRefresh    = TRUE;
static int      Max_Position   = 0;
static int      Max_Velocity   = 0;
static unsigned Control_Status = 0;
static unsigned Monitor_Status = 0;
static unsigned LastInputBits  = 0;
static unsigned LastOutputBits = 0;
static unsigned AnalogValue[ANALOG_VALUE_COUNT] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

/*
------------------------------------------
Used by global_parameter_dialog_proc, etc.
------------------------------------------ */
static int  NULL_DAC_INDEX         = 0;
static int  LVDT_DAC_INDEX         = 1;
static int  JOG_SHOT_VALVE_INDEX   = 2;
static int  FT_VALVE_INDEX         = 3;
static int  RETRACT_VALVE_INDEX    = 4;
static int  ZERO_SPEED_CHECK_INDEX = 5;
static int  NULL_DAC_2_INDEX       = 6;
static int  LVDT_DAC_2_INDEX       = 7;

FTII_VAR_ARRAY GlobalParameters;

/*
---------------------------
Used by control_mods_dialog
--------------------------- */
const static int CTRL_PROG_NUMBER_LEN = 4;   /* Used to compare line numbers like .022 */
static bool         UsingAltStep;
static TCHAR NormalStep[] = TEXT( ".025 D0" );
static STRING_CLASS AltStepNum;
static STRING_CLASS AltStepEnd;
static STRING_CLASS AltStepVel;
static STRING_CLASS AltStepAcc;
static STRING_CLASS AltStepPos;
static STRING_CLASS AltStepVelPrefix;
static STRING_CLASS AltStepAccPrefix;
static STRING_CLASS AltStepPosPrefix;
static STRING_CLASS ParkStepNum;
static STRING_CLASS ParkStep2Num;
static STRING_CLASS ParkStepVolts;
static STRING_CLASS ParkStepVoltsPrefix;
static STRING_CLASS StartStepNum;
static STRING_CLASS StartStepVolts;
static STRING_CLASS StartStepVoltsPrefix;
static bool         UpdatingControlModsDialog = false;

/*
------------------------------------------
Bit controls for the current values dialog
------------------------------------------ */
struct BIT_CONTROL {
    unsigned     bit;
    bool         is_visible;
    WINDOW_CLASS w;
    STRING_CLASS name;
    BIT_CONTROL() { bit=0; is_visible=false; }
    ~BIT_CONTROL() {}
    };

/*
------------
Als Log Bits
------------ */
static unsigned SLOW_SHOT_OUTPUT_MASK   = 0x10000;
static unsigned ACCUMULATOR_OUTPUT_MASK = 0x20000;

static unsigned MANUAL_MODE_MASK        = 0x200000;

/*
-----------------
Input Bit Indexes
----------------- */
const int START_SHOT_BIT_INDEX      =  0;
const int FOLLOW_THROUGH_BIT_INDEX  =  1;
const int RETRACT_BIT_INDEX         =  2;
const int JOG_SHOT_BIT_INDEX        =  3;
const int MANUAL_MODE_BIT_INDEX     =  4;
const int AT_HOME_BIT_INDEX         =  5;
const int OK_FOR_VAC_FAST_BIT_INDEX =  6;
const int VACUUM_ON_BIT_INDEX       =  7;
const int LOW_IMPACT_BIT_INDEX      =  8;
const int FILL_TEST_BIT_INDEX       =  9;
const int NofInputBitControls       = 10;

BIT_CONTROL InputBitControl[NofInputBitControls];

const UINT InputBitControlId[NofInputBitControls] = {
    START_SHOT_TBOX, FOLLOW_THROUGH_TBOX,
    RETRACT_TBOX, JOG_SHOT_TBOX, MANUAL_MODE_TBOX,
    AT_HOME_TBOX, OK_FOR_VAC_FAST_TBOX, VACUUM_ON_TBOX, LOW_IMPACT_TBOX,
    FILL_TEST_TBOX
    };

static BOOLEAN OptoMode = FALSE;

/*
----------------------------------------------------------------------
Indexes and Variable Numbers for the ftii machine level variables file
These are set in the set_dac and valve_test screens and need to be
reloaded whenever the current machine changes.
---------------------------------------------------------------------- */
const int LVDT_1_BOARD_LEVEL_OFFSET_INDEX = 0;
const int LVDT_2_BOARD_LEVEL_OFFSET_INDEX = 1;
const int DIGITAL_POTS_INDEX = 2;

static int MachineVars[] = {
    LVDT_1_BOARD_LEVEL_OFFSET_VN,
    LVDT_2_BOARD_LEVEL_OFFSET_VN,
    DIGITAL_POTS_VN
    };
const int NofMachineVars = sizeof(MachineVars)/sizeof(int);
static BOOLEAN UpdatingSetDacScreen    = FALSE;
static BOOLEAN UpdatingValveTestScreen = FALSE;
static BOOLEAN UsingValveTestOutput    = FALSE;
static BOOLEAN UsingNullValveOutput    = FALSE;

/*
-------------------
Digital pot indexes
------------------- */
const int AMP_2_CURRENT_GAIN = 0;
const int AMP_2_LVDT_GAIN    = 1;
const int AMP_1_CURRENT_GAIN = 2;
const int AMP_1_LVDT_GAIN    = 3;

/*
------------------
Output Bit Indexes
------------------ */
const int SLOW_SHOT_BIT_INDEX       = 0;
const int ACCUMULATOR_BIT_INDEX     = 1;
const int LS1_BIT_INDEX             = 2;
const int LS2_BIT_INDEX             = 3;
const int LS3_BIT_INDEX             = 4;
const int LS4_BIT_INDEX             = 5;
const int LS5_BIT_INDEX             = 6;
const int LS6_BIT_INDEX             = 7;
const int NofOutputBitControls      = 8;

BIT_CONTROL OutputBitControl[NofOutputBitControls];

const UINT OutputBitControlId[NofOutputBitControls] = {
    SLOW_SHOT_TBOX, ACCUMULATOR_TBOX, LS1_TBOX,
    LS2_TBOX, LS3_TBOX, LS4_TBOX, LS5_TBOX, LS6_TBOX
    };

struct STATUS_CONTROL {
    unsigned     bit;
    bool         is_visible;
    WINDOW_CLASS w;
    STATUS_CONTROL() { bit=0; is_visible=false; }
    };

/*
-----------------------------
Error Log Analog Sample Count
----------------------------- */
static int Lindex = -1;

/*
-----------------------------------------------------------------
These two values are set every time I get a new op status message
----------------------------------------------------------------- */
static int CurrentChannelMode  = 1;
static int CurrentFlashVersion = 6;

/*
---------------------------------------------------------
These are used to resize the main window for the terminal
--------------------------------------------------------- */
static bool HaveRects = false;
static RECTANGLE_CLASS OriginalRect;
static RECTANGLE_CLASS TerminalRect;
static int             MainEboxHeight  = 20;
static int             TopOfListBox    = 0;
static int             MainClientWidth = 0;
WNDPROC  OriEditControlProc;

const int NofMonitorStatusControls = 4;
static STATUS_CONTROL MonitorStatusControl[NofMonitorStatusControls];

BOOL CALLBACK AutoShotDisplayProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
bool     begin_monitoring();
void     check_to_see_if_any_threads_are_running();
int      current_board_index();
int      current_board_number();
int      current_channel_mode();
int      current_sub_version();
TCHAR *  current_upload_file_name();
int      current_version();
TCHAR *  current_version_string();
void     display_board_info();
void     fill_global_parameters();
BOOLEAN  is_suretrak_controlled( int i );
int      load_ftii_boards();
void     send_command( TCHAR * machine_name, TCHAR * command, bool need_to_set_event );
void     send_command_to_current_machine( TCHAR * command, bool need_to_set_event );
void     start_board_monitor_thread();
bool     start_chars_are_equal( const char * sorc, const char * s );
void     start_shotsave_thread();
void     stop_monitoring();
void     toggle_io_display();
void     update_downtime( DOWN_DATA_CLASS dd );
unsigned wirebit( int wire_number );

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                          ASCII_0_TO_5_VOLTS                          *
*               Convert unsigned 0-4095 to 0 to 5 volts                *
***********************************************************************/
TCHAR * ascii_0_to_5_volts( unsigned x )
{
double d;

d = (double) x;
d *= 5.0;
d /= 4095.0;
return fixed_string( d, 3, 2 );
}

/***********************************************************************
*                              DAC_VOLTS                               *
***********************************************************************/
double dac_volts( unsigned x )
{
double d;

d = double( x );
d /= 4095.0;
d *= 20.0;
d -= 10.0;
return round( d, 0.01 );
}

/***********************************************************************
*                          ASCII_DAC_VOLTS                             *
***********************************************************************/
TCHAR * ascii_dac_volts( unsigned x )
{
return ascii_double( dac_volts(x) );
}

/***********************************************************************
*                         ASCII_DAC_VOLTS_3                            *
*                         3 decimal places                             *
***********************************************************************/
TCHAR * ascii_dac_volts_3( unsigned x )
{
double d;

d = double( x );
d /= 4095.0;
d *= 20.0;
d -= 10.0;
d = round( d, 0.001 );
return ascii_double( d );
}

/***********************************************************************
*                               SHOWBITS                               *
*                                                                      *
*          Control ..2.  4..1  4...  .32.  Monitor ..6....1            *
***********************************************************************/
static TCHAR * showbits( TCHAR * dest, unsigned sorc )
{
const unsigned mask = 0x80000000;
unsigned u;
int      led;
int      module;

u = sorc;
dest = copystring( dest, TEXT("Control") );
for ( module=0; module<4; module++ )
    {
    *dest++ = SpaceChar;
    *dest++ = SpaceChar;
    for ( led=3; led>=0; led-- )
        {
        if ( u & mask )
            *dest++ = OneChar+led;
        else
            *dest++ = PeriodChar;
        u <<= 1;
        }
    }

dest = copystring( dest, TEXT("  Monitor ") );

u = sorc;
u <<= 24;
for ( module=7; module>=0; module-- )
    {
    if ( u & mask )
        *dest++ = OneChar+module;
    else
        *dest++ = PeriodChar;
    u <<= 1;
    }

*dest = NullChar;
return dest;
}

/***********************************************************************
*                     CURRENT_LVDT_FEEDBACK_INDEX                      *
***********************************************************************/
int current_lvdt_feedback_index( int dac )
{
int i;

/*
---------------------------------------------------
Default is ( CurrentChannelMode == CHANNEL_MODE_5 )
--------------------------------------------------- */
i = 6;

if ( CurrentChannelMode == CHANNEL_MODE_7 )
    {
    if ( dac == 0 )
        i = 4;
    else
        i = 5;
    }

return i;
}

/***********************************************************************
*                 CURRENT_RAW_LVDT_FEEDBACK_INDEX                      *
***********************************************************************/
unsigned current_raw_lvdt_feedback_index( int dac )
{
int i;

/*
----------------------------------------------------
Assume that ( CurrentChannelMode == CHANNEL_MODE_7 )
---------------------------------------------------- */
if ( dac == 0 )
    i = 9;
else
    i = 11;

return i;
}

/***********************************************************************
*                     CURRENT_MEASURED_COMMAND_INDEX                   *
***********************************************************************/
unsigned current_measured_command_index( int dac )
{
int i;

/*
---------------------------------------------------
Default is ( CurrentChannelMode == CHANNEL_MODE_5 )
--------------------------------------------------- */
i = 7;

if ( CurrentChannelMode == CHANNEL_MODE_7 )
    {
    if ( dac == 0 )
        i = 6;
    else
        i = 7;
    }

return i;
}

/***********************************************************************
*                               GET_HELP                               *
***********************************************************************/
void get_help()
{
static char s[] = "name=ft2base.swf";
const int SLEN = sizeof(s);
static STARTUPINFO info = {
    sizeof(STARTUPINFO),
    0, 0, 0,            // lpReserved, lpDesktop, lpTitle
    0,                  // dwX
    0,                  // dwY
    0, 0, 0, 0, 0,      // dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute
    STARTF_USEPOSITION, // dwFlags
    0, 0, 0, 0, 0, 0    // wShowWindow, cbReserved2, lpReserved2, hStdInput, hStdOutput, hStdError
    };

TCHAR   path[MAX_PATH+1];
FILE_CLASS f;
PROCESS_INFORMATION pi;

if ( get_exe_directory(path) )
    {
    lstrcat( path, TEXT("ft2help.txt") );
    f.open_for_write( path );
    f.writebinary( s, SLEN );
    f.close();

    get_exe_directory(path);
    lstrcat( path, TEXT("ft2help.exe") );
    CreateProcess( 0,          // lpApplicationName
        path,                  // lpCommandLine
        0, 0, 0,               // lpProcessAttributes, lpThreadAttributes, bInheritHandles,
        NORMAL_PRIORITY_CLASS, // dwCreationFlags
        0, 0,                  // lpEnvironment, lpCurrentDirectory,
        &info,                 // lpStartupInfo = input
        &pi                    // lpProcessInformation = output
        );
    }
}

/***********************************************************************
*                              DRAW_LINE                               *
***********************************************************************/
static void draw_line( HDC dc, int x1, int y1, int x2, int y2 )
{
MoveToEx( dc, x1, y1, 0 );
LineTo( dc, x2, y2 );
}

/***********************************************************************
*                              WRITE_LOG                               *
***********************************************************************/
static void write_log( TCHAR * sorc )
{
static FILE_CLASS   f;
static TIME_CLASS   t;
static STRING_CLASS s;
static TCHAR * cp;

t.get_local_time();
s = t.text();
s += SpaceString;
s += sorc;
f.open_for_append( TEXT("c:\\ft2.log") );
while ( ErrorLog.count() )
    {
    cp = (TCHAR *)ErrorLog.pop();
    f.writeline( cp );
    delete[] cp;
    }
f.writeline( s.text() );
f.close();
}

/***********************************************************************
*                         PUSH_LOG_MESSAGE                             *
***********************************************************************/
void push_log_message( TCHAR * sorc )
{
static TIME_CLASS   t;
static STRING_CLASS s;
static TCHAR * cp;

t.get_local_time();
s = t.text();
s += SpaceString;
s += sorc;
ErrorLog.push( s.release() );
}

/***********************************************************************
*                      CLEAR_CURRENT_VALUE_COPIES                      *
***********************************************************************/
static void clear_current_value_copies()
{
int i;

Max_Position   = 0;
Max_Velocity   = 0;
Control_Status = 0;
Monitor_Status = 0;
LastInputBits  = 0;
LastOutputBits = 0;
for ( i=0; i<ANALOG_VALUE_COUNT; i++ )
    AnalogValue[i] = 0;
}

/***********************************************************************
*                           FATAL_ERROR_STRING                         *
***********************************************************************/
TCHAR * fatal_error_string( int fatal_error_to_find )
{
static UINT id[] = { STACK_OVERFLOW_STRING, BLOCK_TIMEOUT_STRING, INVALID_BLOCK_STRING,
                     BLOCK_EXECUTION_ERROR_STRING, PROGRAM_STOPPED_BY_USER_STRING,
                     TEST_FATAL_ERROR_STRING };

static int fatal_error_number[] = { STACK_OVERFLOW, BLOCK_TIMEOUT, INVALID_BLOCK,
                                    BLOCK_EXECUTION_ERROR, PROGRAM_STOPPED_BY_USER,
                                    TEST_FATAL_ERROR };

const int nof_errors = sizeof(fatal_error_number)/sizeof(int);

int i;
int myid;

myid = 0;
for ( i=0; i<nof_errors; i++ )
    {
    if ( fatal_error_to_find == fatal_error_number[i] )
        {
        myid = id[i];
        break;
        }
    }
if ( myid > 0 )
    return resource_string( MainInstance, myid );

return EmptyString;
}

/***********************************************************************
*                         WARNING_ERROR_STRING                         *
***********************************************************************/
TCHAR * warning_error_string( int warning_to_find )
{
static UINT id[] = {
    VAC_ABORT_POS_TOO_LONG_STRING, BLOCK_TIMEOUT_WARNING_STRING, FILL_TEST_ABORT_STRING,
    MASTER_ENABLE_DROPPED_OUT_STRING, VAC_ABORT_ZSPEED_STRING, TEST_WARNING_STRING
    };

static int warning_number[] = {
    VAC_ABORT_POS_TOO_LONG, BLOCK_TIMEOUT_WARNING, FILL_TEST_ABORT,
    MASTER_ENABLE_DROPPED_OUT, VAC_ABORT_ZSPEED, TEST_WARNING
    };

const int nof_warnings = sizeof(warning_number)/sizeof(int);

int i;
int myid;

myid = 0;
for ( i=0; i<nof_warnings; i++ )
    {
    if ( warning_to_find == warning_number[i] )
        {
        myid = id[i];
        break;
        }
    }
if ( myid > 0 )
    return resource_string( MainInstance, myid );

return EmptyString;
}

/***********************************************************************
*                        RESOURCE_MESSAGE_BOX                          *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                         OKCANCEL_MESSAGE_BOX                         *
***********************************************************************/
bool okcancel_message_box( UINT msg_id, UINT title_id )
{
if ( resource_message_box(MainInstance, msg_id, title_id, MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK )
    return true;

return false;
}

/***********************************************************************
*                               GET_BYTE                               *
***********************************************************************/
static unsigned get_byte( unsigned sorc, int byte_number )
{
static unsigned mask[4] = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
unsigned n;

if ( byte_number < 0 || byte_number > 3 )
    return 0;

sorc &= mask[byte_number];
n = byte_number * 8;
if ( n )
    sorc >>= n;

return sorc;
}

/***********************************************************************
*                               PUT_BYTE                               *
* Shift the low byte of sorc into byte_number [0,3] of dest.           *
***********************************************************************/
static void put_byte( unsigned & dest, unsigned sorc, int byte_number )
{
static unsigned mask[4] = { 0xFFFFFF00, 0xFFFF00FF, 0xFF00FFFF, 0x00FFFFFF };
unsigned n;

if ( byte_number < 0 || byte_number > 3 )
    return;

sorc &= 0xFF;
n = byte_number * 8;
if ( n )
    sorc <<= n;

dest &= mask[byte_number];
dest |= sorc;
}

/***********************************************************************
*                          SAVE_MACHINE_VARS                           *
***********************************************************************/
static BOOLEAN save_machine_vars()
{
STRING_CLASS s;

s = ftii_machine_settings_name( ComputerName.text(), CurrentMachine.name );
return CurrentMachineVars.put( s.text() );
}

/***********************************************************************
*                        LOAD_MACHINE_VARS                             *
***********************************************************************/
static BOOLEAN load_machine_vars()
{
NAME_CLASS s;
int        i;
int        version;

CurrentMachineVars.empty();
CurrentMachineVars.upsize( NofMachineVars );

for ( i=0; i<NofMachineVars; i++ )
    CurrentMachineVars[i].set( MachineVars[i], 0 );

version = current_version();
s.get_ft2_machine_settings_file_name( ComputerName.text(), CurrentMachine.name );
if ( !file_exists(s.text()) )
    {
    s.get_exe_dir_file_name( ComputerName.text(), ft2_control_program_name(version, CurrentMachine) );
    if ( !file_exists(s.text()) )
        return FALSE;
    }

CurrentMachineVars.get_from( s.text() );

if ( CurrentMachine.monitor_flags & MA_HAS_DIGITAL_SERVO )
    HasDigitalServoAmp = TRUE;
else
    HasDigitalServoAmp = FALSE;

return TRUE;
}

/***********************************************************************
*                         DIGITAL_POT_SETTING                          *
*               ---------- Amp 2 ------  --------- Amp 1 ------        *
*               Current Gain  LVDT Gain  Current Gain  LVDT Gain       *
* byte_offset         0           1            2           3           *
***********************************************************************/
static unsigned digital_pot_setting( int byte_offset )
{
unsigned u;
unsigned mask;
unsigned n;

mask = 0x7F; /* Only 7 bits are significant */
n    = byte_offset * 8;

if ( n > 0 )
    mask <<= n;

u = CurrentMachineVars[DIGITAL_POTS_INDEX].uval();
u &= mask;
if ( n > 0 )
    u >>= n;

return u;
}

/***********************************************************************
*                     SCROLL_LISTBOX_TO_LAST_ENTRY                     *
***********************************************************************/
static void scroll_listbox_to_last_entry()
{
int ti;

ti = MainListBox.count() - MainListBox.visible_row_count();

if ( ti > MainListBox.top_index() )
    MainListBox.set_top_index( ti );
}

/***********************************************************************
*                             SAVE_LISTBOX                             *
***********************************************************************/
static void save_listbox()
{
FILE_CLASS f;
int i;
int n;

n = MainListBox.count();
if ( n > 0 )
    {
    f.open_for_write( TEXT("C:\\terminal.txt") );

    for ( i=0; i<n; i++ )
        f.writeline( MainListBox.item_text(i) );

    f.close();
    }
}

/***********************************************************************
*                         NEW_DOWNTIME_CALLBACK                        *
***********************************************************************/
void new_downtime_callback( TCHAR * topic, short item, HDDEDATA edata )
{
DOWN_DATA_CLASS dd;

if ( dd.set(edata) )
    update_downtime( dd );
}

/***********************************************************************
*                       POSITION_FT2_TEST_DIALOG                       *
***********************************************************************/
static void position_ft2_test_dialog()
{
RECTANGLE_CLASS rp;
RECTANGLE_CLASS r;
long x;

MainWindow.getrect( rp );
Ft2TestDialog.getrect( r );

x = r.width();
r.left = (rp.left + rp.right - x)/2;
r.right = r.left + x;

x = r.height();
r.top = (rp.top + rp.bottom - x)/2;
r.bottom = r.top + x;

Ft2TestDialog.move( r );
}

/***********************************************************************
*                            EDIT_CONTROL_PROC                         *
***********************************************************************/
LRESULT CALLBACK edit_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static STRING_CLASS lastcommand;
static STRING_CLASS s;

if ( msg == WM_CHAR && LOWORD(wParam) == VK_RETURN )
    {
    s.get_text( MainEbox );
    if ( s.isempty() )
        s = EmptyString;

    s.uppercase();
    /*
    ------------------------------------------------
    Check to see if I'm starting the ft2 test dialog
    ------------------------------------------------ */
    if ( s == TString )
        {
        s = EmptyString;
        s.set_text( MainEbox );
        Ft2TestDialog.show();
        position_ft2_test_dialog();
        SetFocus( Ft2TestDialog.control(TEST_7_RADIO) );
        return 0;
        }
    lastcommand = s;
    send_command_to_current_machine( s.text(), true );

    s = EmptyString;
    s.set_text( MainEbox );
    SetFocus( MainEbox );
    return 0;
    }
else if ( (msg == WM_KEYUP || msg == WM_KEYDOWN)  && LOWORD(wParam) == VK_UP )
    {
    if ( msg == WM_KEYUP )
        {
        if ( !lastcommand.isempty() )
            {
            lastcommand.set_text( MainEbox );
            SetFocus( MainEbox );
            }
        }
    }
else
    {
    return CallWindowProc( OriEditControlProc, hWnd, msg, wParam, lParam );
    }

return 0;
}

/***********************************************************************
*                           TOGGLE_TERMINAL                            *
***********************************************************************/
void toggle_terminal()
{
RECTANGLE_CLASS r;

if ( !HaveRects )
    {
    MainWindow.getrect( OriginalRect );
    MainWindow.get_client_rect( r );
    if ( r.width() < MainClientWidth )
        OriginalRect.right += MainClientWidth - r.width();

    if ( r.height() < TopOfListBox )
        OriginalRect.bottom += TopOfListBox - r.height();
    TerminalRect = OriginalRect;
    TerminalRect.bottom += 170;
    HaveRects = true;
    }

toggle_menu_view_state( TerminalIsVisible, MainWindow.handle(), (UINT) FTII_TERMINAL_CHOICE );
if ( TerminalIsVisible )
    {
    ShowWindow( MainEbox,             SW_SHOW );
    ShowWindow( MainListBox.handle(), SW_SHOW );
    MainWindow.move( TerminalRect );
    SetFocus( MainEbox );
    }
else
    {
    ShowWindow( MainEbox, SW_HIDE );
    ShowWindow( MainListBox.handle(), SW_HIDE );
    MainWindow.move( OriginalRect );
    }
}

/***********************************************************************
*                     TOGGLE_LOGGING_SENT_COMMANDS                     *
***********************************************************************/
static void toggle_logging_sent_commands()
{
INI_CLASS ini;
NAME_CLASS s;

toggle_menu_view_state( LoggingSentCommands, MainWindow.handle(), (UINT) FTII_LOG_MESSAGES_CHOICE );

s.get_exe_dir_file_name( MonallIniFile );
ini.set_file( s.text() );
ini.put_boolean( ConfigSection, LogSentCommandsKey, LoggingSentCommands );
}

/***********************************************************************
*                         POSITION_TERMINAL                            *
***********************************************************************/
static void position_terminal()
{

WINDOW_CLASS w;
long x;
long y;

RECTANGLE_CLASS parent_rect;
RECTANGLE_CLASS r;

MainWindow.get_client_rect( parent_rect );

/*
--------------------------
Position the Editbox first
-------------------------- */
w = MainEbox;
w.get_move_rect( r );

x = r.left;
y = parent_rect.bottom - MainEboxHeight - 1;
w.move( x, y, FALSE );

/*
--------------------------------------
The listbox goes on top of the editbox
-------------------------------------- */
w = MainListBox.handle();
w.get_move_rect( r );
r.bottom = y - 1;
w.move( r );
}

/***********************************************************************
*                           RESIZE_TERMINAL                            *
***********************************************************************/
static void resize_terminal()
{
RECTANGLE_CLASS r;

MainWindow.getrect( r );
if ( TerminalIsVisible )
    {
    if ( r.height() != TerminalRect.height() )
        position_terminal();
    TerminalRect = r;
    OriginalRect.moveto( r.left, r.top );
    }
else
    {
    OriginalRect = r;
    TerminalRect.moveto( r.left, r.top );
    }
}

/***********************************************************************
*                             MOVE_TERMINAL                            *
***********************************************************************/
static void move_terminal()
{
RECTANGLE_CLASS r;

MainWindow.getrect( r );
OriginalRect.moveto( r.left, r.top );
TerminalRect.moveto( r.left, r.top );
}

/***********************************************************************
*                      SEND_NULL_OFFSET_TO_BOARD                       *
***********************************************************************/
bool send_null_offset_to_board()
{
STRING_CLASS   s;
FTII_VAR_ARRAY va;

va.upsize(1);
va[0].set( NULL_DAC_OFFSET_VN, 0 );

s = ftii_global_settings_name( ComputerName.text(), CurrentMachine.name );
if ( va.get_from(s.text()) )
    {
    if ( va[0].makeline(s) )
        {
        send_command_to_current_machine( s.text(), true );
        return true;
        }
    }
return false;
}

/***********************************************************************
*                      SEND_LVDT_OFFSET_TO_BOARD                       *
***********************************************************************/
bool send_lvdt_offset_to_board()
{
STRING_CLASS   s;
FTII_VAR_ARRAY va;

va.upsize(1);
va[0].set( LVDT_DAC_OFFSET_VN, 0 );

s = ftii_global_settings_name( ComputerName.text(), CurrentMachine.name );
if ( va.get_from(s.text()) )
    {
    if ( va[0].makeline(s) )
        {
        send_command_to_current_machine( s.text(), true );
        return true;
        }
    }
return false;
}

/***********************************************************************
*                            SET_CONTROL_Y                             *
***********************************************************************/
void set_control_y( int y, UINT id )
{
WINDOW_CLASS    w;
RECTANGLE_CLASS r;

w = CurrentValuesDialog.control( id );
w.get_move_rect( r );
r.bottom = y + r.height();
r.top = y;
w.move( r );
}

/***********************************************************************
*                        SET_CHANNEL_5_8_LABELS                        *
***********************************************************************/
static void set_channel_5_8_labels( int channel_mode )
{
static TCHAR CmdString[]  = TEXT( "Cmd" );
static TCHAR LvdtString[] = TEXT( "LVDT" );
static TCHAR MeasString[] = TEXT( "Meas" );

WINDOW_CLASS w;
RECTANGLE_CLASS r;
int  channel;
int  i;
int  y;
int  delta_y;
int  therm_y_offset;

UINT id;
BOOLEAN checked;
bool   is_visible;

w = CurrentValuesDialog.control( CH_17_20_XBOX );
checked = is_checked( w.handle() );
if ( checked )
    is_visible = false;
else
    is_visible = true;

w = CurrentValuesDialog.control( FT2_MODE_UP_PB );
w.show( is_visible );
w = CurrentValuesDialog.control( FT2_MODE_DOWN_PB );
w.show( is_visible );

/*
-----------------------------------------------------
Get the offset from one analog value tbox to the next
----------------------------------------------------- */
w = CurrentValuesDialog.control( VOLTS_2_TBOX );
w.get_move_rect( r );
delta_y = r.top;
y       = r.top;

w = CurrentValuesDialog.control(FT_A2_THERM);
w.get_move_rect( r );
therm_y_offset = r.top - y;

w = CurrentValuesDialog.control(VOLTS_1_TBOX);
w.get_move_rect( r );
delta_y -= r.top;

/*
-----------------------------------------------------------------------------------
Channel is the channel number to appear on the labels 5-8 if the mode is 1, 2, or 3
----------------------------------------------------------------------------------- */
channel = 3;
if ( channel_mode != CHANNEL_MODE_7 )
    {
    for ( i=CH_3_TBOX; i<=CH_5_TBOX; i++ )
        {
        w = CurrentValuesDialog.control( i );
        if ( i == CH_5_TBOX )
            {
            if ( channel_mode == CHANNEL_MODE_2 )
                channel = 9;
            else if ( channel_mode == CHANNEL_MODE_3 )
                channel = 13;
            }
        w.set_title( int32toasc(channel) );
        channel++;
        }
    }

if ( checked )
    {
    if ( HasDigitalServoAmp )
        {
        //w = CurrentValuesDialog.control( PRESSURE_CONTROL_STATIC );
        //w.show();

        w = CurrentValuesDialog.control( FT_A3_THERM );
        w.post( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 20000) );

        w = CurrentValuesDialog.control( CH_3_TBOX );
        w.set_title( CmdString  );

        w = CurrentValuesDialog.control( CH_4_TBOX );
        w.set_title( MeasString );

        w = CurrentValuesDialog.control( CH_5_TBOX );
        w.set_title( LvdtString );
        }
    /*
    ---------------------------------------------------------------
    else
        {
        w = CurrentValuesDialog.control( PRESSURE_CONTROL_STATIC );
        w.hide();
        }
    --------------------------------------------------------------- */

    w = CurrentValuesDialog.control( CH_6_TBOX );
    w.set_title( CmdString  );

    w = CurrentValuesDialog.control( CH_7_TBOX );
    w.set_title( MeasString );

    w = CurrentValuesDialog.control( CH_8_TBOX );
    w.set_title( LvdtString );

    w = CurrentValuesDialog.control( FT_A6_THERM );
    w.post( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 20000) );
    }
else
    {
    w = CurrentValuesDialog.control( PRESSURE_CONTROL_STATIC );
    w.hide();

    w = CurrentValuesDialog.control( FT_A3_THERM );
    w.post( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

    /*
    -------------------------------------------------------------------------------------------
    The channel number is set in the previous section, as it is different for modes 1, 2, and 3
    ------------------------------------------------------------------------------------------- */
    for ( i=CH_6_TBOX; i<=CH_8_TBOX; i++ )
        {
        w = CurrentValuesDialog.control( i );
        w.set_title( int32toasc(channel) );
        channel++;
        }

    w = CurrentValuesDialog.control( FT_A6_THERM );
    w.post( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );
    }

for ( i=0; i<6; i++ )
    {
    y += delta_y;
    if ( checked )
        {
        if ( (i==0 && channel_mode==CHANNEL_MODE_7) || (i==3 && channel_mode==CHANNEL_MODE_5) )
            {
            set_control_y( y, MIN_VOLTS_TBOX );
            set_control_y( y, MAX_VOLTS_TBOX );
            y += delta_y;
            }
         }
    set_control_y( y, CH_3_TBOX+i    );
    set_control_y( y, VOLTS_3_TBOX+i );
    set_control_y( y+ therm_y_offset, FT_A3_THERM+i );
    }

for ( id=MIN_VOLTS_TBOX; id<=MAX_VOLTS_TBOX; id++ )
    {
    w = CurrentValuesDialog.control(id);
    if ( channel_mode==CHANNEL_MODE_7 || channel_mode == CHANNEL_MODE_5 )
        w.show();
    else
        w.hide();
    }

CurrentValuesDialog.refresh();

NeedToRefreshAnalogs = true;
}

/***********************************************************************
*                        SET_CHANNEL_5_8_LABELS                        *
***********************************************************************/
static void set_channel_5_8_labels()
{
set_channel_5_8_labels( current_channel_mode() );
}

/***********************************************************************
*                          SET_CURRENT_WINDOW                          *
***********************************************************************/
static void set_current_window( WINDOW_CLASS & newwindow )
{
static bool new_is_diagnostic_window;
static bool current_is_diagnostic_window;
int     channel_mode;
BOOLEAN checked;
TCHAR * cp;
HWND w;

if ( !newwindow.handle() )
    return;

if ( newwindow == CurrentWindow )
    return;

new_is_diagnostic_window     = (newwindow == SetDacDialog) || (newwindow == SetNullDialog) || (newwindow == CurrentValuesDialog) || (newwindow == TestAlarmOutputsDialog);
current_is_diagnostic_window = (CurrentWindow == SetDacDialog) || (CurrentWindow == SetNullDialog) || (CurrentWindow == CurrentValuesDialog) || (CurrentWindow == TestAlarmOutputsDialog);

if ( !WaitingToResume )
    {

    if ( CurrentWindow == MainWindow )
        {
        fill_global_parameters();

        w = CurrentValuesDialog.control( VELOCITY_UNITS_TBOX );
        set_text( w, units_name(CurrentPart.velocity_units) );

        w = CurrentValuesDialog.control( POSITION_UNITS_TBOX );
        set_text( w, units_name(CurrentPart.distance_units) );
        }

    /*
    ------------------------------------------------------------------------------------------------------
    If I am leaving the set null, set dac, or valve test screens, turn off the valve test in operation bit
    ------------------------------------------------------------------------------------------------------ */
    checked = FALSE;
    if ( CurrentWindow == SetNullDialog && UsingNullValveOutput )
        {
        /*
        ------------------------------------------------
        I'm leaving the Null Valve. Turn off the output;
        ------------------------------------------------ */
        checked = TRUE;

        if ( newwindow == SetDacDialog || newwindow == ValveTestDialog )
            {
            /*
            -----------------------------------------------------------------------
            If I'm going to another screen that needs the output, don't turn it off
            ----------------------------------------------------------------------- */
            if ( UsingValveTestOutput )
                checked = FALSE;
            }
        }

    if ( CurrentWindow == SetDacDialog || CurrentWindow == ValveTestDialog )
        {
        if ( newwindow != SetDacDialog && newwindow != ValveTestDialog )
            {
            if ( UsingValveTestOutput )
                checked = TRUE;

            if ( newwindow == SetNullDialog && UsingNullValveOutput )
                checked = FALSE;
            }
        }

    if ( checked )
        send_command_to_current_machine( TEXT("OO1=~00100000"), true );

    /*
    -----------------------------------------------------------------------------------
    If I am leaving the set null dialog I need to restore the null offset to the board.
    ----------------------------------------------------------------------------------- */
    if ( CurrentWindow == SetNullDialog )
        send_null_offset_to_board();
    else if ( CurrentWindow == SetDacDialog )
        send_lvdt_offset_to_board();

    //if ( CurrentWindow == MainWindow || newwindow == MainWindow || CurrentWindow != ButtonDialog )
    if ( CurrentWindow != MainWindow && ( newwindow == MainWindow || CurrentWindow != ButtonDialog ) )
        CurrentWindow.hide();

    if ( newwindow == MainWindow && CurrentWindow != ButtonDialog )
        ButtonDialog.hide();

    /*
    ----------------------------------------
    Turn off the operational status messages
    ---------------------------------------- */
    if ( current_is_diagnostic_window && !new_is_diagnostic_window )
        {
        send_command_to_current_machine( DONT_SEND_OP_DATA_PACKETS,   true );
        }

    if ( CurrentWindow == SetDacDialog )
        {
        if ( is_checked( SetDacDialog.control(AMP_1_RADIO) ) )
           cp = CloseValve_1_Loop;
        else
           cp = CloseValve_2_Loop;

        send_command_to_current_machine( cp,  true );
        }

    if ( CurrentWindow == SetNullDialog || CurrentWindow == SetDacDialog || CurrentWindow == TestAlarmOutputsDialog )
        {
        if ( newwindow != SetNullDialog && newwindow != SetDacDialog && newwindow != TestAlarmOutputsDialog )
            send_command_to_current_machine( StartControlPgmCmd,  true );   /* Start the control program */

        if ( CurrentWindow == SetNullDialog )
            NullStatusMessage.set_title( EmptyString );
        }
    else if ( CurrentWindow == ValveTestDialog )
        {
        send_command_to_current_machine( TEXT("OC1=0"),   true );    /* Stop Oscilloscope mode */
        Sleep(1);
        send_command_to_current_machine( TEXT(".J0.1"),   true );   /* Jump to control        */
        if ( newwindow == SetNullDialog || newwindow == SetDacDialog || newwindow == TestAlarmOutputsDialog )
            {
            send_command_to_current_machine( TEXT(".H"), true );
            ResumeDialog = newwindow;
            WaitingToResume = true;
            temp_message( TEXT("Waiting for Control Halt..."), 3000 );
            SetTimer( MainWindow.handle(), RESUME_TIMER_ID, 3000, 0 );
            return;
            }
        }

    /*
    ------------------------------------------------------------------------------------
    If I am switching to the diagnostic screens, turn on the operational status messages
    ------------------------------------------------------------------------------------ */
    if ( new_is_diagnostic_window && !current_is_diagnostic_window )
        {
        load_machine_vars();
        send_command_to_current_machine( SEND_OP_DATA_PACKETS, true );
        }

    if ( (newwindow == MainWindow) && NeedToCloseMainWindow )
        {
        ResumeDialog = newwindow;
        WaitingToResume = true;
        temp_message( TEXT("Waiting for Shutdown..."), 3000 );
        SetTimer( MainWindow.handle(), RESUME_TIMER_ID, 3000, 0 );
        return;
        }
    }

WaitingToResume = false;

if ( newwindow == SetNullDialog || newwindow == SetDacDialog || newwindow == TestAlarmOutputsDialog )
    {
    if ( CurrentWindow != SetNullDialog && CurrentWindow != SetDacDialog && CurrentWindow != ValveTestDialog && CurrentWindow != TestAlarmOutputsDialog )
        {
        send_command_to_current_machine( TEXT(".H"), true );
        Sleep( 100 );
        }

    if ( newwindow == SetNullDialog )
        send_command_to_current_machine( TEXT("OC5=1"), true );
    else if ( newwindow == SetDacDialog )
        send_command_to_current_machine( TEXT("V351=0"), true );

    }
else if ( newwindow == ValveTestDialog )
    {
    ValveTestDialog.post( WM_DBINIT );
    send_command_to_current_machine( TEXT("V390=750"),  true ); /* .5 Volts trigger        */
    Sleep(1);
    send_command_to_current_machine( TEXT("V391=17"),   true ); /* Analog channel 17       */
    Sleep(1);
    send_command_to_current_machine( TEXT("V392=1000"), true ); /* 1000 us per sample      */
    Sleep(1);
    send_command_to_current_machine( TEXT(".J0.105"),   true ); /* Jump to valve test      */
    Sleep(1);
    send_command_to_current_machine( TEXT("OC1=0"),    true );  /* Start Oscilloscope mode */
    Sleep(1);
    send_command_to_current_machine( TEXT("OC1=1"),    true );  /* Start Oscilloscope mode */
    }

/*
------------------------------------------------------------------------------------------------------
If I am entering the set null, set dac, or valve test screens, turn on the valve test in operation bit
------------------------------------------------------------------------------------------------------ */
checked = FALSE;
if ( newwindow == SetNullDialog && UsingNullValveOutput )
    checked = TRUE;

if ( newwindow == SetDacDialog || newwindow == ValveTestDialog )
    {
    if ( UsingValveTestOutput )
        checked = TRUE;
    }
if ( checked )
    send_command_to_current_machine( TEXT("OO1=00100000"), true );

if ( newwindow == CurrentValuesDialog )
    {
    NeedToRefreshAnalogs = true;

    checked = is_checked(SwitchChannelsXbox);
    channel_mode = current_channel_mode();
    if ( (channel_mode==CHANNEL_MODE_5 || channel_mode==CHANNEL_MODE_7) && !checked )
        set_checkbox( SwitchChannelsXbox, TRUE );
    else if ( channel_mode==CHANNEL_MODE_1 && checked )
        set_checkbox( SwitchChannelsXbox, FALSE );
    CurrentValuesDialog.post( WM_DBINIT );
    }

/*
-----------------------------------------------------
If I was on the valve test dialog I missed doing this
----------------------------------------------------- */
if ( CurrentWindow == ValveTestDialog && new_is_diagnostic_window )
    {
    load_machine_vars();
    send_command_to_current_machine( SEND_OP_DATA_PACKETS, true );
    }

CurrentWindow = newwindow;
CurrentWindow.show();
if ( CurrentWindow != ButtonDialog && CurrentWindow != MainWindow )
    CurrentWindow.set_focus();

if ( CurrentWindow == SetDacDialog && HasDigitalServoAmp )
    {
    CurrentWindow.post( WM_DBINIT );
    if ( is_checked( SetDacDialog.control(AMP_1_RADIO) ) )
       cp = OpenValve_1_Loop;
    else
       cp = OpenValve_2_Loop;
    send_command_to_current_machine( cp,  true );
    }


if ( (CurrentWindow == MainWindow) && NeedToCloseMainWindow )
    {
    MainWindow.close();
    }
}

/***********************************************************************
*                            ULTOHEXSTRING                             *
*H 0000 000F and returns a pointer to the null at the end of the string*
***********************************************************************/
static TCHAR * ultohexstring( TCHAR * dest, unsigned u )
{
*dest++ = HChar;
*dest++ = SpaceChar;
copystring( dest, ultohex(u) );
if ( lstrlen(dest) < 8 )
    rjust( dest, 8, ZeroChar );

dest += 4;
insert_char( dest, SpaceChar );

dest += 5;
return dest;
}

/***********************************************************************
*                            DO_CONFIG_WORD                            *
***********************************************************************/
static void do_config_word( TCHAR * sorc )
{
TCHAR s[80];
TCHAR * cp;
unsigned u;

cp = sorc + ConfigWordMsgLen;
u  = asctoul( cp );

cp = copystring( s, sorc );
cp = copystring( cp, TEXT(" = ") );
ultohexstring( cp, u );

MainListBox.set_current_index( MainListBox.add(s) );
scroll_listbox_to_last_entry();
}

/***********************************************************************
*                             PERCENT_TO_HEX                           *
***********************************************************************/
static TCHAR * percent_to_hex( double percent )
{
static TCHAR buf[] = TEXT( "H80000000" );
TCHAR * cp;
int32 x;

percent *= 100.0;
percent = round( percent, 1.0 );
x = (int32) percent;

cp = buf + 5;
insalph( cp, x, 4, ZeroChar, HEX_RADIX );

return buf;
}

/***********************************************************************
*                            SHOULD_BE_HEX                             *
*        Assumes dest points to a string like "V500=H00000000"         *
*        V319_256 = H 0000 0010 = H00000000 + 256                      *
***********************************************************************/
static BOOLEAN should_be_hex( TCHAR * dest, TCHAR * sorc )
{
static TCHAR equalspad[] = TEXT( " = " );
static TCHAR pluspad[]   = TEXT( " + " );

const unsigned HIGH_WORD_MASK = 0xFFFF0000;
const unsigned LOW_WORD_MASK  = 0x0000FFFF;

unsigned u;
unsigned masked_value;

if ( *sorc != VChar )
    return FALSE;

dest = copystring( dest, sorc );
dest = copystring( dest, equalspad );

sorc = findchar( UnderscoreChar, sorc );
if ( !sorc )
    return FALSE;

sorc++;
if ( *sorc == HChar )
    return FALSE;

u = asctoul( sorc );

dest = ultohexstring( dest, u );

masked_value = u & HIGH_WORD_MASK;
if ( masked_value )
    {
    dest = copystring( dest, equalspad );
    dest = ultohexstring( dest, masked_value );
    dest = copystring( dest, pluspad );

    masked_value = u & LOW_WORD_MASK;
    dest = copystring( dest,  ultoascii(masked_value) );
    }

return TRUE;
}

/***********************************************************************
*                          GET_ASCII_RT_DATA                           *
* I am responsible for deleting the buffer. lParam was allocated as    *
* TCHAR.                                                               *
***********************************************************************/
static void get_ascii_rt_data( LPARAM lParam )
{
static const TCHAR analog_response[] = TEXT( "O_CH" );
static const int  analog_response_slen = 4;

static const TCHAR null_response[] = TEXT( "R_AXIS" );
static const int  null_response_slen = 6;

static const TCHAR osw1_response[] = TEXT( "O_OSW1=" );
static const int  osw1_response_slen = 7;

static TCHAR mybuf[150];

static STRING_CLASS sc;

int    channel_index;
TCHAR * buf;
TCHAR * cp;
TCHAR * s;
unsigned x;

buf = (TCHAR *) lParam;
s   = buf;

if ( !buf )
    return;

if ( TerminalIsVisible )
    {
    if ( strings_are_equal(s, ConfigWordMsg, ConfigWordMsgLen) || strings_are_equal(s, ConfigWord2Msg, ConfigWordMsgLen) )
        {
        do_config_word( s );
        }
    else if ( !strings_are_equal( s, analog_response, analog_response_slen) )
        {
        if ( strings_are_equal( s, osw1_response, osw1_response_slen) )
            {
            copystring( mybuf, s );
            cp = mybuf + osw1_response_slen;
            x = (unsigned) hextoul( cp );
            rjust( cp, 8, ZeroChar );
            insert_char( cp, SpaceChar );
            insert_char( cp, HChar );
            cp += 6;
            insert_char( cp, SpaceChar );
            cp += 5;
            *cp++ = SpaceChar;
            showbits( cp, x );
            cp = mybuf;
            }
        else
            {
            cp = s;
            if ( should_be_hex(mybuf, s) )
                cp = mybuf;
            }
        MainListBox.set_current_index( MainListBox.add(cp) );
        scroll_listbox_to_last_entry();
        }
    }

if ( strings_are_equal(s, analog_response, analog_response_slen) )
    {
    sc = s;
    s += analog_response_slen;
    cp = findchar( EqualsChar, s );
    if ( cp )
        {
        *cp = NullChar;
        channel_index = asctoint32( s );
        channel_index--;
        cp++;

        /*
        --------------------------------------------------------------------------
        If the channel index is > 0 then the first hex digit is the channel index,
        unless the channel is 17-20, in which case the channel index is
        the channel index - 4. I skip this digit in the following two lines.
        -------------------------------------------------------------------------- */
        if ( channel_index > 0 )
            cp++;
        x = (unsigned) hextoul( cp );
        if ( x != CurrentAnalogValue[channel_index] )
            {
            CurrentAnalogValue[channel_index] = x;
            if ( CurrentWindow != MainWindow && CurrentWindow != ButtonDialog )
                CurrentWindow.post( WM_NEW_DATA );
            }

        if ( TerminalIsVisible )
            {
            sc += TEXT( " = " );
            if ( channel_index < 16 )
                {
                sc += ascii_0_to_5_volts( x );
                }
            else
                {
                sc += ascii_dac_volts( x );
                }
            sc += TEXT( " Volts" );
            MainListBox.set_current_index( MainListBox.add(sc.text()) );
            scroll_listbox_to_last_entry();
            }
        }
    }
else if ( strings_are_equal(s, null_response, null_response_slen) )
    {
    if ( CurrentWindow == SetNullDialog )
        {
        CurrentWindow.post( WM_NEW_RT_DATA, 0, (LPARAM) buf );
        buf = 0;
        }
    }
else if ( NeedErrorLog )
    {
    /*
    ------------------------------------------------
    Start logging the feedback values for 10 seconds
    ------------------------------------------------ */
    if ( findstring( CycleString, buf) != 0 )
        Lindex = 0;
    write_log( buf );
    }

if ( buf )
    delete[] buf;
}

/***********************************************************************
*                             COPY_ANALOG                              *
***********************************************************************/
static void copy_analog( int channel_index, unsigned short raw_value )
{
unsigned x;

x = (unsigned) raw_value;
x &= 0xFFF;
CurrentAnalogValue[channel_index] = x;
}

/***********************************************************************
*                        SHOW_CURRENT_OP_STATUS                        *
***********************************************************************/
static void show_current_op_status()
{
TCHAR buf[150];
TCHAR * cp;
int i;
unsigned u;

if ( !TerminalIsVisible )
    return;

cp = buf;

for ( i=0; i<2; i++ )
    {
    cp = copystring( cp,  int32toasc((int32)CurrentOpStatus.dac[i]) );
    *cp = SpaceChar;
    cp++;
    }

for ( i=0; i<16; i++ )
    {
    u = (unsigned) CurrentOpStatus.analog[i];
    u &= 0xFFF;
    cp = copystring( cp, ultoascii((unsigned long)u) );
    *cp = SpaceChar;
    cp++;
    }
*cp = NullChar;

MainListBox.set_current_index( MainListBox.add(buf) );
scroll_listbox_to_last_entry();
}

/***********************************************************************
*                            DO_NEW_RT_DATA                            *
***********************************************************************/
static void do_new_rt_data( WPARAM wParam, LPARAM lParam )
{
int i;
FTII_OP_STATUS_DATA * opstat;

switch ( wParam )
    {
    case ASCII_TYPE:
        get_ascii_rt_data( lParam );
        lParam = 0;
        break;

    case OP_STATUS_TYPE:
        CurrentChannelMode  = current_channel_mode();
        CurrentFlashVersion = current_version();
        opstat = (FTII_OP_STATUS_DATA *) lParam;
        if ( CurrentOpStatusIsEmpty || CurrentOpStatus != *opstat )
            {
            CurrentOpStatus        = *opstat;
            CurrentOpStatusIsEmpty = false;

            //show_current_op_status();  debug

            /*
            --------------------------------------------------------
            There are only 16 analog values (4 on the end are blank)
            -------------------------------------------------------- */
            for ( i=0; i<NOF_OP_STATUS_ANALOGS; i++ )
                copy_analog( i, CurrentOpStatus.analog[i] );

            if ( CurrentWindow != MainWindow && CurrentWindow != ButtonDialog )
                CurrentWindow.post( WM_NEW_DATA );
            }
        break;

    case OSCILLOSCOPE_TYPE:
        if ( CurrentWindow == ValveTestDialog )
            {
            ValveTestDialog.post( WM_NEW_DATA, 0, lParam );
            lParam = 0;
            }
        break;
    }

if ( lParam )
    delete[] (char *) lParam;
}

/***********************************************************************
*                           CREATE_CONTROLS                            *
***********************************************************************/
static void create_controls( int nof_rows )
{
RECTANGLE_CLASS r;
bool     need_resize;
int      col;
int      height;
int      id;
int      row;
int      width;
int      x;
int      y;
HDC      dc;

static TCHAR * ColName[] = {
    { TEXT("Board") },
    { TEXT("Address") },
    { TEXT("Port") },
    { TEXT("Comm Status") },
    { TEXT("Machine") },
    { TEXT("Part") },
    { TEXT("Monitor Status") }
    };

static int ColWidth[NofCols] = { 7, 18, 7, 20, 8, 15, 30 };

if ( Dnu )
    delete[] Dnu;

Dnu = 0;
if ( nof_rows > 0 )
    Dnu = new HWND[ nof_rows ];

need_resize = false;
dc = MainWindow.get_dc();

Char_Width  = average_character_width( dc );
Line_Height = character_height( dc );

MainWindow.release_dc( dc );

Line_Height += 2;

if ( nof_rows > 1 )
    NofRows = nof_rows;
else
    NofRows = 1;

MainPushButton    = new CONTROL_INFO[NofRows];
AddressTbox       = new CONTROL_INFO[NofRows];
PortTbox          = new CONTROL_INFO[NofRows];
ConnectedTbox     = new CONTROL_INFO[NofRows];
MachineTbox       = new CONTROL_INFO[NofRows];
PartTbox          = new CONTROL_INFO[NofRows];
MonitorStatusTbox = new CONTROL_INFO[NofRows];

id     = BOARD_1_PB;
x      = 0;
y      = Line_Height;
width  = Char_Width * 5;
height = Line_Height - 2;

for ( row=0; row<NofRows; row++ )
    {
    x = 0;
    width  = Char_Width * ColWidth[BOARD_NUMBER_COLUMN];
    MainPushButton[row].id = id;
    MainPushButton[row].w = CreateWindow(
        TEXT("button"),
        int32toasc(row),
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        x, y, width, height,
        MainWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    x += width;
    width  = Char_Width * ColWidth[TCP_ADDRESS_COLUMN];
    id++;
    AddressTbox[row].id = id;
    AddressTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("192.168.254.100"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        MainWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    x += width;
    width  = Char_Width * ColWidth[TCP_PORT_COLUMN];
    id++;
    PortTbox[row].id = id;
    PortTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("20000"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        MainWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    x += width;
    width  = Char_Width * ColWidth[CONNECTED_COLUMN];
    id++;
    ConnectedTbox[row].id = id;
    ConnectedTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("Startup"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        MainWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    x += width;
    width  = Char_Width * ColWidth[MACHINE_NAME_COLUMN];
    id++;
    MachineTbox[row].id = id;
    MachineTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("MA"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        MainWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    x += width;
    width  = Char_Width * ColWidth[PART_NAME_COLUMN];
    id++;
    PartTbox[row].id = id;
    PartTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("PA"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        MainWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    x += width;
    width  = Char_Width * ColWidth[MONITOR_STATUS_COLUMN];
    id++;
    MonitorStatusTbox[row].id = id;
    MonitorStatusTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("Startup Status"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        MainWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    Dnu[row] = CreateWindow(
        TEXT("static"),
        TEXT(""),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x+width+10, y+1, Char_Width, height,
        MainWindow.handle(),
        (HMENU) 2000,
        MainInstance,
        NULL
        );

    id++;
    y += Line_Height;
    }


/*
------
Titles
------ */
x      = 0;
for ( col=0; col<NofCols; col++ )
    {
    width  = Char_Width * ColWidth[col];
    CreateWindow(
        TEXT("static"),
        ColName[col],
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, 0, width, height,
        MainWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );
    x += width;
    id++;
    }

/*
-----------------------------------------------------
Make a text box to show the message count for testing
width  = Char_Width * 5;
CreateWindow(
    TEXT("static"),
    TEXT("0"),
    WS_CHILD | WS_VISIBLE | SS_LEFT,
    x-width, 0, width, height,
    MainWindow.handle(),
    (HMENU) MAIN_MESSAGE_COUNT_TBOX,
    MainInstance,
    NULL
    );
CreateWindow(
    TEXT("static"),
    TEXT("0"),
    WS_CHILD | WS_VISIBLE | SS_LEFT,
    x, 0, width, height,
    MainWindow.handle(),
    (HMENU) MAIN_MESSAGE_COUNT_1_TBOX,
    MainInstance,
    NULL
    );
----------------------------------------------------- */

MainClientWidth = x;
MainWindow.get_client_rect( r );
if ( r.width() < x )
    {
    if ( HaveRects )
        {
        OriginalRect.right += x - r.width();
        need_resize = true;
        }
    r.right = r.left + x;
    }
if ( r.height() < y )
    {
    if ( HaveRects )
        {
        OriginalRect.bottom += y - r.height();
        need_resize = true;
        }
    r.bottom = r.top + y;
    }
display_board_info();
get_text( CurrentMachine.name, MachineTbox[0].w, MACHINE_NAME_LEN );
CurrentMachine.find( ComputerName.text(), CurrentMachine.name );
if ( IsRemoteMonitor )
    CurrentMachine.monitor_flags |= MA_MONITORING_ON;
CurrentPart.find( CurrentMachine.computer, CurrentMachine.name, CurrentMachine.current_part );
CurrentBoardIndex = current_board_index();
load_machine_vars();
set_checkbox( MainPushButton[0].w, TRUE );

TopOfListBox = y;

id = MAIN_LISTBOX_ID;
MainListBox.init( CreateWindow(TEXT ("listbox"),
                  NULL,
                  WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOSEL,
                  0, y, r.width(), 150,                       // x, y, w, h
                  MainWindow.handle(),
                  (HMENU) id,
                  MainInstance,
                  NULL)
                );

y += 150;
id = MAIN_EBOX;
MainEbox = CreateWindow( TEXT("EDIT"),
              NULL,
              WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
              0, y, r.width(), MainEboxHeight,
              MainWindow.handle(),
              (HMENU) id,
              MainInstance,
              NULL );

OriEditControlProc = (WNDPROC) SetWindowLong( MainEbox, GWL_WNDPROC, (LONG) edit_control_proc );

TerminalRect = OriginalRect;
TerminalRect.bottom += 170;

if ( need_resize )
    MainWindow.move( OriginalRect );
}

/***********************************************************************
*                   RETRY_UPLOAD_TO_ABORTED_MACHINE                    *
***********************************************************************/
static void retry_upload_to_aborted_machine()
{
STRING_CLASS s;

s.get_text( UploadAbortedDialog.control(ABORTED_MACHINE_NAME_TBOX) );
send_command( s.text(), RESET_MACHINE_CMD, true );
}

/***********************************************************************
*                   GET_NEXT_UPLOAD_ABORTED_MACHINE                    *
***********************************************************************/
static bool get_next_upload_aborted_machine()
{
if ( UploadAbortedList.count() )
    {
    UploadAbortedList.rewind();
    UploadAbortedList.next();
    set_text( AbortedMachineNameTbox, UploadAbortedList.text() );
    UploadAbortedList.remove();
    return true;
    }
return false;
}

/***********************************************************************
*                        UPLOAD_ABORTED_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK upload_aborted_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int id;
HWND       w;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        UploadAbortedDialog = hWnd;
        AbortedMachineNameTbox = UploadAbortedDialog.control( ABORTED_MACHINE_NAME_TBOX );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                retry_upload_to_aborted_machine();
                if ( !get_next_upload_aborted_machine() )
                    UploadAbortedDialog.hide();
                return TRUE;

            case IDCANCEL:
                if ( !get_next_upload_aborted_machine() )
                    UploadAbortedDialog.hide();
                return TRUE;
            }
        break;

    case WM_CTLCOLORSTATIC:
        w = (HWND) lParam;
        if ( w == AbortedMachineNameTbox )
            {
            SetBkColor( (HDC) wParam, OrangeColor );
            return (int) OrangeBackgroundBrush;
            }
        break;

    case WM_CLOSE:
        UploadAbortedDialog.hide();
        return FALSE;

    case WM_DESTROY:
        UploadAbortedDialog = (HWND) 0;
        break;
    }

return FALSE;
}

/***********************************************************************
*                      SHOW_UPLOAD_ABORTED_DIALOG                      *
***********************************************************************/
static void show_upload_aborted_dialog( int i )
{
STRING_CLASS s;
s.get_text( MachineTbox[i].w );
if ( !s.isempty() )
    {
    if ( !is_suretrak_controlled(i) )
        {
        send_command( s.text(), RESET_MACHINE_CMD, true );
        }
    else if ( !UploadAbortedDialog.is_visible() )
        {
        s.set_text( UploadAbortedDialog.control(ABORTED_MACHINE_NAME_TBOX) );
        UploadAbortedDialog.show();
        }
    else
        {
        UploadAbortedList.append( s.text() );
        }
    }
}

/***********************************************************************
*                       DO_NEW_FT2_CONNECT_STATE                       *
***********************************************************************/
static void do_new_ft2_connect_state( WPARAM wParam, LPARAM lParam )
{
int      i;
unsigned new_state;
TCHAR *  s;

i = (int) wParam;
new_state = 0;

if ( i != NO_INDEX )
    {
    new_state = (unsigned) lParam;

    if ( new_state == NO_FT2_CONNECT_STATE || new_state & NOT_MONITORED_STATE )
        {
        s = NoConnectState.text();
        }
    else if ( new_state & NOT_CONNECTED_STATE )
        {
        s = NotConnected.text();
        }
    else if ( new_state & CONNECTING_STATE )
        {
        s = Connecting.text();
        }
    else if ( new_state & CONNECTED_STATE )
        {
        s = Connected.text();
        if ( new_state & UPLOAD_WAS_ABORTED_STATE && (i < NofRows) )
            show_upload_aborted_dialog(i);
        if ( CurrentWindow == CurrentValuesDialog )
            {
            if ( is_checked(MainPushButton[i].w) )
                send_command_to_current_machine( SEND_OP_DATA_PACKETS, true );
            }
        }
    else
        {
        s = unknown_string();
        }

    if ( i < NofRows )
        {
        set_text( ConnectedTbox[i].w, s );
        if ( new_state & NOT_MONITORED_STATE )
            set_text( MonitorStatusTbox[i].w, NotMonitored.text() );
        else if ( new_state & ZERO_STROKE_LENGTH_STATE )
            set_text( MonitorStatusTbox[i].w, ZeroStrokeLength.text() );
        }
    }
}

/***********************************************************************
*                           CREATE_TOOLTIPS                            *
* The first time you call this it creates the tooltip window.          *
* Thereafter it just updates the text.                                 *
***********************************************************************/
static void create_tooltips()
{
static TCHAR default_text[] = TEXT( "None" );

WINDOW_CLASS    w;
RECTANGLE_CLASS r;
FTII_LIMIT_SWITCH_WIRE_CLASS lsw;
PARAMETER_CLASS p;

int i;
int led;
int module;
int wire;
UINT id;

long halfway_across;
long halfway_up;
BOOLEAN firstime;
STRING_CLASS s;

TOOLINFO ti;

lsw.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
p.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

firstime = FALSE;
if ( !ToolTipWindow )
    {
    firstime = TRUE;
    ToolTipWindow = CreateWindowEx(
        0,
        TOOLTIPS_CLASS,     /* Class name */
        0,                  /* Title */
        TTS_ALWAYSTIP,      /* Style */
        CW_USEDEFAULT,      /* x */
        CW_USEDEFAULT,      /* x */
        CW_USEDEFAULT,      /* x */
        CW_USEDEFAULT,      /* x */
        CurrentValuesDialog.handle(),
        0,
        MainInstance,
        0
        );
    SendMessage( ToolTipWindow, TTM_SETDELAYTIME, TTDT_AUTOPOP, 30000 );
    }

ti.cbSize = sizeof(TOOLINFO);
ti.uFlags = 0;
ti.hwnd   = CurrentValuesDialog.handle();
ti.hinst  = MainInstance;
ti.uId    = OPTO_TOOLTIP_1;

if ( firstime )
    {
    /*
    -------------------------------------------------------------
    Input messages are constant. Assume this is a control system.
    ------------------------------------------------------------- */
    wire = 16;
    id = CONTROL_INPUT_18_STRING;
    for ( module=0; module<4; module++ )
        {
        w = CurrentValuesDialog.control( INPUT_1_OPTO+module );
        w.get_move_rect( r );
        halfway_across = (r.left + r.right) / 2;
        halfway_up = (r.top + r.bottom) / 2;
        for ( led=0; led<4; led++ )
            {
            wire++;
            ti.lpszText = default_text;
            if ( wire > 17 && wire < 30 )
                {
                ti.lpszText = resource_string( id );
                id++;
                }
            ti.rect.left   = r.left;
            ti.rect.right  = r.right;
            ti.rect.top    = r.top;
            ti.rect.bottom = r.bottom;
            if ( led == 0 )
                {
                ti.rect.left = halfway_across;
                ti.rect.top  = halfway_up;
                }
            else if ( led == 1 )
                {
                ti.rect.left   = halfway_across;
                ti.rect.bottom = halfway_up;
                }
            else if ( led == 2 )
                {
                ti.rect.right = halfway_across;
                ti.rect.top   = halfway_up;
                }
            else if ( led == 3 )
                {
                ti.rect.right  = halfway_across;
                ti.rect.bottom = halfway_up;
                }
            SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );
            }
        }
    }

/*
-------
Outputs
------- */
wire = 16;
ti.uId    = OPTO_TOOLTIP_17;
for ( module=0; module<4; module++ )
    {
    w = CurrentValuesDialog.control( OUTPUT_1_OPTO+module );
    w.get_move_rect( r );
    halfway_across = (r.left + r.right) / 2;
    halfway_up = (r.top + r.bottom) / 2;
    for ( led=0; led<4; led++ )
        {
        wire++;
        ti.lpszText = default_text;
        if ( wire == 17 )
            s = TEXT( "Slow Shot Output  " );
        else if ( wire == 18 )
            s = TEXT( "Accumulator  " );
        else if ( wire == 21 && UsingValveTestOutput )
            s = TEXT( "Valve Test in Operation  " );
        else if ( wire == 28 )
            s = TEXT( "Warning (Active Low)  " );
        else
            s = EmptyString;
        if ( wire == DownTimeAlarmWire )
            s += TEXT( "Downtime Alarm Wire  " );
        if ( wire == MonitorWire )
            s += TEXT( "Monitor Wire  " );

        for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
            {
            if ( lsw[i] == (unsigned) wire )
                {
                s += TEXT( "Limit Switch #" );
                s += i+1;
                s += TEXT( "  " );
                }
            }

        for ( i=0; i<MAX_PARMS; i++ )
            {
            if ( wire == (int) p.parameter[i].limits[ALARM_MIN].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=Low Alarm  " );
                }
            if ( wire == (int) p.parameter[i].limits[ALARM_MAX].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=High Alarm  " );
                }
            if ( wire == (int) p.parameter[i].limits[WARNING_MIN].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=Low Warning  " );
                }
            if ( wire == (int) p.parameter[i].limits[WARNING_MAX].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=High Warning  " );
                }
            }

        if ( s.len() > 0 )
            ti.lpszText = s.text();

        ti.rect.left   = r.left;
        ti.rect.right  = r.right;
        ti.rect.top    = r.top;
        ti.rect.bottom = r.bottom;
        if ( led == 0 )
            {
            ti.rect.left = halfway_across;
            ti.rect.top  = halfway_up;
            }
        else if ( led == 1 )
            {
            ti.rect.left   = halfway_across;
            ti.rect.bottom = halfway_up;
            }
        else if ( led == 2 )
            {
            ti.rect.right = halfway_across;
            ti.rect.top   = halfway_up;
            }
        else if ( led == 3 )
            {
            ti.rect.right  = halfway_across;
            ti.rect.bottom = halfway_up;
            }

        if ( firstime )
            SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );
        else
            SendMessage( ToolTipWindow, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti );
        ti.uId++;
        }
    }

}

/***********************************************************************
*                        REFRESH_CURRENT_VALUES                        *
***********************************************************************/
static void refresh_current_values()
{
NAME_CLASS filename;
NAME_CLASS s;
WINDOW_CLASS w;

s = current_version_string();
s.set_text( CurrentValuesDialog.control(FLASH_VERSION_TBOX) );

s = current_upload_file_name();
s.set_text( CurrentValuesDialog.control(UPLOAD_FILE_NAME_TBOX) );

filename = s;
filename.replace( DotTxt, DotMsg );
s.get_exe_dir_file_name( filename.text() );
if ( !s.file_exists() )
    s = default_ftii_control_message_name();
if ( s.file_exists() )
    StepMessage.load( s.text() );

w = CurrentValuesDialog.control( TOGGLE_IO_DISPLAY_PB );
if ( CurrentMachine.suretrak_controlled )
    {
    w.show();
    create_tooltips();
    }
else
    {
    if ( OptoMode )
        toggle_io_display();
    w.hide();
    }
}

/***********************************************************************
*                       REFRESH_CURRENT_MACHINE                        *
***********************************************************************/
static void refresh_current_machine( TCHAR * new_machine_name )
{
CurrentMachine.find( ComputerName.text(), new_machine_name );
if ( IsRemoteMonitor )
    CurrentMachine.monitor_flags |= MA_MONITORING_ON;
CurrentPart.find( CurrentMachine.computer, CurrentMachine.name, CurrentMachine.current_part );
load_machine_vars();
}

/***********************************************************************
*                      NEW_MONITOR_SETUP_CALLBACK                      *
***********************************************************************/
void new_monitor_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
const DWORD BYTES_TO_COPY = sizeof(TCHAR)*(MACHINE_NAME_LEN + 1);
MACHINE_CLASS m;
DWORD   bytes_copied;
TCHAR * s;
HWND    w;

s = maketext( MACHINE_NAME_LEN );

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_copied = DdeGetData( edata, (LPBYTE) s, BYTES_TO_COPY, 0 );
if ( bytes_copied <= 0 || MyMonitorSetupCount>0 )
    {
    if ( MyMonitorSetupCount > 0 )
        MyMonitorSetupCount--;
    delete[] s;
    return;
    }

*(s+MACHINE_NAME_LEN) = NullChar;
fix_dde_name( s );

if ( !m.find(ComputerName.text(), s) )
    {
    delete[] s;
    return;
    }

if ( m.ft_board_number == current_board_number() )
    {
    if ( strings_are_equal(CurrentMachine.name, m.name) || (m.monitor_flags & MA_MONITORING_ON) )
        {
        if ( IsRemoteMonitor )
            m.monitor_flags |= MA_MONITORING_ON;

        refresh_current_machine( s );

        /*
        ---------------------------------------------------------------------
        If this machine is not being monitored then close the realtime window
        --------------------------------------------------------------------- */
        if ( (CurrentWindow != MainWindow) && !(m.monitor_flags & MA_MONITORING_ON) )
            set_current_window( MainWindow );
        }
    }

if ( CurrentWindow == CurrentValuesDialog && strings_are_equal(CurrentMachine.name, s) )
    {
    w = CurrentValuesDialog.control( VELOCITY_UNITS_TBOX );
    set_text( w, units_name(CurrentPart.velocity_units) );

    w = CurrentValuesDialog.control( POSITION_UNITS_TBOX );
    set_text( w, units_name(CurrentPart.distance_units) );
    }

MainWindow.post( WM_NEWSETUP, 0, (LPARAM) s );
}

/***********************************************************************
*                      POSITION_BUTTON_CHILD                           *
***********************************************************************/
static void position_button_child( WINDOW_CLASS & w )
{

WINDOW_CLASS pbw;
long x;
long y;
long width;

RECTANGLE_CLASS parent_rect;
RECTANGLE_CLASS pbr;
RECTANGLE_CLASS r;

ButtonDialog.get_client_rect( parent_rect );
w.get_move_rect( r );

pbw = ButtonDialog.control( EXIT_RADIO );
pbw.get_move_rect( pbr );

/*
---------------------------------------------------------
Center the dialog in the area to the right of the buttons
--------------------------------------------------------- */
x     = pbr.width() + 2*pbr.left;
width = parent_rect.width() - x;
width -= r.width();
width /= 2;

if ( x > 0 )
    x += width;

y = parent_rect.height() - r.height();
if ( y > 0 )
    y /= 2;
else
    y = 0;
r.moveto( x, y );
w.move( r );
}

/***********************************************************************
*                   SEND_COMMAND_TO_CURRENT_MACHINE                    *
***********************************************************************/
static void send_command_to_current_machine( TCHAR * command, bool need_to_set_event )
{
static TCHAR NullMessage[] = TEXT("NULL");
static STRING_CLASS s;
TCHAR * cp;

if ( command )
    cp = command;
else
    cp = NullMessage;

if ( TerminalIsVisible )
    {
    MainListBox.set_current_index( MainListBox.add(cp) );
    scroll_listbox_to_last_entry();
    }

if ( command )
    {
    s = command;
    s += CrString;
    send_command( CurrentMachine.name, s.text(), need_to_set_event );
    }
else
    {
    send_command( CurrentMachine.name, 0, need_to_set_event );
    }
}

/***********************************************************************
*                    DEACTIVATE_CONTROL_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK deactivate_control_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                EndDialog( hWnd, TRUE );
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, FALSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         FILL_MACHINE_LISTBOX                         *
***********************************************************************/
static BOOLEAN fill_machine_listbox( HWND hWnd )
{
MACHINE_NAME_LIST_CLASS m;
COMPUTER_CLASS          c;
LISTBOX_CLASS           lb;
STRING_CLASS            s;

c.rewind();
while ( c.next() )
    {
    if ( !c.is_this_computer() )
        {
        if ( c.is_present() )
            {
            m.add_computer( c.name() );
            }
        }
    }

if ( m.count() > 0 )
    {
    lb.init( hWnd, CHOOSE_MACHINE_LB );
    m.rewind();
    while ( m.next() )
        {
        s = m.computer_name();
        s += TwoSpacesString;
        s += m.name();
        lb.add( s.text() );
        }
    }

return TRUE;
}

/***********************************************************************
*                           REMOTE_SHUTDOWN                            *
***********************************************************************/
static void remote_shutdown()
{
const BITSETYPE mask = MA_MONITORING_ON;
NAME_CLASS   s;
DB_TABLE     t;
BITSETYPE    my_monitor_flags;

ShutdownStatusListbox.add( TEXT("Turning monitoring back on...") );
s.get_machset_file_name( ComputerName.text() );
t.open( s.text(), MACHSET_RECLEN, PWL );
t.reset_search_mode();
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, CurrentMachine.name, MACHINE_NAME_LEN );
if ( t.get_next_key_match(1, WITH_LOCK) )
    {
    my_monitor_flags = BITSETYPE( t.get_long(MACHSET_MONITOR_FLAGS_OFFSET) );
    my_monitor_flags |= mask;
    t.put_long( MACHSET_MONITOR_FLAGS_OFFSET, my_monitor_flags, BITSETYPE_LEN );
    t.rec_update();
    t.unlock_record();
    }
t.close();
ShutdownStatusListbox.add( TEXT("Poking the monitor topic...") );
MyMonitorSetupCount++;
poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentMachine.name );
}

/***********************************************************************
*                        LOAD_ONE_MACHINE_ONLY                         *
***********************************************************************/
void load_one_machine_only( HWND hWnd )
{
const BITSETYPE mask = ~MA_MONITORING_ON;
STRING_CLASS m;
STRING_CLASS c;
NAME_CLASS   s;
DB_TABLE     t;
BITSETYPE    my_monitor_flags;
LISTBOX_CLASS lb;
TCHAR * cp;

lb.init( hWnd, CHOOSE_MACHINE_LB );
c = lb.selected_text();
if ( !c.isempty() )
    {
    cp = findchar( SpaceChar, c.text() );
    if ( cp )
        {
        *cp = NullChar;
        cp += 2;
        m = cp;
        ComputerName = c;
        refresh_current_machine( m.text() );
        m = TEXT( "Computer = " );
        m += ComputerName;
        m += TEXT( "  Machine = " );
        m += CurrentMachine.name;
        m += TEXT( "  Part = " );
        m += CurrentPart.name;
        IsRemoteMonitor = TRUE;
        if ( CurrentMachine.monitor_flags & MA_MONITORING_ON )
            {
            my_monitor_flags = CurrentMachine.monitor_flags;
            my_monitor_flags &= mask;
            s.get_machset_file_name( ComputerName.text() );
            t.open( s.text(), MACHSET_RECLEN, PWL );
            t.reset_search_mode();
            t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, CurrentMachine.name, MACHINE_NAME_LEN );
            if ( t.get_next_key_match(1, WITH_LOCK) )
                {
                t.put_long( MACHSET_MONITOR_FLAGS_OFFSET, my_monitor_flags, BITSETYPE_LEN );
                }
            t.rec_update();
            t.unlock_record();
            t.close();
            }
        }
    }
}

/***********************************************************************
*                CHOOSE_MONITOR_MACHINE_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK choose_monitor_machine_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        fill_machine_listbox( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                load_one_machine_only( hWnd );
                EndDialog( hWnd, TRUE );
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, FALSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                SEND_OUTPUT_BITS_TO_CURRENT_MACHINE                   *
***********************************************************************/
void send_output_bits_to_current_machine( unsigned bits, BOOLEAN set_bits )
{
static TCHAR OnString[]  = TEXT("OO1=" );
static TCHAR OffString[] = TEXT("OO1=~" );
TCHAR s[30];
TCHAR * cp;

if ( set_bits )
    cp = copystring( s, OnString );
else
    cp = copystring( s, OffString );

cp = copystring( cp, ultohex((unsigned long) bits) );
send_command_to_current_machine( s, true );
}

static TCHAR HiresSensorConfigFile[] = TEXT( "hires_sensor_test_config.csv" );
static int32 HIRES_SENSOR_START_SHOT_WIRE_INDEX  = 0;
static int32 HIRES_SENSOR_FOLLOW_THRU_WIRE_INDEX = 1;
static int32 HIRES_SENSOR_RETRACT_WIRE_INDEX     = 2;
static int32 HIRES_SENSOR_CYCLE_TIME_INDEX       = 3;
static int32 HIRES_SENSOR_FOLLOW_THRU_TIME_INDEX = 4;
static int32 HIRES_SENSOR_RETRACT_TIME_INDEX     = 5;
static int32 HIRES_SENSOR_RETRACT_END_TIME_INDEX = 6;
static int32 HIRES_SENSOR_SETPOINT_COUNT         = 7;

/***********************************************************************
*                        STOP_HIRES_SENSOR_TEST                        *
***********************************************************************/
static void stop_hires_sensor_test()
{
unsigned bit;

CurrentHiresSensorPeriod = -1;

bit = wirebit( HiresSensorStartShotWire );
bit |= wirebit( HiresSensorFollowThruWire );
bit |= wirebit( HiresSensorRetractWire );
if ( bit > 0 )
    send_output_bits_to_current_machine( bit, FALSE );
}

/***********************************************************************
*                      DO_HIRES_SENSOR_TEST_TIMER                      *
***********************************************************************/
static void do_hires_sensor_test_timer( HWND w )
{
int      wire_number;
unsigned bit;
int      ms;

if ( CurrentHiresSensorPeriod < NO_SHOT_PERIOD )
    return;

CurrentHiresSensorPeriod++;
if ( CurrentHiresSensorPeriod > END_OF_SHOT_PERIOD )
    CurrentHiresSensorPeriod = START_SHOT_PERIOD;

/*
-------------------------
Turn off the current wire
------------------------- */
wire_number = NO_WIRE;
switch ( CurrentHiresSensorPeriod )
    {
    case FOLLOW_THRU_PERIOD:
        wire_number = HiresSensorStartShotWire;
        break;
    case RETRACT_PERIOD:
        wire_number = HiresSensorFollowThruWire;
        break;
    case END_OF_SHOT_PERIOD:
        wire_number = HiresSensorRetractWire;
        break;
    }

if ( wire_number != NO_WIRE )
    {
    bit = wirebit( wire_number );
    send_output_bits_to_current_machine( bit, FALSE );
    }

/*
--------------------
Turn on the new wire
-------------------- */
wire_number = NO_WIRE;
switch ( CurrentHiresSensorPeriod )
    {
    case START_SHOT_PERIOD:
        wire_number = HiresSensorStartShotWire;
        break;
    case FOLLOW_THRU_PERIOD:
        wire_number = HiresSensorFollowThruWire;
        break;
    case RETRACT_PERIOD:
        wire_number = HiresSensorRetractWire;
        break;
    }

if ( wire_number != NO_WIRE )
    {
    bit = wirebit( wire_number );
    send_output_bits_to_current_machine( bit, TRUE );
    }

ms = HiresSensorPeriodMs[CurrentHiresSensorPeriod];
if ( ms > 0 )
    SetTimer( MainWindow.handle(), HIRES_SENSOR_TEST_TIMER_ID, (UINT) ms, 0 );
else
    CurrentHiresSensorPeriod = -1;
}

/***********************************************************************
*                    SET_GLOBALS_FROM_EBOXES                           *
***********************************************************************/
static void set_globals_from_eboxes( HWND w )
{
int          end_ms;
int          start_ms;
STRING_CLASS s;

s.get_text( w, FOLLOW_THRU_TIME_EBOX );
start_ms = s.int_value();
HiresSensorPeriodMs[START_SHOT_PERIOD]  = start_ms * 1000;

s.get_text( w, RETRACT_TIME_EBOX );
end_ms = s.int_value();

HiresSensorPeriodMs[FOLLOW_THRU_PERIOD] = (end_ms - start_ms) * 1000;
start_ms = end_ms;
s.get_text( w, RETRACT_END_TIME_EBOX );
end_ms = s.int_value();
HiresSensorPeriodMs[RETRACT_PERIOD]     = (end_ms - start_ms) * 1000;

start_ms = end_ms;
s.get_text( w, CYCLE_TIME_EBOX );
end_ms = s.int_value();
HiresSensorPeriodMs[END_OF_SHOT_PERIOD] = (end_ms - start_ms) * 1000;

s.get_text( w, START_SHOT_WIRE_EBOX );
HiresSensorStartShotWire  = s.int_value();

s.get_text( w, FOLLOW_THRU_WIRE_EBOX );
HiresSensorFollowThruWire = s.int_value();

s.get_text( w, RETRACT_WIRE_EBOX );
HiresSensorRetractWire = s.int_value();
}

/***********************************************************************
*                    GET_HIRES_SENSOR_TEST_CONFIG                      *
***********************************************************************/
void get_hires_sensor_test_config( HWND hWnd )
{
int            i;
int            id;
NAME_CLASS     s;
SETPOINT_CLASS sp;

s.get_exe_dir_file_name( ComputerName.text(), HiresSensorConfigFile );
if ( !sp.get(s.text()) )
    {
    sp.setsize( HIRES_SENSOR_SETPOINT_COUNT );
    for ( i=0; i<HIRES_SENSOR_SETPOINT_COUNT; i++ )
        sp[i].value = 0;
    }

id = START_SHOT_WIRE_EBOX;
for ( i=0; i<HIRES_SENSOR_SETPOINT_COUNT; i++ )
    sp[i].value.set_text( hWnd, id++ );

set_globals_from_eboxes( hWnd );
}

/***********************************************************************
*                    PUT_HIRES_SENSOR_TEST_CONFIG                      *
***********************************************************************/
void put_hires_sensor_test_config( HWND hWnd )
{
static TCHAR * Description[] = {
    { TEXT( "Start Shot Wire" ) },
    { TEXT( "Follow Thru Wire" ) },
    { TEXT( "Retract Wire" ) },
    { TEXT( "Cycle Time" ) },
    { TEXT( "Follow Thru Time" ) },
    { TEXT( "Retract Time" ) },
    { TEXT( "Retract End Time" ) }
    };
int i;
int id;
NAME_CLASS     s;
SETPOINT_CLASS sp;

sp.setsize( HIRES_SENSOR_SETPOINT_COUNT );
id = START_SHOT_WIRE_EBOX;
for ( i=0; i<HIRES_SENSOR_SETPOINT_COUNT; i++ )
    {
    sp[i].value.get_text( hWnd, id++ );
    sp[i].desc = Description[i];
    }

s.get_exe_dir_file_name( ComputerName.text(), HiresSensorConfigFile );
sp.put( s.text() );
}

/***********************************************************************
*                    HIRES_SENSOR_TEST_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK hires_sensor_test_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        HiresSensorTestDialog = hWnd;
        get_hires_sensor_test_config( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                put_hires_sensor_test_config( hWnd );
                set_globals_from_eboxes( hWnd );
                if ( CurrentHiresSensorPeriod < NO_SHOT_PERIOD )
                    {
                    CurrentHiresSensorPeriod = NO_SHOT_PERIOD;
                    do_hires_sensor_test_timer( hWnd );
                    }
                return TRUE;

            case IDCANCEL:
                stop_hires_sensor_test();
                HiresSensorTestDialog.hide();
                return TRUE;
            }
        break;

    case WM_CLOSE:
        HiresSensorTestDialog.hide();
        return FALSE;

    case WM_DESTROY:
        HiresSensorTestDialog = (HWND) 0;
        break;
    }

return FALSE;
}

/***********************************************************************
*                         SAVE_NULL_VALUE                              *
***********************************************************************/
static void save_null_value()
{
STRING_CLASS filename;

temp_message( resource_string(SAVING_STRING) );

filename = ftii_global_settings_name( ComputerName.text(), CurrentMachine.name );
FtiiAutoNull.put_into( filename.text() );

set_text( GlobalParameterDialog.control(NULL_DAC_EDITBOX), int32toasc(FtiiAutoNull[0].ival()) );
}

/***********************************************************************
*                        SEND_AUTO_NULL_COMMAND                        *
***********************************************************************/
static void send_auto_null_command()
{
send_command_to_current_machine( TEXT("OC6=1"),  true );
NullStatusMessage.set_title( resource_string(AUTO_NULL_BEGIN_STRING) );
ManualInfoTbox.hide();
}

/***********************************************************************
*                      SEND_MANUAL_NULL_COMMAND                        *
***********************************************************************/
static void send_manual_null_command()
{
ManualNullInProgress = true;
send_command_to_current_machine( TEXT("V350=0"), false );
send_command_to_current_machine( TEXT("VF"), true );
Sleep( 100 );
send_command_to_current_machine( TEXT("OC5=1"),  true );
NullStatusMessage.set_title( resource_string(MANUAL_NULLING_STRING) );
ManualInfoTbox.show();

FtiiAutoNull[0].set( NULL_DAC_OFFSET_VN, 0 );
save_null_value();
}

/***********************************************************************
*                        DO_NEW_SET_NULL_VALUES                        *
***********************************************************************/
static void do_new_set_null_values()
{
static int          i;
static HWND         w;
static unsigned     x;
static STRING_CLASS ns;
static bool         firstime = true;

i = current_lvdt_feedback_index( DAC_1 );
if ( firstime || AnalogValue[i] != CurrentAnalogValue[i] )
    {
    AnalogValue[i] = CurrentAnalogValue[i];

    x = AnalogValue[i];
    ns = ascii_dac_volts_3( x );
    w  = SetNullDialog.control( NULL_VOLTS_TBOX );
    ns.set_text( w );
    }

firstime = false;
}

/***********************************************************************
*                      DO_NEW_SET_NULL_ASCII_DATA                      *
***********************************************************************/
void do_new_set_null_ascii_data( LPARAM lParam )
{
static const int UNUSABLE_NULL_LEVEL = 0;
static const int AUTO_NULL_FAIL      = 1;
static const int AUTO_NULL_SUCCESS   = 2;
static const int NULL_ALREADY_GOOD   = 3;

struct MY_ENTRY {
    TCHAR * s;
    UINT    id;
    };

static MY_ENTRY response[] = {
    { TEXT("BEGINNING_NULL"),    UNUSABLE_NULL_LEVEL_STRING },    /* BEGINNING_NULL_EXCEEDS_ALLOWABLE_RANGE  */
    { TEXT("AUTO_NULL_FAILED"),  AUTO_NULL_FAIL_STRING      },    /* AUTO_NULL_FAILED_UNABLE_TO_CORRECT      */
    { TEXT("AUTO_NULL_SUCCESS"), AUTO_NULL_SUCCESS_STRING   },    /* AUTO_NULL_SUCCESS                       */
    { TEXT("NULL_ALREADY_GOOD"), NULL_ALREADY_GOOD_STRING   }     /* NULL_ALREADY_GOOD_NO_NEED_TO_CORRECT    */
    };

int nof_responses = sizeof(response)/sizeof(MY_ENTRY);

TCHAR * cp;
TCHAR * sorc;
int     i;

sorc = (TCHAR *) lParam;
if ( sorc )
    {
    for( i=0; i<nof_responses; i++ )
        {
        cp = findstring( response[i].s, sorc );
        if ( cp )
            {
            switch ( i )
                {
                case UNUSABLE_NULL_LEVEL:
                    break;

                case AUTO_NULL_FAIL:
                    break;

                case AUTO_NULL_SUCCESS:
                    cp = findchar( TEXT('#'), cp );
                    if ( cp )
                        {
                        cp++;
                        FtiiAutoNull[0] = cp;
                        save_null_value();
                        }
                    break;

                case NULL_ALREADY_GOOD:
                    break;
                }

            NullStatusMessage.set_title( resource_string(response[i].id) );
            break;
            }
        }

    delete[] sorc;
    }
}
/***********************************************************************
*                   POSITION_SHUTDOWN_STATUS_DIALOG                    *
***********************************************************************/
static void position_shutdown_status_dialog( HWND hWnd )
{
WINDOW_CLASS w;
RECTANGLE_CLASS rp;
RECTANGLE_CLASS r;

w = hWnd;
w.get_move_rect( r );
MainWindow.get_client_rect( rp );

if ( r.height() < rp.height() )
    {
    r.bottom = r.top + rp.height();
    w.move( r );
    }
}

/***********************************************************************
*                        SHUTDOWN_STATUS_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK shutdown_status_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
if ( msg == WM_INITDIALOG )
    position_shutdown_status_dialog( hWnd );
return FALSE;
}

/***********************************************************************
*                          SET_NULL_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK set_null_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SetNullDialog = hWnd;
        NullStatusMessage = SetNullDialog.control( SET_NULL_MSG_TBOX );
        ManualInfoTbox    = SetNullDialog.control( MANUAL_INSTRUCTIONS_TBOX );
        NullStatusMessage.set_title( EmptyString );

        position_button_child( SetNullDialog );
        SetNullDialog.hide();
        FtiiAutoNull[0].set( NULL_DAC_OFFSET_VN, 0 );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case AUTO_NULL_PB:
                send_auto_null_command();
                return TRUE;

            case MANUAL_NULL_PB:
                send_manual_null_command();
                return TRUE;
            }
        break;

    case WM_NEW_DATA:
        do_new_set_null_values();
        return TRUE;

    case WM_NEW_RT_DATA:
        do_new_set_null_ascii_data( lParam );
        return TRUE;
    }

return FALSE;

}
/***********************************************************************
*               POSITON_CHOOSE_VALVE_TYPE_DIALOG_PROC                  *
***********************************************************************/
static void position_choose_valve_type_dialog( HWND sorc )
{
WINDOW_CLASS parent;
WINDOW_CLASS w;
RECTANGLE_CLASS rp;
RECTANGLE_CLASS r;

w = sorc;
parent = GetParent( sorc );
parent.getrect( rp );
w.getrect( r );
rp.left = rp.right - r.width();
rp.top  = rp.bottom - r.height();
w.move( rp );
}

/***********************************************************************
*                       SHOW_REVISION_D_CONTROLS                       *
***********************************************************************/
static void show_revision_d_controls()
{
static UINT id[] = {
    LVDT_BOARD_LEVEL_OFFSET_EBOX,
    LVDT_DIGITAL_POT_EBOX,
    LVDT_DIGITAL_POT_SPIN,
    AMP_1_RADIO,
    AMP_2_RADIO,
    OFFSET_VOLTAGE_STATIC,
    OFFSET_VOLT_RANGE_STATIC,
    SPAN_STATIC,
    SPAN_RANGE_STATIC
    };
const int nof_ids = sizeof(id)/sizeof(int);

WINDOW_CLASS w;
int          i;
bool         is_visible;

if ( HasDigitalServoAmp )
    is_visible = true;
else
    is_visible = false;

for ( i=0; i<nof_ids; i++ )
    {
    w = SetDacDialog.control( id[i] );
    w.show( is_visible );
    }
}

/***********************************************************************
*                        REFRESH_SET_DAC_DIALOG                        *
***********************************************************************/
static void refresh_set_dac_dialog()
{
WINDOW_CLASS w;
int          i;
unsigned     u;
BOOLEAN      is_amp_1;
double       d;
STRING_CLASS s;

show_revision_d_controls();

if ( HasDigitalServoAmp )
    {
    is_amp_1 = is_checked( SetDacDialog.handle(), AMP_1_RADIO );

    if ( is_amp_1 )
        u = digital_pot_setting( AMP_1_LVDT_GAIN );
    else
        u = digital_pot_setting( AMP_2_LVDT_GAIN );

    w = SetDacDialog.control( LVDT_DIGITAL_POT_SPIN );
    w.send( UDM_SETPOS,   0, MAKELPARAM((WORD) u, (WORD) 0) );

    if ( is_amp_1 )
        i = LVDT_1_BOARD_LEVEL_OFFSET_INDEX;
    else
        i = LVDT_2_BOARD_LEVEL_OFFSET_INDEX;

    d = CurrentMachineVars[i].realval();
    w = SetDacDialog.control( LVDT_BOARD_LEVEL_OFFSET_EBOX );
    w.set_title( ascii_double(d) );
    }
}

/***********************************************************************
*                         INIT_SET_DAC_DIALOG                          *
***********************************************************************/
static void init_set_dac_dialog()
{
static UDACCEL accel = { 1, 1 };
WINDOW_CLASS  w;

w = SetDacDialog.control( DAC_COMMAND_THERM );
w.send( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 20000) );

w = SetDacDialog.control( COMMAND_VOLTS_THERM );
w.send( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

w = SetDacDialog.control( LVDT_FEEDBACK_THERM );
w.send( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

w = SetDacDialog.control( LVDT_RAW_FEEDBACK_THERM );
w.send( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

CheckRadioButton( SetDacDialog.handle(), AMP_1_RADIO, AMP_2_RADIO, AMP_1_RADIO );

w = SetDacDialog.control( LVDT_DIGITAL_POT_SPIN );
w.send( UDM_SETRANGE, 0, MAKELPARAM((WORD) 127, (WORD) 0) );
w.send( UDM_SETACCEL, 0, (LPARAM) &accel );
w.send( UDM_SETPOS,   0, MAKELPARAM((WORD) 0, (WORD) 0) );

show_revision_d_controls();
}

/***********************************************************************
*                        DO_NEW_SET_DAC_VALUES                         *
***********************************************************************/
static void do_new_set_dac_values()
{
static STRING_CLASS ns;
static STRING_CLASS os;
static WINDOW_CLASS wc;
static HWND         w;
static int          i;
static double       d;
static unsigned     x;
static bool         should_be_visible;
static bool         firstime = true;

BOOLEAN is_amp_1;
int     mydac;

is_amp_1 = is_checked( SetDacDialog.control(AMP_1_RADIO) );
mydac = 0;
if ( !is_amp_1 )
    mydac = 2;

w  = SetDacDialog.control( DAC_COMMAND_TBOX );
i  = CurrentOpStatus.dac[mydac];
d = (double) i;
d /= 1000.0;
d = round( d, 0.01 );
ns = ascii_double( d );
os.get_text( w );
if ( firstime || os != ns )
    {
    ns.set_text( w );
    w = SetDacDialog.control( DAC_COMMAND_THERM );
    i += 10000;
    PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
    }

i = current_measured_command_index( mydac );
if ( firstime || AnalogValue[i] != CurrentAnalogValue[i] )
    {
    AnalogValue[i] = CurrentAnalogValue[i];
    x = AnalogValue[i];
    ns = ascii_dac_volts( x );
    w  = SetDacDialog.control( COMMAND_VOLTS_TBOX );
    ns.set_text( w );

    w = SetDacDialog.control( COMMAND_VOLTS_THERM );
    PostMessage( w, PBM_SETPOS, (WPARAM) x, 0L );
    }

i = current_lvdt_feedback_index( mydac );
if ( firstime || AnalogValue[i] != CurrentAnalogValue[i] )
    {
    AnalogValue[i] = CurrentAnalogValue[i];

    x = AnalogValue[i];
    ns = ascii_dac_volts( x );
    w  = SetDacDialog.control( LVDT_FEEDBACK_TBOX );
    ns.set_text( w );

    w = SetDacDialog.control( LVDT_FEEDBACK_THERM );
    PostMessage( w, PBM_SETPOS, (WPARAM) x, 0L );
    }

if ( HasDigitalServoAmp )
    {
    i = current_raw_lvdt_feedback_index( mydac );
    if ( firstime || AnalogValue[i] != CurrentAnalogValue[i] )
        {
        AnalogValue[i] = CurrentAnalogValue[i];

        x = AnalogValue[i];
        ns = ascii_dac_volts( x );
        w  = SetDacDialog.control( LVDT_RAW_FEEDBACK_TBOX );
        ns.set_text( w );

        w = SetDacDialog.control( LVDT_RAW_FEEDBACK_THERM );
        PostMessage( w, PBM_SETPOS, (WPARAM) x, 0L );
        }
    }

firstime = false;
}

/***********************************************************************
*                     SEND_LVDT_DIGITAL_POT_COMMAND                    *
***********************************************************************/
static void send_lvdt_digital_pot_command( BOOLEAN have_enter_key )
{
static TCHAR amp_1_cmd[] = TEXT("ODP1=");
static TCHAR amp_2_cmd[] = TEXT("ODP3=");
STRING_CLASS s;
STRING_CLASS cmd;
int          i;
unsigned     u;

s.get_text( SetDacDialog.control(LVDT_DIGITAL_POT_EBOX) );
if ( is_checked(SetDacDialog.control(AMP_1_RADIO)) )
    {
    cmd = amp_1_cmd;
    i = AMP_1_LVDT_GAIN;
    }
else
    {
    cmd = amp_2_cmd;
    i = AMP_2_LVDT_GAIN;
    }
cmd += s;

if ( have_enter_key )
    temp_message( cmd.text(), 1000 );
send_command_to_current_machine( cmd.text(), true );

u = CurrentMachineVars[DIGITAL_POTS_INDEX].uval();
put_byte( u, s.uint_value(), i );
CurrentMachineVars[DIGITAL_POTS_INDEX] = u;
save_machine_vars();
}

/***********************************************************************
*                         SEND_SET_DAC_COMMAND                         *
***********************************************************************/
void send_set_dac_command( int id )
{
static TCHAR amp_1_cmd[] = TEXT( "V361=" );
static TCHAR amp_2_cmd[] = TEXT( "V362=" );

STRING_CLASS s;
double   d;
double   copy;
int      i;
int      vi;
BOOLEAN  is_amp_1;
HWND     w;
HWND     control_with_focus;

is_amp_1 = is_checked( SetDacDialog.control(AMP_1_RADIO) );

control_with_focus = GetFocus();
if ( id == SEND_TO_BOARD_PB )
    {
    if ( control_with_focus == SetDacDialog.control(LVDT_DIGITAL_POT_EBOX) )
        {
        send_lvdt_digital_pot_command( TRUE );
        return;
        }
    else
        {
        w = SetDacDialog.control( LVDT_BOARD_LEVEL_OFFSET_EBOX );
        if ( w == control_with_focus )
            {
            s.get_text( w );
            d = s.real_value();
            copy = d;
            d *= 1000.0;
            d = round( d, 1.0 );
            i = int( d );
            if ( is_amp_1 )
                {
                s = amp_1_cmd;
                vi = LVDT_1_BOARD_LEVEL_OFFSET_INDEX;
                }
            else
                {
                s = amp_2_cmd;
                vi = LVDT_2_BOARD_LEVEL_OFFSET_INDEX;
                }
            s += i;
            temp_message( s.text(), 1000 );
            send_command_to_current_machine( s.text(), true );

            CurrentMachineVars[vi] = copy;
            save_machine_vars();
            return;
            }
        }
    }

s.get_text( SetDacDialog.control(DAC_VOLTS_EBOX) );
if ( s.isempty() )
    return;

d = s.real_value();
d *= 1000.0;
d = round( d, 1.0 );
i = int( d );

if ( is_amp_1 )
    s = TEXT( "OD1=V" );
else
    s = TEXT( "OD3=V" );
s += i;

temp_message( s.text(), 1000 );
send_command_to_current_machine( s.text(), true );
}

/***********************************************************************
*                          SEND_TEST_MESSAGE                           *
***********************************************************************/
static void send_test_message( int id )
{
static int last_id = 0;

/*
---------------------------------------------------------
When you press a down arrow I get two identical messages.
Ignore the second one
--------------------------------------------------------- */
if ( id == last_id )
    {
    last_id = 0;
    return;
    }

last_id = id;

switch( id )
    {
    case TEST_7_RADIO:
        send_command_to_current_machine( TEXT("OD1=V0"),  false );
        send_command_to_current_machine( TEXT("OD3=V0"),  false );
        send_command_to_current_machine( TEXT("OD2=V0"),  false );
        send_command_to_current_machine( TEXT("OD4=V0"),  false );
        send_command_to_current_machine( TEXT("ODP1=0"),  false );
        send_command_to_current_machine( TEXT("ODP3=0"),  false );
        send_command_to_current_machine( TEXT("ODP2=0"),  false );
        send_command_to_current_machine( TEXT("ODP4=0"),  false );
        send_command_to_current_machine( TEXT("OC12=0"),  true );
        break;

    case TEST_8A_RADIO:
        send_command_to_current_machine( TEXT("OD1=V-5000"),  false );
        send_command_to_current_machine( TEXT("OD3=V-5000"),  true );
        break;

    case TEST_8B_RADIO:
        send_command_to_current_machine( TEXT("OD1=V5000"),  false );
        send_command_to_current_machine( TEXT("OD3=V5000"),  true );
        break;

    case TEST_9A_RADIO:
        send_command_to_current_machine( TEXT("OC10=1"),  false );
        send_command_to_current_machine( TEXT("OC11=1"),  true );
        break;

    case TEST_9B_RADIO:
        send_command_to_current_machine( TEXT("OC10=0"),  false );
        send_command_to_current_machine( TEXT("OC11=0"),  true );
        break;

    case TEST_10A_RADIO:
        send_command_to_current_machine( TEXT("OC12=1"),  true );
        break;

    case TEST_10B_RADIO:
        send_command_to_current_machine( TEXT("OC12=0"),  true );
        break;

    case TEST_11A_RADIO:
        send_command_to_current_machine( TEXT("ODP2=127"),  false );
        send_command_to_current_machine( TEXT("ODP4=127"),  true );
        break;

    case TEST_11B_RADIO:
        send_command_to_current_machine( TEXT("ODP2=0"),  false );
        send_command_to_current_machine( TEXT("ODP4=0"),  true );
        break;

    case TEST_11C_RADIO:
        send_command_to_current_machine( TEXT("OD1=V0"),  false );
        send_command_to_current_machine( TEXT("OD3=V0"),  true );
        break;

    case TEST_18A_RADIO:
        send_command_to_current_machine( TEXT("ODP1=14"),  false );
        send_command_to_current_machine( TEXT("ODP3=14"),  true );
        break;

    case TEST_18B_RADIO:
        send_command_to_current_machine( TEXT("ODP1=41"),  false );
        send_command_to_current_machine( TEXT("ODP3=41"),  true );
        break;

    case TEST_18C_RADIO:
        send_command_to_current_machine( TEXT("ODP1=96"),  false );
        send_command_to_current_machine( TEXT("ODP3=96"),  true );
        break;

    case TEST_19_RADIO:
        send_command_to_current_machine( TEXT("OD2=V5000"),  false );
        send_command_to_current_machine( TEXT("OD4=V5000"),  true );
        break;
    }
}

/***********************************************************************
*                        FT2_TEST_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK ft2_test_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                Ft2TestDialog.hide();
                return TRUE;

            case TEST_7_RADIO:
            case TEST_8A_RADIO:
            case TEST_8B_RADIO:
            case TEST_9A_RADIO:
            case TEST_9B_RADIO:
            case TEST_10A_RADIO:
            case TEST_10B_RADIO:
            case TEST_11A_RADIO:
            case TEST_11B_RADIO:
            case TEST_11C_RADIO:
            case TEST_18A_RADIO:
            case TEST_18B_RADIO:
            case TEST_18C_RADIO:
            case TEST_19_RADIO:
               if ( cmd == BN_CLICKED )
                    send_test_message( id );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        Ft2TestDialog.hide();
        return FALSE;
    }

return FALSE;
}

/***********************************************************************
*                           SET_DAC_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK set_dac_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;
static int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SetDacDialog = hWnd;
        position_button_child( SetDacDialog );
        init_set_dac_dialog();
        SetDacDialog.hide();
        SetDacDialog.send( WM_DBINIT );
        break;

    case WM_DBINIT:
        if ( UpdatingSetDacScreen )
            {
            UpdatingSetDacScreen = FALSE;
            }
        else
            {
            UpdatingSetDacScreen = TRUE;
            refresh_set_dac_dialog();
            SetDacDialog.send( WM_DBINIT );
            }
        return TRUE;

    case WM_NEW_DATA:
        do_new_set_dac_values();
        return TRUE;

    case WM_VSCROLL:
        if ( HWND(lParam) == SetDacDialog.control(LVDT_DIGITAL_POT_SPIN) && !UpdatingSetDacScreen )
            {
            if ( id == SB_ENDSCROLL )
                {
                /*
                --------------------------------------------------------------------------------------------
                The SB_ENDSCROLL message is only sent when the mouse button is released. Sending the command
                at this time prevents me from banging away at the st2 board.
                -------------------------------------------------------------------------------------------- */
                send_lvdt_digital_pot_command( FALSE );
                return TRUE;
                }
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            /*
            --------------------------------------------------
            case CHANGE_VALVE_TYPE_PB:
                DialogBox(
                    MainInstance,
                    TEXT( "CHOOSE_VALVE_TYPE_DIALOG" ),
                    hWnd,
                    (DLGPROC) choose_valve_type_dialog_proc );
                return TRUE;
            -------------------------------------------------- */

//            case LVDT_DIGITAL_POT_EBOX:
//                if ( cmd == EN_CHANGE && !UpdatingSetDacScreen )
//                    send_lvdt_digital_pot_command();
 //               return TRUE;

            case AMP_1_RADIO:
            case AMP_2_RADIO:
                if ( !UpdatingSetDacScreen && HasDigitalServoAmp )
                    {
                    if ( id == AMP_1_RADIO )
                        {
                        send_command_to_current_machine( CloseValve_2_Loop,  true );
                        send_command_to_current_machine( OpenValve_1_Loop,  true );
                        }
                    else
                        {
                        send_command_to_current_machine( CloseValve_1_Loop,  true );
                        send_command_to_current_machine( OpenValve_2_Loop,  true );
                        }
                    }
                refresh_set_dac_dialog();
                return TRUE;

            case SEND_TO_BOARD_PB:
                send_set_dac_command( id );
                return TRUE;

            case PLUS_5_VOLTS_PB:
                set_text( hWnd, DAC_VOLTS_EBOX, TEXT("5.0") );
                send_set_dac_command( id );
                return TRUE;

            case MINUS_5_VOLTS_PB:
                set_text( hWnd, DAC_VOLTS_EBOX, TEXT("-5.0") );
                send_set_dac_command( id );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        START_IF_OK_TO_DEACTIVATE                     *
***********************************************************************/
void start_if_ok_to_deactivate( WINDOW_CLASS & w )
{
BOOL ok_to_deactivate;

ok_to_deactivate = DialogBox( MainInstance,
                              TEXT("DEACTIVATE_CONTROL_DIALOG"),
                              ButtonDialog.handle(),
                              (DLGPROC) deactivate_control_dialog_proc );

if ( ok_to_deactivate )
    set_current_window( w );
}

/***********************************************************************
*                           MV_FROM_PERCENT                            *
*    Converts to signed mv from percent of range [-10v, 10v]           *
***********************************************************************/
static int mv_from_percent( double sorc )
{
sorc *= 100.0;
return (int) round( sorc, 1.0 );
}

/***********************************************************************
*                           MV_FROM_PERCENT                            *
*    Converts to signed mv from percent of range [-10v, 10v]           *
***********************************************************************/
static int mv_from_percent( STRING_CLASS & sorc )
{
double   x;
int      i;

if ( sorc.contains(PeriodChar) )
    {
    x = sorc.real_value();
    x *= 100.0;
    i = (int) round( x, 1.0 );
    }
else
    {
    i = sorc.int_value();
    i *= 100;
    }

return i;
}

/***********************************************************************
*                 REPLACE_VOLTS_FROM_PERCENT_VALUE                     *
*           Volts are saved as volts*1000 which == %*100               *
***********************************************************************/
STRING_CLASS & replace_volts_from_percent_value( TCHAR * sorc, float new_value )
{
double   x;
int      i;

x = (double) new_value;
x *= 100.0;
i = (int) round( x, 1.0 );

return replace_value( sorc, i );
}

/***********************************************************************
*                       SAVE_GLOBAL_PARAMETERS                         *
***********************************************************************/
static void save_global_parameters()
{
HWND         w;
int32        i;
TCHAR      * cp;
STRING_CLASS s;
INI_CLASS    ini;

w = GlobalParameterDialog.handle();

s = ftii_editor_settings_name( ComputerName.text(), CurrentMachine.name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

s.get_text( w, MIN_LI_EDITBOX );
ini.put_string( MinLowImpactPercentKey, s.text() );

s.get_text( w, LS1_DESCRIPTION_EDITBOX );
ini.put_string( LS1DescriptionKey, s.text() );

s.get_text( w, LS2_DESCRIPTION_EDITBOX );
ini.put_string( LS2DescriptionKey, s.text() );

s.get_text( w, LS3_DESCRIPTION_EDITBOX );
ini.put_string( LS3DescriptionKey, s.text() );

s.get_text( w, LS4_DESCRIPTION_EDITBOX );
ini.put_string( LS4DescriptionKey, s.text() );

s.get_text( w, LS5_DESCRIPTION_EDITBOX );
ini.put_string( LS5DescriptionKey, s.text() );

s.get_text( w, LS6_DESCRIPTION_EDITBOX );
ini.put_string( LS6DescriptionKey, s.text() );

s.get_text( w, MAX_VELOCITY_EDITBOX );
ini.put_string( MaxVelocityKey, s.text() );

/*
---------------------------------------------------
See which limit switch is used for the vacuum check
--------------------------------------------------- */
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    if ( is_checked(w, VACUUM_1_RADIO + i) )
        break;
    }

if ( i>=MAX_FTII_LIMIT_SWITCHES )
    i = 0;

ini.put_int( VacuumLimitSwitchKey, i );

s.get_text( w, NULL_DAC_EDITBOX );
GlobalParameters[NULL_DAC_INDEX] = s.int_value();

s.get_text( w, LVDT_DAC_EDITBOX );
GlobalParameters[LVDT_DAC_INDEX] = s.int_value();

s.get_text( w, JOG_SHOT_VALVE_EDITBOX );
GlobalParameters[JOG_SHOT_VALVE_INDEX] = mv_from_percent( s );

s.get_text( w, FT_VALVE_EDITBOX );
GlobalParameters[FT_VALVE_INDEX] = mv_from_percent( s );

s.get_text( w, RETRACT_VALVE_EDITBOX );
GlobalParameters[RETRACT_VALVE_INDEX] = mv_from_percent( s );

s.get_text( w, ZERO_SPEED_CHECK_EDITBOX );
GlobalParameters[ZERO_SPEED_CHECK_INDEX] = CurrentPart.x4_from_dist( s.real_value() );

//s.get_text( w, NULL_DAC_2_EDITBOX );
//GlobalParameters[NULL_DAC_2_INDEX] = s.int_value();

//s.get_text( w, LVDT_DAC_2_EDITBOX );
//GlobalParameters[LVDT_DAC_2_INDEX] = s.int_value();

s = ftii_global_settings_name( ComputerName.text(), CurrentMachine.name );
GlobalParameters.put( s.text() );

/*
--------------------------------------
Make text that main window will delete
-------------------------------------- */
cp = maketext( CurrentMachine.name );
MainWindow.send( WM_NEWSETUP, 0, (LPARAM) cp );
}

/***********************************************************************
*                           PERCENT_FROM_MV                            *
*         Converts signed mv to percent of range [-10, 10];            *
***********************************************************************/
static TCHAR * percent_from_mv( int sorc )
{
double d;
d = double( sorc );
d /= 100.0;
return ascii_double( d );
}

/***********************************************************************
*                    FILL_LIMIT_SWITCH_DESCRIPTIONS                    *
* This function duplicates part of the fill_global_parameters          *
* function. This is necessary because the part editor can now change   *
* the descriptions of the first four limit switches and so I need      *
* to reload these if the part has changed.                             *
***********************************************************************/
static void fill_limit_switch_descriptions()
{
INI_CLASS    ini;
STRING_CLASS s;
HWND         w;

w = GlobalParameterDialog.handle();

if ( !w )
    return;

s = ftii_editor_settings_name( ComputerName.text(), CurrentMachine.name );
if ( file_exists(s.text()) )
    {
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );

    s = ini.get_string( LS1DescriptionKey );
    if ( s != UNKNOWN )
        s.set_text( w, LS1_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS2DescriptionKey );
    if ( s != UNKNOWN )
        s.set_text( w, LS2_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS3DescriptionKey );
    if ( s != UNKNOWN )
        s.set_text( w, LS3_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS4DescriptionKey );
    if ( s != UNKNOWN )
        s.set_text( w, LS4_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS5DescriptionKey );
    if ( s != UNKNOWN )
        s.set_text( w, LS5_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS6DescriptionKey );
    if ( s != UNKNOWN )
        s.set_text( w, LS6_DESCRIPTION_EDITBOX );
    }
}

/***********************************************************************
*                       FILL_GLOBAL_PARAMETERS                         *
***********************************************************************/
static void fill_global_parameters()
{
static int vars[] = { NULL_DAC_OFFSET_VN, LVDT_DAC_OFFSET_VN, JOG_SHOT_SETTING_VN,
                      FT_SETTING_VN,      RETRACT_SETTING_VN, ZERO_SPEED_CHECK_VN
                      //NULL_DAC_2_OFFSET_VN, LVDT_DAC_2_OFFSET_VN
                      };
const int nof_vars = sizeof(vars)/sizeof(int);

INI_CLASS    ini;
SURETRAK_PARAM_CLASS SureTrakParameters;
HWND         w;
STRING_CLASS s;
int          i;
int          limit_switch_number;

w = GlobalParameterDialog.handle();

if ( !w )
    return;

SureTrakParameters.find( ComputerName.text() );

s = ftii_editor_settings_name( ComputerName.text(), CurrentMachine.name );
if ( file_exists(s.text()) )
    {

    ini.set_file( s.text() );
    ini.set_section( ConfigSection );

    s = ini.get_string( MinLowImpactPercentKey );
    if ( s == UNKNOWN )
        s = EmptyString;
    s.set_text( w, MIN_LI_EDITBOX );

    s = ini.get_string( LS1DescriptionKey );
    if ( s == UNKNOWN )
        s = EmptyString;
    if ( s.isempty() )
        s = SureTrakParameters.limit_switch_desc( 0 );
    s.set_text( w, LS1_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS2DescriptionKey );
    if ( s == UNKNOWN )
        s = EmptyString;
    if ( s.isempty() )
        s = SureTrakParameters.limit_switch_desc( 1 );
    s.set_text( w, LS2_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS3DescriptionKey );
    if ( s == UNKNOWN )
        s = EmptyString;
    if ( s.isempty() )
        s = SureTrakParameters.limit_switch_desc( 2 );
    s.set_text( w, LS3_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS4DescriptionKey );
    if ( s == UNKNOWN )
        s = EmptyString;
    if ( s.isempty() )
        s = SureTrakParameters.limit_switch_desc( 3 );
    s.set_text( w, LS4_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS5DescriptionKey );
    if ( s == UNKNOWN )
        s = EmptyString;
    s.set_text( w, LS5_DESCRIPTION_EDITBOX );

    s = ini.get_string( LS6DescriptionKey );
    if ( s == UNKNOWN )
        s = EmptyString;
    s.set_text( w, LS6_DESCRIPTION_EDITBOX );

    if ( ini.find( MaxVelocityKey) )
        s = ini.get_string();
    else
        s = (double) SureTrakParameters.max_velocity();
    s.set_text( w, MAX_VELOCITY_EDITBOX );

    if ( ini.find(VacuumLimitSwitchKey) )
        limit_switch_number = ini.get_int();
    else
        limit_switch_number = SureTrakParameters.vacuum_limit_switch_number();
    }
else
    {
    set_text( w, MIN_LI_EDITBOX,          ascii_float(SureTrakParameters.min_low_impact_percent()) );
    set_text( w, LS1_DESCRIPTION_EDITBOX, SureTrakParameters.limit_switch_desc(0) );
    set_text( w, LS2_DESCRIPTION_EDITBOX, SureTrakParameters.limit_switch_desc(1) );
    set_text( w, LS3_DESCRIPTION_EDITBOX, SureTrakParameters.limit_switch_desc(2) );
    set_text( w, LS4_DESCRIPTION_EDITBOX, SureTrakParameters.limit_switch_desc(3) );
    set_text( w, MAX_VELOCITY_EDITBOX, ascii_float(SureTrakParameters.max_velocity()) );
    limit_switch_number = SureTrakParameters.vacuum_limit_switch_number();
    }

CheckRadioButton( w, VACUUM_1_RADIO, VACUUM_4_RADIO, VACUUM_1_RADIO + limit_switch_number );

GlobalParameters.upsize( nof_vars );
for ( i=0; i<nof_vars; i++ )
    GlobalParameters[i].set( vars[i], 0 );

s = ftii_global_settings_name( ComputerName.text(), CurrentMachine.name );
if ( file_exists(s.text()) )
    {
    GlobalParameters.get_from( s.text() );
    }
else
    {
    SureTrakParameters.find( ComputerName.text() );
    GlobalParameters[NULL_DAC_INDEX]         = SureTrakParameters.null_dac_offset();
    GlobalParameters[LVDT_DAC_INDEX]         = SureTrakParameters.lvdt_dac_offset();
    GlobalParameters[JOG_SHOT_VALVE_INDEX]   = mv_from_percent( (double) SureTrakParameters.jog_valve_percent() );
    GlobalParameters[FT_VALVE_INDEX]         = mv_from_percent( (double) SureTrakParameters.ft_valve_percent() );
    GlobalParameters[RETRACT_VALVE_INDEX]    = mv_from_percent( (double) SureTrakParameters.retract_valve_percent() );
    GlobalParameters[ZERO_SPEED_CHECK_INDEX] = CurrentPart.x4_from_dist( (double) SureTrakParameters.zero_speed_check_pos() );
    }

set_text( w, NULL_DAC_EDITBOX,       int32toasc((int32) GlobalParameters[NULL_DAC_INDEX].ival()) );
set_text( w, LVDT_DAC_EDITBOX,       int32toasc((int32) GlobalParameters[LVDT_DAC_INDEX].ival()) );
//set_text( w, NULL_DAC_2_EDITBOX,     int32toasc((int32) GlobalParameters[NULL_DAC_2_INDEX].ival()) );
//set_text( w, LVDT_DAC_2_EDITBOX,     int32toasc((int32) GlobalParameters[LVDT_DAC_2_INDEX].ival()) );
set_text( w, JOG_SHOT_VALVE_EDITBOX, percent_from_mv(GlobalParameters[JOG_SHOT_VALVE_INDEX].ival()) );
set_text( w, FT_VALVE_EDITBOX,       percent_from_mv(GlobalParameters[FT_VALVE_INDEX].ival())       );
set_text( w, RETRACT_VALVE_EDITBOX,  percent_from_mv(GlobalParameters[RETRACT_VALVE_INDEX].ival())  );

set_text( w, ZERO_SPEED_CHECK_EDITBOX, ascii_double( CurrentPart.dist_from_x4(GlobalParameters[ZERO_SPEED_CHECK_INDEX].realval())) );
set_text( w, GLOBAL_DIST_UNITS_XBOX, units_name(CurrentPart.distance_units) );
}

/***********************************************************************
*                    GLOBAL_PARAMETER_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK global_parameter_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        GlobalParameterDialog = w;
        position_button_child( GlobalParameterDialog );
        GlobalParameterDialog.hide();
        return TRUE;

    case WM_HELP:
        //gethelp( HELP_CONTEXT, GLOBAL_SURETRAK_PARAMETERS_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_CHANGES_BUTTON:
                temp_message( resource_string(SAVING_STRING) );
                save_global_parameters();
                return TRUE;

            case CANCEL_CHANGES_BUTTON:
                fill_global_parameters();
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                       INIT_CONTROL_MODS_DIALOG                       *
***********************************************************************/
static void init_control_mods_dialog()
{
UsingAltStep = false;
AltStepNum   = TEXT( ".025 " );
AltStepEnd   = TEXT( "_E1.34" );
AltStepVelPrefix = TEXT( "VA1" );
AltStepAccPrefix = TEXT( "AC" );
AltStepPosPrefix = TEXT( "P" );
AltStepVel       = TEXT( "80" );
AltStepAcc       = TEXT( "0" );
AltStepPos       = TEXT( "60" );

StartStepNum         = TEXT( ".022 " );
StartStepVolts       = TEXT( "-1000" );
StartStepVoltsPrefix = TEXT( "UD1=" );

ParkStepNum          = TEXT( ".075 " );
ParkStep2Num         = TEXT( ".129 " );
ParkStepVolts        = TEXT( "-2000" );
ParkStepVoltsPrefix  = TEXT( "UD1=" );
}

/***********************************************************************
*                          FT2_VOLTS_TOASCII                           *
*           Convert -1000 to -1.000, 10000 to 10.000, etc.             *
***********************************************************************/
TCHAR * ft2_volts_toascii( TCHAR * ft2_volts )
{
static TCHAR buf[8];    /* -10.000 */
TCHAR * cp;
TCHAR * dest;
TCHAR * sorc;
int     count;

sorc = ft2_volts;
dest = buf;
if ( *sorc == MinusChar )
    *dest++ = *sorc++;

count = 0;
cp = sorc;
while ( *cp >= ZeroChar && *cp <= NineChar )
    {
    count++;
    cp++;
    }
if ( count > 5 )
    count = 5;

while ( count )
    {
    if ( count == 3 )
        *dest++= DecimalChar;
    *dest++ = *sorc++;
    count--;
    }

*dest = NullChar;
return buf;
}

/***********************************************************************
*                      ASCII_VOLTS_TO_FT2_VOLTS                        *
*           Convert -1.0 to -1000, 10.000 to 10000, etc.               *
* The user will enter the volts in decimal like -9.54. I need to       *
* convert this into -9540 for the board.                               *
***********************************************************************/
TCHAR * ascii_volts_to_ft2_volts( TCHAR * sorc )
{
const int buflen = 6;
static TCHAR buf[buflen+1];    /* -10000 */
TCHAR * dest;
int     chars_needed;
int     n;
bool    past_decimal_point;

past_decimal_point = false;
chars_needed = 3; /* Number of chars after the decimal point */
n = 0;

dest = buf;
while ( (n < buflen) && (chars_needed > 0) && (*sorc != NullChar) )
    {
    if ( *sorc == DecimalChar )
        {
        sorc++;
        past_decimal_point = true;
        }
    else
        {
        *dest++ = *sorc++;
        if ( past_decimal_point )
            chars_needed--;
        n++;
        }
    }

if ( n == 0 )
    {
    copystring( buf, ZeroString );
    }
else
    {
    while ( chars_needed > 0 )
        {
        *dest++ = ZeroChar;
        chars_needed--;
        }
    }

*dest = NullChar;

n = asctoint32( buf );
copystring( buf, int32toasc((int32) n), buflen );

return buf;
}

/***********************************************************************
*                        GET_ALT_STEP_FROM_LINE                        *
* #.025 VA180AC0P60_E1.34 #<--- Alternative short step and then acc.   *
***********************************************************************/
static void get_alt_step_from_line( TCHAR * sorc )
{
TCHAR buf[31];
TCHAR * cp;
TCHAR * ep;

copystring( buf, sorc, 30 );

cp = findstring( AltStepVelPrefix.text(), buf );
if ( !cp )
    return;

cp += AltStepVelPrefix.len();
ep = findstring( AltStepAccPrefix.text(), cp );
if ( !ep )
    return;
*ep = NullChar;
AltStepVel = cp;

cp = ep + AltStepAccPrefix.len();
ep = findstring( AltStepPosPrefix.text(), cp );
if ( !ep )
    return;
*ep = NullChar;
AltStepAcc = cp;

cp = ep + AltStepPosPrefix.len();
ep = findstring( AltStepEnd.text(), cp );
if ( !ep )
    return;
*ep = NullChar;
AltStepPos = cp;
}

/***********************************************************************
*                      GET_STRING_VALUE_FROM_LINE                      *
*                          .0XX UD1=-2000                              *
***********************************************************************/
static TCHAR * get_string_value_from_line( TCHAR * sorc )
{
static TCHAR zerostring[] = TEXT( "0" );
TCHAR * cp;

cp = findchar( EqualsChar, sorc );
if ( cp )
    {
    cp++;
    return cp;
    }

return zerostring;
}

/***********************************************************************
*                     LOAD_CONTROL_MODS_FROM_FILE                      *
***********************************************************************/
static bool load_control_mods_from_file()
{
NAME_CLASS   s;
FILE_CLASS   f;
TCHAR      * cp;
bool         no_comment;
bool         have_alt_step;
bool         have_park_step;
bool         have_start_step;
int          version;

have_alt_step   = false;
have_park_step  = false;
have_start_step = false;

s.get_ft2_ctrl_prog_steps_file_name( ComputerName.text(), CurrentMachine.name );
if ( file_exists(s.text()) )
    {

    if ( f.open_for_read(s.text()) )
        {
        /*
        -----------------------
        Get the start valve step
        ----------------------- */
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;

            if ( !have_park_step )
                {
                if ( strings_are_equal(cp, ParkStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                    {
                    ParkStepVolts = get_string_value_from_line( cp );
                    have_park_step = true;
                    }
                }

            if ( !have_start_step )
                {
                if ( strings_are_equal(cp, StartStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                    {
                    StartStepVolts = get_string_value_from_line( cp );
                    have_start_step = true;
                    }
                }

            if ( have_park_step && have_start_step )
                break;
            }

        /*
        ----------------------
        Get the alternate step
        ---------------------- */
        UsingAltStep = false;
        f.rewind();
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;

            no_comment = true;
            if ( *cp == PoundChar )
                {
                no_comment = false;
                cp++;
                }

            if ( strings_are_equal(cp, AltStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                {
                UsingAltStep = no_comment;
                get_alt_step_from_line( cp );
                have_alt_step = true;
                break;
                }
            }

        f.close();
        }
    }

if ( have_park_step && have_alt_step && have_start_step )
    return true;

version = current_version();
s.get_exe_dir_file_name( ComputerName.text(), ft2_control_program_name(version, CurrentMachine) );
if ( !file_exists(s.text()) )
    return false;

if ( f.open_for_read(s.text()) )
    {
    UsingAltStep = false;
    while ( true )
        {
        cp = f.readline();
        if ( !cp )
            break;

        /*
        ------------------------
        Get the start valve step
        ------------------------ */
        if ( !have_start_step )
            {
            if ( strings_are_equal(cp, StartStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                {
                StartStepVolts = get_string_value_from_line( cp );
                have_start_step = true;
                }
            }

        /*
        ----------------------
        Get the alternate step
        ---------------------- */
        if ( !have_alt_step )
            {
            no_comment = true;
            if ( *cp == PoundChar )
                {
                no_comment = false;
                cp++;
                }

            if ( strings_are_equal(cp, AltStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                {
                UsingAltStep = no_comment;
                get_alt_step_from_line( cp );
                have_alt_step = true;
                }
            }

        /*
        -----------------------
        Get the park valve step
        ----------------------- */
        if ( !have_park_step )
            {
            if ( strings_are_equal(cp, ParkStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                {
                ParkStepVolts = get_string_value_from_line( cp );
                have_park_step = true;
                }
            }

        if ( have_park_step && have_alt_step && have_start_step )
            break;
        }
    f.close();
    }

return true;
}

/***********************************************************************
*                            ALT_STEP_TEXT                             *
***********************************************************************/
TCHAR * alt_step_text()
{
static TCHAR buf[100];
TCHAR * cp;

cp = copystring( buf, AltStepNum.text() );
cp = copystring( cp,  AltStepVelPrefix.text() );
cp = copystring( cp,  AltStepVel.text() );
cp = copystring( cp,  AltStepAccPrefix.text() );
cp = copystring( cp,  AltStepAcc.text() );
cp = copystring( cp,  AltStepPosPrefix.text() );
cp = copystring( cp,  AltStepPos.text() );
cp = copystring( cp,  AltStepEnd.text() );

return buf;
}

/***********************************************************************
*                        SHOW_ALT_STEP_CONTROLS                        *
***********************************************************************/
static void show_alt_step_controls()
{
UINT ids[] = { SLOW_SHOT_VELOCITY_EBOX,   ACCELERATION_EBOX,   ACCUMULATOR_ON_POSITION_EBOX,
               SLOW_SHOT_VELOCITY_STATIC, ACCELERATION_STATIC, ACCUMULATOR_ON_POSITION_STATIC };
const int nof_ids = sizeof(ids)/sizeof(UINT);

int i;
int command;
WINDOW_CLASS w;

command = SW_SHOW;
if ( is_checked(ControlModsDialog.control(NORMAL_STEP_RADIO)) )
    command = SW_HIDE;
for ( i=0; i<nof_ids; i++ )
    {
    w = ControlModsDialog.control(ids[i]);
    w.show( command );
    }

}
/***********************************************************************
*                            SHOW_ALT_STEP                             *
*  Construct the Alt Step line and display it in the ALT_STEP_TBOX.    *
***********************************************************************/
static void show_alt_step()
{
set_text( ControlModsDialog.control(ALT_STEP_TBOX), alt_step_text() );
}

/***********************************************************************
*                           SHOW_START_STEP                            *
***********************************************************************/
static void show_start_step()
{
STRING_CLASS s;

s.upsize(20);

s = StartStepNum;
s += StartStepVoltsPrefix;
s += StartStepVolts;
s.set_text( ControlModsDialog.control(VOLTAGE_STEP_TBOX) );
}

/***********************************************************************
*                            SHOW_PARK_STEP                            *
***********************************************************************/
static void show_park_step()
{
STRING_CLASS s;

s.upsize(20);

s = ParkStepNum;
s += ParkStepVoltsPrefix;
s += ParkStepVolts;
s.set_text( ControlModsDialog.control(PARK_VOLTAGE_STEP_TBOX) );
}

/***********************************************************************
*                       SAVE_CONTROL_MOD_CHANGES                       *
***********************************************************************/
static void save_control_mod_changes()
{
STRING_CLASS s;
FILE_CLASS   f;

s = ftii_ctrl_prog_steps_name( ComputerName.text(), CurrentMachine.name );

if ( f.open_for_write(s.text()) )
    {
    s = resource_string( SAVING_STRING );
    temp_message( s.text(), 500 );

    s.get_text( ControlModsDialog.control(VOLTAGE_STEP_TBOX) );
    f.writeline( s.text() );

    if ( UsingAltStep )
        {
        s.get_text( ControlModsDialog.control(ALT_STEP_TBOX) );
        }
    else
        {
        s = PoundString;
        s += alt_step_text();
        }
    f.writeline( s.text() );

    s.get_text( ControlModsDialog.control(PARK_VOLTAGE_STEP_TBOX) );
    f.writeline( s.text() );

    /*
    ---------------------
    Step .129 is the same
    --------------------- */
    if ( s.replace(ParkStepNum.text(), ParkStep2Num.text()) )
        f.writeline( s.text() );

    f.close();
    }
else
    {
    MessageBox( 0, s.text(), resource_string(CANT_OPEN_FILE_STRING), MB_OK | MB_SYSTEMMODAL );
    }

/*
--------------------------------------
Make text that main window will delete
-------------------------------------- */
MainWindow.send( WM_NEWSETUP, 0, (LPARAM) maketext(CurrentMachine.name) );
}

/***********************************************************************
*                      GET_ALT_STEP_VELOCITY_FOR_EBOX                  *
***********************************************************************/
static void get_alt_step_velocity_for_ebox( STRING_CLASS & dest )
{
TCHAR * buf;
TCHAR * cp;
int        i;
double     d;
BOOLEAN    is_percent;

is_percent = FALSE;
if ( !AltStepVel.contains(HChar) )
    {
    d = AltStepVel.real_value();
    d *= 4;
    d = CurrentPart.velocity_from_x4( d );
    }
else
    {
    buf = 0;
    d   = 0.0;
    if ( !AltStepVel.isempty() )
        buf = maketext( AltStepVel.text() );
    if ( buf )
        {
        is_percent = TRUE;
        strip( buf );
        i = 0;
        if ( is_numeric(*buf) )
            i = asctoint32( buf );
        cp = findchar( HChar, buf );
        if ( cp )
            {
            cp += 2; /* Skip the H8 */
            i += (int) hextoul( cp );
            }
        d = (double) i;
        d /= 100.0;

        delete[] buf;
        }
    }
dest.upsize( MAX_DOUBLE_LEN );
rounded_double_to_tchar( dest.text(), d );
if ( is_percent )
    dest += PercentString;
}

/***********************************************************************
*                         COPY_MODS_TO_SCREEN                          *
***********************************************************************/
static void copy_mods_to_screen()
{
STRING_CLASS s;
HWND   w;
WINDOW_CLASS wc;
int    x;
double d;

s.upsize(30);

w = ControlModsDialog.handle();
UpdatingControlModsDialog = true;

/*
----------
Start Step
---------- */
set_text( w, VOLTAGE_EBOX, ft2_volts_toascii(StartStepVolts.text()) );
show_start_step();

/*
----------
Park Valve
---------- */
set_text( w, PARK_VOLTAGE_EBOX, ft2_volts_toascii(ParkStepVolts.text()) );
show_park_step();

/*
--------
Alt Step
-------- */
if ( UsingAltStep )
    show_alt_step();
else
    set_text( w, ALT_STEP_TBOX, NormalStep );


x = NORMAL_STEP_RADIO;
if ( UsingAltStep )
    x = ALT_STEP_RADIO;
CheckRadioButton( w, NORMAL_STEP_RADIO, ALT_STEP_RADIO, x );

show_alt_step_controls();

/*
--------
Velocity
-------- */
get_alt_step_velocity_for_ebox( s );
s.set_text( w, SLOW_SHOT_VELOCITY_EBOX );

/*
------------
Acceleration
------------ */
d = AltStepAcc.real_value();
d *= 4;
d = CurrentPart.dist_from_x4( d );

s.upsize( MAX_DOUBLE_LEN );
rounded_double_to_tchar( s.text(), d );
s.set_text( w, ACCELERATION_EBOX );

/*
-----------------------
Accumulator On Position
----------------------- */
d = AltStepPos.real_value();
d = CurrentPart.dist_from_x4( d );

s.upsize( MAX_DOUBLE_LEN );
rounded_double_to_tchar( s.text(), d );
s.set_text( w, ACCUMULATOR_ON_POSITION_EBOX );

UpdatingControlModsDialog = false;
}

/***********************************************************************
*                      LOAD_CURRENT_CONTROL_MODS                       *
***********************************************************************/
static void load_current_control_mods()
{
load_control_mods_from_file();
copy_mods_to_screen();
}

/***********************************************************************
*                         CHECK_FOR_NEW_BOARD                          *
***********************************************************************/
static void check_for_new_board()
{
static STRING_CLASS my_machine;

if ( my_machine != CurrentMachine.name )
    {
    my_machine = CurrentMachine.name;
    load_current_control_mods();
    }
}

/***********************************************************************
*                       DO_ALT_STEP_TYPE_SELECT                        *
***********************************************************************/
static void do_alt_step_type_select()
{
if ( is_checked(ControlModsDialog.control(NORMAL_STEP_RADIO)) )
    {
    UsingAltStep = false;
    set_text( ControlModsDialog.control(ALT_STEP_TBOX), NormalStep );
    }
else
    {
    UsingAltStep = true;
    show_alt_step();
    }
}

/***********************************************************************
*                           UPDATE_PARK_STEP                           *
***********************************************************************/
static void update_park_step()
{
STRING_CLASS s;

s.get_text( ControlModsDialog.control(PARK_VOLTAGE_EBOX) );
ParkStepVolts = ascii_volts_to_ft2_volts( s.text() );
show_park_step();
}

/***********************************************************************
*                          UPDATE_START_STEP                           *
***********************************************************************/
static void update_start_step()
{
STRING_CLASS s;

s.get_text( ControlModsDialog.control(VOLTAGE_EBOX) );
StartStepVolts = ascii_volts_to_ft2_volts( s.text() );
show_start_step();
}

/***********************************************************************
*                      EXTRACT_ALT_STEP_VELOCITY                       *
***********************************************************************/
static void extract_alt_step_velocity( STRING_CLASS & s )
{
double d;

d = s.real_value();

if ( s.contains(PercentChar) )
    {
    s = percent_to_hex( d );
    }
else
    {
    d = CurrentPart.x4_from_velocity( d );
    d /= 4;
    d += .5;
    s = int32toasc( (int32) d );
    }
}

/***********************************************************************
*                           UPDATE_ALT_STEP                            *
* The user has changed one of the three alt step values. Update the    *
* globals and display the new step if the alt step is selected.        *
***********************************************************************/
static void update_alt_step( int id )
{
double d;
STRING_CLASS s;
s.upsize( MAX_NUMBER_LEN );

s.get_text( ControlModsDialog.control(id) );
d = s.real_value();

switch ( id )
    {
    case SLOW_SHOT_VELOCITY_EBOX:
        extract_alt_step_velocity( s );
        break;

    case ACCELERATION_EBOX:
        d = CurrentPart.x4_from_dist( d );
        d /= 4;
        break;

    case ACCUMULATOR_ON_POSITION_EBOX:
        d = CurrentPart.x4_from_dist( d );
        break;
    }

if ( id != SLOW_SHOT_VELOCITY_EBOX )
    {
    d += .5;
    s = int32toasc( (int32) d );
    }

switch ( id )
    {
    case SLOW_SHOT_VELOCITY_EBOX:
        AltStepVel = s;
        break;

    case ACCELERATION_EBOX:
        AltStepAcc = s;
        break;

    case ACCUMULATOR_ON_POSITION_EBOX:
        AltStepPos = s;
        break;
    }

if ( UsingAltStep )
    show_alt_step();
}

/***********************************************************************
*                       CONTROL_MODS_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK control_mods_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ControlModsDialog = w;
        position_button_child( ControlModsDialog );
        init_control_mods_dialog();
        ControlModsDialog.hide();
        return TRUE;

    case WM_SHOWWINDOW:
        if ( id != 0 )
            check_for_new_board();
        break;

    case WM_HELP:
        //gethelp( HELP_CONTEXT, GLOBAL_SURETRAK_PARAMETERS_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case NORMAL_STEP_RADIO:
            case ALT_STEP_RADIO:
                if ( cmd == BN_CLICKED && !UpdatingControlModsDialog )
                    {
                    do_alt_step_type_select();
                    show_alt_step_controls();
                    return TRUE;
                    }
                break;

            case SLOW_SHOT_VELOCITY_EBOX:
            case ACCELERATION_EBOX:
            case ACCUMULATOR_ON_POSITION_EBOX:
                if ( cmd == EN_CHANGE && !UpdatingControlModsDialog )
                    {
                    update_alt_step( id );
                    return TRUE;
                    }
                break;

            case PARK_VOLTAGE_EBOX:
                if ( cmd == EN_CHANGE && !UpdatingControlModsDialog )
                    {
                    update_park_step();
                    return TRUE;
                    }
                break;

            case VOLTAGE_EBOX:
                if ( cmd == EN_CHANGE && !UpdatingControlModsDialog )
                    {
                    update_start_step();
                    return TRUE;
                    }
                break;

            case IDOK:
                save_control_mod_changes();
                return TRUE;

            case IDCANCEL:
                load_current_control_mods();
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                  SEND_CURRENT_DIGITAL_POT_COMMAND                    *
***********************************************************************/
static void send_current_digital_pot_command( bool need_temp_message )
{
static TCHAR amp_1_cmd[] = TEXT("ODP2=");
STRING_CLASS s;
STRING_CLASS cmd;
unsigned u;

s.get_text( ValveTestDialog.control(CURRENT_DIGITAL_POT_EBOX) );
cmd = amp_1_cmd;
cmd += s;

send_command_to_current_machine( cmd.text(), true );

if ( need_temp_message )
    temp_message( cmd.text(), 1000 );

u = CurrentMachineVars[DIGITAL_POTS_INDEX].uval();
put_byte( u, s.uint_value(), AMP_1_CURRENT_GAIN );
CurrentMachineVars[DIGITAL_POTS_INDEX] = u;
save_machine_vars();
}

/***********************************************************************
*                          LOAD_MASTER_TRACE                           *
***********************************************************************/
static void load_master_trace()
{
FILE_CLASS f;
NAME_CLASS s;

HaveValveMasterData = false;
ValveMasterMax = ValveTestMax;
ValveMasterMin = ValveTestMin;

s.get_valve_test_master_file_name( ComputerName.text(), CurrentMachine.name );
if ( file_exists(s.text()) )
    {
    if ( f.open_for_read(s.text()) )
        {
        f.readbinary( &ValveMasterMax,  sizeof(int) );
        f.readbinary( &ValveMasterMin,  sizeof(int) );
        f.readbinary( ValveMasterData, sizeof(ValveMasterData) );
        f.close();
        HaveValveMasterData = true;
        }
    }
}

/***********************************************************************
*                          SAVE_MASTER_TRACE                           *
***********************************************************************/
static void save_master_trace()
{
FILE_CLASS f;

int i;
for ( i=0; i<2000; i++ )
    {
    ValveMasterData[i] = ValveTestData[i];
    }
ValveMasterMax = ValveTestMax;
ValveMasterMin = ValveTestMin;
HaveValveMasterData = true;

if ( f.open_for_write(valve_test_master_filename(ComputerName.text(), CurrentMachine.name)) )
    {
    f.writebinary( &ValveMasterMax,  sizeof(int) );
    f.writebinary( &ValveMasterMin,  sizeof(int) );
    f.writebinary( ValveMasterData, sizeof(ValveMasterData) );
    f.close();
    }
}

/***********************************************************************
*                        PLOT_OSCILLOSCOPE_DATA                        *
***********************************************************************/
static void plot_oscilloscope_data( int * data )
{
const int DEFAULT_MAX = 5000;
const int DEFAULT_MIN =    0;

RECTANGLE_CLASS r;
HDC             dc;
HPEN            oldpen;
WINDOW_CLASS    w;

int             dy;
int             i;
int             height;
int             width;
int             x;
int             y;
int             ymax;
int             ymin;
int             yrange;

/*
--------------------------------------
Figure the range to show on the screen
-------------------------------------- */
ymax = DEFAULT_MAX;
if ( ValveTestMax > ymax || ValveMasterMax > ymax )
    {
    ymax = ValveTestMax;
    if ( ValveMasterMax > ymax )
        ymax = ValveMasterMax;
    ymax /= 1000;
    if ( ymax < 10 )
        ymax++;
    ymax *= 1000;
    }

ymin = DEFAULT_MIN;

if ( ValveTestMin < ymin || ValveMasterMin < ymin )
    {
    ymin = ValveTestMin;
    if ( ValveMasterMin < ymax )
        ymin = ValveMasterMin;

    ymin /= 1000;
    if ( ymin < 0 && ymin > -10 )
        ymin--;
    ymin *= 1000;
    }

w = ValveTestDialog.control(VALVE_TEST_MAX_Y_XBOX);
w.set_title( int32toasc((int32) ymax/1000) );

w = ValveTestDialog.control(VALVE_TEST_MIN_Y_XBOX);
w.set_title( int32toasc((int32) ymin/1000) );

yrange = ymax - ymin;
if ( !yrange )
    yrange = 1;

dc = ValveTestDialog.get_dc();

oldpen = 0;
if ( data == ValveMasterData )
    oldpen = (HPEN) SelectObject( dc, MasterPen );

w =  ValveTestDialog.control( Y_AXIS_LINE );

w.get_move_rect( r );
x = r.right + 2;

ValveTestDialog.get_client_rect( r );
r.left = x;

r.right--;
r.bottom--;

width  = r.width();
height = r.height();

x = r.left;
dy = data[0] - ymin;
dy *= height;
dy /= yrange;
y = r.bottom - dy;

MoveToEx( dc, r.left, y, 0 );

for ( i=1; i<2000; i++ )
    {
    x = r.left + i*width/2000;
    dy = data[i] - ymin;
    dy *= height;
    dy /= yrange;
    y = r.bottom - dy;
    LineTo( dc, x, y );
    }

if ( oldpen )
    SelectObject( dc, oldpen );

ValveTestDialog.release_dc( dc );
}

/***********************************************************************
*                       DO_NEW_OSCILLOSCOPE_DATA                       *
***********************************************************************/
static void do_new_oscilloscope_data( LPARAM lParam )
{
int i;
int x;
FTII_OSCILLOSCOPE_DATA * op;

if ( !lParam )
    return;

op = (FTII_OSCILLOSCOPE_DATA *) lParam;

x = op->ch_17_20[0][0];
ValveTestMax = x;
ValveTestMin = x;
if ( op )
    {
    for ( i=0; i<2000; i++ )
        {
        x = op->ch_17_20[i][0];
        ValveTestData[i] = x;
        if ( x > ValveTestMax )
            ValveTestMax = x;
        if ( x < ValveTestMin )
            ValveTestMin = x;
        }

    if ( !HaveValveMasterData )
        {
        ValveMasterMax = ValveTestMax;
        ValveMasterMin = ValveTestMin;
        }

    /*
    -------------------------------------------------------
    All the data was originally allocated as character data
    ------------------------------------------------------- */
    delete[] (char *) lParam;

    ValveTestDialog.refresh();
    }
}

/***********************************************************************
*              SHOW_VALVE_TEST_REVISION_D_CONTROLS                     *
***********************************************************************/
static void show_valve_test_revision_d_controls()
{
static UINT id[] = {
    CURRENT_DIGITAL_POT_EBOX,
    CURRENT_DIGITAL_POT_SPIN,
    DIGITAL_POT_STATIC
    };
const int nof_ids = sizeof(id)/sizeof(int);

WINDOW_CLASS w;
int          i;
bool         is_visible;

if ( HasDigitalServoAmp )
    is_visible = true;
else
    is_visible = false;

for ( i=0; i<nof_ids; i++ )
    {
    w = ValveTestDialog.control( id[i] );
    w.show( is_visible );
    }
}

/***********************************************************************
*                      REFRESH_VALVE_TEST_SCREEN                       *
***********************************************************************/
static void refresh_valve_test_screen()
{
WINDOW_CLASS w;
unsigned u;

show_valve_test_revision_d_controls();

if ( HasDigitalServoAmp )
    {
    u = digital_pot_setting( AMP_1_CURRENT_GAIN );

    w = ValveTestDialog.control( CURRENT_DIGITAL_POT_SPIN );
    w.send( UDM_SETPOS,   0, MAKELPARAM((WORD) u, (WORD) 0) );
    }
}

/***********************************************************************
*                         INIT_VALVE_TEST_DATA                         *
***********************************************************************/
static void init_valve_test_data()
{
static UDACCEL accel = { 1, 1 };
WINDOW_CLASS w;
unsigned u;
int i;

for ( i=0; i<2000; i++ )
    {
    ValveTestData[i]   = 0;
    ValveMasterData[i] = 0;
    }

MasterPen = CreatePen( PS_SOLID, 1, RedColor );

u = digital_pot_setting( AMP_1_CURRENT_GAIN );


w = ValveTestDialog.control( CURRENT_DIGITAL_POT_SPIN );
w.send( UDM_SETRANGE, 0, MAKELPARAM((WORD) 127, (WORD) 0) );
w.send( UDM_SETACCEL, 0, (LPARAM) &accel );
w.send( UDM_SETPOS,   0, MAKELPARAM((WORD) u, (WORD) 0) );

show_valve_test_revision_d_controls();
}

/***********************************************************************
*                        VALVE_TEST_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK valve_test_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static UINT timer_id = 0;

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ValveTestDialog = w;
        position_button_child( ValveTestDialog );
        ValveTestDialog.hide();
        init_valve_test_data();
        ValveTestDialog.send( WM_DBINIT ); /* This will turn off the UpdatingValveTestScreen */
        return TRUE;

    case WM_DBINIT:
        if ( UpdatingValveTestScreen )
            {
            UpdatingValveTestScreen = FALSE;
            }
        else
            {
            UpdatingValveTestScreen = TRUE;
            load_master_trace();
            refresh_valve_test_screen();
            ValveTestDialog.send( WM_DBINIT );
            }
        return TRUE;

    case WM_PAINT:
        ValveTestDialog.post( WM_NEW_DATA, 0, 0 );
        break;

    case WM_HELP:
        //gethelp( HELP_CONTEXT, GLOBAL_SURETRAK_PARAMETERS_HELP );
        return TRUE;

    case WM_TIMER:
        if ( timer_id )
            {
            KillTimer( w, timer_id );
            timer_id = 0;
            ValveTestDialog.refresh();
            }
        return TRUE;

    case WM_NEW_DATA:
        if ( lParam )
            {
            if ( timer_id )
                {
                KillTimer( w, timer_id );
                timer_id = 0;
                }
            do_new_oscilloscope_data( lParam );
            timer_id = SetTimer (w, 0, 5000, NULL) ;
            }
        else
            {
            if ( HaveValveMasterData )
                plot_oscilloscope_data( ValveMasterData );
            if ( timer_id )
                plot_oscilloscope_data( ValveTestData );
            }

        return TRUE;

    case WM_VSCROLL:
        if ( HWND(lParam) == ValveTestDialog.control(CURRENT_DIGITAL_POT_SPIN) && !UpdatingSetDacScreen )
            {
            if ( id == SB_ENDSCROLL )
                {
                /*
                --------------------------------------------------------------------------------------------
                The SB_ENDSCROLL message is only sent when the mouse button is released. Sending the command
                at this time prevents me from banging away at the st2 board.
                -------------------------------------------------------------------------------------------- */
                send_current_digital_pot_command( false );
                return TRUE;
                }
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_AS_MASTER_TRACE_PB:
                if ( GetFocus() == ValveTestDialog.control(CURRENT_DIGITAL_POT_EBOX) )
                    send_current_digital_pot_command( true );
                else
                    save_master_trace();
                return TRUE;
            }
        break;

    case WM_DESTROY:
        if ( timer_id )
            {
            KillTimer( w, timer_id );
            timer_id = 0;
            }

        if ( MasterPen )
            {
            DeleteObject( MasterPen );
            MasterPen = 0;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      INIT_CURRENT_VALUES_DIALOG                      *
***********************************************************************/
static void init_current_values_dialog()
{
WINDOW_CLASS w;
int          i;
INI_CLASS    ini;
STRING_CLASS s;
TCHAR      * cp;
unsigned     x;
BOOLEAN      is_checked;

/*
----------------------------------------------
Get the original name of the AnalogCSV control
---------------------------------------------- */
AnalogCSVTitle.get_text( CurrentValuesDialog.control(ANALOG_CSV_XBOX) );

SwitchChannelsXbox = CurrentValuesDialog.control( CH_17_20_XBOX );
i  = current_channel_mode();
is_checked = FALSE;
if ( i == CHANNEL_MODE_5 || i == CHANNEL_MODE_7 )
    is_checked = TRUE;
set_checkbox( SwitchChannelsXbox, is_checked );

position_button_child( CurrentValuesDialog );

w = CurrentValuesDialog.control( CYCLE_START_TBOX );
w.hide();

w = CurrentValuesDialog.control( ERROR_MESSAGE_TBOX );
set_text( w.handle(), EmptyString );

w = CurrentValuesDialog.control( WAITING_FOR_TBOX );
set_text( w.handle(), EmptyString );

w = CurrentValuesDialog.control( CONTROL_STATE_TBOX );
set_text( w.handle(), EmptyString );

/*
-------------
Analog Values
------------- */
for ( i=0; i<8; i++ )
    {
    w = CurrentValuesDialog.control( FT_A1_THERM+i );
    PostMessage( w.handle(), PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

    w = CurrentValuesDialog.control( VOLTS_1_TBOX+i );
    set_text( w.handle(), EmptyString );
    }

MonitorStatusControl[0].bit = CYCLE_START_BIT;
MonitorStatusControl[0].w   = CurrentValuesDialog.control( CYCLE_START_TBOX );
MonitorStatusControl[0].w.hide();

MonitorStatusControl[1].bit = SHOT_IN_PROGRESS_BIT;
MonitorStatusControl[1].w   = CurrentValuesDialog.control( SHOT_IN_PROGRESS_TBOX );
MonitorStatusControl[1].w.hide();

MonitorStatusControl[2].bit = ZSPEED_BIT;
MonitorStatusControl[2].w   = CurrentValuesDialog.control( ZERO_SPEED_TBOX );
MonitorStatusControl[2].w.hide();

MonitorStatusControl[3].bit = SHOT_COMPLETE_BIT;
MonitorStatusControl[3].w   = CurrentValuesDialog.control( SHOT_COMPLETE_TBOX );
MonitorStatusControl[3].w.hide();

w = CurrentValuesDialog.control( FOLLOWING_OVERFLOW_TBOX );
w.hide();

s = exe_directory();
s.cat_path( FtiiIniFile );

ini.set_file( s.text() );

for ( i=0; i<NofInputBitControls; i++ )
    {
    InputBitControl[i].w = CurrentValuesDialog.control( InputBitControlId[i] );
    InputBitControl[i].w.hide();
    s = TEXT( "InputBitCtrl" );
    s += i;
    if ( ini.find(CurrentValuesSection, s.text()) )
        {
        s = ini.get_string();
        cp = findchar( CommaChar, s.text() );
        if ( cp )
            {
            *cp = NullChar;
            x = s.uint_value();
            InputBitControl[i].bit = 1;
            if ( x > 0 )
                InputBitControl[i].bit <<= x;
            cp++;
            InputBitControl[i].name = cp;
            }
        }
    }

for ( i=0; i<NofOutputBitControls; i++ )
    {
    OutputBitControl[i].w = CurrentValuesDialog.control( OutputBitControlId[i] );
    OutputBitControl[i].w.hide();
    s = TEXT( "OutputBitCtrl" );
    s += i;
    if ( ini.find(CurrentValuesSection, s.text()) )
        {
        s = ini.get_string();
        cp = findchar( CommaChar, s.text() );
        if ( cp )
            {
            *cp = NullChar;
            x = s.uint_value();
            OutputBitControl[i].bit = 1;
            if ( x > 0 )
                OutputBitControl[i].bit <<= x;
            cp++;
            OutputBitControl[i].name = cp;
            }
        }
    }
}

/***********************************************************************
*                              LOG_ALS_FILE                            *
***********************************************************************/
void log_als_file( STRING_CLASS & s )
{
TIME_CLASS t;
FILE_CLASS f;

if ( !AlsLogIsActive )
    return;

t.get_local_time();
s += SpaceChar;
s += t.hhmmss();
if ( f.open_for_append(AlsLogFile) )
    {
    f.writeline( s.text() );
    f.close();
    }
}

/***********************************************************************
*                             LOG_INPUT_BITS                           *
***********************************************************************/
static void log_input_bits()
{
static TCHAR * instring[32] = {
    { TEXT("Mon 0") },
    { TEXT("Mon 1") },
    { TEXT("Mon 2") },
    { TEXT("Mon 3") },
    { TEXT("Mon 4") },
    { TEXT("Mon 5") },
    { TEXT("Mon 6") },
    { TEXT("Mon 7") },
    { TEXT("Mon 8") },
    { TEXT("Mon 9") },
    { TEXT("Mon 10") },
    { TEXT("Mon 11") },
    { TEXT("Mon 12") },
    { TEXT("Mon 13") },
    { TEXT("Mon 14") },
    { TEXT("Mon 15") },
    { TEXT("Pump On") },
    { TEXT("Start Shot") },
    { TEXT("Follow Through") },
    { TEXT("Retract") },
    { TEXT("Jog Shot") },
    { TEXT("Manual Mode") },
    { TEXT("At Home") },
    { TEXT("Bit 23") },
    { TEXT("Bit 24") },
    { TEXT("OK for FS") },
    { TEXT("Vac ON") },
    { TEXT("Low Impact") },
    { TEXT("Fill Test") },
    { TEXT("Bit 29 (0-31)") },
    { TEXT("Bit 30 (0-31)") },
    { TEXT("E-Stop") } };

static STRING_CLASS s;
unsigned mask;
unsigned changedbits;
int      i;

changedbits = LastInputBits ^ CurrentOpStatus.isw1;
if ( changedbits == 0 )
    return;

mask = 1;
for ( i=0; i<32; i++ )
    {
    if ( mask & changedbits )
        {
        s = instring[i];
        if ( mask & CurrentOpStatus.isw1 )
            s += OnString;
        else
            s += OffString;
        if ( NeedErrorLog )
            write_log( s.text() );
        }
    mask <<= 1;
    }
}

/***********************************************************************
*                            LOG_OUTPUT_BITS                           *
***********************************************************************/
static void log_output_bits()
{
static TCHAR * outstring[32] = {
    { TEXT("MonOut 0") },
    { TEXT("MonOut 1") },
    { TEXT("MonOut 2") },
    { TEXT("MonOut 3") },
    { TEXT("MonOut 4") },
    { TEXT("MonOut 5") },
    { TEXT("MonOut 6") },
    { TEXT("MonOut 7") },
    { TEXT("MonOut 8") },
    { TEXT("MonOut 9") },
    { TEXT("MonOut 10") },
    { TEXT("MonOut 11") },
    { TEXT("MonOut 12") },
    { TEXT("MonOut 13") },
    { TEXT("MonOut 14") },
    { TEXT("MonOut 15") },
    { TEXT("Slow Shot      OUTPUT") },
    { TEXT("Accumulator    OUTPUT") },
    { TEXT("OutBit 18") },
    { TEXT("OutBit 19") },
    { TEXT("OutBit 20") },
    { TEXT("OutBit 21") },
    { TEXT("OutBit 22") },
    { TEXT("OutBit 23") },
    { TEXT("OutBit 24") },
    { TEXT("LS 5") },
    { TEXT("LS 6") },
    { TEXT("Fault") },
    { TEXT("LS 1") },
    { TEXT("LS 2") },
    { TEXT("LS 3") },
    { TEXT("LS 4") }
    };

static STRING_CLASS s;
unsigned mask;
unsigned changedbits;
int      i;

changedbits = LastOutputBits ^ CurrentOpStatus.osw1;
if ( changedbits == 0 )
    return;

mask = 1;
for ( i=0; i<32; i++ )
    {
    if ( mask & changedbits )
        {
        s = outstring[i];
        if ( mask & CurrentOpStatus.osw1 )
            s += OnString;
        else
            s += OffString;
        if ( NeedErrorLog )
            write_log( s.text() );
        if ( i == 16 || i == 17)
            log_als_file( s );
        }
    mask <<= 1;
    }
}

/***********************************************************************
*                             UPDATE_OPTOS                             *
* This updates the opto displays. I assume OptoMode is TRUE.           *
* Only the control optos are displayed.                                *
***********************************************************************/
void update_optos( unsigned outputbits, unsigned inputbits )
{
const unsigned mask = 0xF;
int i;

/*
-----------------------------------
The control bits are the high words
----------------------------------- */
outputbits >>= 16;
inputbits >>= 16;

for ( i=0; i<NOF_OPTOS; i++ )
    {
    InputOpto[i].send( WM_NEW_DATA, (WPARAM) (inputbits & mask), 0 );
    inputbits >>= 4;
    OutputOpto[i].send( WM_NEW_DATA, (WPARAM) (outputbits & mask), 0 );
    outputbits >>= 4;
    }
}

/***********************************************************************
*                        DO_NEW_CURRENT_VALUES                         *
***********************************************************************/
static void do_new_current_values()
{
static STRING_CLASS ns;
static STRING_CLASS os;
static WINDOW_CLASS wc;
static HWND         w;
static int          i;
static int          chan;
static DWORD        t;
static unsigned     x;
static double       d;
static bool         should_be_visible;
static BOOLEAN      show_channels_17_20;
static int          channel_mode;
static int          cmdchan;
static int          lvdtchan;
static STRING_CLASS s;
static STRING_CLASS logs;
static SYSTEMTIME   st;
static FILE_CLASS   f;

bool is_dac_analog;

if ( Lindex >= 0 )
    logs = TEXT("Pos = " );

channel_mode  = current_channel_mode();

w = CurrentValuesDialog.control( FT_CURRENT_POSITION_TBOX );
d = CurrentPart.dist_from_x4( (double) CurrentOpStatus.pos );
if ( IsHiresSensor )
    {
    d /= 16.0;
    ns = fixed_string( d, 5, 3 );
    }
else
    {
    ns = fixed_string( d, 4, 2 );
    }

if ( Lindex >= 0 )
    {
    logs += ns;
    logs += TEXT(", Vel = " );
    }
os.get_text( w );
if ( os != ns || NeedRefresh )
    {
    ns.set_text( w );
    w = CurrentValuesDialog.control( FT_POS_THERM );
    if ( CurrentOpStatus.pos > Max_Position )
        {
        Max_Position = CurrentOpStatus.pos;
        if ( IsHiresSensor )
            ns = fixed_string( d, 4, 2 );
        ns.set_text( CurrentValuesDialog.control(MAX_POSITION_TBOX) );
        PostMessage( w, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) Max_Position) );
        }
    i = 0;
    maxint( i, CurrentOpStatus.pos );
    PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
    }

w  = CurrentValuesDialog.control( FT_CURRENT_VELOCITY_TBOX );
d  = CurrentPart.velocity_from_x4((double)(CurrentOpStatus.vel * 4));
ns = fixed_string( d, 4, 4 );
if ( Lindex >= 0 )
    logs += ns;

os.get_text( w );
if ( os != ns || NeedRefresh )
    {
    ns.set_text( w );
    w = CurrentValuesDialog.control( FT_VEL_THERM );
    if ( CurrentOpStatus.vel > Max_Velocity )
        {
        Max_Velocity = CurrentOpStatus.vel;
        PostMessage( w, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) Max_Velocity) );
        }
    i = 0;
    maxint( i, CurrentOpStatus.vel );
    PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
    }

w = CurrentValuesDialog.control( CURRENT_STEP_NUMBER_TBOX );
ns = int32toasc( CurrentOpStatus.blk_no );
os.get_text( w );
if ( os != ns || NeedRefresh )
    {
    ns.set_text( w );
    w = CurrentValuesDialog.control( WAITING_FOR_TBOX );
    os = StepMessage.message( (int32) ns.int_value() );
    os.set_text( w );
    if ( NeedErrorLog )
        {
        s = TEXT("Current Step = ");
        s += ns;
        s += SpaceString;
        s += os;
        write_log( s.text() );
        }
    }

if ( CurrentOpStatus.status_word1 != Control_Status || NeedRefresh )
    {
    Control_Status = CurrentOpStatus.status_word1;
    w = CurrentValuesDialog.control( CONTROL_STATE_TBOX );
    if ( Control_Status & PROGRAM_RUNNING_BIT )
        {
        ns = resource_string( CONTROL_ACTIVE_STRING );
        }
    else if ( !(Control_Status & PROGRAM_LOADED_BIT) )
        {
        ns = resource_string( NO_PROGRAM_STRING );
        }
    else if ( Control_Status & FATAL_ERROR_BIT )
        {
        ns = resource_string( CONTROL_FATAL_STRING );
        }
    else if ( Control_Status & WARNING_BIT )
        {
        ns = resource_string( CONTROL_WARNING_STRING );
        }
    else if ( !(CurrentOpStatus.isw4 & MASTER_ENABLE_BIT) )
        {
        ns = resource_string( NO_MASTER_ENABLE_STRING );
        }
    else
        {
        ns = resource_string( UNKNOWN_STRING );
        }
    ns.set_text( w );
    if ( NeedErrorLog )
        {
        s = TEXT("Status Word 1 = ");
        s += ns;
        write_log( s.text() );
        }

    /*
    ----------------------------------------------------------------------
    This section used to be outside the if brackets. I moved it in 9/14/05
    ---------------------------------------------------------------------- */
    w = CurrentValuesDialog.control( ERROR_MESSAGE_TBOX );
    if ( Control_Status & FATAL_ERROR_BIT )
        {
        ns = fatal_error_string( CurrentOpStatus.fatal_error );
        ns.set_text( w );
        if ( NeedErrorLog )
            write_log( ns.text() );
        }
    else if ( Control_Status & WARNING_BIT )
        {
        ns = warning_error_string( CurrentOpStatus.warning );
        ns.set_text( w );
        if ( NeedErrorLog )
            write_log( ns.text() );
        }
    else
        {
        set_text( w, EmptyString );
        }
    }

if ( OptoMode )
    update_optos( CurrentOpStatus.osw1, CurrentOpStatus.isw1 );

if ( CurrentOpStatus.isw1 != LastInputBits || NeedRefresh )
    {
    for ( i=0; i<NofInputBitControls; i++ )
        {
        x = InputBitControl[i].bit & CurrentOpStatus.isw1;
        if ( x )
            should_be_visible = true;
        else
            should_be_visible = false;
        if ( should_be_visible != InputBitControl[i].is_visible )
            {
            InputBitControl[i].is_visible = should_be_visible;
            if ( !OptoMode )
                InputBitControl[i].w.show( should_be_visible );
            }
        }

    if ( NeedErrorLog )
        {
        log_input_bits();
        }
    else
        {
        Lindex = -1;
        }
    LastInputBits = CurrentOpStatus.isw1;
    }

if ( CurrentOpStatus.osw1 != LastOutputBits || NeedRefresh )
    {
    for ( i=0; i<NofOutputBitControls; i++ )
        {
        x = OutputBitControl[i].bit & CurrentOpStatus.osw1;
        if ( x )
            should_be_visible = true;
        else
            should_be_visible = false;
        if ( should_be_visible != OutputBitControl[i].is_visible )
            {
            OutputBitControl[i].is_visible = should_be_visible;
            if ( !OptoMode )
                OutputBitControl[i].w.show( should_be_visible );
            }
        }

    //if ( NeedErrorLog )
    log_output_bits();
    LastOutputBits = CurrentOpStatus.osw1;
    }

if ( Monitor_Status != CurrentOpStatus.monitor_status || NeedRefresh )
    {
    for ( i=0; i<NofMonitorStatusControls; i++ )
        {
        x = MonitorStatusControl[i].bit & CurrentOpStatus.monitor_status;
        if ( x )
            should_be_visible = true;
        else
            should_be_visible = false;
        if ( should_be_visible != MonitorStatusControl[i].is_visible )
            {
            MonitorStatusControl[i].is_visible = should_be_visible;
            MonitorStatusControl[i].w.show( should_be_visible );
            }
        }

    Monitor_Status = CurrentOpStatus.monitor_status;
    }

show_channels_17_20 = is_checked( SwitchChannelsXbox );

if ( show_channels_17_20 )
    {
    if ( channel_mode == CHANNEL_MODE_7 )
        {
        w = CurrentValuesDialog.control(VOLTS_3_TBOX);
        i = CurrentOpStatus.dac[0];
        d = (double) i;
        d /= 1000.0;
        d = round( d, 0.01 );
        ns = ascii_double( d );
        os.get_text( w );
        if ( os != ns )
            {
            ns.set_text( w );
            w = CurrentValuesDialog.control( FT_A3_THERM );
            i += 10000;
            PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
            }
        }

    w = CurrentValuesDialog.control(VOLTS_6_TBOX);
    /*
    -------------------------------------------------------
    Pressure Control is shown on the second set of readings
    ------------------------------------------------------- */
    if ( channel_mode == CHANNEL_MODE_7 )
        i = CurrentOpStatus.dac[2];
    else
        i = CurrentOpStatus.dac[0];

    d = (double) i;
    d /= 1000.0;
    d = round( d, 0.01 );
    ns = ascii_double( d );
    os.get_text( w );
    if ( os != ns )
        {
        ns.set_text( w );
        w = CurrentValuesDialog.control( FT_A6_THERM );
        i += 10000;
        PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
        }
    }

chan = 0;
for ( i=0; i<8; i++ )
    {
    if ( show_channels_17_20 )
        {
        if ( channel_mode == CHANNEL_MODE_7 )
            {
            if ( i == 2 || i == 5 )
                {
                chan++;
                continue;
                }
            switch ( i )
                {
                case 3:
                    chan = 6;   /* W1 = Meas. Command 1 */
                    break;
                case 4:
                    chan = 4;   /* C1 = LVDT 1 */
                    break;
                case 6:
                    chan = 7;   /* W2 = Meas Command 2 */
                    break;
                case 7:
                    chan = 5;   /* C3 = LVDT 2 */
                    break;
                }
            }
        else
            {
            /*
            -----------------------------------------------------------
            The 6th box is the command voltage and has already been set
            ----------------------------------------------------------- */
            if ( i == 5 )
                {
                chan++;
                continue;
                }
            }
        }

    if ( AnalogValue[chan] != CurrentAnalogValue[chan] || NeedToRefreshAnalogs || NeedRefresh )
        {
        AnalogValue[chan] = CurrentAnalogValue[chan];
        w  = CurrentValuesDialog.control( VOLTS_1_TBOX+i);

        x = AnalogValue[chan];

        is_dac_analog = false;
        switch ( channel_mode )
            {
            case 4:
                if ( chan > 3 )
                    is_dac_analog = true;
                break;
            case 5:
                if ( chan > 5 )
                    is_dac_analog = true;
                break;
            case 7:
                if ( chan > 1 )
                    is_dac_analog = true;
                break;
            }

        if ( is_dac_analog )
            ns = ascii_dac_volts( x );
        else
            ns = ascii_0_to_5_volts( x );

        ns.set_text( w );
        w = CurrentValuesDialog.control( FT_A1_THERM+i );
        PostMessage( w, PBM_SETPOS, (WPARAM) x, 0L );
        }

    chan++;
    }

if ( Lindex >= 0 )
    {
    if ( show_channels_17_20 )
        {
        cmdchan  = current_measured_command_index( DAC_1 );
        lvdtchan = current_lvdt_feedback_index( DAC_1 );

        logs += TEXT( ", Meas CMD = " );
        x = AnalogValue[cmdchan];
        logs += ascii_dac_volts( x );

        logs += TEXT( ", LVDT Feedback = " );
        x = AnalogValue[lvdtchan];
        logs += ascii_dac_volts( x );
        }

    /*
    -------------------------
    Channel 1 = head pressure
    ------------------------- */
    logs += TEXT( ", Ch 1= " );
    i = CurrentPart.analog_sensor[0];
    x = AnalogValue[0];
    logs += ascii_double( AnalogSensor[i].converted_value(x) );

    /*
    -------------------------
    Channel 2 = head pressure
    ------------------------- */
    logs += TEXT( ", Ch 2= " );
    i = CurrentPart.analog_sensor[1];
    x = AnalogValue[1];
    logs += ascii_double( AnalogSensor[i].converted_value(x) );

    write_log( logs.text() );
    Lindex++;
    if ( Lindex > 9 )
        Lindex = -1;
    }

/*
--------------------------------------------------------------------------
See if I should save the new value to the analog.csv file for Mark Potratz
-------------------------------------------------------------------------- */
if ( LoggingAnalogs )
    {
    t = GetTickCount();
    if ( AnalogCSVStartMs == 0 )
        AnalogCSVStartMs = t;
    d = t - AnalogCSVStartMs;
    d /= 1000.0;
    logs.upsize( MAX_DOUBLE_LEN );
    double_to_tchar( logs.text(), d, 1 );
    logs.set_text( CurrentValuesDialog.control(ANALOG_CSV_XBOX) );
    logs += CommaChar;
    x = AnalogValue[AnalogCSVChannel-1];
    logs += ascii_double( AnalogSensor[AnalogCSVSensor].converted_value(x) );
    if ( f.open_for_append(AnalogCSVFile) )
        {
        f.writeline( logs );
        f.close();
        }
    }

NeedToRefreshAnalogs = false;
NeedRefresh          = FALSE;
}

/***********************************************************************
*                              NEED_BRUSH                              *
***********************************************************************/
bool need_brush( HWND w )
{
int i;

for ( i=0; i<NofInputBitControls; i++ )
    {
    if ( InputBitControl[i].w == w )
        return true;
    }

for ( i=0; i<NofOutputBitControls; i++ )
    {
    if ( OutputBitControl[i].w == w )
        return true;
    }

for ( i=0; i<NofMonitorStatusControls; i++ )
    {
    if ( MonitorStatusControl[i].w == w )
        return true;
    }

return false;
}

/***********************************************************************
*                          SAVE_CHANNEL_MODE                           *
***********************************************************************/
static void save_channel_mode( int new_mode )
{
NAME_CLASS  s;
INI_CLASS   ini;

s.get_ft2_editor_settings_file_name( CurrentMachine.computer, CurrentMachine.name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
ini.put_int( CurrentChannelModeKey, new_mode );
}

/***********************************************************************
*                       SEND_CHANNEL_MODE_COMMAND                      *
***********************************************************************/
static int send_channel_mode_command()
{
STRING_CLASS s;
int mode;

if ( is_checked(SwitchChannelsXbox) )
    {
    if ( HasDigitalServoAmp )
        mode = CHANNEL_MODE_7;
    else
        mode = CHANNEL_MODE_5;
    }
else
    {
    mode = CHANNEL_MODE_1;
    }

s = ChannelModeSetString;
s += mode;;

send_command_to_current_machine( s.text(), true );
save_channel_mode( mode );

return mode;
}

/***********************************************************************
*                       SEND_CHANNEL_MODE_COMMAND                      *
***********************************************************************/
static void send_channel_mode_command( int new_mode )
{
STRING_CLASS s;

s = ChannelModeSetString;
s += new_mode;

send_command_to_current_machine( s.text(), true );
save_channel_mode( new_mode );
}

/***********************************************************************
*                          TOGGLE_IO_DISPLAY                           *
*   Toggle between the text display and the opto module display        *
***********************************************************************/
static void toggle_io_display()
{
int i;

/*
-------------------------------
Hide the visible controls first
------------------------------- */
if ( OptoMode )
    {
    OptoMode = FALSE;
    for ( i=0; i<NOF_OPTOS; i++ )
        {
        InputOpto[i].hide();
        OutputOpto[i].hide();
        }
    }
else
    {
    OptoMode = TRUE;
    for ( i=0; i<NofInputBitControls; i++ )
        {
        if ( InputBitControl[i].is_visible )
            InputBitControl[i].w.hide();
        }
    for ( i=0; i<NofOutputBitControls; i++ )
        {
        if ( OutputBitControl[i].is_visible )
            OutputBitControl[i].w.hide();
        }
    }

if ( OptoMode )
    {
    for ( i=0; i<NOF_OPTOS; i++ )
        {
        InputOpto[i].show();
        OutputOpto[i].show();
        }
    update_optos( LastOutputBits, LastInputBits );
    }
else
    {
    for ( i=0; i<NofInputBitControls; i++ )
        {
        if ( InputBitControl[i].is_visible )
            InputBitControl[i].w.show();
        }
    for ( i=0; i<NofOutputBitControls; i++ )
        {
        if ( OutputBitControl[i].is_visible )
            OutputBitControl[i].w.show();
        }
    }

if ( OptoMode )
    {
    set_text( CurrentValuesDialog.control(TOGGLE_IO_DISPLAY_PB), TEXT("T E X T") );
    SendMessage( ToolTipWindow, TTM_ACTIVATE, (WPARAM) TRUE, 0 );
    }
else
    {
    set_text( CurrentValuesDialog.control(TOGGLE_IO_DISPLAY_PB), TEXT("O P T O") );
    SendMessage( ToolTipWindow, TTM_ACTIVATE, (WPARAM) FALSE, 0 );
    }
}

/***********************************************************************
*                            KILL_LOG_FILE                             *
***********************************************************************/
void kill_log_file()
{
static TCHAR logfile[] = TEXT("c:\\ft2.log" );
static TCHAR tempfile[] = TEXT("c:\\ft2copy.log" );
static TCHAR zipfile[] = TEXT("c:\\ft2.zip" );

STRING_CLASS s;
STRING_CLASS e;

if ( is_checked(CurrentValuesDialog.control(LOG_FILE_ZIP_XBOX)) )
    {
    if ( file_exists(zipfile) )
        DeleteFile( zipfile );

    if ( file_exists(logfile) )
        {
        if ( file_exists(tempfile) )
            DeleteFile( tempfile );
        if ( MoveFile(logfile, tempfile) )
            {
            s = TEXT( "pkzip25 -add -lev=9 " );
            s += zipfile;
            s += SpaceString;
            s += tempfile;
            e = exe_directory();
            execute_and_wait( s.text(), e.text() );
            }
        DeleteFile( tempfile );
        }
    }
else
    {
    DeleteFile( logfile );
    }
}

/***********************************************************************
*                            DO_OPTO_CLICK                             *
* wParam is the wire [1,2,3,4] of this opto. lParam is the window      *
* handle.                                                              *
***********************************************************************/
static void do_opto_click( WPARAM wParam, LPARAM lParam )
{
const int WIRES_PER_OPTO = 4;
int i;
int wire_to_toggle;
unsigned bit;
BOOLEAN  turn_on;

wire_to_toggle = 0;

for ( i=0; i<NOF_TEST_ALARM_OPTOS; i++ )
    {
    if ( TestAlarmOpto[i].handle() == (HWND) lParam )
        {
        wire_to_toggle += (int) wParam;
        bit = wirebit( wire_to_toggle );
        turn_on = TRUE;
        if ( bit & CurrentOpStatus.osw1 )
            turn_on = FALSE;
        send_output_bits_to_current_machine( bit, turn_on );
        break;
        }
    wire_to_toggle += WIRES_PER_OPTO;
    }
}

/***********************************************************************
*                      SEND_ZERO_MESSAGE_TO_BOARD                      *
***********************************************************************/
static void send_zero_message_to_board()
{
if ( CurrentOpStatus.status_word1 & PROGRAM_RUNNING_BIT )
    {
    if ( !(MANUAL_MODE_MASK & CurrentOpStatus.isw1) )
        return;
    }

send_command_to_current_machine( ZeroPositionCmd, true );
}

/***********************************************************************
*                      DO_NEW_ALARM_OUTPUT_VALUES                      *
***********************************************************************/
static void do_new_alarm_output_values()
{
const unsigned mask = 0xF;
static unsigned Last_Osw1;

unsigned outputbits;
int i;

if ( CurrentOpStatus.osw1 != Last_Osw1 || NeedRefresh )
    {
    Last_Osw1 = CurrentOpStatus.osw1;
    outputbits = Last_Osw1;;

    for ( i=0; i<NOF_TEST_ALARM_OPTOS; i++ )
        {
        TestAlarmOpto[i].send( WM_NEW_DATA, (WPARAM) (outputbits & mask), 0 );
        outputbits >>= 4;
        }
    }
}

/***********************************************************************
*                        INIT_TEST_ALARM_OPTOS                         *
***********************************************************************/
static void init_test_alarm_optos()
{
int id;
int i;
id = MONITOR_OUTPUT_1_OPTO;
for ( i=0; i<NOF_TEST_ALARM_OPTOS; i++ )
    {
    TestAlarmOpto[i] = TestAlarmOutputsDialog.control( id );
    id++;
    }
}

/***********************************************************************
*                     TEST_ALARM_OUTPUTS_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK test_alarm_outputs_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
const unsigned ALL_BITS = 0xFFFFFFFF;
int id;
BOOLEAN turn_on;
HWND w;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        TestAlarmOutputsDialog = hWnd;
        TestAlarmOutputsDialog.hide();
        init_test_alarm_optos();
        break;

    case WM_NEW_DATA:
        do_new_alarm_output_values();
        return TRUE;

    case WM_OPTO_CLICK:
        do_opto_click( wParam, lParam );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case TOGGLE_ALL_OUTPUTS_XBOX:
                w = GetDlgItem( hWnd, TOGGLE_ALL_OUTPUTS_XBOX );
                turn_on = is_checked( w );
                if ( turn_on )
                    id = TURN_OFF_ALL_OUTPUTS_STRING;
                else
                    id = TURN_ON_ALL_OUTPUTS_STRING;
                set_text( w, resource_string(id) );
                send_output_bits_to_current_machine( ALL_BITS, turn_on );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                               DO_UPDOWN                              *
***********************************************************************/
static void do_updown( int id )
{
int mymode;
int i;
WINDOW_CLASS w;

mymode = current_channel_mode();

if ( id == FT2_MODE_UP_PB )
    {
    if ( mymode < CHANNEL_MODE_3 )
        mymode++;
    else
        mymode = 0;
    }
else
    {
    if ( mymode > CHANNEL_MODE_1 )
        mymode--;
    else
        mymode = 0;
    }

if ( mymode > 0 )
    {
    send_channel_mode_command( mymode );
    send_command_to_current_machine( SEND_OP_DATA_PACKETS, true );

    i = 5;
    if ( mymode == CHANNEL_MODE_2 )
        i = 9;
    else if ( mymode == CHANNEL_MODE_3 )
        i = 13;

    for ( id=CH_5_TBOX; id<=CH_8_TBOX; id++ )
        {
        w = CurrentValuesDialog.control( id );
        w.set_title( int32toasc(i) );
        i++;
        }
    }

}

/***********************************************************************
*                            TOGGLE_ANALOG_CSV                         *
***********************************************************************/
static void toggle_analog_csv_xbox()
{
BOOLEAN      b;
INI_CLASS    ini;
NAME_CLASS s;

b = is_checked( CurrentValuesDialog.control(ANALOG_CSV_XBOX) );
if ( b )
    {
    AnalogCSVStartMs = 0;
    s.get_local_ini_file_name( MonallIniFile );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    if ( ini.find(AnalogCSVChannelKey) )
        AnalogCSVChannel = ini.get_int();
    if ( AnalogCSVChannel<1 || AnalogCSVChannel>MAX_FT2_CHANNELS )
        AnalogCSVChannel = 1;
    AnalogCSVSensor = CurrentPart.analog_sensor[AnalogCSVChannel-1];
    if ( ini.find(AnalogCSVFileKey) )
        {
        AnalogCSVFile = ini.get_string();
        if ( AnalogCSVFile.file_exists() )
            AnalogCSVFile.delete_file();
        }
    }
else
    {
    AnalogCSVTitle.set_text( CurrentValuesDialog.control(ANALOG_CSV_XBOX) );
    }

LoggingAnalogs = b;
}

/***********************************************************************
*                         CURRENT_VALUES_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK current_values_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOOLEAN firstime = TRUE;
static HWND w;
static int  id;
static HWND error_log_tbox_handle = 0;

MSG     mymsg;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentValuesDialog = hWnd;
        error_log_tbox_handle = GetDlgItem( hWnd, ERROR_LOG_TBOX );
        init_current_values_dialog();
        /*
        -------------------------------------------------------------------
        Initialize the window classes for the input and output opto modules
        ------------------------------------------------------------------- */
        for ( id=0; id<NOF_OPTOS; id++ )
            {
            InputOpto[id]  = CurrentValuesDialog.control(  INPUT_1_OPTO+id );
            OutputOpto[id] = CurrentValuesDialog.control( OUTPUT_1_OPTO+id );
            }
        CurrentValuesDialog.hide();
        break;

    case WM_NEW_DATA:
        do_new_current_values();
        return TRUE;

    case WM_DBINIT:
        if ( firstime )
            {
            set_channel_5_8_labels();
            firstime = FALSE;
            }
        refresh_current_values();
        NeedRefresh = TRUE;
        return TRUE;

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        if ( OptoMode )
            {
            mymsg.lParam  = lParam;
            mymsg.wParam  = wParam;
            mymsg.message = msg;
            mymsg.hwnd    = hWnd;
            SendMessage( ToolTipWindow, TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &mymsg );
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case FT2_MODE_UP_PB:
            case FT2_MODE_DOWN_PB:
                do_updown( id );
                return TRUE;

            case CH_17_20_XBOX:
                set_channel_5_8_labels( send_channel_mode_command() );
                /*
                ----------------------------------------------------------------
                I must now restart the op status packets so the board reads v429
                ---------------------------------------------------------------- */
                send_command_to_current_machine( SEND_OP_DATA_PACKETS, true );
                return TRUE;

            case TOGGLE_IO_DISPLAY_PB:
                toggle_io_display();
                return TRUE;

            case MAX_POSITION_RESET_BUTTON:
                w = CurrentValuesDialog.control( FT_POS_THERM );
                PostMessage( w, PBM_SETPOS, (WPARAM) 0, 0L );
                Max_Position = 0;
                set_text( CurrentValuesDialog.control(FT_CURRENT_POSITION_TBOX), TEXT("0") );
                set_text( CurrentValuesDialog.control(MAX_POSITION_TBOX), TEXT("0") );
                PostMessage( w, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) Max_Position) );
                send_zero_message_to_board();
                return TRUE;

            case ANALOG_CSV_XBOX:
                toggle_analog_csv_xbox();
                return TRUE;

            case LOG_FILE_VIEW_PB:
                if ( is_windows_nt() )
                    WinExec( "c:\\windows\\system32\\notepad.exe c:\\ft2.log", SW_SHOWNORMAL );
                else
                    WinExec( "c:\\progra~1\\accessories\\wordpad.exe c:\\ft2.log", SW_SHOWNORMAL );
                return TRUE;

            case LOG_FILE_KILL_PB:
                if ( MessageBox(hWnd, TEXT("Delete FT2.LOG?"), TEXT("Confirm"), MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK )
                    kill_log_file();
                return TRUE;

            case LOG_FILE_SETUP_PB:
                if ( NeedErrorLog )
                    {
                    NeedErrorLog = FALSE;
                    set_text( hWnd, LOG_FILE_SETUP_PB, resource_string(OFF_STRING) );
                    }
                else
                    {
                    NeedErrorLog = TRUE;
                    set_text( hWnd, LOG_FILE_SETUP_PB, resource_string(ON_STRING) );
                    }
            InvalidateRect( error_log_tbox_handle, 0, TRUE );
            }
        break;

    case WM_CTLCOLORSTATIC:
        w = (HWND) lParam;
        if ( w == error_log_tbox_handle )
            {
            if ( NeedErrorLog )
                {
                SetBkColor( (HDC) wParam, OrangeColor );
                return (int) OrangeBackgroundBrush;
                }
            }
        else if ( need_brush(w) )
            {
            SetBkColor( (HDC) wParam, GreenColor );
            return (int) GreenBackgroundBrush;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      CREATE_DIAGNOSTIC_DIALOGS                       *
***********************************************************************/
static void create_diagnostic_dialogs()
{
CreateDialog(
    MainInstance,
    TEXT("CURRENT_VALUES_DIALOG"),
    ButtonDialog.handle(),
    (DLGPROC) current_values_dialog_proc );

UpdatingSetDacScreen = TRUE;
CreateDialog(
    MainInstance,
    TEXT("SET_DAC_DIALOG"),
    ButtonDialog.handle(),
    (DLGPROC) set_dac_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("SET_NULL_DIALOG"),
    ButtonDialog.handle(),
    (DLGPROC) set_null_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("GLOBAL_PARAMETER_DIALOG"),
    ButtonDialog.handle(),
    (DLGPROC) global_parameter_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("CONTROL_MODS_DIALOG"),
    ButtonDialog.handle(),
    (DLGPROC) control_mods_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("TEST_ALARM_OUTPUTS_DIALOG"),
    ButtonDialog.handle(),
    (DLGPROC) test_alarm_outputs_dialog_proc );

UpdatingValveTestScreen = TRUE;
CreateDialog(
    MainInstance,
    TEXT("VALVE_TEST_DIALOG"),
    ButtonDialog.handle(),
    (DLGPROC) valve_test_dialog_proc );
}

/***********************************************************************
*                          END_BUTTON_DIALOG                           *
***********************************************************************/
static void end_button_dialog()
{
set_current_window( MainWindow );
}

/***********************************************************************
*                        POSITION_BUTTON_DIALOG                        *
***********************************************************************/
static void position_button_dialog()
{
static bool firstime       = true;
static bool is_fullscreen  = false;

bool new_is_fullscreen;
long x;
long y;
RECTANGLE_CLASS dr;
RECTANGLE_CLASS r;
APPBARDATA ab;

/*
--------------------------------------------------------------------
If the taskbar is always on top I use my function to get the window,
otherwise I just use the desktop client size.
-------------------------------------------------------------------- */
ab.cbSize = sizeof( APPBARDATA );
ab.hWnd   = 0;

if ( SHAppBarMessage(ABM_GETSTATE, &ab) == ABS_ALWAYSONTOP )
    new_is_fullscreen = false;
else
    new_is_fullscreen = true;

if ( firstime || new_is_fullscreen != is_fullscreen )
    {
    firstime = false;
    is_fullscreen = new_is_fullscreen;

    if ( is_fullscreen )
        GetClientRect( GetDesktopWindow(), &dr );
    else
        dr = desktop_client_rect();

    ButtonDialog.getrect( r );
    x = 0;
    y = dr.bottom - r.height();
    r.moveto( x, y );

    y = r.height();
    r.setsize( dr.width(), y );

    ButtonDialog.move( r );

    }
}

/***********************************************************************
*                          START_NULL_DIALOG                           *
***********************************************************************/
static void start_null_dialog()
{

if ( CurrentWindow == SetDacDialog || CurrentWindow == ValveTestDialog || CurrentWindow == TestAlarmOutputsDialog )
    set_current_window( SetNullDialog );
else
    start_if_ok_to_deactivate( SetNullDialog );
}

/***********************************************************************
*                       START_SET_DAC_DIALOG                           *
***********************************************************************/
static void start_set_dac_dialog()
{

if ( CurrentWindow == SetNullDialog || CurrentWindow == ValveTestDialog || CurrentWindow == TestAlarmOutputsDialog )
    set_current_window( SetDacDialog );
else
    start_if_ok_to_deactivate( SetDacDialog );
}

/***********************************************************************
*                       START_VALVE_TEST_DIALOG                        *
***********************************************************************/
static void start_valve_test_dialog()
{

if ( CurrentWindow == SetNullDialog || CurrentWindow == SetDacDialog || CurrentWindow == TestAlarmOutputsDialog )
    set_current_window( ValveTestDialog );
else
    start_if_ok_to_deactivate( ValveTestDialog );
}

/***********************************************************************
*                       START_ALARM_TEST_DIALOG                        *
***********************************************************************/
static void start_alarm_test_dialog()
{
if ( CurrentWindow == SetNullDialog || CurrentWindow == SetDacDialog || CurrentWindow == ValveTestDialog )
    set_current_window( TestAlarmOutputsDialog );
else
    start_if_ok_to_deactivate( TestAlarmOutputsDialog );
}

/**********************************************************************
*                               SHOW_BUTTONS                          *
**********************************************************************/
static void show_buttons()
{
static UINT id[] = { F2_RADIO, F3_RADIO, F4_RADIO, F5_RADIO, F9_RADIO, F10_RADIO };
const int nof_ids = sizeof(id)/sizeof(UINT);
BOOLEAN      is_enabled;
int          i;
WINDOW_CLASS w;

is_enabled = TRUE;
if ( CurrentPasswordLevel < ServiceToolsGrayLevel )
    is_enabled = FALSE;

for ( i=0; i<nof_ids; i++ )
    {
    w = ButtonDialog.control( id[i] );
    w.enable( is_enabled );
    }
}

/***********************************************************************
*                           BUTTON_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK button_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ButtonDialog = hWnd;
        position_button_dialog();
        /*
        ----------------------------------------------------------------------
        Once the button dialog is created I can create the rest of the dialogs
        ---------------------------------------------------------------------- */
        create_diagnostic_dialogs();
        break;

    case WM_SHOWWINDOW:
        if ( (BOOL) wParam )
            position_button_dialog();
        break;

    case WM_COMMAND:
        if ( !WaitingToResume )
            {
            if ( CurrentPasswordLevel < ServiceToolsGrayLevel )
                {
                if ( id >= F2_RADIO && id <= F10_RADIO )
                    id = F8_RADIO;
                }

            switch ( id )
                {
                case F2_RADIO:
                    start_set_dac_dialog();
                    return TRUE;

                case F3_RADIO:
                    start_null_dialog();
                    return TRUE;

                case F4_RADIO:
                    start_valve_test_dialog();
                    return TRUE;

                case F5_RADIO:
                    start_alarm_test_dialog();
                    return TRUE;

                case F8_RADIO:
                    set_current_window( CurrentValuesDialog );
                    return TRUE;

                case F9_RADIO:
                    set_current_window( GlobalParameterDialog );
                    return TRUE;

                case F10_RADIO:
                    set_current_window( ControlModsDialog );
                    return TRUE;

                case EXIT_RADIO:
                case IDCANCEL:
                    end_button_dialog();
                    return TRUE;
                }
            }
        break;

    case WM_CLOSE:
        end_button_dialog();
        return TRUE;
    }

return FALSE;

}

/***********************************************************************
*                         OUTPUT_LIST_UPDATE                           *
***********************************************************************/
static void output_list_update()
{
STRING_CLASS s;
FTII_LIMIT_SWITCH_WIRE_CLASS lsw;
PARAMETER_CLASS p;

int base_length;
int i;
int led;
int module;
int wire;

if ( !OutputListbox.handle() )
    return;

OutputListbox.empty();
lsw.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
p.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
wire = 0;
for ( module=0; module<8; module++ )
    {
    for ( led=0; led<4; led++ )
        {
        wire++;
        s = TEXT( "Wire ");
        s += wire;
        if ( module < 4 )
            {
            s += TEXT( " Monitor Module " );
            s += module*4+led+1;
            }
        else
            {
            s += TEXT( " Control Module " );
            s += module - 3;
            s += TEXT( " Led " );
            s += led + 1;
            }
        s += SpaceString;
        base_length = s.len();
        if ( wire == DownTimeAlarmWire )
            s += TEXT( "Downtime Alarm Wire  " );
        if ( wire == MonitorWire )
            s += TEXT( "Monitor Wire  " );

        for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
            {
            if ( lsw[i] == (unsigned) wire )
                {
                s += TEXT( "Limit Switch #" );
                s += i+1;
                s += TEXT( "  " );
                }
            }

        for ( i=0; i<MAX_PARMS; i++ )
            {
            if ( wire == (int) p.parameter[i].limits[ALARM_MIN].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=Low Alarm  " );
                }
            if ( wire == (int) p.parameter[i].limits[ALARM_MAX].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=High Alarm  " );
                }
            if ( wire == (int) p.parameter[i].limits[WARNING_MIN].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=Low Warning  " );
                }
            if ( wire == (int) p.parameter[i].limits[WARNING_MAX].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=High Warning  " );
                }
            }

        if ( s.len() > base_length )
            OutputListbox.add( s.text() );
        }
    }
}

/***********************************************************************
*                       SIZE_OUTPUT_LIST_DIALOG                        *
***********************************************************************/
static void size_output_list_dialog()
{
RECTANGLE_CLASS r;
WINDOW_CLASS w;

OutputListDialog.get_client_rect( r );
w = OutputListbox.handle();

w.move( r );
}

/***********************************************************************
*                       OUTPUT_LIST_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK output_list_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        OutputListDialog = hWnd;
        OutputListbox.init( hWnd, OUTPUT_LIST_LB );
        break;

    case WM_SIZE:
        size_output_list_dialog();
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case EXIT_RADIO:
            case IDCANCEL:
                OutputListDialog.hide();
                return TRUE;
            }
        break;

    case WM_CLOSE:
        OutputListDialog.hide();
        return FALSE;
    }

return FALSE;
}

/***********************************************************************
*                       END_UPLOAD_FLASH_DIALOG                        *
***********************************************************************/
void end_upload_flash_dialog()
{
HWND w;

w = UploadFlashDialog.handle();
UploadFlashDialog = 0;

if ( w )
    {
    EndDialog( w, 0 );
    }
}

/***********************************************************************
*                      UPLOAD_FLASH_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK upload_flash_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        UploadFlashDialog = hWnd;
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case EXIT_RADIO:
            case IDCANCEL:
                end_upload_flash_dialog();
                return TRUE;
            }
        break;

    case WM_CLOSE:
        end_upload_flash_dialog();
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    SEND_DATA_SERVER_SHOT_NOTIFICATION                *
*                                                                      *
*                           M01,VISITRAK,NS,1395                       *
***********************************************************************/
static void send_data_server_shot_notification( TCHAR * mach_part, TCHAR * shot )
{
static TCHAR ns[] = TEXT( ",NS," );
static STRING_CLASS s;

s = mach_part;
s += ns;
s += shot;

poke_data( DDE_MONITOR_TOPIC, ItemList[DS_NOTIFY_INDEX].name, s.text() );
}

/***********************************************************************
*                         POKE_NEWSHOT_MESSAGE                         *
***********************************************************************/
void poke_newshot_message( WPARAM wParam, LPARAM lParam )
{
TCHAR * buf;
TCHAR * shot;
TCHAR * cp;

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
shot = (TCHAR *) wParam;

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
-------------------------------------------------
I have to clean up the memory used by the strings
------------------------------------------------- */
delete[] buf;
delete[] shot;
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
*                          PASSWORD_LEVEL_CALLBACK                     *
***********************************************************************/
void password_level_callback( TCHAR * topic, short item, HDDEDATA edata )
{
TCHAR buf[MAX_INTEGER_LEN+3];
DWORD bytes_copied;
DWORD bufsize = sizeof( buf );
INI_CLASS ini;

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

/*
-----------------------------------
Reload the Service Tools Gray Level
----------------------------------- */
ini.set_file( ini_file_name(SureTrakIniFile) );
ini.set_section( ConfigSection );
ServiceToolsGrayLevel = ini.get_int( ServiceToolsGrayLevelKey );

show_buttons();
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
*                         NEED_CONNECTED_BRUSH                         *
***********************************************************************/
static HBRUSH need_connected_brush( HWND w )
{
STRING_CLASS s;

s.get_text( w );
if ( s == NotConnected )
    return RedBackgroundBrush;

return 0;
}

/***********************************************************************
*                          NEED_MONITOR_BRUSH                          *
***********************************************************************/
static HBRUSH need_monitor_brush( HWND w )
{
TCHAR buf[101];

if ( get_text(buf, w, 100) )
    {
    if ( compare( buf, CycleString, CycleStringLen) == 0 )
        return GreenBackgroundBrush;

    if ( compare( buf, FatalString, FatalStringLen) == 0 )
        return RedBackgroundBrush;

    if ( compare( buf, WarningString, WarningStringLen) == 0 )
        return OrangeBackgroundBrush;

    if ( compare( buf, NoFileString, NoFileStringLen) == 0 )
        return RedBackgroundBrush;

    if ( UploadFailed == buf )
        return RedBackgroundBrush;

    if ( NoResponse == buf )
        return RedBackgroundBrush;
    }

return 0;
}

HDC MyDc;

/***********************************************************************
*                               TEXT_OUT                               *
***********************************************************************/
static void text_out( int x, int y, TCHAR * sorc )
{
TextOut( MyDc, (int) x, (int) y, sorc, lstrlen(sorc) );
}

/**********************************************************************
*                                CANEXIT                              *
**********************************************************************/
bool canexit()
{
NAME_CLASS s;
INI_CLASS  ini;
int        pw_level_needed;

/*
------------------------------
Always shut down from eventman
------------------------------ */
if ( ShutdownAnyway )
    return true;

s.get_exe_dir_file_name( SureTrakIniFile );
ini.set_file( s.text() );
pw_level_needed = ini.get_int( ConfigSection, ExitEnableLevelKey );

return ( pw_level_needed <= CurrentPasswordLevel );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
static int  i;
static int  id;
static int  cmd;
static bool is_diagnostic_window;
HWND w;
TCHAR * s;
HBRUSH   my_brush;
COLORREF my_color;
int      mymode;
BOOLEAN  checked;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        MainWindow    = hWnd;
        CurrentWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        return 0;

    case WM_DBINIT:
        hourglass_cursor();
        set_menu_view_state( LoggingSentCommands, MainWindow.handle(), (UINT) FTII_LOG_MESSAGES_CHOICE );
        MainWindow.getrect( OriginalRect );
        if ( !MainWindow.is_minimized() )
            HaveRects = true;
        client_dde_startup( hWnd );
        register_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX, new_monitor_setup_callback );
        register_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX,     new_downtime_callback );
        register_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX,   plot_min_callback          );
        register_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX,   password_level_callback    );
        create_controls( load_ftii_boards() );
        get_current_password_level();
        if ( IsRemoteMonitor )
            {
            MyMonitorSetupCount++;
            poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentMachine.name );
            temp_message( TEXT("Shutting Down Current Monitor..."), 0 );
            SetTimer( MainWindow.handle(), REMOTE_MONITOR_TIMER_ID, 3000, 0 );
            }
        else
            {
            begin_monitoring();
            restore_cursor();
            }
        return 0;

    case WM_SYSCOMMAND:
        if ( wParam == SC_CLOSE )
            {
            if ( !canexit() )
                {
                resource_message_box( LOW_PW_LEVEL_STRING, UNABLE_TO_COMPLY_STRING );
                return 0;
                }
            }
        break;

    case WM_SIZE:
        resize_terminal();
        break;

    case WM_MOVE:
        move_terminal();
        break;

    case WM_CTLCOLORSTATIC:
        w = (HWND) lParam;
        for ( i=0; i<NofRows; i++ )
            {
            if ( w == MonitorStatusTbox[i].w )
                {
                my_brush = need_monitor_brush( w );
                if ( my_brush )
                    {
                    if ( my_brush == GreenBackgroundBrush )
                        my_color = GreenColor;
                    else if ( my_brush == OrangeBackgroundBrush )
                        my_color = OrangeColor;
                    else
                        my_color = RedColor;
                    SetBkColor( (HDC) wParam, my_color );
                    return (int) my_brush;
                    }
                break;
                }
            if ( w == ConnectedTbox[i].w )
                {
                my_brush = need_connected_brush( w );
                if ( my_brush )
                    {
                    my_color = RedColor;
                    SetBkColor( (HDC) wParam, my_color );
                    return (int) my_brush;
                    }
                }
            }
        break;

    case WM_TIMER:
        if ( RESUME_TIMER_ID == (UINT) wParam )
            {
            KillTimer( hWnd, RESUME_TIMER_ID );
            if ( WaitingToResume )
                set_current_window( ResumeDialog );
            }
        else if ( CLOSING_TIMER_ID == (UINT) wParam )
            {
            if ( ShutdownState == SHUTTING_DOWN_MONITORING )
                {
                if ( !BoardMonitorIsRunning )
                    {
                    ShutdownState = SHUTTING_DOWN_SHOTSAVE;
                    if ( ShotSaveIsRunning )
                        {
                        ShutdownStatusListbox.add( resource_string(SHUTTING_DOWN_SHOTSAVE_STRING) );
                        SetEvent( ShotSaveEvent );
                        return 0;
                        }
                    else
                        {
                        ShutdownStatusListbox.add( resource_string(SHOTSAVE_NOT_RUNNING_STRING) );
                        }
                    }

                }

            if ( ShutdownState == SHUTTING_DOWN_SHOTSAVE )
                {
                if ( !ShotSaveIsRunning )
                    {
                    if ( IsRemoteMonitor )
                        {
                        ShutdownState = SHUTTING_DOWN_REMOTE;
                        remote_shutdown();
                        ShutdownStatusListbox.add( resource_string(SHUTTING_DOWN_REMOTE_STRING) );
                        return 0;
                        }
                    else
                        {
                        ShutdownState = SHUTDOWN_COMPLETE;
                        }
                    }
                }


            if ( ShutdownState == SHUTTING_DOWN_REMOTE )
                ShutdownState = SHUTDOWN_COMPLETE;

            if ( ShutdownState == SHUTDOWN_COMPLETE )
                {
                KillTimer( hWnd, CLOSING_TIMER_ID );
                MainWindow.post( WM_CLOSE );
                }
            }
        else if ( REMOTE_MONITOR_TIMER_ID == (UINT) wParam )
            {
            KillTimer( hWnd, REMOTE_MONITOR_TIMER_ID );
            kill_temp_message();
            begin_monitoring();
            restore_cursor();
            }
        else if ( HIRES_SENSOR_TEST_TIMER_ID == (UINT) wParam )
            {
            KillTimer( hWnd, HIRES_SENSOR_TEST_TIMER_ID );
            do_hires_sensor_test_timer( hWnd );
            }
        return 0;

    case WM_EV_SHUTDOWN:
        ShutdownAnyway = true;
        PostMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_NEW_RT_DATA:
        do_new_rt_data( wParam, lParam );
        return 0;

    case WM_NEW_FT2_STATE:
        do_new_ft2_connect_state( wParam, lParam );
        return 0;

    case WM_NEWSHOT:
        if ( lParam && wParam )
            poke_newshot_message( wParam, lParam );

        return 0;

    case WM_POKEMON:
        /*
        -----------------------------------------------------------
        wParam is the index to the item name, lParam is a pointer
        to a string allocated for this purpose (which I must free).
        ----------------------------------------------------------- */
        s = 0;
        if ( lParam )
            {
            s = (TCHAR *) lParam;
            poke_data( DDE_MONITOR_TOPIC, ItemList[wParam].name, s );

            if ( wParam == (WPARAM) DOWN_DATA_INDEX )
                send_data_server_down_notification( s );

            /*
            -------------------------------------------------------------
            I have to clean up the memory used by the machine name string
            ------------------------------------------------------------- */
            delete[] s;
            }
        return 0;

    case WM_COMMAND:
        if ( cmd == BN_CLICKED )

            {
            for ( i=0; i<NofRows; i++ )
                {
                if ( id == MainPushButton[i].id )
                    {
                    is_diagnostic_window = (CurrentWindow == SetDacDialog) || (CurrentWindow == SetNullDialog) || (CurrentWindow == CurrentValuesDialog);
                    if ( is_diagnostic_window )
                       send_command_to_current_machine( DONT_SEND_OP_DATA_PACKETS,   true );
                    get_text( CurrentMachine.name, MachineTbox[i].w, MACHINE_NAME_LEN );
                    CurrentMachine.find( ComputerName.text(), CurrentMachine.name );
                    if ( IsRemoteMonitor )
                        CurrentMachine.monitor_flags |= MA_MONITORING_ON;
                    CurrentPart.find( CurrentMachine.computer, CurrentMachine.name, CurrentMachine.current_part );
                    CurrentBoardIndex = current_board_index();
                    load_machine_vars();

                    mymode = current_channel_mode();

                    checked = FALSE;
                    if ( mymode == CHANNEL_MODE_5 || mymode == CHANNEL_MODE_7 )
                        checked = TRUE;
                    set_checkbox( SwitchChannelsXbox, checked );
                    set_channel_5_8_labels( mymode );

                    clear_current_value_copies();
                    if ( is_diagnostic_window )
                        send_command_to_current_machine( SEND_OP_DATA_PACKETS,   true );
                    if ( CurrentWindow != MainWindow )
                        fill_global_parameters();
                    if ( CurrentWindow == CurrentValuesDialog )
                        CurrentValuesDialog.post( WM_DBINIT );
                    /*
                    ---------------------------------------------------
                    Show or hide the controls for the digital servo amp
                    --------------------------------------------------- */
                    show_revision_d_controls();
                    refresh_valve_test_screen();

                    return 0;
                    }
                }
            }

        switch (id)
            {
            case F2_KEY:
            case F3_KEY:
            case F4_KEY:
            case F5_KEY:
            case F6_KEY:
            case F7_KEY:
            case F8_KEY:
            case F9_KEY:
            case F10_KEY:
            case EXIT_PB:
                if ( !WaitingToResume )
                    {
                    if ( CurrentWindow == MainWindow )
                        {
                        if ( CurrentMachine.monitor_flags & MA_MONITORING_ON )
                            set_current_window( ButtonDialog );
                        else
                            resource_message_box( NOT_MONITORED_STRING, UNABLE_TO_COMPLY_STRING );
                        }
                    else
                        {
                        if ( cmd == BN_CLICKED || cmd == 1 )     // 1 set by TranslateAccelerator
                            {
                            if ( CurrentPasswordLevel < ServiceToolsGrayLevel )
                                {
                                if ( id >= F2_KEY && id <= F10_KEY )
                                    id = F8_KEY;
                                }
                            switch ( id )
                                {
                                case F2_KEY:
                                    start_set_dac_dialog();
                                    break;

                                case F3_KEY:
                                    start_null_dialog();
                                    break;

                                case F4_KEY:
                                    start_valve_test_dialog();
                                    break;

                                case F5_KEY:
                                    start_alarm_test_dialog();
                                    break;

                                case F8_KEY:
                                    set_current_window( CurrentValuesDialog );
                                    break;

                                case F9_KEY:
                                    set_current_window( GlobalParameterDialog );
                                    break;

                                case F10_KEY:
                                    set_current_window( ControlModsDialog );
                                    break;

                                case EXIT_PB:
                                    set_current_window( MainWindow );
                                    break;
                                }

                            if ( id != EXIT_PB )
                                CheckRadioButton( ButtonDialog.handle(), F2_RADIO, EXIT_RADIO, (F2_RADIO + id - F2_KEY) );
                            }
                        }
                    }
                return 0;

            case MINIMIZE_MENU_CHOICE:
                MainWindow.minimize();
                return 0;

            case FTII_SAVE_LISTBOX_CHOICE:
                save_listbox();
                return 0;

            case FTII_HIRES_SENSOR_TEST_CHOICE:
                HiresSensorTestDialog.show();
                return 0;

            case FTII_CHECK_CONNECTION_CHOICE:
                send_command_to_current_machine( TEXT("V319"), true );
                return 0;

            case FTII_CLEAR_DO_NOT_UPLOAD_CHOICE:
                send_command_to_current_machine( CLEAR_DO_NOT_UPLOAD_CMD, false );
                return 0;

            case FTII_RESET_MACHINE_CHOICE:
                send_command_to_current_machine( CLEAR_DO_NOT_UPLOAD_CMD, false );
                send_command_to_current_machine( RESET_MACHINE_CMD, true );
                if ( NeedErrorLog )
                    write_log( ResetString );
                return 0;

            case FTII_SETUP_MENU_CHOICE:
                if ( CurrentMachine.monitor_flags & MA_MONITORING_ON )
                    set_current_window( ButtonDialog );
                else
                    resource_message_box( NOT_MONITORED_STRING, UNABLE_TO_COMPLY_STRING );
                return 0;

            case FTII_TERMINAL_CHOICE:
                toggle_terminal();
                return 0;

            case FTII_CLEAR_TERMINAL_CHOICE:
                MainListBox.empty();
                return 0;

            case FTII_LOG_MESSAGES_CHOICE:
                toggle_logging_sent_commands();
                return 0;

            case EXIT_MENU_CHOICE:
                if ( canexit() )
                    {
                    if ( ShutdownState == NOT_SHUTTING_DOWN )
                        MainWindow.post( WM_CLOSE );
                    else if ( ShutdownState == SHUTTING_DOWN_MONITORING )
                        BoardMonitorIsRunning = false;
                    else if ( ShutdownState == SHUTTING_DOWN_SHOTSAVE )
                        ShotSaveIsRunning = false;
                    }
                else
                    {
                    resource_message_box( LOW_PW_LEVEL_STRING, UNABLE_TO_COMPLY_STRING );
                    }
                return 0;

            case FTII_OUTPUT_LIST_CHOICE:
                OutputListDialog.show();
                output_list_update();
                if ( TerminalIsVisible )
                    SetFocus( MainEbox );
                return 0;

            case UPLOAD_FLASH_MENU_CHOICE:
                DialogBox(
                    MainInstance,
                    TEXT( "UPLOAD_FLASH_DIALOG" ),
                    MainWindow.handle(),
                    (DLGPROC) upload_flash_dialog_proc
                     );
                return 0;

            case IDHELP:
                get_help();
                return 0;
            }
        break;

    case WM_HELP:
        get_help();
        return 0;

    case WM_NEWSETUP:
        send_command( (TCHAR *) lParam, RESET_MACHINE_CMD, true );
        delete[] (TCHAR *) lParam;
        if ( CurrentWindow != MainWindow )
            fill_limit_switch_descriptions();
        create_tooltips();
        return 0;

    case WM_CLOSE:
        if ( CurrentWindow != MainWindow )
            {
            NeedToCloseMainWindow = true;
            set_current_window( MainWindow );
            return 0;
            }

        if ( ShutdownState == NOT_SHUTTING_DOWN )
            {
            ShutdownState = SHUTTING_DOWN_MONITORING;
            stop_monitoring();
            ShutdownStatusDialog  = CreateDialog(
               MainInstance,
               TEXT("SHUTDOWN_STATUS_DIALOG"),
               MainWindow.handle(),
               (DLGPROC) shutdown_status_dialog_proc );
            ShutdownStatusDialog.show();
            ShutdownStatusListbox.init( ShutdownStatusDialog.control(SHUTDOWN_STATUS_LB) );
            ShutdownStatusListbox.add( resource_string(SHUTTING_DOWN_MONITOR_STRING) );
            SetTimer( MainWindow.handle(), CLOSING_TIMER_ID, 100, 0 );
            }

        if ( ShutdownState == SHUTDOWN_COMPLETE )
            {
            ShutdownStatusListbox.add( TEXT("Exiting from main dialog proc...") );
            break;
            }
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                          GET_HISTORY_LENGTH                          *
***********************************************************************/
static void get_history_length()
{
INI_CLASS ini;
STRING_CLASS fname;

fname = exe_directory();
fname.cat_path( VisiTrakIniFile );

ini.set_file( fname.text() );

if ( ini.find(ConfigSection, LastNKey) )
    HistoryShotCount = ini.get_int();
}

/***********************************************************************
*                        CHECK_DISK_FREE_SPACE                         *
***********************************************************************/
static void check_disk_free_space()
{
TCHAR root[MAX_PATH];

if ( get_root_directory(root) )
    {
    if ( free_kilobytes(root) < MIN_FREE_DISK_KB )
        resource_message_box( LOW_DISK_SPACE_STRING, NO_SHOTS_SAVED_STRING );
    }
}

/***********************************************************************
*                        CHECK_FOR_WORKSTATION                         *
* If this computer has no machines of it's own then this is a laptop   *
* office workstation that has been taken out and hooked to a dcm.      *
*                                                                      *
* I need to ask which machine I am going to monitor and then take      *
* over monitoring from the normal DCT.                                 *
*                                                                      *
***********************************************************************/
static void check_for_workstation()
{
NAME_CLASS s;
DB_TABLE   t;
int        n;

s.get_machset_file_name( ComputerName.text() );
if ( t.open(s.text(), MACHSET_RECLEN, PFL) )
    {
    n = t.nof_recs();
    t.close();
    if ( n < 1 )
        {
        DialogBox(
            MainInstance,
            TEXT("CHOOSE_MONITOR_MACHINE_DIALOG"),
            MainWindow.handle(),
            (DLGPROC) choose_monitor_machine_dialog_proc
            );
        }
    }
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOLEAN init( int cmd_show )
{
WNDCLASS       wc;
COMPUTER_CLASS c;
STRING_CLASS   s;
INI_CLASS     ini;
int           w;
TIME_CLASS t;
FILE_CLASS f;

if ( is_previous_instance(MyClassName, 0) )
    return FALSE;

if ( names_startup() )
    {
    c.startup();
    ComputerName = c.whoami();
    check_for_workstation();
    get_history_length();
    skip_startup();
    shifts_startup();
    dcurve_startup();
    units_startup();
    AnalogSensor.get();
    check_disk_free_space();
    }

s = default_ftii_control_message_name();
StepMessage.load( s.text() );

/*
-----------------------------------------
Initialize the downtime and alarm options
----------------------------------------- */
ini.set_file( ini_file_name(MonallIniFile) );
ini.set_section( ConfigSection );

ClearAlarmsEachShot         = ini.get_boolean( ClearAlarmsEachShotKey );
ClearDownWireFromPopup      = ini.get_boolean( ClearDownWireFromPopupKey );
ClearMonitorWireOnDown      = ini.get_boolean( ClearMonitorWireOnDownKey );
IsHiresSensor               = ini.get_boolean( IsHiresSensorKey );
SaveAllAlarmShots           = ini.get_boolean( SaveAllAlarmShotsKey      );
SubtractTimeoutFromAutoDown = ini.get_boolean( SubtractTimeoutKey        );
DownTimeAlarmWire           = ini.get_int( DownTimeAlarmWireKey );
MonitorWire                 = ini.get_int( MonitorWireKey       );
AlsLogIsActive              = ini.get_boolean( AlsLogIsActiveKey );
LoggingSentCommands         = ini.get_boolean( LogSentCommandsKey );

/*
------------------------------------------------------------------------------------------
Many systems used to hold AT_HOME high so I can't use that to tell when the
shot is over and it is ok to upload. I use the retract signal instead and wait
two seconds instead. You can turn this off by setting UploadRetractTimeout=0 in monall.ini
------------------------------------------------------------------------------------------ */
if ( ini.find(UploadRetractTimeoutKey) )
    UploadRetractTimeout = (unsigned) ini.get_ul();
else
    UploadRetractTimeout = 2;

ini.set_file( ini_file_name(SureTrakIniFile) );
ini.set_section( ConfigSection );

UsingValveTestOutput  = ini.get_boolean( UseValveTestOutputKey );
UsingNullValveOutput  = ini.get_boolean( UseNullValveOutputKey );
ServiceToolsGrayLevel = ini.get_int( ServiceToolsGrayLevelKey );

/*
-------
Uploads
------- */
NoUploadWire  = ini.get_ul( NoUploadWireKey );
NoUploadWireMask = 0;
if ( NoUploadWire )
    NoUploadWireMask = mask_from_wire( NoUploadWire );

RetractWire = ini.get_ul( RetractWireKey );
if ( RetractWire == NO_WIRE )
    RetractWire = 20;
RetractWireMask = mask_from_wire( RetractWire );

if ( AlsLogIsActive )
    {
    t.get_local_time();
    s = TEXT("PROGRAM START ");
    s += t.hhmmss();
    if ( f.open_for_append(AlsLogFile) )
        {
        f.writeline( s.text() );
        f.close();
        }
    }

/*
-------------------------------
Load the monitor status strings
------------------------------- */
NoConnectState = resource_string( NO_CONNECT_STRING );
NotMonitored   = resource_string( NOT_MONITORED_STRING );
Connected      = resource_string( CONNECTED_STRING );
Connecting     = resource_string( CONNECTING_STRING    );
NotConnected   = resource_string( NOT_CONNECTED_STRING );
ZeroStrokeLength = resource_string( ZERO_STROKE_LENGTH_STRING );
UploadFailed   = resource_string( UPLOAD_FAIL_STRING );
NoResponse     = resource_string( NO_RESPONSE_STRING );
/*
---------------------------------------
Create an event for the shotsave thread
--------------------------------------- */
ShotSaveEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !ShotSaveEvent )
    resource_message_box( UNABLE_TO_CONTINUE_STRING, SHOTSAVE_EVENT_FAIL_STRING );

register_vtww_opto_control();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MyClassName );
wc.lpszMenuName  = TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1); //GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

s = resource_string( MainInstance, MONITOR_WINDOW_TITLE_STRING );

w = 765;

#ifdef UNICODE
if ( PRIMARYLANGID(GetUserDefaultLangID())==LANG_JAPANESE )
    w = 790;
#endif

CreateWindow(
    MyClassName,
    s.text(),
    WS_OVERLAPPEDWINDOW,
    0, 100,     // X,y
    w, 90,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );

start_shotsave_thread();

GreenBackgroundBrush  = CreateSolidBrush ( GreenColor );
OrangeBackgroundBrush = CreateSolidBrush ( OrangeColor );
RedBackgroundBrush    = CreateSolidBrush ( RedColor );

AutoShotDisplayWindow  = CreateDialog(
   MainInstance,
   TEXT("AUTO_SHOT"),
   MainWindow.handle(),
   (DLGPROC) AutoShotDisplayProc );

AutoShotDisplayWindow.hide();

Ft2TestDialog  = CreateDialog(
   MainInstance,
   TEXT("FT2_TEST_DIALOG"),
   MainWindow.handle(),
   (DLGPROC) ft2_test_dialog_proc );

Ft2TestDialog.hide();

CreateDialog(
    MainInstance,
    TEXT("BUTTON_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) button_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("OUTPUT_LIST_DIALOG"),
    NULL,
    (DLGPROC) output_list_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("HIRES_SENSOR_TEST_DIALOG"),
    NULL,
    (DLGPROC) hires_sensor_test_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("UPLOAD_ABORTED_DIALOG"),
    NULL,
    (DLGPROC) upload_aborted_dialog_proc );

return TRUE;
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( void )
{
unregister_for_event( DDE_CONFIG_TOPIC,  PW_LEVEL_INDEX      );
unregister_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX       );
unregister_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX     );
unregister_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX );
client_dde_shutdown();

if ( ShotSaveEvent )
    {
    CloseHandle( ShotSaveEvent );
    ShotSaveEvent = 0;
    }

if ( GreenBackgroundBrush )
    {
    DeleteObject( GreenBackgroundBrush );
    GreenBackgroundBrush = 0;
    }

if ( OrangeBackgroundBrush )
    {
    DeleteObject( OrangeBackgroundBrush );
    OrangeBackgroundBrush = 0;
    }

if ( RedBackgroundBrush )
    {
    DeleteObject( RedBackgroundBrush );
    RedBackgroundBrush = 0;
    }

if ( Dnu )
    {
    delete[] Dnu;
    Dnu = 0;
    }

if ( MainPushButton )
    {
    delete[] MainPushButton;
    MainPushButton = 0;
    }

if ( AddressTbox )
    {
    delete[] AddressTbox;
    AddressTbox = 0;
    }

if ( PortTbox )
    {
    delete[] PortTbox;
    PortTbox = 0;
    }

if ( ConnectedTbox )
    {
    delete[] ConnectedTbox;
    ConnectedTbox = 0;
    }

if ( MachineTbox )
    {
    delete[] MachineTbox;
    MachineTbox = 0;
    }

if ( PartTbox )
    {
    delete[] PartTbox;
    PartTbox = 0;
    }

if ( MonitorStatusTbox )
    {
    delete[] MonitorStatusTbox;
    MonitorStatusTbox = 0;
    }

dcurve_shutdown();
skip_shutdown();
shifts_shutdown();
units_shutdown();

shutdown_vtww_opto_control();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG     msg;
BOOL    status;

#ifdef _DEBUG
int     flags;

flags  = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG);
flags |= _CRTDBG_CHECK_ALWAYS_DF;
_CrtSetDbgFlag( flags );

#endif

InitCommonControls();

MainInstance = this_instance;

if ( !init(cmd_show) )
    return 0;

AccelHandle = LoadAccelerators( MainInstance, AccelName );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status && MainWindow.handle() )
        status = TranslateAccelerator( MainWindow.handle(), AccelHandle, &msg );

    if ( !status && AutoShotDisplayWindow.handle() )
        status = IsDialogMessage( AutoShotDisplayWindow.handle(), &msg );

    if ( !status && GlobalParameterDialog.handle() )
        status = IsDialogMessage( GlobalParameterDialog.handle(), &msg );

    if ( !status && ControlModsDialog.handle() )
        status = IsDialogMessage( ControlModsDialog.handle(), &msg );

    if ( !status && SetDacDialog.handle() )
        status = IsDialogMessage( SetDacDialog.handle(), &msg );

    if ( !status && CurrentValuesDialog.handle() )
        status = IsDialogMessage( CurrentValuesDialog.handle(), &msg );

    if ( !status && SetNullDialog.handle() )
        status = IsDialogMessage( SetNullDialog.handle(), &msg );

    if ( !status && ValveTestDialog.handle() )
        status = IsDialogMessage( ValveTestDialog.handle(), &msg );

    if ( !status && Ft2TestDialog.handle() )
        status = IsDialogMessage( Ft2TestDialog.handle(), &msg );

    if ( !status && OutputListDialog.handle() )
        status = IsDialogMessage( OutputListDialog.handle(), &msg );

    if ( !status && TestAlarmOutputsDialog.handle() )
        status = IsDialogMessage( TestAlarmOutputsDialog.handle(), &msg );

    if ( !status && ShutdownStatusDialog.handle() )
        status = IsDialogMessage( ShutdownStatusDialog.handle(), &msg );

    if ( !status && HiresSensorTestDialog.handle() )
        status = IsDialogMessage( HiresSensorTestDialog.handle(), &msg );

    if ( !status && UploadAbortedDialog.handle() )
        status = IsDialogMessage( UploadAbortedDialog.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();

_CrtDumpMemoryLeaks();

return(msg.wParam);
}