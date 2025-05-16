#include <windows.h>
#include <commctrl.h>
#include <ddeml.h>

#include "..\include\visiparm.h"
#include "..\include\catclass.h"
#include "..\include\chaxis.h"
#include "..\include\color.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\evclass.h"
#include "..\include\fileclas.h"
#include "..\include\ftanalog.h"
#include "..\include\ft.h"
#include "..\include\iniclass.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\part_analog_sensor.h"
#include "..\include\alarmsum.h"
#include "..\include\marks.h"
#include "..\include\rectclas.h"
#include "..\include\textlen.h"
#include "..\include\textlist.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\param_index_class.h"
#include "..\include\subs.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\timeclas.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"

#define _MAIN_
#include "..\include\events.h"

#include "gridclas.h"
#include "rtmlist.h"
#include "resource.h"
#include "extern.h"

const int NO_DISPLAY        = 0;
const int POS_TIME_DISPLAY  = 1;
const int POS_ONLY_DISPLAY  = 2;
const int TIME_ONLY_DISPLAY = 3;    /* Plot.cpp knows this too */

#define REAL_TIME_MAIN_MENU    118
#define REAL_TIME_MENU         103
#define OVERLAY_MENU           101
#define CORNER_PARAMETERS_MENU 104
#define ABOUT_BOX_MENU         105
#define WHATS_NEW_MENU         106

#define DISPLAY_CONFIGURATION_MENU 605
#define COLOR_DEFINITION_MENU      606
#define CHOOSE_RT_MACHINES_MENU    608

#define CHOOSE_PART_MENU          113
#define CHOOSE_SHOT_MENU          109
#define EXIT_MENU                 108
#define MINIMIZE_MENU             114
#define SAVE_MENU                 115
#define SAVE_AS_MASTER_MENU       116
#define PRINT_MENU                117
#define MULTIPLE_SHOT_PRINT_MENU  119
#define CLEAR_BACKUP_LIST_MENU    120
#define ADD_TO_BACKUP_LIST_MENU   121
#define MACHINE_LIST_MENU         122

#define POS_TIME_MENU             501
#define POS_ONLY_MENU             502
#define TIME_ONLY_MENU            503
#define LAST_50_SCREEN_MENU       504
#define STD_SCREEN_MENU           505
#define VIEW_REFERENCE_TRACE_MENU 506
#define VIEW_PARAMETERS_MENU      507
#define BOX_SCREEN_MENU           508
#define TREND_SCREEN_MENU         509
#define CHOOSE_TREND_PARAMS_MENU  510
#define TREND_CONFIG_MENU         511
#define TREND_REALTIME_MENU       512
#define PROFILE_SCREEN_MENU       513
#define DOWNBAR_SCREEN_MENU       514
#define DOWNBAR_CONFIG_MENU       515
#define TREND_ALLSHOTS_MENU       516
#define STAT_SUMMARY_MENU         517
#define NO_STAT_SUMMARY_MENU      518
#define STATUS_SCREEN_MENU        519
#define EDIT_STATUS_SCREEN_MENU   520
#define SAVE_STATUS_SCREEN_MENU   521
#define XDCR_MENU                 522
#define FONT_STATUS_SCREEN_MENU   523

#define PART_EDITOR_MENU          701
#define MACHINE_EDITOR_MENU       702
#define BOARD_MONITOR_MENU        703
#define MONITOR_CONFIG_MENU       704
#define SENSORS_MENU              705
#define BACKUP_AND_RESTORE_MENU   706
#define SETUPS_MENU               707
#define DOWNTIME_MENU             708
#define EDITDOWN_MENU             709
#define SURETRAK_CONTROL_MENU     710
#define VISIEDIT_MENU             711
#define EDIT_SHIFTS_MENU          712
#define STANDARD_DOWNTIMES_MENU   713
#define CHOOSE_DOWNTIME_MACHINE_MENU 714
#define SHIFT_REPORT_MENU         715           /* Consecutive run_tool exes */

#define OPERATOR_SIGNON_MENU      800
#define EXPORT_SHOTS_MENU         801
#define ALARM_SUMMARY_MENU        803
#define PASSWORDS_MENU            804
#define RESTORE_PASSWORD_MENU     805
#define ALARM_LOG_SETUP_MENU      806
#define EXPORT_ALARMS_MENU        807
#define CSV_SHOT_FILE_MENU        808
#define DOWNTIME_POPUP_MENU       809
#define RESTORE_FOCUS_TO_HMI_MENU 810
#define HELP_MENU                 900
#define FT2_HELP_MENU             901

/*
-----------------
WM_NEW_DATA types
----------------- */
const WPARAM LOAD_MAIN_MENU_REQ = 1;
const WPARAM SCREEN_REFRESH_REQ = 2;

/*
-----------------
Zoom box controls
----------------- */
#define POS_TEXT_BOX      110
#define TIME_TEXT_BOX     120
#define VELOCITY_TEXT_BOX 130
#define ROD_TEXT_BOX      140
#define HEAD_TEXT_BOX     150
#define DIFF_TEXT_BOX     160
#define CHAN_4_TEXT_BOX   170
#define START_BOX_OFFSET    0
#define END_BOX_OFFSET      1
#define DELTA_BOX_OFFSET    2

/*
--------------------------------
Choose corner parameter controls
-------------------------------- */
#define CORNER_PARAMETER_LISTBOX    101
#define AVAILABLE_PARAMETER_LISTBOX 102

/*
-------
Strings
------- */
#define LISTBOX_FULL_MESSAGE          1
#define HEAD_PRESSURE_LABEL_STRING    3
#define ROD_PRESSURE_LABEL_STRING     4
#define DIFF_PRESSURE_LABEL_STRING    5
#define DISTANCE_TYPE_STRING          6
#define POSITION_TYPE_STRING          7
#define PRESSURE_TYPE_STRING          8
#define TIME_TYPE_STRING             10
#define VELOCITY_TYPE_STRING         12
#define CHANNEL_TYPE_STRING          36
#define CHANNEL_1_LABEL_STRING       16
#define CHANNEL_2_LABEL_STRING       17
#define CHANNEL_3_LABEL_STRING       18
#define CHANNEL_4_LABEL_STRING       19


const UINT PASSWORD_LEVEL_TIMEOUT_TIMER_NUMBER = 2;

/*
-------------------------------
Display corner parameter titles
------------------------------- */
STRING_CLASS ParameterListTitle[NOF_PARAMETER_LIST_TITLES];

/*
------------------------------------
Display corner parameter limit types
------------------------------------ */
int LowLimitType  = ALARM_MIN;
int HighLimitType = ALARM_MAX;

/*
---------------------------------
Display corner parameter controls
--------------------------------- */
#define FIRST_CORNER_PARAM_NAME         110
#define FIRST_CORNER_PARAM_VALUE        130
#define FIRST_CORNER_PARAM_UNITS        150
#define DISPLAY_PARAMETER_LISTBOX       101

#define LEFT_AXIS_TYPE   1
#define RIGHT_AXIS_TYPE  2

#define POS_BASED_X_TICS  7
#define TIME_BASED_X_TICS 4
#define ANALOG_Y_TICS     4
#define COUNT_Y_TICS      4

HACCEL  AccelHandle;
TCHAR   AccelName[] = TEXT("PROFILE_HOT_KEYS");

HWND    MainPlotWindow              = 0;
HWND    MenuHelpWindow              = 0;
HWND    ChospartWindow              = 0;
HWND    ChosshotWindow              = 0;
HWND    ParameterWindow             = 0; /* Corner Parameters */
HWND    ZoomBoxWindow               = 0;
HWND    SmallZoomBoxWindow          = 0;
HWND    CornerParamDialogWindow     = 0; /* Corner Parameter Setup */
HWND    ParamHistoryWindow          = 0;
HWND    ViewParametersDialogWindow  = 0;
HWND    ColorDefinitionDialogWindow = 0;
HWND    AlarmSummaryDialogWindow    = 0;
HWND    LoadFromNetworkDialogWindow = 0;

bool    IsHPM = false;

BOOLEAN ReadyForZoom = FALSE;

GRID_CLASS Grid;
BOOLEAN    GridIsVisible = FALSE;

/*
-----------------------------
Number of columns in the grid
----------------------------- */
int NofColumns = NOF_STATSUM_STRINGS;

int     CurrentDisplay = POS_TIME_DISPLAY;
int     LastPosDisplay = POS_TIME_DISPLAY; /* For Display Toggle */

HDC     MyDc           = NULL;
int     NofDcOwners    = 0;
RECT    MainWindowRect;
RECT    PlotWindowRect;
RECT    PrinterMargin;
RECT    RealTimeButtonRect = { 0, 0, 0, 0};

TCHAR   MyClassName[]            = TEXT( "Profile" );
TCHAR   MyWindowTitle[]          = TEXT( "Plot" );
TCHAR   MainPlotWindowClass[]    = TEXT( "MainPlotClass" );
TCHAR   ParameterWindowClassName[] = TEXT( "ParameterWindowClass" );
TCHAR   XdcrWindowClass[]        = TEXT( "XdcrPlotClass" );

TCHAR   DefaultArialFontName[]          = TEXT( "Arial" );
TCHAR   DefaultArialBoldFontName[]      = TEXT( "Arial Bold" );
TCHAR   DefaultTimesRomanFontName[]     = TEXT( "Times New Roman" );

STRING_CLASS ArialFontName;
STRING_CLASS ArialBoldFontName;
STRING_CLASS TimesRomanFontName;

TCHAR   NewSetupMachine[MACHINE_NAME_LEN+1];

TCHAR   MainHelpFileName[]  = TEXT( "v5help.hlp" );
TCHAR   CpkHelpFileName[]   = TEXT( "cpk.hlp" );
TCHAR   ToolsHelpFileName[] = TEXT( "toolmenu.hlp" );

int     NofCornerParameters;
int     CornerParameters[MAX_PARMS];

short   MyScreenType      = STANDARD_SCREEN_TYPE;

static short StandardProfileScreenType = ZABISH_SCREEN_TYPE;

/*
------------------------------------
Cursor line and zoom state variables
------------------------------------ */
BOOLEAN HaveLeftCursorLine  = FALSE;
BOOLEAN HaveCursorLine      = FALSE;
BOOLEAN Zooming             = FALSE;
BOOLEAN Printing            = FALSE;

static BOOLEAN WasRealtimeBeforeZoom      = FALSE;
static BOOLEAN WasSingleMachineBeforeZoom = FALSE;
static int     DisplayBeforeZoom          = POS_TIME_DISPLAY;

static BOOLEAN HaveMonitorProgram = TRUE;
static BOOLEAN HaveSureTrakControl = FALSE;
static BOOLEAN UseDowntimeEntry    = FALSE;

BOOLEAN QuickPrint = FALSE;

static int CursorStartY;
static int CursorEndY;
static int LeftCursorX;
static int CursorX;

static int LeftZoomIndex = 0;
static int ZoomIndex     = 0;
const  int NofZoomAnalogChannels = 4;
static BITSETYPE ZoomAnalogType[NofZoomAnalogChannels] = {0, 0, 0, 0};

/*
----------------------------------------
Channel axis labels for the current part
---------------------------------------- */
CHAXIS_LABEL_CLASS ChannelAxisLabel;

/*
-----------------------
Alarm summary variables
----------------------- */
static SYSTEMTIME FirstAlarmDate;
static int32      FirstAlarmShift;
static SYSTEMTIME CurrentAlarmDate;
static int32      CurrentAlarmShift;
static SYSTEMTIME LastAlarmDate;
static int32      LastAlarmShift;

/*
--------------------------------------------
Font for the live shot/selected shot control
-------------------------------------------- */
HFONT RealTimeFont = NULL;

/*
-----------------------
Status screen variables
----------------------- */
BOOLEAN Editing_Status_Screen = FALSE;

/*
------------------------
Downtime entry variables
------------------------ */
static HWND DowntimeEntryDialog;
static HWND DowntimeEntryEbox;
static HWND DowntimeEntryOkButton;
static LISTBOX_CLASS DowntimeEntryLb;
static bool DowntimeEntryIsChanging = false;
static WNDPROC OldEditProc;
static STRING_CLASS DowntimeEntryMachine;
static SYSTEMTIME   DowntimeEntrySt;

#define PARAMETER_INFO_BOX 1
#define ZOOM_INFO_BOX      2
#define XDCR_INFO_BOX      3

static int InfoBoxType;
static BOOL NeedCornerParamUpdate;
static BOOL PTestIsRunning = FALSE;
static HWND PTestWindow    = 0;

static HFONT PrinterFont = 0;

static TCHAR CommaChar          = TEXT( ',' );
static TCHAR NChar              = TEXT( 'N' );
static TCHAR NullChar           = TEXT( '\0' );
static TCHAR PercentChar        = TEXT( '%' );
static TCHAR SpaceChar          = TEXT( ' ' );
static TCHAR TabChar            = TEXT( '\t' );
static TCHAR YChar              = TEXT( 'Y' );

static TCHAR CsvSuffix[]        = TEXT(".csv");
static TCHAR EmptyString[]      = TEXT("");
static TCHAR PercentString[]    = TEXT( "%" );
static TCHAR TabString[]        = TEXT( "\t" );
static TCHAR TwoSpacesString[]  = TEXT( "  " );
static TCHAR SpaceString[]      = TEXT( " " );
static TCHAR HmiSection[]       = TEXT( "HMI" );
static TCHAR ClassKey[]         = TEXT( "ClassName" );

BOOLEAN       begin_box_display( void );
BOOLEAN       begin_downbar_display( void );
BOOLEAN       begin_status_display();
BOOLEAN       begin_trend_display( void );
void          choose_status_screen_font();
void          downbar_left_button_up( POINT & p );
void          do_downbar_button( int id );
void          do_new_dtr_config( void );
void          do_new_tc_config( WPARAM force_change );
void          end_box_display( void );
void          end_downbar_display( void );
void          end_status_display();
void          end_trend_display( void );
BOOLEAN       box_display_startup( void );
void          mouse_to_trend( int32 x, int32 y );
void          paint_box_display( PAINTSTRUCT & ps );
void          paint_downbar_display( void );
void          paint_status_display( PAINTSTRUCT & ps );
void          paint_trend_display( const RECT & ur );
void          box_screen_down_data_change( TCHAR * machine_name, TCHAR * category, TCHAR * subcategory );
void          box_screen_left_button_up( POINT & p );
void          box_screen_new_machine_status( TCHAR * machine_name, BOARD_DATA new_status );
void          box_screen_new_monitor_setup( TCHAR * machine_name );
void          box_screen_new_shot( TCHAR * machine_name );
void          status_screen_down_data_change( TCHAR * machine_name, TCHAR * category, TCHAR * subcategory );
void          status_screen_left_button_up( POINT & p );
void          status_screen_new_machine_status( TCHAR * machine_name, BOARD_DATA new_status );
void          status_screen_new_shot( TCHAR * machine_name );
void          status_screen_timer_update();
void          save_status_config();
void          toggle_pressure_display_type();
void          trend_allshots( void );
void          trend_escape( void );
void          trend_screen_new_shot( void );
void          trend_left_button_up( POINT & p );
void          trend_screen_reset( void );
void          trend_realtime( void );

void    load_profile( void );
void    load_profile( PROFILE_NAME_ENTRY * p );
BOOLEAN load_reference_trace();
void init_global_curve_types();

void read_corner_parameters( int * dest, int & n );
void save_corner_parameters( int * sorc, int n );

BOOL CALLBACK chospart_proc(                     HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK chosshot_proc(                     HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK color_definition_dialog_proc(      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK corner_param_dialog_proc(          HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK display_configuration_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK downtime_report_setup_proc(        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK edit_operators_dialog_proc(        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK edit_shifts_dialog_proc(           HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK enter_password_proc(               HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK export_alarms_dialog_proc(         HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK export_shots_proc(                 HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK print_multiple_shots_proc(         HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK realtime_machine_list_proc(        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK trend_config_dialog_proc(          HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK view_parameters_dialog_proc(       HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK parameter_window_proc(             HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT CALLBACK xdcr_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

int CALLBACK ParamHistoryProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void position_parameter_window();
void position_param_history_window( void );
void position_view_parameters_window( void );
void toggle_profile_save_flags( void );
void save_as_master_trace( void );
BOOLEAN is_alarm( short parameter_index, float value );
BOOLEAN is_warning( short parameter_index, float value );
void fill_view_parameters_listbox( void );

int  default_password_level();
void restore_colors( void );
void save_colors( void );
void set_default_password_level( bool need_log );
void update_screen_type( short new_screen_type );
void print_alarm_summary( void );
void do_new_control_limits( void );
void set_smooth( int32 n, float factor );

BOOLEAN append_overlay_shot( void );
void pressure_display_check();
BOOLEAN write_olmsted_file( STRING_CLASS & destfile, STRING_CLASS & sorcfile );
BOOLEAN write_counts_file(  STRING_CLASS & destfile, STRING_CLASS & sorcfile );

/***********************************************************************
*                              OVERLAYING                              *
***********************************************************************/
bool overlaying()
{
if ( !OverlayMenuState )
    return false;

if ( IsHPM )
    return  (CurrentDisplay == TIME_ONLY_DISPLAY);

//if ( CurrentDisplay == POS_ONLY_DISPLAY )  Lawyer code
  //  return false;

return true;
}

/***********************************************************************
*                          VIEWING_REFERENCE                           *
***********************************************************************/
bool viewing_reference()
{
if ( !ViewReferenceMenuState )
    return false;

if ( IsHPM )
    return  (CurrentDisplay == TIME_ONLY_DISPLAY);

//if ( CurrentDisplay == POS_ONLY_DISPLAY ) Lawyer code
  //  return false;

return true;
}

/***********************************************************************
*                              DRAW_LINE                               *
***********************************************************************/
static void draw_line( int x1, int y1, int x2, int y2 )
{
MoveToEx( MyDc, x1, y1, 0 );
LineTo( MyDc, x2, y2 );
}

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                               TEXT_OUT                               *
***********************************************************************/
void text_out( int32 x, int32 y, TCHAR * sorc )
{

TextOut( MyDc, (int) x, (int) y, sorc, lstrlen(sorc) );
}

/***********************************************************************
*                               TEXT_OUT                               *
***********************************************************************/
void text_out( int32 x, int32 y, UINT sorc_id )
{
TCHAR * s;

s = resource_string( sorc_id );
if ( s )
    TextOut( MyDc, (int) x, (int) y, s, lstrlen(s) );
}

/***********************************************************************
*                         SET_ZOOM_BOX_LABELS                          *
***********************************************************************/
void set_zoom_box_labels( HWND w )
{
static STRING_CLASS pressurelabel[3];
static bool firstime = true;

int channel;
int c;
int i;
int id;
int zi;
STRING_CLASS s;

if ( firstime )
    {
    firstime = false;
    id = CHAN_1_ZOOM_TITLE_TBOX;
    for ( i=0; i<3; i++ )
        {
        pressurelabel[i].get_text( w, id );
        id++;
        }
    }

if ( IsPressureDisplay )
    {
    id = CHAN_1_ZOOM_TITLE_TBOX;
    for ( i=0; i<3; i++ )
        {
        pressurelabel[i].set_text( w, id );
        id++;
        }

    if ( CurrentMachine.diff_curve_number == NO_DIFF_CURVE_NUMBER )
        {
        set_text( w, CHAN_3_ZOOM_TITLE_TBOX, EmptyString );
        id = DIFF_TEXT_BOX;
        for ( i=0; i<3; i++ )
            {
            set_text( w, id, EmptyString );
            id++;
            }
        }

    set_text( w, CHAN_4_ZOOM_TITLE_TBOX, EmptyString );

    id = CHAN_4_TEXT_BOX;
    for ( i=0; i<3; i++ )
        {
        set_text( w, id, EmptyString );
        id++;
        }
    }
else
    {
    zi = 0;
    for ( c=0; c<NofPlotCurves; c++ )
        {
        if ( zi >= NofZoomAnalogChannels )
            break;
        i = PlotCurve[c].curve_index;
        channel = Ph.curves[i].ft_channel_number();
        if ( channel != NO_FT_CHANNEL )
            {
            channel--;
            s = ChannelAxisLabel[channel];
            if ( s.isempty() )
                s = PlotCurve[c].label;
            s.set_text( w, CHAN_1_ZOOM_TITLE_TBOX+zi );
            ZoomAnalogType[zi] = Ph.curves[i].ft_channel_type();
            zi++;
            }
        }

    while ( zi < NofZoomAnalogChannels )
        {
        ZoomAnalogType[zi] = 0;
        set_text( w, CHAN_1_ZOOM_TITLE_TBOX+zi, EmptyString );
        zi++;
        }
    }
}

/***********************************************************************
*                        MY_DESKTOP_CLIENT_RECT                        *
***********************************************************************/
RECT & my_desktop_client_rect()
{
static RECT r;
HWND w;

r.top = r.bottom = r.right = r.left = 0;

if ( IsFullScreen )
    {
    w = GetDesktopWindow();
    if ( w )
        {
        GetClientRect( w, &r );
        }
    }
else
    {
    r = desktop_client_rect();
    }

return r;
}

/***********************************************************************
*                       SHOW_FIXED_AXIS_MESSAGE                        *
*                                                                      *
*    This is called by the paint_me routine so I can use MyDc.         *
*                                                                      *
***********************************************************************/
void show_fixed_axis_message()
{
RECT  r;
HFONT old_font;
STRING_CLASS s;
SIZE         sz;

old_font = (HFONT) SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) );

s = resource_string( FIXED_AXIS_STRING );

GetTextExtentPoint32( MyDc, s.text(), s.len(), &sz );

GetClientRect( MainPlotWindow, &r );

r.top+= 2;
r.bottom = r.top + (sz.cy );
r.left = r.right - (sz.cx);

SetTextAlign( MyDc, TA_TOP | TA_LEFT );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR] );

SetBkMode( MyDc, TRANSPARENT );

TextOut( MyDc, r.left, r.top+2, s.text(), s.len() );

SelectObject( MyDc, old_font );
}

/***********************************************************************
*                             GET_FT2_HELP                             *
***********************************************************************/
void get_ft2_help()
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
*                               GETHELP                                *
***********************************************************************/
void gethelp( TCHAR * fname, UINT helptype, DWORD context )
{
TCHAR path[MAX_PATH+1];

if ( get_exe_directory(path) )
    {
    lstrcat( path, fname );
    WinHelp( MainWindow, path, helptype, context );
    }

}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( UINT helptype, DWORD context )
{
gethelp( MainHelpFileName, helptype, context );
}

/***********************************************************************
*                   END_DOWNTIME_ENTRY_DIALOG_PROC                     *
***********************************************************************/
static void end_downtime_entry_dialog()
{
HWND w;
if ( DowntimeEntryDialog )
    {
    w = DowntimeEntryDialog;
    DowntimeEntryDialog = 0;
    DestroyWindow( w );
    }
}

/***********************************************************************
*                       SET_CAT_AND_SUBCAT_TABS                        *
***********************************************************************/
void set_cat_and_subcat_tabs()
{
int tabs[] = { 30, 100 };
const int n = sizeof(tabs)/sizeof(int);

SendDlgItemMessage( DowntimeEntryDialog, CAT_AND_SUBCAT_LB, LB_SETTABSTOPS, (WPARAM) n, (LPARAM) tabs );
}

/***********************************************************************
*                      FILL_CAT_AND_SUBCAT_LB                          *
***********************************************************************/
static void fill_cat_and_subcat_lb()
{
STRING_CLASS  s;
int i;

if ( DowntimeEntryIsChanging )
    return;

DowntimeEntryIsChanging = true;
for ( i=0; i<SubCats.count(); i++ )
    {
    s = SubCats[i].catnumber;
    s.cat_w_char( SubCats[i].subnumber, TabChar );
    s.cat_w_char( Cats.name(SubCats[i].catnumber), TabChar );
    s += SubCats[i].name;
    DowntimeEntryLb.add( s.text() );
    }
DowntimeEntryIsChanging = false;
}

/***********************************************************************
*                         SET_XBOX_FROM_LB                             *
***********************************************************************/
static void set_xbox_from_lb()
{
static STRING_CLASS s;

if ( DowntimeEntryIsChanging )
    return;

DowntimeEntryIsChanging = true;

s = DowntimeEntryLb.selected_text();
if ( s.len() > 4 )
    *(s.text()+4) = NullChar;
set_text( DowntimeEntryEbox, s.text() );
eb_select_all( DowntimeEntryEbox );
EnableWindow( DowntimeEntryOkButton, TRUE );

DowntimeEntryIsChanging = false;
}

/***********************************************************************
*                          SET_LB_FROM_XBOX                            *
***********************************************************************/
static void set_lb_from_xbox()
{
const int FULL_ENTRY_LENGTH = DOWNCAT_NUMBER_LEN * 2;
static STRING_CLASS s;
static int  i;
static BOOL is_ok;

if ( DowntimeEntryIsChanging )
    return;

DowntimeEntryIsChanging = true;
is_ok = FALSE;

if ( s.get_text(DowntimeEntryEbox) )
    {
    if ( DowntimeEntryLb.findprefix(s.text()) )
        {
        i = DowntimeEntryLb.current_index();
        DowntimeEntryLb.setcursel( i );
        if ( s.len() == FULL_ENTRY_LENGTH )
            is_ok = TRUE;
        }
    else
        {
        DowntimeEntryLb.select_all( FALSE );
        }
    }

EnableWindow( DowntimeEntryOkButton, is_ok );
DowntimeEntryIsChanging = false;
}

/***********************************************************************
*                            SAVE_CLASS_NAME                           *
***********************************************************************/
void save_class_name( HWND w )
{
TCHAR buf[256];
int   n;

n = GetClassName( w, buf, 256 );
if ( n > 0 )
    {
    buf[n] = NullChar;
    put_ini_string( VisiTrakIniFile, HmiSection, ClassKey, buf );
    }
}

/***********************************************************************
*                            EnumWindowsProc                           *
***********************************************************************/
BOOL CALLBACK my_enum_windows_proc( HWND w, LPARAM lParam )
{
STRING_CLASS s;

s.get_text( w );
if ( !s.isempty() )
    {
    //MessageBox( 0, s.text(), "Name", MB_YESNO );
    if ( s.contains( TEXT("Frech") ) )
        {
        save_class_name( w );
        SetForegroundWindow( w );
        return FALSE;
        }
    }
//if ( )
//    return TRUE;

return TRUE;
}

/***********************************************************************
*                          RESTORE_FOCUS_TO_HMI                        *
***********************************************************************/
void restore_focus_to_hmi()
{
static TCHAR defclass[] = TEXT( "VisiTrakEditor" );
TCHAR * cp;
HWND w;

cp = get_ini_string( VisiTrakIniFile, HmiSection, ClassKey );
if ( unknown(cp) )
    {
    cp = defclass;
    put_ini_string( VisiTrakIniFile, HmiSection, ClassKey, cp );
    }
w = FindWindow( cp, NULL );
if ( w != NULL )
    {
    SetForegroundWindow( w );
    }
else
    {
    EnumWindows( my_enum_windows_proc, 0 );
    }
}

/***********************************************************************
*                        SET_DOWNTIME_FROM_CHOICE                      *
***********************************************************************/
static bool set_downtime_from_choice()
{
const TCHAR NoPartString[] = NO_PART;
const int FULL_ENTRY_LENGTH = DOWNCAT_NUMBER_LEN * 2;

TCHAR           worker[OPERATOR_NUMBER_LEN+1];
STRING_CLASS    s;
NAME_CLASS      filename;
HCURSOR         old_cursor;
DB_TABLE        t;
TCHAR         * cp;
COMPUTER_CLASS  c;
DSTAT_CLASS     d;
PART_NAME_ENTRY pn;
int             last_record;
int             current_record;
TIME_CLASS      new_time;
TIME_CLASS      old_time;
BOOLEAN         is_new_record;

last_record    = -1;
current_record = -1;

pn.set( c.whoami(), DowntimeEntryMachine.text(), NoPartString );
*worker = NullChar;

if ( !s.get_text(DowntimeEntryEbox) )
    return false;

if ( s.len() != FULL_ENTRY_LENGTH )
    return false;

if ( !DowntimeEntryLb.findprefix(s.text()) )
    return false;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

if ( !waitfor_shotsave_semaphor(pn.computer, pn.machine)  )
    {
    resource_message_box( MainInstance, CANNOT_COMPLY_STRING, SEMAPHOR_FAIL_STRING );
    SetCursor( old_cursor );
    return false;
    }

is_new_record = TRUE;
new_time.set( DowntimeEntrySt );
filename = downtime_dbname( pn.computer, pn.machine );
if ( filename.file_exists() )
    {
    if ( t.open(filename.text(), DOWNTIME_RECLEN, FL) )
        {
        last_record = (int) t.nof_recs();
        last_record--;
        if ( last_record >= 0 )
            {
            t.goto_record_number( last_record );
            if ( t.get_rec() )
                {
                if ( get_time_class_from_db(old_time, t, DOWNTIME_DATE_OFFSET) )
                    {

                    if ( old_time >= new_time )
                        {
                        /*
                        -------------------------------------------------------------------------
                        Either the is the record I am modifying or a new downtime record has been
                        inserted since we started. In either case, replace it with the new one.
                        ------------------------------------------------------------------------- */
                        is_new_record = FALSE;
                        }
                    }
                }
            }

        t.put_date( DOWNTIME_DATE_OFFSET, DowntimeEntrySt );
        t.put_time( DOWNTIME_TIME_OFFSET, DowntimeEntrySt );
        if ( is_new_record )
            {
            copystring( pn.part, CurrentMachine.current_part );
            copystring( worker,  CurrentMachine.opnumber );
            }
        else
            {
            t.get_alpha( pn.part, DOWNTIME_PART_NAME_OFFSET, PART_NAME_LEN );
            t.get_alpha( worker,  DOWNTIME_OPERATOR_OFFSET,  OPERATOR_NUMBER_LEN );
            }
        cp = s.text();
        d.set_cat( cp );
        t.put_alpha( DOWNTIME_CAT_OFFSET, cp, DOWNCAT_NUMBER_LEN );
        cp += DOWNCAT_NUMBER_LEN;
        d.set_subcat( cp );
        t.put_alpha( DOWNTIME_SUBCAT_OFFSET, cp, DOWNCAT_NUMBER_LEN );

        if ( is_new_record )
            t.rec_append();
        else
            t.rec_update();
        current_record = (int) t.current_record_number();
        if ( is_new_record )
            last_record = current_record;
        t.close();
        }
    }


free_shotsave_semaphore();

/*
-------------------------------------------------------------------------------
Only update the dstat if the downtime has not changed since the dialog appeared
------------------------------------------------------------------------------- */
if ( current_record >= 0 && current_record == last_record )
    {
    if ( !strings_are_equal(pn.part, NoPartString) )
        {
        d.set_time( DowntimeEntrySt );
        d.put( pn.computer, pn.machine );
        cp = make_downtime_event_string( pn.machine, pn.part, DowntimeEntrySt, d.category(), d.subcategory(), worker );
        if ( cp )
            {
            poke_data( DDE_MONITOR_TOPIC, ItemList[DOWN_DATA_INDEX].name, cp );
            delete[] cp;
            }
        }
    }

SetCursor( old_cursor );
return true;
}

/***********************************************************************
*                     DOWNTIME_ENTRY_SUB_PROC                          *
* Subclass the edit control so if the operator presses an up           *
* or down arrow, the current selection in the listbox changes.         *
***********************************************************************/
LRESULT CALLBACK downtime_entry_sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  i;

if ( msg == WM_KEYDOWN )
    {
    if ( wParam == VK_UP || wParam == VK_DOWN )
        {
        i = DowntimeEntryLb.current_index();
        if ( wParam == VK_UP && i > 0 )
            i--;
        else if ( wParam == VK_DOWN && i < (DowntimeEntryLb.count() - 1) )
            i++;
        else
            return 0;

        DowntimeEntryLb.setcursel( i );
        set_xbox_from_lb();
        return 0;
        }

    }

return CallWindowProc( OldEditProc, w, msg, wParam, lParam );
}

/***********************************************************************
*                         SHOW_DOWNTIME_ENTRY_TIME                     *
***********************************************************************/
void show_downtime_entry_time()
{
COMPUTER_CLASS c;
DB_TABLE       t;
STRING_CLASS   s;
int32          n;
TCHAR          subcat[DOWNCAT_NUMBER_LEN+1];

set_text( DowntimeEntryDialog, EVENT_MACHINE_TBOX, DowntimeEntryMachine.text() );

init_systemtime_struct( DowntimeEntrySt );

s = downtime_dbname( c.whoami(), DowntimeEntryMachine.text() );

GetLocalTime( &DowntimeEntrySt );

if ( t.open(s.text(), DOWNTIME_RECLEN, FL) )
    {
    n = t.nof_recs();
    if ( n > 0 )
        {
        n--;
        t.goto_record_number(n);
        t.get_current_record( NO_LOCK );
        t.get_alpha( subcat, DOWNTIME_SUBCAT_OFFSET, DOWNCAT_NUMBER_LEN );
        if ( strings_are_equal( subcat, DOWNCAT_UNSP_DOWN_SUBCAT) )
            {
            t.get_date( DowntimeEntrySt, DOWNTIME_DATE_OFFSET );
            t.get_time( DowntimeEntrySt, DOWNTIME_TIME_OFFSET );
            }
        }
    t.close();
    }

s = EmptyString;
s.cat_w_char( alphadate(DowntimeEntrySt), SpaceChar );
s += alphatime( DowntimeEntrySt );

s.set_text( DowntimeEntryDialog, EVENT_TIME_TBOX );
}

/***********************************************************************
*                    POSITION_DOWNTIME_ENTRY_DIALOG                    *
***********************************************************************/
void position_downtime_entry_dialog()
{
WINDOW_CLASS w;
RECTANGLE_CLASS desktop;
RECTANGLE_CLASS r;
LONG x;
LONG y;

GetClientRect( GetDesktopWindow(), &desktop );

w = DowntimeEntryDialog;
w.getrect( r );

x = desktop.right - desktop.left;
x /= 2;
x -= r.width()/2;
if ( x < 0 )
    x = 0;

y = desktop.bottom - desktop.top;
y /= 2;
y -= r.height()/2;
if ( y < 0 )
    y = 0;

w.move( x, y );
}

/***********************************************************************
*                       DOWNTIME_ENTRY_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK downtime_entry_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static STRING_CLASS s;
int  id;
int  cmd;
id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        DowntimeEntryDialog = hWnd;
        position_downtime_entry_dialog();
        DowntimeEntryEbox     = GetDlgItem( hWnd, CAT_AND_SUBCAT_EBOX );
        DowntimeEntryOkButton = GetDlgItem( hWnd, IDOK );
        EnableWindow( DowntimeEntryOkButton, FALSE );
        DowntimeEntryLb.init( GetDlgItem(hWnd, CAT_AND_SUBCAT_LB) );
        show_downtime_entry_time();
        OldEditProc = (WNDPROC) SetWindowLong( DowntimeEntryEbox, GWL_WNDPROC, (LONG) downtime_entry_sub_proc );
        set_cat_and_subcat_tabs();
        fill_cat_and_subcat_lb();
        SetFocus( DowntimeEntryEbox );
        return TRUE;

    case WM_CLOSE:
        end_downtime_entry_dialog();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, DOWNTIME_ENTRY_SCREEN_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CAT_AND_SUBCAT_LB:
                if ( cmd == LBN_SELCHANGE )
                    {
                    set_xbox_from_lb();
                    }
                return TRUE;

            case CAT_AND_SUBCAT_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    set_lb_from_xbox();
                    }
                return TRUE;

            case IDOK:
                if ( !set_downtime_from_choice() )
                    {
                    resource_message_box( MainInstance, NO_DOWNTIME_SELECTED_STRING, CANNOT_COMPLY_STRING );
                    return TRUE;
                    }

            case IDCANCEL:
                end_downtime_entry_dialog();
                return TRUE;

            }
        break;
    }
return FALSE;
}

/***********************************************************************
*                       LOAD_REALTIME_MACHINE_LIST                     *
***********************************************************************/
void load_realtime_machine_list()
{
COMPUTER_CLASS c;
NAME_CLASS  s;
DB_TABLE    t;
BITSETYPE   monitor_flags;
TCHAR       machine_name[MACHINE_NAME_LEN+1];

if ( c.count() > 1 )
    RealtimeMachineList.load();
else
    {
    RealtimeMachineList.empty();
    s = machset_dbname( c.whoami() );
    if ( s.file_exists() )
        {
        if ( t.open(s.text(), MACHSET_RECLEN, PFL) )
            {
            while ( t.get_next_record(NO_LOCK) )
                {
                monitor_flags = (BITSETYPE) t.get_long( MACHSET_MONITOR_FLAGS_OFFSET );
                if ( monitor_flags & MA_MONITORING_ON )
                    {
                    t.get_alpha( machine_name, MACHSET_MACHINE_NAME_OFFSET,  MACHINE_NAME_LEN );
                    RealtimeMachineList.append(machine_name);
                    }
                }
            t.close();
            }
        }
    }
}

/***********************************************************************
*                            LOAD_MACHLIST                             *
***********************************************************************/
static void load_machlist()
{
COMPUTER_CLASS c;
STRING_CLASS   lastco;
FILE_CLASS     f;
NAME_CLASS     s;
bool           is_remote;
TEXT_LIST_CLASS t;

MachList.empty();

c.rewind();
while ( c.next() )
    {
    if ( LoadFromNetworkDialogWindow )
        set_text( LoadFromNetworkDialogWindow, NETWORK_COMPUTER_TBOX, c.name() );

    if ( c.is_present() )
        MachList.add_computer( c.name() );
    else
        t.append( c.name() );
    }

/*
-------------------------------------------------------
Save the list of machines on each computer that I found
------------------------------------------------------- */
if ( MachList.count() > 0 )
    {
    is_remote = false;
    MachList.rewind();
    while ( MachList.next() )
        {
        if ( lastco != MachList.computer_name() )
            {
            if ( is_remote )
                f.close();
            lastco = MachList.computer_name();
            is_remote = false;
            if ( lastco != c.whoami() )
                {
                is_remote = true;
                if ( s.get_machine_list_file_name(lastco.text()) )
                    {
                    if ( !f.open_for_write(s) )
                        is_remote = false;
                    }
                }
            }
        if ( is_remote )
            f.writeline( MachList.name() );
        }

    if ( is_remote )
        f.close();
    }

/*
-----------------------------------------------------------------------------------------------------------
If any computers are off line, see if I can load from the machine_list_file that was saved the last time it
was on line.
----------------------------------------------------------------------------------------------------------- */
if ( t.count() > 0 )
    {
    t.rewind();
    while ( t.next() )
        {
        lastco = t.text();
        if ( s.get_machine_list_file_name(lastco.text()) )
            {
            if ( f.open_for_read(s.text()) )
                {
                while ( true )
                    {
                    s = f.readline();
                    if ( s.isempty() )
                        break;
                    MachList.add( lastco.text(), s.text(), NO_SORT );
                    }
                f.close();
                }
            }
        }
    }
}

/***********************************************************************
*                            LOAD_PART_INFO                            *
***********************************************************************/
static BOOLEAN load_part_info()
{
COMPUTER_CLASS c;
INI_CLASS  ini;
NAME_CLASS s;

if ( !c.find(ComputerName) )
    return FALSE;

if ( !c.is_present() )
    return FALSE;

if ( !strings_are_equal(MachineName, CurrentMachine.name) )
    {
    CurrentMachine.find( ComputerName, MachineName );
    init_global_curve_types();
    }

if ( CurrentPart.find(ComputerName, MachineName, PartName) )
    {
    CurrentSort.load( ComputerName, MachineName, PartName );
    CurrentParam.find( ComputerName, MachineName, PartName );
    CurrentMarks.load( ComputerName, MachineName, PartName );
    CurrentFtAnalog.load( ComputerName, MachineName, PartName );

    s.get_part_display_ini_name( ComputerName, MachineName, PartName );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );

    PostImpactDisplayTime = 0.0;
    if ( ini.find(DisplaySecondsKey) )
        {
        s = ini.get_string();
        PostImpactDisplayTime = s.float_value();
        }

    if ( PartBasedDisplay )
        read_corner_parameters( CornerParameters, NofCornerParameters );

    ChannelAxisLabel.get( ComputerName, MachineName, PartName );

    return TRUE;
    }
else
    {
    PartName[0] = NullChar;
    }
return FALSE;
}

/***********************************************************************
*                            LOAD_PART_INFO                            *
***********************************************************************/
static BOOLEAN load_part_info( PROFILE_NAME_ENTRY * p )
{
lstrcpy( ComputerName, p->computer );
lstrcpy( MachineName,  p->machine );
lstrcpy( PartName,     p->part );
lstrcpy( ShotName,     p->shot );

return load_part_info();
}

/***********************************************************************
*                        SHOW_LAST_ERROR                               *
***********************************************************************/
void show_last_error()
{
DWORD status;
TCHAR s[34];

status = GetLastError();
s[0] = TEXT('$');
int32toasc( s+1, status, HEX_RADIX );
MessageBox( MainPlotWindow, s, TEXT("Last Error"), MB_OK );
}

/***********************************************************************
*                       SELECT_PRINTER_FONT                            *
***********************************************************************/
static void select_printer_font()
{
TEXTMETRIC tm;

if ( !MyDc )
    return;

if ( !PrinterFont )
    return;

SelectObject( MyDc, PrinterFont ) ;

GetTextMetrics( MyDc, &tm );
MyCharHeight = tm.tmHeight - tm.tmInternalLeading;
MyCharWidth  = tm.tmAveCharWidth;
}

/***********************************************************************
*                         DELETE_PRINTER_FONT                          *
***********************************************************************/
static void delete_printer_font()
{
if ( PrinterFont )
    {
    DeleteObject( PrinterFont );
    PrinterFont = 0;
    }
}

/***********************************************************************
*                           CREATE_PRINTER_FONT                        *
***********************************************************************/
static void create_printer_font( RECT & r )
{
long       divisor;
long       height;
long       width;
TEXTMETRIC tm;
HFONT      f;
TCHAR    * name;

if ( !MyDc )
    return;

if ( MyScreenType == TREND_SCREEN_TYPE && GridIsVisible )
    {
    width  = r.right - r.left;
    height = r.bottom - r.top;

    if ( width > height )
        {
        height /= CurrentParam.count() + 9;
        width  = 0;
        }
    else
        {
        height = 0;
        width /= 130;
        }

    name = TimesRomanFontName.text();
    }
else
    {
    width   = 0;
    divisor = 100;
    if ( is_windows_nt() )
        divisor = 130;

    /*
    -----------------------------------------
    Get the aspect ratio of the original font
    ----------------------------------------- */
    GetTextMetrics( MyDc, &tm );
    height = tm.tmHeight - tm.tmInternalLeading;

    /*
    ---------------------------------------------------
    Adjust by the ratio of the new width to the old one
    --------------------------------------------------- */
    height *= ( r.right - r.left );
    height /= divisor;
    height /= tm.tmAveCharWidth;

    name = ArialFontName.text();
    }

f = create_font( MyDc, width, height, name );
if ( f )
    {
    /*
    ---------------------------------------------------------------
    The following gives me a default and assures that MyFont is not
    currently selected (it is bad form to delete the current font).
    --------------------------------------------------------------- */
    if ( PrinterFont )
        {
        SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) ) ;
        DeleteObject( PrinterFont );
        }

    PrinterFont = f;
    }
}

/***********************************************************************
*                          GET_DEFAULT_PRINTER_DC                      *
***********************************************************************/
HDC get_default_printer_dc()
{
PRINTER_INFO_2 * p;
DWORD     n;
DWORD     bytes_needed;
DWORD     slen;
HDC       mydc;


DEVMODE * dm;
TCHAR   * cp;
TCHAR     s[MAX_PATH+1];
HANDLE    ph;

mydc = 0;

GetProfileString( TEXT("windows"), TEXT("device"), TEXT("..."), s, MAX_PATH );
cp = findchar( CommaChar, s );
if ( cp )
    *cp = NullChar;

if ( OpenPrinter(s, &ph, 0) )
    {
    bytes_needed = 0;

    if ( !GetPrinter(ph, 2, 0, 0, &bytes_needed) )
        {
        if ( bytes_needed > 0 )
            {
            slen = sizeof( PRINTER_INFO_2 );
            n = bytes_needed / slen + 1;
            slen *= n;

            p = new PRINTER_INFO_2[n];
            if ( p )
                {
                if ( GetPrinter(ph, 2, (BYTE *) p, slen, &bytes_needed) )
                    {
                    dm = p->pDevMode;
                    if ( dm )
                        {
                        if ( dm->dmOrientation == DMORIENT_PORTRAIT )
                            {
                            PrinterMargin.right  =   0;    /* mils */
                            PrinterMargin.bottom = 500;
                            }
                        else
                            {
                            PrinterMargin.right  = 500;  /* mils */
                            PrinterMargin.bottom =   0;
                            }
                        mydc = CreateDC( 0, s, 0, 0 );
                        }

                    }
                delete[] p;
                }
            else
                {
                resource_message_box( MainInstance, PRINTER_LIST_STRING, NO_MEMORY_STRING );
                }
            }
        }

    ClosePrinter( ph );
    }

return mydc;
}

/***********************************************************************
*                            GET_PRINTER_DC                            *
***********************************************************************/
HDC get_printer_dc()
{
HDC        mydc;
static PRINTDLG   pd;
DEVMODE  * dm;

pd.lStructSize = sizeof(PRINTDLG);
pd.hwndOwner   = MainPlotWindow;
pd.hDevMode    = 0;
pd.hDevNames   = 0;
pd.hDC         = 0;
pd.Flags       = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_HIDEPRINTTOFILE | PD_NOSELECTION | PD_NOPAGENUMS;
pd.nFromPage   = 1;
pd.nToPage     = 1;
pd.nMinPage    = 1;
pd.nMaxPage    = 1;
pd.nCopies     = 1;
pd.hInstance   = MainInstance;
pd.lCustData            = 0;
pd.lpfnPrintHook        = 0;
pd.lpfnSetupHook        = 0;
pd.lpPrintTemplateName  = 0;
pd.lpSetupTemplateName  = 0;
pd.hPrintTemplate       = 0;
pd.hSetupTemplate       = 0;

mydc = 0;
if ( PrintDlg(&pd) )
    {
    mydc = pd.hDC;
    if ( pd.hDevMode )
        {
        dm = (DEVMODE *) GlobalLock( pd.hDevMode );
        if ( dm->dmOrientation == DMORIENT_PORTRAIT )
            {
            PrinterMargin.right  =   0;    /* mils */
            PrinterMargin.bottom = 500;
            }
        else
            {
            PrinterMargin.right  = 500;  /* mils */
            PrinterMargin.bottom =   0;
            }

        GlobalUnlock( pd.hDevMode );
        GlobalFree( pd.hDevMode );
        }

    if ( pd.hDevNames )
        GlobalFree( pd.hDevNames );
    }

return mydc;
}

/***********************************************************************
*                           CLEAR_ONE_AXIS                             *
***********************************************************************/
static void clear_one_axis( short i )
{
if ( !Axis[i].is_fixed )
    {
    Axis[i].max = 0.0;
    Axis[i].min = 0.0;
    Axis[i].is_on_hold = FALSE;
    Axis[i].is_clear   = TRUE;
    }
}

/***********************************************************************
*                             CLEAR_Y_AXES                             *
***********************************************************************/
static void clear_y_axes()
{
short i;

for ( i=0; i<NOF_AXES; i++ )
    {
    if ( i != TIME_AXIS && i != X_POS_AXIS )
        clear_one_axis( i );
    }

}

/***********************************************************************
*                              CLEAR_AXES                              *
***********************************************************************/
static void clear_axes()
{
short i;

for ( i=0; i<NOF_AXES; i++ )
    clear_one_axis( i );
}

/***********************************************************************
*                         PLOT_CURVE_INDEX                             *
***********************************************************************/
static int plot_curve_index( BITSETYPE curve_type )
{
int i;

for ( i=0; i<MAX_CURVES; i++ )
    {
    if ( (Ph.curves[i].type & curve_type) == curve_type )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                          RESET_PLOT_CURVES                           *
***********************************************************************/
static void reset_plot_curves()
{
int i;

NofPlotCurves = 0;
for ( i=0; i<MAX_CONTINUOUS_CHANNELS; i++ )
    {
    PlotCurve[i].curve_index = NO_INDEX;
    *PlotCurve[i].label      = NullChar;
    }
}

/***********************************************************************
*                           SET_PLOT_CURVES                            *
***********************************************************************/
void set_plot_curves()
{
int  channel;
int  i;
int  nof_channels;
BITSETYPE my_curve_types;
BITSETYPE curve_type;

if ( !HaveHeader )
    return;

reset_plot_curves();

my_curve_types = PreImpactPlotCurveTypes | PostImpactPlotCurveTypes;

if ( my_curve_types & PRESSURE_DISPLAY_MASK )
    {
    if ( my_curve_types & HEAD_PRESSURE_CURVE )
        {
        i  = plot_curve_index( HEAD_PRESSURE_CURVE );
        if ( i != NO_INDEX )
            {
            PlotCurve[NofPlotCurves].curve_index = i;
            lstrcpy( PlotCurve[NofPlotCurves].label, resource_string(HEAD_PRESSURE_LABEL_STRING) );
            NofPlotCurves++;
            }
        }

    if ( my_curve_types & ROD_PRESSURE_CURVE )
        {
        i  = plot_curve_index( ROD_PRESSURE_CURVE );
        if ( i != NO_INDEX )
            {
            PlotCurve[NofPlotCurves].curve_index = i;
            lstrcpy( PlotCurve[NofPlotCurves].label, resource_string(ROD_PRESSURE_LABEL_STRING) );
            NofPlotCurves++;
            }
        }

    if ( my_curve_types & DIFF_PRESSURE_CURVE )
        {
        i  = plot_curve_index( DIFF_PRESSURE_CURVE );
        if ( i != NO_INDEX )
            {
            PlotCurve[NofPlotCurves].curve_index = i;
            lstrcpy( PlotCurve[NofPlotCurves].label, resource_string(DIFF_PRESSURE_LABEL_STRING) );
            NofPlotCurves++;
            }
        }
    }
else
    {
    curve_type = CHAN_1_ANALOG_CURVE;
    nof_channels = MAX_CONTINUOUS_CHANNELS;
    if ( CurrentMachine.is_ftii )
        nof_channels = NOF_FTII_ANALOGS;
    for ( channel=0; channel<nof_channels; channel++ )
        {
        if ( NofPlotCurves >= MAX_CONTINUOUS_CHANNELS )
            break;
        if ( my_curve_types & curve_type )
            {
            i  = plot_curve_index( curve_type );
            if ( i != NO_INDEX )
                {
                PlotCurve[NofPlotCurves].curve_index = i;
                i = channel;
                if ( i < 4 )
                    i += CHANNEL_1_LABEL_STRING;
                else
                    i += CHANNEL_5_LABEL_STRING - 4;
                lstrcpy( PlotCurve[NofPlotCurves].label, resource_string(i) );
                NofPlotCurves++;
                }
            }
        /*
        ---------------------------------------------------------
        Each consecutive curve type is the next bit
        except the DIFF_PRESSURE_CURVE which is inbetween 4 and 5
        --------------------------------------------------------- */
        curve_type <<= 1;
        if ( curve_type == DIFF_PRESSURE_CURVE )
            curve_type <<= 1;
        }
    }
}

/***********************************************************************
*                                ZOOMED                                *
***********************************************************************/
static BOOLEAN zoomed()
{

if ( LeftZoomIndex != 0 || ZoomIndex != 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           HIDE_ALL_WINDOWS                           *
***********************************************************************/
void hide_all_windows()
{
ShowWindow( MainPlotWindow,     SW_HIDE );
ShowWindow( SmallZoomBoxWindow, SW_HIDE );

ShowWindow( ParameterWindow,     SW_HIDE );
ShowWindow( ParamHistoryWindow, SW_HIDE );
ShowWindow( ZoomBoxWindow,      SW_HIDE );
XdcrWindow.hide();
}

/***********************************************************************
*                           SHOW_ALL_WINDOWS                           *
***********************************************************************/
void show_all_windows()
{
int state;

if ( IsIconic(MainWindow) )
    ShowWindow( MainWindow, SW_RESTORE );

if ( !IsWindowVisible(MainPlotWindow) )
    ShowWindow( MainPlotWindow, SW_SHOW );

if ( MyScreenType == ZABISH_SCREEN_TYPE )
    {
    ShowWindow( SmallZoomBoxWindow, SW_HIDE );
    if ( !zoomed() )
        ShowWindow( ZoomBoxWindow,  SW_HIDE );

    if ( InfoBoxType == XDCR_INFO_BOX )
        {
        XdcrWindow.show();
        state = SW_HIDE;
        }
    else
        {
        XdcrWindow.hide();
        state = SW_SHOW;
        }

    ShowWindow( ParameterWindow, state );
    ShowWindow( ParamHistoryWindow, state );
    if ( zoomed() )
        ShowWindow( ZoomBoxWindow,  SW_SHOW );
    }
else if ( MyScreenType == STANDARD_SCREEN_TYPE )
    {
    ShowWindow( ParamHistoryWindow, SW_HIDE );
    ShowWindow( ZoomBoxWindow,      SW_HIDE );
    if ( !zoomed() )
        ShowWindow( SmallZoomBoxWindow, SW_HIDE );

    if ( InfoBoxType == XDCR_INFO_BOX )
        {
        XdcrWindow.show();
        state = SW_HIDE;
        }
    else
        {
        XdcrWindow.hide();
        state = SW_SHOW;
        }

    ShowWindow( ParameterWindow,    SW_SHOW );
    if ( zoomed() )
        ShowWindow( SmallZoomBoxWindow, SW_SHOW );
    }
else
    {
    ShowWindow( MainPlotWindow,     SW_SHOW );
    ShowWindow( SmallZoomBoxWindow, SW_HIDE );

    ShowWindow( ParameterWindow,    SW_HIDE );
    ShowWindow( ParamHistoryWindow, SW_HIDE );
    ShowWindow( ZoomBoxWindow,      SW_HIDE );
    XdcrWindow.hide();
    }

}

/***********************************************************************
*                        TOGGLE_MENU_VIEW_STATE                        *
***********************************************************************/
void toggle_menu_view_state( BOOLEAN & state, UINT menu_choice_id )
{
toggle_menu_view_state( state, MainWindow, menu_choice_id );
}

/***********************************************************************
*                          SELECT_MYFONT                               *
***********************************************************************/
static void select_myfont()
{
TEXTMETRIC tm;

if ( !MyDc )
    return;

if ( MyFont )
    SelectObject( MyDc, MyFont );
else
    SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) ) ;

GetTextMetrics( MyDc, &tm );
MyCharHeight = tm.tmHeight - tm.tmInternalLeading;
MyCharWidth  = tm.tmAveCharWidth;
}

/***********************************************************************
*                               GET_MYDC                               *
***********************************************************************/
static void get_mydc()
{

if ( !MyDc )
    {
    MyDc = GetDC( MainPlotWindow );
    select_myfont();
    }

NofDcOwners++;
}

/***********************************************************************
*                             RELEASE_MYDC                             *
***********************************************************************/
static void release_mydc()
{

if ( !MyDc )
    return;

if ( NofDcOwners )
    NofDcOwners--;

if ( NofDcOwners == 0 )
    {
    ReleaseDC( MainPlotWindow, MyDc );
    MyDc = NULL;
    }

}

/***********************************************************************
*                          DRAW_VERTICAL_TICS                          *
***********************************************************************/
void draw_vertical_tics( RECT & r, int nof_tics, HPEN gridpen,  HPEN borderpen )
{

int i;
int x;
int range;
HPEN oldpen;

oldpen = (HPEN) SelectObject( MyDc, borderpen );

range = r.right - r.left;
for ( i=0; i<=nof_tics; i++ )
    {
    if ( i == 1 )
        SelectObject( MyDc, gridpen );
    else if ( i == nof_tics )
        SelectObject( MyDc, borderpen );

    x  = range * i;
    x /= nof_tics;
    x += r.left;

    MoveToEx( MyDc, x, r.top, 0 );
    LineTo( MyDc, x, r.bottom );
    }

SelectObject( MyDc, oldpen );
}

/***********************************************************************
*                         DRAW_HORIZONTAL_TICS                         *
***********************************************************************/
static void draw_horizontal_tics( RECT & r, int nof_tics, HPEN gridpen,  HPEN borderpen )
{

int i;
int y;
HPEN oldpen;

oldpen = (HPEN) SelectObject( MyDc, borderpen );

for ( i=0; i<=nof_tics; i++ )
    {
    if ( i == 1 )
        SelectObject( MyDc, gridpen );
    if ( i == nof_tics )
        SelectObject( MyDc, borderpen );
    y = r.top + (r.bottom - r.top - 1) * i / nof_tics;
    MoveToEx( MyDc, r.left, y, 0 );
    LineTo( MyDc, r.right, y );
    }

SelectObject( MyDc, oldpen );
}

/***********************************************************************
*                           DRAW_CURSOR_LINE                           *
***********************************************************************/
static void draw_cursor_line( int x )
{
int     orimode;

get_mydc();

if ( MyDc )
    {
    orimode = SetROP2( MyDc, R2_NOT );

    MoveToEx( MyDc, x, CursorStartY, 0 );
    LineTo(   MyDc, x, CursorEndY      );

    SetROP2( MyDc, orimode );
    }

release_mydc();
}

/***********************************************************************
*                          ERASE_CURSOR_LINE                           *
***********************************************************************/
static void erase_cursor_line()
{

if ( HaveCursorLine )
    {
    draw_cursor_line( CursorX );
    HaveCursorLine = FALSE;
    }
}

/***********************************************************************
*                       ERASE_LEFT_CURSOR_LINE                         *
***********************************************************************/
static void erase_left_cursor_line()
{

if ( HaveLeftCursorLine )
    {
    draw_cursor_line( LeftCursorX );
    HaveLeftCursorLine = FALSE;
    }
}

/***********************************************************************
*                     TOGGLE_BIG_PARAM_DISPLAY_TYPE                    *
***********************************************************************/
void toggle_big_param_display_type()
{
static BOOLEAN was_realtime       = TRUE;
static BOOLEAN was_single_machine = FALSE;
static short   old_screen_type    = PROFILE_SCREEN_TYPE;

if ( MyScreenType != BIG_PARAM_SCREEN_TYPE )
    {
    old_screen_type    = MyScreenType;
    was_realtime       = RealTime;
    was_single_machine = SingleMachineOnly;
    RealTime          = TRUE;
    SingleMachineOnly = TRUE;
    update_screen_type( BIG_PARAM_SCREEN_TYPE );
    }
else
    {
    RealTime          = was_realtime;
    SingleMachineOnly = was_single_machine;
    update_screen_type( old_screen_type );
    }

}

/***********************************************************************
*                      POSITION_XDCR_WINDOW                            *
***********************************************************************/
void position_xdcr_window()
{
RECT d;
RECT r;

d = my_desktop_client_rect();
GetWindowRect( MainPlotWindow,     &r );

d.top = r.bottom;
XdcrWindow.move( d );
}

/***********************************************************************
*                          SET_INFO_BOX_TYPE                           *
***********************************************************************/
void set_info_box_type( int new_type )
{

WINDOW_CLASS zoombox;
WINDOW_CLASS parambox;

if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == TREND_SCREEN_TYPE || MyScreenType == STATUS_SCREEN_TYPE )
    return;

parambox = ParameterWindow;
if ( MyScreenType == ZABISH_SCREEN_TYPE )
    zoombox  = ZoomBoxWindow;
else
    zoombox = SmallZoomBoxWindow;

if ( new_type != InfoBoxType )
    {
    switch ( InfoBoxType )
        {
        case ZOOM_INFO_BOX:
            zoombox.hide();
            break;

        case PARAMETER_INFO_BOX:
            if ( new_type == XDCR_INFO_BOX )
                ShowWindow( ParamHistoryWindow, SW_HIDE );
            break;

        case XDCR_INFO_BOX:
            XdcrWindow.hide();
            parambox.show();
            break;
        }

    switch ( new_type )
        {
        case ZOOM_INFO_BOX:
            zoombox.show();
            set_zoom_box_labels( zoombox.handle() );
            break;

        case PARAMETER_INFO_BOX:
            if ( MyScreenType == ZABISH_SCREEN_TYPE )
                ShowWindow( ParamHistoryWindow, SW_SHOW );
            break;

        case XDCR_INFO_BOX:
            parambox.hide();
            position_xdcr_window();
            XdcrWindow.show();
            break;
        }
    }

InfoBoxType = new_type;
}

/***********************************************************************
*                             DRAW_GRID                                *
***********************************************************************/
void draw_grid( int rindex )
{

int  nof_tics;
HPEN borderpen;
HPEN gridpen;

if ( !Region[rindex].is_visible )
    return;

gridpen   = CreatePen( PS_SOLID, 1, CurrentColor[GRID_COLOR]   );
borderpen = CreatePen( PS_SOLID, 1, CurrentColor[BORDER_COLOR] );

if ( gridpen && borderpen )
    {
    /*
    --------
    Vertical
    -------- */
    if ( CurrentDisplay != POS_TIME_DISPLAY )
        nof_tics = POS_BASED_X_TICS + TIME_BASED_X_TICS;

    else if ( rindex == POS_BASED_ANALOG || rindex == POS_BASED_COUNT )
        nof_tics = POS_BASED_X_TICS;

    else
        nof_tics = TIME_BASED_X_TICS;

    draw_vertical_tics( Region[rindex].rect, nof_tics, gridpen, borderpen );

    /*
    ----------
    Horizontal
    ---------- */
    if ( rindex == POS_BASED_ANALOG || rindex == TIME_BASED_ANALOG )
        nof_tics = ANALOG_Y_TICS;
    else
        nof_tics = COUNT_Y_TICS;

    draw_horizontal_tics( Region[rindex].rect, nof_tics, gridpen, borderpen );
    }

if ( gridpen )
    DeleteObject( gridpen );

if ( borderpen )
    DeleteObject( borderpen );
}

/***********************************************************************
*                             X_AXIS_LABEL                             *
***********************************************************************/
static void x_axis_label( RECT & r, TCHAR * sorc )
{

int  x;
int  y;

SetTextAlign( MyDc, TA_TOP | TA_CENTER );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );
SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );

x = (r.left + r.right)/2;
y = r.bottom + 2 * MyCharHeight;

TextOut( MyDc, x, y, sorc, lstrlen(sorc) );
}

/***********************************************************************
*                             ROD_HEAD_LABEL                           *
***********************************************************************/
static void rod_head_label()
{

int  c;
int  slen;
int  x;
int  y;

SetTextAlign( MyDc, TA_BOTTOM | TA_LEFT );
SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );

x = 1;

y = Region[POS_BASED_COUNT].rect.top;
y -= MyCharHeight/2;

for ( c=0; c<2; c++ )
    if ( NofPlotCurves > c )
        {
        slen = lstrlen( PlotCurve[c].label );
        if ( slen )
            {
            SetTextColor( MyDc, CurrentColor[FIRST_ANALOG_AXIS+c] );
            TextOut( MyDc, x, y, PlotCurve[c].label, slen );
            y -= MyCharHeight;
            }
        }

SetTextAlign( MyDc, TA_BOTTOM | TA_RIGHT );
y = Region[TIME_BASED_COUNT].rect.top;
y -= MyCharHeight/2;
x = Region[RIGHT_AXIS].rect.right - 1;

for ( c=2; c<4; c++ )
    if ( NofPlotCurves > c )
        {
        slen = lstrlen( PlotCurve[c].label );
        if ( slen )
            {
            SetTextColor( MyDc, CurrentColor[FIRST_ANALOG_AXIS+c] );
            TextOut( MyDc, x, y, PlotCurve[c].label, slen );
            y -= MyCharHeight;
            }
        }

SetTextAlign( MyDc, TA_BOTTOM | TA_LEFT );
}

/***********************************************************************
*                             Y_AXIS_LABEL                             *
***********************************************************************/
static void y_axis_label( RECT & r, TCHAR * sorc, TCHAR * sorc2, BITSETYPE axis_type, COLORREF color1, COLORREF color2 )
{

int      i;
int      delta_x;
int      delta_y;
int      slen;
int      x;
int      y;
int      height;
COLORREF oricolor;
SIZE     sorc_size;

if ( axis_type == LEFT_AXIS_TYPE )
    x = PlotWindowRect.left + 4 + MyCharWidth / 2;
else
    x = PlotWindowRect.right - 4 - MyCharWidth / 2;

slen   = lstrlen(sorc);

height = slen * MyCharHeight;
y = r.top;
delta_y = r.bottom - r.top;
if ( height < delta_y )
    y += (delta_y - height)/2;

oricolor = GetTextColor( MyDc );
SetTextColor( MyDc, color1 );
SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );
SetTextAlign( MyDc, TA_BOTTOM | TA_CENTER );

delta_x = 0;
for ( i=0; i<slen; i++ )
    {
    GetTextExtentPoint32( MyDc, sorc, 1, &sorc_size );
    if ( delta_x < sorc_size.cx )
        delta_x = sorc_size.cx;
    TextOut( MyDc, x, y, sorc, 1 );
    y += MyCharHeight;
    sorc++;
    }

SetTextColor( MyDc, color2 );
slen   = lstrlen(sorc2);
height = slen * MyCharHeight;
y = r.top;
delta_y = r.bottom - r.top;
if ( height < delta_y )
    y += (delta_y - height)/2;

if ( axis_type == LEFT_AXIS_TYPE )
    x += delta_x;
else
    x -= delta_x;

for ( i=0; i<slen; i++ )
    {
    TextOut( MyDc, x, y, sorc2, 1 );
    y += MyCharHeight;
    sorc2++;
    }

SetTextColor( MyDc, oricolor );
}

/***********************************************************************
*                             Y_AXIS_LABEL                             *
***********************************************************************/
static void y_axis_label( RECT & r, TCHAR * sorc, TCHAR * sorc2, BITSETYPE axis_type )
{
COLORREF c;
c = CurrentColor[TEXT_COLOR];

y_axis_label( r, sorc, sorc2, axis_type, c, c );
}

/***********************************************************************
*                             FIX_DECIMALS                             *
***********************************************************************/
void fix_decimals( TCHAR * buf, short decimal_places )
{

short slen;
TCHAR * cp;

cp = findchar( TEXT('.'), buf, 6 );
if ( cp )
    {
    slen = (short) lstrlen( cp );
    if ( decimal_places <= 0 )
        {
        *cp = NullChar;
        return;
        }
    slen--;
    cp++;
    if ( slen > decimal_places )
        {
        cp += decimal_places;
        *cp = NullChar;
        }
    }

}

/***********************************************************************
*                              DRAW_AXIS                               *
***********************************************************************/
void draw_axis( int axdex, short decimal_places, RECT & r,
                UINT alignment, BITSETYPE axis_type, short nof_tics,
                short labels_to_skip )
{

int      height;
int      i;
int      x;
int      y;
float    range;
float    value;
TCHAR    buf[10];
COLORREF oricolor;
UINT     oralign;
INT      orbkmode;

oralign  = GetTextAlign( MyDc );
oricolor = GetTextColor( MyDc );
orbkmode = GetBkMode( MyDc );

if ( axis_type == LEFT_AXIS_TYPE )
    {
    alignment |= TA_RIGHT;
    x = r.left - MyCharWidth;
    }
else
    {
    alignment |= TA_LEFT;
    x = r.right + MyCharWidth;
    }

SetTextAlign( MyDc, alignment );
SetTextColor( MyDc, CurrentColor[axdex] );
SetBkMode(    MyDc, TRANSPARENT );

if ( decimal_places < 0 )
    {
    decimal_places = 0;
    value = Axis[axdex].max;
    if ( value < 10000.0 )
        decimal_places = 1;
    if ( value < 1000.0 )
        decimal_places = 2;
    if ( value < 100.0 )
        decimal_places = 3;
    if ( value < 10.0 )
        decimal_places = 4;
    }

height = r.bottom - r.top;
range  = Axis[axdex].max - Axis[axdex].min;
for ( i=0; i<=nof_tics; i++ )
    {
    y = height * i;
    y /= nof_tics;
    y = r.bottom - y;
    value = Axis[axdex].min + range * float(i)/float(nof_tics);
    insalph( buf, value, 6, TEXT(' ') );
    *(buf+6) = NullChar;
    fix_decimals( buf, decimal_places );
    if ( labels_to_skip )
        labels_to_skip--;
    else
        TextOut( MyDc, x, y, buf, lstrlen(buf) );
    }

SetTextAlign( MyDc, oralign  );
SetTextColor( MyDc, oricolor );
SetBkMode(    MyDc, orbkmode );
}

/***********************************************************************
*                             DRAW_X_AXIS                              *
***********************************************************************/
static void draw_x_axis( MINMAX &limits, int decimal_places,
RECT & r, short nof_tics, short labels_to_skip, COLORREF textcolor )
{

int  i;
int  x;
int  y;
int  width;
int  dy;
double value;
double range;
TCHAR buf[MAX_DOUBLE_LEN+1];
UINT oralign;
UINT alignment;
UINT alignmask;

oralign = GetTextAlign( MyDc );

SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );
SetTextColor( MyDc, textcolor                      );

alignment = TA_CENTER | TA_TOP;
alignmask = TA_BASELINE | TA_TOP;

dy =  (short) MyCharHeight;

y = Region[POS_BASED_COUNT].rect.bottom;

width  = r.right - r.left;
range  = double(limits.max) - double(limits.min);
for ( i=0; i<=nof_tics; i++ )
    {
    if ( labels_to_skip )
        labels_to_skip--;
    else
        {
        SetTextAlign( MyDc, alignment );
        x = width * i;
        x /= nof_tics;
        x += r.left;
        value = double(limits.min) + range * double(i)/double(nof_tics);
        if ( double_to_tchar( buf, value, decimal_places ) )
            TextOut( MyDc, x, y+dy, buf, lstrlen(buf) );
        else
            lstrcpy( buf, TEXT("***") );
        alignment ^= alignmask;
        }
    }

SetTextAlign( MyDc, oralign );
}

/***********************************************************************
*                              SHOW_CURVE                              *
***********************************************************************/
void show_curve( short rindex, short yaxis, short xaxis, short ycurve, short xcurve, COLORREF color )
{
int32   i;
int     last_point;
int     pen_width;
int     n;

float * xp;
float * yp;

float   dif;
float * myx;
float * dest;

HPEN     oldpen;
HPEN     mypen;
RECT     r;

if ( !HaveHeader )
    return;

if ( !Region[rindex].is_visible )
    return;

r = Region[rindex].rect;

/*
-----------------------------------------------------------
Initialize the variables used to adjust the time based data
----------------------------------------------------------- */
dif = 0.0;

/*
-----------------------------------------------------------------------
If this is the time axis and I am overlaying and display is pos/time
then I need to adjust the data to begin at the p3 point of the original
plot. I am going to use the diff variable for this.
----------------------------------------------------------------------- */
if ( (overlaying() || viewing_reference()) && xaxis == TIME_AXIS && CurrentDisplay == POS_TIME_DISPLAY )
    {
    i = plot_curve_index( TIME_CURVE );
    dif = Axis[xaxis].min - Points[i][Ph.last_pos_based_point];
    }

pen_width = 1;
if ( Printing )
    {
    pen_width = 2;
    }
else
    {
    if ( CurvePenWidth > 0 )
        pen_width = CurvePenWidth;
    }
mypen = CreatePen( PS_SOLID, pen_width, color );
if ( mypen )
    {
    oldpen = (HPEN) SelectObject( MyDc, mypen );
    }

xp = Points[xcurve];
yp = Points[ycurve];

/*
-------------------------------------
Default to plotting all of the points
------------------------------------- */
i = 0;
last_point = Ph.n - 1;

/*
--------------------------------------------------------------------
If this is an analog curve, check to see if I need to plot all of it
-------------------------------------------------------------------- */
if ( !(Ph.curves[ycurve].type & (TIME_CURVE | POSITION_CURVE | VELOCITY_CURVE)) )
    {
    /*
    ------------------------------------------
    See if I should skip the pre impact points
    ------------------------------------------ */
    if ( !(Ph.curves[ycurve].type & PreImpactPlotCurveTypes) )
        {
        i = Ph.last_pos_based_point + 1;
        xp += i;
        yp += i;
        }
    /*
    -------------------------------------------
    See if I should skip the post impact points
    ------------------------------------------- */
    if ( !(Ph.curves[ycurve].type & PostImpactPlotCurveTypes) )
        last_point = Ph.last_pos_based_point;
    }


if ( rindex == POS_BASED_ANALOG || rindex == POS_BASED_COUNT )
    {
    if ( last_point > Ph.last_pos_based_point )
        last_point = Ph.last_pos_based_point;
    }

if ( last_point <= i )
    return;

n  = 1 + last_point - i;

/*
---------------------------------------------
I'm done using i so I can use it as a counter
---------------------------------------------  */
myx = 0;
if ( not_float_zero(dif) )
    {
    myx = new float[n];
    if ( myx )
        {
        dest = myx;
        i = n;
        while ( i )
            {
            *dest = *xp + dif;
            dest++;
            xp++;
            i--;
            }
        xp = myx;
        }
    }

plot_data( MyDc, r, Axis[xaxis].min, Axis[xaxis].max, Axis[yaxis].min, Axis[yaxis].max, xp, yp, n );

if ( myx )
    delete [] myx;

if ( mypen )
    {
    SelectObject( MyDc, oldpen );
    DeleteObject( mypen );
    }

}

/***********************************************************************
*                      CALCULATE_YAXIS_LIMITS                          *
*   This can now be called more than once for a given curve.           *
*   If the limits have been set before, they will be adjusted only.    *
***********************************************************************/
static void calculate_yaxis_limits( int curv, int yaxis )
{
int     n          = 0;
float * fp         = 0;
float * xp         = 0;
float   xmax       = 0.0;
float   xmin       = 0.0;
bool    need_check = false;

if ( Axis[yaxis].is_fixed )
    return;

n  = Ph.n;
fp = Points[curv];

if ( CurrentDisplay == TIME_ONLY_DISPLAY && Axis[TIME_AXIS].is_on_hold )
    {
    xp   = Points[TimeCurve];
    xmin = Axis[TIME_AXIS].min;
    xmax = Axis[TIME_AXIS].max;
    }
else if ( CurrentDisplay == POS_ONLY_DISPLAY && Axis[X_POS_AXIS].is_on_hold )
    {
    n    = Ph.last_pos_based_point+1;
    xp   = Points[PosCurve];
    xmin = Axis[X_POS_AXIS].min;
    xmax = Axis[X_POS_AXIS].max;
    }
else if ( yaxis == VELOCITY_AXIS && CurrentDisplay != TIME_ONLY_DISPLAY )
    {
    n    = Ph.last_pos_based_point+1;
    if ( Axis[X_POS_AXIS].is_on_hold )
        {
        xp   = Points[PosCurve];
        xmin = Axis[X_POS_AXIS].min;
        xmax = Axis[X_POS_AXIS].max;
        }
    else
        {
        xp = 0;
        }
    }
else if ( yaxis == Y_POS_AXIS )
    {
    xp   = Points[TimeCurve];
    xmin = Axis[TIME_AXIS].min;
    xmax = Axis[TIME_AXIS].max;
    }
else
    {
    n = 0;
    }

if ( n )
    {
    if ( Axis[yaxis].is_clear )
        {
        Axis[yaxis].min = Ph.curves[curv].max;
        Axis[yaxis].max = Ph.curves[curv].min;
        }

    while( n )
        {
        need_check = true;
        if ( xp )
            {
            need_check = xmin <= *xp && *xp <= xmax;
            xp++;
            }

        if ( need_check )
            {
            if ( *fp > Axis[yaxis].max )
               Axis[yaxis].max = *fp;

            if ( *fp < Axis[yaxis].min )
               Axis[yaxis].min = *fp;
            }
        fp++;
        n--;
        }
    }
else
    {
    if ( Ph.curves[curv].max > Axis[yaxis].max || Axis[yaxis].is_clear )
        Axis[yaxis].max = Ph.curves[curv].max;

    if ( Ph.curves[curv].min < Axis[yaxis].min || Axis[yaxis].is_clear )
        Axis[yaxis].min = Ph.curves[curv].min;
    }

/*
-----------------------------------------------
Make all unzoomed analog channels start at zero
if ( IsPressureDisplay && !zoomed() && FIRST_ANALOG_AXIS <= yaxis && yaxis <= LAST_ANALOG_AXIS )
----------------------------------------------- */
if ( !zoomed() && FIRST_ANALOG_AXIS <= yaxis && yaxis <= LAST_ANALOG_AXIS )
    {
    if ( Axis[yaxis].min > 0.0 )
        Axis[yaxis].min = 0.0;
    }

Axis[yaxis].is_clear = FALSE;
}

/***********************************************************************
*                        ADJUST_ALL_Y_AXIS_LIMITS                      *
***********************************************************************/
void adjust_all_y_axis_limits()
{
int  channel;
int  i;
int  x;

if ( !HaveHeader )
    return;

for ( channel=0; channel<NofPlotCurves; channel++ )
    {
    i = PlotCurve[channel].curve_index;
    if ( i == NO_INDEX )
        continue;

    x = FIRST_ANALOG_AXIS + channel;
    if ( !Axis[x].is_on_hold )
        calculate_yaxis_limits( i, x );
    }

i = plot_curve_index( VELOCITY_CURVE );
if ( i != NO_INDEX )
    {
    x = VELOCITY_AXIS;
    if ( !Axis[x].is_on_hold )
        calculate_yaxis_limits( i, x );
    }

i = plot_curve_index( POSITION_CURVE );
if ( i != NO_INDEX )
    {
    x = Y_POS_AXIS;
    if ( !Axis[x].is_on_hold )
        if ( CurrentDisplay == TIME_ONLY_DISPLAY || CurrentDisplay == POS_TIME_DISPLAY || Axis[TIME_AXIS].is_on_hold )
            calculate_yaxis_limits( i, x );
    }

}

/***********************************************************************
*                            SHOW_LEFT_AXES                            *
***********************************************************************/
void show_left_axes()
{

int  channel;
int  next_channel;
int  true_channel;
int  i;
int  x;
RECT r;
BOOLEAN firstime;
STRING_CLASS s1;
STRING_CLASS s2;
COLORREF     c1;
COLORREF     c2;

if ( !HaveHeader )
    return;

firstime = TRUE;

/*
-------------------------------------
Show the axes for the analog channels
channel is the index of PlotCurve.
i is the index of Ph.curves.
x is the index of the screen axis
------------------------------------- */
for ( channel=0; channel<2; channel++ )
    {
    if ( channel >= NofPlotCurves )
        break;

    i = PlotCurve[channel].curve_index;
    if ( i == NO_INDEX )
        continue;

    x = FIRST_ANALOG_AXIS + channel;
    r = Region[POS_BASED_ANALOG].rect;
    if ( firstime )
        {
        draw_axis( x,-1, r, TA_TOP, LEFT_AXIS_TYPE, 4, 1 );

        if ( IsPressureDisplay )
            {
            s1 = resource_string( PRESSURE_TYPE_STRING );
            s2 = units_name( Ph.curves[i].units );
            c1 = CurrentColor[TEXT_COLOR];
            c2 = c1;
            }
        else
            {
            true_channel = Ph.curves[i].ft_channel_number();
            if ( true_channel != NO_FT_CHANNEL )
                true_channel--;
            if ( i >= CHAN_1_ANALOG_CURVE_INDEX )
                s1 = ChannelAxisLabel[true_channel];
            if ( s1.isempty() )
                s1 = PlotCurve[channel].label;
            s1 += SpaceString;
            s1 += units_name( Ph.curves[i].units );

            c1 = CurrentColor[x];
            c2 = c1;
            next_channel = channel + 1;
            if ( next_channel < NofPlotCurves )
                {
                i = PlotCurve[next_channel].curve_index;
                true_channel = Ph.curves[i].ft_channel_number();
                if ( true_channel != NO_FT_CHANNEL )
                    true_channel--;
                if ( i >= CHAN_1_ANALOG_CURVE_INDEX )
                    s2 = ChannelAxisLabel[true_channel];
                if ( s2.isempty() )
                    s2 = PlotCurve[next_channel].label;
                if ( i != NO_INDEX )
                    {
                    s2 += SpaceString;
                    s2 += units_name( Ph.curves[i].units );
                    }
                c2 = CurrentColor[x+1];
                }
            }
        y_axis_label( r, s1.text(), s2.text(), LEFT_AXIS_TYPE, c1, c2 );
        firstime = FALSE;
        }
    else
        {
        draw_axis( x,-1, r, TA_BASELINE, LEFT_AXIS_TYPE, 4, 1 );
        }
    }

/*
------------------------------------
Show the axis for the velocity curve
------------------------------------ */
i = plot_curve_index( VELOCITY_CURVE );
if ( i != NO_INDEX )
    {
    x = VELOCITY_AXIS;
    r = Region[POS_BASED_COUNT].rect;
    draw_axis( x,  2, r, TA_BASELINE, LEFT_AXIS_TYPE, 4, 0 );
    y_axis_label( r, resource_string(VELOCITY_TYPE_STRING), units_name(Ph.curves[i].units), LEFT_AXIS_TYPE );
    }

}

/***********************************************************************
*                            SHOW_RIGHT_AXES                           *
***********************************************************************/
void show_right_axes()
{
int  channel;
int  true_channel;
int  x;
BOOLEAN firstime;
STRING_CLASS s1;
STRING_CLASS s2;
COLORREF     c1;
COLORREF     c2;

RECT    r;
int     i;

if ( !HaveHeader )
    return;

firstime = TRUE;
for ( channel=2; channel<NofPlotCurves; channel++ )
    {
    i = PlotCurve[channel].curve_index;
    if ( i == NO_INDEX )
        continue;

    x = FIRST_ANALOG_AXIS + channel;
    r = Region[TIME_BASED_ANALOG].rect;
    if ( firstime )
        {
        draw_axis( x, -1, r, TA_TOP, RIGHT_AXIS_TYPE, 4, 1 );
        firstime = FALSE;

        if ( IsPressureDisplay )
            {
            s1 = resource_string( PRESSURE_TYPE_STRING );
            s2 = units_name( Ph.curves[i].units );
            c1 = CurrentColor[TEXT_COLOR];
            c2 = c1;
            }
        else
            {
            true_channel = Ph.curves[i].ft_channel_number();
            if ( true_channel != NO_FT_CHANNEL )
                true_channel--;
            s1 = ChannelAxisLabel[true_channel];
            s1 += SpaceString;
            s1 += units_name( Ph.curves[i].units );

            c1 = CurrentColor[x];
            c2 = c1;
            if ( (channel+1) < NofPlotCurves )
                {
                i = PlotCurve[channel+1].curve_index;
                true_channel = Ph.curves[i].ft_channel_number();
                if ( true_channel != NO_FT_CHANNEL )
                    true_channel--;
                s2 = ChannelAxisLabel[true_channel];
                if ( i != NO_INDEX )
                    {
                    s2 += SpaceString;
                    s2 += units_name( Ph.curves[i].units );
                    }
                c2 = CurrentColor[x+1];
                }
            }
        y_axis_label( r, s2.text(), s1.text(), RIGHT_AXIS_TYPE, c2, c1 );
        }
    else
        {
        draw_axis( x, -1, r, TA_BASELINE, RIGHT_AXIS_TYPE, 4, 1 );
        }
    }

/*
-----------------------
Show the position curve
----------------------- */
i = plot_curve_index( POSITION_CURVE );
if ( i != NO_INDEX )
    {
    x = Y_POS_AXIS;

    if ( !Axis[x].is_on_hold )
        {
        r = Region[TIME_BASED_COUNT].rect;
        if ( CurrentDisplay != POS_ONLY_DISPLAY )
            draw_axis( x,     2, r, TA_BASELINE, RIGHT_AXIS_TYPE, 4, 0 );

        y_axis_label( r, resource_string(POSITION_TYPE_STRING), units_name(Ph.curves[i].units), RIGHT_AXIS_TYPE );
        }

    }
}

/***********************************************************************
*                            SHOW_X_AXES                               *
***********************************************************************/
void show_x_axes()
{
RECT    r;
int     i;
UINT    id;
int     decimal_places;
short   nof_tics;
MINMAX  range;
float   x;
TCHAR   pos_x_label[13 + UNITS_LEN];
STRING_CLASS s;

if ( !HaveHeader )
    return;

for ( i=0; i<MAX_CURVES; i++ )
    {
    if ( Ph.curves[i].type & POSITION_CURVE )
        {
        if ( CurrentDisplay == TIME_ONLY_DISPLAY )
            continue;

        if ( !Axis[X_POS_AXIS].is_on_hold )
            {
            Axis[X_POS_AXIS].min = Ph.curves[i].min;
            Axis[X_POS_AXIS].max = Points[i][Ph.last_pos_based_point];
            }

        lstrcpy( pos_x_label, resource_string(POSITION_TYPE_STRING) );
        lstrcat( pos_x_label, TEXT(" (") );
        lstrcat( pos_x_label, units_name(Ph.curves[i].units) );
        lstrcat( pos_x_label, TEXT(")") );

        range.max = Axis[X_POS_AXIS].max;
        range.min = Axis[X_POS_AXIS].min;

        if ( range.max >= 1000.0 )
            decimal_places = 1;
        else if ( range.max < 10.0 )
            decimal_places = 3;
        else
            decimal_places = 2;

        r = Region[POS_BASED_COUNT].rect;
        nof_tics = POS_BASED_X_TICS;
        if ( CurrentDisplay != POS_TIME_DISPLAY )
            nof_tics += TIME_BASED_X_TICS;
        draw_x_axis( range, decimal_places, r, nof_tics, 0, CurrentColor[X_POS_AXIS] );
        x_axis_label( r, pos_x_label );
        }

    if ( Ph.curves[i].type == TIME_CURVE )
        {
        if ( CurrentDisplay == POS_ONLY_DISPLAY )
            continue;

        if ( !Axis[TIME_AXIS].is_on_hold )
            {
            Axis[TIME_AXIS].max = Ph.curves[i].max;
            if ( CurrentDisplay != TIME_ONLY_DISPLAY )
                {
                /*
                -------------------------------------------
                Set the time axis min to the time at impact
                ------------------------------------------- */
                Axis[TIME_AXIS].min = Points[i][Ph.last_pos_based_point];

                /*
                ------------------------------------------------
                Use the post impact display time if there is one
                ------------------------------------------------ */
                if ( PostImpactDisplayTime > 0.0 )
                    {
                    x = Axis[TIME_AXIS].min + PostImpactDisplayTime;
                    if ( x < Axis[TIME_AXIS].max )
                        Axis[TIME_AXIS].max = x;
                    }
                }
            else
                {
                Axis[TIME_AXIS].min = 0.0;
                }
            }

        range.max = Axis[TIME_AXIS].max;
        range.min = Axis[TIME_AXIS].min;

        nof_tics       = TIME_BASED_X_TICS;
        id = SEC_TIME_LABEL_STRING;

        if ( CurrentDisplay == POS_TIME_DISPLAY )
            {
            /*
            --------------------------------
            Display the axis in milliseconds
            -------------------------------- */
            range.max -= range.min;
            range.min = 0.0;
            range.max *= 1000.0;
            decimal_places = 0;
            id = MS_TIME_LABEL_STRING;
            }
        else
            {
            decimal_places = 3;
            nof_tics += POS_BASED_X_TICS;
            }

        r = Region[TIME_BASED_COUNT].rect;
        draw_x_axis( range, decimal_places, r, nof_tics, 0, CurrentColor[TIME_AXIS] );
        s = resource_string( id );
        x_axis_label( r, s.text() );
        }
    }

}

/***********************************************************************
*                           CREATE_BIGFONT                             *
***********************************************************************/
static void create_bigfont( HDC dc )
{
const TCHAR big_number[] = TEXT( "0000.00" );
const TCHAR gap_text[]   = TEXT( "WW" );

long  height;
long  width;
long  max_height;

int32 i;
int32 j;
int32 n;
int32 y;
int32 gap_len;
int32 name_len;
int32 number_len;
int32 navg;
int32 slen;
float value;
COLORREF my_text_color;
TCHAR s[MAX_DOUBLE_LEN+1];

HFONT      BigFont;
SIZE       textsize;
TEXTMETRIC tm;

slen = lstrlen( big_number );
GetTextExtentPoint32( dc, big_number, slen, &textsize );
number_len = textsize.cx;

slen = lstrlen( gap_text );
GetTextExtentPoint32( dc, gap_text, slen, &textsize );
gap_len = textsize.cx;

name_len = 0;
n = 0;
/*
------------------------------------------------------------
Get the length of the longest parameter name plus the number
------------------------------------------------------------ */
for ( j=0; j<NofCornerParameters; j++ )
    {
    i = CornerParameters[j];
    if ( i > 0 && i < MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            {
            slen = lstrlen( CurrentParam.parameter[i].name );
            GetTextExtentPoint32( dc, CurrentParam.parameter[i].name, slen, &textsize );
            if ( textsize.cx > name_len )
                name_len = textsize.cx;
            n++;
            }
        }
    }

/*
----------------------------------------------------------
Calculate the number of average characters this represents
---------------------------------------------------------- */
GetTextMetrics( dc, &tm );
navg = (name_len + gap_len + number_len) / tm.tmAveCharWidth;

/*
--------
Round up
-------- */
navg++;

if ( n <= 0 )
    n = 1;

/*
----------------------------------------------------------
Now, calculate the size of the largest average char to fit
---------------------------------------------------------- */
width = (PlotWindowRect.right - PlotWindowRect.left ) / navg;
height          = (PlotWindowRect.bottom - PlotWindowRect.top) / n;

max_height = 3*width + width/2;
if ( height > max_height )
    height = max_height;

BigFont = create_font( dc, width, height, ArialBoldFontName.text() );

if ( !BigFont )
    return;

SelectObject( dc, BigFont );
GetTextMetrics( dc, &tm );

SetBkColor(   dc, CurrentColor[BACKGROUND_COLOR] );
y = PlotWindowRect.top + tm.tmHeight;

for ( j=0; j<NofCornerParameters; j++ )
    {
    i = CornerParameters[j];
    if ( i > 0 && i < MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            {
            SetTextColor( dc, CurrentColor[TEXT_COLOR]       );
            SetTextAlign( dc, TA_BOTTOM | TA_LEFT );
            slen = lstrlen( CurrentParam.parameter[i].name );
            TextOut( dc, PlotWindowRect.left, y, CurrentParam.parameter[i].name, slen );

            SetTextAlign( dc, TA_BOTTOM | TA_RIGHT );
            value = Parms[i];
            rounded_double_to_tchar( s, (double) value );
            slen  = lstrlen( s );

            if ( is_alarm(i, value) )
                my_text_color = AlarmColor;
            else if ( is_warning(i, value) )
                my_text_color = WarningColor;
            else
                my_text_color = CurrentColor[TEXT_COLOR];

            SetTextColor( dc, my_text_color );
            TextOut( dc, PlotWindowRect.right, y, s, slen );

            y += tm.tmHeight;
            }
        }
    }

SelectObject( dc, GetStockObject(SYSTEM_FIXED_FONT) ) ;
DeleteObject( BigFont );
}

/***********************************************************************
*                             LOAD_MYFONT                              *
*                                                                      *
*                    The rectangle is the plot window.                 *
*                                                                      *
***********************************************************************/
static void load_myfont()
{

long       lines_per_page;
long       height;
HFONT      f;
TEXTMETRIC tm;

height = PlotWindowRect.bottom - PlotWindowRect.top;
if ( height < 1000 )
    lines_per_page = 25L;
else
    lines_per_page = 35L;

height /= lines_per_page;

f = create_font( MyDc, 0, height, ArialFontName.text() );

if ( f )
    {
    /*
    ---------------------------------------------------------------
    The following gives me a default and assures that MyFont is not
    currently selected (it is bad form to delete the current font).
    --------------------------------------------------------------- */
    SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) ) ;

    if ( MyFont )
        DeleteObject( MyFont );

    MyFont = f;
    SelectObject( MyDc, MyFont );
    GetTextMetrics( MyDc, &tm );
    MyCharHeight = tm.tmHeight - tm.tmInternalLeading;
    MyCharWidth  = tm.tmAveCharWidth;
    }
}

/***********************************************************************
*                               X_VALUE                                *
***********************************************************************/
BOOLEAN x_value( float & dest, int & axis_index, int screen_x )
{

int    ax;
RECT * r;
float  xrange;

ax = NO_AXIS;
r    = 0;

if ( Region[POS_BASED_COUNT].is_visible )
    {
    if ( screen_x >= Region[POS_BASED_COUNT].rect.left && screen_x <= Region[POS_BASED_COUNT].rect.right )
        {
        ax = X_POS_AXIS;
        r  = &Region[POS_BASED_COUNT].rect;
        }
    }

if ( Region[TIME_BASED_COUNT].is_visible )
    {
    if ( screen_x >= Region[TIME_BASED_COUNT].rect.left && screen_x <= Region[TIME_BASED_COUNT].rect.right )
        {
        ax = TIME_AXIS;
        r  = &Region[TIME_BASED_COUNT].rect;
        }
    }

if ( ax == NO_AXIS )
    return FALSE;

axis_index = ax;
dest   = float( screen_x );
xrange = float( r->right - r->left );
dest  -= r->left;
dest  *= Axis[ax].max - Axis[ax].min;

if ( not_float_zero(xrange) )
    dest /= xrange;

dest += Axis[ax].min;

return TRUE;
}

/***********************************************************************
*                            SHOW_SHOT_INFO                            *
***********************************************************************/
void show_shot_info()
{
POINT p;
SIZE  sz;
int32 width;
STRING_CLASS s;

if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
    {
    s = MachineName;
    s += TEXT(" : ");
    s += PartName;
    s += TEXT(" : ");
    s += resource_string( SHOT_LABEL_STRING );
    s += TEXT(" ");
    s += int32toasc( extlong(ShotName, SHOT_LEN) );
    s += TEXT(" : ");
    s += DateString;

    p.y = PlotWindowRect.top + 2;
    if ( !Printing && GetTextExtentPoint32(MyDc, s.text(), s.len(), &sz) )
        {
        SetTextAlign( MyDc, TA_TOP | TA_LEFT );
        width = (PlotWindowRect.right - PlotWindowRect.left);
        width -= sz.cx;
        width /= 2;
        p.x = PlotWindowRect.left + width;
        }
    else
        {
        SetTextAlign( MyDc, TA_TOP | TA_CENTER );
        p.x = (PlotWindowRect.left + PlotWindowRect.right)/2;
        }


    SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );
    SetTextColor( MyDc, CurrentColor[TEXT_COLOR] );

    TextOut( MyDc, p.x, p.y, s.text(), s.len() );
    }
}

/***********************************************************************
*                           SHOW_OFFLINE_INFO                          *
***********************************************************************/
void show_offline_info()
{
POINT p;
SIZE  sz;
int32 width;
STRING_CLASS s;

if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
    {
    s = MachineName;
    s += TEXT(" : ");
    s += resource_string( OFF_LINE_STRING );

    p.y = PlotWindowRect.top + 2;
    if ( !Printing && GetTextExtentPoint32(MyDc, s.text(), s.len(), &sz) )
        {
        SetTextAlign( MyDc, TA_TOP | TA_LEFT );
        width = (PlotWindowRect.right - PlotWindowRect.left);
        width -= sz.cx;
        width /= 2;
        p.x = PlotWindowRect.left + width;
        }
    else
        {
        SetTextAlign( MyDc, TA_TOP | TA_CENTER );
        p.x = (PlotWindowRect.left + PlotWindowRect.right)/2;
        }


    SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );
    SetTextColor( MyDc, CurrentColor[TEXT_COLOR] );

    TextOut( MyDc, p.x, p.y, s.text(), s.len() );
    }
}

/***********************************************************************
*                              FIND_INDEX                              *
***********************************************************************/
int find_index( float xpos, int axis_index, int i )
{

int     increment;
int     min_index;
float * p;
float   min_delta;
float   delta;

if ( axis_index == X_POS_AXIS )
    p = Points[POSITION_CURVE_INDEX];
else
    p = Points[TIME_CURVE_INDEX];

min_index = i;
min_delta = absolute_value( xpos - p[i] );
increment = 1;
if ( p[i] > xpos )
    increment = -1;

while ( i >= 0 && i < NofPoints )
    {
    delta = absolute_value( xpos - p[i] );
    if ( delta <= min_delta )
        {
        min_delta = delta;
        min_index = i;
        }
    else
        break;
    i += increment;
    }

return min_index;
}

/***********************************************************************
*                              HIGH_INDEX                              *
*                                                                      *
*  Return the upper index of the first index pair that contain the     *
*  target point. high_index in [ 1, NofPoints ).                       *
*                                                                      *
*  h = high_index( 1.0, X_POS_AXIS );                                  *
*  if ( h != NO_INDEX )                                                *
*      1.0 is contained in the interval from point h-1 to h (points    *
*      start at 0).                                                    *
*                                                                      *
***********************************************************************/
int high_index( float target, int axis_index )
{
int     i;
float * p;
float   x1;
float   x2;

if ( axis_index == X_POS_AXIS )
    p = Points[POSITION_CURVE_INDEX];
else
    p = Points[TIME_CURVE_INDEX];

i = 0;
while ( i < NofPoints )
    {
    x1 = p[i];
    i++;
    x2 = p[i];

    if ( x2 < x1 )
        swap( x1, x2 );

    if ( target >= x1 && target <= x2 )
        break;
    }

if ( i >= NofPoints )
    return NO_INDEX;

return i;
}

/***********************************************************************
*                             TIME_OF_POS                              *
***********************************************************************/
static BOOLEAN time_of_pos( float & target )
{
float xmax;
float xmin;
float xrange;
float percentage;
float * p;
int   i;

i = high_index( target, X_POS_AXIS );
if ( i == NO_INDEX )
    return FALSE;

/*
------------------------------------------------------------
Positions are always pre-impact. If I am past impact, backup
------------------------------------------------------------ */
minint( i, Ph.last_pos_based_point );

p = Points[POSITION_CURVE_INDEX];

xmax   = p[i];
xmin   = p[i-1];
xrange = xmax - xmin;

p = Points[TIME_CURVE_INDEX];

if ( is_float_zero(xrange) )
    {
    target = p[i];
    return TRUE;
    }

percentage = (target - xmin) /xrange;

xmin   = p[i-1];
xrange = p[i] - xmin;

target = xmin + percentage * xrange;

return TRUE;
}

/***********************************************************************
*                             POS_OF_TIME                              *
***********************************************************************/
static BOOLEAN pos_of_time( float & target )
{
float xmax;
float xmin;
float xrange;
float percentage;
float * p;
int   i;

/*
--------------------------------------------------------------------
Times are always post-impact so start at the first time-based point.
-------------------------------------------------------------------- */
p = Points[TIME_CURVE_INDEX];
i = Ph.last_pos_based_point + 1;
while ( i < NofPoints )
    {
    if ( target <= p[i] )
        break;
    i++;
    }

minint( i, NofPoints - 1 );

xmax   = p[i];
xmin   = p[i-1];
xrange = xmax - xmin;

p = Points[POSITION_CURVE_INDEX];

if ( is_float_zero(xrange) )
    {
    target = p[i];
    return TRUE;
    }

percentage = (target - xmin) /xrange;

xmin   = p[i-1];
xrange = p[i] - xmin;

target = xmin + percentage * xrange;

return TRUE;
}

/***********************************************************************
*                           SHOW_ZOOM_VALUE                            *
***********************************************************************/
void show_zoom_value( int text_box_id, BITSETYPE curve_type, int i1, int i2, short decimal_places )
{
TCHAR s[MAX_FLOAT_LEN+1];
float x1;
float x2;
HWND w;
int  curve_index;

if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == TREND_SCREEN_TYPE || MyScreenType == BIG_PARAM_SCREEN_TYPE )
    return;

if ( MyScreenType == STATUS_SCREEN_TYPE )
    return;

curve_index = plot_curve_index( curve_type );
if ( curve_index == NO_INDEX )
    return;

if ( MyScreenType == ZABISH_SCREEN_TYPE )
    w = ZoomBoxWindow;
else
    w = SmallZoomBoxWindow;

x1 = Points[curve_index][i1];
x2 = Points[curve_index][i2];

ftoasc( s, x1 );
fix_decimals( s, decimal_places );
SetDlgItemText( w, text_box_id+START_BOX_OFFSET, s  );

ftoasc( s, x2 );
fix_decimals( s, decimal_places );
SetDlgItemText( w, text_box_id+END_BOX_OFFSET, s  );

ftoasc( s, x2-x1 );
fix_decimals( s, decimal_places );
SetDlgItemText( w, text_box_id+DELTA_BOX_OFFSET, s  );
}

/***********************************************************************
*                            SHOW_ZOOM_INFO                            *
***********************************************************************/
void show_zoom_info()
{
const int analog_tbox[NofZoomAnalogChannels] =
    {
    ROD_TEXT_BOX,
    HEAD_TEXT_BOX,
    DIFF_TEXT_BOX,
    CHAN_4_TEXT_BOX
    };
int   ax;
float lx;
int   i;
int   i1;
int   i2;
short decimal_places;

if ( x_value(lx, ax, LeftCursorX) )
    {
    LeftZoomIndex = find_index( lx, ax, LeftZoomIndex );
    if ( x_value(lx, ax, CursorX) )
        {
        ZoomIndex = find_index( lx, ax, ZoomIndex );
        }
    }

if ( LeftCursorX <= CursorX )
    {
    i1 = LeftZoomIndex;
    i2 = ZoomIndex;
    }
else
    {
    i1 = ZoomIndex;
    i2 = LeftZoomIndex;
    }

decimal_places = 4;
//if ( CurrentPart.distance_units == INCH_UNITS )
  //decimal_places = 2;

show_zoom_value( POS_TEXT_BOX,      POSITION_CURVE,      i1, i2, decimal_places );
show_zoom_value( TIME_TEXT_BOX,     TIME_CURVE,          i1, i2, 3 );
show_zoom_value( VELOCITY_TEXT_BOX, VELOCITY_CURVE,      i1, i2, 3 );

if ( IsPressureDisplay )
    {
    show_zoom_value( ROD_TEXT_BOX,      ROD_PRESSURE_CURVE,  i1, i2, 2 );
    show_zoom_value( HEAD_TEXT_BOX,     HEAD_PRESSURE_CURVE, i1, i2, 2 );
    if ( CurrentMachine.diff_curve_number != NO_DIFF_CURVE_NUMBER )
        show_zoom_value( DIFF_TEXT_BOX,     DIFF_PRESSURE_CURVE, i1, i2, 2 );
    }
else
    {
    for ( i=0; i<NofZoomAnalogChannels; i++ )
        {
        if ( ZoomAnalogType[i] )
            show_zoom_value( analog_tbox[i], ZoomAnalogType[i], i1, i2, 2 );
        }
    }
}

/***********************************************************************
*                        SHOW_CORNER_PARAMETERS                        *
***********************************************************************/
static void show_corner_parameters()
{
if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == TREND_SCREEN_TYPE || MyScreenType == BIG_PARAM_SCREEN_TYPE )
    return;

if ( MyScreenType == STATUS_SCREEN_TYPE )
    return;

InvalidateRect( ParameterWindow, 0, TRUE );
}

/***********************************************************************
*                              DRAW_MARK                               *
***********************************************************************/
void draw_mark( FLOAT target, int ax, MARKLIST_ENTRY  * me )
{
COLORS_CLASS c;

int   y;
int   penwidth;
int   rx;
int   ix;
int   slen;
UINT  align_flags;
float xrange;
float x;
float xmax;
float xmin;
HPEN  mypen;
HPEN  oldpen;

const COLORREF white_color = RGB( 255, 255, 255 );
const COLORREF black_color = RGB(   0,   0,   0 );
COLORREF pencolor;

if ( ax == X_POS_AXIS )
    rx = POS_BASED_COUNT;
else
    rx = TIME_BASED_COUNT;

xmax = Axis[ax].max;
xmin = Axis[ax].min;

xrange = xmax - xmin;

if ( is_float_zero(xrange) )
    return;

if ( target < xmin )
    {
    if ( zoomed() )
        return;
    ix = Region[rx].rect.left;
    }
else if ( xmax < target )
    {
    if ( zoomed() )
        return;
    ix = Region[rx].rect.right;
    }
else
    {
    /*
    --------------------------------------
    target - xmin
    ------------- * (right - left) + left;
     xmax - xmin
    -------------------------------------- */

    x = Region[rx].rect.right - Region[rx].rect.left;
    x *= target - xmin;
    x /= xrange;
    x += Region[rx].rect.left;
    ix = (int) x;
    }

penwidth = 1;
if ( me->symbol & BLINE_MARK )
    {
    penwidth = MyCharWidth / 2;
    maxint( penwidth, 1 );
    }

if ( MyDc )
    {
    pencolor = c[me->color];
    if ( Printing && pencolor == white_color )
        pencolor = black_color;
    mypen = CreatePen( PS_SOLID, penwidth, pencolor );
    if ( mypen )
        {
        oldpen = (HPEN) SelectObject( MyDc, mypen );
        if ( me->symbol & VLINE_MARK )
            {
            MoveToEx( MyDc, ix, Region[POS_BASED_ANALOG].rect.top,  0 );
            LineTo(   MyDc, ix, Region[POS_BASED_COUNT].rect.bottom   );
            }
        else if ( me->symbol & BLINE_MARK )
            {
            y = Region[POS_BASED_COUNT].rect.top - Region[POS_BASED_COUNT].rect.bottom;
            y /= 30;
            y += Region[POS_BASED_COUNT].rect.bottom;

            MoveToEx( MyDc, ix, y,  0 );
            LineTo(   MyDc, ix, Region[POS_BASED_COUNT].rect.bottom   );
            }

        if ( me->symbol & (VLINE_MARK | NOLINE_MARK) )
            {
            slen = lstrlen( me->label );
            if ( slen )
                {
                if ( me->symbol & VLINE_MARK )
                    align_flags = TA_BOTTOM | TA_LEFT;
                else
                    align_flags = TA_BOTTOM | TA_CENTER;
                SetTextAlign( MyDc, align_flags );
                SetBkMode(    MyDc, TRANSPARENT );
                SetTextColor( MyDc, pencolor );
                y = Region[POS_BASED_ANALOG].rect.bottom - MyCharHeight/4;
                TextOut( MyDc, ix, y, me->label, slen );
                if ( me->symbol & NOLINE_MARK )
                    {
                    y += MyCharHeight/4;
                    TextOut( MyDc, ix, y, TEXT("."), 1 );
                    }
                }
            }
        SelectObject( MyDc, oldpen );
        DeleteObject( mypen );
        }
    }

}

/***********************************************************************
*                            SHOW_POS_MARK                             *
***********************************************************************/
static void show_pos_mark( float target, MARKLIST_ENTRY * me )
{

int   ax;

ax = X_POS_AXIS;

if ( CurrentDisplay == TIME_ONLY_DISPLAY )
    {
    if ( !time_of_pos(target) )
        return;
    ax = TIME_AXIS;
    }

draw_mark( target, ax, me );
}

/***********************************************************************
*                            SHOW_TIME_MARK                            *
***********************************************************************/
static void show_time_mark( float target, MARKLIST_ENTRY * me, PARAMETER_ENTRY * pe )
{

int   ax;

/*
-------------------------------------------
Time-based parameters are in ms past impact
Convert this to seconds from start.
------------------------------------------- */
target = adjust_for_units( SECOND_UNITS, target, pe->units );
target += Points[TIME_CURVE_INDEX][Ph.last_pos_based_point];

ax = TIME_AXIS;

if ( CurrentDisplay == POS_ONLY_DISPLAY )
    {
    if ( !pos_of_time(target) )
        return;
    ax = X_POS_AXIS;
    }

draw_mark( target, ax, me );
}

/***********************************************************************
*                              SHOW_MARKS                              *
***********************************************************************/
static void show_marks()
{
short i;
short pi;
float value;
MARKLIST_ENTRY  * me;
PARAMETER_ENTRY * pe;

for ( i=0; i<CurrentMarks.n; i++ )
    {
    me =  CurrentMarks.array+i;
    switch ( me->parameter_type )
        {
        case PARAMETER_MARK:
            pi = me->parameter_number - 1;
            if ( pi < 0 || pi >= MAX_PARMS )
                continue;
            value = Parms[pi];
            pe = CurrentParam.parameter+pi;
            if ( pe->vartype & POSITION_VAR )
                {
                show_pos_mark( value, me );
                }
            else if ( pe->vartype & TIME_VAR )
                {
                show_time_mark( value, me, pe );
                }
            break;

        case START_POS_MARK:
             show_pos_mark( (float) CurrentPart.user_avg_velocity_start[me->parameter_number-1], me );
            break;

        case END_POS_MARK:
             show_pos_mark( CurrentPart.user_avg_velocity_end[me->parameter_number-1], me );
            break;

        case VEL_POS_MARK:
             show_pos_mark( CurrentPart.user_velocity_position[me->parameter_number-1], me );
            break;
        }
    }
}

/***********************************************************************
*                    SHOW_FT2_IN_LOWER_LEFT_CORNER                     *
***********************************************************************/
void show_ft2_in_lower_left_corner()
{
const UINT align_flags = TA_BASELINE | TA_LEFT;

HFONT old_font;

old_font = (HFONT) SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) );

SetTextAlign( MyDc, align_flags );
SetBkMode(    MyDc, TRANSPARENT );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );

TextOut( MyDc, PlotWindowRect.left, PlotWindowRect.bottom-3, TEXT("II") , 2 );
SelectObject( MyDc, old_font );
}

/***********************************************************************
*                           PAINT_BACKGROUND                           *
***********************************************************************/
static void paint_background()
{
const int count = 4;
const int rindex[] = { POS_BASED_ANALOG, POS_BASED_COUNT, TIME_BASED_ANALOG, TIME_BASED_COUNT };

int i;
int ri;

for ( ri=0; ri<count; ri++ )
    {
    i = rindex[ri];
    draw_grid( i );
    }

show_x_axes();
adjust_all_y_axis_limits();

if ( viewing_reference() )
    {
    if (load_reference_trace() )
        {
        adjust_all_y_axis_limits();
        load_profile();
        }
    }

show_left_axes();
show_right_axes();

if ( IsPressureDisplay )
    rod_head_label();

if ( CurrentMachine.is_ftii )
    {
    show_ft2_in_lower_left_corner();
    }
}

/***********************************************************************
*                           PAINT_THE_CURVES                           *
***********************************************************************/
static void paint_the_curves()
{

int pi;
int i;
short yaxis;

yaxis = FIRST_ANALOG_AXIS;

for ( pi=0; pi<NofPlotCurves; pi++ )
    {
    i = PlotCurve[pi].curve_index;

    if ( i == NO_INDEX )
        continue;

    /*
    -----------
    Lawyer Code
    ----------- */
    if ( !IsHPM || (CurrentDisplay == TIME_ONLY_DISPLAY) ) // || (CurrentDisplay == POS_ONLY_DISPLAY) )
        show_curve( POS_BASED_ANALOG,  yaxis, X_POS_AXIS, i, PosCurve,  CurrentColor[pi+FIRST_ANALOG_AXIS] );
    show_curve( TIME_BASED_ANALOG, yaxis,  TIME_AXIS, i, TimeCurve, CurrentColor[pi+FIRST_ANALOG_AXIS] );

    yaxis++;
    }

if ( Region[POS_BASED_COUNT].is_visible )
    show_curve( POS_BASED_COUNT, VELOCITY_AXIS, X_POS_AXIS, VELOCITY_CURVE_INDEX, PosCurve, CurrentColor[VELOCITY_AXIS] );

if ( Region[TIME_BASED_COUNT].is_visible )
    {
    show_curve( TIME_BASED_COUNT, Y_POS_AXIS, TIME_AXIS, POSITION_CURVE_INDEX, TimeCurve, CurrentColor[Y_POS_AXIS] );
    if ( CurrentDisplay == TIME_ONLY_DISPLAY )
        show_curve( TIME_BASED_COUNT, VELOCITY_AXIS, TIME_AXIS, VELOCITY_CURVE_INDEX, TimeCurve, CurrentColor[VELOCITY_AXIS] );
    }

}

/***********************************************************************
*                             SHOW_OVERLAYS                            *
***********************************************************************/
static void show_overlays()
{
int i;

if ( overlaying() && NofOverlayShots > 1 )
    {
    for ( i=1; i<NofOverlayShots; i++ )
        {
        load_profile( OverlayShot+i );
        paint_the_curves();
        }
    i = NofOverlayShots - 1;
    load_part_info( OverlayShot+i );
    }
}

/***********************************************************************
*                       SET_REALTIME_MENU_STRING                       *
***********************************************************************/
void set_realtime_menu_string( void )
{
static BOOLEAN firstime = TRUE;
UINT         id;
STRING_CLASS s;
SIZE         textsize;
HDC          dc;
HBRUSH       oldbrush;
HBRUSH       mybrush;
INT          orbkmode;
COLORREF     mycolor;
HFONT        oldfont;

if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == BIG_PARAM_SCREEN_TYPE || MyScreenType == DOWNBAR_SCREEN_TYPE )
    return;

if ( MyScreenType == STATUS_SCREEN_TYPE )
    return;

if ( RealTime )
    {
    if ( SingleMachineOnly )
        id = REALTIME_SINGLE_MENU_STRING;
    else
        id = REALTIME_ON_MENU_STRING;
    }
else
    {
    id = REALTIME_OFF_MENU_STRING;
    }

s = resource_string( id );

dc = GetDC( MainPlotWindow );

if ( RealTimeButtonRect.right > 0 )
    {
    mybrush  = CreateSolidBrush( CurrentColor[BACKGROUND_COLOR] );
    oldbrush = (HBRUSH) SelectObject( dc, mybrush );
    FillRect( dc, &RealTimeButtonRect, mybrush );
    SelectObject( dc, oldbrush );
    DeleteObject( mybrush );
    }

oldfont = 0;
if ( firstime )
    {
    if ( ArialFontName != DefaultArialFontName )
        {
        oldfont = (HFONT) SelectObject( dc, GetStockObject(DEFAULT_GUI_FONT) ) ;
        textsize.cy = character_height( dc );
        RealTimeFont = create_font( dc, 0, textsize.cy, ArialFontName.text() );
        SelectObject( dc, oldfont );
        oldfont = 0;
        }
    firstime = FALSE;
    }

if ( RealTimeFont != NULL )
    oldfont = (HFONT) SelectObject( dc, RealTimeFont );
else
    oldfont = (HFONT) SelectObject( dc, GetStockObject(DEFAULT_GUI_FONT) ) ;

GetTextExtentPoint32( dc, s.text(), s.len(), &textsize );

RealTimeButtonRect.top    = 1;
RealTimeButtonRect.left   = 0;
RealTimeButtonRect.right  = textsize.cx + 3;
RealTimeButtonRect.bottom = RealTimeButtonRect.top + textsize.cy - 3;
if ( (MainWindowRect.right) > 640 )
    RealTimeButtonRect.bottom++;

if ( id == REALTIME_OFF_MENU_STRING )
    mycolor = AlarmColor;
else
    mycolor = GetSysColor( COLOR_MENU );

mybrush  = CreateSolidBrush( mycolor );
oldbrush = (HBRUSH) SelectObject( dc, mybrush );
FillRect( dc, &RealTimeButtonRect, mybrush );
SelectObject( dc, oldbrush );
DeleteObject( mybrush );

orbkmode = GetBkMode( dc );
SetBkMode( dc, TRANSPARENT );

SetTextAlign( dc, TA_TOP | TA_LEFT );
SetTextColor( dc, RGB(0, 0, 0) );
TextOut( dc, RealTimeButtonRect.left+1, RealTimeButtonRect.top - 1, s.text(), s.len() );

SetBkMode( dc, orbkmode );
if ( oldfont )
    SelectObject( dc, oldfont );

ReleaseDC( MainPlotWindow, dc );
}

/***********************************************************************
*                        PAINT_BIG_PARAM_DISPLAY                       *
***********************************************************************/
static void paint_big_param_display( PAINTSTRUCT & ps )
{
create_bigfont( ps.hdc );
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static void paint_me()
{
COMPUTER_CLASS c;
BOOLEAN     need_to_set_curves;
PAINTSTRUCT ps;
int         i;

if ( MyScreenType == PROFILE_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE || MyScreenType == ZABISH_SCREEN_TYPE )
    {
    if ( !c.online_status(ComputerName) )
        {
        BeginPaint( MainPlotWindow, &ps );
        MyDc = ps.hdc;
        show_offline_info();
        MyDc = NULL;
        EndPaint( MainPlotWindow, &ps );
        return;
        }
    }
/*
---------------------------------------
Do nothing if there is no update region
--------------------------------------- */
if ( Printing || !GetUpdateRect(MainPlotWindow, NULL, FALSE) )
    {
    BeginPaint( MainPlotWindow, &ps );
    EndPaint( MainPlotWindow, &ps );
    return;
    }

if ( overlaying() && NofOverlayShots > 1 )
    {
    HaveHeader = FALSE;
    load_part_info( OverlayShot );
    }

if ( !HaveHeader )
    {
    need_to_set_curves = TRUE;
    load_profile();
    if ( need_to_set_curves )
        {
        clear_y_axes();
        set_plot_curves();
        }
    }

BeginPaint( MainPlotWindow, &ps );

if ( MyScreenType == BOX_SCREEN_TYPE )
    {
    paint_box_display( ps );
    }
else if ( MyScreenType == STATUS_SCREEN_TYPE )
    {
    paint_status_display( ps );
    }
else if ( HaveHeader && MyScreenType == BIG_PARAM_SCREEN_TYPE )
    {
    paint_big_param_display( ps );
    }
else if ( HaveHeader || MyScreenType == TREND_SCREEN_TYPE || MyScreenType == DOWNBAR_SCREEN_TYPE )
    {
    /*
    ----------------------------------------------------
    Set the global device context so everyone can use it
    ---------------------------------------------------- */
    MyDc = ps.hdc;

    /*
    -------------
    Load the font
    ------------- */
    select_myfont();

    if ( MyScreenType == TREND_SCREEN_TYPE )
        {
        paint_trend_display( ps.rcPaint );
        }

    else if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
        {
        paint_downbar_display();
        }
    else
        {
        /*
        -------------------------------------------------------------
        The screen has been cleared so I need to paint the background
        ------------------------------------------------------------- */
        paint_background();
        paint_the_curves();
        show_overlays();
        show_marks();
        show_shot_info();

        for ( i=0; i<NOF_AXES; i++ )
            {
            if ( Axis[i].is_fixed )
                {
                show_fixed_axis_message();
                break;
                }
            }

        if ( viewing_reference() )
            {
            if ( load_reference_trace() )
                {
                if ( viewing_reference() )   //Lawyer Code, check viewing_ref again for and old ref trace
                    {
                    CurrentColor = ReferenceColor;
                    paint_the_curves();
                    CurrentColor = NormalColor;
                    }
                load_profile();
                }
            }
        }

    MyDc = NULL;
    }

EndPaint( MainPlotWindow, &ps );
set_realtime_menu_string();
}

/***********************************************************************
*                                                                      *
*                   SIZE_RECTANGLES_TO_CURRENT_DISPLAY                 *
*                                                                      *
*  For the pos/time display, the right edge of the pos-based and       *
*  left edge of the time-based plots are 7/10ths of the way            *
*  across the ploting region.                                          *
*                                                                      *
*  Otherwise I set all the windows to the total width.                 *
*                                                                      *
***********************************************************************/
static void size_rectangles_to_current_display()
{

int  x;
RECT r;

r.top    = Region[POS_BASED_ANALOG].rect.top;
r.left   = Region[POS_BASED_ANALOG].rect.left;
r.right  = Region[TIME_BASED_COUNT].rect.right;
r.bottom = Region[TIME_BASED_COUNT].rect.bottom;

if ( CurrentDisplay == POS_TIME_DISPLAY )
    {
    x = ( r.right - r.left ) * 7;
    x /= 10;
    x += r.left;
    }
else
    x = r.right;

Region[POS_BASED_ANALOG].rect.right = x;
Region[POS_BASED_COUNT].rect.right  = x;

if ( CurrentDisplay == POS_TIME_DISPLAY )
    x++;
else
    x = r.left;

Region[TIME_BASED_ANALOG].rect.left = x;
Region[TIME_BASED_COUNT].rect.left  = x;

}

/***********************************************************************
*                                                                      *
*                         SIZE_PROFILE_WINDOW                          *
*                                                                      *
*  This should only be called in response to an actual size change     *
*  because I create a new font each time. Since everything is global,  *
*  it is not necessary to call for any other reason.                   *
*                                                                      *
*  Creating the new font also sets the size of the MyCharHeight and    *
*  MyCharWidth variables.                                              *
*                                                                      *
*  You should get the device context (MyDc) before calling this.       *
*                                                                      *
***********************************************************************/
static void size_profile_window( RECT mr )
{

int  x;
int  y;
SIZE s;;

PlotWindowRect = mr;

if ( !Printing )
    load_myfont();

mr.top += MyCharHeight;

Region[BOTTOM_AXIS].rect.left   = mr.left;
Region[BOTTOM_AXIS].rect.right  = mr.right;
y                               = mr.bottom - 1;
Region[BOTTOM_AXIS].rect.bottom = y;
y                              -= 3 * MyCharHeight;
Region[BOTTOM_AXIS].rect.top    = y;

y--;
Region[POS_BASED_COUNT].rect.bottom  = y;
Region[TIME_BASED_COUNT].rect.bottom = y;

y                                    = mr.top + MyCharHeight;
Region[POS_BASED_ANALOG].rect.top    = y;
Region[TIME_BASED_ANALOG].rect.top   = y;

y = y + (Region[POS_BASED_COUNT].rect.bottom - y) / 2;

Region[POS_BASED_ANALOG].rect.bottom  = y;
Region[TIME_BASED_ANALOG].rect.bottom = y;

y++;
Region[POS_BASED_COUNT].rect.top      = y;
Region[TIME_BASED_COUNT].rect.top     = y;

/*
-------------------------------------------------------------
See how much room I need for two vertical labels and a number
------------------------------------------------------------- */
GetTextExtentPoint32( MyDc, TEXT("W P 000000."), 11, &s );
x = s.cx;

Region[POS_BASED_ANALOG].rect.left   = mr.left  + x;
Region[POS_BASED_COUNT].rect.left    = mr.left  + x;
Region[TIME_BASED_ANALOG].rect.right = mr.right - x;
Region[TIME_BASED_COUNT].rect.right  = mr.right - x;

size_rectangles_to_current_display();

Region[LEFT_AXIS].rect.left   = mr.left;
Region[LEFT_AXIS].rect.right  = Region[POS_BASED_ANALOG].rect.left - 1;
Region[LEFT_AXIS].rect.top    = mr.top;
Region[LEFT_AXIS].rect.bottom = mr.bottom - 1;

Region[RIGHT_AXIS].rect.left   = Region[TIME_BASED_ANALOG].rect.right + 1;
Region[RIGHT_AXIS].rect.right  = mr.right;
Region[RIGHT_AXIS].rect.top    = mr.top;
Region[RIGHT_AXIS].rect.bottom = mr.bottom - 1;

}

/***********************************************************************
*                       SIZE_PLOT_WINDOW_TO_SCREEN                     *
***********************************************************************/
static void size_plot_window_to_screen()
{
RECT  mr;

GetClientRect( MainPlotWindow, &mr);
MyDc = GetDC( MainPlotWindow );
size_profile_window( mr );
ReleaseDC(MainPlotWindow, MyDc );
MyDc = NULL;
}

/***********************************************************************
*                        PRINTER_ZOOM_BOX_HEIGHT                       *
*   This assumes you have already created the printer font.            *
***********************************************************************/
static int printer_zoom_box_height()
{
const int  lineheight = 1;
TEXTMETRIC tm;
int        ch;
int        vgap;
int        total;

GetTextMetrics( MyDc, &tm );

ch     = tm.tmHeight - tm.tmInternalLeading;
vgap   = tm.tmExternalLeading;

total = 4*vgap + 7*(vgap + ch) + 5*lineheight;

return total;
}

/***********************************************************************
*                     PRINTER_PARAMETER_BOX_HEIGHT                     *
*   This assumes you have already created the printer font.            *
***********************************************************************/
static int printer_parameter_box_height()
{
const int  lineheight = 1;
TEXTMETRIC tm;
int        ch;
int        n;
int        vgap;
int        total;

GetTextMetrics( MyDc, &tm );

ch     = tm.tmHeight - tm.tmInternalLeading;
vgap   = tm.tmExternalLeading;

if ( ViewParametersDisplay )
    {
    n = CurrentParam.count() / 2;
    if ( n*2 < CurrentParam.count() )
        n++;
    }
else
    {
    n = NofCornerParameters/2;
    if ( 2*n < NofCornerParameters )
        n++;
    }
/*
---------------------
Add one for the title
--------------------- */
n++;

total = 4*vgap + n*(vgap + ch) + 5*lineheight;

return total;
}

/***********************************************************************
*                            DRAW_RECTANGLE                            *
***********************************************************************/
static void draw_rectangle( RECT & r )
{
MoveToEx( MyDc, r.left, r.top, 0 );
LineTo( MyDc, r.right, r.top    );
LineTo( MyDc, r.right, r.bottom );
LineTo( MyDc, r.left,  r.bottom );
LineTo( MyDc, r.left,  r.top    );
}

/***********************************************************************
*                            PRINT_ZOOM_BOX                            *
***********************************************************************/
static void print_zoom_box()
{
TEXTMETRIC tm;
int        col;
int        row;
int        dx;
int        dy;
int        x;
int        y;
int        y0;
TCHAR      s[MAX_FLOAT_LEN+1];
UINT       id;
RECT       r;
HWND       w;

if ( MyScreenType == ZABISH_SCREEN_TYPE )
    w = ZoomBoxWindow;
else
    w = SmallZoomBoxWindow;

GetTextMetrics( MyDc, &tm );

dy = tm.tmHeight - tm.tmInternalLeading; + tm.tmExternalLeading;
dy *=3;
dy /=2;

y  = PlotWindowRect.bottom + 2*dy;
r  = PlotWindowRect;
r.top = y - dy/2;
r.bottom = y + 4*dy;

dx = PlotWindowRect.right - PlotWindowRect.left;
dx /= 6;

SetBkMode(    MyDc, TRANSPARENT );
SetTextAlign( MyDc, TA_TOP | TA_CENTER );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );

x = dx + dx/2;

text_out( x, y, ZOOM_PRINT_POSITION_STRING );
x += dx;
text_out( x, y, ZOOM_PRINT_TIME_STRING );
x += dx;

text_out( x, y, ZOOM_PRINT_VELOCITY_STRING );
x += dx;

text_out( x, y, ZOOM_PRINT_ROD_PRES_STRING );
x += dx;

text_out( x, y, ZOOM_PRINT_HEAD_PRES_STRING );

y += dy - 4;

draw_line( r.left, y, r.right, y );

y += 8;
x = r.left + tm.tmAveCharWidth;

SetTextAlign( MyDc, TA_TOP | TA_LEFT );
text_out( x, y,      ZOOM_PRINT_START_STRING );
text_out( x, y+dy,   ZOOM_PRINT_END_STRING   );
text_out( x, y+2*dy, ZOOM_PRINT_DELTA_STRING );

x = dx;
x *= 7;
x /= 4;
SetTextAlign( MyDc, TA_TOP | TA_RIGHT );

y0 = y;
id = FIRST_ZOOM_VALUE_TBOX;
for ( col = 0; col<5; col++ )
    {
    y = y0;
    for ( row=0; row<3; row++ )
        {
        get_text( s, w, id, MAX_FLOAT_LEN );
        text_out( x, y, s );
        y += dy;
        id++;
        }
    id += 7;
    x += dx;
    }

draw_rectangle( r );

x = dx;
for ( col = 0; col<5; col++ )
    {
    draw_line( x, r.top, x, r.bottom );
    x += dx;
    }

}

/***********************************************************************
*                       SHOW_PRINTER_PARAMETERS                        *
***********************************************************************/
static void show_printer_parameters()
{
const int  lineheight = 1;
const int  linewidth  = 1;

TEXTMETRIC tm;
int        ascent;
int        ch;
int        hgap;
int        i;
int        j;
int        n;
int        nof_lines;
int        vgap;
int        dx;
int        x;
int        y;
RECT       r;
INT        orbkmode;
TCHAR    * cp;
HPEN       mypen;
HPEN       oldpen;
COLORREF   my_text_color;

GetTextMetrics( MyDc, &tm );

ch     = tm.tmHeight - tm.tmInternalLeading;
vgap   = tm.tmExternalLeading;
ascent = tm.tmAscent - tm.tmInternalLeading;
hgap   = tm.tmAveCharWidth / 2;

orbkmode = GetBkMode( MyDc );
SetBkMode(    MyDc, TRANSPARENT );
SetTextAlign( MyDc, TA_BASELINE | TA_LEFT );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );

x  = PlotWindowRect.left + hgap + linewidth;
y  = PlotWindowRect.bottom + vgap;
dx = (PlotWindowRect.right - PlotWindowRect.left)/2  - 5*linewidth;
dx /= 12;

if ( ViewParametersDisplay )
    n = CurrentParam.count();
else
    n = NofCornerParameters;

/*
-----------------------------
Calculate the number of lines
----------------------------- */
nof_lines = n/2;
if ( 2*nof_lines < n )
    nof_lines++;

for ( j=0; j<n; j++ )
    {
    if ( j == 0 || j == nof_lines )
        {
        if ( j == 0 )
            x  = PlotWindowRect.left + hgap + linewidth;
        else
            x = (PlotWindowRect.right + PlotWindowRect.left)/2 + 2*linewidth + hgap;

        y  = PlotWindowRect.bottom + vgap + lineheight + 3*vgap + ascent;

        TextOut( MyDc, x,      y, ParameterListTitle[0].text(), ParameterListTitle[0].len() );
        TextOut( MyDc, x+5*dx, y, ParameterListTitle[1].text(), ParameterListTitle[1].len() );
        TextOut( MyDc, x+7*dx, y, ParameterListTitle[2].text(), ParameterListTitle[2].len() );
        TextOut( MyDc, x+9*dx, y, ParameterListTitle[3].text(), ParameterListTitle[3].len() );

        y += ch + lineheight + 2*vgap;
        }

    if ( ViewParametersDisplay )
        i = j+1;
    else
        i = CornerParameters[j];

    if ( i > 0 && i <= MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            {
            cp = CurrentParam.parameter[i].name;
            TextOut( MyDc, x, y, cp, lstrlen(cp) );

            cp = ascii_float( CurrentParam.parameter[i].limits[LowLimitType].value );
            TextOut( MyDc, x+5*dx, y, cp, lstrlen(cp) );

            cp = ascii_float( CurrentParam.parameter[i].limits[HighLimitType].value );
            TextOut( MyDc, x+7*dx, y, cp, lstrlen(cp) );

            if ( is_alarm(i, Parms[i]) )
                my_text_color = AlarmColor;
            else if ( is_warning(i, Parms[i]) )
                my_text_color = WarningColor;
            else
                my_text_color = CurrentColor[TEXT_COLOR];

            SetTextColor( MyDc, my_text_color );

            cp = ascii_float( Parms[i] );
            TextOut( MyDc, x+9*dx, y, cp, lstrlen(cp) );

            SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );

            cp = units_name( CurrentParam.parameter[i].units );
            TextOut( MyDc, x+11*dx, y, cp, lstrlen(cp) );
            }

        y += ch + vgap;
        }
    }

SetBkMode( MyDc, orbkmode );

mypen  = CreatePen( PS_SOLID, 1, CurrentColor[BORDER_COLOR]   );
if ( mypen )
    {
    /*
    -----------------------------
    Draw the box around the title
    ----------------------------- */
    oldpen = (HPEN) SelectObject( MyDc, mypen );

    r.left   = PlotWindowRect.left;
    r.top    = PlotWindowRect.bottom + 2*vgap;
    r.right  = r.left + (PlotWindowRect.right - PlotWindowRect.left)/2  - linewidth;
    r.bottom = r.top + vgap + ch + vgap + lineheight;

    draw_rectangle( r );

    /*
    ----------------------------------
    Draw the box around the parameters
    ---------------------------------- */
    r.top     = r.bottom;
    r.bottom += vgap + nof_lines*(ch + vgap) + lineheight;

    draw_rectangle( r );

    /*
    -----------------------------
    Draw the box around the title
    ----------------------------- */
    r.left   = r.right + 1 + linewidth;
    r.top    = PlotWindowRect.bottom + 2*vgap;
    r.right  = PlotWindowRect.right;
    r.bottom = r.top + vgap + ch + vgap + lineheight;

    draw_rectangle( r );

    /*
    ----------------------------------
    Draw the box around the parameters
    ---------------------------------- */
    r.top     = r.bottom;
    r.bottom += vgap + nof_lines*(ch + vgap) + lineheight;
    draw_rectangle( r );

    SelectObject( MyDc, oldpen );
    DeleteObject( mypen );
    }
}

/***********************************************************************
*                               PRINT_ME                               *
***********************************************************************/
void print_me()
{
static DOCINFO di = { sizeof(DOCINFO), TEXT("TrueTrak Profile"), 0 };
BOOLEAN        need_to_set_curves;
int            x;
RECT           mr;
POINT          p;
HDC            dc;

if ( (PrintingMultiplePlots && MultipleShotDialogWindow) || QuickPrint )
    dc = get_default_printer_dc();
else
    dc = get_printer_dc();

QuickPrint = FALSE;

if ( dc != 0 )
    {
    Printing = TRUE;
    MyDc     = dc;

    if ( overlaying() && NofOverlayShots > 1 )
        {
        HaveHeader = FALSE;
        load_part_info( OverlayShot );
        }

    if ( !HaveHeader || viewing_reference() )
        {
        need_to_set_curves = FALSE;
        if ( !HaveHeader )
            need_to_set_curves = TRUE;
        load_profile();
        if ( need_to_set_curves )
            {
            clear_y_axes();
            set_plot_curves();
            }
        }

    CurrentColor = PrNormalColor;

    if ( StartDoc(MyDc, &di) > 0 )
        {
        if ( StartPage(MyDc) > 0 )
            {
            p.x = GetDeviceCaps( MyDc, PHYSICALOFFSETX );
            p.y = GetDeviceCaps( MyDc, PHYSICALOFFSETY );

            mr.left   = p.x;
            mr.top    = p.y;
            mr.right  = GetDeviceCaps( MyDc, HORZRES ) - p.x;
            mr.bottom = GetDeviceCaps( MyDc, VERTRES ) - p.y;

            x = GetDeviceCaps( MyDc, LOGPIXELSX );

            if ( PrinterMargin.left > 0 )
                mr.left  += (x * PrinterMargin.left) / 1000;

            if ( PrinterMargin.right > 0 )
                mr.right -= (x * PrinterMargin.right) / 1000;

            x = GetDeviceCaps( MyDc, LOGPIXELSY );

            if ( PrinterMargin.top > 0 )
                mr.top += (x * PrinterMargin.top) / 1000;

            if ( PrinterMargin.bottom > 0 )
                mr.bottom -= (x * PrinterMargin.bottom) / 1000;

            create_printer_font( mr );
            select_printer_font();

            if ( zoomed() )
                mr.bottom -= printer_zoom_box_height();
            else
                mr.bottom -= printer_parameter_box_height();

            size_profile_window( mr );

            if ( AlarmSummaryDialogWindow )
                {
                print_alarm_summary();
                }
            else if ( MyScreenType == TREND_SCREEN_TYPE )
                {
                paint_trend_display( mr );
                }
            else if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
                {
                paint_downbar_display();
                }
            else if ( HaveHeader )
                {
                paint_background();
                paint_the_curves();
                show_overlays();
                show_marks();
                show_shot_info();
                if ( viewing_reference() )
                    {
                    if ( load_reference_trace() )
                        {
                        if ( viewing_reference() )  // Lawyer Code incase of old ref trace
                            {
                            CurrentColor = PrReferenceColor;
                            paint_the_curves();
                            CurrentColor = PrNormalColor;
                            }
                        load_profile();
                        }
                    }

                if ( zoomed() )
                    print_zoom_box();
                else
                    show_printer_parameters();
                }

            select_myfont();
            delete_printer_font();

            if ( EndPage(MyDc) > 0 )
                EndDoc( MyDc );
            }
        }
    Printing = FALSE;
    DeleteDC( MyDc );
    MyDc = NULL;
    CurrentColor = NormalColor;
    }

/*
------------------------------------
Resize the window to the screen size
------------------------------------ */
size_plot_window_to_screen();
}

/***********************************************************************
*                           TURN_OFF_ZOOMING                           *
***********************************************************************/
static void turn_off_zooming()
{

if ( Zooming )
    {
    Zooming = FALSE;
    erase_cursor_line();
    erase_left_cursor_line();
    }
}

/***********************************************************************
*                         DO_OVERLAY_TOGGLE                            *
***********************************************************************/
static void do_overlay_toggle( int id )
{
NofOverlayShots = 0;
if ( !OverlayMenuState )
    append_overlay_shot();

toggle_menu_view_state( OverlayMenuState, id );
if ( ChosshotWindow )
    SendMessage( ChosshotWindow, WM_OVERLAY_CHANGE, 0, 0L );
}

/***********************************************************************
*                         SET_ZOOM_GRAYNESS                            *
*  Originally this was just for zoom menu items but now is for         *
*  any that should just work with the profile plot screen.             *
***********************************************************************/
static void set_zoom_grayness()
{
UINT  item[] = {
    ZOOM_MENU,
    UNZOOM_MENU,
    VIEW_REFERENCE_TRACE_MENU,
    OVERLAY_MENU,
    SAVE_AS_MASTER_MENU,
    CHOOSE_SHOT_MENU
    };

const int32 NOF_ITEMS = sizeof(item)/sizeof(UINT);

int32 i;
UINT  menu_flags;
HMENU menu_handle;

if ( MyScreenType == STANDARD_SCREEN_TYPE || MyScreenType == ZABISH_SCREEN_TYPE )
    menu_flags = MF_ENABLED;
else
    menu_flags = MF_GRAYED;

menu_handle = GetMenu( MainWindow );

if ( menu_handle )
    {
    for ( i=0; i<NOF_ITEMS; i++ )
        EnableMenuItem( menu_handle, item[i], menu_flags );
    }
}

/***********************************************************************
*                            LOAD_MAIN_MENU                            *
***********************************************************************/
static void load_main_menu()
{
const UINT bad_pos_ids[] = { OVERLAY_MENU, VIEW_REFERENCE_TRACE_MENU, SAVE_AS_MASTER_MENU, POS_ONLY_MENU };
const NOF_BAD_POS_IDS = sizeof(bad_pos_ids)/sizeof(UINT);

const UINT bad_time_ids[] = { POS_ONLY_MENU };
const NOF_BAD_TIME_IDS = sizeof(bad_time_ids)/sizeof(UINT);

const UINT bad_other_ids[] = { POS_ONLY_MENU };
const NOF_BAD_OTHER_IDS = sizeof(bad_other_ids)/sizeof(UINT);

static TCHAR ProfileMenuFile[] = TEXT( "Profile" );

bool have_menu;
BOOLEAN localstate;

have_menu = false;

if ( IsHPM )
    {
    if ( MyScreenType == STANDARD_SCREEN_TYPE || MyScreenType == ZABISH_SCREEN_TYPE )
        {
        if ( CurrentDisplay == POS_TIME_DISPLAY || CurrentDisplay == POS_ONLY_DISPLAY )
            {
            load_menu( MainWindow, ProfileMenuFile, CurrentPasswordLevel, bad_pos_ids, NOF_BAD_POS_IDS );
            have_menu = true;
            }
        else if ( CurrentDisplay == TIME_ONLY_DISPLAY )
            {
            load_menu( MainWindow, ProfileMenuFile, CurrentPasswordLevel, bad_time_ids, NOF_BAD_TIME_IDS );
            have_menu = true;
            }
        }
    }

if ( !have_menu )
    load_menu( MainWindow, ProfileMenuFile, CurrentPasswordLevel, bad_other_ids, NOF_BAD_OTHER_IDS );

set_zoom_grayness();

if ( !HaveMonitorProgram )
    EnableMenuItem( GetMenu(MainWindow), BOARD_MONITOR_MENU, MF_GRAYED );

if ( !HaveSureTrakControl )
    EnableMenuItem( GetMenu(MainWindow), SURETRAK_CONTROL_MENU, MF_GRAYED );

/*
------------------------------------------------
I need to check the menu items if they are valid
Since the state is already set, I will use a
local variable.
------------------------------------------------ */
if ( overlaying() )
    {
    localstate = FALSE;
    toggle_menu_view_state( localstate, OVERLAY_MENU );
    }

if ( viewing_reference() )
    {
    localstate = FALSE;
    toggle_menu_view_state( localstate, VIEW_REFERENCE_TRACE_MENU );
    }
}

/***********************************************************************
*                         SET_CURRENT_DISPLAY                          *
***********************************************************************/
static void set_current_display( int new_display_state )
{
clear_y_axes();

CurrentDisplay = new_display_state;

if ( CurrentDisplay == POS_TIME_DISPLAY )
    {
    Region[POS_BASED_ANALOG].is_visible  = TRUE;
    Region[POS_BASED_COUNT].is_visible   = TRUE;
    Region[TIME_BASED_ANALOG].is_visible = TRUE;
    Region[TIME_BASED_COUNT].is_visible  = TRUE;
    }

else if ( CurrentDisplay == POS_ONLY_DISPLAY )
    {
    Region[POS_BASED_ANALOG].is_visible  = TRUE;
    Region[POS_BASED_COUNT].is_visible   = TRUE;
    Region[TIME_BASED_ANALOG].is_visible = FALSE;
    Region[TIME_BASED_COUNT].is_visible  = FALSE;
    }

else if ( CurrentDisplay == TIME_ONLY_DISPLAY )
    {
    Region[POS_BASED_ANALOG].is_visible  = FALSE;
    Region[POS_BASED_COUNT].is_visible   = FALSE;
    Region[TIME_BASED_ANALOG].is_visible = TRUE;
    Region[TIME_BASED_COUNT].is_visible  = TRUE;
    }

size_rectangles_to_current_display();

load_main_menu();
}

/***********************************************************************
*                     TOGGLE_POS_TIME_DISPLAY_TYPE                     *
***********************************************************************/
void toggle_pos_time_display_type()
{
int new_display_type;

if (  MyScreenType != STANDARD_SCREEN_TYPE && MyScreenType != ZABISH_SCREEN_TYPE )
    return;

if ( IsHPM )
    return;

new_display_type = NO_DISPLAY;

if ( CurrentDisplay == POS_TIME_DISPLAY )
    new_display_type = POS_ONLY_DISPLAY;
else if ( CurrentDisplay == POS_ONLY_DISPLAY )
    new_display_type = POS_TIME_DISPLAY;

if ( new_display_type != NO_DISPLAY )
    {
    set_current_display( new_display_type );
    InvalidateRect( MainPlotWindow, 0, TRUE );
    }
}

/***********************************************************************
*                        CREATE_CHOSPART_WINDOW                        *
***********************************************************************/
static void create_chospart_window()
{
if ( ChospartWindow )
    {
    if ( !ChosshotWindow || !IsWindowVisible(ChosshotWindow) )
        ShowWindow( ChospartWindow, SW_SHOW );
    SetFocus( ChospartWindow );
    }
else
    {
    ChospartWindow  = CreateDialog( MainInstance, TEXT("CHOOSE_PART"), MainWindow, (DLGPROC) chospart_proc );
    }

PostMessage( ChospartWindow, WM_DBINIT, 0, 0L );
}

/***********************************************************************
*                        CREATE_CHOSSHOT_WINDOW                        *
***********************************************************************/
static void create_chosshot_window()
{
static TCHAR CsDialog[] = TEXT( "CHOOSE_SHOTS" );

if ( ChosshotWindow )
    {
    if ( IsWindowVisible(ChosshotWindow) )
        SendMessage( ChosshotWindow, WM_NEWPART, 0, 0L );
    else
        ShowWindow ( ChosshotWindow, SW_SHOW );
    }
else
    {
    ChosshotWindow = CreateDialog( MainInstance, CsDialog, MainWindow, (DLGPROC) chosshot_proc );
    }
}

/***********************************************************************
*                             SET_ZOOM_AXIS                            *
***********************************************************************/
static void set_zoom_axis( int ax, int left, int right, int region_index )
{
RECT r;
float arange;
float xrange;
float x;

r  = Region[region_index].rect;

if ( left < r.left )
    left = r.left;

if ( right > r.right )
    right = r.right;

/*
-----------------------
Get the existing ranges
----------------------- */
xrange = float( r.right - r.left );
arange = Axis[ax].max - Axis[ax].min;

right -= r.left;
x = Axis[ax].min + arange * float(right) / xrange;
if ( x < Axis[ax].max )
    Axis[ax].max = x;

left -= r.left;
x = Axis[ax].min + arange * float(left) / xrange;
if ( x > Axis[ax].min )
    Axis[ax].min = x;

Axis[ax].is_on_hold = TRUE;
}

/***********************************************************************
*                                 ZOOM                                 *
***********************************************************************/
static void zoom( POINT p )
{
int new_display;

int left;
int right;
int i;

if ( p.x == LeftCursorX )
    {
    turn_off_zooming();
    return;
    }

new_display = CurrentDisplay;

if ( p.x > LeftCursorX )
    {
    left  = LeftCursorX;
    right = p.x;
    }
else
    {
    left  = p.x;
    right = LeftCursorX;
    }

if ( CurrentDisplay == POS_TIME_DISPLAY )
    {
    i = Region[POS_BASED_ANALOG].rect.right;
    if ( right <= i )
        new_display = POS_ONLY_DISPLAY;
    else if ( left >= i )
        new_display = TIME_ONLY_DISPLAY;
    }

get_mydc();

if ( MyFont )
    SelectObject( MyDc, MyFont );
else
    SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) ) ;

if ( new_display == POS_ONLY_DISPLAY || new_display == POS_TIME_DISPLAY )
    set_zoom_axis( X_POS_AXIS, left, right, POS_BASED_ANALOG );

if ( new_display == TIME_ONLY_DISPLAY || new_display == POS_TIME_DISPLAY )
    set_zoom_axis( TIME_AXIS,  left, right, TIME_BASED_ANALOG );

set_current_display( new_display );
clear_y_axes();

release_mydc();

HaveLeftCursorLine = FALSE;
HaveCursorLine     = FALSE;
Zooming            = FALSE;

InvalidateRect( MainPlotWindow, 0, TRUE );
}

/***********************************************************************
*                                UNZOOM                                *
***********************************************************************/
static void unzoom( BOOLEAN need_update )
{
clear_axes();

LeftCursorX   = 0;
LeftZoomIndex = 0;
ZoomIndex     = 0;
HaveLeftCursorLine = FALSE;
HaveCursorLine     = FALSE;
Zooming            = FALSE;
set_info_box_type( PARAMETER_INFO_BOX );

if ( WasRealtimeBeforeZoom && !RealTime )
    {
    RealTime = WasRealtimeBeforeZoom;
    SingleMachineOnly = WasSingleMachineBeforeZoom;
    set_realtime_menu_string();
    }

if ( CurrentDisplay != DisplayBeforeZoom )
    set_current_display( DisplayBeforeZoom );

if ( need_update )
    InvalidateRect( MainPlotWindow, 0, TRUE );
}

/***********************************************************************
*                                UNZOOM                                *
***********************************************************************/
static void unzoom()
{
unzoom( TRUE );
}

/***********************************************************************
*                          NEW_SHOT_CALLBACK                           *
***********************************************************************/
void new_shot_callback( TCHAR * topic, short item, HDDEDATA edata )
{
const DWORD BYTES_TO_COPY = sizeof(TCHAR) * (MAX_INTEGER_LEN+1);
DWORD bytes_copied;
int32 shot_number;
TCHAR buf[MAX_INTEGER_LEN+1];
TCHAR topicbuf[MAX_DDE_TOPIC_LEN+1];
COMPUTER_CLASS c;

if ( !RealTime )
    return;

if ( Zooming )
    return;

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;


lstrcpy( topicbuf, topic );
fix_dde_name( topicbuf );

if ( !MachList.find(topicbuf) )
    return;

c.set_online_status( MachList.computer_name(), TRUE );

/*
------------------------------------------------------------------------
If there are more than one computers, see if this machine is on the list
------------------------------------------------------------------------ */
if ( c.count() > 1 )
    if ( !RealtimeMachineList.find(MachList.name()) )
        return;

if ( SingleMachineOnly && !strings_are_equal(MachineName, MachList.name()) )
    return;

lstrcpy( ComputerName, MachList.computer_name() );
lstrcpy( MachineName,  MachList.name()          );

if ( !strings_are_equal(MachineName, CurrentMachine.name) )
    {
    CurrentMachine.find( ComputerName, MachineName );
    init_global_curve_types();
    }
lstrcpy( PartName, CurrentMachine.current_part );

bytes_copied = DdeGetData( edata, (LPBYTE) buf, BYTES_TO_COPY, 0 );
if ( bytes_copied > 0 )
    {
    shot_number = extlong( buf, MAX_INTEGER_LEN, DECIMAL_RADIX );
    insalph( ShotName, shot_number, SHOT_LEN, TEXT('0'), DECIMAL_RADIX );
    *(ShotName+SHOT_LEN) = NullChar;
    SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
    }
}

/***********************************************************************
*                  START_PASSWORD_LEVEL_TIMEOUT_TIMER                  *
*   This, of course, does nothing if there is no timeout setting.      *
***********************************************************************/
static bool start_password_level_timeout_timer()
{
INI_CLASS ini;
NAME_CLASS s;
double     x;

if ( PasswordLevelTimeoutId != 0 )
    return false;

s.get_local_ini_file_name( VisiTrakIniFile );
if ( s.file_exists() )
    {
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    if ( ini.find(ConfigSection, PasswordLevelTimeoutKey) )
        {
        s = ini.get_string();
        x = s.real_value();
        if ( x > 0.0 )
            {
            x *= 60000.0;
            PasswordLevelTimeoutMs = (UINT) x;
            PasswordLevelTimeoutId = SetTimer( MainWindow, PASSWORD_LEVEL_TIMEOUT_TIMER_NUMBER, PasswordLevelTimeoutMs, 0 );
            }
        }
    }

if ( PasswordLevelTimeoutId != 0 )
    return true;

return false;
}

/***********************************************************************
*                          PASSWORD_LEVEL_CALLBACK                     *
***********************************************************************/
void password_level_callback( TCHAR * topic, short item, HDDEDATA edata )
{
static bool firstime = true;

int32 old_level;
int32 default_level;
TCHAR buf[MAX_INTEGER_LEN+3];
DWORD bytes_copied;
DWORD bufsize = sizeof( buf );

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

old_level = CurrentPasswordLevel;
bytes_copied = DdeGetData( edata, (LPBYTE) buf, bufsize, 0 );
if ( bytes_copied > 0 )
    {
    *(buf+MAX_INTEGER_LEN) = NullChar;
    fix_dde_name( buf );
    CurrentPasswordLevel = asctoint32( buf );
    load_main_menu();
    }

default_level = default_password_level();
if ( firstime )
    {
    firstime = false;
    if ( CurrentPasswordLevel == LOWEST_PASSWORD_LEVEL )
        set_default_password_level( false );
    if ( CurrentPasswordLevel > default_level )
        {
        start_password_level_timeout_timer();
        }
    }
else
    {
    if ( (PasswordLevelTimeoutId==0) && (old_level <= default_level) && (CurrentPasswordLevel > default_level) )
        {
        start_password_level_timeout_timer();
        }
    else if ( CurrentPasswordLevel <= default_level )
        {
        PasswordLevelTimeoutMs = 0;
        }
    }
}

/***********************************************************************
*                          NEW_PART_SETUP_CALLBACK                     *
***********************************************************************/
void new_part_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
const DWORD BYTES_TO_COPY = sizeof(TCHAR) * (PART_NAME_LEN+1);

COMPUTER_CLASS c;
DWORD bytes_copied;
TCHAR buf[PART_NAME_LEN+1];
TCHAR topicbuf[MAX_DDE_TOPIC_LEN+1];

if ( Zooming )
    return;

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

lstrcpy( topicbuf, topic );
fix_dde_name( topicbuf );

/*
----------------------------------------------------------------------------------
My assumption here is that if someone changed the part the computer must be online
---------------------------------------------------------------------------------- */
if ( MachList.get_computer_name(buf, topicbuf) )
    c.set_online_status( buf, TRUE );

if ( !strings_are_equal(topicbuf, MachineName, MACHINE_NAME_LEN) )
    return;

buf[0] = NullChar;
bytes_copied = DdeGetData( edata, (LPBYTE) buf, BYTES_TO_COPY, 0 );
if ( bytes_copied > 0 )
    {
    fix_dde_name( buf );
    if ( !strings_are_equal(buf, PartName, PART_NAME_LEN) )
        return;
    if ( Zooming )
        turn_off_zooming();
    load_part_info();
    if ( MyScreenType == TREND_SCREEN_TYPE )
        trend_screen_reset();
    else
        SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
    }
}

/***********************************************************************
*                      POSITION_MAIN_PLOT_WINDOW                       *
***********************************************************************/
void position_main_plot_window()
{
RECT pbr;
RECT r;
HWND w;

show_all_windows();

w = 0;
if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
    w = ParameterWindow;

r = my_desktop_client_rect();
if ( w )
    {
    GetWindowRect( w, &pbr );
    r.bottom = pbr.top;
    r.bottom++;
    }

GetWindowRect( MainWindow, &pbr );
r.top = pbr.bottom;

MoveWindow( MainPlotWindow, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                       CHOOSE_CORNER_PARAMETERS                       *
***********************************************************************/
void choose_corner_parameters()
{
DialogIsActive = TRUE;
DialogBox( MainInstance, TEXT("CHOOSE_PARAMETER_LIST"), MainWindow, (DLGPROC) corner_param_dialog_proc );
}

/***********************************************************************
*                      POSITION_ZOOM_BOX_WINDOW                        *
***********************************************************************/
static void position_zoom_box_window()
{
HWND w;
RECT r;
RECT cr;

if ( MyScreenType == ZABISH_SCREEN_TYPE )
    {
    w = ZoomBoxWindow;
    }
else if ( MyScreenType == STANDARD_SCREEN_TYPE )
    {
    w = SmallZoomBoxWindow;
    }
else
    {
    w = 0;
    }

if ( !w )
    return;

GetWindowRect( w, &r );
cr = my_desktop_client_rect();
OffsetRect( &r, cr.right - r.right, cr.bottom - r.bottom );
MoveWindow( w, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
if ( w == SmallZoomBoxWindow || w == ZoomBoxWindow )
    set_zoom_box_labels( w );
}

/***********************************************************************
*                              ZOOMBOXPROC                             *
***********************************************************************/
int CALLBACK ZoomBoxProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
return FALSE;
}

/***********************************************************************
*                         SMALLZOOMBOXPROC                             *
***********************************************************************/
int CALLBACK SmallZoomBoxProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
return FALSE;
}

/***********************************************************************
*                      FILL_CORNER_PARAM_LISTBOX                       *
***********************************************************************/
static void fill_corner_param_listbox()
{
short i;
short j;
BOOLEAN is_corner;

/*
-----------------------------------
Fill the corner parameters in order
----------------------------------- */
for ( j=0; j<NofCornerParameters; j++ )
    {
    i = CornerParameters[j];
    if ( i > 0 && i < MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            SendDlgItemMessage( CornerParamDialogWindow, CORNER_PARAMETER_LISTBOX, LB_ADDSTRING, 0,(LPARAM)(LPSTR) CurrentParam.parameter[i].name );
        }
    }

/*
---------------------------------
Put the rest in the available box
--------------------------------- */
for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
        {
        is_corner = FALSE;
        for ( j=0; j<NofCornerParameters; j++ )
            {
            if ( (i+1) == CornerParameters[j] )
                {
                is_corner = TRUE;
                break;
                }
            }
        if ( !is_corner )
            SendDlgItemMessage( CornerParamDialogWindow, AVAILABLE_PARAMETER_LISTBOX, LB_ADDSTRING, 0,(LPARAM)(LPSTR) CurrentParam.parameter[i].name );
        }
    }
}

/***********************************************************************
*                           HANDLE_SELECTION                           *
***********************************************************************/
static void handle_selection( int box )
{
int i;
int otherbox;
LISTBOX_CLASS lb;
STRING_CLASS  s;

otherbox = AVAILABLE_PARAMETER_LISTBOX;
if ( box == otherbox )
    otherbox = CORNER_PARAMETER_LISTBOX;

lb.init( CornerParamDialogWindow, box );
i = lb.selected_index();
if ( i != NO_INDEX )
    {
    s = lb.item_text(i);
    lb.remove( i );
    if ( !s.isempty() )
    {
        lb.init( CornerParamDialogWindow, otherbox );
        lb.add( s.text() );
        }
    }
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes()
{
TCHAR s[PARAMETER_NAME_LEN+1];
LRESULT status;
short parameter_number;
short i;
short j;
short old_count;

old_count = NofCornerParameters;
NofCornerParameters = 0;
for ( j=0; j<MAX_PARMS; j++ )
    {
    parameter_number = NO_PARAMETER_NUMBER;
    status = SendDlgItemMessage( CornerParamDialogWindow, CORNER_PARAMETER_LISTBOX, LB_GETTEXT, j, (LPARAM)(LPSTR) s );
    if ( status != LB_ERR )
        {
        for ( i=0; i<MAX_PARMS; i++ )
            {
            if ( strings_are_equal(s, CurrentParam.parameter[i].name, PARAMETER_NAME_LEN) )
                {
                parameter_number = i+1;
                break;
                }
            }
        }
    CornerParameters[j] = parameter_number;
    if (  parameter_number != NO_PARAMETER_NUMBER )
        NofCornerParameters = j+1;
    }

save_corner_parameters( CornerParameters, NofCornerParameters );
if ( NofCornerParameters != old_count )
    {
    position_parameter_window();
    position_main_plot_window();
    if ( MyScreenType == ZABISH_SCREEN_TYPE )
        position_param_history_window();
    }
}

/***********************************************************************
*                         SHUT_DOWN_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK shut_down_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
UINT default_button;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        default_button = SHUT_DOWN_PLOT_RADIO;
        if ( boolean_from_ini(DisplayIniFile, ConfigSection, ExitAllDefaultKey) )
            default_button = SHUT_DOWN_ALL_RADIO;
        CheckRadioButton( hWnd, SHUT_DOWN_PLOT_RADIO, SHUT_DOWN_ALL_RADIO, default_button );
        SetFocus( GetDlgItem(hWnd, IDOK) );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( is_checked(hWnd, SHUT_DOWN_ALL_RADIO) )
                    {
                    /*
                    -----------------------------------------------------------------------
                    Tell eventman to shut down. This will shut down everyone, including me.
                    ----------------------------------------------------------------------- */
                    poke_data( DDE_EXIT_TOPIC,  ItemList[NEW_MACH_STATE_INDEX].name, UNKNOWN );
                    }
                else
                    {
                    PostMessage( MainPlotWindow, WM_DESTROY, 0, 0L );
                    PostMessage( MainWindow,     WM_CLOSE,   0, 0L );
                    }

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                     POSITION_CORNER_PARAM_DIALOG                     *
***********************************************************************/
static void position_corner_param_dialog()
{
WINDOW_CLASS    w;
RECTANGLE_CLASS dr;
RECTANGLE_CLASS r;
int             i;
POINT           p;

if ( !CornerParamDialogWindow )
    return;

GetCursorPos( &p );

w = CornerParamDialogWindow;
w.getrect( r );

p.x -= r.width()/2;
p.y -= r.height()/2;

dr = my_desktop_client_rect();
if ( p.x < 0 )
    p.x = 0;

if ( p.y < 0 )
    p.y = 0;

i = dr.right - r.width();
if ( p.x > i )
    p.x = i;

i = dr.bottom - r.height();
if ( p.y > i )
    p.y = i;

w.move( p.x, p.y );
}

/***********************************************************************
*                       CORNER_PARAM_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK corner_param_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CornerParamDialogWindow = hWnd;
        position_corner_param_dialog();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_corner_param_listbox();
        SetFocus( CornerParamDialogWindow );
        return TRUE;

    case WM_COMMAND:
        if ( cmd == LBN_SELCHANGE )
            {
            handle_selection( id );
            return TRUE;
            }

        switch ( id )
            {
            case IDOK:
                save_changes();
                show_corner_parameters();
                if ( MyScreenType == BIG_PARAM_SCREEN_TYPE )
                    InvalidateRect( MainPlotWindow, 0, TRUE );
                else
                    InvalidateRect( ParamHistoryWindow, 0, TRUE );

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, CORNER_PARAMS_MENU_HELP );
        return TRUE;

    case WM_DESTROY:
        CornerParamDialogWindow = 0;
        break;
    }

return FALSE;
}

/***********************************************************************
*                       GET_NEW_DOWNTIME_MACHINE                       *
***********************************************************************/
static void get_new_downtime_machine( LISTBOX_CLASS & lb )
{
STRING_CLASS s;
s = lb.selected_text();
if ( !s.isempty() )
    {
    if ( replace_tab_with_null(s.text()) )
        {
        if ( MachList.find(s.text()) )
            {
            copystring( DowntimeComputerName, MachList.computer_name() );
            copystring( DowntimeMachineName,  MachList.name() );
            SendMessage( MainWindow, WM_NEWPART, 0, 0L );
            }
        }
    }
}

/***********************************************************************
*                 FILL_CHOOSE_DOWNTIME_MACHINE_LISTBOX                 *
***********************************************************************/
static BOOLEAN fill_choose_downtime_machine_listbox( LISTBOX_CLASS & lb )
{
int          count;
STRING_CLASS s;
DB_TABLE     t;
int          last_record;
TIME_CLASS   tc;

count = 0;

MachList.rewind();
while ( MachList.next() )
    {
    s = downtime_dbname( MachList.computer_name(), MachList.name() );
    if ( file_exists(s.text()) )
        {
        if ( t.open(s.text(), DOWNTIME_RECLEN, FL) )
            {
            last_record = (int) t.nof_recs();
            last_record--;

            if ( last_record > 0 )
                {
                if ( t.goto_record_number(last_record) )
                    {
                    if ( t.get_rec() )
                        {
                        if ( get_time_class_from_db(tc, t, DOWNTIME_DATE_OFFSET) )
                            {
                            s = MachList.name();
                            s += TabString;
                            s += tc.text();
                            lb.add( s.text() );
                            count++;
                            }
                        }
                    }
                }
            t.close();
            }
        }
    }

if ( count )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                CHOOSE_DOWNTIME_MACHINE_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK choose_downtime_machine_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int title_ids[] = {
    DOWNTIME_MACHINE_DATE_TBOX
    };
const int nof_title_ids = sizeof(title_ids)/sizeof(int);

static LISTBOX_CLASS lb;

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_listbox_tabs_from_title_positions( hWnd, DOWNTIME_MACHINE_LISTBOX, title_ids, nof_title_ids );
        lb.init( hWnd, DOWNTIME_MACHINE_LISTBOX );
        break;

    case WM_DBINIT:
        if ( fill_choose_downtime_machine_listbox(lb) )
            ShowWindow( hWnd, SW_SHOW );
        else
            resource_message_box( MainInstance, NO_DOWNTIME_DATA_STRING, NOTHING_TO_DISPLAY_STRING );
        break;

    case WM_DBCLOSE:
        lb.empty();
        ShowWindow( hWnd, SW_HIDE );
        break;

    case WM_CLOSE:
        SendMessage( hWnd, WM_DBCLOSE, 0, 0L );
        return TRUE;

    case WM_COMMAND:
        if ( cmd == LBN_SELCHANGE )
            {
            get_new_downtime_machine(lb);
            return TRUE;
            }

        switch ( id )
            {
            case IDOK:
            case IDCANCEL:
                SendMessage( hWnd, WM_DBCLOSE, 0, 0L );
                return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                       SAVE_SIGNED_ON_OPERATOR                        *
***********************************************************************/
static void save_signed_on_operator( HWND w )
{
BOOLEAN        have_operator;
COMPUTER_CLASS c;
LISTBOX_CLASS  lb;
DB_TABLE       t;
int            i;
int            n;
TCHAR        * cp;
TCHAR          operator_number[OPERATOR_NUMBER_LEN + 1];

have_operator = FALSE;
lb.init( w, OPERATOR_LISTBOX );
i = lb.selected_index();
if ( i != NO_INDEX )
    {
    /*
    ----------------------------------------------
    Find this operator in the operator table. This
    is possible because the listbox is not sorted
    ---------------------------------------------- */
    t.open( operator_dbname(), OPERATOR_RECLEN, PFL );
    if ( t.goto_record_number(i) )
        {
        if ( t.get_rec() )
            {
            have_operator = t.get_alpha( operator_number, OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
            }
        }
    }
t.close();

if ( !have_operator )
    {
    cp = lb.selected_text();
    MessageBox( NULL, cp, resource_string(MainInstance, OPERATOR_NOT_FOUND), MB_OK );
    return;
    }

/*
----------------------------------------------
Put this operator number into all the machines
---------------------------------------------- */
lb.init( w, OPERATOR_MACHINE_LISTBOX );
n = lb.count();

if ( n <= 0 )
    return;

t.open( machset_dbname(c.whoami()), MACHSET_RECLEN, PWL );
for ( i=0; i<n; i++ )
    {
    if ( lb.is_selected(i) )
        {
        cp = lb.item_text( i );
        t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, cp, MACHINE_NAME_LEN );
        t.reset_search_mode();
        if ( t.get_next_key_match(1, WITH_LOCK) )
            {
            t.put_alpha( MACHSET_OPERATOR_NUMBER_OFFSET, operator_number, OPERATOR_NUMBER_LEN );
            t.rec_update();
            t.unlock_record();
            }
        }
    }
t.close();
}

/***********************************************************************
*                    FILL_OPERATOR_SIGNON_LISTBOXES                    *
***********************************************************************/
static void fill_operator_signon_listboxes( HWND w )
{
COMPUTER_CLASS c;
LISTBOX_CLASS  lb;
DB_TABLE       t;
TCHAR          s[OPERATOR_FIRST_NAME_LEN + OPERATOR_LAST_NAME_LEN + 1];

lb.init( w, OPERATOR_MACHINE_LISTBOX );
lb.empty();

MachList.rewind();
while ( MachList.next() )
    {
    if ( lstrcmp(MachList.computer_name(), c.whoami()) == 0 )
        {
        lb.add( MachList.name() );
        }
    }

if ( lb.count() < 1 )
    resource_message_box( MainInstance, NO_MACHINES_HERE, EMPTY_LIST );
else
    lb.select_all();

lb.init( w, OPERATOR_LISTBOX );
lb.empty();

t.open( operator_dbname(), OPERATOR_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( s, OPERATOR_FIRST_NAME_OFFSET, OPERATOR_FIRST_NAME_LEN );
    lstrcat( s, TEXT(" ") );
    t.get_alpha( s + lstrlen(s), OPERATOR_LAST_NAME_OFFSET,  OPERATOR_LAST_NAME_LEN );
    lb.add( s );
    }
t.close();
}

/***********************************************************************
*                          ENABLE_EDIT_BUTTON                          *
***********************************************************************/
static void enable_edit_button( HWND w )
{
TCHAR * cp;
int32   level_needed;

cp = get_ini_string( VisiTrakIniFile, ButtonLevelsSection, EditOperatorsButtonKey );
if ( !unknown(cp) )
    {
    level_needed = asctoint32( cp );
    if ( CurrentPasswordLevel >= level_needed )
        EnableWindow( GetDlgItem(w,EDIT_OPERATORS_BUTTON), TRUE );
    }
}

/***********************************************************************
*                         OPERATOR_SIGNON_PROC                         *
***********************************************************************/
BOOL CALLBACK operator_signon_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
LISTBOX_CLASS lb;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        enable_edit_button( hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_operator_signon_listboxes(hWnd);
        SetFocus( GetDlgItem(hWnd,OPERATOR_LISTBOX) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case ALL_OPERATOR_MACHINES_BUTTON:
                lb.init( hWnd, OPERATOR_MACHINE_LISTBOX );
                lb.select_all();
                return TRUE;

            case EDIT_OPERATORS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("EDIT_OPERATORS_DIALOG"),
                    hWnd, /*        MainPlotWindow, */
                    (DLGPROC) edit_operators_dialog_proc );
                fill_operator_signon_listboxes(hWnd);
                return TRUE;

            case IDOK:
                save_signed_on_operator( hWnd );

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            DO_POPUP_MENU                             *
***********************************************************************/
static void do_popup_menu( LPARAM lParam )
{
HMENU m;
POINT p;
UINT  menu_id;
UINT  rid;

if ( DialogIsActive )
    return;

turn_off_zooming();

m = CreatePopupMenu();
if ( !m )
    return;

p.x = LOWORD( lParam );
p.y = HIWORD( lParam );

ClientToScreen( MainPlotWindow, (LPPOINT) &p );

if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
    {
    AppendMenu( m, MF_STRING, CHOOSE_DOWNTIME_MACHINE_MENU, resource_string(CHOOSE_DOWNTIME_MACHINE_MENU_STRING) );
    AppendMenu( m, MF_STRING, DOWNBAR_CONFIG_MENU, resource_string(DOWNBAR_CONFIG_MENU_STRING) );
    }

if ( MyScreenType == TREND_SCREEN_TYPE )
    {
    if ( !RealTime && !GridIsVisible )
        AppendMenu( m, MF_STRING, TREND_REALTIME_MENU, resource_string(TREND_REALTIME_MENU_STRING) );

    AppendMenu( m, MF_STRING, TREND_ALLSHOTS_MENU, resource_string(TREND_ALLSHOTS_MENU_STRING) );
    AppendMenu( m, MF_STRING, TREND_CONFIG_MENU,   resource_string(TREND_CONFIG_MENU_STRING) );
    if ( GridIsVisible )
        {
        menu_id = NO_STAT_SUMMARY_MENU;
        rid = NO_STAT_SUMMARY_MENU_STRING;
        }
    else
        {
        menu_id = STAT_SUMMARY_MENU;
        rid = STAT_SUMMARY_MENU_STRING;
        }
    AppendMenu( m, MF_STRING, menu_id, resource_string(rid) );
    }

if ( MyScreenType == STANDARD_SCREEN_TYPE || MyScreenType == ZABISH_SCREEN_TYPE )
    {
    if ( zoomed() )
        AppendMenu( m, MF_STRING, UNZOOM_MENU, resource_string(UNZOOM_MENU) );
    else
        AppendMenu( m, MF_STRING, ZOOM_MENU,   resource_string(ZOOM_MENU) );

    AppendMenu( m, MF_STRING, TREND_SCREEN_MENU, resource_string(TREND_SCREEN_MENU_STRING) );
    AppendMenu( m, MF_STRING, BOX_SCREEN_MENU,   resource_string(BOX_SCREEN_MENU_STRING)   );
    AppendMenu( m, MF_STRING, STATUS_SCREEN_MENU, resource_string(STATUS_SCREEN_MENU_STRING)   );

    if ( InfoBoxType != XDCR_INFO_BOX && !zoomed() )
        AppendMenu( m, MF_STRING, XDCR_MENU,    resource_string(XDCR_MENU_STRING) );
    else if ( InfoBoxType == XDCR_INFO_BOX )
        AppendMenu( m, MF_STRING, PROFILE_SCREEN_MENU, resource_string(STD_SCREEN_MENU) );

    if ( CurrentDisplay != POS_TIME_DISPLAY && !zoomed() )
        AppendMenu( m, MF_STRING, POS_TIME_MENU,       resource_string(POS_TIME_MENU) );

    if ( CurrentDisplay != POS_ONLY_DISPLAY && !IsHPM ) // && !zoomed() )
        AppendMenu( m, MF_STRING, POS_ONLY_MENU,       resource_string(POS_ONLY_MENU) );

    if ( CurrentDisplay != TIME_ONLY_DISPLAY )
        AppendMenu( m, MF_STRING, TIME_ONLY_MENU,      resource_string(TIME_ONLY_MENU) );
    }
else
    {
    AppendMenu( m, MF_STRING, PROFILE_SCREEN_MENU, resource_string(STD_SCREEN_MENU) );
    }

if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
    {
    AppendMenu( m, MF_STRING, STATUS_SCREEN_MENU, resource_string(STATUS_SCREEN_MENU_STRING)   );
    }

if ( MyScreenType == STATUS_SCREEN_TYPE && CurrentPasswordLevel >= EditStatusScreenLevel )
    {
    if ( Editing_Status_Screen )
        {
        AppendMenu( m, MF_STRING, FONT_STATUS_SCREEN_MENU, resource_string(FONT_STATUS_SCREEN_MENU_STRING) );
        AppendMenu( m, MF_STRING, SAVE_STATUS_SCREEN_MENU, resource_string(SAVE_STATUS_SCREEN_MENU_STRING) );
        }
    else
        {
        AppendMenu( m, MF_STRING, EDIT_STATUS_SCREEN_MENU, resource_string(EDIT_STATUS_SCREEN_MENU_STRING) );
        }
    }

TrackPopupMenu( m, TPM_LEFTALIGN | TPM_LEFTBUTTON, p.x, p.y, 0, MainWindow, 0 );

DestroyMenu( m );
}

/***********************************************************************
*                            DO_ZOOM_BUTTON                            *
***********************************************************************/
static void do_zoom_button( POINT p )
{
if ( p.x < Region[POS_BASED_ANALOG].rect.left || p.x > Region[TIME_BASED_ANALOG].rect.right )
    return;

if ( !Zooming )
    {
    if ( !zoomed() )
        {
        WasRealtimeBeforeZoom      = RealTime;
        WasSingleMachineBeforeZoom = SingleMachineOnly;
        DisplayBeforeZoom          = CurrentDisplay;

        if ( RealTime )
            {
            RealTime = FALSE;
            SingleMachineOnly = FALSE;
            set_realtime_menu_string();
            }
        }

    if ( HaveHeader && ReadyForZoom )
        {
        Zooming = TRUE;
        CursorStartY = Region[POS_BASED_ANALOG].rect.top;
        CursorEndY   = Region[POS_BASED_COUNT].rect.bottom;

        CursorX = p.x;
        draw_cursor_line( CursorX );
        HaveCursorLine = TRUE;

        set_info_box_type( ZOOM_INFO_BOX );
        show_zoom_info();
        }
    }
else if ( !HaveLeftCursorLine )
    {
    LeftCursorX = p.x;
    draw_cursor_line( LeftCursorX );
    HaveLeftCursorLine = TRUE;
    }
else
    {
    Zooming = FALSE;
    zoom( p );
    }
SetFocus( MainWindow );
}

/***********************************************************************
*                       DO_KEYBOARD_ZOOM_BUTTON                        *
***********************************************************************/
static void do_keyboard_zoom_button()
{
POINT p;

if ( !Zooming )
    p.x = Region[POS_BASED_ANALOG].rect.left + 2;
else
    p.x = CursorX;

p.y = Region[POS_BASED_ANALOG].rect.bottom + Region[POS_BASED_ANALOG].rect.top;
p.y /= 2;
do_zoom_button( p );
}

/***********************************************************************
*                     TOGGLE_CURRENT_DISPLAY_TYPE                      *
***********************************************************************/
static void toggle_current_display_type()
{
int new_display_type;

if ( MyScreenType == TREND_SCREEN_TYPE )
    {
    if ( GridIsVisible )
        GridIsVisible = FALSE;
    else
        GridIsVisible = TRUE;
    }
else
    {
    if ( CurrentDisplay == POS_ONLY_DISPLAY || CurrentDisplay == POS_TIME_DISPLAY )
        {
        LastPosDisplay   = CurrentDisplay;
        new_display_type = TIME_ONLY_DISPLAY;
        }
    else
        {
        new_display_type = LastPosDisplay;
        }

    clear_y_axes();
    set_current_display( new_display_type );
    }

InvalidateRect( MainPlotWindow, 0, TRUE );
}

/***********************************************************************
*                         PAINT_PLOT_BACKGROUND                        *
***********************************************************************/
void paint_plot_background( HWND w, HDC dc )
{
HBRUSH oldbrush;
HBRUSH mybrush;
RECT r;

GetClientRect(w, &r);

mybrush  = (HBRUSH) CreateSolidBrush( CurrentColor[BACKGROUND_COLOR] );
oldbrush = (HBRUSH) SelectObject( dc, mybrush );

FillRect( dc, &r, mybrush );

SelectObject( dc, oldbrush );
DeleteObject( mybrush );
}

/***********************************************************************
*                          TOGGLE_REALTIME                             *
***********************************************************************/
void toggle_realtime()
{
if ( RealTime )
    {
    if ( !SingleMachineOnly || (MyScreenType == TREND_SCREEN_TYPE) )
        RealTime = FALSE;

    SingleMachineOnly = FALSE;
    }
else if ( MyScreenType == TREND_SCREEN_TYPE )
    {
    trend_realtime();
    return;
    }
else
    {
    RealTime = TRUE;
    }

set_realtime_menu_string();
}

/***********************************************************************
*                          SHOW_LAST_SCREEN_TYPE                       *
*                                                                      *
* This lets the program restore whatever was showing the last time     *
* the program was run.                                                 *
*                                                                      *
***********************************************************************/
static void show_last_screen_type()
{
TCHAR * cp;
short   i;

cp = get_ini_string( DisplayIniFile, ConfigSection, TEXT("LastScreenType") );
if ( !unknown(cp) )
    {
    i = (short) asctoint32( cp );
    if ( MyScreenType != i )
        update_screen_type( i );
    }
}

/***********************************************************************
*                       DO_VIEW_PARAMETERS_MENU                        *
***********************************************************************/
void do_view_parameters_menu()
{
turn_off_zooming();
toggle_menu_view_state( ViewParametersDisplay, VIEW_PARAMETERS_MENU );

if ( ViewParametersDisplay )
    {
    ShowWindow( ViewParametersDialogWindow, SW_SHOW );
    fill_view_parameters_listbox();
    }
else
    ShowWindow( ViewParametersDialogWindow, SW_HIDE );
}

/***********************************************************************
*                                SIZE_GRID                             *
***********************************************************************/
static void size_grid()
{
RECT r;

GetClientRect( MainPlotWindow, &r );
r.top += r.bottom;
r.top /= 2;

SetWindowPos( Grid.m_ctrlWnd, 0, r.left, r.top, r.right-r.left, r.bottom-r.top, SWP_NOZORDER );
}

/***********************************************************************
*                              CREATE_GRID                             *
***********************************************************************/
static void create_grid()
{
CUGCell cell;
RECT    r;
int32   col;
int32   i;
UINT    id;

GetClientRect( MainPlotWindow, &r );
r.top += r.bottom;
r.top /= 2;

Grid.CreateGrid( WS_CHILD|WS_VISIBLE, r, MainPlotWindow, 104 );
Grid.GetCell( 0,0,&cell );
Grid.SetNumberCols( NofColumns, FALSE );

i = Grid.GetRowHeight( 0 );
i *= 2;
Grid.SetTH_NumberRows( 2 );
Grid.SetTH_Height( i );

cell.SetText( resource_string(ALARM_LIMITS_STRING) );
Grid.SetCell( LOW_ALARM_COLUMN_NUMBER, -2, &cell );

Grid.JoinCells( LOW_ALARM_COLUMN_NUMBER, -2, HIGH_ALARM_COLUMN_NUMBER, -2 );

Grid.SetSH_Width( 150 );

id = FIRST_STATSUM_STRING;
for ( col=0; col<NOF_STATSUM_STRINGS; col++ )
    {
    cell.SetText( resource_string(id) );
    Grid.SetCell( col, -1, &cell );
    id++;
    }

GridIsVisible = FALSE;
ShowWindow( Grid.m_ctrlWnd, SW_HIDE );
}

/***********************************************************************
*                            GOTO_NEXT_SHOT                            *
*                Previous shot if direction is < 0.                    *
***********************************************************************/
static void goto_next_shot( int direction )
{
int32    new_record_number;
int32    record_number;
int32    shot_number;
int32    last_record;
NAME_CLASS s;
DB_TABLE t;
BOOLEAN  have_shot;

have_shot = FALSE;
s.get_graphlst_dbname( ComputerName, MachineName, PartName );
if ( s.file_exists() )
    {
    if ( t.open(s.text(), GRAPHLST_RECLEN, PFL) )
        {
        t.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, asctoint32(ShotName), SHOT_LEN );
        if ( t.get_next_key_match( 1, NO_LOCK ) )
            {
            last_record = (int32) t.nof_recs();
            last_record--;
            record_number = t.current_record_number();
            new_record_number = record_number + direction;
            if ( new_record_number < 0 )
                new_record_number = 0;
            else if ( new_record_number > last_record )
                new_record_number = last_record;

            if ( new_record_number != record_number )
                {
                if ( t.goto_record_number( new_record_number) )
                    {
                    if ( t.get_current_record(NO_LOCK) )
                        {
                        shot_number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
                        insalph( ShotName, shot_number, SHOT_LEN, TEXT('0'), DECIMAL_RADIX );
                        *(ShotName+SHOT_LEN) = NullChar;
                        have_shot = TRUE;
                        }
                    }
                }
            }
        t.close();
        }
    }

if ( have_shot )
    SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
}

/***********************************************************************
*                            DO_ZOOM_ARROW_KEYS                        *
***********************************************************************/
BOOLEAN do_zoom_arrow_keys( int key )
{
static int direction;
POINT  p;

if ( !Zooming && !zoomed() )
    {
    /*
    ------------------------------------------------
    See if the operator wants to go to the next shot
    ------------------------------------------------ */
    if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
        {
        direction = 0;
        if ( key == VK_RIGHT )
            direction = 1;
        else if ( key == VK_LEFT )
            direction = -1;
        else if ( key == VK_UP )
            direction = -10;
        else if ( key == VK_DOWN )
            direction = 10;
        if ( direction )
            {
            if ( InfoBoxType == XDCR_INFO_BOX )
                XdcrWindow.post( WM_NEW_DATA, 0, (LPARAM) direction );
            else
                goto_next_shot( direction );
            }
        }
    return FALSE;
    }

if ( key == VK_SPACE || key == VK_RETURN )
    {
    do_keyboard_zoom_button();
    return TRUE;
    }

if ( !Zooming )
    {
    if ( key == VK_LEFT || key == VK_UP )
        p.x = Region[TIME_BASED_ANALOG].rect.right - 2;
    else if ( key == VK_RIGHT || key == VK_DOWN )
        p.x = Region[POS_BASED_ANALOG].rect.left + 2;
    else
        return FALSE;

    p.y = Region[POS_BASED_ANALOG].rect.bottom + Region[POS_BASED_ANALOG].rect.top;
    p.y /= 2;
    do_zoom_button( p );
    return TRUE;
    }

p.x = CursorX;

if ( GetKeyState(VK_SHIFT) < 0 )
    {
    if ( key == VK_LEFT )
        key = VK_UP;
    else if ( key == VK_RIGHT )
        key = VK_DOWN;
    }

switch ( key )
    {
    case VK_DOWN:
         p.x += 10;
        break;

    case VK_RIGHT:
        p.x++;
        break;

    case VK_UP:
        p.x -= 10;
        break;

    case VK_LEFT:
        p.x--;
        break;

    default:
        return FALSE;
    }

if ( p.x < Region[POS_BASED_ANALOG].rect.left || p.x > Region[TIME_BASED_ANALOG].rect.right )
    return 0;

erase_cursor_line();
CursorX = p.x;
if ( !HaveLeftCursorLine )
    LeftCursorX = p.x;
draw_cursor_line( CursorX );
HaveCursorLine = TRUE;
show_zoom_info();
return TRUE;
}

/***********************************************************************
*                          ADD_TO_BACKUP_LIST                          *
***********************************************************************/
void add_to_backup_list()
{
DB_TABLE t;
STRING_CLASS s;
BOOLEAN      need_to_append;

center_temp_message();
temp_message( resource_string(ADDING_TO_BACKUP_STRING) );


need_to_append = FALSE;
s = backup_list_dbname();
if ( !file_exists(s.text()) )
    {
    t.create( s.text() );
    need_to_append = TRUE;
    }

if ( t.open(s.text(), BACKUP_LIST_RECLEN, FL) )
    {
    t.put_alpha( BACKUP_LIST_CN_OFFSET, ComputerName, COMPUTER_NAME_LEN );
    t.put_alpha( BACKUP_LIST_MN_OFFSET, MachineName,  MACHINE_NAME_LEN );
    t.put_alpha( BACKUP_LIST_PN_OFFSET, PartName,     PART_NAME_LEN );
    t.put_alpha( BACKUP_LIST_SN_OFFSET, ShotName,     SHOT_LEN );

    if ( !need_to_append )
        {
        /*
        -----------------------------------------------------------
        If there is already a record that matches this, do nothing.
        ----------------------------------------------------------- */
        if ( !t.get_next_key_match(4, FALSE) )
            {
            if ( t.goto_first_equal_or_greater_record(4) )
                {
                t.rec_insert();
                }
            else
                {
                need_to_append = TRUE;
                }
            }
        }

    if ( need_to_append )
        {
        t.rec_append();
        }

    t.close();
    }
}

/***********************************************************************
*                             DO_XDCR_CLICK                            *
*   If I click on the screen send the x4 position to XdcrWindow        *
***********************************************************************/
static void do_xdcr_click( POINT p )
{
float x;
double d;
int   axis_index;
WPARAM x4_value;

if ( x_value( x, axis_index, p.x ) )
    {
    if ( axis_index == X_POS_AXIS )
        {
        d = (double) x;
        d = CurrentPart.x4_from_dist( d );
        d = round( d, 1.0 );
        x4_value = (WPARAM) d;
        XdcrWindow.post( WM_NEW_DATA, d, 0 );
        }
    }
}

/***********************************************************************
*                            MAIN_PLOT_PROC                            *
***********************************************************************/
LRESULT CALLBACK main_plot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOOLEAN first_paint = TRUE;
POINT   p;
int     id;
LPNMHDR nm;
MSG     mymsg;

id  = LOWORD( wParam );

if ( MyScreenType == BOX_SCREEN_TYPE && ToolTipWindow )
    {
    switch ( msg )
        {
        case WM_LBUTTONDOWN:
        case WM_MOUSEMOVE:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            mymsg.lParam  = lParam;
            mymsg.wParam  = wParam;
            mymsg.message = msg;
            mymsg.hwnd    = hWnd;
            SendMessage( ToolTipWindow, TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &mymsg );
            break;
        }
    }

switch ( msg )
    {
    case WM_TIMER:
        if ( MyScreenType == STATUS_SCREEN_TYPE )
            {
            KillTimer( hWnd, StatusScreenTimerId );
            StatusScreenTimerId = 0;
            status_screen_timer_update();
            }
        return 0;

    case WM_NOTIFY:
        nm = (LPNMHDR) lParam;
        if ( nm->code == HDN_ENDTRACK && MyScreenType == STATUS_SCREEN_TYPE )
            {
            /*
            ----------------------------------------------------------------------
            The status screen needs to be redisplayed to show the new column width
            ---------------------------------------------------------------------- */
            NeedNewStatusScreenColumnWidths = TRUE;
            InvalidateRect( MainPlotWindow, 0, TRUE );
            }
        break;

    case WM_COMMAND:

        if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
            {
            if ( id == ESCAPE_KEY || (id >= FirstDownCatButton && id <= LastDownCatButton) )
                {
                do_downbar_button( id );
                return 0;
                }
            }

        switch ( id )
            {
            case SPACEBAR_KEY:
                if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
                    do_keyboard_zoom_button();
                return 0;

            case ESCAPE_KEY:
                if ( Zooming )
                    {
                    turn_off_zooming();
                    set_info_box_type( PARAMETER_INFO_BOX );
                    }
                else if ( MyScreenType == TREND_SCREEN_TYPE )
                    trend_escape();
                return 0;

            case F5_KEY:
                toggle_current_display_type();
                return 0;

            case F6_KEY:
                toggle_big_param_display_type();
                return 0;

            case F7_KEY:
                toggle_pos_time_display_type();
                return 0;

            case F8_KEY:
                if ( MyScreenType == TREND_SCREEN_TYPE )
                    trend_allshots();
                return 0;

            case F9_KEY:
                add_to_backup_list();
                return 0;

            case F10_KEY:
                restore_focus_to_hmi();
                return 0;

            case REALTIME_TOGGLE_KEY:
                if ( (MyScreenType != BOX_SCREEN_TYPE) && (MyScreenType != STATUS_SCREEN_TYPE) )
                    toggle_realtime();
                return 0;

            case VIEW_PARAMETERS_KEY:
                do_view_parameters_menu();
                return 0;

            }
        break;

    case WM_SIZE:
        size_plot_window_to_screen();
        size_grid();
        break;

    case WM_CREATE:
        MainPlotWindow = hWnd;
        create_grid();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:

        /*
        ------------------------------------------------
        Set the plot window to the bottom of the z order
        ------------------------------------------------ */
        SetWindowPos( hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        box_display_startup();

        ToolTipWindow = CreateWindowEx(
            0,
            TOOLTIPS_CLASS,     /* Class name */
            0,                  /* Title */
            TTS_ALWAYSTIP,      /* Style */
            CW_USEDEFAULT,      /* x */
            CW_USEDEFAULT,      /* x */
            CW_USEDEFAULT,      /* x */
            CW_USEDEFAULT,      /* x */
            MainPlotWindow,
            0,
            MainInstance,
            0
            );

        set_realtime_menu_string();
        return 0;

    case WM_SETFOCUS:
        if ( ChosshotWindow )
            SetFocus( ChosshotWindow );
        else if ( !GridIsVisible && !Editing_Status_Screen )
            SetFocus( MainWindow );
        ReadyForZoom = TRUE;
        return 0;

    case WM_ERASEBKGND:
        paint_plot_background( hWnd, (HDC) wParam );
        return 1;  /* 1 tells windows I have erased the window */

    case WM_PAINT:
        paint_me();
        if ( NeedCornerParamUpdate )
            {
            NeedCornerParamUpdate = FALSE;
            show_corner_parameters();
            if ( ViewParametersDisplay )
                fill_view_parameters_listbox();
            }
        if ( first_paint )
            {
            first_paint = FALSE;
            show_last_screen_type();
            }

        if ( PrintingMultiplePlots && MultipleShotDialogWindow )
            {
            print_me();
            PostMessage( MultipleShotDialogWindow, WM_NEWSHOT, 0, 0L );
            }
        return 0;

    case WM_RBUTTONUP:
        do_popup_menu(lParam);
        return 0;

    case WM_MOUSEMOVE:
        if ( MyScreenType == TREND_SCREEN_TYPE )
            {
            mouse_to_trend( (int32) LOWORD(lParam), (int32) HIWORD(lParam) );
            }
        else if ( HaveHeader && ReadyForZoom )
            {
            if ( Zooming )
                {
                p.x = (LONG) LOWORD(lParam);
                if ( Region[POS_BASED_ANALOG].rect.left <= p.x && p.x <= Region[TIME_BASED_ANALOG].rect.right )
                    {
                    erase_cursor_line();
                    CursorX = p.x;
                    if ( !HaveLeftCursorLine )
                        LeftCursorX = p.x;
                    draw_cursor_line( CursorX );
                    HaveCursorLine = TRUE;
                    show_zoom_info();
                    }
                }
            else
                {
                if ( HaveCursorLine )
                    erase_cursor_line();

                if ( HaveLeftCursorLine )
                    erase_left_cursor_line();
                }
            }
        return 0;

    case WM_LBUTTONUP:
        p.x = (LONG) ( LOWORD(lParam) );
        p.y = (LONG) ( HIWORD(lParam) );

        if ( MyScreenType == TREND_SCREEN_TYPE )
            {
            if ( PtInRect(&RealTimeButtonRect, p) )
                toggle_realtime();
            else
                trend_left_button_up( p );
            }
        else if ( MyScreenType == BOX_SCREEN_TYPE )
            {
            box_screen_left_button_up( p );
            }
        else if ( MyScreenType == STATUS_SCREEN_TYPE )
            {
            status_screen_left_button_up( p );
            }
        else if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
            {
            downbar_left_button_up( p );
            }
        else if ( !Zooming && PtInRect(&RealTimeButtonRect, p) )
            {
            toggle_realtime();
            }
        else if ( Zooming || zoomed() )
            {
            do_zoom_button( p );
            return 0;
            }
        else if ( InfoBoxType == XDCR_INFO_BOX )
            {
            do_xdcr_click( p );
            return 0;
            }

        break;

    case WM_KEYDOWN:
        if ( do_zoom_arrow_keys((int) wParam) )
            return 0;
        break;

    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}


/***********************************************************************
*                     REGISTER_FOR_CURRENT_MACHINE                       *
***********************************************************************/
static void register_for_current_machine()
{
register_for_event( MachList.name(), SHOT_NUMBER_INDEX, new_shot_callback );
register_for_event( MachList.name(), PART_SETUP_INDEX,  new_part_setup_callback );
}

/***********************************************************************
*                   UNREGISTER_FOR_CURRENT_MACHINE                     *
***********************************************************************/
static void unregister_for_current_machine()
{
unregister_for_event( MachList.name(), SHOT_NUMBER_INDEX );
unregister_for_event( MachList.name(), PART_SETUP_INDEX  );
}

/***********************************************************************
*                   UNREGISTER_FOR_NEW_SHOT_EVENTS                     *
***********************************************************************/
static void unregister_for_new_shot_events()
{
MachList.rewind();
while ( MachList.next() )
    unregister_for_current_machine();
}

/***********************************************************************
*                             DO_NEWSETUP                              *
***********************************************************************/
static void do_newsetup()
{
COMPUTER_CLASS c;
STRING_CLASS   s;

if ( MachList.find(NewSetupMachine) )
    {
    /*
    ------------------------------------
    See if this machine has been deleted
    ------------------------------------ */
    if ( !machine_exists(MachList.computer_name(), MachList.name()) )
        {
        unregister_for_current_machine();
        MachList.remove();
        }
    else if ( strings_are_equal(MachList.name(), CurrentMachine.name) )
        {
        CurrentMachine.find( ComputerName, MachineName );
        if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
            SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
        }
    }
else
    {
    /*
    ---------------------
    This is a new machine
    --------------------- */
    s = machine_computer_name( NewSetupMachine );
    if ( !s.isempty() )
        {
        MachList.add( s.text(), NewSetupMachine, SORT_BY_MACHINE );
        if ( MachList.find(NewSetupMachine) )
            register_for_current_machine();

        /*
        ---------------------------------------------------------------------
        Add this to the list of machines to monitor if it is on this computer
        --------------------------------------------------------------------- */
        if ( s == c.whoami() )
            {
            if ( !RealtimeMachineList.find(NewSetupMachine) )
                {
                if ( RealtimeMachineList.append(NewSetupMachine) )
                    {
                    RealtimeMachineList.save();
                    begin_box_display();
                    }
                }
            }
        }
    }
}

/***********************************************************************
*                           DOWN_DATA_CALLBACK                         *
***********************************************************************/
void down_data_callback( TCHAR * topic, short item, HDDEDATA edata )
{
DOWN_DATA_CLASS dd;
COMPUTER_CLASS  c;
TCHAR           s[COMPUTER_NAME_LEN+1];

if ( dd.set(edata) )
    {
    /*
    ----------------------------------------------
    If the computer was offline set it back online
    ---------------------------------------------- */
    if ( MachList.get_computer_name(s, dd.machine_name()) )
        c.set_online_status( s, TRUE );
    if ( MyScreenType == BOX_SCREEN_TYPE )
        box_screen_down_data_change( dd.machine_name(), dd.category(), dd.subcategory() );

    else if ( MyScreenType == STATUS_SCREEN_TYPE )
        status_screen_down_data_change( dd.machine_name(), dd.category(), dd.subcategory() );

    if ( !UseDowntimeEntry )
        return;

    /*
    ----------------------------------------------------------------------------------
    Do nothing if the downtime entry is already displayed (I will miss this downtime).
    ---------------------------------------------------------------------------------- */
    if ( DowntimeEntryDialog )
        return;

    if ( MachList.find(dd.machine_name()) )
        {
        if ( strings_are_equal(c.whoami(), MachList.computer_name()) )
            {
            if ( strings_are_equal(DOWNCAT_SYSTEM_CAT, dd.category()) && strings_are_equal(DOWNCAT_UNSP_DOWN_SUBCAT, dd.subcategory()) )
                {
                DowntimeEntryMachine = dd.machine_name();
                ShowWindow( CreateDialog( MainInstance, TEXT("DOWNTIME_ENTRY_DIALOG"), MainWindow, (DLGPROC) downtime_entry_dialog_proc ), SW_SHOW );
                }
            }
        }
    }
}

/***********************************************************************
*                             SHOW_TIMEOUT_MESSAGE                     *
***********************************************************************/
static void show_timeout_message( TCHAR * machine_name )
{
const COLORREF white = RGB( 255, 255, 255 );

HDC   dc;
RECT  r;
HFONT old_font;
STRING_CLASS s;
SIZE         sz;

dc  = GetDC( MainPlotWindow );
old_font = (HFONT) SelectObject( dc, GetStockObject(SYSTEM_FIXED_FONT) );

s = resource_string( TIMEOUT_STRING );
s += SpaceString;
s += machine_name;

GetTextExtentPoint32( dc, s.text(), s.len(), &sz );

GetClientRect( MainPlotWindow, &r );

r.top+= 2;
r.bottom = r.top + (sz.cy + 6 );
r.left = r.right - (sz.cx + 6);

fill_rectangle( dc, r, AlarmColor );
draw_rectangle( dc, r, white );

SetTextAlign( dc, TA_TOP | TA_LEFT );
SetTextColor( dc, CurrentColor[TEXT_COLOR] );

SetBkMode( dc, TRANSPARENT );

TextOut( dc, r.left+2, r.top+2, s.text(), s.len() );

SelectObject( dc, old_font );

ReleaseDC( MainPlotWindow, dc );
}

/***********************************************************************
*                      NEW_MACHINE_STATE_CALLBACK                      *
***********************************************************************/
void new_machine_state_callback( TCHAR * topic, short item, HDDEDATA edata )
{
NEW_MACHINE_STATE_CLASS ms;

if ( !ms.set(edata) )
    return;

if ( MyScreenType == BOX_SCREEN_TYPE )
    box_screen_new_machine_status( ms.machine_name(), ms.new_status() );
else if ( MyScreenType == STATUS_SCREEN_TYPE )
    status_screen_new_machine_status( ms.machine_name(), ms.new_status() );
else
    {
    /*if ( strings_are_equal(ms.machine_name(), MachineName) ) */
    if ( ms.new_status() & VIS_TIME_OUT )
        show_timeout_message( ms.machine_name() );
    }
}

/***********************************************************************
*                      NEW_MACHINE_SETUP_CALLBACK                      *
***********************************************************************/
void new_machine_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
const DWORD BYTES_TO_COPY = sizeof(TCHAR) * (MACHINE_NAME_LEN+1);
DWORD          bytes_copied;
COMPUTER_CLASS c;
TCHAR          s[COMPUTER_NAME_LEN+1];

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_copied = DdeGetData( edata, (LPBYTE) NewSetupMachine, BYTES_TO_COPY, 0 );
if ( bytes_copied > 0 )
    {
    *(NewSetupMachine+MACHINE_NAME_LEN) = NullChar;
    fix_dde_name( NewSetupMachine );
    if ( MachList.get_computer_name(s,NewSetupMachine) )
        c.set_online_status( s, TRUE );
    SendMessage( MainWindow, WM_NEWSETUP, 0, 0L );
    }
}

/***********************************************************************
*                        MONITOR_SETUP_CALLBACK                        *
***********************************************************************/
void monitor_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
DWORD   bytes_copied;
TCHAR   machine_name[MACHINE_NAME_LEN+1];
int32   slen;
COMPUTER_CLASS c;

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

slen = (MACHINE_NAME_LEN + 1) * sizeof( TCHAR );

bytes_copied = DdeGetData( edata, (LPBYTE) machine_name, slen, 0 );
if ( bytes_copied > 0 )
    {
    *(machine_name+MACHINE_NAME_LEN) = NullChar;
    fix_dde_name( machine_name );

    if ( c.count() > 1 )
        box_screen_new_monitor_setup( machine_name );
    else
        {
        if ( MyScreenType == BOX_SCREEN_TYPE )
            end_box_display();
        load_realtime_machine_list();
        if ( MyScreenType == BOX_SCREEN_TYPE )
            {
            begin_box_display();
            InvalidateRect( MainPlotWindow, 0, TRUE );
            }
        }

    /*
    --------------------------------------------
    Reload the current machine if it is this one
    -------------------------------------------- */
    if ( strings_are_equal(MachineName, CurrentMachine.name) )
        CurrentMachine.find( ComputerName, MachineName );
    }
}

/***********************************************************************
*                          UPDATE_SCREEN_TYPE                          *
***********************************************************************/
void update_screen_type( short new_screen_type )
{
static BOOLEAN was_realtime       = FALSE;
static BOOLEAN was_single_machine = FALSE;
short old_screen_type;

if ( new_screen_type == PROFILE_SCREEN_TYPE || new_screen_type == STANDARD_SCREEN_TYPE || new_screen_type == ZABISH_SCREEN_TYPE )
    {
    /*
    -------------------------------------
    See if I am turning off the xdcr test
    ------------------------------------- */
    if ( InfoBoxType == XDCR_INFO_BOX )
        set_info_box_type( PARAMETER_INFO_BOX );

    if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == STATUS_SCREEN_TYPE )
        HaveHeader = FALSE;
    }

if ( new_screen_type == MyScreenType )
    {
    return;
    }

if ( new_screen_type == STANDARD_SCREEN_TYPE || new_screen_type == ZABISH_SCREEN_TYPE )
    StandardProfileScreenType = new_screen_type;

if ( zoomed() || Zooming )
    {
    turn_off_zooming();
    unzoom( FALSE );
    }
else
    {
    clear_axes();
    }

switch ( MyScreenType )
    {
    case STATUS_SCREEN_TYPE:
        end_status_display();
        break;

    case BOX_SCREEN_TYPE:
        end_box_display();
        break;

    case TREND_SCREEN_TYPE:
        end_trend_display();
        RealTime          = was_realtime;
        SingleMachineOnly = was_single_machine;
        break;

    case DOWNBAR_SCREEN_TYPE:
        end_downbar_display();
        RealTime          = was_realtime;
        SingleMachineOnly = was_single_machine;
        break;

    default:
        break;
    }

old_screen_type = MyScreenType;

/*
-------------------------------------------------------------------------
If you ask for a profile screen you get whichever standard was last used.
------------------------------------------------------------------------- */
if ( new_screen_type == PROFILE_SCREEN_TYPE )
    MyScreenType = StandardProfileScreenType;
else
    MyScreenType = new_screen_type;

position_parameter_window();
position_zoom_box_window();
position_main_plot_window();
if ( MyScreenType == ZABISH_SCREEN_TYPE )
    position_param_history_window();

show_corner_parameters();

SetFocus( MainWindow );

switch ( MyScreenType )
    {
    case STATUS_SCREEN_TYPE:
        begin_status_display();
        InvalidateRect( MainPlotWindow, 0, TRUE );
        break;

    case BOX_SCREEN_TYPE:
        begin_box_display();
        InvalidateRect( MainPlotWindow, 0, TRUE );
        break;

    case TREND_SCREEN_TYPE:
        was_realtime       = RealTime;
        was_single_machine = SingleMachineOnly;
        begin_trend_display();
        break;

    case DOWNBAR_SCREEN_TYPE:
        begin_downbar_display();

        /* if ( !begin_downbar_display() )
            {
            resource_message_box( MainInstance, NO_DOWNTIME_DATA_STRING, NOTHING_TO_DISPLAY_STRING );
            update_screen_type( PROFILE_SCREEN_TYPE );
            return;
            }
        */
        was_realtime       = RealTime;
        was_single_machine = SingleMachineOnly;
        RealTime           = FALSE;
        SingleMachineOnly  = FALSE;

        if ( old_screen_type == TREND_SCREEN_TYPE || old_screen_type == BOX_SCREEN_TYPE || old_screen_type == STATUS_SCREEN_TYPE )
            InvalidateRect( MainPlotWindow, 0, TRUE );
        break;

    case BIG_PARAM_SCREEN_TYPE:
        if ( old_screen_type == TREND_SCREEN_TYPE || old_screen_type == BOX_SCREEN_TYPE || old_screen_type == STATUS_SCREEN_TYPE )
            InvalidateRect( MainPlotWindow, 0, TRUE );
        break;

    default:
        load_main_menu();
        break;
    }

set_zoom_grayness();
}

/***********************************************************************
*                         IS_CURRENT_SHOT                              *
***********************************************************************/
BOOLEAN is_current_shot( PROFILE_NAME_ENTRY * p )
{

if ( lstrcmp(p->computer, ComputerName) == 0 )
    if ( lstrcmp(p->machine, MachineName) == 0 )
        if ( lstrcmp(p->part, PartName) == 0 )
            if ( lstrcmp(p->shot, ShotName) == 0 )
                return TRUE;

return FALSE;
}

/***********************************************************************
*                          REMOVE_OVERLAY_SHOT                         *
***********************************************************************/
static void remove_overlay_shot( int dead_shot )
{
int i;

/*
------------------------------------
Do nothing if there is only one shot
------------------------------------ */
if ( NofOverlayShots < 2 )
    return;

if ( dead_shot < 0 || dead_shot >= NofOverlayShots )
    return;

NofOverlayShots--;
for ( i=dead_shot; i<NofOverlayShots; i++ )
    OverlayShot[i] = OverlayShot[i+1];

}

/***********************************************************************
*                         APPEND_OVERLAY_SHOT                          *
*  If the new shot is already on the list, Remove it unless it is      *
*  the only one. Return TRUE only if I add to the list;                *
***********************************************************************/
static BOOLEAN append_overlay_shot()
{
int i;

if ( NofOverlayShots > 1 )
    {
    for ( i=0; i<NofOverlayShots; i++ )
        {
        if ( is_current_shot(OverlayShot+i) )
            {
            remove_overlay_shot(i);
            return FALSE;
            }
        }
    }

/*
------------------------------------------
Remove the oldest shot if the list is full
------------------------------------------ */
if ( NofOverlayShots >= MAX_OVERLAY_SHOTS )
    remove_overlay_shot(0);

lstrcpy( OverlayShot[NofOverlayShots].computer, ComputerName );
lstrcpy( OverlayShot[NofOverlayShots].machine,  MachineName );
lstrcpy( OverlayShot[NofOverlayShots].part,     PartName );
lstrcpy( OverlayShot[NofOverlayShots].shot,     ShotName );

NofOverlayShots++;
return TRUE;
}

/***********************************************************************
*                              DO_NEWSHOT                              *
***********************************************************************/
static void do_newshot()
{
COMPUTER_CLASS  c;
BOOLEAN need_reset;
int     old_count;

if ( !load_part_info() )
    return;

ReadyForZoom = FALSE;
HaveHeader   = FALSE;
need_reset   = TRUE;

c.set_online_status( ComputerName, TRUE );
old_count = NofCornerParameters;
HaveNewXdcrPart = true;

if ( MyScreenType == BOX_SCREEN_TYPE )
    {
    load_profile();
    box_screen_new_shot( MachineName );
    return;
    }
else if ( MyScreenType == STATUS_SCREEN_TYPE )
    {
    status_screen_new_shot( MachineName );
    return;
    }
else if ( MyScreenType == TREND_SCREEN_TYPE )
    {
    trend_screen_new_shot();
    return;
    }
else if ( MyScreenType == BIG_PARAM_SCREEN_TYPE )
    {
    InvalidateRect( MainPlotWindow, 0, TRUE );
    return;
    }

if ( overlaying() )
    {
    if ( append_overlay_shot() )
        {
        get_mydc();
        load_profile();
        paint_the_curves();
        show_shot_info();
        show_corner_parameters();
        release_mydc();
        need_reset = FALSE;
        }
    else
        {
        load_part_info( OverlayShot );
        }
    }

if ( need_reset )
    {
    if ( PartBasedDisplay && old_count != NofCornerParameters )
        {
        position_parameter_window();
        position_main_plot_window();
        if ( MyScreenType == ZABISH_SCREEN_TYPE )
            position_param_history_window();
        }
    reset_plot_curves();
    InvalidateRect( MainPlotWindow, 0, TRUE );
    NeedCornerParamUpdate = TRUE;
    }

if ( zoomed() )
    show_zoom_info();
else if ( IsWindowVisible(ParamHistoryWindow) )
    InvalidateRect( ParamHistoryWindow, 0, TRUE );
else if ( InfoBoxType == XDCR_INFO_BOX )
    XdcrWindow.refresh();

ReadyForZoom = TRUE;
}

/***********************************************************************
*                         SAVE_LAST_SHOT_NAME                          *
***********************************************************************/
void save_last_shot_name()
{
put_ini_string( DisplayIniFile, ConfigSection, LastMachineKey, MachineName );
put_ini_string( DisplayIniFile, ConfigSection, LastPartKey,    PartName );
put_ini_string( DisplayIniFile, ConfigSection, LastShotKey,    ShotName );
}

/***********************************************************************
*                         REDISPLAY_LAST_SHOT                          *
***********************************************************************/
BOOLEAN redisplay_last_shot()
{
TCHAR * cp;

cp = get_ini_string( DisplayIniFile, ConfigSection, LastMachineKey );
if ( lstrcmp(cp, UnknownString) == 0 )
    return FALSE;

if ( !MachList.find(cp) )
    return FALSE;
lstrcpyn( ComputerName, MachList.computer_name(), COMPUTER_NAME_LEN+1 );

lstrcpyn( MachineName, cp, MACHINE_NAME_LEN+1 );

cp = get_ini_string( DisplayIniFile, ConfigSection, LastPartKey );
if ( unknown(cp) )
    return FALSE;

lstrcpyn( PartName, cp, PART_NAME_LEN+1 );
if ( !part_exists(ComputerName, MachineName, PartName) )
    {
    PartName[0] = NullChar;
    return FALSE;
    }


cp = get_ini_string( DisplayIniFile, ConfigSection, LastShotKey );
if ( unknown(cp) )
    return FALSE;

lstrcpyn( ShotName, cp, SHOT_LEN+1 );
return TRUE;
}

/***********************************************************************
*                             FILL_VERSION                             *
***********************************************************************/
void fill_version( HWND w )
{
TCHAR * cp;

cp = get_ini_string( VisiTrakIniFile, ConfigSection, TEXT("Version") );
if ( cp )
    set_text( w, ABOUT_VERSION_TEXTBOX, cp );

cp = get_ini_string( VisiTrakIniFile, ConfigSection, SerialNumberKey );
if ( cp )
    if ( !unknown(cp) )
        set_text( w, ABOUT_SERIAL_EBOX, cp );
}

/***********************************************************************
*                          SAVE_SERIAL_NUMBER                          *
***********************************************************************/
static bool save_serial_number( HWND w )
{
STRING_CLASS s;
TCHAR * cp;

w = GetDlgItem( w, ABOUT_SERIAL_EBOX );

s.get_text( w );
s.uppercase();
cp = s.text();
if ( *cp == NChar )
    {
    cp++;
    put_ini_string( VisiTrakIniFile, ConfigSection, SerialNumberKey, cp );
    set_text( w, cp );
    return true;
    }
return false;
}

/***********************************************************************
*                           ABOUT_DIALOG_PROC                          *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK about_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        fill_version( hWnd );
        return TRUE;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( save_serial_number(hWnd) )
                    return TRUE;
            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                    POSITION_MACHINE_LIST_LISTBOX                     *
***********************************************************************/
static void position_machine_list_listbox( HWND hWnd )
{
RECTANGLE_CLASS r;
RECTANGLE_CLASS rc;
WINDOW_CLASS w;

w = hWnd;
w.get_client_rect( r );

w = w.control( MACHINE_LIST_LB );
w.get_move_rect( rc );
rc.left = r.left + 4;
rc.right = r.right - 4;
rc.bottom = r.bottom - 4;
w.move(rc);
}

/***********************************************************************
*                       MAKE_A_LIST_OF_MACHINES                        *
***********************************************************************/
static void make_a_list_of_machines( HWND hWnd )
{
COMPUTER_CLASS c;
TCHAR          address[TCP_ADDRESS_LEN+1];
DB_TABLE       t;
LISTBOX_CLASS  lb;
NAME_CLASS     s;
MACHINE_CLASS  m;
int            board;
BOOLEAN        monitoring_is_on;
BOOLEAN        exists;
TCHAR          co[COMPUTER_NAME_LEN+1];
lb.init( hWnd, MACHINE_LIST_LB );
lb.set_tabs( COMPUTER_NAME_STATIC, ADDRESS_STATIC );

s.upsize( MAX_PATH );
MachList.rewind();
while ( MachList.next() )
    {
    board = 0;
    monitoring_is_on = FALSE;
    address[0] = NullChar;
    copystring( co, MachList.computer_name() );
    exists = FALSE;
    if ( c.online_status(co) )
        {
        s.get_machset_file_name(co);
        copystring( m.computer, MachList.computer_name() );
        copystring( m.name,     MachList.name() );

        if ( m.read_machset_db(s.text()) )
            {
            exists = true;
            board = m.ft_board_number;
            monitoring_is_on = FALSE;
            if ( m.monitor_flags & MA_MONITORING_ON )
                monitoring_is_on = TRUE;

            s.get_ft2_boards_file_name( co );
            if ( s.file_exists() )
                {
                t.open( s.text(), FTII_BOARDS_RECLEN, PFL );
                while ( t.get_next_record(FALSE) )
                    {
                    if ( board == (int) t.get_short(FTII_BOARDS_NUMBER_OFFSET) )
                        {
                        t.get_alpha( address, FTII_BOARDS_ADDRESS_OFFSET, TCP_ADDRESS_LEN );
                        break;
                        }
                    }
                }
            }
        }
    s = MachList.name();
    s += TabChar;
    s += co;
    s += TabChar;
    c.find( co );
    s += c.directory();
    s += TabChar;
    if ( exists )
        {
        if ( monitoring_is_on )
            s += YChar;
        else
            s += NChar;
        s += TabChar;
        s += board;
        s += TabChar;
        s += address;
        }
    else
        {
        s += resource_string( OFF_LINE_STRING );
        }

    lb.add( s.text() );
    }
}

/***********************************************************************
*                        MACHINE_LIST_DIALOG_PROC                      *
* A modal dialog that displays a list of the machines I know about     *
* and the computers and directories associated with them.              *
***********************************************************************/
BOOL CALLBACK machine_list_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        make_a_list_of_machines( hWnd );
        return TRUE;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_SIZE:
        position_machine_list_listbox( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
            case IDOK:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                             CLOSE_DIALOG                             *
***********************************************************************/
void close_dialog( HWND w )
{
DialogIsActive = FALSE;
EndDialog( w, 0 );
}

/***********************************************************************
*                       ENABLE_PREV_NEXT_BUTTONS                       *
***********************************************************************/
void enable_prev_next_buttons( HWND w )
{
BOOLEAN by_shift;
BOOLEAN by_day;
BOOLEAN prev_is_enabled;
BOOLEAN next_is_enabled;
HWND    prev_window;
HWND    next_window;
HWND    focus_window;

by_shift = is_checked(w, BY_SHIFT_RADIO );
by_day   = is_checked(w, BY_DAY_RADIO );

prev_is_enabled = FALSE;
next_is_enabled = FALSE;

if ( by_day )
    {
    if ( !samedate(CurrentAlarmDate, FirstAlarmDate)  )
        prev_is_enabled = TRUE;

    if ( !samedate(CurrentAlarmDate, LastAlarmDate)  )
        next_is_enabled = TRUE;
    }
else if ( by_shift )
    {
    if ( !samedate(CurrentAlarmDate, FirstAlarmDate) || CurrentAlarmShift != FirstAlarmShift )
        prev_is_enabled = TRUE;

    if ( !samedate(CurrentAlarmDate, LastAlarmDate) || CurrentAlarmShift != LastAlarmShift )
        next_is_enabled = TRUE;
    }

prev_window  = GetDlgItem( w, PREV_BUTTON );
next_window  = GetDlgItem( w, NEXT_BUTTON );
focus_window = GetFocus();

EnableWindow( prev_window, prev_is_enabled );
EnableWindow( next_window, next_is_enabled );

if ( focus_window == prev_window && !prev_is_enabled )
    {
    if ( next_is_enabled )
        SetFocus( next_window );
    else
        SetFocus( GetDlgItem(w, ALARM_RADIO) );
    }
else if ( focus_window == next_window && !next_is_enabled )
    {
    if ( prev_is_enabled )
        SetFocus( prev_window );
    else
        SetFocus( GetDlgItem(w, ALARM_RADIO) );
    }
}

/***********************************************************************
*                           NEXT_TIME_PERIOD                           *
***********************************************************************/
static void next_time_period( HWND w, int button_id )
{
BOOLEAN    by_shift;
BOOLEAN    status;
DB_TABLE   t;
NAME_CLASS filename;
SYSTEMTIME mydate;
int32      myshift;

by_shift = is_checked(w, BY_SHIFT_RADIO );

filename = alarmsum_dbname( ComputerName, MachineName, PartName );
if ( !filename.file_exists() )
    return;

if ( !t.open(filename.text(), ALARMSUM_RECLEN, PFL) )
    return;

t.put_date( ALARMSUM_DATE_OFFSET,  CurrentAlarmDate );
status = t.get_next_key_match( 1, NO_LOCK );
if ( status && by_shift )
    {
    status = FALSE;
    while ( TRUE )
        {
        t.get_date( mydate, ALARMSUM_DATE_OFFSET );
        if ( !samedate( mydate, CurrentAlarmDate) )
            break;

        myshift = t.get_long( ALARMSUM_SHIFT_OFFSET );
        if ( myshift == CurrentAlarmShift )
            {
            status = TRUE;
            break;
            }

        if ( !t.get_next_record(NO_LOCK) )
            break;
        }
    }

if ( status )
    {
    myshift = CurrentAlarmShift;
    mydate  = CurrentAlarmDate;

    while ( TRUE )
        {
        if ( button_id == NEXT_BUTTON )
            status = t.get_next_record( NO_LOCK );
        else
            status = t.get_prev_record( NO_LOCK );

        if ( !status )
            break;

        t.get_date( mydate, ALARMSUM_DATE_OFFSET );
        myshift = t.get_long( ALARMSUM_SHIFT_OFFSET );

        if ( !samedate( mydate, CurrentAlarmDate) )
            break;

        if ( by_shift && myshift != CurrentAlarmShift )
            break;
        }

    if ( status )
        {
        CurrentAlarmShift = myshift;
        CurrentAlarmDate  = mydate;
        enable_prev_next_buttons( w );
        }
    }

t.close();
}

/***********************************************************************
*                        GET_ALARM_DATE_EXTENTS                        *
***********************************************************************/
static BOOLEAN get_alarm_date_extents()
{
DB_TABLE  t;
NAME_CLASS s;
BOOLEAN   status;
int32     n;

init_systemtime_struct( FirstAlarmDate );
init_systemtime_struct( LastAlarmDate );

s = alarmsum_dbname( ComputerName, MachineName, PartName );

if ( !s.file_exists() )
    return FALSE;

if ( !t.open(s.text(), ALARMSUM_RECLEN, PFL) )
    return FALSE;

n = t.nof_recs();

status = FALSE;
if ( n > 0 )
    {
    if (  t.get_next_record(NO_LOCK) )
        {
        t.get_date( FirstAlarmDate, ALARMSUM_DATE_OFFSET );
        FirstAlarmShift = t.get_long( ALARMSUM_SHIFT_OFFSET );
        n--;
        if ( t.goto_record_number(n) )
            {
            if ( t.get_rec() )
                {
                t.get_date( LastAlarmDate, ALARMSUM_DATE_OFFSET );
                LastAlarmShift = t.get_long( ALARMSUM_SHIFT_OFFSET );
                status = TRUE;
                }
            }
        }
    }

t.close();
return status;
}

/***********************************************************************
*                            ALARM_LB_STRING                           *
***********************************************************************/
static TCHAR * alarm_lb_string( HWND w, int32 i, ALARM_SUMMARY_CLASS & as )
{
const  int32 BUFLEN = PARAMETER_NAME_LEN + 1 + PARMLIST_FLOAT_LEN + 1 + ALARMSUM_COUNTS_LEN + 2 + PARMLIST_FLOAT_LEN + 1 + ALARMSUM_COUNTS_LEN +1;
static TCHAR buf[BUFLEN+1];
TCHAR      * cp;
int32        low_count;
int32        high_count;
int32        total_count;
int32        low_type;
int32        high_type;
BOOLEAN      is_percent;

total_count = as.total_shot_count();
if ( total_count <= 0 )
    total_count = 1;

if ( is_checked(w, ALARM_RADIO) )
    {
    low_type  = ALARM_MIN;
    high_type = ALARM_MAX;
    }
else
    {
    low_type  = WARNING_MIN;
    high_type = WARNING_MAX;
    }

low_count  = as.count( i+1, low_type );
high_count = as.count( i+1, high_type );

is_percent = is_checked(w, PERCENT_RADIO );
if ( is_percent )
    {
    low_count *= 100;
    low_count /= total_count;
    high_count *= 100;
    high_count /= total_count;
    }

cp = buf;

cp = copy_w_char( cp, CurrentParam.parameter[i].name, TabChar );
cp = copy_w_char( cp, ascii_float(CurrentParam.parameter[i].limits[low_type].value), TabChar );

cp = copystring( cp, int32toasc(low_count) );
if ( is_percent )
    *cp++ = PercentChar;
*cp++ = TabChar;

cp = copy_w_char( cp, ascii_float(CurrentParam.parameter[i].limits[high_type].value), TabChar );

cp = copystring( cp, int32toasc(high_count) );
if ( is_percent )
    *cp++ = PercentChar;
*cp = NullChar;

return buf;
}

/***********************************************************************
*                          GET_ALARM_PERIOD_TYPE                       *
***********************************************************************/
void get_alarm_period_type( BOOLEAN & by_shift, BOOLEAN & by_date )
{
if ( AlarmSummaryDialogWindow )
    {
    by_shift = is_checked(AlarmSummaryDialogWindow, BY_SHIFT_RADIO );

    if ( by_shift )
        by_date = TRUE;
    else
        by_date  = is_checked(AlarmSummaryDialogWindow, BY_DAY_RADIO );
    }
}

/***********************************************************************
*                         DATE_AND_SHIFT_STRING                        *
*                          "MMDDYYYY Shift 10"                         *
***********************************************************************/
TCHAR * date_and_shift_string( TIME_CLASS & t, int32 shift )
{
static TCHAR * buf = 0;
static TCHAR * cp;
int32          slen;

if ( !buf )
    {
    cp = resource_string(SHIFT_STRING);
    slen = lstrlen(cp) + ALPHADATE_LEN + 2;
    buf = maketext( slen );
    }

if ( buf )
    {
    cp = copy_w_char( buf, t.mmddyyyy(), SpaceChar );
    cp = copy_w_char( cp, resource_string(SHIFT_STRING), SpaceChar );
    lstrcpy( cp, int32toasc(shift) );
    return buf;
    }

return EmptyString;
}

/***********************************************************************
*                          PRINT_ALARM_SUMMARY                         *
***********************************************************************/
static void print_alarm_summary()
{
ALARM_SUMMARY_CLASS as;
BOOLEAN             by_shift;
BOOLEAN             by_date;
BOOLEAN             is_alarm;
BOOLEAN             is_percent;
int32               i;
int32               n;
int32               dx;
int32               dy;
int32               low_count;
int32               high_count;
int32               total_count;
int32               x;
int32               x_low_limit;
int32               x_low_value;
int32               x_high_limit;
int32               x_high_value;
int32               y;
int32               low_type;
int32               high_type;
TCHAR               buf[ALARMSUM_COUNTS_LEN+3];
TCHAR             * cp;
TEXT_LEN_CLASS      tmax;
STRING_CLASS        s;
UINT                rid;

if ( !AlarmSummaryDialogWindow )
    return;

if ( is_checked(AlarmSummaryDialogWindow, ALARM_RADIO) )
    {
    is_alarm  = TRUE;
    low_type  = ALARM_MIN;
    high_type = ALARM_MAX;
    }
else
    {
    is_alarm = FALSE;
    low_type  = WARNING_MIN;
    high_type = WARNING_MAX;
    }

dx  = MyCharWidth;
x   = dx;

dy  = character_height( MyDc );
y   = dy;

as.set_part( ComputerName, MachineName, PartName );
get_alarm_period_type( by_shift, by_date );

s = resource_string( MACHINE_INTRO_STRING );
s += MachineName;
s += resource_string( PART_INTRO_STRING );
s += PartName;
if ( by_date )
    {
    s += TwoSpacesString;
    s += alphadate( CurrentAlarmDate );
    }

if ( by_shift )
    {
    s += TwoSpacesString;
    s += resource_string( SHIFT_STRING );
    s += int32toasc( CurrentAlarmShift );
    as.get_counts( CurrentAlarmDate, CurrentAlarmShift );
    }
else if ( by_date )
    {
    as.get_counts( CurrentAlarmDate );
    }
else
    {
    as.get_counts();
    s += TwoSpacesString;
    s += resource_string( DOWNTIME_FROM_STRING );
    s += date_and_shift_string( as.start_time(), as.start_shift_number() );
    s += resource_string( DOWNTIME_TO_STRING );
    s += date_and_shift_string( as.end_time(), as.end_shift_number() );
    }

SetTextAlign( MyDc, TA_TOP | TA_CENTER );
text_out( PlotWindowRect.right/2, y, s.text() );

y += 2*dy;

SetTextAlign( MyDc, TA_TOP | TA_LEFT );

/*
------------------------------------------
Get the width of the widest parameter name
------------------------------------------ */
tmax.init( MyDc );

n = CurrentParam.count();
for ( i=0; i<n; i++ )
    tmax.check( CurrentParam.parameter[i].name );

x_low_limit  = x + tmax.width() + dx;

tmax.init( MyDc );

if ( is_alarm )
    {
    tmax.check( resource_string(LOW_ALARM_STRING) );
    tmax.check( resource_string(HIGH_ALARM_STRING) );
    }
else
    {
    tmax.check( resource_string(LOW_WARNING_STRING) );
    tmax.check( resource_string(HIGH_WARNING_STRING) );
    }
tmax.check( resource_string(QUANTITY_BELOW_STRING) );
tmax.check( resource_string(QUANTITY_ABOVE_STRING) );

dx *= 7;
maxint32( dx, tmax.width() );
dx += MyCharWidth;

x_low_value  = x_low_limit  + dx;
x_high_limit = x_low_value  + dx;
x_high_value = x_high_limit + dx;

text_out( x, y, resource_string(PARAMETER_STRING) );

if ( is_alarm )
    rid = LOW_ALARM_STRING;
else
    rid = LOW_WARNING_STRING;

text_out( x_low_limit, y, resource_string(rid) );
text_out( x_low_value, y, resource_string(QUANTITY_BELOW_STRING) );

if ( is_alarm )
    rid = HIGH_ALARM_STRING;
else
    rid = HIGH_WARNING_STRING;
text_out( x_high_limit, y, resource_string(rid) );

text_out( x_high_value, y, resource_string(QUANTITY_ABOVE_STRING) );

y += 2*dy;


total_count = as.total_shot_count();
if ( total_count <= 0 )
    total_count = 1;

is_percent = is_checked(AlarmSummaryDialogWindow, PERCENT_RADIO );

for ( i=0; i<n; i++ )
    {
    text_out( x, y, CurrentParam.parameter[i].name );

    low_count  = as.count( i+1, low_type );
    high_count = as.count( i+1, high_type );

    if ( is_percent )
        {
        low_count *= 100;
        low_count /= total_count;
        high_count *= 100;
        high_count /= total_count;
        }

    text_out( x_low_limit, y, ascii_float(CurrentParam.parameter[i].limits[low_type].value) );

    cp = buf;
    cp = copystring( cp, int32toasc(low_count) );
    if ( is_percent )
        *cp++ = PercentChar;
    *cp = NullChar;
    text_out( x_low_value, y, buf );

    text_out( x_high_limit, y, ascii_float(CurrentParam.parameter[i].limits[high_type].value) );

    cp = buf;
    cp = copystring( cp, int32toasc(high_count) );
    if ( is_percent )
        *cp++ = PercentChar;
    *cp = NullChar;
    text_out( x_high_value, y, buf );
    y += dy;
    }

y += dy;

s = resource_string( TOTAL_SHOTS_INTRO_STRING );
s += int32toasc( total_count );
text_out( x, y, s.text() );
y += dy;

if ( is_alarm )
    {
    s = resource_string( TOTAL_ALARM_SHOTS_INTRO_STRING );
    i = as.total_alarm_count();
    }
else
    {
    i = as.total_warning_count();
    s = resource_string( TOTAL_WARNING_SHOTS_INTRO_STRING );
    }

if ( is_percent )
    {
    i *= 100;
    i /= total_count;
    }

s += int32toasc( i );
if ( is_percent )
    s += PercentString;

text_out( x, y, s.text() );
}

/***********************************************************************
*                            GET_ALARM_DATA                            *
***********************************************************************/
static void get_alarm_data( HWND w )
{
ALARM_SUMMARY_CLASS as;
LISTBOX_CLASS       lb;
int32               i;
int32               n;
BOOLEAN             by_shift;
BOOLEAN             by_date;
BOOLEAN             is_percent;
TCHAR             * cp;
STRING_CLASS        s;
INT                 top_index;

hourglass_cursor();

lb.init( w, ALARM_SUMMARY_LISTBOX );

as.set_part( ComputerName, MachineName, PartName );

get_alarm_period_type( by_shift, by_date );

if ( by_date )
    cp = alphadate( CurrentAlarmDate );
else
    cp = EmptyString;

set_text( w, ALARM_DATE_TBOX, cp );

if ( by_shift )
    {
    s = resource_string( SHIFT_STRING );
    s += int32toasc( CurrentAlarmShift );
    cp = s.text();
    }
else
    {
    cp = EmptyString;
    }

set_text( w, ALARM_SHIFT_TBOX, cp );

if ( by_shift )
    as.get_counts( CurrentAlarmDate, CurrentAlarmShift );
else if ( by_date )
    as.get_counts( CurrentAlarmDate );
else
    as.get_counts();

if ( lb.count() )
    top_index = lb.top_index();
else
    top_index = NO_INDEX;

lb.redraw_off();
lb.empty();

n = CurrentParam.count();

for ( i=0; i<n; i++ )
    lb.add( alarm_lb_string(w, i, as) );

if ( top_index != NO_INDEX )
    lb.set_top_index( top_index );

lb.redraw_on();

n = as.total_shot_count();
set_text( w, ALARM_TOTAL_SHOTS_TBOX, int32toasc(n) );

if ( n <= 0 )
    n = 1;

if ( is_checked(w, ALARM_RADIO) )
    i = as.total_alarm_count();
else
    i = as.total_warning_count();

is_percent = is_checked(w, PERCENT_RADIO );
if ( is_percent )
    {
    i *= 100;
    i /= n;
    }

s = int32toasc( i );

if ( is_percent )
    s += PercentString;

set_text( w, ALARM_OUT_TBOX, s.text() );

restore_cursor();
}

/***********************************************************************
*                      SET_ALARM_SUMMARY_TAB_STOPS                     *
***********************************************************************/
static void set_alarm_summary_tab_stops( HWND w )
{
int tabs[4];
int x;

x = LOWORD( GetDialogBaseUnits() );
x /= 2;

tabs[0] = 22 * x;
tabs[1] = 31 * x;
tabs[2] = 41 * x;
tabs[3] = 51 * x;

SendDlgItemMessage( w, ALARM_SUMMARY_LISTBOX, LB_SETTABSTOPS, (WPARAM) 4, (LPARAM) tabs );
}

/***********************************************************************
*                           ALARM_SUMMARY_PROC                         *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK alarm_summary_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
HWND w;
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        set_alarm_summary_tab_stops( hWnd );
        set_text( hWnd, ALARM_MACHINE_TBOX, MachineName );
        set_text( hWnd, ALARM_PART_TBOX,    PartName );
        CheckRadioButton( hWnd, PERCENT_RADIO, COUNT_RADIO, PERCENT_RADIO );
        CheckRadioButton( hWnd, BY_SHIFT_RADIO, ALL_DATA_RADIO, BY_SHIFT_RADIO );
        CheckRadioButton( hWnd, WARNING_RADIO,  ALARM_RADIO,    ALARM_RADIO );
        if ( !get_alarm_date_extents() )
            {
            resource_message_box( MainInstance, NO_ALARM_DATA_STRING, NOTHING_TO_DISPLAY_STRING );
            close_dialog( hWnd );
            return TRUE;
            }

        CurrentAlarmDate  = LastAlarmDate;
        CurrentAlarmShift = LastAlarmShift;
        enable_prev_next_buttons( hWnd );
        AlarmSummaryDialogWindow = hWnd;
        get_alarm_data( hWnd );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, ALARM_SUMMARY_SCREEN_HELP );
        return TRUE;

    case WM_DBCLOSE:
    case WM_CLOSE:
        AlarmSummaryDialogWindow = 0;
        close_dialog( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
            case IDOK:
                close_dialog( hWnd );
                return TRUE;

            case BY_SHIFT_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    w = GetDlgItem( hWnd, PREV_BUTTON );
                    set_text( w, resource_string(PREV_SHIFT_STRING) );
                    EnableWindow( w, TRUE );

                    w = GetDlgItem( hWnd, NEXT_BUTTON );
                    set_text( w, resource_string(NEXT_SHIFT_STRING) );
                    enable_prev_next_buttons( hWnd );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            case BY_DAY_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    w = GetDlgItem( hWnd, PREV_BUTTON );
                    set_text( w, resource_string(PREV_DAY_STRING) );
                    EnableWindow( w, TRUE );

                    w = GetDlgItem( hWnd, NEXT_BUTTON );
                    set_text( w, resource_string(NEXT_DAY_STRING) );
                    enable_prev_next_buttons( hWnd );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            case ALL_DATA_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    w = GetDlgItem( hWnd, PREV_BUTTON );
                    set_text( w, resource_string(PREV_STRING) );

                    w = GetDlgItem( hWnd, NEXT_BUTTON );
                    set_text( w, resource_string(NEXT_STRING) );
                    enable_prev_next_buttons( hWnd );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            case ALARM_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    set_text( hWnd, ALARM_OUT_TITLE_TBOX, resource_string(TOTAL_ALARM_SHOTS_STRING) );
                    set_text( hWnd, ALARM_LOW_TITLE_TBOX, resource_string(LOW_ALARM_STRING) );
                    set_text( hWnd, ALARM_HIGH_TITLE_TBOX, resource_string(HIGH_ALARM_STRING) );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            case WARNING_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    set_text( hWnd, ALARM_OUT_TITLE_TBOX, resource_string(TOTAL_WARNING_SHOTS_STRING) );
                    set_text( hWnd, ALARM_LOW_TITLE_TBOX, resource_string(LOW_WARNING_STRING) );
                    set_text( hWnd, ALARM_HIGH_TITLE_TBOX, resource_string(HIGH_WARNING_STRING) );
                    get_alarm_data( hWnd );
                    }

            case COUNT_RADIO:
            case PERCENT_RADIO:
                if ( cmd == BN_CLICKED )
                    get_alarm_data( hWnd );

                return TRUE;

            case PREV_BUTTON:
            case NEXT_BUTTON:
                if ( cmd == BN_CLICKED )
                    {
                    next_time_period( hWnd, id );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          ALARM_LOG_SETUP_PROC                        *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK alarm_log_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_DBCLOSE:
    case WM_CLOSE:
        close_dialog( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
            case ALARM_LOG_SETUP_OK_BUTTON:
                close_dialog( hWnd );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          write_std_units_csv_file                    *
***********************************************************************/
BOOLEAN write_std_units_csv_file( STRING_CLASS & destfile, STRING_CLASS & sorcfile )
{
static BOOLEAN firstime = TRUE;
static PART_ANALOG_SENSOR AnalogSensor;
static TCHAR title[] = TEXT( "Type,Sample,Pos,Time,Velocity,Inputs,Outputs,A1,A2,A3,A4,A5,A6,A7,A8" );
FILE_CLASS f;
STRING_CLASS s;
const int last_channel = NOF_FTII_ANALOGS - 1;
int          ai;
int          i;
int          c;
double       real_timer_frequency;
double       x;
FTII_PROFILE fp;

if ( firstime )
    {
    AnalogSensor.get();
    firstime = FALSE;
    }

if ( !fp.get(sorcfile) )
    return FALSE;

real_timer_frequency = (double) fp.timer_frequency;

if ( !f.open_for_write(destfile) )
    return FALSE;

f.writeline( title );

/*
-----------------------------------------
The next row is the units for each column
----------------------------------------- */
s = TEXT( "Pos/Time,Count," );
s += units_name( CurrentPart.distance_units );
s += CommaChar;
s += units_name( SECOND_UNITS );
s += CommaChar;
s += units_name( CurrentPart.velocity_units );
s += TEXT( ",Hex Bits,Hex Bits" );
for ( c=0; c<NOF_FTII_ANALOGS; c++ )
    {
    s += CommaChar;
    ai = (int) CurrentPart.analog_sensor[c];
    s += units_name( AnalogSensor[ai].units_index() );
    }
f.writeline( s.text() );

for ( i=0; i<fp.n; i++ )
    {
    if ( i < fp.np )
        s = TEXT( "P," );
    else
        s = TEXT( "T," );

    s.cat_w_char( int32toasc( int32(i)),           CommaChar );

    x = CurrentPart.dist_from_x4( (double) fp.position[i] );
    s += x;
    s += CommaChar;

    x = (double) fp.timer[i];
    x /= real_timer_frequency;
    s += x;
    s += CommaChar;

    x = (double) fp.velocity[i];
    x = CurrentPart.velocity_from_x4( x );
    s += x;
    s += CommaChar;

    s.cat_w_char( ultohex( fp.isw1[i]), CommaChar );
    s += ultohex( fp.osw1[i] );

    for ( c=0; c<NOF_FTII_ANALOGS; c++ )
        {
        s += CommaChar;
        ai = (int) CurrentPart.analog_sensor[c];
        s += AnalogSensor[ai].converted_value( fp.analog[c][i] );
        }
    f.writeline( s.text() );
    }

f.close();

return TRUE;
}

/***********************************************************************
*                          SET_CSV_SHOT_NAME                           *
***********************************************************************/
static void set_csv_shot_name( HWND hWnd )
{
STRING_CLASS s;
TCHAR myshotname[SHOT_LEN+1];

s.get_text( hWnd, CSV_SHOT_FILE_EBOX );
if ( s.isempty() )
    s = TEXT( "C:\\" );
else
    dir_from_filename( s.text() );

copystring( myshotname, ShotName );
fix_shotname( myshotname );
s.cat_path( myshotname );
s += CsvSuffix;
s.set_text( hWnd, CSV_SHOT_FILE_EBOX );
}

/***********************************************************************
*                       CSV_SHOT_FILE_DIALOG_PROC                      *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK csv_shot_file_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static TCHAR counts_type[]     = TEXT( "COUNTS" );
static TCHAR raw_counts_type[] = TEXT( "RAW" );
static TCHAR std_units_type[]  = TEXT( "STD" );
static BOOLEAN use_shot_number = FALSE;

TCHAR   * cp;
int       id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        {
        TCHAR * cp;
        cp = get_ini_string( DisplayIniFile, ConfigSection, CsvShotFileNameKey );
        if ( !unknown(cp) )
            set_text( hWnd, CSV_SHOT_FILE_EBOX, cp );
        cp = get_ini_string( DisplayIniFile, ConfigSection, CsvShotFileTypeKey );
        id = SAVE_COUNTS_RADIO;
        if ( strings_are_equal(cp, raw_counts_type) )
            id = SAVE_RAW_COUNTS_RADIO;
        else if ( strings_are_equal(cp, std_units_type) )
            id = SAVE_STD_UNITS_RADIO;
        CheckRadioButton( hWnd, SAVE_COUNTS_RADIO, SAVE_STD_UNITS_RADIO, id );
        use_shot_number = boolean_from_ini( DisplayIniFile, ConfigSection, CsvUseShotNumberKey );
        set_checkbox( hWnd, USE_SHOT_NUMBER_XBOX, use_shot_number );
        if ( use_shot_number )
            set_csv_shot_name( hWnd );
        return TRUE;
        }

    case WM_COMMAND:
        switch ( id )
            {
            case USE_SHOT_NUMBER_XBOX:
                use_shot_number = is_checked( hWnd, USE_SHOT_NUMBER_XBOX );
                if ( use_shot_number )
                    set_csv_shot_name( hWnd );
                return TRUE;

            case IDOK:
                {
                NAME_CLASS dest;
                NAME_CLASS sorc;
                sorc.get_profile_ii_name( ComputerName, MachineName, PartName, ShotName );
                dest.get_text( hWnd, CSV_SHOT_FILE_EBOX );
                if ( is_checked(hWnd, SAVE_RAW_COUNTS_RADIO) )
                    {
                    cp = raw_counts_type;
                    write_counts_file( dest, sorc );
                    }
                else if ( is_checked(hWnd, SAVE_STD_UNITS_RADIO) )
                    {
                    cp = std_units_type;
                    write_std_units_csv_file( dest, sorc );
                    }
                else
                    {
                    cp = counts_type;
                    write_olmsted_file( dest, sorc );
                    }

                put_ini_string( DisplayIniFile, ConfigSection, CsvShotFileNameKey, dest.text() );
                put_ini_string( DisplayIniFile, ConfigSection, CsvShotFileTypeKey, cp );
                }
            case IDCANCEL:
                boolean_to_ini( DisplayIniFile, ConfigSection, CsvUseShotNumberKey, use_shot_number );
                close_dialog( hWnd );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                              RUN_MONALL                              *
***********************************************************************/
static void run_monall()
{
static STARTUPINFO info = {
    sizeof(STARTUPINFO),
    0, 0, 0,            // lpReserved, lpDesktop, lpTitle
    0,                  // dwX
    0,                  // dwY
    0, 0, 0, 0, 0,      // dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute
    0,                  // dwFlags
    0, 0, 0, 0, 0, 0    // wShowWindow, cbReserved2, lpReserved2, hStdInput, hStdOutput, hStdError
    };

STRING_CLASS path;
PROCESS_INFORMATION pi;

path = exe_directory();
path.cat_path( TEXT("monall.exe") );

CreateProcess( 0,          // lpApplicationName
    path.text(),           // lpCommandLine
    0, 0, 0,               // lpProcessAttributes, lpThreadAttributes, bInheritHandles,
    HIGH_PRIORITY_CLASS,   // dwCreationFlags
    0, 0,                  // lpEnvironment, lpCurrentDirectory,
    &info,                 // lpStartupInfo = input
    &pi                    // lpProcessInformation = output
    );
}

/***********************************************************************
*                               RUN_TOOL                               *
***********************************************************************/
static void run_tool( int id )
{
const   int NOF_EXES = 15;
TCHAR   path[MAX_PATH+1+MACHINE_NAME_LEN+1+PART_NAME_LEN+1];
TCHAR * cp;

static TCHAR * taskname[] =
    {
    TEXT("editpart"),
    TEXT("emachine"),
    TEXT("monall"),
    TEXT("monedit"),
    TEXT("esensor"),
    TEXT("bupres"),
    TEXT("v5setups"),
    TEXT("downtime"),
    TEXT("editdown"),
    TEXT("suretrak"),
    TEXT("visiedit"),
    TEXT(""),                  //This was inadvertantly assigned to Edit Shifts
    TEXT("stddown"),
    TEXT(""),                  //This was inadvertantly assigned to Edit Shifts
    TEXT("shiftrpt")
    };

id -= PART_EDITOR_MENU;
if ( id == 2 )
   {
   run_monall();
   }
else if ( id < NOF_EXES )
    {
    if ( get_exe_directory(path) )
        {
        lstrcat( path, taskname[id] );
        lstrcat( path, TEXT(".exe") );
        if ( id == 0 || id == 8 )
            {
            cp = path + lstrlen(path);
            *cp = SpaceChar;
            cp++;
            cp = copy_w_char( cp, MachineName, SpaceChar );
            lstrcpy( cp, PartName );
            }
        }
    execute( path );
    }
}

/***********************************************************************
*                           GET_CURRENT_SHOT                           *
***********************************************************************/
BOOLEAN get_current_shot()
{
DB_TABLE   t;
int32      shot_number;
BOOLEAN    have_shot;
NAME_CLASS s;

have_shot   = FALSE;
shot_number = NO_SHOT_NUMBER;

s = plookup_dbname( ComputerName, MachineName );
if ( !s.file_exists() )
    return FALSE;

t.open( s.text(), PLOOKUP_RECLEN, PFL );
t.put_alpha(PLOOKUP_PART_NAME_OFFSET, PartName, PART_NAME_LEN);
t.reset_search_mode();
if ( t.get_next_key_match(1, FALSE) )
    {
    shot_number = t.get_long( PLOOKUP_LAST_SHOT_OFFSET );
    have_shot   = TRUE;
    }
t.close();

lstrcpy( ShotName, int32toasc(shot_number) );
fix_shotname( ShotName );

return have_shot;
}

/***********************************************************************
*                       CREATE_MENU_HELP_BOX                           *
***********************************************************************/
void create_menu_help_box()
{
const UINT STATUS_BOX_ID = 2;
RECT r;
long x;
long y;
long h;
long w;

MenuHelpWindow = CreateStatusWindow(
    WS_CHILD | WS_VISIBLE,
    TEXT("Menu Help"),
    MainPlotWindow,
    STATUS_BOX_ID
    );

if ( MenuHelpWindow )
    {
    GetWindowRect( MenuHelpWindow, &r );
    h = r.bottom - r.top;
    w = r.right  - r.left;

    GetClientRect( MainPlotWindow, &r);
    x = r.left;
    y = r.bottom - h;
    y -= 2;
    MoveWindow( MenuHelpWindow, x, y, w, h, TRUE );
    }
}

/***********************************************************************
*                         SHOW_MENU_HELP_TEXT                          *
***********************************************************************/
void show_menu_help_text( UINT id )
{

struct MENU_HELP_TEXT_ENTRY {
    UINT id;
    UINT resource_id;
    };

static MENU_HELP_TEXT_ENTRY helplist[] =
    {
    { CHOOSE_PART_MENU,          CHOOSE_PART_MENU_HELP          },
    { CHOOSE_SHOT_MENU,          CHOOSE_SHOT_MENU_HELP          },
    { EDIT_SHIFTS_MENU,          EDIT_SHIFTS_MENU_HELP          },
    { EXIT_MENU,                 EXIT_MENU_HELP                 },
    { MINIMIZE_MENU,             MINIMIZE_MENU_HELP             },
    { SAVE_MENU,                 SAVE_MENU_HELP                 },
    { SAVE_AS_MASTER_MENU,       SAVE_AS_MASTER_MENU_HELP       },
    { PRINT_MENU,                PRINT_MENU_HELP                },
    { MULTIPLE_SHOT_PRINT_MENU,  MULTIPLE_SHOT_PRINT_MENU_HELP  },
    { POS_TIME_MENU,             POS_TIME_MENU_HELP             },
    { POS_ONLY_MENU,             POS_ONLY_MENU_HELP             },
    { TIME_ONLY_MENU,            TIME_ONLY_MENU_HELP            },
    { LAST_50_SCREEN_MENU,       LAST_50_SCREEN_MENU_HELP       },
    { STD_SCREEN_MENU,           STD_SCREEN_MENU_HELP           },
    { VIEW_REFERENCE_TRACE_MENU, VIEW_REFERENCE_TRACE_MENU_HELP },
    { VIEW_PARAMETERS_MENU,      VIEW_PARAMETERS_MENU_HELP      },
    { BOX_SCREEN_MENU,           BOX_SCREEN_MENU_HELP           },
    { TREND_SCREEN_MENU,         TREND_SCREEN_MENU_HELP         },
    { PROFILE_SCREEN_MENU,       PROFILE_SCREEN_MENU_HELP       },
    { DOWNBAR_SCREEN_MENU,       DOWNBAR_SCREEN_MENU_HELP       },
    { PART_EDITOR_MENU,          PART_EDITOR_MENU_HELP          },
    { MACHINE_EDITOR_MENU,       MACHINE_EDITOR_MENU_HELP       },
    { BOARD_MONITOR_MENU,        BOARD_MONITOR_MENU_HELP        },
    { MONITOR_CONFIG_MENU,       MONITOR_CONFIG_MENU_HELP       },
    { BACKUP_AND_RESTORE_MENU,   BACKUP_AND_RESTORE_MENU_HELP   },
    { SETUPS_MENU,               SETUPS_MENU_HELP               },
    { DOWNTIME_MENU,             DOWNTIME_MENU_HELP             },
    { EDITDOWN_MENU,             EDITDOWN_MENU_HELP             },
    { SURETRAK_CONTROL_MENU,     SURETRAK_CONTROL_MENU_HELP     },
    { OPERATOR_SIGNON_MENU,      OPERATOR_SIGNON_MENU_HELP      },
    { EXPORT_SHOTS_MENU,         EXPORT_SHOTS_MENU_HELP         },
    { ALARM_SUMMARY_MENU,        ALARM_SUMMARY_MENU_HELP        },
    { PASSWORDS_MENU,            PASSWORDS_MENU_HELP            },
    { RESTORE_PASSWORD_MENU,     RESTORE_PASSWORD_MENU_HELP     },
    { HELP_MENU,                 HELP_MENU_HELP                 },
    { UNZOOM_MENU,               UNZOOM_MENU_HELP               },
    { ZOOM_MENU,                 ZOOM_MENU_HELP                 },
    { DISPLAY_CONFIGURATION_MENU, DISPLAY_CONFIGURATION_MENU_HELP },
    { COLOR_DEFINITION_MENU,     COLOR_DEFINITION_MENU_HELP     },
    { CHOOSE_RT_MACHINES_MENU,   CHOOSE_RT_MACHINES_MENU_HELP   },
    { CORNER_PARAMETERS_MENU,    CORNER_PARAMETERS_MENU_HELP    },
    { OVERLAY_MENU,              OVERLAY_MENU_HELP              },
    { TREND_REALTIME_MENU,       TREND_REALTIME_MENU_HELP       },
    { TREND_ALLSHOTS_MENU,       TREND_ALLSHOTS_MENU_HELP       },
    { TREND_CONFIG_MENU,         TREND_CONFIG_MENU_HELP         },
    { STAT_SUMMARY_MENU,         STAT_SUMMARY_MENU_HELP         },
    { NO_STAT_SUMMARY_MENU,      NO_STAT_SUMMARY_MENU_HELP      },
    { VISIEDIT_MENU,             VISIEDIT_MENU_HELP             },
    { CLEAR_BACKUP_LIST_MENU,    CLEAR_BACKUP_LIST_MENU_HELP    },
    { ADD_TO_BACKUP_LIST_MENU,   ADD_TO_BACKUP_LIST_MENU_HELP   },
    { ALARM_LOG_SETUP_MENU,      ALARM_LOG_SETUP_MENU_HELP      },
    { STATUS_SCREEN_MENU,        STATUS_SCREEN_MENU_HELP        },
    { EDIT_STATUS_SCREEN_MENU,   EDIT_STATUS_SCREEN_MENU_HELP   },
    { SAVE_STATUS_SCREEN_MENU,   SAVE_STATUS_SCREEN_MENU_HELP   },
    { FONT_STATUS_SCREEN_MENU,   FONT_STATUS_SCREEN_MENU_HELP   },
    { EXPORT_ALARMS_MENU,        EXPORT_ALARMS_MENU_HELP        },
    { XDCR_MENU,                 XDCR_MENU_HELP                 },
    { STANDARD_DOWNTIMES_MENU,   STANDARD_DOWNTIMES_MENU_HELP   },
    { SHIFT_REPORT_MENU,         SHIFT_REPORT_MENU_HELP         },
    { WHATS_NEW_MENU,            WHATS_NEW_MENU_HELP            },
    { ABOUT_BOX_MENU,            ABOUT_BOX_MENU_HELP            }
    };

static int32 n = sizeof(helplist)/sizeof(MENU_HELP_TEXT_ENTRY);
int32 i;

if ( !MenuHelpWindow )
    return;

for ( i=0; i<n; i++ )
    {
    if ( id == helplist[i].id )
        {
        set_text( MenuHelpWindow, resource_string(helplist[i].resource_id) );
        return;
        }
    }

set_text( MenuHelpWindow, EmptyString );
}

/***********************************************************************
*                      START_ALARM_SUMMARY_DIALOG                      *
***********************************************************************/
void start_alarm_summary_dialog()
{
DialogIsActive = TRUE;
DialogBox(
    MainInstance,
    TEXT("ALARM_SUMMARY_DIALOG"),
    MainPlotWindow,
    (DLGPROC) alarm_summary_proc );
}

/***********************************************************************
*                        CHECK_ZOOM_BOX_LABELS                         *
* Check to see if either of the zoom boxes is visible. If so, update   *
* the labels and curve types.                                          *
***********************************************************************/
static void check_zoom_box_labels()
{
HWND w;
w = 0;

if ( IsWindowVisible(ZoomBoxWindow) )
    w = ZoomBoxWindow;
else if ( IsWindowVisible(SmallZoomBoxWindow) )
    w = SmallZoomBoxWindow;
if ( w )
    set_zoom_box_labels( w );
}

/***********************************************************************
*                         FORCE_DOWNTIME_POPUP                         *
***********************************************************************/
static void force_downtime_popup()
{
COMPUTER_CLASS c;

if ( c.find(ComputerName) )
    {
    if ( c.is_this_computer() )
        {
        DowntimeEntryMachine = MachineName;
        ShowWindow( CreateDialog( MainInstance, TEXT("DOWNTIME_ENTRY_DIALOG"), MainWindow, (DLGPROC) downtime_entry_dialog_proc ), SW_SHOW );
        }
    }
}

/***********************************************************************
*                          DELETE_BACKUP_LIST                          *
***********************************************************************/
static void delete_backup_list()
{
NAME_CLASS s;

s.get_backup_list_dbname();
s.delete_file();
}

/***********************************************************************
*                           CHECK_FOR_HUMAN                            *
*                                                                      *
*  The original intent of this is to check for human input and         *
*  restore the default password level if there is no input in the      *
*  specified time. The code for this is commented out below. It        *
*  only works for XP. At the present time the default level is         *
*  restored after a fixed time. If the program is restarted, it        *
*  resets the timer.                                                   *
*                                                                      *
***********************************************************************/
static void check_for_human()
{
//LASTINPUTINFO info;
//DWORD         now;

if ( PasswordLevelTimeoutMs == 0 )
    {
    if ( PasswordLevelTimeoutId != 0 )
        {
        KillTimer( MainWindow, PasswordLevelTimeoutId );
        PasswordLevelTimeoutId = 0;
        return;
        }
    }

/*
-----------------------------------------------------
if ( IsWindowsNT )
    {
    info.cbSize = sizeof( LASTINPUTINFO );
    if ( GetLastInputInfo(&info) )
        {
        now = GetTickCount();
        if ( now > info.dwTime )
            now -= info.dwTime;
        else
            return;
        }
    }
else
    {
    now = PasswordLevelTimeoutMs + 1;
    }

if ( now > PasswordLevelTimeoutMs )
    {
----------------------------------------------------- */

KillTimer( MainWindow, PasswordLevelTimeoutId );
set_default_password_level( false );
PasswordLevelTimeoutId = 0;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
LRESULT CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
const WPARAM VK_P = 0x50;
const WPARAM VK_R = 0x52;
const WPARAM VK_U = 0x55;
const WPARAM VK_V = 0x56;
const WPARAM VK_Z = 0x5A;
int id;
int cmd;

static BOOLEAN was_pressure_display;
static BOOLEAN ShowingAllWindows = FALSE;
static BOOLEAN first_time        = TRUE;
static TEXT_LIST_CLASS t;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_TIMER:
        if ( wParam == PASSWORD_LEVEL_TIMEOUT_TIMER_NUMBER )
            {

            check_for_human();
            return 0;
            }
        break;

    case WM_KEYDOWN:
        if ( wParam == VK_R )
            {
            if ( (MyScreenType != BOX_SCREEN_TYPE) && (MyScreenType != STATUS_SCREEN_TYPE) )
                toggle_realtime();
            }
        else if ( wParam == VK_V )
            {
            do_view_parameters_menu();
            }
        else if ( wParam == VK_P )
            {
            toggle_pressure_display_type();
            SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
            }
        else if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
            {
            if ( wParam == VK_Z && !Zooming )
                {
                do_keyboard_zoom_button();
                return 0;
                }
            else if ( wParam == VK_U && (Zooming || zoomed()) )
                {
                unzoom();
                SetFocus( MainWindow );
                return 0;
                }

            if ( do_zoom_arrow_keys((int) wParam) )
                return 0;
            }
        break;

    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_MENUSELECT:
        show_menu_help_text( (UINT) id );
        return 0;

    case WM_ENTERMENULOOP:
        create_menu_help_box();
        return 0;

    case WM_EXITMENULOOP:
        if ( MenuHelpWindow )
            {
            DestroyWindow( MenuHelpWindow );
            MenuHelpWindow = 0;
            }
        return 0;

    case WM_HELP:
        if ( MyScreenType == TREND_SCREEN_TYPE )
            {
            if ( GridIsVisible )
                gethelp( HELP_CONTEXT, STAT_SUMMARY_HELP );
            else
                gethelp( HELP_CONTEXT, TREND_CHART_HELP );
            }
        else if ( MyScreenType == STATUS_SCREEN_TYPE )
            {
            gethelp( HELP_CONTEXT, STATUS_SCREEN_HELP );
            }
        else
            {
            gethelp( HELP_CONTEXT, PROFILE_MAIN_SCREEN_HELP );
            }

        return 0;

    case WM_ACTIVATEAPP:
            if ( !DialogIsActive && BOOL(wParam) && IsWindowVisible(MainPlotWindow) )
                SetFocus( MainPlotWindow );
            break;

    case WM_ACTIVATE:
        if ( id == WA_INACTIVE && cmd != 0 )
            {
            /*
            ----------------------------------------
            Cmd is zero if the menu is not minimized
            ---------------------------------------- */
            if ( !ShowingAllWindows && IsWindowVisible(MainPlotWindow) )
                {
                hide_all_windows();
                }
            return 0;
            }
        else if ( id != WA_INACTIVE )
            {
            if ( IsIconic(MainPlotWindow)  )
                {
                ShowingAllWindows = TRUE;
                show_all_windows();
                ShowingAllWindows = FALSE;
                }
            return 0;
            }
        break;

    case WM_DBINIT:
        /*
        -------------------------------------------------------------
        If I ask for too many dde conects without getting my messages
        things come apart, so I spread them out as per below.
        ------------------------------------------------------------- */
        if ( first_time )
            {
            first_time = FALSE;
            if ( redisplay_last_shot() )
                {
                SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
                }
            else
                {
                create_chospart_window();
                }

            load_main_menu();
            set_zoom_grayness();
            client_dde_startup( hWnd );
            register_for_event( DDE_CONFIG_TOPIC,  MACH_SETUP_INDEX, new_machine_setup_callback );
            register_for_event( DDE_MONITOR_TOPIC, NEW_MACH_STATE_INDEX, new_machine_state_callback );
            register_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX,      down_data_callback );
            register_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX,  monitor_setup_callback );
            register_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX,        password_level_callback );

            MachList.rewind();
            while ( MachList.next() )
                t.append( MachList.name() );
            t.rewind();
            PostMessage( hWnd, WM_DBINIT, 0, 0L );
            }
        else
            {
            if ( t.next() )
                {
                register_for_event( t.text(), SHOT_NUMBER_INDEX, new_shot_callback );
                register_for_event( t.text(), PART_SETUP_INDEX,  new_part_setup_callback );
                PostMessage( hWnd, WM_DBINIT, 0, 0L );
                }
            else
                {
                t.empty();
                get_current_password_level();
                }
            }
        return 0;

    case WM_EV_SHUTDOWN:
        PostMessage( MainPlotWindow, WM_CLOSE, 0, 0L );
        PostMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        save_last_shot_name();
        PostQuitMessage( 0 );
        return 0;

    case WM_NEWPART:
        HaveHeader = FALSE;
        if ( OverlayMenuState )
            NofOverlayShots = 0;
        load_part_info();
        reset_plot_curves();

        if ( MyScreenType == TREND_SCREEN_TYPE )
            {
            get_current_shot();
            trend_screen_reset();
            }
        else if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
            {
            get_current_shot();
            do_new_dtr_config();
            }
        else
            {
            create_chosshot_window();
            }
        return 0;

    case WM_NEW_TC_CONFIG:
        do_new_tc_config( wParam );
        return 0;

    case WM_NEW_CL_CONFIG:
        do_new_control_limits();
        return 0;

    case WM_NEW_DTR_CONFIG:
        do_new_dtr_config();
        return 0;

    case WM_NEWSHOT:
        do_newshot();
        return 0;

    case WM_NEWSETUP:
        do_newsetup();
        return 0;

    case WM_NEW_DATA:
        if ( wParam == LOAD_MAIN_MENU_REQ )
            load_main_menu();
        if ( wParam == SCREEN_REFRESH_REQ )
            InvalidateRect( MainPlotWindow, 0, TRUE );
        return 0;

    case WM_KEYUP:
        if ( wParam == VK_SNAPSHOT && SnapshotIsActive )
            {
            if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
                {
                QuickPrint = TRUE;
                print_me();
                /*
                -------------------------------------
                Empty this picture from the clipboard
                ------------------------------------- */
                if ( OpenClipboard(hWnd) )
                    {
                    EmptyClipboard();
                    CloseClipboard();
                    }
                return 0;
                }
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case HELP_MENU:
                gethelp( HELP_CONTEXT, TABLE_OF_CONTENTS_HELP );
                return 0;

            case EXIT_MENU:
                //DialogIsActive = TRUE;
                //DialogBox( MainInstance, TEXT("SHUT_DOWN_DIALOG"), MainWindow, (DLGPROC) shut_down_dialog_proc );
                if ( boolean_from_ini(DisplayIniFile, ConfigSection, ExitAllDefaultKey) )
                    {
                    /*
                    -----------------------------------------------------------------------
                    Tell eventman to shut down. This will shut down everyone, including me.
                    ----------------------------------------------------------------------- */
                    poke_data( DDE_EXIT_TOPIC,  ItemList[NEW_MACH_STATE_INDEX].name, UNKNOWN );
                    }
                else
                    {
                    PostMessage( MainPlotWindow, WM_DESTROY, 0, 0L );
                    PostMessage( MainWindow,     WM_CLOSE,   0, 0L );
                    }
                return 0;

            case MINIMIZE_MENU:
                poke_data( DDE_ACTION_TOPIC, ItemList[PLOT_MIN_INDEX].name, TEXT("0") );
                CloseWindow( MainWindow );
                return 0;

            case SAVE_MENU:
                toggle_profile_save_flags();
                InvalidateRect( ParamHistoryWindow, 0, TRUE );
                return 0;

            case SAVE_AS_MASTER_MENU:
                save_as_master_trace();
                return 0;

            case POS_TIME_MENU:
                set_current_display( POS_TIME_DISPLAY );
                InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case POS_ONLY_MENU:
                set_current_display( POS_ONLY_DISPLAY );
                InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case TIME_ONLY_MENU:
                set_current_display( TIME_ONLY_DISPLAY );
                InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case LAST_50_SCREEN_MENU:
                update_screen_type( ZABISH_SCREEN_TYPE );
                return 0;

            case STD_SCREEN_MENU:
                update_screen_type( STANDARD_SCREEN_TYPE );
                return 0;

            case PROFILE_SCREEN_MENU:
                update_screen_type( PROFILE_SCREEN_TYPE );
                return 0;

            case BOX_SCREEN_MENU:
                if ( MyScreenType != BOX_SCREEN_TYPE )
                    {
                    update_screen_type( BOX_SCREEN_TYPE );
                    }
                return 0;

            case STATUS_SCREEN_MENU:
                update_screen_type( STATUS_SCREEN_TYPE );
                return 0;

            case EDIT_STATUS_SCREEN_MENU:
                Editing_Status_Screen = TRUE;
                return 0;

            case SAVE_STATUS_SCREEN_MENU:
                save_status_config();
                return 0;

            case FONT_STATUS_SCREEN_MENU:
                choose_status_screen_font();
                return 0;

            case TREND_CONFIG_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("TREND_CONFIG_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) trend_config_dialog_proc );
                return 0;

            case DOWNBAR_CONFIG_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("DOWNTIME_REPORT_SETUP_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) downtime_report_setup_proc );
                return 0;

            case CHOOSE_DOWNTIME_MACHINE_MENU:
                if ( ChooseDowntimeMachineWindow )
                    SendMessage( ChooseDowntimeMachineWindow, WM_DBINIT, 0, 0L );
                return 0;

            case TREND_REALTIME_MENU:
                trend_realtime();
                return 0;

            case DOWNBAR_SCREEN_MENU:
                copystring( DowntimeComputerName, ComputerName );
                copystring( DowntimeMachineName,  MachineName );
                update_screen_type( DOWNBAR_SCREEN_TYPE );
                return 0;

            case TREND_SCREEN_MENU:
                update_screen_type( TREND_SCREEN_TYPE );
                return 0;

            case PRINT_MENU:
                turn_off_zooming();
                print_me();
                return 0;

            case MULTIPLE_SHOT_PRINT_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("PRINT_MULTIPLE_SHOTS_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) print_multiple_shots_proc );
                return 0;

            case UNZOOM_MENU:
                unzoom();
                SetFocus( MainWindow );
                return 0;

            case ZOOM_MENU:
                if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == STATUS_SCREEN_TYPE )
                    return 0;
                if ( !Zooming )
                    do_keyboard_zoom_button();
                SetFocus( MainWindow );
                return 0;

            case XDCR_MENU:
                set_info_box_type( XDCR_INFO_BOX );
                return 0;

            case REAL_TIME_MENU:
            case REAL_TIME_MAIN_MENU:
                toggle_realtime();
                return 0;

            case OVERLAY_MENU:
                do_overlay_toggle( id );
                return 0;

            case VIEW_REFERENCE_TRACE_MENU:
                turn_off_zooming();
                toggle_menu_view_state( ViewReferenceMenuState, id );
                InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case VIEW_PARAMETERS_MENU:
                do_view_parameters_menu();
                return 0;

            case COLOR_DEFINITION_MENU:
                ShowWindow( ColorDefinitionDialogWindow, SW_SHOW );
                SetFocus( ColorDefinitionDialogWindow );
                return 0;

            case CHOOSE_PART_MENU:
                turn_off_zooming();
                if ( MyScreenType == BOX_SCREEN_TYPE )
                    update_screen_type( STANDARD_SCREEN_TYPE );
                create_chospart_window();
                return 0;

            case CHOOSE_SHOT_MENU:
                turn_off_zooming();
                create_chosshot_window();
                return 0;

            case CHOOSE_RT_MACHINES_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("REALTIME_MACHINE_LIST"),
                    MainWindow,
                   (DLGPROC) realtime_machine_list_proc );
                return 0;

            case CORNER_PARAMETERS_MENU:
                choose_corner_parameters();
                return 0;

            case DISPLAY_CONFIGURATION_MENU:
                turn_off_zooming();
                DialogIsActive = TRUE;
                was_pressure_display = IsPressureDisplay;

                DialogBox(
                    MainInstance,
                    TEXT("DISPLAY_CONFIGURATION"),
                    MainWindow,
                   (DLGPROC) display_configuration_dialog_proc );

                check_zoom_box_labels();
                InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case CLEAR_BACKUP_LIST_MENU:
                delete_backup_list();
                return 0;

            case ADD_TO_BACKUP_LIST_MENU:
                add_to_backup_list();
                return 0;

            case WHATS_NEW_MENU:
                gethelp( HELP_CONTEXT, WHATS_NEW_HELP );
                return 0;

            case FT2_HELP_MENU:
                get_ft2_help();
                return 0;

            case MACHINE_LIST_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("MACHINE_LIST_DIALOG"),
                    MainWindow,
                    (DLGPROC) machine_list_dialog_proc );
                return 0;

            case ABOUT_BOX_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("ABOUT_DIALOG"),
                    MainWindow,
                    (DLGPROC) about_dialog_proc );
                return 0;

            case TREND_ALLSHOTS_MENU:
                trend_allshots();
                return 0;

            case STAT_SUMMARY_MENU:
            case NO_STAT_SUMMARY_MENU:
                toggle_current_display_type();
                return 0;

            case PART_EDITOR_MENU:
            case MACHINE_EDITOR_MENU:
            case BOARD_MONITOR_MENU:
            case MONITOR_CONFIG_MENU:
            case SENSORS_MENU:
            case BACKUP_AND_RESTORE_MENU:
            case SETUPS_MENU:
            case DOWNTIME_MENU:
            case EDITDOWN_MENU:
            case SURETRAK_CONTROL_MENU:
            case VISIEDIT_MENU:
            case STANDARD_DOWNTIMES_MENU:
            case SHIFT_REPORT_MENU:
                turn_off_zooming();
                run_tool( id );
                return 0;

            case OPERATOR_SIGNON_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("OPERATOR_SIGNON_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) operator_signon_proc );
                return 0;

            case EXPORT_SHOTS_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("EXPORT_SHOTS_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) export_shots_proc );
                return 0;

            case ALARM_SUMMARY_MENU:
                start_alarm_summary_dialog();
                return 0;

            case ALARM_LOG_SETUP_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("ALARM_LOG_SETUP_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) alarm_log_setup_proc );
                return 0;

            case DOWNTIME_POPUP_MENU:
                force_downtime_popup();
                return 0;

            case PASSWORDS_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("PASSWORD_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) enter_password_proc );
                return 0;

            case RESTORE_FOCUS_TO_HMI_MENU:
                restore_focus_to_hmi();
                return 0;

            case RESTORE_PASSWORD_MENU:
                set_default_password_level( true );
                return 0;

            case EDIT_SHIFTS_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                   MainInstance,
                   TEXT("EDIT_SHIFTS_DIALOG"),
                   NULL,
                   (DLGPROC) edit_shifts_dialog_proc );
                return 0;

            case EXPORT_ALARMS_MENU:
                if ( !DialogIsActive && !ExportAlarmsDialogWindow )
                    {
                    DialogIsActive = TRUE;
                    DialogBox(
                        MainInstance,
                        TEXT("EXPORT_ALARMS_DIALOG"),
                        MainPlotWindow,
                        (DLGPROC) export_alarms_dialog_proc );
                    }
                return 0;

            case CSV_SHOT_FILE_MENU:
                if ( !DialogIsActive )
                    {
                    DialogIsActive = TRUE;
                    DialogBox(
                        MainInstance,
                        TEXT("CSV_SHOT_FILE_DIALOG"),
                        MainPlotWindow,
                        (DLGPROC) csv_shot_file_dialog_proc );
                    }
                return 0;

            }
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                          GET_HISTORY_LENGTH                          *
***********************************************************************/
static void get_history_length()
{
TCHAR  * cp;
short slen;

cp = get_ini_string( VisiTrakIniFile, ConfigSection, TEXT("LastN") );
slen = lstrlen( cp );
if ( slen > 0 )
    HistoryShotCount = extshort( cp, slen );
}

/***********************************************************************
*                           SET_FULL_SCREEN                            *
***********************************************************************/
void set_full_screen()
{
APPBARDATA ab;
UINT       taskbar_state;

IsFullScreen = FALSE;

ab.cbSize = sizeof( APPBARDATA );
ab.hWnd   = 0;
taskbar_state = SHAppBarMessage( ABM_GETSTATE, &ab );

if ( taskbar_state == ABS_ALWAYSONTOP )
    return;

if ( *get_ini_string(DisplayIniFile, ConfigSection, TEXT("HideMenu")) == TEXT('Y') )
    IsFullScreen = TRUE;
}

/***********************************************************************
*                           MAIN_MENU_WIDTH                            *
*                                                                      *
***********************************************************************/
static int main_menu_width()
{
HWND desktop_window;
RECT r;

desktop_window = GetDesktopWindow();
if ( desktop_window )
    {
    GetClientRect( desktop_window, &r );
    return r.right - r.left;
    }

return 160;
}

/***********************************************************************
*                          SAVE_SCREEN_TYPES                           *
***********************************************************************/
static void save_screen_types()
{
TCHAR * cp;

cp = int32toasc( (int32) StandardProfileScreenType );
put_ini_string( DisplayIniFile, ConfigSection, TEXT("LastStdProfileType"), cp );

cp = int32toasc( (int32) MyScreenType );
put_ini_string( DisplayIniFile, ConfigSection, TEXT("LastScreenType"), cp );
}

/***********************************************************************
*                       CHECK_FOR_MONITOR_PROGRAM                      *
***********************************************************************/
static void check_for_monitor_program()
{
NAME_CLASS s;

if ( s.get_exe_dir_file_name( MonallProgramName) )
    HaveMonitorProgram = s.file_exists();
}

/***********************************************************************
*                      LOAD_FROM_NETWORK_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK load_from_network_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
return FALSE;
}

/***********************************************************************
*                        SET_CURRENT_DIRECTORY                         *
***********************************************************************/
void set_current_directory()
{
TCHAR   path[MAX_PATH+1];

if ( get_exe_directory(path) )
    {
    remove_backslash_from_path( path );
    SetCurrentDirectory( path );
    }
}

/***********************************************************************
*                     SET_DATE_I_CAN_SHOW_PRESSURE                     *
*                            Lawyer Code                               *
***********************************************************************/
static void set_date_i_can_show_pressure()
{
const TCHAR mydate[] = TEXT( "03/13/2002" );
SYSTEMTIME st;

init_systemtime_struct( st );
extmdy( st, mydate );

SystemTimeToFileTime( &st, &DateICanShowPressure );
}

/***********************************************************************
*                        PRESSURE_DISPLAY_CHECK                        *
* Check the currently loaded profile shot date to see if I may display *
* pre-impact pressure traces. Lawyer Code.                             *
***********************************************************************/
void pressure_display_check()
{
bool need_refresh = false;

if ( CompareFileTime(&Ph.time_of_shot, &DateICanShowPressure) < 0 )
    {
    if ( !IsHPM )
        {
        if ( OverlayMenuState )
            {
            if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
                {
                if ( NofOverlayShots > 0 )
                    need_refresh = true;
                }
            do_overlay_toggle( OVERLAY_MENU );
            }
        IsHPM = true;
        if ( CurrentDisplay != TIME_ONLY_DISPLAY)
            PostMessage( MainWindow, WM_NEW_DATA, LOAD_MAIN_MENU_REQ, 0L );
        if ( need_refresh )
            PostMessage( MainWindow, WM_NEW_DATA, SCREEN_REFRESH_REQ, 0L );
        }
    }
else
    {
    if ( IsHPM )
        {
        IsHPM = false;
        if ( CurrentDisplay != TIME_ONLY_DISPLAY)
            PostMessage( MainWindow, WM_NEW_DATA, LOAD_MAIN_MENU_REQ, 0L );
        }
    }
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( int cmd_show )
{
WNDCLASS wc;
COMPUTER_CLASS c;
int h;
int i;
int w;
int y;

RECTANGLE_CLASS r;
HWND desktop_window;
TCHAR * cp;
static DWORD status;

RealTime               = TRUE;
HaveHeader             = FALSE;
OverlayMenuState       = FALSE;
ViewReferenceMenuState = FALSE;
NofOverlayShots        = 0;

set_date_i_can_show_pressure();

LoadFromNetworkDialogWindow = 0;
names_startup();
c.startup();
if ( c.count() > 1 )
    LoadFromNetworkDialogWindow = CreateDialog( MainInstance, TEXT("LOAD_FROM_NETWORK_DIALOG"), 0, (DLGPROC) load_from_network_dialog_proc );
set_current_directory();

*DowntimeComputerName = NullChar;
*DowntimeMachineName  = NullChar;

load_machlist();
load_realtime_machine_list();
get_history_length();
read_corner_parameters( CornerParameters, NofCornerParameters );
lstrcpy( ComputerName, c.whoami() );
units_startup();
dcurve_startup();
shifts_startup();
sensor_list_startup();
init_global_curve_types();
check_for_monitor_program();

cp = get_ini_string( DisplayIniFile, ConfigSection, MainFontNameKey );
if ( !unknown(cp) )
    {
    ArialFontName      = cp;
    ArialBoldFontName  = cp;
    TimesRomanFontName = cp;
    }
else
    {
    ArialFontName      = DefaultArialFontName;
    ArialBoldFontName  = DefaultArialBoldFontName;
    TimesRomanFontName = DefaultTimesRomanFontName;
    }

/*
----------------------------------------------
Load the downtime categories and subcategories
---------------------------------------------- */
Cats.read();
SubCats.read();

HaveSureTrakControl = boolean_from_ini( VisiTrakIniFile, ConfigSection, HaveSureTrakKey     );
UseDowntimeEntry    = boolean_from_ini( VisiTrakIniFile, ConfigSection, UseDowntimeEntryKey );
PartBasedDisplay    = boolean_from_ini( VisiTrakIniFile, ConfigSection, PartBasedDisplayKey );

/*
----------------------------
Get the standard screen type
---------------------------- */
cp = get_ini_string( DisplayIniFile, ConfigSection, TEXT("LastStdProfileType") );
if ( !unknown(cp) )
    StandardProfileScreenType = (short) asctoint32( cp );
/*
---------------------------
Get the width of the curves
--------------------------- */
cp = get_ini_string( DisplayIniFile, ConfigSection, CurvePenWidthKey );
if ( !unknown(cp) )
    CurvePenWidth = (int) asctoint32( cp );

/*
------------------------------------------------------------------------------
If the snapshot key is active then pressing printscreen prints to the printer.
The default is yes.
------------------------------------------------------------------------------ */
if ( !boolean_from_ini(DisplayIniFile, ConfigSection, SnapshotKey) )
    SnapshotIsActive = FALSE;

if ( LoadFromNetworkDialogWindow )
    DestroyWindow( LoadFromNetworkDialogWindow );

LoadFromNetworkDialogWindow = 0;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("PROFILE")  );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

wc.lpszClassName = MainPlotWindowClass;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_plot_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = 0;
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

if ( !RegisterClass(&wc) )
    status = GetLastError();

wc.lpszClassName = XdcrWindowClass;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) xdcr_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = 0;
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

if ( !RegisterClass(&wc) )
    status = GetLastError();

/*
--------------------------------------------
Regisger the corner parameter display window
-------------------------------------------- */
wc.lpszClassName = ParameterWindowClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) parameter_window_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

w = main_menu_width();

/*
---------------------------------------------------------------
If this is a touchscreen display I need to make the menu bigger
--------------------------------------------------------------- */
h = GetSystemMetrics( SM_CYMENU );
y = -2;

if ( boolean_from_ini( DisplayIniFile, ConfigSection, HaveTouchScreenKey) )
    {
    y = 0;
    }

MainWindow = CreateWindowEx(
    WS_EX_APPWINDOW,
    MyClassName,
    MyWindowTitle,
    WS_POPUP,
    0, y,                       /* X,y */
    w, h,                       /* W,h */
    NULL,
    NULL,
    MainInstance,
    NULL
    );

GetWindowRect( MainWindow, &MainWindowRect );

/*
------------------------------------
Get the bounds of the desktop window
------------------------------------ */
desktop_window = GetDesktopWindow();
if ( desktop_window )
    GetClientRect( desktop_window, &r );
else
    r = MainWindowRect;

w = r.width();
h = r.height();

set_full_screen();

/*
---------------------------------------------------
Create a popup window that covers the entire screen
CreateWindow(
--------------------------------------------------- */
MainPlotWindow = CreateWindowEx(
    WS_EX_TOOLWINDOW,
    MainPlotWindowClass,               // window class name
    0,                                 // window title
    WS_POPUP,
    0, 0,
    w, h,                              // W,H
    MainWindow,
    NULL,
    MainInstance,
    NULL
    );

/*
------------------------------------------
Create the Corner Parameter Display Window
------------------------------------------ */
ParameterWindow = CreateWindowEx(
    WS_EX_TOOLWINDOW,
    ParameterWindowClassName,           // window class name
    0,                                  // window title
    WS_POPUP,
    0, 0,
    w, 200,                              // W,H
    MainPlotWindow, //NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateWindowEx(
    WS_EX_TOOLWINDOW,
    XdcrWindowClass,                   // window class name
    0,                                 // window title
    WS_POPUP,
    0, 0,
    w, h,                              // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainPlotWindow )
    status = GetLastError();

for ( i=0; i<NOF_REGIONS; i++ )
    {
    Region[i].is_visible = TRUE;
    }

clear_axes();
restore_colors();

set_current_display( POS_TIME_DISPLAY );

PrinterMargin.left   = 0;
PrinterMargin.right  = 500;    /* mils */
PrinterMargin.top    = 0;
PrinterMargin.bottom = 500;

InfoBoxType = PARAMETER_INFO_BOX;

ChooseDowntimeMachineWindow = CreateDialog(
    MainInstance,
    TEXT("CHOOSE_DOWNTIME_MACHINE_DIALOG"),
    MainPlotWindow,
    (DLGPROC) choose_downtime_machine_dialog_proc );
ShowWindow( ChooseDowntimeMachineWindow, SW_HIDE );

ZoomBoxWindow  = CreateDialog(
    MainInstance,
    TEXT("ZOOM_BOX_DIALOG"),
    MainPlotWindow,
    (DLGPROC) ZoomBoxProc );

SmallZoomBoxWindow  = CreateDialog(
    MainInstance,
    TEXT("SMALL_ZOOM_BOX_DIALOG"),
    MainPlotWindow,
    (DLGPROC) SmallZoomBoxProc );

ParamHistoryWindow  = CreateDialog(
    MainInstance,
    TEXT( "PARAM_HISTORY_DIALOG" ),
    MainPlotWindow,
    (DLGPROC) ParamHistoryProc );

ViewParametersDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("VIEW_PARAMETERS_DIALOG"),
    MainWindow,
    (DLGPROC) view_parameters_dialog_proc );

ColorDefinitionDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("COLOR_DEFINITION_DIALOG"),
    MainWindow,
    (DLGPROC) color_definition_dialog_proc );

ShowWindow( ChospartWindow, SW_SHOW );
ShowWindow( ViewParametersDialogWindow, SW_HIDE );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );

if ( IsIconic(MainWindow) )
    cmd_show = SW_MINIMIZE;

position_param_history_window();
position_main_plot_window();
position_zoom_box_window();
position_view_parameters_window();

if ( cmd_show == SW_MINIMIZE )
    CloseWindow( MainWindow );
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown()
{
COMPUTER_CLASS c;

if ( MyFont )
    {
    DeleteObject( MyFont );
    MyFont = NULL;
    }

if ( PrinterFont )
    {
    DeleteObject( PrinterFont );
    PrinterFont = NULL;
    }

if ( RealTimeFont )
    {
    DeleteObject( RealTimeFont );
    RealTimeFont = NULL;
    }

unregister_for_event( DDE_CONFIG_TOPIC,  MACH_SETUP_INDEX );
unregister_for_event( DDE_MONITOR_TOPIC, NEW_MACH_STATE_INDEX );
unregister_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX );
unregister_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX );
unregister_for_event( DDE_CONFIG_TOPIC,  PW_LEVEL_INDEX      );

unregister_for_new_shot_events();
client_dde_shutdown();

Cats.cleanup();
SubCats.cleanup();

save_screen_types();
dcurve_shutdown();
shifts_shutdown();
units_shutdown();
sensor_list_shutdown();
c.shutdown();
save_colors();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL  status;

if ( is_previous_instance(MyClassName, 0) )
    return 0;

MainInstance = this_instance;

init( cmd_show );

AccelHandle = LoadAccelerators( this_instance, AccelName );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status && ChospartWindow )
        status = IsDialogMessage( ChospartWindow, &msg );

    if ( !status && ChosshotWindow )
        status = IsDialogMessage( ChosshotWindow, &msg );

    if ( !status && ViewParametersDialogWindow )
        status = IsDialogMessage( ViewParametersDialogWindow, &msg );

    if ( !status && ParamHistoryWindow )
        status = IsDialogMessage( ParamHistoryWindow, &msg );

    if ( !status && ColorDefinitionDialogWindow )
        status = IsDialogMessage( ColorDefinitionDialogWindow, &msg );

    if ( !status && DowntimeEntryDialog )
        status = IsDialogMessage( DowntimeEntryDialog, &msg );

    if ( !status && ChooseDowntimeMachineWindow )
        status = IsDialogMessage( ChooseDowntimeMachineWindow, &msg );

    if ( !status && MainPlotWindow )
        status = TranslateAccelerator( MainPlotWindow, AccelHandle, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
