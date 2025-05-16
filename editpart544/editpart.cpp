#include <windows.h>
#include <commctrl.h>
#include <math.h>
#include <ugctrl.h>

#include "..\include\visiparm.h"
#include "..\include\asensor.h"
#include "..\include\chaxis.h"
#include "..\include\color.h"
#include "..\include\dbclass.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\structs.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\machine.h"
#include "..\include\mrunlist.h"
#include "..\include\param.h"
#include "..\include\param_index_class.h"
#include "..\include\part.h"
#include "..\include\marks.h"
#include "..\include\fileclas.h"
#include "..\include\ftanalog.h"
#include "..\include\stparam.h"
#include "..\include\stpres.h"
#include "..\include\stsetup.h"
#include "..\include\plotclas.h"
#include "..\include\rectclas.h"
#include "..\include\runlist.h"
#include "..\include\textlen.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\v5help.h"
#include "..\include\warmupcl.h"
#include "..\include\wclass.h"
#include "..\include\workclas.h"

#include "..\include\ftii.h"
#include "..\include\ftii_file.h"

#define _MAIN_
#include "PartData.h"
#include "resource.h"
#include "resrc1.h"
#include "..\include\events.h"

#define FIRST_TAB_STRING     1

#define OPEN_CHOICE          102

#define NO_TAB              -1
#define BASIC_SETUP_TAB      0
#define ADVANCED_SETUP_TAB   1
#define PARAMETER_LIMITS_TAB 2
#define USER_DEFINED_TAB     3
#define PROFILE_MARKS_TAB    4
#define FASTRAK_CHANNELS_TAB 5
#define SURETRAK_PROFILE_TAB 6   // If there is no suretrak then tab 6 = setup sheet
#define PRESSURE_CONTROL_TAB 7   // If there is suretrak contrtol but no pressure control then tab 7 is the setup sheet
#define SETUP_SHEET_TAB      8
#define TAB_COUNT            9

#define MAX_TAB_LEN   25
#define HAS_PART_PREFIX_LEN 3

struct HELP_ENTRY
    {
    UINT id;
    DWORD context;
    };

struct FULL_HELP_ENTRY
    {
    UINT  id;
    UINT  type;
    DWORD context;
    };

struct MY_WINDOW_INFO
        {
        DLGPROC procptr;
        HWND    dialogwindow;
        short   password_level;
        };

extern GENERIC_LIST_CLASS MachineList;

HACCEL  AccelHandle;
TCHAR   AccelName[] = TEXT("EDITPART_HOT_KEYS");

HINSTANCE MainInstance;
HWND      MainWindow                  = 0;

HWND      MainDialogWindow            = 0;
HWND      ParamDialogWindow           = 0;
HWND      AddPartDialogWindow         = 0;
HWND      CopyToDialogWindow          = 0;
HWND      CopyMultipleDialogWindow    = 0;
HWND      AddMarksDialogWindow        = 0;
HWND      SuretrakProfileWindow       = 0;
HWND      ExtendedChannelsDialog      = 0;

LISTBOX_CLASS ParamLimitLb;

/*
--------
SureTrak
-------- */
static BOOLEAN                 SureTrakHasChanged = FALSE;
static BOOLEAN                 HaveSureTrakPlot = FALSE;
static WINDOW_CLASS            SureTrakPlotWindow;
static PLOT_CLASS              SureTrakPlot;
static SURETRAK_SETUP_CLASS    SureTrakSetup;
       SURETRAK_PARAM_CLASS    SureTrakParam;
static PRESSURE_CONTROL_CLASS  PressureControl;
static WARMUP_CLASS            Warmup;
static BOOLEAN                 LoadingSureTrakSteps;
static BOOLEAN                 LoadingPressureControl;

/*
--------------
Ftii Constants
-------------- */
static const int FTII_VACUUM_WAIT_INPUT = 58;
static const int FTII_LOW_IMPACT_INPUT  = 60;

/*
----
Bits
---- */
static const unsigned Bit31 = 0x80000000;
static const unsigned Bit30 = 0x40000000;

/*
-----------------
Multipart runlist
----------------- */
static BOOLEAN                HaveMultipartRunlist = FALSE;

static const COLORREF BlackColor = RGB( 0,   0, 0 );
static const COLORREF RedColor   = RGB( 255, 0, 0 );

static BOOLEAN IsOfficeWorkstation = FALSE;

/*
--------------------------------
These are initialized in startup
-------------------------------- */
static       COLORREF EboxBackgroundColor = 0;
static       HBRUSH   EboxBackgroundBrush = 0;

static const int32    ST_POS_AXIS_ID = 1;
static const int32    ST_VEL_AXIS_ID = 2;
static const int32    ST_TIME_AXIS_ID = 1;
static const int32    ST_PRES_AXIS_ID = 2;

static HFONT MyFont;

HWND      TabControl;

TCHAR     EmptyString[]                 = TEXT( "" );
TCHAR     Ft2ClassName[]                = TEXT( "FASTRAK2" );
TCHAR     MyClassName[]                 = TEXT( "EditPart" );
TCHAR     SureTrakPlotWindowClassName[] = TEXT( "SureTrakPlotWindow" );
TCHAR     EditPartIniFile[]             = TEXT( "editpart.ini" );
TCHAR     SureTrakIniFile[]             = TEXT( "suretrak.ini" );
TCHAR     VisiTrakIniFile[]             = TEXT( "visitrak.ini" );
TCHAR     ConfigSection[]               = TEXT( "Config" );
TCHAR     MultipartSetupSection[]       = TEXT( "Multipart" );
TCHAR     CurrentTabKey[]               = TEXT( "CurrentTab" );
TCHAR     HavePressureControlKey[]      = TEXT( "HavePressureControl" );
TCHAR     HaveSureTrakKey[]             = TEXT( "HaveSureTrak" );
TCHAR     HaveMultipartRunlistKey[]     = TEXT( "HaveMultipartRunlist" );
TCHAR     IsoPageOneKey[]               = TEXT( "IsoPageOne" );
TCHAR     IsoPageTwoKey[]               = TEXT( "IsoPageTwo" );
TCHAR     LastMachineKey[]              = TEXT( "LastMachine" );
TCHAR     LastPartKey[]                 = TEXT( "LastPart" );
TCHAR     MonitorLogFileKey[]           = TEXT( "MonitorLogFile" );
TCHAR     MonitorLogFormatKey[]         = TEXT( "MonitorLogFormat" );
TCHAR     ParameterTabEditLevelKey[]    = TEXT( "ParameterTabEditLevel" );
TCHAR     PrintParametersKey[]          = TEXT( "PrintParameters" );
TCHAR     SaveAllPartsLevelKey[]        = TEXT( "SaveAllPartsLevel" );
TCHAR     SaveChangesLevelKey[]         = TEXT( "SaveChangesLevel" );
TCHAR     ShowSetupSheetKey[]           = TEXT( "ShowSetupSheet" );
TCHAR     SetupSheetLevelKey[]          = TEXT( "SetupSheetLevel" );
TCHAR     EditOtherComputerKey[]        = TEXT( "EditOtherComputerLevel" );
TCHAR     EditOtherCurrentPartKey[]     = TEXT( "EditOtherCurrentPartLevel" );
TCHAR     EditOtherSuretrakKey[]        = TEXT( "EditOtherSuretrakLevel" );
TCHAR     EditOtherCurrentSuretrakKey[] = TEXT( "EditOtherCurrentSuretrakLevel" );
TCHAR     EditPressureSetupLevelKey[]   = TEXT( "EditPressureSetupLevel" );
TCHAR     EditPressureProfileLevelKey[] = TEXT( "EditPressureProfileLevel" );
TCHAR     HideSureTrakLimitSwitchWiresKey[] = TEXT("HideSureTrakLimitSwitchWires");
TCHAR     VelocityChangesLevelKey[]     = TEXT( "VelocityChangesLevel" );

MACHINE_CLASS   CurrentMachine;
PART_CLASS      CurrentPart;
PARAMETER_CLASS CurrentParam;
PARAM_INDEX_CLASS CurrentSort;
MARKS_CLASS     CurrentMarks;
FTANALOG_CLASS  CurrentFtAnalog;
FTII_LIMIT_SWITCH_WIRE_CLASS CurrentLimitSwitchWire;
short           CurrentSelectedMark  = 0;
short           CurrentPasswordLevel = 5;
short           DefaultPasswordLevel = 5;
short           EditOtherComputerLevel         = EDIT_OTHER_COMPUTER_LEVEL;
short           EditOtherCurrentPartLevel      = EDIT_OTHER_CURRENT_PART_LEVEL;
short           EditOtherSuretrakLevel         = EDIT_OTHER_COMPUTER_LEVEL;
short           EditOtherCurrentSuretrakLevel  = EDIT_OTHER_CURRENT_SURETRAK_LEVEL;
short           EditPressureSetupLevel         = ENGINEER_PASSWORD_LEVEL;
short           EditPressureProfileLevel       = ENGINEER_PASSWORD_LEVEL;
short           SaveChangesLevel               = ENGINEER_PASSWORD_LEVEL;
short           OriginalAnalogSensor[MAX_FT2_CHANNELS];  /* I fill this when I load the part, check when I save */
BOOLEAN         HaveCurrentPart     = FALSE;
BOOLEAN         HavePressureControl = FALSE;
BOOLEAN         HaveSureTrakControl = FALSE;
BOOLEAN         HideSureTrakLimitSwitchWires = FALSE;
BOOLEAN         ShowEofFS           = FALSE;

static TCHAR * DialogName[TAB_COUNT] = {
              TEXT("BASIC_SETUP"),
              TEXT("ADVANCED_SETUP"),
              TEXT("PARAMETER_LIMITS"),
              TEXT("USER_DEFINED_POSITIONS"),
              TEXT("PROFILE_MARKS"),
              TEXT("FASTRAK_CHANNELS"),
              TEXT("SURETRAK_PROFILE"),
              TEXT("PRESSURE_CONTROL_PROFILE"),
              TEXT("SETUP_SHEET_DIALOG")
              };

static MY_WINDOW_INFO WindowInfo[TAB_COUNT];
static int            CurrentDialogNumber = NO_TAB;
static int            AlarmIndex[NOF_ALARM_LIMIT_TYPES] = { WARNING_MIN, WARNING_MAX, ALARM_MIN, ALARM_MAX };

/*
-------------------------------------------------------------------
If this is true I need to make sure channels 5 and 7 are -10 to +10
This is only used when upgrading from monitor only to st2.
------------------------------------------------------------------- */
static BOOLEAN NeedToCheckChannels5and7 = FALSE;

/*
--------------------------------------
Basic Setup Distance Buttons and Units
-------------------------------------- */
static short   NofDistanceButtons    = 3;
static int     DistanceButton[]      = { INCHES_BUTTON, MM_BUTTON, CM_BUTTON };
static short   DistanceUnits[]       = { INCH_UNITS,    MM_UNITS,  CM_UNITS  };
static BOOLEAN DistanceButtonState[] = {FALSE, FALSE, FALSE };

/*
--------------------------------------
Basic Setup Velocity Buttons and Units
-------------------------------------- */
static int     NofVelocityButtons    = 4;
static int     VelocityButton[]      = { IPS_BUTTON, FPM_BUTTON, CMPS_BUTTON, MPS_BUTTON };
static short   VelocityUnits[]       = { IPS_UNITS,  FPM_UNITS,  CMPS_UNITS,  MPS_UNITS  };
static BOOLEAN VelocityButtonState[] = { FALSE, FALSE, FALSE, FALSE };

/*
--------------------------------------------------------
A list of modified edit controls on the suretrak profile
-------------------------------------------------------- */
static WINDOW_ARRAY_CLASS StModList;
static STRING_CLASS       SaveChangesTitle;
static short              VelocityChangesLevel      = NO_PASSWORD_LEVEL;
static BOOLEAN            NeedToCallVelocityChanges = FALSE;

/*
----------------------------------------------------------------------------
Normally hidden text box to tell operator there are unsaved ST param changes
---------------------------------------------------------------------------- */
static WINDOW_CLASS HaveStModTbox;

static MULTIPART_RUNLIST_CLASS MultipartRunlist;
static BOOLEAN                 MultipartRunlistHasChanged = FALSE;
static STRING_CLASS            NotUsedString;                         /* Used by Multipart Runlist load/save */

/*
-------------------
Missing File Dialog
------------------- */
static STRING_CLASS MissingFileDesc;
static STRING_CLASS MissingFileAction;

/*
-------------------------------------------------------------------------
The number of editable fastrak analog channels is 4 for the fastrak board
and 8 for the ftii. I set this when I load the CurrentMachine.
------------------------------------------------------------------------- */
int Nof_Editable_Channels = MAX_EDITABLE_FT_CHANNELS;

/*
---------------
Printer Globals
--------------- */
HFONT PrinterFont = 0;

struct ADD_MARKS_ENTRY
    {
    TCHAR       parameter_type;
    short       parameter_number;
    };

static  ADD_MARKS_ENTRY AddMarksList[MAX_MARKS];
static  short   NofAddMarks;
static  WNDPROC OldEditProc;

static const TCHAR NullChar    = TEXT( '\0' );
static const TCHAR SpaceChar   = TEXT( ' '  );
static const TCHAR CommaChar   = TEXT( ','  );
static const TCHAR EqualsChar  = TEXT( '='  );
static const TCHAR HChar       = TEXT( 'H'  );
static const TCHAR UChar       = TEXT( 'U'  );
static const TCHAR VChar       = TEXT( 'V'  );
static const TCHAR YChar       = TEXT( 'Y'  );
static const TCHAR PercentChar = TEXT( '%'  );
static const TCHAR PeriodChar  = TEXT( '.'  );
static const TCHAR PoundChar   = TEXT( '#'  );
static const TCHAR TabChar     = TEXT( '\t' );
static const TCHAR PercentString[] = TEXT("%");
static const TCHAR SpaceString[]   = TEXT(" ");
static       TCHAR LowerXString[]  = TEXT("x");
static       TCHAR OneString[]     = TEXT("1");
static       TCHAR ZeroString[]    = TEXT("0");

BOOL CALLBACK calculator_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOLEAN choose_wire( HWND sorc, HWND parent, int output_type, int output_index );
BOOL CALLBACK ParamDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void  load_new_parameter( short new_parameter_index );
int32 draw_vertical_string( HDC dc, TCHAR * sorc, int32 x, int32 y );
void  register_choose_wire_control();
BOOLEAN save_dos_suretrak( PART_CLASS & part, SURETRAK_SETUP_CLASS & setup, SURETRAK_PARAM_CLASS param, HWND w );
static void refresh_velocity_control_plot( void );
static void refresh_pressure_control_plot( void );
static void load_extended_channels();

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
int resource_message_box( UINT msg_id, UINT title_id, UINT boxtype )
{
return resource_message_box( MainInstance, msg_id, title_id, boxtype );
}

/***********************************************************************
*                          IS_REMOTE_MONITOR                           *
***********************************************************************/
static BOOLEAN is_remote_monitor()
{
if ( IsOfficeWorkstation )
    {
    if ( FindWindow(Ft2ClassName,0) )
        return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                     SEND_NEW_MONITOR_SETUP_EVENT                     *
***********************************************************************/
static void send_new_monitor_setup_event()
{
TCHAR       * cp;
RUNLIST_CLASS r;
BOOLEAN       runlist_is_enabled;

/*
---------------------------------------------------------------------------
See if this is the current part. Check the runlist first as the warmup part
might be current.
--------------------------------------------------------------------------- */
if ( r.get(CurrentMachine.computer, CurrentMachine.name) )
    cp = r[0].partname;
else
    cp = CurrentMachine.current_part;

if ( strings_are_equal(cp, CurrentPart.name) )
    {
    if ( HaveMultipartRunlist )
        {
        MultipartRunlist.create_runlist( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        runlist_is_enabled = MultipartRunlist.is_enabled;
        }
    else
        {
        Warmup.create_runlist( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        runlist_is_enabled = Warmup.is_enabled;
        }

    if ( !runlist_is_enabled )
        {
        /*
        --------------------------------------------------------------------------------------------
        Make sure an alternate part has not been left in the current part field of the machset table
        -------------------------------------------------------------------------------------------- */
        cp = current_part_name( CurrentPart.computer, CurrentPart.machine );
        if ( !strings_are_equal(cp, CurrentPart.name) )
            set_current_part_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        }

    if ( CurrentMachine.monitor_flags & MA_MONITORING_ON || is_remote_monitor() )
        poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentMachine.name );
    }
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
    WinHelp( MainWindow, fname, helptype, context );
    }
}

/***********************************************************************
*                          GET_DEFAULT_PRINTER_DC                      *
***********************************************************************/
HDC get_default_printer_dc( void )
{
HDC       dc;
HANDLE    ph;
TCHAR     s[MAX_PATH+1];
TCHAR   * cp;

DWORD     bytes_needed;
HGLOBAL   dh;
DEVMODE * dm;

dc = 0;
dm = 0;

GetProfileString( TEXT("windows"), TEXT("device"), TEXT("..."), s, MAX_PATH );

cp = findchar( CommaChar, s );
if ( cp )
    {
    *cp = NullChar;

    if ( OpenPrinter(s, &ph, 0) )
        {
        bytes_needed = DocumentProperties( MainWindow, ph, s, 0, 0, 0 );
        dh = GlobalAlloc( GHND, bytes_needed );
        if ( dh )
            {
            dm = (DEVMODE *) GlobalLock( dh );
            DocumentProperties( MainWindow, ph, s, dm, 0, DM_OUT_BUFFER );
            if ( dm->dmFields & DM_ORIENTATION )
                {
                dm->dmOrientation = DMORIENT_PORTRAIT;
                DocumentProperties( MainWindow, ph, s, dm, dm, DM_IN_BUFFER | DM_OUT_BUFFER );
                }
            }
        dc = CreateDC( 0, s, 0, dm );
        if ( dh )
            GlobalUnlock( dh );
        ClosePrinter( ph );
        }
    }

return dc;
}

/***********************************************************************
*                         DELETE_PRINTER_FONT                          *
***********************************************************************/
static void delete_printer_font( HDC dc )
{
if ( PrinterFont )
    {
    SelectObject( dc, GetStockObject(SYSTEM_FIXED_FONT) ) ;
    DeleteObject( PrinterFont );
    }
}

/***********************************************************************
*                           SELECT_PRINTER_FONT                        *
***********************************************************************/
static void select_printer_font( HDC dc )
{
if ( PrinterFont )
    SelectObject( dc, PrinterFont ) ;
}

/***********************************************************************
*                           CREATE_PRINTER_FONT                        *
***********************************************************************/
static void create_printer_font( HDC dc, RECT & r )
{
long       height;
long       width;
static TCHAR FontName[] = TEXT( "MS Sans Serif" );

if ( dc )
    {
    delete_printer_font( dc );

    width  = 0;
    height = r.bottom - r.top;
    height /= 66;

    PrinterFont = create_font( dc, width, height, FontName );
    select_printer_font( dc );
    }
}

/***********************************************************************
*                               TEXT_OUT                               *
***********************************************************************/
void text_out( HDC dc, int32 x, int32 y, TCHAR * sorc )
{
TextOut( dc, (int) x, (int) y, sorc, lstrlen(sorc) );
}

/***********************************************************************
*                    PRINT_GLOBAL_SURETRAK_PARAMS                      *
***********************************************************************/
void print_global_suretrak_params( HDC dc, int y, int dx, int dy, RECT & border )
{
RECT r;

r.left = (border.left + border.right + dx) / 2;
r.right = border.right;

SetTextAlign( dc, TA_TOP | TA_LEFT );
text_out( dc, r.left, y, resource_string(SURETRAK_GLOBAL_PARAMETERS_STRING) );

y += dy;
r.top = y + dy/2;
r.bottom = r.top + dy*8;
Rectangle( dc, r.left, r.top-2, r.right, r.bottom+2 );
}

/***********************************************************************
*                       PRINT_FASTRAK_CHANNELS                         *
***********************************************************************/
void print_fastrak_channels( HDC dc, int y, int dx, int dy, RECT & border )
{
const int CHANNEL_NUMBER_INDEX = 0;
const int SENSOR_DESC_INDEX    = 1;
const int PRES_TYPE_INDEX      = 2;
const int NOF_POSITIONS        = 3;
const int MAXLINE = 256;

int  i;
int  width;
int  x[NOF_POSITIONS];
RECT r;
HWND w;
TCHAR  buf[MAXLINE+1];
LISTBOX_CLASS  lb;
TEXT_LEN_CLASS t;

r.left = border.left;

/*
---------------
Print the title
--------------- */
SetTextAlign( dc, TA_TOP | TA_LEFT );
text_out( dc, r.left, y, resource_string(CHANNELS_TAB_STRING) );

w = WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow;

x[CHANNEL_NUMBER_INDEX] = r.left + dx;

get_text( buf, w, FASTRAK_CHANNEL_STATIC, MAXLINE );
width = pixel_width( dc, buf );
x[SENSOR_DESC_INDEX] = x[CHANNEL_NUMBER_INDEX] + width + 2*dx;

x[CHANNEL_NUMBER_INDEX] += width/2;

get_text( buf, w, ANALOG_SENSOR_STATIC, MAXLINE );
t.init( dc );
t.check( buf );

for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    lb.init( w, FT_CHANNEL_1_LISTBOX+i );
    t.check( lb.selected_text() );
    }

x[PRES_TYPE_INDEX] = x[SENSOR_DESC_INDEX] + t.width() + 2*dx;

t.init( dc );
get_text( buf, w, HEAD_PRES_STATIC, MAXLINE );
t.check( buf );

get_text( buf, w, ROD_PRES_STATIC, MAXLINE );
t.check( buf );

r.right = x[PRES_TYPE_INDEX] + t.width() + 2*dx;

y += dy;
r.top = y + dy/2;
r.bottom = r.top + dy*6;
Rectangle( dc, r.left, r.top-2, r.right, r.bottom+2 );

y += dy;

SetTextAlign( dc, TA_TOP | TA_CENTER );
get_text( buf, w, FASTRAK_CHANNEL_STATIC, MAXLINE );
text_out( dc, x[CHANNEL_NUMBER_INDEX], y, buf );

SetTextAlign( dc, TA_TOP | TA_LEFT );
get_text( buf, w, ANALOG_SENSOR_STATIC, MAXLINE );
text_out( dc, x[SENSOR_DESC_INDEX], y, buf );


for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    y += dy;

    /*
    --------------
    Channel number
    -------------- */
    SetTextAlign( dc, TA_TOP | TA_CENTER );
    text_out( dc, x[CHANNEL_NUMBER_INDEX], y, int32toasc(i+1) );

    /*
    -----------
    Sensor type
    ----------- */
    SetTextAlign( dc, TA_TOP | TA_LEFT );
    lb.init( w, FT_CHANNEL_1_LISTBOX+i );
    text_out( dc, x[SENSOR_DESC_INDEX], y, lb.selected_text() );

    /*
    -------------
    Pressure type
    ------------- */
    *buf = NullChar;
    if ( is_checked(w, HEAD_PRES_CHAN_1_BUTTON+i) )
        get_text( buf, w, HEAD_PRES_STATIC, MAXLINE );
    else if ( is_checked(w, ROD_PRES_CHAN_1_BUTTON+i) )
        get_text( buf, w, ROD_PRES_STATIC, MAXLINE );

    if ( *buf != NullChar )
        text_out( dc, x[PRES_TYPE_INDEX], y, buf );
    }
}

/***********************************************************************
*                        PRINT_SURETRAK_PARAMS                         *
***********************************************************************/
static int print_suretrak_params( HDC dc, int y, int dx, int dy, RECT & border )
{
const int MAXLINE = 256;
int    i;
int    id;
int    x1;
int    x;
int    velocity_x;
int    gain_x;
RECT   r;
TCHAR  buf[MAXLINE+1];

r.left  = border.left;
r.right = (border.left + border.right - dx) / 2;

SetTextAlign( dc, TA_TOP | TA_LEFT );
text_out( dc, r.left, y, resource_string(SURETRAK_PARAMETERS_STRING) );

y += dy;
r.top = y + dy/2;
r.bottom = r.top + dy*7;
Rectangle( dc, r.left, r.top-2, r.right, r.bottom+2 );

y += dy;
x = r.left + dx;

lstrcpy( buf, resource_string(VEL_LOOP_GAIN_STRING) );
x1 = x + pixel_width( dc, buf ) + 2 * dx;
text_out( dc, x,  y, buf );
text_out( dc, x1, y, ascii_float(SureTrakSetup.velocity_loop_gain()) );

velocity_x = x + pixel_width(dc, resource_string(BREAK_1_STRING)) + 2*dx;

get_text( buf, WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow, VELOCITY_STATIC, MAXLINE );
gain_x = velocity_x + pixel_width(dc, buf) + 2*dx;

y += dy;
y += dy/2;

text_out( dc, velocity_x, y, buf );
text_out( dc, gain_x,     y, resource_string(GAIN_STRING) );

id = BREAK_1_STRING;
for ( i=0; i<NOF_VEL_GAIN_SETTINGS; i++ )
    {
    y += dy;
    text_out( dc, x,          y, resource_string(id) );
    text_out( dc, velocity_x, y, ascii_float(SureTrakSetup.gain_break_velocity(i)) );
    text_out( dc, gain_x,     y, ascii_float(SureTrakSetup.gain_break_gain(i))     );
    id++;
    }

return (int) r.bottom;
}

/***********************************************************************
*                            PRINT_PARAMETERS                          *
***********************************************************************/
void print_parameters( HDC dc, int y, int dx, int dy, RECT & border )
{
const int MAXLINE       = 256;
const int NOF_POSITIONS =  10;  /* Desc, units, low wire high wire low wire high wire */
const int PARAM_DESC_INDEX      = 0;
const int UNITS_INDEX           = 1;
const int WARN_LOW_INDEX        = 2;
const int WARN_LOW_WIRE_INDEX   = 3;
const int WARN_HIGH_INDEX       = 4;
const int WARN_HIGH_WIRE_INDEX  = 5;
const int ALARM_LOW_INDEX       = 6;
const int ALARM_LOW_WIRE_INDEX  = 7;
const int ALARM_HIGH_INDEX      = 8;
const int ALARM_HIGH_WIRE_INDEX = 9;

struct LABEL_ENTRY {
    int  id;
    UINT flags;
    };

const LABEL_ENTRY Label[NOF_POSITIONS]     = {
        { PARAM_DESC_STATIC, TA_TOP | TA_LEFT   },
        { UNITS_STATIC,      TA_TOP | TA_LEFT   },
        { LOW_STATIC,        TA_TOP | TA_LEFT   },
        { WIRE_STATIC,       TA_TOP | TA_CENTER },
        { HIGH_STATIC,       TA_TOP | TA_LEFT   },
        { WIRE_STATIC,       TA_TOP | TA_CENTER },
        { LOW_STATIC,        TA_TOP | TA_LEFT   },
        { WIRE_STATIC,       TA_TOP | TA_CENTER },
        { HIGH_STATIC,       TA_TOP | TA_LEFT   },
        { WIRE_STATIC,       TA_TOP | TA_CENTER }
        };

TCHAR buf[MAXLINE+1];
int   i;
int   n;
int   n_to_print;
int   x[NOF_POSITIONS];
int   x1;
HWND  w;
RECT  r;
TEXT_LEN_CLASS t;

w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;

x[PARAM_DESC_INDEX] = border.left + dx;

/*
-------------------------------------------
Get the width of the  widest parameter name
------------------------------------------- */
t.init( dc );

get_text( buf, w, PARAM_DESC_STATIC, MAXLINE );
t.check( buf );

n_to_print = 0;
n = CurrentParam.count();
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].has_any_limits() )
        {
        t.check( CurrentParam.parameter[i].name );
        n_to_print++;
        }
    }

x[UNITS_INDEX] = x[PARAM_DESC_INDEX] + t.width() + 2*dx;

/*
----------------------------------------
Get the width of the widest units string
---------------------------------------- */
t.init( dc );
get_text( buf, w, UNITS_STATIC, MAXLINE );
t.check( buf );
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].has_any_limits() )
        t.check( units_name(CurrentParam.parameter[i].units) );
    }

x[WARN_LOW_INDEX]        = x[UNITS_INDEX] + t.width() + 2*dx;

x[WARN_LOW_WIRE_INDEX]   = x[WARN_LOW_INDEX]       + 10*dx;
x[WARN_HIGH_INDEX]       = x[WARN_LOW_WIRE_INDEX]  + 5*dx;
x[WARN_HIGH_WIRE_INDEX]  = x[WARN_HIGH_INDEX]      + 10*dx;
x[ALARM_LOW_INDEX]       = x[WARN_HIGH_WIRE_INDEX] + 5*dx;
x[ALARM_LOW_WIRE_INDEX]  = x[ALARM_LOW_INDEX]      + 10*dx;
x[ALARM_HIGH_INDEX]      = x[ALARM_LOW_WIRE_INDEX] + 5*dx;
x[ALARM_HIGH_WIRE_INDEX] = x[ALARM_HIGH_INDEX]     + 10*dx;

r.left  = border.left;
r.right = x[ALARM_HIGH_WIRE_INDEX] + 4*dx;

SetTextAlign( dc, TA_TOP | TA_LEFT );
text_out( dc, r.left, y, resource_string(PARAMETERS_TAB_STRING) );

y += dy;
r.top = y + dy/2;
r.bottom = r.top + dy*(n_to_print+3);
Rectangle( dc, r.left, r.top-2, r.right, r.bottom+2 );

y += dy;
SetTextAlign( dc, TA_TOP | TA_CENTER );

x1 = x[WARN_LOW_INDEX] + x[ALARM_LOW_INDEX] - dx;
x1 /= 2;

get_text( buf, w, WARN_LIMITS_STATIC, MAXLINE );
text_out( dc, x1, y, buf );

x1 = x[ALARM_LOW_INDEX] + x[ALARM_HIGH_WIRE_INDEX] + 2*dx;
x1 /= 2;

get_text( buf, w, ALARM_LIMITS_STATIC, MAXLINE );
text_out( dc, x1, y, buf );

y += dy;
SetTextAlign( dc, TA_TOP | TA_LEFT );

get_text( buf, w, PARAM_DESC_STATIC, MAXLINE );
text_out( dc, x[PARAM_DESC_INDEX], y, buf );

for ( i=0; i<NOF_POSITIONS; i++ )
    {
    SetTextAlign( dc, Label[i].flags );
    get_text( buf, w, Label[i].id, MAXLINE );
    text_out( dc, x[i], y, buf );
    };

SetTextAlign( dc, TA_TOP | TA_LEFT );
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].has_any_limits() )
        {
        y += dy;
        text_out( dc, x[PARAM_DESC_INDEX],      y, CurrentParam.parameter[i].name );
        text_out( dc, x[UNITS_INDEX],           y, units_name(CurrentParam.parameter[i].units) );
        text_out( dc, x[WARN_LOW_INDEX],        y, ascii_float(CurrentParam.parameter[i].limits[WARNING_MIN].value)      );
        text_out( dc, x[WARN_LOW_WIRE_INDEX],   y, int32toasc(CurrentParam.parameter[i].limits[WARNING_MIN].wire_number) );
        text_out( dc, x[WARN_HIGH_INDEX],       y, ascii_float(CurrentParam.parameter[i].limits[WARNING_MAX].value)      );
        text_out( dc, x[WARN_HIGH_WIRE_INDEX],  y, int32toasc(CurrentParam.parameter[i].limits[WARNING_MAX].wire_number) );
        text_out( dc, x[ALARM_LOW_INDEX],       y, ascii_float(CurrentParam.parameter[i].limits[ALARM_MIN].value)      );
        text_out( dc, x[ALARM_LOW_WIRE_INDEX],  y, int32toasc(CurrentParam.parameter[i].limits[ALARM_MIN].wire_number) );
        text_out( dc, x[ALARM_HIGH_INDEX],      y, ascii_float(CurrentParam.parameter[i].limits[ALARM_MAX].value)      );
        text_out( dc, x[ALARM_HIGH_WIRE_INDEX], y, int32toasc(CurrentParam.parameter[i].limits[ALARM_MAX].wire_number) );
        }
    }
}

/***********************************************************************
*                          PRINT_SETUP_SHEET                           *
***********************************************************************/
static int print_setup_sheet( HDC dc, int x, int y, int dx, int dy )
{
NAME_CLASS   s;
FILE_CLASS   f;
TCHAR      * cp;
RECT         r;
TEXT_LEN_CLASS t;

s = setup_sheet_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
if ( s.file_exists() )
    {
    /*
    -----------------------------------------
    Y is the bottom of the previous rectangle
    ----------------------------------------- */
    y += dy;
    text_out( dc, x, y, resource_string(SETUP_SHEET_TAB_STRING) );
    y += dy + dy/4;

    r.left   = x;
    r.top    = y;
    r.bottom = y;

    t.init( dc );

    if ( f.open_for_read(s.text()) )
        {
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;
            r.bottom += dy;
            t.check( cp );
            }

        r.right = r.left + t.width() + 2*dx;
        r.bottom += 4;
        Rectangle( dc, r.left, r.top, r.right, r.bottom );

        x += dx;
        y += 2;

        f.rewind();
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;
            text_out( dc, x, y, cp );
            y += dy;
            }
        f.close();
        }
    }

return (int) r.bottom;
}

/***********************************************************************
*                         PRINT_SURETRAK_STEPS                         *
***********************************************************************/
void print_suretrak_steps( HDC dc, int y, int dx, int dy, RECT & border )
{
const int MAXLINE = 256;

int  id;
HWND dw;
HWND w;

int  column;
int  dialog_box_char_height;
int  dialog_box_char_width;
int  label_width;
int  label_height;
int  ls_desc_x;
int  ls_value_x;
int  ls_y;
int  step;
int  width;
int  x;

RECT r;
TCHAR  buf[MAXLINE+1];

const int VAC_WAIT_COLUMN      =  3;
const int LI_COLUMN            =  4;
const int LI_FROM_INPUT_COLUMN =  5;

struct SURETRAK_COLUMN_ENTRY
    {
    int     width;
    int     position;
    UINT    align_flags;
    };

SURETRAK_COLUMN_ENTRY sc[] = {
    { 0, 0, TA_TOP | TA_RIGHT  }, /* Accel */
    { 0, 0, TA_TOP | TA_RIGHT  }, /* Target Velocity */
    { 0, 0, TA_TOP | TA_RIGHT  }, /* Ending Position */
    { 0, 0, TA_TOP | TA_CENTER }, /* Vacuum Wait */
    { 0, 0, TA_TOP | TA_RIGHT  }, /* Low Impact Percentage */
    { 0, 0, TA_TOP | TA_CENTER }  /* LI from Input */
    };

const int nof_columns = sizeof(sc)/sizeof(SURETRAK_COLUMN_ENTRY);

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

r.left   = 0;
r.right  = 4;
r.top    = 0;
r.bottom = 8;

MapDialogRect( w, &r );

dialog_box_char_width  = r.right;
dialog_box_char_height = r.bottom;

x      = 4 * dx;
y     += 3 * dy;
id     = ACCEL_STATIC;
for ( column=0; column<nof_columns; column++ )
    {
    GetWindowRect( GetDlgItem(w, id), &r );
    label_width  = r.right - r.left;
    label_height = r.bottom - r.top;

    r.left    = x;
    r.bottom  = y;

    /*
    ----------------------------------------------------------------
    The printer label width is the screen label width times
    the ratio of the printer font to the screen font (plus a fudge).
    ---------------------------------------------------------------- */
    sc[column].width  = label_width * (dx+1);
    sc[column].width /= dialog_box_char_width;
    r.right = r.left + sc[column].width;

    r.top  = label_height * (dy+1);
    r.top /= dialog_box_char_height;
    r.top  = r.bottom - r.top;

    get_text( buf, w, id, MAXLINE );
    DrawText( dc, buf, -1, &r, DT_CENTER | DT_WORDBREAK );

    if ( column == VAC_WAIT_COLUMN || column == LI_FROM_INPUT_COLUMN )
        {
        sc[column].position = r.left + sc[column].width/2;
        }
    else if ( column == LI_COLUMN )
        {
        sc[column].position = r.left + sc[column].width;
        }
    else
        {
        /*
        -------------------------------------------------------------
        Position the numbers 3/4 of the way to the right (right just)
        ------------------------------------------------------------- */
        sc[column].position = r.left + sc[column].width*3/4;
        }

    x = r.right + dx;

    id++;
    }

/*
-------------------------------------
Show the limit switch positions title
------------------------------------- */
SetTextAlign( dc, TA_TOP | TA_LEFT );
get_text( buf, w, LIMIT_SWITCH_POS_STATIC, MAXLINE );

ls_desc_x = x + 3*dx;
text_out( dc, ls_desc_x, y-2*dy, buf );

/*
---------------------------------
Get the width of the widest label
--------------------------------- */
label_width = 0;
for ( column=0; column<MAX_FTII_LIMIT_SWITCHES; column++ )
    {
    width = pixel_width( dc, SureTrakParam.limit_switch_desc(column) );
    if ( width > label_width )
        label_width = width;
    }

/*
---------------------------------------------------------------------
Calculate the position of the limit switch position (right justified)
--------------------------------------------------------------------- */
ls_value_x = ls_desc_x + label_width + 6*dx;

id = ACCEL_1_EDITBOX;
for ( step=0; step<MAX_ST_STEPS; step++ )
    {
    for ( column=0; column<nof_columns; column++ )
        {
        if ( column == VAC_WAIT_COLUMN || column == LI_FROM_INPUT_COLUMN )
            {
            if ( is_checked(w, id) )
                {
                SetTextAlign( dc, sc[column].align_flags );
                text_out( dc, sc[column].position, y, LowerXString );
                }
            }
        else
            {
            dw = GetDlgItem(w, id );
            get_text( buf, dw, MAXLINE );
            if ( *buf != NullChar )
                {
                SetTextAlign( dc, sc[column].align_flags );
                text_out( dc, sc[column].position, y, buf );
                }
            }
        id++;
        }

    if ( step < MAX_ST_LIMIT_SWITCHES || (CurrentMachine.is_ftii && (step<MAX_FTII_LIMIT_SWITCHES)) )
        {
        ls_y = y - dy/2;
        SetTextAlign( dc, TA_TOP | TA_LEFT );
        text_out( dc, ls_desc_x, ls_y, SureTrakParam.limit_switch_desc(step) );

        SetTextAlign( dc, TA_TOP | TA_RIGHT );
        get_text( buf, w, LS1_EDITBOX+step, MAXLINE );
        text_out( dc, ls_value_x, ls_y, buf );
        }

    y += dy;
    }
}

/***********************************************************************
*                            PRINT_HEADER                              *
***********************************************************************/
static void print_header( HDC dc, int y, int dy, RECT & r )
{
TIME_CLASS t;

SetTextAlign( dc, TA_TOP | TA_RIGHT );
text_out( dc, r.right, y, CurrentMachine.name );

y += dy;
text_out( dc, r.right, y, CurrentPart.name );

/*
-----------
Todays Date
----------- */
y += dy;
t.get_local_time();
text_out( dc, r.right, y, t.mmddyyyy() );
}

/***********************************************************************
*                          PRINT_PART_SETUP                            *
***********************************************************************/
static void print_part_setup()
{
const int MAXLINE = 256;
static DOCINFO di = { sizeof(DOCINFO), TEXT("TrueTrak Profile"), 0 };

struct EBOX_TBOX_ENTRY {
    int editbox;
    int textbox;
    };

EBOX_TBOX_ENTRY id[] = {
    { PLUNGER_DIAMETER_EDITBOX,       PLUNGER_DIAMETER_STATIC   },
    { CSFS_RISE_VELOCITY_EDITBOX,     VEL_FOR_RISE_STATIC       },
    { SLEEVE_FILL_DISTANCE_EDITBOX,   P1_TO_P3_STATIC           },
    { TIME_FOR_INTENS_PRES_EDITBOX,   TIME_FOR_INTENS_STATIC    },
    { RUNNER_FILL_DISTANCE_EDITBOX,   P2_TO_P3_STATIC           },
    { PRES_FOR_RESPONSE_TIME_EDITBOX, PRES_FOR_RESPONSE_STATIC  },
    { CSFS_MIN_POSITION_EDITBOX,      MIN_CSFS_POS_STATIC       },
    { BISCUIT_TIME_DELAY_EDITBOX,     BISCUIT_TIME_DELAY_STATIC },
    { CSFS_MIN_VELOCITY_EDITBOX,      MIN_CSFS_VEL_STATIC       }
    };
const int nof_ids = sizeof(id)/sizeof(int);

HDC     dc;
HWND    w;
RECT    r;
RECT    tr;
RECT    dr;
POINT   p;
int     i;
int     line;
int     n;
int     x;
int     x1;
int     y;
int     dy;
int     dx;
int     label_width;
int     pos_label_width;
BOOLEAN have_alarms;
TCHAR   buf[MAXLINE+1];

dc = get_default_printer_dc();
if ( dc )
    {

    if ( StartDoc(dc, &di) > 0 )
        {
        if ( StartPage(dc) > 0 )
            {
            p.x = GetDeviceCaps( dc, PHYSICALOFFSETX );
            if ( p.x == 0 )
                {
                /*
                ------------------------------------
                If no margin force a 1/2 inch margin
                ------------------------------------ */
                p.x = .50 * GetDeviceCaps( dc, LOGPIXELSX );
                }
            p.y = GetDeviceCaps( dc, PHYSICALOFFSETY );
            if ( p.y == 0 )
                {
                p.y = .50 * GetDeviceCaps( dc, LOGPIXELSY );
                }
            r.left   = p.x;

            r.top    = p.y;
            r.right  = GetDeviceCaps( dc, HORZRES ) - p.x;
            r.bottom = GetDeviceCaps( dc, VERTRES ) - p.y;
            create_printer_font( dc, r );
            dy = character_height( dc );
            dx = average_character_width( dc );

            y = dy;
            print_header( dc, y, dy, r );

            /*
            --------------------------------------------
            Draw the rectangle for the basic part setups
            -------------------------------------------- */
            dr = r;
            dr.top = y + dy/2;
            dr.bottom = dr.top + dy*7;
            dr.right = dr.left + (dr.right - dr.left)/2;
            Rectangle( dc, dr.left, dr.top, dr.right, dr.bottom );

            y += dy;
            x  = r.left + dx;
            w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

            /*
            --------------
            Distance Units
            -------------- */
            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, DISTANCE_UNITS_STATIC, MAXLINE );
            lstrcat( buf, SpaceString );
            lstrcat( buf, units_name(CurrentPart.distance_units) );
            text_out( dc, x, y, buf );

            /*
            --------------
            Velocity Units
            -------------- */
            y += dy;
            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, VELOCITY_UNITS_STATIC, MAXLINE );
            lstrcat( buf, SpaceString );
            lstrcat( buf, units_name(CurrentPart.velocity_units) );
            text_out( dc, x, y, buf );

            /*
            -------------------
            Total Stroke Length
            ------------------- */
            y += dy;
            y += dy/2;
            x += 8 * dx;
            SetTextAlign( dc, TA_TOP | TA_RIGHT );
            get_text( buf, w, TOTAL_STROKE_LEN_EDITBOX, MAXLINE );
            text_out( dc, x, y, buf );

            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, TOTAL_STROKE_STATIC, MAXLINE );
            text_out( dc, x+dx, y, buf );

            /*
            -----------------
            Min Stroke Length
            ----------------- */
            y += dy;
            SetTextAlign( dc, TA_TOP | TA_RIGHT );
            get_text( buf, w, MIN_STROKE_LEN_EDITBOX, MAXLINE );
            text_out( dc, x, y, buf );

            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, MIN_STROKE_STATIC, MAXLINE );
            text_out( dc, x+dx, y, buf );

            /*
            ------------
            EOS Velocity
            ------------ */
            y += dy;
            SetTextAlign( dc, TA_TOP | TA_RIGHT );
            get_text( buf, w, EOS_VEL_EDITBOX, MAXLINE );
            text_out( dc, x, y, buf );

            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, EOS_STATIC, MAXLINE );
            text_out( dc, x+dx, y, buf );

            /*
            --------------
            Advanced Setup
            -------------- */
            y = dr.bottom + dy;
            SetTextAlign( dc, TA_TOP | TA_LEFT );
            text_out( dc, r.left, y, resource_string(ADVANCED_TAB_STRING) );

            y += dy;

            /*
            -----------------------------------------------
            Draw the rectangle for the advanced part setups
            ----------------------------------------------- */
            dr.top = y + dy/2;
            dr.bottom = dr.top + dy*6;
            dr.right = r.right - dx;
            Rectangle( dc, dr.left, dr.top, dr.right, dr.bottom );

            w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

            i = 0;
            for ( line=0; line<5; line++ )
                {
                y += dy;
                x  = r.left + 8*dx;
                SetTextAlign( dc, TA_TOP | TA_RIGHT );
                get_text( buf, w, id[i].editbox, MAXLINE );
                text_out( dc, x, y, buf );

                SetTextAlign( dc, TA_TOP | TA_LEFT );
                get_text( buf, w, id[i].textbox, MAXLINE );
                text_out( dc, x+dx, y, buf );
                i++;

                if ( i < nof_ids )
                    {
                    x = dr.left + (dr.right - dr.left)/2;
                    x += 8*dx;
                    SetTextAlign( dc, TA_TOP | TA_RIGHT );
                    get_text( buf, w, id[i].editbox, MAXLINE );
                    text_out( dc, x, y, buf );

                    SetTextAlign( dc, TA_TOP | TA_LEFT );
                    get_text( buf, w, id[i].textbox, MAXLINE );
                    text_out( dc, x+dx, y, buf );
                    i++;
                    }
                }

            /*
            ---------
            Positions
            --------- */
            y += 2*dy + dy/3;
            SetTextAlign( dc, TA_TOP | TA_LEFT );
            text_out( dc, r.left, y, resource_string(POSITIONS_TAB_STRING) );

            y += dy;

            /*
            -------------------------------------------------
            Draw the rectangle for the user defined positions
            ------------------------------------------------- */
            dr.top = y + dy/4;
            dr.bottom = dr.top + dy*7;
            Rectangle( dc, dr.left, dr.top-2, dr.right, dr.bottom+2 );

            w = WindowInfo[USER_DEFINED_TAB].dialogwindow;

            label_width = (dr.right - dr.left) / 7;
            tr.top      = dr.top + 2*dy;
            tr.bottom   = dr.bottom - dy;
            tr.left     = dr.left + dx;
            tr.right    = dr.left + label_width;

            get_text( buf, w, LS_POS_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            x = tr.right + 9*dx;

            tr.left  += label_width + 12*dx;
            tr.right += label_width + 12*dx;

            get_text( buf, w, VEL_POS_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            tr.top   -= dy/2;
            tr.left  += label_width + 12*dx;
            tr.right += label_width + 12*dx;
            tr.bottom = dr.bottom - dy/2;

            get_text( buf, w, AVG_VEL_RANGE_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            x = dr.left + label_width + 3*dx;
            y  = dr.top + dy;
            get_text( buf, w, POSITION_STATIC, MAXLINE );
            text_out( dc, x, y, buf );

            x += label_width + 12 * dx;
            text_out( dc, x, y, buf );

            pos_label_width = pixel_width(dc, buf);

            /*
            --------------------
            Start position label
            -------------------- */
            tr.top    = dr.top;
            tr.bottom = tr.top + 2*dy;
            tr.left   = x + label_width + 12 * dx;
            tr.right  = tr.left + pos_label_width;
            get_text( buf, w, START_POS_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            /*
            -----------------
            End postion label
            ----------------- */
            if ( 7*dx > pos_label_width )
                pos_label_width = 7*dx;

            pos_label_width += dx;

            tr.left  += pos_label_width;
            tr.right += pos_label_width;
            get_text( buf, w, END_POS_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            y  = dr.top + 2*dy;
            line = 1;
            for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
                {
                /*
                ------------
                Line numbers
                ------------ */
                x  = dr.left + label_width + 2*dx;
                SetTextAlign( dc, TA_TOP | TA_LEFT );
                lstrcpy( buf, int32toasc(line) );
                text_out( dc, x, y, buf );

                x1 = x + label_width + 12 * dx;
                text_out( dc, x1, y, buf );

                x1 += label_width + 12 * dx;
                text_out( dc, x1, y, buf );

                /*
                ---------------------
                Limit switch position
                --------------------- */
                x1 = x + 7*dx;
                SetTextAlign( dc, TA_TOP | TA_RIGHT );
                get_text( buf, w, LIMIT_SWITCH_POSITION_1_EDITBOX+i, MAXLINE );
                text_out( dc, x1, y, buf );

                /*
                -----------------
                Velocity position
                ----------------- */
                x1 += label_width + 12 * dx;
                get_text( buf, w, VELOCITY_POSITION_1_EDITBOX+i, MAXLINE );
                text_out( dc, x1, y, buf );

                /*
                -----------------------------
                Velocity range start position
                ----------------------------- */
                x1 += label_width + 12 * dx;
                get_text( buf, w, AVG_VELOCITY_START_1_EDITBOX+i, MAXLINE );
                text_out( dc, x1, y, buf );

                /*
                ---------------------------
                Velocity range end position
                --------------------------- */
                x1 +=  pos_label_width;
                get_text( buf, w, AVG_VELOCITY_END_1_EDITBOX+i, MAXLINE );
                text_out( dc, x1, y, buf );

                line++;
                y += dy;
                }

            y += 2*dy;
            print_fastrak_channels( dc, y, dx, dy, r );

            if ( HaveSureTrakControl )
                {
                /*
                ----------------
                Suretrak profile
                ---------------- */
                y += 8*dy + dy/2;
                SetTextAlign( dc, TA_TOP | TA_LEFT );
                text_out( dc, r.left, y, resource_string(SURETRAK_TAB_STRING) );

                y += dy;

                dr.top = y + dy/2;
                dr.bottom = dr.top + dy*8;
                Rectangle( dc, dr.left, dr.top-2, dr.right, dr.bottom+2 );

                print_suretrak_steps( dc, y, dx, dy, r );

                y += 10 * dy;

                y = print_suretrak_params( dc, y, dx, dy, r );
                /* print_global_suretrak_params( dc, y, dx, dy, r ); */
                }

            if ( *get_ini_string(EditPartIniFile, ConfigSection, ShowSetupSheetKey) == YChar )
                {
                y = print_setup_sheet( dc, r.left, y, dx, dy );
                }

            lstrcpy( buf, get_ini_string(EditPartIniFile, ConfigSection, IsoPageOneKey) );
            if ( !unknown(buf) && lstrlen(buf) > 0 )
                {
                y = r.bottom - dy - 1;
                x = r.left + dx;
                text_out( dc, x, y, buf );
                }
            EndPage(dc);
            }

        if ( *get_ini_string(EditPartIniFile, ConfigSection, PrintParametersKey) == YChar )
            {
            /*
            ---------------------------------
            See if there is anything to print
            --------------------------------- */
            have_alarms = FALSE;
            n = CurrentParam.count();
            for ( i=0; i<n; i++ )
                {
                if ( CurrentParam.parameter[i].has_any_limits() )
                    have_alarms = TRUE;
                }

            if ( have_alarms )
                {
                StartPage( dc );
                select_printer_font( dc );

                y = dy;
                print_header( dc, y, dy, r );

                print_parameters( dc, y, dx, dy, r );
                lstrcpy( buf, get_ini_string(EditPartIniFile, ConfigSection, IsoPageTwoKey) );
                if ( !unknown(buf) && lstrlen(buf) > 0 )
                    {
                    y = r.bottom - dy - 1;
                    x = r.left + dx;
                    text_out( dc, x, y, buf );
                    }
                EndPage(dc);
                }
            }

        EndDoc( dc );
        delete_printer_font( dc );
        }
    DeleteDC( dc );
    dc = NULL;
    }
}

/***********************************************************************
*                          GET_OVERVIEW_HELP                           *
***********************************************************************/
static void get_overview_help()
{
DWORD context;

context = BASIC_PART_HELP;

if ( CurrentDialogNumber >= 0 && CurrentDialogNumber < TAB_COUNT )
    {
    switch ( CurrentDialogNumber )
        {
        case BASIC_SETUP_TAB:
            context = BASIC_PART_HELP;
            break;

        case ADVANCED_SETUP_TAB:
            context = ADV_PART_OVERVIEW_HELP;
            break;

        case PARAMETER_LIMITS_TAB:
            context = PARAMETER_LIMITS_OVERVIEW_HELP;
            break;

        case USER_DEFINED_TAB:
            context = USER_DEF_POS_OVERVIEW_HELP;
            break;

        case PROFILE_MARKS_TAB:
            context = MARKS_SETUP_OVERVIEW_HELP;
            break;

        case FASTRAK_CHANNELS_TAB:
            context = CHANNELS_SETUP_OVERVIEW_HELP;
            break;

        case SURETRAK_PROFILE_TAB:
            if ( HaveSureTrakControl )
                context = PART_SETUP_SURETRAK_HELP;
            else
                context = SETUP_SHEET_HELP;
            break;

        case SETUP_SHEET_TAB:
            context = SETUP_SHEET_HELP;
            break;

        case PRESSURE_CONTROL_TAB:
            context = PRESSURE_CONTROL_HELP;
            break;
        }
    }

gethelp( HELP_CONTEXT, context );
}

/***********************************************************************
*                      POSITION_DROPDOWN_WINDOW                        *
***********************************************************************/
static void position_dropdown_window( HWND w )
{

RECT pr;
RECT mr;
int  x;

if ( !w )
    return;

if ( !IsWindowVisible(w) )
    return;

GetWindowRect( MainWindow, &mr );
GetWindowRect( w,          &pr );

x = mr.right - pr.right;

pr.left  += x;
pr.right += x;

x = mr.bottom - pr.top;

pr.top    += x;
pr.bottom += x;

MoveWindow( w, pr.left, pr.top, pr.right-pr.left, pr.bottom-pr.top, TRUE );
}

/***********************************************************************
*                     POSITION_CURRENT_DIALOG                          *
***********************************************************************/
static void position_current_dialog()
{
RECT r;
int  h;
int  w;
int  x;
int  y;

if ( CurrentDialogNumber != NO_TAB )
    {
    GetWindowRect( WindowInfo[CurrentDialogNumber].dialogwindow, &r );
    w = r.right - r.left;
    h = r.bottom - r.top;

    GetWindowRect( TabControl, &r );

    /*
    ------------------------------------
    Change window rect into display rect
    ------------------------------------ */
    TabCtrl_AdjustRect( TabControl, FALSE, &r );

    y = r.top + ((r.bottom - r.top) - h)/2;
    x = r.left + ( (r.right - r.left) - w )/2;
    MoveWindow( WindowInfo[CurrentDialogNumber].dialogwindow, x, y, w, h, TRUE );
    }

position_dropdown_window( ParamDialogWindow );
position_dropdown_window( SureTrakPlotWindow.handle() );
}

/***********************************************************************
*                          SHOW_CURRENT_DIALOG                         *
***********************************************************************/
static void show_current_dialog( int new_dialog_number )
{
if ( new_dialog_number == CurrentDialogNumber )
    return;

if ( CurrentDialogNumber != NO_TAB )
    {
    ShowWindow( WindowInfo[CurrentDialogNumber].dialogwindow, SW_HIDE );
    }

if ( new_dialog_number >= 0 && new_dialog_number < TAB_COUNT )
    {
    CurrentDialogNumber = new_dialog_number;
    if ( CurrentDialogNumber == SURETRAK_PROFILE_TAB )
        refresh_velocity_control_plot();
    //else if ( CurrentDialogNumber == PRESSURE_CONTROL_TAB )
        //refresh_pressure_control_plot();
    ShowWindow( WindowInfo[CurrentDialogNumber].dialogwindow, SW_SHOW );
    position_current_dialog();
    }
}

/***********************************************************************
*                           SHOW_TAB_CONTROL                           *
***********************************************************************/
static void show_tab_control()
{
int  i;

i = TabCtrl_GetCurSel( TabControl );

if ( i == SURETRAK_PROFILE_TAB && !HaveSureTrakControl )
    i = SETUP_SHEET_TAB;
else if ( i == PRESSURE_CONTROL_TAB && !HavePressureControl )
    i = SETUP_SHEET_TAB;

if ( i != CurrentDialogNumber )
    {
    if ( i == PARAMETER_LIMITS_TAB && CurrentPasswordLevel >= WindowInfo[PARAMETER_LIMITS_TAB].password_level )
        {
        ShowWindow( ParamDialogWindow, SW_SHOW );
        }
    else if ( CurrentDialogNumber == PARAMETER_LIMITS_TAB )
        {
        if ( IsWindowVisible(ParamDialogWindow) )
            ShowWindow( ParamDialogWindow, SW_HIDE );
        }

    if ( i == SURETRAK_PROFILE_TAB) // || i == PRESSURE_CONTROL_TAB )
        SureTrakPlotWindow.show();
    else if ( SureTrakPlotWindow.is_visible() )
        SureTrakPlotWindow.hide();
    }

show_current_dialog( i );
}

/***********************************************************************
*                               EBOX_FLOAT                             *
***********************************************************************/
float ebox_float( HWND w, int editbox_id )
{
UINT  n;
TCHAR s[31];

n = GetDlgItemText( w, editbox_id, s, 30 );
if ( n > 0 )
    return extfloat(s, short(n));

return 0.0;
}

/***********************************************************************
*                              MARK_NAME                               *
***********************************************************************/
TCHAR * mark_name( TCHAR type, short number )
{
static TCHAR s[PARAMETER_NAME_LEN+1];
short i;
int string_id;

lstrcpy( s, UNKNOWN );
i = number;
i--;
if ( type == PARAMETER_MARK )
    {
    if ( i >= 0 && i < CurrentParam.count() )
        lstrcpy( s, CurrentParam.parameter[i].name );
    }
else
    {
    if ( type == START_POS_MARK )
        string_id = START_POS_MARK_STRING + i;
    else if ( type == END_POS_MARK )
        string_id = END_POS_MARK_STRING + i;
    else
        string_id = VEL_POS_MARK_STRING + i;

    LoadString( MainInstance, string_id, s, sizeof(s) );
    }

return s;
}

/***********************************************************************
*                              MARK_NAME                               *
***********************************************************************/
TCHAR * mark_name( MARKLIST_ENTRY & m )
{
return mark_name( m.parameter_type, m.parameter_number );
}

/***********************************************************************
*                          SHOW_CURRENT_MARK                           *
***********************************************************************/
static void show_current_mark()
{
HWND w;
MARKLIST_ENTRY * m;
COLORS_CLASS c;
int x;
TCHAR colorname[COLOR_NAME_LEN+1];

w = WindowInfo[PROFILE_MARKS_TAB].dialogwindow;
if ( !w )
    return;

if ( CurrentSelectedMark >= 0 && CurrentSelectedMark < CurrentMarks.n )
    {
    m = &CurrentMarks.array[CurrentSelectedMark];

    /*
    -----
    Color
    ----- */
    lstrcpy( colorname, c.name(m->color) );
    upper_case( colorname );
    set_current_lb_item( w, MARKS_COLOR_LISTBOX_ID, colorname );

    /*
    ---------
    Line Type
    --------- */
    if ( m->symbol & VLINE_MARK )
        x = MARKS_VLINE_BUTTON;
    else if ( m->symbol & BLINE_MARK )
        x = MARKS_BASE_LINE_BUTTON;
    else
        x = MARKS_NO_LINE_BUTTON;

    CheckRadioButton( w, MARKS_NO_LINE_BUTTON, MARKS_BASE_LINE_BUTTON, x );

    /*
    -----
    Label
    ----- */
    SetDlgItemText( w, MARKS_LABEL_ID, m->label );
    }
}

/***********************************************************************
*                              SHOW_MARKS                              *
***********************************************************************/
static void show_marks()
{
short i;
HWND w;

w = WindowInfo[PROFILE_MARKS_TAB].dialogwindow;
if ( !w )
    return;

SendDlgItemMessage( w, MARKS_LISTBOX_ID, LB_RESETCONTENT, 0, 0L );

if ( !CurrentMarks.n )
    return;

for ( i=0; i<CurrentMarks.n; i++ )
    {
    SendDlgItemMessage( w, MARKS_LISTBOX_ID, LB_ADDSTRING, 0, (LPARAM) ((LPSTR) mark_name(CurrentMarks.array[i])) );
    }

if ( CurrentSelectedMark >= CurrentMarks.n )
    CurrentSelectedMark = CurrentMarks.n - 1;

SendDlgItemMessage( w, MARKS_LISTBOX_ID, LB_SETCURSEL, CurrentSelectedMark, 0L );
show_current_mark();
}

/***********************************************************************
*                       FILL_ANALOG_LIST_BOXES                         *
***********************************************************************/
static void fill_analog_list_boxes( HWND w, int box, int nof_boxes )
{
int        i;
NAME_CLASS s;
TCHAR      name[ASENSOR_DESC_LEN+1];
DB_TABLE   t;

s.get_asensor_dbname();
if ( t.open(s.text(), ASENSOR_RECLEN, PFL) )
    {
    while ( t.get_next_record(FALSE) )
        {
        if ( t.get_alpha(name, ASENSOR_DESC_OFFSET, ASENSOR_DESC_LEN) )
            {
            for ( i=0; i<nof_boxes; i++ )
                SendDlgItemMessage( w, box+i, CB_ADDSTRING, 0, (LPARAM) name );
            }
        }

    t.close();
    }
}

/***********************************************************************
*                       FILL_ANALOG_LIST_BOXES                         *
***********************************************************************/
static void fill_analog_list_boxes()
{
fill_analog_list_boxes( WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow, FT_CHANNEL_1_LISTBOX, MAX_EDITABLE_FTII_CHANNELS );
}

/***********************************************************************
*                              CAT_W_TAB                               *
***********************************************************************/
static void cat_w_tab( TCHAR * dest, const TCHAR * sorc )
{
lstrcat( dest, sorc );
lstrcat( dest, TEXT("\t") );
}

/***********************************************************************
*                               PARMLINE                               *
***********************************************************************/
static TCHAR * parmline( short i )
{
const short LINE_LEN = PARAMETER_NAME_LEN + 1 + UNITS_LEN + 1 + 4*(PARMLIST_FLOAT_LEN+1+WIRE_LEN+1);
static TCHAR s[LINE_LEN+1];
TCHAR wirename[WIRE_LEN+1];
short j;
short k;

*s = NullChar;
cat_w_tab( s, CurrentParam.parameter[i].name );
cat_w_tab( s, units_name(CurrentParam.parameter[i].units) );

for ( k=0; k<NOF_ALARM_LIMIT_TYPES; k++ )
    {
    j = AlarmIndex[k];
    cat_w_tab( s, ascii_float(CurrentParam.parameter[i].limits[j].value) );
    int32toasc( wirename, CurrentParam.parameter[i].limits[j].wire_number, DECIMAL_RADIX );
    cat_w_tab( s, wirename );
    }

*(s+lstrlen(s)-1) = NullChar;
return s;
}

/***********************************************************************
*                       CURRENT_PARAMETER_INDEX                        *
***********************************************************************/
short current_parameter_index()
{
HWND  w;
LRESULT x;

w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;

x = SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_GETCURSEL, 0, 0L);
if ( x == LB_ERR )
    x = NO_PARAMETER_INDEX;

return short(x);
}

/***********************************************************************
*                   REFRESH_CURRENT_PARAMETER_LIMITS                   *
***********************************************************************/
void refresh_current_parameter_limits()
{
int x;
int parameter;

x = current_parameter_index();
if ( x == NO_PARAMETER_INDEX )
    return;

parameter = CurrentSort.parameter_number( x );
ParamLimitLb.replace( parmline(parameter) );
ParamLimitLb.setcursel( x );
}

/***********************************************************************
*                      REFRESH_PARAMETER_LIMITS                        *
***********************************************************************/
static void refresh_parameter_limits()
{
HWND  w;
int i;
int n;
int parameter;
LRESULT x;
LRESULT ti;
STRING_CLASS s;

w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;

x  = SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_GETCURSEL,   0, 0L );
ti = SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_GETTOPINDEX, 0, 0L );
SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_RESETCONTENT, 0, 0L );

n = CurrentParam.count();

if ( x != LB_ERR && x >= LRESULT(n) )
    x = LRESULT(n-1);

for ( i=0; i<n; i++ )
    {
    parameter = CurrentSort.parameter_number( i );
    SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_ADDSTRING, 0, (LPARAM)(LPSTR) parmline(parameter) );
    }

if ( ti != LB_ERR )
    SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_SETTOPINDEX, ti, 0L );

if ( x != LB_ERR )
    {
    SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_SETCURSEL, x, 0L );
    parameter = CurrentSort.parameter_number( x );
    load_new_parameter( parameter );
    SetFocus( GetDlgItem(w, PARAMETER_LIMIT_LISTBOX) );
    }

/*
--------------------
Good shot wire setup
-------------------- */
s = CurrentPart.good_shot_wire;
s.set_text( w, GOOD_SHOT_WIRE_PB );
set_checkbox( w, GOOD_SHOT_NO_WARNINGS_XBOX, CurrentPart.good_shot_requires_no_warnings );
}

/***********************************************************************
*                          REFRESH_BASIC_SETUP                         *
***********************************************************************/
static void refresh_basic_setup()
{
HWND     w;
int      i;
WPARAM   x;
TCHAR    buf[MAX_DOUBLE_LEN+1];

w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

if ( !w )
    return;

x = INCHES_BUTTON;
switch ( CurrentPart.distance_units )
    {
    case INCH_UNITS:
        x = INCHES_BUTTON;
        break;

    case CM_UNITS:
        x = CM_BUTTON;
        break;

    case MM_UNITS:
        x = MM_BUTTON;
        break;
    }

/*
---------------------------------------
Set the state before setting the button
--------------------------------------- */
for ( i=0; i<NofDistanceButtons; i++ )
    {
    if ( DistanceButton[i] == (int) x )
        DistanceButtonState[i] = TRUE;
    else
        DistanceButtonState[i] = FALSE;
    }

CheckRadioButton( w, INCHES_BUTTON, CM_BUTTON, x );

x = IPS_BUTTON;
switch ( CurrentPart.velocity_units )
    {
    case IPS_UNITS:
        x = IPS_BUTTON;
        break;

    case FPM_UNITS:
        x = FPM_BUTTON;
        break;

    case CMPS_UNITS:
        x = CMPS_BUTTON;
        break;

    case MPS_UNITS:
        x = MPS_BUTTON;
        break;
    }

for ( i=0; i<NofVelocityButtons; i++ )
    {
    if ( VelocityButton[i] == (int) x )
        VelocityButtonState[i] = TRUE;
    else
        VelocityButtonState[i] = FALSE;
    }

CheckRadioButton( w, IPS_BUTTON, MPS_BUTTON, x );

rounded_double_to_tchar( buf, (double) CurrentPart.total_stroke_length );
SetDlgItemText( w, TOTAL_STROKE_LEN_EDITBOX, buf );

rounded_double_to_tchar( buf, (double) CurrentPart.min_stroke_length );
SetDlgItemText( w, MIN_STROKE_LEN_EDITBOX, buf );

rounded_double_to_tchar( buf, (double) CurrentPart.eos_velocity );
SetDlgItemText( w, EOS_VEL_EDITBOX, buf );
}

/***********************************************************************
*                          ADD_X_AXIS_TO_PLOT                          *
***********************************************************************/
static void add_x_axis_to_plot()
{
float x;

x = CurrentPart.total_stroke_length;
if ( x > 10.0 )
    {
    x /= 10.0;
    x = ceil(x);
    x *= 10.0;
    }
else
    x = ceil(x);

SureTrakPlot.add_axis( ST_POS_AXIS_ID, X_AXIS_INDEX, 0.0, x, BlackColor );
SureTrakPlot.set_decimal_places( ST_POS_AXIS_ID, 2 );

SureTrakPlot.set_tic_count( X_AXIS_INDEX, 10 );
}

/***********************************************************************
*                        ALL_STEPS_ARE_PERCENT                         *
***********************************************************************/
static BOOLEAN all_steps_are_percent()
{
SureTrakSetup.rewind();

while ( SureTrakSetup.next() )
    {
    if ( !SureTrakSetup.vel_is_percent() )
        return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                          ADD_Y_AXIS_TO_PLOT                          *
***********************************************************************/
static void add_y_axis_to_plot()
{
float ymax;
BOOLEAN all_percent;

all_percent = FALSE;
ymax        = 0.0;

if ( SureTrakSetup.nof_steps() > 0 )
    {
    if ( all_steps_are_percent() )
        {
        ymax = 100.0;
        all_percent = TRUE;
        }
    else
        {
        SureTrakSetup.rewind();
        while ( SureTrakSetup.next() )
            {
            if ( !SureTrakSetup.vel_is_percent() )
                maxfloat( ymax, SureTrakSetup.vel_value() );
            }
        }
    }

if ( !all_percent )
    maxfloat( ymax, SureTrakParam.max_velocity() );

SureTrakPlot.add_axis( ST_VEL_AXIS_ID, Y_AXIS_INDEX, 0.0, ymax, BlackColor );
SureTrakPlot.set_decimal_places( ST_VEL_AXIS_ID, 1 );
SureTrakPlot.set_tic_count( Y_AXIS_INDEX,  5 );
}

/***********************************************************************
*                          ADD_LABELS_TO_PLOT                          *
***********************************************************************/
static void add_labels_to_plot()
{
STRING_CLASS s;
TCHAR      * cp;

SureTrakPlot.empty_labels();

s = resource_string( POSITION_STRING );
s += SpaceString;
s += units_name( CurrentPart.distance_units );

SureTrakPlot.add_label( X_AXIS_INDEX, s.text(), BlackColor );

SureTrakPlot.add_label( Y_AXIS_INDEX, resource_string(VELOCITY_STRING), BlackColor );


if ( all_steps_are_percent() )
    cp = resource_string( PERCENT_STRING );
else
    cp = units_name(CurrentPart.velocity_units);

SureTrakPlot.add_label( Y_AXIS_INDEX, cp, BlackColor );
}

/***********************************************************************
*                          SET_FLOAT_POINT                             *
***********************************************************************/
inline void set_float_point( FLOAT_POINT & p, float x, float y ) { p.x = x; p.y = y; }

/***********************************************************************
*                          ADD_PROFILE_CURVE                           *
***********************************************************************/
static void add_profile_curve()
{
AXIS_ENTRY * ae;
float dx;
float x;
float v;
const int32 MAX_POINTS_PER_STEP = 3;
int32  i;
int32  n;
BOOLEAN y_is_percent;
BOOLEAN no_end_value;
static FLOAT_POINT p[MAX_ST_STEPS*MAX_POINTS_PER_STEP];

n = 0;
x = 0.0;
v = 0.0;
no_end_value = FALSE;

set_float_point( p[n], x, v );
n++;

if ( SureTrakSetup.nof_steps() > 0 )
    {
    y_is_percent = all_steps_are_percent();

    SureTrakSetup.rewind();
    while ( SureTrakSetup.next() )
        {
        if ( no_end_value )
            {
            dx = SureTrakSetup.end_pos_value();
            if ( !SureTrakSetup.accel_is_percent() )
                dx -= SureTrakSetup.accel_value();
            dx -= x;
            dx /= 2;
            x += dx;
            p[n-1].x = x;
            no_end_value = FALSE;
            }

        if ( !SureTrakSetup.accel_is_percent() )
            x += SureTrakSetup.accel_value();

        if ( SureTrakSetup.vel_is_percent() && !y_is_percent )
            {
            ae = SureTrakPlot.findaxis( ST_VEL_AXIS_ID );
            if ( ae )
                v = ae->max_value * SureTrakSetup.vel_value() / 100.0;
            }
        else
            v =  SureTrakSetup.vel_value();

        set_float_point( p[n], x, v );
        n++;

        if ( SureTrakSetup.have_vacuum_wait() )
            {
            i = SureTrakParam.vacuum_limit_switch_number();
            if ( i < MAX_ST_LIMIT_SWITCHES )
                {
                x = SureTrakSetup.limit_switch_pos_value(i);
                }
            else
                {
                x = CurrentPart.limit_switch_position[i-MAX_ST_LIMIT_SWITCHES];
                }
            }
        else
            {
            x = SureTrakSetup.end_pos_value();
            if ( is_float_zero(x) )
                {
                no_end_value = TRUE;
                x = p[n-1].x;
                }
            }

        set_float_point( p[n], x, v );
        n++;

        if ( findchar(PercentChar, SureTrakSetup.low_impact_string()) )
            {
            ae = SureTrakPlot.findaxis( ST_VEL_AXIS_ID );
            if ( ae )
                v = ae->max_value * SureTrakSetup.low_impact_value() / 100.0;
            }

        set_float_point( p[n], x, v );
        n++;
        }
    }

ae = SureTrakPlot.findaxis( ST_POS_AXIS_ID );
if ( ae )
    {
    if ( x < ae->max_value )
        {
        x = ae->max_value;
        set_float_point( p[n], x, v );
        n++;
        }
    }

if ( n > 1 )
    SureTrakPlot.add_curve( ST_POS_AXIS_ID, ST_VEL_AXIS_ID, n, p );
}

/***********************************************************************
*                    REFRESH_VELOCITY_CONTROL_PLOT                     *
***********************************************************************/
static void refresh_velocity_control_plot()
{
if ( CurrentDialogNumber != SURETRAK_PROFILE_TAB )
    return;

SureTrakPlot.empty_curves();
SureTrakPlot.empty_axes();

add_labels_to_plot();

add_x_axis_to_plot();
add_y_axis_to_plot();
add_profile_curve();

SureTrakPlot.size( SureTrakPlotWindow.handle() );
HaveSureTrakPlot = TRUE;
SureTrakPlotWindow.refresh();
}

/***********************************************************************
*                            REFRESH_ST_PARAMS                         *
***********************************************************************/
void refresh_st_params( HWND w )
{
float x;

set_text( w, VEL_LOOP_GAIN_EBOX, ascii_float(SureTrakSetup.velocity_loop_gain())   );

x = SureTrakSetup.ft_stop_pos();
if ( not_float_zero(x) )
    set_text( w, FT_STOP_POS_EBOX, ascii_float(x) );
else
    set_text( w, FT_STOP_POS_EBOX, EmptyString );
set_text( w, BREAK_VEL_1_EBOX,   ascii_float(SureTrakSetup.gain_break_velocity(0)) );
set_text( w, BREAK_GAIN_1_EBOX,  ascii_float(SureTrakSetup.gain_break_gain(0))     );
set_text( w, BREAK_VEL_2_EBOX,   ascii_float(SureTrakSetup.gain_break_velocity(1)) );
set_text( w, BREAK_GAIN_2_EBOX,  ascii_float(SureTrakSetup.gain_break_gain(1))     );
set_text( w, BREAK_VEL_3_EBOX,   ascii_float(SureTrakSetup.gain_break_velocity(2)) );
set_text( w, BREAK_GAIN_3_EBOX,  ascii_float(SureTrakSetup.gain_break_gain(2))     );
}

/***********************************************************************
*                          SET_RUN_IF_ON_TEXT                          *
***********************************************************************/
static void set_run_if_on_text()
{
HWND w;

w = GetDlgItem( SuretrakProfileWindow, WARMUP_RUN_IF_ON_XBOX );

if ( is_checked(w) )
    set_text(w, resource_string(ON_STRING) );
else
    set_text(w, resource_string(OFF_STRING) );
}

/***********************************************************************
*                         GET_CURRENT_PARTLIST                         *
***********************************************************************/
static BOOLEAN get_current_partlist( TEXT_LIST_CLASS & dest )
{
MACHINE_ENTRY * m;
PART_ENTRY    * p;

dest.empty();

m = find_machine_entry( CurrentMachine.name );
if ( m )
    {
    m->partlist.rewind();
    while ( TRUE )
        {
        p = (PART_ENTRY *) m->partlist.next();
        if ( !p )
            break;

        if ( p->name )
            {
            if ( charlen(p->name, PART_NAME_LEN) > 0 )
                dest.append( p->name );
            }
        }
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         REFRESH_WARMUP_SHOT                          *
***********************************************************************/
static void refresh_warmup_shot()
{
HWND            w;
LISTBOX_CLASS   lb;
TEXT_LIST_CLASS t;
BOOLEAN         warmup_part_exists;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

Warmup.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

lb.init( w, WARMUP_PART_CBOX );
lb.empty();

warmup_part_exists = FALSE;
if ( get_current_partlist(t) )
    {
    t.rewind();
    while ( t.next() )
        {
        if ( !strings_are_equal(t.text(), CurrentPart.name) )
            {
            lb.add( t.text() );
            if ( strings_are_equal(t.text(), Warmup.partname) )
                warmup_part_exists = TRUE;
            }
        }

    if ( warmup_part_exists )
        lb.setcursel( Warmup.partname );
    else
        Warmup.is_enabled = FALSE;
    }

set_checkbox( w, WARMUP_ENABLE_XBOX,    Warmup.is_enabled );
set_checkbox( w, WARMUP_RUN_IF_ON_XBOX, Warmup.run_if_on  );
set_run_if_on_text();
set_text( w, WARMUP_WIRE_EBOX, Warmup.wire );
}

/***********************************************************************
*                      REFRESH_MULTIPART_RUNLIST                       *
***********************************************************************/
static void refresh_multipart_runlist()
{
HWND            w;
LISTBOX_CLASS   lb;
int             i;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

lb.init( w, WARMUP_PART_CBOX );
lb.empty();

if ( MultipartRunlist.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
    {
    for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
        {
        if ( MultipartRunlist.partname[i].isempty() )
            lb.add( NotUsedString.text() );
        else
            lb.add( MultipartRunlist.partname[i].text() );
        }
    lb.setcursel( 0 );
    }

set_checkbox( w, WARMUP_ENABLE_XBOX, MultipartRunlist.is_enabled );

MultipartRunlistHasChanged = FALSE;
}

/***********************************************************************
*                           SAVE_WARMUP_SHOT                           *
***********************************************************************/
static void save_warmup_shot()
{
HWND w;
LISTBOX_CLASS   lb;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

Warmup.is_enabled = is_checked( w, WARMUP_ENABLE_XBOX );
Warmup.run_if_on  = is_checked( w, WARMUP_RUN_IF_ON_XBOX );
get_text( Warmup.wire, w, WARMUP_WIRE_EBOX, WARMUP_WIRE_LEN );

lb.init( w, WARMUP_PART_CBOX );
copystring( Warmup.partname, lb.selected_text() );

Warmup.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
}

/***********************************************************************
*                        SAVE_MULTIPART_RUNLIST                        *
***********************************************************************/
static void save_multipart_runlist()
{
HWND w;
LISTBOX_CLASS   lb;
MULTIPART_RUNLIST_CLASS rc;
BOOLEAN b;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

b = is_checked( w, WARMUP_ENABLE_XBOX );
if ( MultipartRunlist.is_enabled != b )
    {
    MultipartRunlist.is_enabled = b;
    MultipartRunlistHasChanged  = TRUE;
    }

if ( !MultipartRunlistHasChanged )
    return;

MultipartRunlist.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
MultipartRunlistHasChanged = FALSE;
}

/***********************************************************************
*                       REFRESH_SURETRAK_SETUP                         *
***********************************************************************/
static void refresh_suretrak_setup()
{
HWND         w;
WINDOW_CLASS wc;
int          box;
//int        window_state;
UINT         checkbox_state;
int32        step_number;
int32        i;
int          n;
bool         wires_are_visible;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

if ( !w )
    return;

show_window( w, FS_POSITION_MSG_TBOX, FALSE );

LoadingSureTrakSteps = TRUE;

step_number = 0;
box         = ACCEL_1_EDITBOX;
SureTrakSetup.rewind();
while ( SureTrakSetup.next() )
    {
    set_text( w, box, SureTrakSetup.accel_string() );
    box++;
    set_text( w, box, SureTrakSetup.vel_string() );
    box++;
    set_text( w, box, SureTrakSetup.end_pos_string() );

    /*
    ---------------------------------------------------
    if ( SureTrakSetup.have_vacuum_wait() )
        window_state = SW_HIDE;
    else
        window_state = SW_SHOW;

    ShowWindow( GetDlgItem(w, box), window_state );
    --------------------------------------------------- */
    box++;

    if ( SureTrakSetup.have_vacuum_wait() )
        {
        checkbox_state = BST_CHECKED;
        ShowWindow( GetDlgItem(w, FS_POSITION_MSG_TBOX), SW_SHOWNORMAL );
        }
    else
        {
        checkbox_state = BST_UNCHECKED;
        }

    CheckDlgButton( w, box, checkbox_state );
    box++;

    set_text( w, box, SureTrakSetup.low_impact_string() );
    box++;

    if ( SureTrakSetup.have_low_impact_from_input() )
        checkbox_state = BST_CHECKED;
    else
        checkbox_state = BST_UNCHECKED;

    CheckDlgButton( w, box, checkbox_state );
    box++;
    step_number++;
    }

/*
-------------------------
Clear any remaining steps
------------------------- */
while ( step_number < MAX_ST_STEPS )
    {
    set_text( w, box, EmptyString );
    box++;
    set_text( w, box, EmptyString );
    box++;
    set_text( w, box, EmptyString );
    EnableWindow( GetDlgItem(w, box), TRUE );
    box++;
    CheckDlgButton( w, box, BST_UNCHECKED );
    box++;
    set_text( w, box, EmptyString );
    box++;
    CheckDlgButton( w, box, BST_UNCHECKED );
    box++;
    step_number++;
    }

box = LS1_EDITBOX;
if ( CurrentMachine.is_ftii )
    n   = MAX_FTII_LIMIT_SWITCHES;
else
    n   = LIMIT_SWITCH_COUNT;

for ( i=0; i<n; i++ )
    {
    set_text( w, box, SureTrakSetup.limit_switch_pos_string(i) );
    box++;
    }

refresh_st_params( w );

if ( StModList.count() )
    {
    StModList.remove_all();
    HaveStModTbox.hide();
    }

if ( HaveMultipartRunlist )
    refresh_multipart_runlist();
else
    refresh_warmup_shot();

/*
---------------------------------------------------------------
The limit switch wires can only be set if this is a FT2 machine
--------------------------------------------------------------- */
wires_are_visible = false;
if ( !HideSureTrakLimitSwitchWires )
    if ( CurrentMachine.is_ftii && CurrentMachine.suretrak_controlled )
        wires_are_visible = true;
    
box = LS1_WIRE_PB;
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    wc = GetDlgItem( w, box );
    if ( wires_are_visible )
        {
        wc.set_title( CurrentLimitSwitchWire.ascii_wire(i) );
        wc.show();
        }
    else
        {
        wc.hide();
        }
    box++;
    }
wc = GetDlgItem( w, ST_LS_WIRE_STATIC );
wc.show( wires_are_visible );

LoadingSureTrakSteps = FALSE;

refresh_velocity_control_plot();
}

/***********************************************************************
*                      ADD_PRESSURE_CONTROL_CURVE                      *
static void add_pressure_control_curve()
{
AXIS_ENTRY * ae;
float x;
float y;
const int32 MAX_POINTS_PER_STEP = 3;
int32  i;
int32  n;
static FLOAT_POINT p[MAX_POINTS_PER_STEP+MAX_PRESSURE_CONTROL_STEPS*MAX_POINTS_PER_STEP];

n = 0;
x = 0.0;
y = PressureControl.arm_intensifier_pressure.float_value();

set_float_point( p[n], x, y );
n++;

for ( i=0; i<PressureControl.nof_steps; i++ )
    {
    x += PressureControl.step[i].ramp.float_value();
    y =  PressureControl.step[i].pressure.float_value();
    set_float_point( p[n], x, y );
    n++;

    x = PressureControl.step[i].holdtime.float_value();
    set_float_point( p[n], x, y );
    n++;
    }

ae = SureTrakPlot.findaxis( ST_POS_AXIS_ID );
if ( ae )
    {
    if ( x < ae->max_value )
        {
        x = ae->max_value;
        set_float_point( p[n], x, y );
        n++;
        }
    }

if ( n > 1 )
    SureTrakPlot.add_curve( ST_POS_AXIS_ID, ST_VEL_AXIS_ID, n, p );

}
***********************************************************************/

/***********************************************************************
*                     ADD_LABELS_TO_PRESSURE_PLOT                      *
***********************************************************************/
static void add_labels_to_pressure_plot()
{
STRING_CLASS  s;

SureTrakPlot.empty_labels();

s = resource_string( TIME_STRING );
s += SpaceString;
s += units_name( MS_UNITS );

SureTrakPlot.add_label( X_AXIS_INDEX, s.text(), BlackColor );

SureTrakPlot.add_label( Y_AXIS_INDEX, resource_string(PRESSURE_STRING), BlackColor );

SureTrakPlot.add_label( Y_AXIS_INDEX, units_name(analog_sensor_units(PressureControl.sensor_number)), BlackColor );
}

/***********************************************************************
*                       ADD_Y_AXIS_TO_PRESSURE_PLOT                    *
***********************************************************************/
static void add_y_axis_to_pressure_plot()
{
float ymax;
float ymin;
ANALOG_SENSOR_SETUP_DATA * as;

ymin = 0.0;
ymax = 1.0;

as = find_analog_sensor( PressureControl.sensor_number );
if ( as )
    {
    ymin = as->minval;
    ymax = as->maxval;
    }

SureTrakPlot.add_axis( ST_VEL_AXIS_ID, Y_AXIS_INDEX, ymin, ymax, BlackColor );
SureTrakPlot.set_decimal_places( ST_VEL_AXIS_ID, 0 );
SureTrakPlot.set_tic_count( Y_AXIS_INDEX,  5 );
}

/***********************************************************************
*                      ADD_X_AXIS_TO_PRESSURE_PLOT                     *
***********************************************************************/
static void add_x_axis_to_pressure_plot()
{
int32 i;

i =  CurrentPart.ms_per_time_sample;
i *= (int32) CurrentPart.nof_time_samples;

SureTrakPlot.add_axis( ST_TIME_AXIS_ID, X_AXIS_INDEX, 0.0, (float) i, BlackColor );
SureTrakPlot.set_decimal_places( ST_TIME_AXIS_ID, 0 );

SureTrakPlot.set_tic_count( X_AXIS_INDEX, 10 );
}
/***********************************************************************
*                    REFRESH_PRESSURE_CONTROL_PLOT                     *
static void refresh_pressure_control_plot()
{

??if ( CurrentDialogNumber != PRESSURE_CONTROL_TAB )
    return;

SureTrakPlot.empty_curves();
SureTrakPlot.empty_axes();

add_labels_to_pressure_plot();

add_x_axis_to_pressure_plot();
add_y_axis_to_pressure_plot();
add_pressure_control_curve();

SureTrakPlot.size( SureTrakPlotWindow.handle() );
HaveSureTrakPlot = TRUE;
SureTrakPlotWindow.refresh();

}
***********************************************************************/

/***********************************************************************
*                              SET_PERCENT                             *
*    Add or remove a percent sign, if necessary, from a string.        *
*    Do nothing if the string is empty. Return TRUE if the string      *
*    has changed.                                                      *
***********************************************************************/
static BOOLEAN set_percent( STRING_CLASS & s, BOOLEAN need_percent )
{
TCHAR * cp;

if ( s.isempty() )
    return FALSE;

cp = s.find( PercentChar );
if ( need_percent )
    {
    if ( !cp )
        {
        s += PercentChar;
        return TRUE;
        }
    }
else
    {
    if ( cp )
        {
        *cp = NullChar;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                           SET_EBOX_PERCENT                           *
*    Add or remove a percent sign, if necessary, from an editbox       *
***********************************************************************/
static void set_ebox_percent( HWND parent, int ebox_id, BOOLEAN need_percent )
{
TCHAR      * cp;
HWND         ebox;
STRING_CLASS s;

ebox = GetDlgItem( parent, ebox_id );
s.get_text( ebox );
if ( s.isempty() )
    return;

cp = s.find( PercentChar );
if ( need_percent )
    {
    if ( !cp )
        {
        s += PercentChar;
        s.set_text( ebox );
        }
    }
else
    {
    if ( cp )
        {
        *cp = NullChar;
        s.set_text( ebox );
        }
    }
}

/***********************************************************************
*                            EBOX_HAS_PERCENT                          *
***********************************************************************/
static BOOLEAN ebox_has_percent( HWND parent, int ebox_id )
{
STRING_CLASS s;

s.get_text( parent, ebox_id );
if ( s.contains(PercentChar) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                        SHOW_NEW_GAIN_CONTROLS                        *
***********************************************************************/
static void show_new_gain_controls( HWND hWnd, BOOLEAN is_enabled )
{
struct CONTROL_ENTRY {
    int id;
    short * password_level;
    };

static CONTROL_ENTRY new_gain_control[] = {
    { PC_LOOP_GAIN_EBOX,       &EditPressureSetupLevel },
    { PC_INTEGRAL_GAIN_EBOX,   &EditPressureSetupLevel },
    { PC_DERIVATIVE_GAIN_EBOX, &EditPressureSetupLevel }
    };

static const int nof_new_gain_controls = sizeof(new_gain_control)/sizeof(CONTROL_ENTRY);

BOOLEAN      b;
int          i;
WINDOW_CLASS w;

if ( is_enabled )
    {
    if ( is_checked(hWnd, ALL_OPEN_LOOP_XBOX) )
        is_enabled = FALSE;
    }

if ( is_enabled )
    {
    if ( is_checked(hWnd, PROPORTIONAL_VALVE_XBOX) )
        is_enabled = FALSE;
    }

for( i=0; i<nof_new_gain_controls; i++ )
    {
    w = GetDlgItem( hWnd, new_gain_control[i].id );
    b = is_enabled;
    if ( *new_gain_control[i].password_level > CurrentPasswordLevel )
        b = FALSE;
    w.enable( b );
    }
}

/***********************************************************************
*                        SHOW_NEW_GAIN_CONTROLS                        *
***********************************************************************/
static void show_new_gain_controls( HWND hWnd )
{
show_new_gain_controls( hWnd, is_checked(hWnd, PRES_CONTROL_ENABLE_XBOX) );
}

/***********************************************************************
*                           LABEL_GOOSE_BUTTON                         *
***********************************************************************/
static void label_goose_button( HWND w, BOOLEAN is_percent )
{
short u;

if ( is_checked( w ) )
    {
    if ( is_percent )
        {
        set_text( w, PercentString );
        return;
        }
    u = analog_sensor_units( (short) PressureControl.sensor_number );
    }
else
    {
    u = MS_UNITS;
    }
set_text( w, units_name(u) );
}

/***********************************************************************
*                           LABEL_GOOSE_BUTTON                         *
*                                                                      *
* I made this version for when you press the button. I need to figure  *
* if the goose command is a pressure or a percent based on the dialog  *
* box contents. The id of the ramp pressure is one less than the       *
* goose button. The id of the setpoint is one more.                    *
*                                                                      *
***********************************************************************/
static void label_goose_button( HWND goose_button, HWND parent, int goose_button_id )
{
short u;
BOOLEAN is_percent;
STRING_CLASS s;

is_percent = FALSE;
if ( is_checked(goose_button) )
    {
    if ( is_checked(parent, PROPORTIONAL_VALVE_XBOX) )
        {
        is_percent = TRUE;
        }
    else
        {
        s.get_text( parent, goose_button_id+1 );
        if ( s.contains(PercentChar) )
            {
            is_percent = TRUE;
            }
        else
            {
            s.get_text( parent, goose_button_id-1 );
            if ( s.contains(PercentChar) )
                is_percent = TRUE;
            }
        }

    if ( is_percent )
        {
        set_text( goose_button, PercentString );
        return;
        }

    u = analog_sensor_units( (short) PressureControl.sensor_number );
    }
else
    {
    u = MS_UNITS;
    }
set_text( goose_button, units_name(u) );
}

/***********************************************************************
*                          CHECK_GOOSE_BUTTON                          *
* If the proportional valve button is checked I need to check the      *
* goose button to see if the label should change.                      *
***********************************************************************/
static void check_goose_button( HWND parent, int goose_button_id )
{
HWND goose_button;

goose_button = GetDlgItem( parent, goose_button_id );
if ( is_checked(goose_button) )
    label_goose_button( goose_button, parent, goose_button_id );
}

/***********************************************************************
*                        DO_ALL_OPEN_LOOP_XBOX                         *
***********************************************************************/
static void do_all_open_loop_xbox( HWND parent )
{
static int xbox[3] = {PC_OPEN_LOOP_1_XBOX, PC_OPEN_LOOP_2_XBOX, PC_OPEN_LOOP_3_XBOX };
INT id;
BOOLEAN b;

b = is_checked( parent, ALL_OPEN_LOOP_XBOX );
for ( id=0; id<3; id++ )
    set_checkbox( parent, xbox[id], b );

show_new_gain_controls( parent );
}

/***********************************************************************
*                   SHOW_PROPORTIONAL_VALVE_CONTROLS                   *
* If the proportional valve checkbox is checked I need to change       *
* pressure labels to percent and hide the analog sensor listbox        *
***********************************************************************/
static void show_proportional_valve_controls( HWND parent, BOOLEAN is_proportional_valve )
{
bool is_visible;
int  id;
STRING_CLASS s;
WINDOW_CLASS w;

if ( is_proportional_valve )
    {
    is_visible = false;
    set_text( parent, PRES_STATIC, resource_string(PERCENT_STRING) );
    }
else
    {
    is_visible = true;
    set_text( parent, PRES_STATIC, resource_string(PRESSURE_STRING) );
    }

/*
--------------------------------------------------------------------------
Hide the pressure sensor listbox and title if this is a proportional valve
-------------------------------------------------------------------------- */
w = GetDlgItem( parent, ST_PRES_SENSOR_CBOX );
w.show( is_visible );
w = GetDlgItem( parent, ST_PRES_SENSOR_STATIC );
w.show( is_visible );

id = RETRACT_VOLTS_STRING;
if ( is_proportional_valve )
    id = RETRACT_PERCENT_STRING;
s = resource_string( id );
s.set_text( parent, RETRACT_VOLTS_STATIC );

id = VALVE_PARK_VOLTS_STRING;
if ( is_proportional_valve )
    id = VALVE_PARK_PERCENT_STRING;
s = resource_string( id );
s.set_text( parent, VALVE_PARK_VOLTS_STATIC );

if ( is_proportional_valve )
    {
    set_checkbox( parent, ALL_OPEN_LOOP_XBOX, TRUE );
    do_all_open_loop_xbox( parent );
    }
else
    {
    show_new_gain_controls( parent );
    }
}

/***********************************************************************
*                      TOGGLE_PROPORTIONAL_VALVE                       *
***********************************************************************/
static void toggle_proportional_valve( HWND parent )
{
static int goosebox[3] = {PC_GOOSE_1_XBOX, PC_GOOSE_2_XBOX, PC_GOOSE_3_XBOX };
static BOOLEAN old_all_open_state = FALSE;
static int old_volt_range_radio = PC_5_VOLT_RANGE_RADIO;

BOOLEAN      is_proportional_valve;
int          id;
STRING_CLASS s;
WINDOW_CLASS w;

is_proportional_valve = is_checked( parent, PROPORTIONAL_VALVE_XBOX );

if ( is_proportional_valve )
    {
    for ( id=PC_2_VOLT_RANGE_RADIO; id<=PC_10_VOLT_RANGE_RADIO; id++ )
        {
        if ( is_checked(parent, id) )
            {
            old_volt_range_radio = id;
            break;
            }
        }
    CheckRadioButton( parent, PC_2_VOLT_RANGE_RADIO, PC_10_VOLT_RANGE_RADIO, PC_10_VOLT_RANGE_RADIO );
    }
else
    {
    CheckRadioButton( parent, PC_2_VOLT_RANGE_RADIO, PC_10_VOLT_RANGE_RADIO, old_volt_range_radio );
    }

if ( is_proportional_valve )
    old_all_open_state = is_checked( parent, ALL_OPEN_LOOP_XBOX );
else
    set_checkbox( parent, ALL_OPEN_LOOP_XBOX, old_all_open_state );
do_all_open_loop_xbox( parent );

for ( id=0; id<3; id++ )
    {

    /*
    -------------------------------------------------
    Add or remove the percent sign from the ramp ebox
    ------------------------------------------------- */
    set_ebox_percent( parent, goosebox[id]-1, is_proportional_valve );
    set_ebox_percent( parent, goosebox[id]+1, is_proportional_valve );
    check_goose_button( parent, goosebox[id] );
    }

show_proportional_valve_controls( parent, is_proportional_valve );
}

/**********************************************************************
*                      show_retctl_controls                           *
**********************************************************************/
static void show_retctl_controls( HWND hWnd, BOOLEAN is_visible )
{
struct CONTROL_ENTRY {
    int id;
    short * password_level;
    };

static CONTROL_ENTRY control[] = {
    { RETCTL_ARM_PRES_EBOX, &EditPressureProfileLevel },
    { RETCTL_PRES_EBOX,     &EditPressureProfileLevel }
    };

const int NOF_CONTROLS = sizeof(control) / sizeof( CONTROL_ENTRY );

static int id[] = { RETCTL_GBOX, RETCTL_ARM_PRES_TBOX, RETCTL_PRES_TBOX };
const int NOF_IDS = sizeof(id) / sizeof( int );

int     i;
WINDOW_CLASS w;

for ( i=0; i<NOF_IDS; i++ )
    {
    w = GetDlgItem( hWnd, id[i] );
    if ( is_visible )
        w.show();
    else
        w.hide();
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    w = GetDlgItem( hWnd, control[i].id );
    if ( is_visible )
        {
        w.show();
        if ( *control[i].password_level > CurrentPasswordLevel )
            w.disable();
        else
            w.enable();
        }
    else
        {
        w.hide();
        }
    }
}

/**********************************************************************
*                      show_retctl_controls                           *
**********************************************************************/
static void show_retctl_controls( HWND hWnd )
{
show_retctl_controls( hWnd,  is_checked(hWnd, RETCTL_ENABLE_XBOX) );
}

/***********************************************************************
*                          SHOW_PRES_CONTROLS                          *
***********************************************************************/
static void show_pres_controls( HWND hWnd )
{
struct CONTROL_ENTRY {
    int id;
    short * password_level;
    };

static CONTROL_ENTRY control[] = {
    { RAMP_1_EBOX,             &EditPressureProfileLevel },
    { PC_GOOSE_1_XBOX,         &EditPressureProfileLevel },
    { PRES_1_EBOX,             &EditPressureProfileLevel },
    { HOLD_TIME_1_EBOX,        &EditPressureProfileLevel },
    { RAMP_2_EBOX,             &EditPressureProfileLevel },
    { PC_GOOSE_2_XBOX,         &EditPressureProfileLevel },
    { PRES_2_EBOX,             &EditPressureProfileLevel },
    { HOLD_TIME_2_EBOX,        &EditPressureProfileLevel },
    { RAMP_3_EBOX,             &EditPressureProfileLevel },
    { PC_GOOSE_3_XBOX,         &EditPressureProfileLevel },
    { PRES_3_EBOX,             &EditPressureProfileLevel },
    { HOLD_TIME_3_EBOX,        &EditPressureProfileLevel },
    { ST_PRES_SENSOR_CBOX,     &EditPressureSetupLevel   },
    { INTENS_ARM_PRES_EBOX,    &EditPressureSetupLevel   },
    { INITIAL_PRES_EBOX,       &EditPressureSetupLevel   },
    { RETRACT_PRES_EBOX,       &EditPressureSetupLevel   },
    { PARK_PRES_EBOX,          &EditPressureSetupLevel   },
    { RETCTL_ENABLE_XBOX,      &EditPressureSetupLevel   },
    { PC_2_VOLT_RANGE_RADIO,   &EditPressureSetupLevel   },
    { PC_5_VOLT_RANGE_RADIO,   &EditPressureSetupLevel   },
    { PC_10_VOLT_RANGE_RADIO,  &EditPressureSetupLevel   },
    { POSITIVE_ONLY_XBOX,      &EditPressureSetupLevel   },
    { PROPORTIONAL_VALVE_XBOX, &EditPressureSetupLevel   },
    { PC_ARM_LS_CBOX,          &EditPressureSetupLevel   },
    { ALL_OPEN_LOOP_XBOX,      &EditPressureSetupLevel   }
    };

static const int nof_controls = sizeof(control)/sizeof(CONTROL_ENTRY);

BOOLEAN      b;
BOOLEAN      is_enabled;
int          i;
WINDOW_CLASS w;

is_enabled = is_checked(hWnd, PRES_CONTROL_ENABLE_XBOX );

for( i=0; i<nof_controls; i++ )
    {
    w = GetDlgItem( hWnd, control[i].id );
    b = is_enabled;
    if ( *control[i].password_level > CurrentPasswordLevel )
        b = FALSE;
    w.enable( b );
    }

if ( is_enabled )
    show_retctl_controls( hWnd );
else
    show_retctl_controls( hWnd, FALSE );

show_new_gain_controls( hWnd, is_enabled );
}

/***********************************************************************
*                    LOAD_PC_ARM_LIMIT_SWITCH_CBOX                     *
***********************************************************************/
static void load_pc_arm_limit_switch_cbox( HWND parent )
{
int           i;
LISTBOX_CLASS lb;
STRING_CLASS  s;

lb.init( parent, PC_ARM_LS_CBOX );
lb.redraw_off();
lb.empty();
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    s = SureTrakParam.limit_switch_desc(i);
    if ( s.isempty() )
        {
        s = TEXT( "LS" );
        s += i+1;
        }
    lb.add( s.text() );
    }
lb.redraw_on();

i = PressureControl.arm_intensifier_ls_number;
if ( i > MAX_FTII_LIMIT_SWITCHES )
    i = 0;

if ( i > 0 )
    i--;

lb.setcursel( i );
}

/***********************************************************************
*                    REFRESH_PRESSURE_CONTROL_SETUP                    *
***********************************************************************/
static void refresh_pressure_control_setup()
{
BOOLEAN       b;
int32         i;
int32         id;
LISTBOX_CLASS lb;
HWND          w;
HWND          button;
PRES_CONTROL_STEP_ENTRY * se;
STRING_CLASS  s;

w = WindowInfo[PRESSURE_CONTROL_TAB].dialogwindow;

if ( !w )
    return;

LoadingPressureControl = TRUE;

lb.init( w, ST_PRES_SENSOR_CBOX );
lb.setcursel( analog_sensor_description((short) PressureControl.sensor_number) );
i = PressureControl.voltage_output_range;
if ( i < 0 || i > 2 )
    i = 1;
CheckRadioButton( w, PC_2_VOLT_RANGE_RADIO, PC_10_VOLT_RANGE_RADIO, PC_2_VOLT_RANGE_RADIO+i );

set_checkbox( w, PRES_CONTROL_ENABLE_XBOX, PressureControl.is_enabled );
set_checkbox( w, POSITIVE_ONLY_XBOX,       PressureControl.use_only_positive_voltages );
set_checkbox( w, RETCTL_ENABLE_XBOX,       PressureControl.retctl_is_enabled );
set_checkbox( w, PROPORTIONAL_VALVE_XBOX,  PressureControl.proportional_valve );

id = RAMP_1_EBOX;
for ( i=0; i<PressureControl.nof_steps; i++ )
    {
    se = &PressureControl.step[i];
    se->ramp.set_text( w, id );
    id++;
    button = GetDlgItem( w, id );
    set_checkbox( button, se->is_goose_step );
    label_goose_button( button, se->is_goose_percent );
    id++;
    se->pressure.set_text( w, id );
    id++;
    set_checkbox( w, id, se->is_open_loop );
    id++;
    se->holdtime.set_text( w, id );
    id++;
    }

while ( i < MAX_PRESSURE_CONTROL_STEPS )
    {
    set_text( w, id, EmptyString );
    id++;
    button = GetDlgItem( w, id );
    set_checkbox( button, FALSE );
    label_goose_button( button, FALSE );
    id++;
    set_text( w, id, EmptyString );
    id++;
    set_checkbox( w, id, FALSE );
    id++;
    set_text( w, id, EmptyString );
    id++;
    i++;
    }

b = FALSE;
for ( i=0; i<PressureControl.nof_steps; i++ )
    {
    if ( PressureControl.step[i].is_open_loop )
        {
        b = TRUE;
        break;
        }
    }

set_checkbox( w, ALL_OPEN_LOOP_XBOX,  b );
do_all_open_loop_xbox( w );

PressureControl.retctl_arm_pres.set_text( w, RETCTL_ARM_PRES_EBOX );
PressureControl.retctl_pres.set_text( w, RETCTL_PRES_EBOX );
PressureControl.pressure_loop_gain.set_text( w, PC_LOOP_GAIN_EBOX );
PressureControl.integral_gain.set_text( w, PC_INTEGRAL_GAIN_EBOX );
PressureControl.derivative_gain.set_text( w, PC_DERIVATIVE_GAIN_EBOX );
PressureControl.retract_volts.set_text( w, RETRACT_PRES_EBOX );
PressureControl.park_volts.set_text( w, PARK_PRES_EBOX );

show_proportional_valve_controls( w, PressureControl.proportional_valve );

show_pres_controls( w );
if ( PressureControl.retctl_is_enabled )
    show_retctl_controls( w );

load_pc_arm_limit_switch_cbox( w );

LoadingPressureControl = FALSE;

//refresh_pressure_control_plot();
}

/***********************************************************************
*                       REFRESH_ADVANCED_SETUP                         *
***********************************************************************/
static void refresh_advanced_setup()
{
HWND w;
UINT id;
double x;

w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

if ( !w )
    return;

SetDlgItemText( w, PLUNGER_DIAMETER_EDITBOX,       ascii_float(CurrentPart.plunger_diameter) );
SetDlgItemText( w, SLEEVE_FILL_DISTANCE_EDITBOX,   ascii_float(CurrentPart.sleeve_fill_distance) );
SetDlgItemText( w, RUNNER_FILL_DISTANCE_EDITBOX,   ascii_float(CurrentPart.runner_fill_distance) );
SetDlgItemText( w, CSFS_MIN_POSITION_EDITBOX,      ascii_float(CurrentPart.csfs_min_position) );
SetDlgItemText( w, CSFS_MIN_VELOCITY_EDITBOX,      ascii_float(CurrentPart.csfs_min_velocity) );
SetDlgItemText( w, CSFS_RISE_VELOCITY_EDITBOX,     ascii_float(CurrentPart.csfs_rise_velocity) );
SetDlgItemText( w, TIME_FOR_INTENS_PRES_EDITBOX,   ascii_float(CurrentPart.time_for_intens_pres) );
SetDlgItemText( w, PRES_FOR_RESPONSE_TIME_EDITBOX, ascii_float(CurrentPart.pres_for_response_time) );

x = (double) CurrentPart.biscuit_time_delay;
if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT && CurrentPart.biscuit_time_delay > 10 )
    x /= 1000.0;
SetDlgItemText( w, BISCUIT_TIME_DELAY_EDITBOX, ascii_double(x) );

if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT )
    id = TIME_TO_MEASURE_BISCUIT_STRING;
else
    id = BISCUIT_TIME_DELAY_STRING;
set_text( w, BISCUIT_TIME_DELAY_STATIC, resource_string(id) );

/*
if ( CurrentPart.setpoint[0].is_numeric() )
    s = CurrentPart.setpoint[0].text();
else
    s = EmptyString;
SetDlgItemText( w, END_OF_FS_VELOCITY_EDITBOX, s );
*/
}

/***********************************************************************
*                           ADD_ONE_PARAMETER                          *
***********************************************************************/
BOOLEAN add_one_parameter( short new_parameter_type )
{
short i;
HWND  w;

i = CurrentParam.count();
if ( i >= MAX_PARMS )
    return FALSE;

CurrentParam.parameter[i].input.type = new_parameter_type;
if ( new_parameter_type == FT_DIGITAL_INPUT )
    {
    CurrentParam.parameter[i].vartype = DIGITAL_INPUT_VAR;
    CurrentParam.parameter[i].units   = DIGITAL_INPUT_UNITS;
    }
else if ( new_parameter_type == FT_EXTENDED_ANALOG_INPUT )
    {
    CurrentParam.parameter[i].input.number = 0;
    CurrentParam.parameter[i].units        = analog_sensor_units( CurrentPart.analog_sensor[0] );
    CurrentParam.parameter[i].vartype      = analog_sensor_vartype( CurrentPart.analog_sensor[0] );
    CurrentFtAnalog.array[i].channel       = 1;
    CurrentFtAnalog.array[i].result_type   = SINGLE_POINT_RESULT_TYPE;
    CurrentFtAnalog.array[i].ind_value     = ZeroString;  /* 0 => No Wire */
    CurrentFtAnalog.array[i].ind_var_type  = TRIGGER_WHEN_GREATER;
    CurrentFtAnalog.array[i].end_value     = ZeroString;
    }
lstrcpyn( CurrentParam.parameter[i].name, resource_string(NEW_PARAMETER_NAME_STRING), PARAMETER_NAME_LEN );

w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;
refresh_parameter_limits();

load_new_parameter( i );
SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_SETCURSEL, i, 0L );
return TRUE;
}

/***********************************************************************
*                         REMOVE_CURRENT_PARAMETER                     *
***********************************************************************/
static void remove_current_parameter()
{
HWND  w;
short x;
short parameter_number;

w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;

x = current_parameter_index();
if ( x == NO_PARAMETER_INDEX )
    return;

if ( CurrentParam.parameter[x].input.type == INTERNAL_PARAMETER )
    {
    resource_message_box( MainInstance, NO_DEL_INTERNAL_PARAM_STRING, CANT_DO_THAT_STRING, MB_OK );
    return;
    }
/*
---------------------------------------------------------------------------------------------
If the sorted position of this parameter is not the same as the parameter number, put it back
--------------------------------------------------------------------------------------------- */
parameter_number = CurrentSort.parameter_number( x );
if ( parameter_number != x )
    CurrentSort.move( parameter_number, x );

CurrentParam.remove( parameter_number );

/*
----------------------------------------------
The ftanalog entry is stored at the same index
---------------------------------------------- */
CurrentFtAnalog.remove( parameter_number );

/*
----------------------------
Reload the parameter listbox
---------------------------- */
refresh_parameter_limits();

if ( parameter_number >= CurrentParam.count() )
    parameter_number--;

if ( parameter_number >= 0 )
    {
    load_new_parameter( parameter_number );
    x = CurrentSort.index( parameter_number );
    SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_SETCURSEL, x, 0L );
    }
}

/***********************************************************************
*                    REFRESH_USER_DEFINED_POSITIONS                    *
***********************************************************************/
static void refresh_user_defined_positions()
{
static UINT ft2_ids[] = { LS_WIRE_1_PB, LS_WIRE_2_PB, LS_WIRE_3_PB, LS_WIRE_4_PB, LS_WIRE_STATIC };
const  int  nof_ft2_ids = sizeof(ft2_ids)/sizeof(UINT);

static UINT ft_ids[] = { LS_POS_3_STATIC, LS_POS_4_STATIC, LIMIT_SWITCH_POSITION_3_EDITBOX, LIMIT_SWITCH_POSITION_4_EDITBOX  };
const  int  nof_ft_ids = sizeof(ft_ids)/sizeof(UINT);

HWND w;
WINDOW_CLASS wc;
int  i;
UINT pb;
BOOLEAN is_visible;

w = WindowInfo[USER_DEFINED_TAB].dialogwindow;

for ( i=0; i<USER_VEL_COUNT; i++ )
    SetDlgItemText( w, VELOCITY_POSITION_1_EDITBOX+i, ascii_float(CurrentPart.user_velocity_position[i]) );

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    SetDlgItemText( w, AVG_VELOCITY_START_1_EDITBOX+i, ascii_float(CurrentPart.user_avg_velocity_start[i]) );
    SetDlgItemText( w, AVG_VELOCITY_END_1_EDITBOX+i, ascii_float(CurrentPart.user_avg_velocity_end[i]) );
    }

/*
---------------------------------------------------------------------------------------
                                     Limit Switches
If this is a suretrak2 control, all the limit switch settings are on the control screen.
If this is a suretrak1 control you can still set these fastrak board limit switches.
Note: LIMIT_SWITCH_COUNT = 4, MAX_ST_LIMIT_SWITCHES = 4
--------------------------------------------------------------------------------------- */
is_visible = TRUE;
if ( CurrentMachine.is_ftii )
    is_visible = FALSE;
show_window( w, FT1_LS_STATIC, is_visible );

is_visible = TRUE;
if ( CurrentMachine.is_ftii && CurrentMachine.suretrak_controlled )
    is_visible = FALSE;;

for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
    {
    show_window( w, LIMIT_SWITCH_POSITION_1_EDITBOX+i, is_visible );
    show_window( w, LS_POS_1_STATIC+i, is_visible );
    }

show_window( w, LS_POS_STATIC, is_visible );
show_window( w, POSITION_STATIC, is_visible );

if ( is_visible )
    {
    for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
        SetDlgItemText( w, LIMIT_SWITCH_POSITION_1_EDITBOX+i, ascii_float(CurrentPart.limit_switch_position[i]) );
    }

/*
------------------------------------------------------
                 Limit Switch Wires
The wire pushbuttons are only visible if this is a ft2
------------------------------------------------------ */
if ( is_visible )
    is_visible = CurrentMachine.is_ftii;

show_window( w, LS_WIRE_STATIC, is_visible );
for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
    show_window( w, LS_WIRE_1_PB+i, is_visible );

if ( is_visible )
    {
    pb = LS_WIRE_1_PB;
    for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
        {
        set_text( w, pb, CurrentLimitSwitchWire.ascii_wire(i) );
        pb++;
        }
    }
}

/***********************************************************************
*                          CHECK_LOW_IMPACT_VALUES                     *
***********************************************************************/
BOOLEAN check_low_impact_values()
{
TCHAR      * cp;
BOOLEAN      status;
float        x;
float        xmin;
STRING_CLASS s;

status = TRUE;
SureTrakSetup.rewind();
while ( SureTrakSetup.next() )
    {
    cp = SureTrakSetup.low_impact_string();
    if ( !is_empty(cp) )
        {
        x = extfloat( cp );
        xmin = SureTrakParam.min_low_impact_percent();
        if ( x < xmin )
            {
            if ( not_float_zero(xmin - x) )
                {
                status = FALSE;
                s = ascii_float(xmin);
                s += PercentString;
                SureTrakSetup.set_low_impact( s.text() );
                SureTrakHasChanged = TRUE;
                }
            }
        }
    }

return status;
}

/***********************************************************************
*                            SHOW_LOW_IMPACT_ERROR                     *
***********************************************************************/
static void show_low_impact_error()
{
TCHAR      * cp;
STRING_CLASS s;

s = resource_string( MainInstance, LI_GT_STRING );
s += ascii_float( SureTrakParam.min_low_impact_percent() );
s += PercentString;

cp = resource_string( MainInstance, INVALID_ENTRY_STRING );
MessageBox( MainWindow, s.text(), cp, MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                            ENABLE_BUTTONS                            *
***********************************************************************/
static void enable_buttons()
{
static INT SureTrakControlId[] = {
    ACCEL_1_EDITBOX, VEL_1_EDITBOX, EP_1_EDITBOX, VW_1_CHECKBOX, LI_1_EDITBOX,
    LIFI_1_CHECKBOX, ACCEL_2_EDITBOX, VEL_2_EDITBOX, EP_2_EDITBOX, VW_2_CHECKBOX, LI_2_EDITBOX, LIFI_2_CHECKBOX,
    ACCEL_3_EDITBOX, VEL_3_EDITBOX, EP_3_EDITBOX, VW_3_CHECKBOX, LI_3_EDITBOX, LIFI_3_CHECKBOX,
    ACCEL_4_EDITBOX, VEL_4_EDITBOX, EP_4_EDITBOX, VW_4_CHECKBOX, LI_4_EDITBOX, LIFI_4_CHECKBOX,
    ACCEL_5_EDITBOX, VEL_5_EDITBOX, EP_5_EDITBOX, VW_5_CHECKBOX, LI_5_EDITBOX, LIFI_5_CHECKBOX,
    ACCEL_6_EDITBOX, VEL_6_EDITBOX, EP_6_EDITBOX, VW_6_CHECKBOX, LI_6_EDITBOX, LIFI_6_CHECKBOX,
    LS1_EDITBOX, LS2_EDITBOX, LS3_EDITBOX, LS4_EDITBOX,LS5_EDITBOX,LS6_EDITBOX,
    DOS_BACKUP_BUTTON
    };
const NOF_SURETRAK_CONTROLS = sizeof(SureTrakControlId)/sizeof(INT);

static INT Ft2SureTrakControlId[] = {
    FT_STOP_POS_EBOX, FT_STOP_POS_STATIC
    };
const NOF_FT2_SURETRAK_CONTROLS = sizeof(Ft2SureTrakControlId)/sizeof(INT);

COMPUTER_CLASS c;
STRING_CLASS   s;
HWND     w;

BOOLEAN  need_dos_button;;
BOOLEAN  is_current_part;
BOOLEAN  is_this_computer;
BOOLEAN  is_enabled;
BOOLEAN  is_ft2;
BOOLEAN  is_visible;
BOOLEAN  was_enabled;

int      i;
UINT     id;
TCHAR  * cp;
short    required_password_level;

/*
-------------------------------------------------------------------
If this machine is being monitored and this is NOT the current part
then show the monitor this part button.
------------------------------------------------------------------- */
id              = 0;
is_enabled      = FALSE;

is_this_computer = strings_are_equal( c.whoami(), CurrentMachine.computer );
is_current_part  = strings_are_equal( CurrentMachine.current_part, CurrentPart.name );

if ( is_this_computer || CurrentPasswordLevel >= EditOtherCurrentPartLevel )
    {
    if ( CurrentMachine.monitor_flags & MA_MONITORING_ON || is_remote_monitor() )
        {
        /*
        --------------------------------------------------------------------
        Enable the "Start Monitoring" button if this is not the current part
        -------------------------------------------------------------------- */
        if ( !is_current_part )
            {
            is_enabled = TRUE;
            id = MONITOR_THIS_PART_STRING;
            }
        }
    else
        {
        id = NOT_MONITORED_STRING;
        }
    }

w = GetDlgItem( MainDialogWindow, MONITOR_THIS_PART_BUTTON );

if ( !id )
    {
    if ( is_current_part )
        id = THIS_IS_CURRENT_PART_STRING;
    else
        id = NOT_CURRENT_PART_STRING;
    }

set_text( w, resource_string(id) );
EnableWindow( w, is_enabled );

is_enabled = TRUE;
if ( !is_this_computer )
    {
    if ( CurrentPasswordLevel < EditOtherCurrentPartLevel )
        {
        if ( CurrentMachine.monitor_flags & MA_MONITORING_ON && is_current_part )
            is_enabled = FALSE;

        if ( CurrentPasswordLevel < EditOtherComputerLevel )
            is_enabled = FALSE;
        }
    }

if ( CurrentPasswordLevel < SaveChangesLevel )
    is_enabled = FALSE;

w = GetDlgItem( MainDialogWindow, SAVE_CHANGES_BUTTON );
if ( VelocityChangesLevel != NO_PASSWORD_LEVEL )
    {
    /*
    --------------------------------------------------------
    Make sure the title of the save chages button is correct
    -------------------------------------------------------- */
    if ( is_enabled )
        {
        SaveChangesTitle.set_text( w );
        NeedToCallVelocityChanges = FALSE;
        }
    else if ( CurrentPasswordLevel >= VelocityChangesLevel )
        {
        set_text( w, resource_string(VELOCITY_CHANGES_STRING) );
        NeedToCallVelocityChanges = TRUE;
        is_enabled = TRUE;
        }
    }

EnableWindow( w, is_enabled );

if ( HaveSureTrakControl )
    {
    was_enabled = is_enabled;
    is_ft2      = CurrentMachine.is_ftii;
    need_dos_button = TRUE;
    if ( is_ft2 )
        need_dos_button = FALSE;
    if ( !is_this_computer )
        {
        if ( CurrentPasswordLevel < EditOtherSuretrakLevel )
            is_enabled = FALSE;

        if ( is_current_part && CurrentPasswordLevel < EditOtherCurrentSuretrakLevel )
            is_enabled = FALSE;
        }

    show_window( SuretrakProfileWindow, DOS_BACKUP_BUTTON, need_dos_button );

    for ( i=0; i<NOF_SURETRAK_CONTROLS; i++ )
        {
        w = GetDlgItem( SuretrakProfileWindow, SureTrakControlId[i] );
        EnableWindow( w, is_enabled );
        }

    for ( i=0; i<NOF_FT2_SURETRAK_CONTROLS; i++ )
        {
        w = GetDlgItem( SuretrakProfileWindow, Ft2SureTrakControlId[i] );
        show_window( w, is_ft2 );
        if ( is_ft2 )
            EnableWindow( w, is_enabled );
        }

    is_visible = CurrentMachine.suretrak_controlled;

    show_window( SuretrakProfileWindow, LIMIT_SWITCH_POS_STATIC, is_visible );
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        {
        if ( is_visible )
            {
            if ( (i==LIMIT_SWITCH_COUNT) && (!is_ft2) )
                is_visible = FALSE;
            }
        show_window( SuretrakProfileWindow, LS1_EDITBOX+i,   is_visible );
        show_window( SuretrakProfileWindow, LS1_DESC_TBOX+i, is_visible );
        show_window( SuretrakProfileWindow, LS1_STATIC+i,    is_visible );
        }

    is_visible = CurrentMachine.suretrak_controlled;
    is_visible &= is_ft2;
    if ( HideSureTrakLimitSwitchWires )
        is_visible = FALSE;
    show_window(SuretrakProfileWindow, ST_LS_WIRE_STATIC, is_visible );

    /*
    ----------------------------------------------------------------------
    If this machine has a cyclone board I need to hide break gains 2 and 3
    ---------------------------------------------------------------------- */
    if ( CurrentMachine.is_cyclone )
        {
        show_window(SuretrakProfileWindow, BREAK_VEL_2_EBOX, FALSE );
        show_window(SuretrakProfileWindow, BREAK_VEL_3_EBOX, FALSE );
        s = resource_string( INTEGRAL_GAIN_STRING );
        s.set_text( SuretrakProfileWindow, BREAK_2_STATIC );
        s = resource_string( DERIVATIVE_GAIN_STRING );
        s.set_text( SuretrakProfileWindow, BREAK_3_STATIC );
        }
    else
        {
        show_window(SuretrakProfileWindow, BREAK_VEL_2_EBOX, TRUE );
        show_window(SuretrakProfileWindow, BREAK_VEL_3_EBOX, TRUE );
        s = TEXT("2" );
        s.set_text( SuretrakProfileWindow, BREAK_2_STATIC );
        s = TEXT("3" );
        s.set_text( SuretrakProfileWindow, BREAK_3_STATIC );
        }

    is_enabled = was_enabled;
    }

if ( is_current_part )
    is_enabled = FALSE;

w = GetDlgItem( MainDialogWindow, DELETE_PART_BUTTON  );
EnableWindow( w, is_enabled );

is_enabled = TRUE;
if ( !is_this_computer && (CurrentPasswordLevel < EditOtherComputerLevel) )
    is_enabled = FALSE;

if ( CurrentPasswordLevel < ENGINEER_PASSWORD_LEVEL )
    is_enabled = FALSE;

w = GetDlgItem( MainDialogWindow, CREATE_PART_BUTTON  );
EnableWindow( w, is_enabled );

is_enabled = FALSE;
required_password_level = ENGINEER_PASSWORD_LEVEL;
cp = get_ini_string(EditPartIniFile, ConfigSection, SaveAllPartsLevelKey );
if ( !unknown(cp) )
    required_password_level = (short) asctoint32( cp );
if ( CurrentPasswordLevel >= required_password_level )
    is_enabled = TRUE;

w = GetDlgItem( MainDialogWindow, SAVE_ALL_PARTS_BUTTON  );
EnableWindow( w, is_enabled );

is_enabled = FALSE;

cp = get_ini_string(EditPartIniFile, ConfigSection, SetupSheetLevelKey );
if ( !unknown(cp) )
    {
    required_password_level = (short) asctoint32( cp );
    if ( CurrentPasswordLevel < required_password_level )
        is_enabled = TRUE;
    w = GetDlgItem( WindowInfo[SETUP_SHEET_TAB].dialogwindow, SETUP_EBOX );
    SendMessage( w, EM_SETREADONLY, (WPARAM) (BOOL) is_enabled, 0 );
    }
}

/***********************************************************************
*                     SAVE_EDIT_CONTROL_TO_FILE                        *
***********************************************************************/
static void save_edit_control_to_file( TCHAR * file_name, HWND w, UINT id )
{
HANDLE  fh;
TCHAR * s;
DWORD   slen;
DWORD   bytes_to_write;
DWORD   bytes_written;

const DWORD access_mode  = GENERIC_WRITE;
const DWORD share_mode   = 0;
const DWORD create_flags = CREATE_ALWAYS;

w = GetDlgItem( w, id);
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
            CloseHandle( fh );
            }
        }
    delete[] s;
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
DWORD   max_slen;
BOOL    status;

const DWORD access_mode  = GENERIC_READ;
const DWORD share_mode   = FILE_SHARE_READ | FILE_SHARE_WRITE;
const DWORD create_flags = OPEN_EXISTING;

w = GetDlgItem( w, id );
max_slen = SendMessage(  w, EM_GETLIMITTEXT, 0, 0 );

fh = CreateFile( file_name, access_mode, share_mode, NULL, create_flags, FILE_FLAG_SEQUENTIAL_SCAN, 0 );

if ( fh != INVALID_HANDLE_VALUE )
    {
    bytes_to_read = GetFileSize( fh, 0 );
    slen = bytes_to_read/sizeof(TCHAR);
    if ( slen > max_slen )
        slen = max_slen;

    s = maketext( slen+1 );
    if ( s )
        {
        status = ReadFile( fh, s, bytes_to_read, &bytes_read, 0 );
        if ( status && bytes_read > 0 )
            {
            *(s+slen) = NullChar;
            set_text( w, s );
            SendMessage( w, EM_SETMODIFY, (WPARAM) (UINT) FALSE, 0 );
            }
        delete[] s;
        }
    CloseHandle( fh );
    }
}

/***********************************************************************
*                           SHOW_SETUP_SHEET                           *
***********************************************************************/
static void show_setup_sheet()
{
HWND         w;
STRING_CLASS s;
TCHAR        sorc[MAX_PATH+1];
BOOL         have_file;

if ( !HaveCurrentPart )
    return;

w = WindowInfo[SETUP_SHEET_TAB].dialogwindow;

s = setup_sheet_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

have_file = file_exists( s.text() );
if ( !have_file )
    {
    if ( get_exe_directory(sorc) )
        {
        append_filename_to_path( sorc, SETUP_SHEET_FILE );
        have_file = CopyFile( sorc, s.text(), TRUE );
        }
    }

if ( have_file )
    load_edit_control_from_file( w, SETUP_EBOX, s.text() );
else
    set_text( w, SETUP_EBOX, EmptyString );
}

/***********************************************************************
*                  ZERO_BREAK_GAIN_CHANGED_DIALOG_PROC                 *
***********************************************************************/
BOOL CALLBACK zero_break_gain_changed_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
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
*                        MISSING_FILE_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK missing_file_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MissingFileDesc.set_text( hWnd, MISSING_FILE_DESC_TBOX );
        MissingFileAction.set_text( hWnd, MISSING_FILE_ACTION_TBOX );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
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
*                       SHOW_MISSING_FILE_DIALOG                       *
***********************************************************************/
static void show_missing_file_dialog( int desc_id, int action_id )
{
MissingFileDesc = resource_string( desc_id );
MissingFileAction = resource_string( action_id );

DialogBox(
    MainInstance,
    TEXT("MISSING_FILE_DIALOG"),
    MainDialogWindow,
    (DLGPROC) missing_file_dialog_proc );
}

/***********************************************************************
*               UPDATE_SURETRAK_PARAMS_WITH_FT2_SETTINGS               *
***********************************************************************/
static void update_suretrak_params_with_ft2_settings()
{
static TCHAR * key[] = { LS1DescriptionKey, LS2DescriptionKey, LS3DescriptionKey, LS4DescriptionKey, LS5DescriptionKey, LS6DescriptionKey };
int        i;
INI_CLASS  ini;
NAME_CLASS s;

s = ftii_editor_settings_name( CurrentPart.computer, CurrentPart.machine );
if ( !s.file_exists() )
    return;

ini.set_file( s.text() );
ini.set_section( ConfigSection );

for( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    if ( ini.find(key[i]) )
        {
        s = ini.get_string();
        if ( s != unknown_string() )
            SureTrakParam.set_limit_switch_desc( i, s.text() );
        }
    }

if ( ini.find(VacuumLimitSwitchKey) )
    SureTrakParam.set_vacuum_limit_switch_number( ini.get_int() );
if ( ini.find(MinLowImpactPercentKey) )
    SureTrakParam.set_min_low_impact_percent( (float) ini.get_double() );
if ( ini.find(MaxVelocityKey) )
    SureTrakParam.set_max_velocity( (float) ini.get_double() );
}

/***********************************************************************
*                GET_CURRENT_LIMIT_SWITCH_WIRE_NUMBERS                 *
***********************************************************************/
static void get_current_limit_switch_wire_numbers()
{
if ( CurrentMachine.is_ftii )
    CurrentLimitSwitchWire.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
else
    CurrentLimitSwitchWire.empty();
}

/***********************************************************************
*                            GET_CURRENT_PART                          *
***********************************************************************/
static void get_current_part()
{
short          i;
short          mysensor;
WPARAM         x;
TCHAR          mypart[PART_NAME_LEN+1];
TCHAR        * cp;
HWND           w;
LISTBOX_CLASS  lb;
NAME_CLASS     s;
CHAXIS_LABEL_CLASS label;

if ( !get_lb_select(MainDialogWindow, PART_LISTBOX_ID, mypart) )
    return;

s.get_ftchan_file_name( CurrentMachine.computer, CurrentMachine.name, mypart );
if ( !s.file_exists() )
    show_missing_file_dialog( MISSING_FILE_DESC_STRING, MISSING_FILE_ACTION_STRING );


if ( !CurrentPart.find(CurrentMachine.computer, CurrentMachine.name, mypart) )
    return;

/*
---------------------------------------------------------------------------------------
Make a copy of the sensors so I can check for changes when I save.
(This is only necessary because the extended sensors screen saves directly to the part)
--------------------------------------------------------------------------------------- */
for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    {
    mysensor = CurrentPart.analog_sensor[i];
    OriginalAnalogSensor[i] = mysensor;
    if ( !analog_sensor_exists(mysensor) )
        {
        s = resource_string( NO_ANALOG_SENSOR_START_STRING );
        s += i;
        s += resource_string( NO_ANALOG_SENSOR_END_STRING );
        MessageBox( 0, resource_string(NO_ANALOG_SENSOR_LINE2_STRING), s.text(), MB_OK | MB_SYSTEMMODAL );
        mysensor = DEFAULT_ANALOG_SENSOR;
        if ( CurrentMachine.is_ftii )
            mysensor = DEFAULT_FT2_ANALOG_SENSOR;
        CurrentPart.analog_sensor[i] = mysensor;
        }
    }

HaveCurrentPart = TRUE;

refresh_basic_setup();

CurrentFtAnalog.load(CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
----------------
Parameter Limits
---------------- */
CurrentSort.load( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

if ( CurrentParam.find(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
    refresh_parameter_limits();

if ( CurrentMarks.load(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
    show_marks();

SureTrakParam.find( CurrentPart.computer);
if ( CurrentMachine.is_ftii )
    {
    /*
    ----------------------------------------------------------------------
    This is a ft2 machine, get the settings that I need from the ft2 files
    ---------------------------------------------------------------------- */
    update_suretrak_params_with_ft2_settings();
    }

get_current_limit_switch_wire_numbers();

SureTrakSetup.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
PressureControl.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
SureTrakHasChanged = FALSE;

if ( HaveStModTbox.is_visible() )
    HaveStModTbox.hide();

if ( HaveSureTrakControl && !check_low_impact_values() )
    resource_message_box( MainInstance, SOME_LI_CHANGED_STRING, INVALID_LI_ENTRY_STRING, MB_OK | MB_SYSTEMMODAL );

refresh_user_defined_positions();
refresh_advanced_setup();
refresh_suretrak_setup();
refresh_pressure_control_setup();

/*
---------------------------
Get the channel axis labels
--------------------------- */
label.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
---------------------------------------
Set the analog sensors for each channel
--------------------------------------- */
w = WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow;
for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    cp = analog_sensor_description( CurrentPart.analog_sensor[i] );
    if ( cp )
        {
        lb.init( w, FT_CHANNEL_1_LISTBOX+i );
        lb.setcursel( cp );

        /*
        ---------------------------
        Set the channel axis labels
        --------------------------- */
        if ( lstrlen(label[i]) > 0 )
            set_text( w, CHAN_1_AXIS_LABEL_EBOX+i, label[i] );
        else
            set_text( w, CHAN_1_AXIS_LABEL_EBOX+i, resource_string(CHANNEL_1_LABEL_STRING+i) );
        }
    }

/*
------------------------------------------------------------------------------
If the extended channels dialog is visible I need to load the current settings
------------------------------------------------------------------------------ */
if ( ExtendedChannelsDialog )
    load_extended_channels();

/*
--------------------------------------
Set the head and rod pressure channels
-------------------------------------- */
if ( CurrentPart.head_pressure_channel == NO_FT_CHANNEL )
    x = HEAD_PRES_NO_CHAN_BUTTON;
else
    x = HEAD_PRES_CHAN_1_BUTTON + CurrentPart.head_pressure_channel - 1;
CheckRadioButton( w, HEAD_PRES_CHAN_1_BUTTON, HEAD_PRES_NO_CHAN_BUTTON, x );
if ( CurrentPart.rod_pressure_channel == NO_FT_CHANNEL )
    x = ROD_PRES_NO_CHAN_BUTTON;
else
    x = ROD_PRES_CHAN_1_BUTTON + CurrentPart.rod_pressure_channel - 1;
CheckRadioButton( w, ROD_PRES_CHAN_1_BUTTON, ROD_PRES_NO_CHAN_BUTTON, x );

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    set_text( w, LS1_DESC_TBOX+i, SureTrakParam.limit_switch_desc(i) );

/*
-------------------
Get the Setup Sheet
------------------- */
show_setup_sheet();

enable_buttons();
}

/***********************************************************************
*                          FILL_PART_LISTBOX                           *
***********************************************************************/
short fill_part_listbox( HWND w, int listbox_id, MACHINE_ENTRY * m )
{
PART_ENTRY    * p;
short           n;
LISTBOX_CLASS   lb;

lb.init( w, listbox_id );

n = 0;
lb.empty();

m->partlist.rewind();
while ( TRUE )
    {
    p = (PART_ENTRY *) m->partlist.next();
    if ( !p )
        break;
    lb.add( p->name );
    n++;
    }

return n;
}

/***********************************************************************
*                        HIDE_FASTRAK_CHANNELS                         *
***********************************************************************/
static void hide_fastrak_channels()
{
static UINT eid[] = {
    FT_CHANNEL_5_LISTBOX, FT_CHANNEL_6_LISTBOX, FT_CHANNEL_7_LISTBOX, FT_CHANNEL_8_LISTBOX,
    CHAN_5_TBOX, CHAN_6_TBOX, CHAN_7_TBOX, CHAN_8_TBOX
    };
const int nof_eids = sizeof(eid)/sizeof(UINT);

static UINT id[] = {
    HEAD_PRES_CHAN_5_BUTTON, HEAD_PRES_CHAN_6_BUTTON, HEAD_PRES_CHAN_7_BUTTON, HEAD_PRES_CHAN_8_BUTTON,
    ROD_PRES_CHAN_5_BUTTON, ROD_PRES_CHAN_6_BUTTON, ROD_PRES_CHAN_7_BUTTON, ROD_PRES_CHAN_8_BUTTON,
    CHAN_5_AXIS_LABEL_EBOX, CHAN_6_AXIS_LABEL_EBOX, CHAN_7_AXIS_LABEL_EBOX, CHAN_8_AXIS_LABEL_EBOX
    };
const int nof_ids = sizeof(id)/sizeof(UINT);

WINDOW_CLASS w;
WINDOW_CLASS c;
int i;

w = WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow;
for ( i=0; i<nof_eids; i++ )
    {
    c = w.control( eid[i] );
    c.enable( CurrentMachine.is_ftii );
    }

for ( i=0; i<nof_ids; i++ )
    {
    c = w.control( id[i] );
    if ( CurrentMachine.is_ftii )
        c.show();
    else
        c.hide();
    }
}

/***********************************************************************
*                               FILL_PARTS                             *
***********************************************************************/
static void fill_parts()
{
static bool firstime = true;
MACHINE_ENTRY * m;
TCHAR           mymachine[MACHINE_NAME_LEN+1];
int             n;

HaveCurrentPart = FALSE;

if ( get_cb_select(MainDialogWindow, MACHINE_LISTBOX_ID, mymachine) )
    {
    m = find_machine_entry( mymachine );
    if ( m )
        {
        CurrentMachine.find( m->computer, m->name );

        /*
        --------------------------------------------------
        See if the number of editable channels has changed
        -------------------------------------------------- */
        n = MAX_EDITABLE_FT_CHANNELS;
        if ( CurrentMachine.is_ftii )
            n = MAX_EDITABLE_FTII_CHANNELS;

        if ( firstime || n != Nof_Editable_Channels )
            {
            firstime = false;
            Nof_Editable_Channels = n;
            hide_fastrak_channels();
            }
        fill_part_listbox( MainDialogWindow, PART_LISTBOX_ID, m );
        }
    }
}

/***********************************************************************
*                               FILL_PARTS                             *
*          Call with i = NO_INDEX for the current monitor part.        *
***********************************************************************/
static void fill_parts( INT i )
{
LISTBOX_CLASS   lb;

fill_parts();

lb.init( MainDialogWindow, PART_LISTBOX_ID );
if ( i == NO_INDEX )
    {
    /*
    -------------------------------------------------
    Load the current part if I can, else default to 0
    ------------------------------------------------- */
    if ( !lb.setcursel(CurrentMachine.current_part) )
        i = 0;
    }

if ( i != NO_INDEX )
    lb.setcursel( i );

get_current_part();
}

/***********************************************************************
*                              FILL_PARTS                              *
***********************************************************************/
static void fill_parts( TCHAR * part )
{
LISTBOX_CLASS   lb;

fill_parts();

lb.init( MainDialogWindow, PART_LISTBOX_ID );
lb.setcursel( part );
get_current_part();
}

/***********************************************************************
*                         FILL_MACHINE_CB                              *
*                 Return the number of machines found                  *
***********************************************************************/
static short fill_machine_cb( HWND w, int listbox_id )
{
MACHINE_ENTRY * m;
LISTBOX_CLASS   lb;

lb.init( w, listbox_id );
lb.empty();

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;
    lb.add( m->name );
    }

return (short) lb.count();
}

/***********************************************************************
*                         FILL_MACHINE_CB                              *
*                 Return the number of machines found                  *
***********************************************************************/
static short fill_machine_cb( HWND w, int listbox_id, TCHAR * machine_name )
{
MACHINE_ENTRY * m;
LISTBOX_CLASS   lb;
INT             i;
BOOLEAN         have_machine;

have_machine = FALSE;

lb.init( w, listbox_id );
lb.empty();

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;
    lb.add( m->name );
    if ( strings_are_equal( m->name, machine_name, MACHINE_NAME_LEN) )
        have_machine = TRUE;
    }

i = lb.count();

if ( have_machine )
    lb.setcursel( machine_name );
else if ( i > 0 )
    lb.setcursel( 0 );

return (short) i;
}

/***********************************************************************
*                             FILL_MACHINES                            *
***********************************************************************/
static void fill_machines()
{
TCHAR startup_machine[MACHINE_NAME_LEN+1];
TCHAR startup_part[PART_NAME_LEN+1];
TCHAR * cp;
LISTBOX_CLASS lb;
BOOLEAN have_startup_part;

have_startup_part = get_startup_part( startup_machine, startup_part );

if ( !have_startup_part )
    {
    cp = get_ini_string(EditPartIniFile, ConfigSection, LastMachineKey );
    if ( !unknown(cp) )
        {
        copystring( startup_machine, cp );
        cp = get_ini_string(EditPartIniFile, ConfigSection, LastPartKey );
        if ( !unknown(cp) )
            {
            copystring( startup_part, cp );
            have_startup_part = TRUE;
            }
        }
    }

if ( fill_machine_cb(MainDialogWindow, MACHINE_LISTBOX_ID, startup_machine) )
    {
    if ( have_startup_part )
        fill_parts( startup_part );
    else
        fill_parts( NO_INDEX );
    }
}

/***********************************************************************
*                          CREATE_TAB_CONTROL                          *
***********************************************************************/
static void create_tab_control()
{
TC_ITEM tie;
int i;
int tab_item_number;
TCHAR buf[MAX_TAB_LEN+1];

TabControl = GetDlgItem( MainDialogWindow, TAB_CONTROL_ID );
if ( TabControl == NULL)
    return;

tie.mask = TCIF_TEXT | TCIF_IMAGE;
tie.iImage = -1;
tie.pszText = buf;

for ( i=0; i<(TAB_COUNT); i++ )
    {
    if ( !HaveSureTrakControl )
        if ( i == SURETRAK_PROFILE_TAB || i == PRESSURE_CONTROL_TAB )
            continue;

    if ( !HavePressureControl )
        if ( i == PRESSURE_CONTROL_TAB )
            continue;

    if ( i == SETUP_SHEET_TAB )
        if ( *get_ini_string(EditPartIniFile, ConfigSection, ShowSetupSheetKey) != YChar )
            continue;

    LoadString( MainInstance, FIRST_TAB_STRING + i, buf, sizeof(buf) );
    /*
    ----------------------------------------------------------------------------------
    If there is no suretrak then the actual tab number is one less for the setup sheet
    ---------------------------------------------------------------------------------- */
    tab_item_number = i;
    if ( i == SETUP_SHEET_TAB && !HaveSureTrakControl )
        tab_item_number -= 2;
    else if ( i == SETUP_SHEET_TAB && !HavePressureControl )
        tab_item_number--;

    if ( TabCtrl_InsertItem(TabControl, tab_item_number, &tie) == -1 )
        {
        DestroyWindow(  TabControl);
        TabControl = 0;
        }
    }

show_tab_control();
}

/***********************************************************************
*                          SHOW_CONTEXT_HELP                           *
***********************************************************************/
static BOOL show_context_help( HWND w, LPARAM lParam )
{
const int32 NOF_CONTROLS = 3;
const int32 NOF_STATIC_CONTROLS = 5;

static UINT myid[] = {
    TOTAL_STROKE_LEN_EDITBOX,
    MIN_STROKE_LEN_EDITBOX,
    EOS_VEL_EDITBOX };

static UINT static_id[] = {
    DIST_UNITS_STATIC,
    VEL_UNITS_STATIC,
    TOTAL_STROKE_STATIC,
    MIN_STROKE_STATIC,
    EOS_STATIC };

static DWORD mycontext[] = {
    TOTAL_STROKE_HELP,
    MIN_STROKE_HELP,
    EOS_HELP };

static DWORD static_context[] = {
    DIST_UNITS_HELP,
    VEL_UNITS_HELP,
    TOTAL_STROKE_HELP,
    MIN_STROKE_HELP,
    EOS_HELP };

int32 i;
POINT p;
HWND  bw;
HWND  sw;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

bw = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    sw = GetDlgItem(bw, static_id[i] );
    if ( GetWindowRect(sw, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            gethelp( HELP_CONTEXTPOPUP, static_context[i] );
            return TRUE;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(bw, myid[i]) == w )
        {
        gethelp( HELP_CONTEXTPOPUP, mycontext[i] );
        return TRUE;
        }
    }

get_overview_help();
return TRUE;
}

static short DestUnitsId = NO_UNITS;
static short SorcUnitsId = NO_UNITS;

/***********************************************************************
*                             FLOAT_ADJUST                             *
***********************************************************************/
static void float_adjust( float & x )
{

x = adjust_for_units( DestUnitsId, x, SorcUnitsId );

}

/***********************************************************************
*                             FLOAT_ADJUST                             *
***********************************************************************/
static void float_adjust( double & x )
{

x = adjust_for_units( DestUnitsId, x, SorcUnitsId );

}


/***********************************************************************
*                        CHANGE_DISTANCE_UNITS                         *
***********************************************************************/
static void change_distance_units()
{
int   old_button_index;
int   new_button_index;
int   i;
int   j;
int   n;
HWND  w;

w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

if ( !w )
    return;

new_button_index = NO_INDEX;

for ( i=0; i<NofDistanceButtons; i++ )
    {
    if ( is_checked(w, DistanceButton[i]) )
        {
        new_button_index = i;
        break;
        }
    }

if ( new_button_index == NO_INDEX )
    return;

old_button_index  = new_button_index;

/*
-----------------------
Get the previous button
----------------------- */
for ( i=0; i<NofDistanceButtons; i++ )
    {
    if ( DistanceButtonState[i] )
        {
        old_button_index = i;
        break;
        }
    }

if ( old_button_index == new_button_index )
    return;

DestUnitsId = DistanceUnits[new_button_index];
SorcUnitsId = DistanceUnits[old_button_index];

CurrentPart.distance_units = DestUnitsId;

float_adjust( CurrentPart.total_stroke_length );
float_adjust( CurrentPart.min_stroke_length   );

refresh_basic_setup();

float_adjust( CurrentPart.sleeve_fill_distance );
float_adjust( CurrentPart.runner_fill_distance );
float_adjust( CurrentPart.csfs_min_position    );
float_adjust( CurrentPart.plunger_diameter     );

refresh_advanced_setup();

for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
    float_adjust( CurrentPart.limit_switch_position[i] );

for ( i=0; i<USER_VEL_COUNT; i++ )
    float_adjust( CurrentPart.user_velocity_position[i] );

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    float_adjust( CurrentPart.user_avg_velocity_end[i]   );
    float_adjust( CurrentPart.user_avg_velocity_start[i] );
    }

refresh_user_defined_positions();

n = CurrentParam.count();
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].vartype & (DISTANCE_VAR | POSITION_VAR) )
        {
        SorcUnitsId = CurrentParam.parameter[i].units;
        for ( j=0; j<NOF_ALARM_LIMIT_TYPES; j++ )
            float_adjust( CurrentParam.parameter[i].limits[j].value );
        CurrentParam.parameter[i].units = DestUnitsId;
        }
    }

refresh_parameter_limits();
}

/***********************************************************************
*                        CHANGE_VELOCITY_UNITS                         *
***********************************************************************/
static void change_velocity_units()
{
int   old_button_index;
int   new_button_index;
int   i;
int   j;
int   n;
HWND  w;

w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

if ( !w )
    return;

new_button_index = NO_INDEX;

for ( i=0; i<NofVelocityButtons; i++ )
    {
    if ( is_checked(w, VelocityButton[i]) )
        {
        new_button_index = i;
        break;
        }
    }

if ( new_button_index == NO_INDEX )
    return;

old_button_index  = new_button_index;

/*
-----------------------
Get the previous button
----------------------- */
for ( i=0; i<NofVelocityButtons; i++ )
    {
    if ( VelocityButtonState[i] )
        {
        old_button_index = i;
        break;
        }
    }

if ( old_button_index == new_button_index )
    return;

DestUnitsId = VelocityUnits[new_button_index];
SorcUnitsId = VelocityUnits[old_button_index];

CurrentPart.velocity_units = DestUnitsId;

float_adjust( CurrentPart.eos_velocity );

refresh_basic_setup();

float_adjust( CurrentPart.csfs_min_velocity );
float_adjust( CurrentPart.csfs_rise_velocity );

refresh_advanced_setup();

n = CurrentParam.count();
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].vartype & VELOCITY_VAR )
        {
        SorcUnitsId = CurrentParam.parameter[i].units;
        for ( j=0; j<NOF_ALARM_LIMIT_TYPES; j++ )
            float_adjust( CurrentParam.parameter[i].limits[j].value );
        CurrentParam.parameter[i].units = DestUnitsId;
        }
    }

refresh_parameter_limits();
}

/***********************************************************************
*                              IS_A_STEP                               *
*                                                                      *
* This checks to see if there is a number in one of the three          *
* editboxes begining with the id sent. I assume box is the id of the   *
* accel editbox.                                                       *
*                                                                      *
***********************************************************************/
BOOLEAN is_a_step( HWND w, int box  )
{
int32   i;
TCHAR   buf[STSTEPS_FLOAT_LEN+1];
TCHAR * cp;

for ( i=0; i<3; i++ )
    {
    if ( get_text(buf, w, box, STSTEPS_FLOAT_LEN) )
        {
        cp = buf;
        while ( *cp != NullChar )
            {
            if ( IsCharAlphaNumeric(*cp) && !IsCharAlpha(*cp) )
                return TRUE;
            cp++;
            }
        }
    box++;
    }

return FALSE;
}

/***********************************************************************
*                      GET_NEW_SURETRAK_STEPS                          *
***********************************************************************/
static void get_new_suretrak_steps()
{
HWND  w;
int32 i;
int   box;
TCHAR buf[MAX_FLOAT_LEN+1];

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

box = ACCEL_1_EDITBOX;
SureTrakSetup.clear_steps();
for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    if ( !is_a_step(w, box) )
        break;

    SureTrakSetup.append();

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_accel( buf );
    box++;

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_vel( buf );
    box++;

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_end_pos( buf );
    box++;

    SureTrakSetup.set_have_vacuum_wait( is_checked(w, box) );
    box++;

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_low_impact( buf );
    box++;

    SureTrakSetup.set_have_low_impact_from_input( is_checked(w, box) );
    box++;

    if ( SureTrakSetup.accel_is_percent() && SureTrakSetup.vel_is_percent() )
        resource_message_box( MainInstance, NO_PERCENT_PERCENT_STRING, INVALID_ENTRY_STRING, MB_OK );
    }

box = LS1_EDITBOX;
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    get_text(buf, w, box, STLIMITS_FLOAT_LEN );
    SureTrakSetup.set_limit_switch_pos( buf, i );
    box++;
    }

SureTrakHasChanged = TRUE;
}

/***********************************************************************
*                          IS_VACUUM_CHECKBOX                          *
***********************************************************************/
static BOOLEAN is_vacuum_checkbox( int id )
{
int i;
int bid;

bid = VW_1_CHECKBOX;
for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    if ( bid == id )
        return TRUE;

    bid += CONTROLS_PER_STEP;
    }

return FALSE;
}

/***********************************************************************
*                      CHECK_VACUUM_WAIT_BUTTONS                       *
***********************************************************************/
static void check_vacuum_wait_buttons( HWND w, int id )
{
int i;
int bid;

if ( !is_vacuum_checkbox(id) )
    return;

/*
---------------------------------------------------------
If this button was just unchecked, there is nothing to do
--------------------------------------------------------- */
if ( !is_checked(w, id) )
    {
    /*
    ---------------------------------------------
    Enable the end position editbox for this step
    ShowWindow( GetDlgItem(w, id-1), SW_SHOW );
    --------------------------------------------- */

    /*
    ------------------------------------
    Erase the fast shot position message
    ------------------------------------ */
    ShowWindow( GetDlgItem(w, FS_POSITION_MSG_TBOX), SW_HIDE );

    /*
    ----------------------------------
    Redisplay the limit switch editbox
    ---------------------------------- */
    i = SureTrakParam.vacuum_limit_switch_number();
    if ( i != NO_INDEX )
        InvalidateRect( GetDlgItem(w, LS1_EDITBOX+i), 0, TRUE );

    return;
    }

bid = VW_1_CHECKBOX;
for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    if ( bid != id )
        {
        if ( is_checked(w, bid) )
            {
            set_checkbox( w, bid, FALSE );
            EnableWindow( GetDlgItem(w, bid-1), TRUE );
            }

        }

    bid += CONTROLS_PER_STEP;
    }

//ShowWindow( GetDlgItem(w, id-1), SW_HIDE );
ShowWindow( GetDlgItem(w, FS_POSITION_MSG_TBOX), SW_SHOWNORMAL );

i = SureTrakParam.vacuum_limit_switch_number();
if ( i != NO_INDEX )
    {
    InvalidateRect( GetDlgItem(w, LS1_EDITBOX+i), 0, TRUE );

    /*
    --------------------------------
    See if the limit switch is empty
    -------------------------------- */
    i = SureTrakParam.vacuum_limit_switch_number();
    if ( is_empty(SureTrakSetup.limit_switch_pos_string(i)) )
        {
        SetFocus( GetDlgItem(w, LS1_EDITBOX+i) );
        resource_message_box( w, MainInstance, NO_START_OF_FS_LS_STRING, SHOULD_BE_ENTERED_STRING, MB_OK | MB_SYSTEMMODAL );
        }
    }
}

/***********************************************************************
*                            UPDATE_ST_PARAMS                          *
***********************************************************************/
static void update_st_params( HWND parent, int id )
{
STRING_CLASS s;

if ( s.get_text(parent, id) )
    {
    switch ( id )
        {
        case VEL_LOOP_GAIN_EBOX:
            SureTrakSetup.set_velocity_loop_gain( s.float_value() );
            break;

        case FT_STOP_POS_EBOX:
            SureTrakSetup.set_ft_stop_pos( s.float_value() );
            break;

        case BREAK_GAIN_1_EBOX:
            SureTrakSetup.set_gain_break_gain(0, s.float_value() );
            break;

        case BREAK_VEL_1_EBOX:
            SureTrakSetup.set_gain_break_velocity(0, s.float_value() );
            break;

        case BREAK_GAIN_2_EBOX:
            SureTrakSetup.set_gain_break_gain(1, s.float_value() );
            break;

        case BREAK_VEL_2_EBOX:
            SureTrakSetup.set_gain_break_velocity(1, s.float_value() );
            break;

        case BREAK_GAIN_3_EBOX:
            SureTrakSetup.set_gain_break_gain(2, s.float_value() );
            break;

        case BREAK_VEL_3_EBOX:
            SureTrakSetup.set_gain_break_velocity(2, s.float_value() );
            break;
        }
    }
else
    {
    switch ( id )
        {
        case FT_STOP_POS_EBOX:
            SureTrakSetup.set_ft_stop_pos( 0.0 );
            break;
        }
    }

SureTrakHasChanged = TRUE;
}

/***********************************************************************
*                       ST_PARAMS_CONTEXT_HELP                         *
***********************************************************************/
static void st_params_context_help( HWND parent, LPARAM lParam )
{
static HELP_ENTRY static_help[] = {
      { VEL_LOOP_GAIN_STATIC,             SURETRAK_VEL_LOOP_GAIN_HELP  },
      { BREAK_VEL_STATIC,                 SURETRAK_BREAK_VEL_HELP      },
      { BREAK_SETTINGS_STATIC,            SURETRAK_BREAK_VEL_HELP      },
      { BREAK_GAIN_STATIC,                SURETRAK_BREAK_GAIN_HELP     }
      };

const int32 NOF_STATIC_CONTROLS = sizeof(static_help)/sizeof(HELP_ENTRY);

int32 i;
POINT p;
HWND  w;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    w = GetDlgItem( parent, static_help[i].id );
    if ( GetWindowRect(w, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            gethelp( HELP_CONTEXTPOPUP, static_help[i].context );
            return;
            }
        }
    }

gethelp( HELP_CONTEXT, SURETRAK_PART_PARAMETER_HELP );
}

/***********************************************************************
*                           ADD_TO_STMODLIST                           *
***********************************************************************/
static void add_to_stmodlist( HWND w )
{
/*
------------------------------------------------------------
Show the unsaved changes message if this is the first change
------------------------------------------------------------ */
if ( !StModList.count() )
    HaveStModTbox.show();

if ( !StModList.contains(w) )
    {
    StModList.add( w );
    InvalidateRect( w, 0, TRUE );
    }
}

/***********************************************************************
*                  POSITION_MULTIPART_RUNLIST_DIALOG                   *
***********************************************************************/
static void position_multipart_runlist_dialog( HWND w )
{
RECTANGLE_CLASS r;
WINDOW_CLASS    wc;
long dx;
long dy;

wc = GetDlgItem( WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow, EDIT_MULTIPART_RUNLIST_BUTTON );
wc.getrect( r );

dx = r.left;
dy = r.top;

wc = GetDlgItem( w, HPM_1_CBOX );
wc.getrect( r );

dx -= r.left;
dy -= r.top;

wc = w;
wc.offset( dx, dy );
}

/***********************************************************************
*                         FILL_RUNLIST_CBOXES                          *
***********************************************************************/
static void fill_runlist_cboxes( HWND w )
{
TEXT_LIST_CLASS t;
unsigned int    i;
int             id;
LISTBOX_CLASS   lb;
STRING_CLASS    s;
FILE_CLASS      f;
TCHAR         * cp;
WINDOW_CLASS    wc;

wc = w;

/*
----------------------------------
Load the labels if there is a file
---------------------------------- */
s = multipart_runlist_labels_file();
if ( f.open_for_read(s) )
    {
    cp = f.readline();
    if ( cp )
        wc.set_title( cp );
    id = HPM_0_TBOX;
    while ( cp )
        {
        cp = f.readline();
        if ( cp )
            set_text( w, id, cp );
        id++;
        }

    f.close();
    }

if ( get_current_partlist(t) )
    {
    for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
        {
        lb.init( w, HPM_0_CBOX+i );
        lb.add( NotUsedString.text() );
        t.rewind();
        while ( t.next() )
            {
            lb.add( t.text() );
            }
        if ( MultipartRunlist.partname[i].isempty() )
            lb.setcursel( NotUsedString.text() );
        else
            lb.setcursel( MultipartRunlist.partname[i].text() );
        }
    }
}

/***********************************************************************
*                   RECORD_MULTIPART_RUNLIST_CHANGES                   *
***********************************************************************/
static void record_multipart_runlist_changes( HWND w )
{
int i;
LISTBOX_CLASS lb;
STRING_CLASS s;

for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
    {
    lb.init( w, HPM_0_CBOX+i );
    s = lb.selected_text();
    if ( s == NotUsedString )
        s = unknown_string();
    MultipartRunlist.partname[i] = s;
    }

MultipartRunlistHasChanged = TRUE;

/*
--------------------------------
Refill the listbox on the parent
-------------------------------- */
w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

lb.init( w, WARMUP_PART_CBOX );
lb.empty();

for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
    {
    if ( unknown(MultipartRunlist.partname[i].text()) )
        lb.add( NotUsedString.text() );
    else
        lb.add( MultipartRunlist.partname[i].text() );
    }

lb.setcursel( 0 );

add_to_stmodlist( lb.handle() );
}

/***********************************************************************
*                      MULTIPART_RUNLIST_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK multipart_runlist_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        position_multipart_runlist_dialog( hWnd );
        fill_runlist_cboxes( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                record_multipart_runlist_changes( hWnd );

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
*                        MSDOS_BACKUP_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK msdos_backup_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
BOOLEAN status;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        if ( id == IDOK )
            {
            hourglass_cursor();
            status = save_dos_suretrak( CurrentPart, SureTrakSetup, SureTrakParam, hWnd );
            restore_cursor();
            if ( !status )
                resource_message_box( MainInstance, FLOPPY_WRITE_ERROR_STRING, CANT_DO_THAT_STRING, MB_OK );
            }

        if ( id == IDOK || id == IDCANCEL )
            {
            EndDialog( hWnd, 0 );
            return TRUE;
            }
    }

return FALSE;
}

/***********************************************************************
*                    SURETRAK_PROFILE_CONTEXT_HELP                     *
***********************************************************************/
static void suretrak_profile_context_help( HWND cw, LPARAM lParam )
{
const int32 NOF_CONTROLS = 1;

static HELP_ENTRY control_help[] = {
      { DOS_BACKUP_BUTTON,       SURETRAK_MSDOS_BUTTON_HELP }
      };

static HELP_ENTRY static_help[] = {
      { ACCEL_STATIC,                     SURETRAK_ACCEL_POPUP_HELP    },
      { VELOCITY_STATIC,                  SURETRAK_VELOCITY_POPUP_HELP },
      { ENDING_POS_STATIC,                SURETRAK_END_POS_POPUP_HELP  },
      { VACUUM_WAIT_STATIC,               SURETRAK_VAC_WAIT_POPUP_HELP },
      { LOW_IMPACT_STATIC,                SURETRAK_LI_POPUP_HELP       },
      { LOW_IMPACT_FROM_INPUT_STATIC,     SURETRAK_LI_INPUT_HELP       },
      { LIMIT_SWITCH_POS_STATIC,          SURETRAK_LS_HELP             },
      { VEL_LOOP_GAIN_STATIC,             SURETRAK_VEL_LOOP_GAIN_HELP  },
      { BREAK_VEL_STATIC,                 SURETRAK_BREAK_VEL_HELP      },
      { BREAK_GAIN_STATIC,                SURETRAK_BREAK_GAIN_HELP     },
      { WARMUP_GROUP_STATIC,              SURETRAK_WARMUP_SHOT_HELP    },
      { FT_STOP_POS_STATIC,               FOLLOW_THROUGH_STOP_POSITION_HELP },
      { BREAK_2_STATIC,                   SHOT_CONTROL_INTEGRAL_GAIN_HELP },
      { BREAK_3_STATIC,                   SHOT_CONTROL_DERIVATIVE_GAIN_HELP }
      };

const int32 NOF_STATIC_CONTROLS = sizeof(static_help)/sizeof(HELP_ENTRY);

DWORD mycontext;
int32 i;
POINT p;
HWND  dw;
HWND  w;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

dw = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    w = GetDlgItem( dw, static_help[i].id );
    if ( GetWindowRect(w, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            mycontext = static_help[i].context;
            if ( !CurrentMachine.is_cyclone )
                {
                if ( static_help[i].id == BREAK_2_STATIC || static_help[i].id == BREAK_3_STATIC )
                    mycontext = SURETRAK_BREAK_GAIN_HELP;
                }
            gethelp( HELP_CONTEXTPOPUP, static_help[i].context );
            return;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(dw, control_help[i].id) == cw )
        {
        gethelp( HELP_CONTEXTPOPUP, control_help[i].context );
        return;
        }
    }

w = GetDlgItem( dw, LS4_EDITBOX );
GetWindowRect( w, &r );

/*
----------------------------------------------------------------------
The low word of lParam is the x position of the mouse. If it is to the
right of the limit switch stuff then get the break gain help screen.
---------------------------------------------------------------------- */
if ( LOWORD(lParam) > r.right )
    {
    gethelp( HELP_CONTEXT, SURETRAK_PART_PARAMETER_HELP );
    }
else
    {
    get_overview_help();
    }
}

/***********************************************************************
*                             GET_PERCENTS                             *
***********************************************************************/
static void get_percents( HWND goose_button, PRES_CONTROL_STEP_ENTRY * se )
{
BOOLEAN ramp_contains_percent;

if ( PressureControl.proportional_valve || se->pressure.contains(PercentChar) )
    se->is_percent = TRUE;
else
    se->is_percent = FALSE;

ramp_contains_percent = se->ramp.contains( PercentChar );

if ( !se->is_goose_step && ramp_contains_percent )
    {
    se->is_goose_step = TRUE;
    set_checkbox( goose_button, TRUE );
    }

se->is_goose_percent = FALSE;
if ( se->is_goose_step )
    {
    if ( se->is_percent || ramp_contains_percent )
        se->is_goose_percent = TRUE;
    }
}

/***********************************************************************
*                    GET_NEW_PRESSURE_CONTROL_STEPS                    *
***********************************************************************/
static void get_new_pressure_control_steps()
{
int32 i;
HWND  w;
HWND  goose_button;
LISTBOX_CLASS lb;
PRES_CONTROL_STEP_ENTRY * se;

w = WindowInfo[PRESSURE_CONTROL_TAB].dialogwindow;

if ( !w )
    return;

lb.init( w, ST_PRES_SENSOR_CBOX );

PressureControl.sensor_number = analog_sensor_number( lb.selected_text() );

for ( i=0; i<3; i++ )
    {
    if ( is_checked(w,  PC_2_VOLT_RANGE_RADIO+i) )
        {
        PressureControl.voltage_output_range = i;
        break;
        }
    }

PressureControl.retctl_arm_pres.get_text( w, RETCTL_ARM_PRES_EBOX );
PressureControl.integral_gain.get_text( w, PC_INTEGRAL_GAIN_EBOX );
PressureControl.derivative_gain.get_text( w, PC_DERIVATIVE_GAIN_EBOX );
PressureControl.pressure_loop_gain.get_text( w, PC_LOOP_GAIN_EBOX );
PressureControl.retctl_pres.get_text(w, RETCTL_PRES_EBOX );
PressureControl.retract_volts.get_text(w, RETRACT_PRES_EBOX );
PressureControl.park_volts.get_text(w, PARK_PRES_EBOX );
PressureControl.is_enabled = is_checked( w, PRES_CONTROL_ENABLE_XBOX );
PressureControl.use_only_positive_voltages = is_checked( w, POSITIVE_ONLY_XBOX );
PressureControl.retctl_is_enabled = is_checked( w, RETCTL_ENABLE_XBOX );
PressureControl.proportional_valve = is_checked( w, PROPORTIONAL_VALVE_XBOX );

lb.init( w, PC_ARM_LS_CBOX );
PressureControl.arm_intensifier_ls_number = lb.selected_index() + 1;

se = &PressureControl.step[0];
se->ramp.get_text( w, RAMP_1_EBOX );
goose_button = GetDlgItem( w, PC_GOOSE_1_XBOX );
se->is_goose_step = is_checked( goose_button );
se->pressure.get_text( w, PRES_1_EBOX );
get_percents( goose_button, se );
se->is_open_loop = is_checked( w, PC_OPEN_LOOP_1_XBOX );
se->holdtime.get_text( w, HOLD_TIME_1_EBOX  );

se = &PressureControl.step[1];
se->ramp.get_text( w, RAMP_2_EBOX );
goose_button = GetDlgItem( w, PC_GOOSE_2_XBOX );
se->is_goose_step = is_checked( goose_button );
se->pressure.get_text( w, PRES_2_EBOX );
get_percents( goose_button, se );
se->is_open_loop = is_checked( w, PC_OPEN_LOOP_2_XBOX );
se->holdtime.get_text( w, HOLD_TIME_2_EBOX  );

se = &PressureControl.step[2];
se->ramp.get_text(     w, RAMP_3_EBOX );
goose_button = GetDlgItem( w, PC_GOOSE_3_XBOX );
se->is_goose_step = is_checked( goose_button );
se->pressure.get_text( w, PRES_3_EBOX );
get_percents( goose_button, se );
se->is_open_loop = is_checked( w, PC_OPEN_LOOP_3_XBOX );
se->holdtime.get_text( w, HOLD_TIME_3_EBOX  );

PressureControl.nof_steps = 0;
for ( i=0; i<MAX_PRESSURE_CONTROL_STEPS; i++ )
    {
    if ( !PressureControl.step[i].pressure.isempty() )
        PressureControl.nof_steps = i+1;
    }
}

/***********************************************************************
*                            SETUP_SHEET_PROC                          *
***********************************************************************/
BOOL CALLBACK setup_sheet_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        break;
    }

return FALSE;
}

/***********************************************************************
*                  PRESSURE_CONTROL_CONTEXT_HELP                       *
***********************************************************************/
static BOOL pressure_control_context_help( HWND w, LPARAM lParam )
{

static UINT static_id[] = {
    RAMP_STATIC,
    PRES_STATIC,
    HOLD_TIME_STATIC,
    PRESSURE_LOOP_GAIN_STATIC,
    RETRACT_VOLTS_STATIC,
    VALVE_PARK_VOLTS_STATIC,
    RETCTL_ARM_PRES_TBOX,
    RETCTL_PRES_TBOX,
    ST_PRES_SENSOR_STATIC,
    OUTPUT_VOLTAGE_RANGE_STATIC,
    RETCTL_GBOX
    };
const int32 NOF_STATIC_CONTROLS = sizeof(static_id)/sizeof(UINT);

static DWORD static_context[] = {
    PC_RAMP_HELP,
    PC_PRES_HELP,
    PC_HOLD_TIME_HELP,
    PC_LOOP_GAIN_HELP,
    RETRACT_VOLTS_HELP,
    VALVE_PARK_VOLTS_HELP,
    RETCTL_ARM_PRES_HELP,
    RETCTL_PRES_HELP,
    ST_PRES_SENSOR_HELP,
    OUTPUT_VOLTAGE_RANGE_HELP,
    RETCTL_HELP
    };

static UINT myid[] = {
    RAMP_1_EBOX,
    PC_GOOSE_1_XBOX,
    PRES_1_EBOX,
    PC_OPEN_LOOP_1_XBOX,
    HOLD_TIME_1_EBOX,
    RAMP_2_EBOX,
    PC_GOOSE_2_XBOX,
    PRES_2_EBOX,
    PC_OPEN_LOOP_2_XBOX,
    HOLD_TIME_2_EBOX,
    RAMP_3_EBOX,
    PC_GOOSE_3_XBOX,
    PRES_3_EBOX,
    PC_OPEN_LOOP_3_XBOX,
    HOLD_TIME_3_EBOX,
    PRES_CONTROL_ENABLE_XBOX,
    POSITIVE_ONLY_XBOX,
    RETCTL_ENABLE_XBOX,
    PROPORTIONAL_VALVE_XBOX,
    ALL_OPEN_LOOP_XBOX,
    ST_PRES_SENSOR_CBOX,
    PC_2_VOLT_RANGE_RADIO,
    PC_5_VOLT_RANGE_RADIO,
    PC_10_VOLT_RANGE_RADIO,
    PC_LOOP_GAIN_EBOX,
    RETRACT_PRES_EBOX,
    PARK_PRES_EBOX,
    RETCTL_PRES_EBOX,
    RETCTL_ARM_PRES_EBOX
    };
const int32 NOF_CONTROLS = sizeof(myid)/sizeof(UINT);

static DWORD mycontext[] = {
    PC_RAMP_HELP,
    PC_GOOSE_BUTTON_HELP,
    PC_PRES_HELP,
    PC_OPEN_LOOP_HELP,
    PC_HOLD_TIME_HELP,
    PC_RAMP_HELP,
    PC_GOOSE_BUTTON_HELP,
    PC_PRES_HELP,
    PC_OPEN_LOOP_HELP,
    PC_HOLD_TIME_HELP,
    PC_RAMP_HELP,
    PC_GOOSE_BUTTON_HELP,
    PC_PRES_HELP,
    PC_OPEN_LOOP_HELP,
    PC_HOLD_TIME_HELP,
    PRES_CONTROL_ENABLE_HELP,
    POSITIVE_ONLY_HELP,
    RETCTL_ENABLE_HELP,
    PROPORTIONAL_VALVE_HELP,
    PC_OPEN_LOOP_HELP,
    ST_PRES_SENSOR_HELP,
    OUTPUT_VOLTAGE_RANGE_HELP,
    OUTPUT_VOLTAGE_RANGE_HELP,
    OUTPUT_VOLTAGE_RANGE_HELP,
    PC_LOOP_GAIN_HELP,
    RETRACT_VOLTS_HELP,
    VALVE_PARK_VOLTS_HELP,
    RETCTL_PRES_HELP,
    RETCTL_ARM_PRES_HELP
    };

int32 i;
POINT p;
HWND  bw;
HWND  sw;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

bw = WindowInfo[PRESSURE_CONTROL_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    sw = GetDlgItem(bw, static_id[i] );
    if ( GetWindowRect(sw, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            gethelp( HELP_CONTEXTPOPUP, static_context[i] );
            return TRUE;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(bw, myid[i]) == w )
        {
        gethelp( HELP_CONTEXTPOPUP, mycontext[i] );
        return TRUE;
        }
    }

get_overview_help();
return TRUE;
}

/***********************************************************************
*                        DO_GOOSE_BUTTON_PRESS                         *
***********************************************************************/
static void do_goose_button_press( HWND goose_button, HWND parent, int goose_button_id )
{
HWND ebox;

BOOLEAN goose_button_pressed;
BOOLEAN ramp_is_percent;
STRING_CLASS s;
TCHAR * cp;

ebox = GetDlgItem( parent, goose_button_id-1 );

/*
------------------------------------------------------------------------------------------
At the present time the only time you can use percents is if you have a proportional valve
------------------------------------------------------------------------------------------ */
if ( is_checked(parent, PROPORTIONAL_VALVE_XBOX) )
    {
    s.get_text( ebox );
    ramp_is_percent = s.contains( PercentChar );
    goose_button_pressed = is_checked( goose_button );
    if ( ramp_is_percent && !goose_button_pressed )
        {
        cp = s.find( PercentChar );
        if ( cp )
            *cp = NullChar;
        s.set_text( ebox );
        }
    else if ( goose_button_pressed && !ramp_is_percent )
        {
        if ( !s.isempty() )
            {
            s += PercentChar;
            s.set_text( ebox );
            }
        set_text( goose_button, PercentString );
        return;
        }
    }

label_goose_button( goose_button, FALSE );
}

/***********************************************************************
*                         DO_RAMP_VALUE_CHANGE                         *
*    This checks for a ramp change from % to no % or vise versa.       *
***********************************************************************/
static void do_ramp_value_change( HWND parent, int id )
{
HWND goose_button;
HWND ebox;

BOOLEAN goose_button_pressed;
BOOLEAN ramp_is_percent;
STRING_CLASS s;

ebox = GetDlgItem( parent, id );
goose_button = GetDlgItem( parent, id+1 );

s.get_text( ebox );
ramp_is_percent = s.contains( PercentChar );
goose_button_pressed = is_checked( goose_button );

/*
------------------------------------------------------------------------------------------
At the present time the only time you can use percents is if you have a proportional valve
------------------------------------------------------------------------------------------ */
if ( !is_checked(parent, PROPORTIONAL_VALVE_XBOX) )
    {
    if ( ramp_is_percent )
        {
        set_percent( s, FALSE );
        s.set_text( ebox );
        }
    return;
    }

/*
--------------------------------------------
See if a % has just been added to the string
-------------------------------------------- */
if ( ramp_is_percent && !goose_button_pressed )
    {
    set_checkbox( goose_button, TRUE );
    set_text( goose_button, PercentString );
    }
else if ( goose_button_pressed && !ramp_is_percent )
    {
    set_checkbox( goose_button, FALSE );
    label_goose_button( goose_button, FALSE );
    }
}

/***********************************************************************
*                       PRESSURE_CONTROL_PROFILE_PROC                  *
***********************************************************************/
BOOL CALLBACK pressure_control_profile_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;
int   cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        LoadingPressureControl = TRUE;
        fill_analog_list_boxes( hWnd, ST_PRES_SENSOR_CBOX, 1 );
        LoadingPressureControl = FALSE;
        break;

    case WM_CONTEXTMENU:
        return pressure_control_context_help( (HWND) wParam, lParam );

    case WM_NEW_DATA:
        /*
        ------------------------------------------------------------------------------------------------
        This is used by the ls description screen to tell me the limit switch descriptions have changed.
        ------------------------------------------------------------------------------------------------ */
        load_pc_arm_limit_switch_cbox( hWnd );
        break;

    case WM_COMMAND:
        if ( id == PC_GOOSE_1_XBOX || id == PC_GOOSE_2_XBOX || id == PC_GOOSE_3_XBOX )
            {
            if ( cmd == BN_CLICKED )
                do_goose_button_press( (HWND) lParam, hWnd, id );
            }

        if ( id == PRES_CONTROL_ENABLE_XBOX )
            {
            show_pres_controls( hWnd );
            }

        if ( id == RETCTL_ENABLE_XBOX )
            {
            show_retctl_controls( hWnd );
            }

        if ( id == PROPORTIONAL_VALVE_XBOX )
            toggle_proportional_valve(hWnd);

        if ( id == ALL_OPEN_LOOP_XBOX )
            do_all_open_loop_xbox( hWnd );

        if ( !LoadingPressureControl )
            {
            if ( id == ST_PRES_SENSOR_CBOX && cmd == CBN_SELCHANGE )
                {
                get_new_pressure_control_steps();
                //refresh_pressure_control_plot();
                }
            else if ( cmd == EN_CHANGE || cmd == BN_CLICKED )
                {
                if ( id >= RAMP_1_EBOX && id <= POSITIVE_ONLY_XBOX )
                    {
                    if ( id == RAMP_1_EBOX || id == RAMP_2_EBOX || id == RAMP_3_EBOX )
                        do_ramp_value_change( hWnd, id );
                    get_new_pressure_control_steps();
                    //SureTrakPlot.empty_curves();
                    //add_pressure_control_curve();
                    //SureTrakPlotWindow.refresh();
                    return TRUE;
                    }
                }
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                SAVE_LIMIT_SWITCH_DESCRIPTION_CHANGES                 *
*                                                                      *
* I changed this, 4/18/08 V6.51, so that I save the changes to the     *
* ini file when you press OK on the ls desc screen. I used to do it    *
* when you pressed save changes on the main screen but since this is   *
* not part based it didn't make any sense. I also added a post to the  *
* pressure control window to tell him there is a new list of           *
* descriptions.                                                        *
*                                                                      *
***********************************************************************/
static void save_limit_switch_description_changes( HWND w )
{
bool         have_change;
int          i;
INI_CLASS    ini;
STRING_CLASS s;
WINDOW_CLASS wc;

have_change = false;
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    s.get_text( w, LS1_DESC_EBOX+i );
    if ( s.text() != SureTrakParam.limit_switch_desc(i) )
        {
        SureTrakParam.set_limit_switch_desc( i, s.text() );
        s.set_text( SuretrakProfileWindow, LS1_DESC_TBOX+i );
        have_change = true;;
        }
    }

if ( have_change )
    {
    s= ftii_editor_settings_name( CurrentPart.computer, CurrentPart.machine );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    ini.put_string( LS1DescriptionKey, SureTrakParam.limit_switch_desc(0) );
    ini.put_string( LS2DescriptionKey, SureTrakParam.limit_switch_desc(1) );
    ini.put_string( LS3DescriptionKey, SureTrakParam.limit_switch_desc(2) );
    ini.put_string( LS4DescriptionKey, SureTrakParam.limit_switch_desc(3) );
    ini.put_string( LS5DescriptionKey, SureTrakParam.limit_switch_desc(4) );
    ini.put_string( LS6DescriptionKey, SureTrakParam.limit_switch_desc(5) );
    }

wc = WindowInfo[PRESSURE_CONTROL_TAB].dialogwindow;
wc.post( WM_NEW_DATA );
}

/***********************************************************************
*                    INIT_LIMIT_SWITCH_DESCRIPTIONS                    *
***********************************************************************/
static void init_limit_switch_descriptions( HWND w )
{
int i;

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    set_text( w, LS1_DESC_EBOX+i, SureTrakParam.limit_switch_desc(i) );
    SendDlgItemMessage( w, LS1_DESC_EBOX+i, EM_LIMITTEXT, DESC_LEN-1, 0 );
    }
}

/***********************************************************************
*                   LIMIT_SWITCH_DESC_DIALOG_PROC                      *
*   This is a popup screen that allows the user to edit the limit      *
*   switch descriptions. It only works for FT2.                        *
***********************************************************************/
BOOL CALLBACK limit_switch_desc_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        init_limit_switch_descriptions( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save_limit_switch_description_changes( hWnd );
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
*                USER_CLICKED_LIMIT_SWITCH_DESCRIPTIONS                *
***********************************************************************/
BOOLEAN user_clicked_limit_switch_descriptions( int x, int y )
{
RECTANGLE_CLASS r;
RECTANGLE_CLASS r2;
POINT p;
WINDOW_CLASS w;

w = GetDlgItem( SuretrakProfileWindow, LS1_DESC_TBOX );
w.getrect( r );
w = GetDlgItem( SuretrakProfileWindow, LS6_DESC_TBOX );
w.getrect( r2 );

r.bottom = r2.bottom;

p.x = x;
p.y = y;
if ( r.contains(p) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                          SURETRAK_PROFILE_PROC                       *
***********************************************************************/
BOOL CALLBACK suretrak_profile_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static const COLORREF mycolor  = RGB( 255, 255,   0 );

static const int warmup_control[] = {
    WARMUP_WIRE_EBOX,
    WARMUP_RUN_IF_ON_XBOX,
    WARMUP_IF_STATIC,
    WARMUP_IS_STATIC
    };
static const int nof_warmup_controls = sizeof(warmup_control)/sizeof(int);

int  bid;
int  i;
int  id;
int  cmd;
HWND w;
HDC  dc;
bool need_color;
bool is_modified;
WINDOW_CLASS    wc;
RECTANGLE_CLASS dest;

static HBRUSH mybrush = 0;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SuretrakProfileWindow = hWnd;
        HaveStModTbox = GetDlgItem( hWnd, HAVE_ST_MOD_TBOX );
        HaveStModTbox.hide();
        LoadingSureTrakSteps = TRUE;
        for ( i= ACCEL_1_EDITBOX; i<=LS6_EDITBOX; i++ )
            set_text( hWnd, i, EmptyString );
        LoadingSureTrakSteps = FALSE;
        ShowWindow( GetDlgItem(hWnd, FS_POSITION_MSG_TBOX), SW_HIDE );
        if (  HaveMultipartRunlist )
            {
            /*
            ----------------------------------------------------------
            Move the multipart edit button to the right of the listbox
            ---------------------------------------------------------- */
            wc = GetDlgItem( hWnd, WARMUP_PART_CBOX );
            wc.get_move_rect( dest );
            wc = GetDlgItem( hWnd, EDIT_MULTIPART_RUNLIST_BUTTON );
            wc.move( dest.right+2, dest.top );

            /*
            ----------------------------------------------
            Hide the warmup controls I'm not going to need
            ---------------------------------------------- */
            for ( i=0; i<nof_warmup_controls; i++ )
                ShowWindow( GetDlgItem(hWnd, warmup_control[i]), SW_HIDE );

            /*
            ---------------------------------
            Change the title on the group box
            --------------------------------- */
            set_text( hWnd, WARMUP_GROUP_STATIC, resource_string(MULTIPART_RUNLIST_LABEL) );
            }
        else
            {
            wc = GetDlgItem( hWnd, EDIT_MULTIPART_RUNLIST_BUTTON );
            wc.hide();
            }
        break;

    case WM_CONTEXTMENU:
        id  = LOWORD( lParam );
        cmd = HIWORD( lParam );
        if ( user_clicked_limit_switch_descriptions(id, cmd) && CurrentMachine.is_ftii )
            {
            DialogBox(
                MainInstance,
                TEXT("LIMIT_SWITCH_DESC_DIALOG"),
                SuretrakProfileWindow,
                (DLGPROC) limit_switch_desc_dialog_proc );
            }
        else
            {
            suretrak_profile_context_help( (HWND) wParam, lParam );
            }
        return TRUE;

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
        w  = (HWND) lParam;
        dc = (HDC)  wParam;

        if ( HaveStModTbox == w )
            {
            SetTextColor( dc, RedColor );
            SetBkColor( dc, EboxBackgroundColor );
            return (int) EboxBackgroundBrush;
            }

        i = SureTrakParam.vacuum_limit_switch_number() + LS1_EDITBOX;

        is_modified = false;
        need_color  = false;
        if ( w == GetDlgItem(hWnd, FS_POSITION_MSG_TBOX) )
            {
            need_color = true;
            }
        else if ( w == GetDlgItem(hWnd, i) )
            {
            bid = VW_1_CHECKBOX;
            for ( i=0; i<MAX_ST_STEPS; i++ )
                {
                if ( is_checked(hWnd, bid) )
                    {
                    need_color = true;
                    break;
                    }
                bid += CONTROLS_PER_STEP;
                }
            }

        w  = (HWND) lParam;
        if ( msg == WM_CTLCOLOREDIT && StModList.contains(w) )
            {
            is_modified = true;
            SetTextColor( dc, RedColor );
            }

        if ( need_color )
            {
            if ( !mybrush )
                mybrush = CreateSolidBrush( mycolor );

            if ( mybrush )
                {
                SetBkColor( dc, mycolor );
                return (int) mybrush;
                }
            }
        else if ( is_modified )
            {
            SetBkColor( dc, EboxBackgroundColor );
            return (int) EboxBackgroundBrush;
            }
        break;

    case WM_DESTROY:
        if ( mybrush )
            {
            DeleteObject( mybrush );
            mybrush = 0;
            }
        break;

    case WM_COMMAND:
        if ( id == DOS_BACKUP_BUTTON )
            {
            DialogBox(
                MainInstance,
                TEXT("MSDOS_BACKUP_DIALOG"),
                MainDialogWindow,
                (DLGPROC) msdos_backup_dialog_proc );
            return TRUE;
            }

        if ( cmd == BN_CLICKED )
            {
            switch ( id )
                {
                case LS1_WIRE_PB:
                case LS2_WIRE_PB:
                case LS3_WIRE_PB:
                case LS4_WIRE_PB:
                case LS5_WIRE_PB:
                case LS6_WIRE_PB:
                    if ( choose_wire( GetDlgItem(hWnd, id), hWnd, LIMIT_SWITCH_OUTPUT_TYPE, id-LS1_WIRE_PB)  )
                        {
                        SureTrakHasChanged = TRUE;
                        if ( !HaveStModTbox.is_visible() )
                            HaveStModTbox.show();
                        }
                    return TRUE;

                case EDIT_MULTIPART_RUNLIST_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("MULTIPART_RUNLIST_DIALOG"),
                    MainDialogWindow,
                    (DLGPROC) multipart_runlist_dialog_proc );
                return TRUE;
                }

            if ( id == WARMUP_RUN_IF_ON_XBOX )
                {
                set_run_if_on_text();
                }
            else
                {
                check_vacuum_wait_buttons( hWnd, id );
                }
            }

        if ( cmd == CBN_SELCHANGE || cmd == EN_CHANGE || cmd == BN_CLICKED )
            {
            if ( !LoadingSureTrakSteps )
                {
                w = (HWND) lParam;
                add_to_stmodlist( w );

                if ( (id >= BREAK_GAIN_1_EBOX && id <= VEL_LOOP_GAIN_EBOX) || id == FT_STOP_POS_EBOX )
                    {
                    update_st_params( hWnd, id );
                    }
                else if ( id >= ACCEL_1_EDITBOX && id <= LS6_EDITBOX )
                    {
                    get_new_suretrak_steps();
                    SureTrakPlot.empty_curves();
                    SureTrakPlot.empty_axes();
                    add_x_axis_to_plot();
                    add_y_axis_to_plot();
                    add_profile_curve();
                    SureTrakPlotWindow.refresh();
                    return TRUE;
                    }
                }
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          BASIC_SETUP_PROC                            *
***********************************************************************/
BOOL CALLBACK basic_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_CONTEXTMENU:
        return show_context_help( (HWND) wParam, lParam );

    case WM_DBINIT:
        SetWindowContextHelpId( GetDlgItem(hWnd, INCHES_BUTTON), 100 );
        SetWindowContextHelpId( GetDlgItem(hWnd, MM_BUTTON),     100 );
        SetWindowContextHelpId( GetDlgItem(hWnd, CM_BUTTON),     100 );
        return TRUE;

    case WM_COMMAND:
        if ( cmd == BN_CLICKED )
            {
            switch ( id )
                {
                case INCHES_BUTTON:
                case MM_BUTTON:
                case CM_BUTTON:
                    change_distance_units();
                    return TRUE;

                case IPS_BUTTON:
                case FPM_BUTTON:
                case CMPS_BUTTON:
                case MPS_BUTTON:
                    change_velocity_units();
                    return TRUE;

                }
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      RETURN_FROM_PARAMETER_EDIT                      *
***********************************************************************/
void return_from_parameter_edit()
{
if ( CurrentDialogNumber != NO_TAB )
    SetFocus( WindowInfo[CurrentDialogNumber].dialogwindow );
}

/***********************************************************************
*                            N_SELECTED_LB                             *
***********************************************************************/
static short n_selected_lb( HWND w, int box )
{
LRESULT x;

x = SendDlgItemMessage( w, box, LB_GETSELCOUNT, 0, 0L );
if ( x == LB_ERR )
    x = 0;

return short( x );
}

/***********************************************************************
*                      GET_SELECTED_COPYTO_MACHINE                     *
***********************************************************************/
static MACHINE_ENTRY * get_selected_copyto_machine()
{
TCHAR machine[MACHINE_NAME_LEN+1];

if ( !get_cb_select( CopyToDialogWindow, COPYTO_MACHINE_CB_ID, machine) )
    return 0;

return find_machine_entry( machine );
}

/***********************************************************************
*                        COPYTO_SELECTED_PARTS                         *
***********************************************************************/
static void copyto_selected_parts( BITSETYPE copy_flags )
{
INT * ip;
LRESULT x;
short i;
short n;
TCHAR name[PART_NAME_LEN+1];
MACHINE_ENTRY * m;

n = n_selected_lb( CopyToDialogWindow, COPYTO_PART_LISTBOX_ID );
if ( n <= 0 )
    return;

m = get_selected_copyto_machine();
if ( !m )
    return;

ip = new INT[n];
x  = 0;
if ( ip )
    x = SendDlgItemMessage( CopyToDialogWindow, COPYTO_PART_LISTBOX_ID, LB_GETSELITEMS, n, (LPARAM) ip );

if ( x != LB_ERR && x > 0 )
    {
    n = short( x );
    for ( i=0; i<n; i++ )
        {
        x = SendDlgItemMessage( CopyToDialogWindow, COPYTO_PART_LISTBOX_ID, LB_GETTEXT, ip[i], (LPARAM) ((LPSTR) name) );
        if ( x != LB_ERR )
            copypart( m->computer, m->name, name, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, copy_flags );
        }

    SendDlgItemMessage( CopyToDialogWindow, COPYTO_PART_LISTBOX_ID, LB_GETTEXT, ip[0], (LPARAM) ((LPSTR) name) );
    }

if ( ip )
    delete[] ip;
}

/***********************************************************************
*                            COPYTO_MACHINE                            *
***********************************************************************/
void copyto_machine( MACHINE_ENTRY * m, BITSETYPE copy_flags, short copy_type )
{
PART_ENTRY    * p;
BOOLEAN         need_to_copy;

m->partlist.rewind();
while ( TRUE )
    {
    p = (PART_ENTRY *) m->partlist.next();
    if ( !p )
        break;

    need_to_copy = TRUE;
    if ( copy_type == COPYTO_SAME_PARTS_RADIO )
        if ( !strings_are_equal(CurrentPart.name, p->name, PART_NAME_LEN) )
            need_to_copy = FALSE;

    if ( need_to_copy )
        copypart( m->computer, m->name, p->name, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, copy_flags );

    /*
    -----------------------------------------------------------------------
    If I have a warmup part, make sure it exists on the destination machine
    ----------------------------------------------------------------------- */
    if ( copy_flags & COPY_PART_WARMUP_PART )
        {
        if ( lstrlen(Warmup.partname) > 0 )
            {
            if ( !part_exists( m->computer, m->name, Warmup.partname) )
                {
                copy_flags = COPY_PART_ALL & ~COPY_PART_WARMUP_PART;
                copypart( m->computer, m->name, Warmup.partname, CurrentPart.computer, CurrentPart.machine, Warmup.partname, copy_flags );
                }
            }
        }
    }
}

/***********************************************************************
*                          COPYTO_COPY_FLAGS                           *
***********************************************************************/
static BITSETYPE copyto_copy_flags()
{
BITSETYPE copy_flags;

copy_flags = 0;

if ( is_checked(CopyToDialogWindow, COPYTO_BASIC_CHECKBOX)  )
    copy_flags |= COPY_PART_BASIC;

if ( is_checked(CopyToDialogWindow, COPYTO_ADVANCED_CHECKBOX) )
    copy_flags |= COPY_PART_ADVANCED;

if ( is_checked(CopyToDialogWindow, COPYTO_PARAMETER_CHECKBOX) )
    copy_flags |= COPY_PART_PARAMETERS;

if ( is_checked(CopyToDialogWindow, COPYTO_MARKS_CHECKBOX) )
    copy_flags |= COPY_PART_MARKS;

if ( is_checked(CopyToDialogWindow, COPYTO_USER_POSITIONS_CHECKBOX) )
    copy_flags |= COPY_PART_USER_POSITIONS;

if ( is_checked(CopyToDialogWindow, COPYTO_CHANNELS_CHECKBOX) )
    copy_flags |= COPY_PART_CHANNELS;

if ( is_checked(CopyToDialogWindow, COPYTO_REFERENCE_CHECKBOX) )
    copy_flags |= COPY_PART_REFERENCE_TRACE;

if ( is_checked(CopyToDialogWindow, COPYTO_SURETRAK_CHECKBOX) )
    copy_flags |= COPY_PART_SURETRAK_SETUP;

if ( is_checked(CopyToDialogWindow, COPYTO_PARAM_NAME_CHECKBOX) )
    copy_flags |= COPY_PART_PARAM_NAMES;

if ( is_checked(CopyToDialogWindow, COPYTO_WARMUP_PART_CHECKBOX) )
    copy_flags |= COPY_PART_WARMUP_PART;

if ( is_checked(CopyToDialogWindow, COPYTO_PART_DISPLAY_CHECKBOX) )
    copy_flags |= COPY_PART_DISPLAY_SETUP;

if ( is_checked(CopyToDialogWindow, COPYTO_CHANNEL_AXIS_LABELS_XBOX) )
    copy_flags |= COPY_PART_CHAXIS_LABEL;

if ( is_checked(CopyToDialogWindow, COPYTO_PARAM_SORT_XBOX) )
    copy_flags |= COPY_PART_PARAM_SORT;

if ( is_checked(CopyToDialogWindow, COPYTO_PRES_CONTROL_XBOX) )
    copy_flags |= COPY_PART_PRES_CONTROL;

return copy_flags;
}

/***********************************************************************
*                            COPYTO_COPY_TYPE                          *
***********************************************************************/
static short copyto_copy_type()
{
short copy_type;

copy_type = 0;

if ( is_checked(CopyToDialogWindow, COPYTO_SEL_PARTS_RADIO) )
    copy_type = COPYTO_SEL_PARTS_RADIO;

else if ( is_checked(CopyToDialogWindow, COPYTO_ALL_PARTS_RADIO) )
    copy_type = COPYTO_ALL_PARTS_RADIO;

else if ( is_checked(CopyToDialogWindow, COPYTO_ALL_PARTS_MACH_RADIO) )
    copy_type = COPYTO_ALL_PARTS_MACH_RADIO;

else if ( is_checked(CopyToDialogWindow, COPYTO_SAME_PARTS_RADIO) )
    copy_type = COPYTO_SAME_PARTS_RADIO;

return copy_type;
}

/***********************************************************************
*                             COPYTO_COPY                              *
***********************************************************************/
static void copyto_copy()
{
MACHINE_ENTRY * m;
BITSETYPE       copy_flags;
short           copy_type;

if ( !CopyToDialogWindow )
    return;

copy_flags = copyto_copy_flags();
copy_type  = copyto_copy_type();

if ( copy_type == COPYTO_SEL_PARTS_RADIO )
    {
    copyto_selected_parts( copy_flags );
    }
else if ( copy_type == COPYTO_ALL_PARTS_RADIO )
    {
    m = get_selected_copyto_machine();
    if ( m )
        copyto_machine( m, copy_flags, copy_type );
    }
else
    {
    MachineList.rewind();
    while ( TRUE )
        {
        m = (MACHINE_ENTRY *) MachineList.next();
        if ( !m )
            break;
        copyto_machine( m, copy_flags, copy_type );
        }
    }
}

/***********************************************************************
*                          FILL_COPYTO_PARTS                           *
***********************************************************************/
static void fill_copyto_parts()
{
MACHINE_ENTRY * m;
TCHAR           mymachine[MACHINE_NAME_LEN+1];

if ( !CopyToDialogWindow )
    return;

if ( get_cb_select( CopyToDialogWindow, COPYTO_MACHINE_CB_ID, mymachine) )
    {
    m = find_machine_entry( mymachine );
    if ( m )
        fill_part_listbox( CopyToDialogWindow, COPYTO_PART_LISTBOX_ID, m );
    }
}

/***********************************************************************
*                        POSITION_COPY_DIALOG                          *
***********************************************************************/
static void position_copy_dialog( HWND hWnd )
{
RECT r;
int  h;
int  w;
int  x;
int  y;

GetWindowRect( hWnd, &r );

w = r.right - r.left;
h = r.bottom - r.top;

GetWindowRect( GetDlgItem(MainDialogWindow,PART_LISTBOX_ID), &r );

x = r.right;
y = r.top;

MoveWindow( hWnd, x, y, w, h, TRUE );
}

/***********************************************************************
*                         COPY_TO_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK copy_to_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        CopyToDialogWindow = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DESTROY:
        CopyToDialogWindow = 0;
        break;

    case WM_HELP:
        gethelp( HELP_CONTEXT, COPY_TO_EXISTING_HELP );
        return TRUE;

    case WM_DBINIT:
        position_copy_dialog( hWnd );
        fill_machine_cb( hWnd, COPYTO_MACHINE_CB_ID );
        SendDlgItemMessage( hWnd, COPYTO_MACHINE_CB_ID, CB_SETCURSEL, 0, 0L );
        fill_copyto_parts();
        CheckRadioButton( hWnd, COPYTO_SEL_PARTS_RADIO, COPYTO_SAME_PARTS_RADIO, COPYTO_SEL_PARTS_RADIO );
        if ( HaveMultipartRunlist )
            set_text(hWnd, COPYTO_WARMUP_PART_CHECKBOX, resource_string(MULTIPART_RUNLIST_LABEL) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case COPYTO_MACHINE_CB_ID:
                if ( cmd == CBN_SELCHANGE )
                    {
                    fill_copyto_parts();
                    return TRUE;
                    }
                break;

            case IDOK:
                hourglass_cursor();
                copyto_copy();
                restore_cursor();
                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                DestroyWindow( hWnd );
                break;
            }
    break;
    }


return FALSE;
}

/***********************************************************************
*                           FILL_MULTIPLE_LB                           *
***********************************************************************/
static void fill_multiple_lb()
{
MACHINE_ENTRY * m;
TCHAR s[MACHINE_NAME_LEN+HAS_PART_PREFIX_LEN+1];
INT tab_stops[2] = { 6, 25 };

if ( !CopyMultipleDialogWindow )
    return;

SendDlgItemMessage( CopyMultipleDialogWindow, CMULTIPLE_MACHINE_LISTBOX_ID, LB_RESETCONTENT, 0, 0L );
SendDlgItemMessage( CopyMultipleDialogWindow, CMULTIPLE_MACHINE_LISTBOX_ID, LB_SETTABSTOPS, (WPARAM) 2, (LPARAM) tab_stops );

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;

    if ( strings_are_equal(CurrentMachine.name, m->name, MACHINE_NAME_LEN) )
        continue;

    if ( find_part_entry(m, CurrentPart.name) )
        lstrcpy( s, TEXT("\tY\t") );
    else
        lstrcpy( s, TEXT("\t \t") );

    lstrcat( s, m->name );
    SendDlgItemMessage( CopyMultipleDialogWindow, CMULTIPLE_MACHINE_LISTBOX_ID, LB_ADDSTRING, 0, (LPARAM) (LPSTR) s );
    }
}

/***********************************************************************
*                            COPY_MULTIPLE                             *
***********************************************************************/
static void copy_multiple()
{
MACHINE_ENTRY * m;
TCHAR s[MACHINE_NAME_LEN+HAS_PART_PREFIX_LEN+1];
TCHAR * cp;
short   i;
short   n;
INT   * ip;
LRESULT x;
HWND    w;
INT     box;
BITSETYPE copy_flags;

w = CopyMultipleDialogWindow;
box = CMULTIPLE_MACHINE_LISTBOX_ID;

cp = s + HAS_PART_PREFIX_LEN;

ip = 0;
n  = n_selected_lb( w, box );
if ( n > 0 )
    {
    ip = new INT[n];
    x  = 0;
    if ( ip )
        x = SendDlgItemMessage( w, box, LB_GETSELITEMS, n, (LPARAM) ip );

    if ( x != LB_ERR && x > 0 )
        {
        n = short( x );
        for ( i=0; i<n; i++ )
            {
            x = SendDlgItemMessage( w, box, LB_GETTEXT, ip[i], (LPARAM) ((LPSTR) s) );
            if ( x != LB_ERR )
                {
                m = find_machine_entry( cp );
                if ( m )
                    {
                    copypart( m->computer, m->name, CurrentPart.name, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, COPY_PART_ALL );

                    if ( lstrlen(Warmup.partname) > 0 )
                        {
                        if ( !part_exists( m->computer, m->name, Warmup.partname) )
                            {
                            copy_flags = COPY_PART_ALL & ~COPY_PART_WARMUP_PART;
                            copypart( m->computer, m->name, Warmup.partname, CurrentPart.computer, CurrentPart.machine, Warmup.partname, copy_flags );
                            }
                        }


                    reload_partlist( m->name );
                    }
                }
            }
        }
    }

if ( ip )
    delete[] ip;
}

/***********************************************************************
*                     COPY_MULTIPLE_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK copy_multiple_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        CopyMultipleDialogWindow = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DESTROY:
        CopyMultipleDialogWindow = 0;
        break;

    case WM_DBINIT:
        position_copy_dialog( hWnd );
        fill_multiple_lb();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                copy_multiple();
                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                DestroyWindow( hWnd );
                break;
            }
    break;
    }


return FALSE;
}

/***********************************************************************
*                     POSITION_ADD_PART_DIALOG                         *
***********************************************************************/
static void position_add_part_dialog( HWND hWnd )
{
RECT r;
int  h;
int  w;
int  x;
int  y;

GetWindowRect( hWnd, &r );

w = r.right - r.left;
h = r.bottom - r.top;

GetWindowRect( GetDlgItem(MainDialogWindow,PART_LISTBOX_ID), &r );

x = r.right;
y = r.top;

MoveWindow( hWnd, x, y, w, h, TRUE );
}

/***********************************************************************
*                              SUB_PROC                                *
***********************************************************************/
LRESULT CALLBACK sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
TCHAR c;

if ( msg == WM_CHAR )
    {
    c = (TCHAR) wParam;
    if ( c != VK_BACK )
        {
        if ( !IsCharAlphaNumeric(c) )
            {
            MessageBeep( 0xFFFFFFFF );
            return 0;
            }
        }
    }

return CallWindowProc( OldEditProc, w, msg, wParam, lParam );
}

/***********************************************************************
*                         ADD_PART_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK add_part_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;
HWND  w;
TCHAR name[PART_NAME_LEN+1];

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        AddPartDialogWindow = hWnd;
        w = GetDlgItem( AddPartDialogWindow, ADD_PART_PART_EDITBOX );
        OldEditProc = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) sub_proc );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        position_add_part_dialog( hWnd );
        limit_text( hWnd, ADD_PART_PART_EDITBOX, PART_NAME_LEN );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                get_text( name, hWnd, ADD_PART_PART_EDITBOX, PART_NAME_LEN+1 );
                if ( !is_empty(name) )
                    {
                    copypart( CurrentPart.computer, CurrentPart.machine, name, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, COPY_PART_ALL );
                    reload_partlist( CurrentPart.machine );
                    fill_parts( name );
                    }

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_DESTROY:
        AddPartDialogWindow = 0;
        break;
    }


return FALSE;
}

/***********************************************************************
*                      SET_PARAMETER_LISTBOX_TABS                      *
***********************************************************************/
static void set_parameter_listbox_tabs( HWND w )
{
const int n = 9;
int tabs[n];
int i;

int fieldlen[n] = {
    88,
    31,
    30,
    13,
    30,
    15,
    30,
    13,
    30
    };

/*
-------------
Set tab stops
------------- */
tabs[0] = fieldlen[0];
for ( i=1; i<n; i++ )
    tabs[i] = tabs[i-1] + fieldlen[i];

SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_SETTABSTOPS, (WPARAM) n, (LPARAM) tabs );
}

/***********************************************************************
*                       NEW_PARAMETER_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK new_parameter_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
HWND w;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SetFocus( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CREATE_FT_ANALOG_BUTTON:
            case CREATE_FT_ONOFF_BUTTON:
            case CREATE_FT_TARGET_BUTTON:
            case CREATE_FT_DIGITAL_BUTTON:
            case CREATE_FT_PLC_BUTTON:
            case CREATE_FT_EXTENDED_ANALOG_BUTTON:
                add_one_parameter( FT_ANALOG_INPUT + id - CREATE_FT_ANALOG_BUTTON );
                EndDialog( hWnd, 0 );

                w = GetDlgItem( ParamDialogWindow, PARAMETER_DESC_EDITBOX );
                SetFocus( w );
                eb_select_all( w );
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
*                    UPDATE_GOOD_SHOT_WIRE_SETUP                       *
***********************************************************************/
static void update_good_shot_wire_setup()
{
HWND         w;
unsigned     wire;
STRING_CLASS s;

/*
--------------------
Good shot wire setup
-------------------- */
w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;
s.get_text( w, GOOD_SHOT_WIRE_PB );

wire = s.uint_value();
if ( wire > HIGH_FT2_OUTPUT_WIRE )
    wire = NO_WIRE;
CurrentPart.good_shot_wire = wire;
CurrentPart.good_shot_requires_no_warnings = is_checked( w, GOOD_SHOT_NO_WARNINGS_XBOX );
}

/***********************************************************************
*                            MOVE_LB_LINE                              *
* This changes the sort order of the current line, moving it up or     *
* down in the listbox.                                                 *
***********************************************************************/
static void move_lb_line( int key )
{
int x;

x = ParamLimitLb.current_index();
if ( x == NO_INDEX )
    return;

if ( key == VK_DOWN )
    {
    if ( x < (ParamLimitLb.count()-1) )
        {
        CurrentSort.move( x+1, x );
        refresh_current_parameter_limits();
        ParamLimitLb.setcursel(x+1);
        refresh_current_parameter_limits();
        }
    }
else if ( key == VK_UP )
    {
    if ( x > 0 )
        {
        CurrentSort.move( x-1, x );
        refresh_current_parameter_limits();
        ParamLimitLb.setcursel(x-1);
        refresh_current_parameter_limits();
        }
    }
}

/***********************************************************************
*                         PARAMETER_LIMITS_PROC                        *
***********************************************************************/
BOOL CALLBACK parameter_limits_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     cmd;
int     id;
LRESULT x;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ParamLimitLb.init( hWnd, PARAMETER_LIMIT_LISTBOX );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        set_parameter_listbox_tabs( hWnd );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, PARAMETER_LIMITS_OVERVIEW_HELP );
        return TRUE;

    case WM_VKEYTOITEM:     /* Message from PARAMETER_LIMIT_LISTBOX */
        if ( id == VK_DOWN || id == VK_UP )
            {
            if ( GetKeyState(VK_SHIFT) < 0 )
                {
                move_lb_line( id );
                return -2;
                }
            }
        return -1;

    case WM_COMMAND:
        switch ( id )
            {
            case PARAMETER_LIMIT_LISTBOX:
                if ( cmd == LBN_DBLCLK )
                    {
                    if ( !IsWindowVisible(ParamDialogWindow) && (CurrentPasswordLevel >= WindowInfo[PARAMETER_LIMITS_TAB].password_level) )
                        {
                        ShowWindow( ParamDialogWindow, SW_SHOW );
                        }
                    }

                if ( cmd == LBN_DBLCLK || cmd == LBN_SELCHANGE )
                    {
                    x = ParamLimitLb.current_index();
                    x = CurrentSort.parameter_number( x );
                    load_new_parameter( short(x) );
                    /*
                    -------------------------------------
                    Set the focus back to me just in case
                    ------------------------------------- */
                    SetFocus( ParamLimitLb.handle() );
                    }

                return TRUE;

            case GOOD_SHOT_WIRE_PB:
                if ( choose_wire(GetDlgItem(hWnd, id), hWnd, GOOD_SHOT_OUTPUT_TYPE, NO_INDEX) )
                    update_good_shot_wire_setup();
                return TRUE;

            case ADD_PARAMETER_BUTTON:
                if ( CurrentPasswordLevel < WindowInfo[PARAMETER_LIMITS_TAB].password_level )
                    {
                    resource_message_box( MainInstance, LOW_PASSWORD_STRING, CANT_DO_THAT_STRING, MB_OK );
                    return TRUE;
                    }
                else
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("NEW_PARAMETER_DIALOG"),
                        MainWindow,
                        (DLGPROC) new_parameter_dialog_proc
                        );
                    }
                return TRUE;

            case REMOVE_PARAMETER_BUTTON:
                if ( CurrentPasswordLevel < WindowInfo[PARAMETER_LIMITS_TAB].password_level )
                    {
                    resource_message_box( MainInstance, LOW_PASSWORD_STRING, CANT_DO_THAT_STRING, MB_OK );
                    }
                else
                    {
                    remove_current_parameter();
                    }
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          EDIT_ANALOG_SENSORS                         *
***********************************************************************/
static void edit_analog_sensors( void )
{
static TCHAR taskname[] = TEXT( "esensor.exe" );
TCHAR path[MAX_PATH+1];

if ( get_exe_directory(path) )
    {
    lstrcat( path, taskname );
    execute( path );
    }
}

/***********************************************************************
*                        LOAD_EXTENDED_CHANNELS                        *
* The extended channels are 9-16, which translate to indexes of        *
* Nof_Editable_Channels to MAX_FT2_CHANNELS-1 (8-15).                  *
* This uses the same listbox ids as the FASTRAK_CHANNELS screen so     *
* I have to use a separate index (c) for the channels.                 *
***********************************************************************/
static void load_extended_channels()
{
int i;
int c;
TCHAR * cp;
LISTBOX_CLASS lb;

if ( !ExtendedChannelsDialog )
    return;

for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    c = Nof_Editable_Channels + i;
    cp = analog_sensor_description( CurrentPart.analog_sensor[c] );
    if ( cp )
        {
        lb.init( ExtendedChannelsDialog, FT_CHANNEL_1_LISTBOX+i );
        lb.setcursel( cp );
        }
    }
}

/***********************************************************************
*                        SAVE_EXTENDED_CHANNELS                        *
***********************************************************************/
static void save_extended_channels()
{
int i;
int c;
TCHAR * cp;
LISTBOX_CLASS lb;

if ( !ExtendedChannelsDialog )
    return;

for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    c = Nof_Editable_Channels + i;

    lb.init( ExtendedChannelsDialog, FT_CHANNEL_1_LISTBOX+i );
    cp = lb.selected_text();
    CurrentPart.analog_sensor[c] = analog_sensor_number( cp );
    }
}

/***********************************************************************
*                       EXTENDED_CHANNELS_PROC                         *
***********************************************************************/
BOOL CALLBACK extended_channels_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ExtendedChannelsDialog = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

   case WM_DBINIT:
        fill_analog_list_boxes( hWnd, FT_CHANNEL_1_LISTBOX, MAX_EDITABLE_FTII_CHANNELS );
        load_extended_channels();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save_extended_channels();
            case IDCANCEL:
                ExtendedChannelsDialog = 0;
                EndDialog( hWnd, 0 );
                break;
            }

        return TRUE;

    }

return FALSE;
}

/***********************************************************************
*                         FASTRAK_CHANNELS_PROC                        *
***********************************************************************/
BOOL CALLBACK fastrak_channels_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

   case WM_DBINIT:
        fill_analog_list_boxes();
        return TRUE;


    case WM_COMMAND:
        switch ( id )
            {
            case EDIT_ANALOG_SENSORS_BUTTON:
                edit_analog_sensors();
                return TRUE;

            case EXTENDED_CHANNELS_PB:
               DialogBox(
                   MainInstance,
                   TEXT("EXTENDED_CHANNELS"),
                   hWnd,
                  (DLGPROC) extended_channels_proc );
                return TRUE;
            }
        break;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, CHANNELS_SETUP_OVERVIEW_HELP );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            USER_DEFINED_PROC                         *
************************************************************************/
BOOL CALLBACK user_defined_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int i;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, USER_DEF_POS_OVERVIEW_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case LS_WIRE_1_PB:
            case LS_WIRE_2_PB:
            case LS_WIRE_3_PB:
            case LS_WIRE_4_PB:
                i = id - LS_WIRE_1_PB;
                choose_wire( GetDlgItem(hWnd, id), hWnd, LIMIT_SWITCH_OUTPUT_TYPE, i );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         FILL_COLOR_LIST_BOX                          *
***********************************************************************/
static void fill_color_list_box()
{
COLORS_CLASS c;
short i;
short n;

n = c.count();
for ( i=0; i<n; i++ )
    {
    SendDlgItemMessage( WindowInfo[PROFILE_MARKS_TAB].dialogwindow, MARKS_COLOR_LISTBOX_ID, LB_ADDSTRING, 0, (LPARAM) ((LPSTR) c.name(i) ) );
    }
}

/***********************************************************************
*                            SET_MARK_LINE                             *
***********************************************************************/
static void set_mark_line( int id )
{
BITSETYPE b;

b = (VLINE_MARK | BLINE_MARK | NOLINE_MARK );
CurrentMarks.array[CurrentSelectedMark].symbol &= ~b;

b = 0;
switch ( id )
    {
    case MARKS_NO_LINE_BUTTON:
        b = NOLINE_MARK;
        break;

    case MARKS_VLINE_BUTTON:
        b = VLINE_MARK;
        break;

    case MARKS_BASE_LINE_BUTTON:
        b = BLINE_MARK;
        break;
    }

CurrentMarks.array[CurrentSelectedMark].symbol |= b;
}

/***********************************************************************
*                          ADD_MARK_TO_LISTBOX                         *
***********************************************************************/
static void add_mark_to_listbox( TCHAR type, short number, TCHAR * s )
{
SendDlgItemMessage( AddMarksDialogWindow, ADD_MARKS_LISTBOX_ID, LB_ADDSTRING, 0, (LPARAM)(LPSTR) s );
AddMarksList[NofAddMarks].parameter_type = type;
AddMarksList[NofAddMarks].parameter_number = number;
NofAddMarks++;
}

/***********************************************************************
*                        fill_add_marks_listbox                        *
***********************************************************************/
static BOOLEAN fill_add_marks_listbox()
{
short i;
PARAMETER_ENTRY * p;
p = CurrentParam.parameter;

SendDlgItemMessage( AddMarksDialogWindow, ADD_MARKS_LISTBOX_ID, LB_RESETCONTENT, 0, 0L );
NofAddMarks = 0;

for ( i=0; i<CurrentParam.count(); i++ )
    {
    if ( p->vartype & (POSITION_VAR | TIME_VAR) )
        {
        if ( CurrentMarks.index(PARAMETER_MARK, i+1) == NO_INDEX )
            add_mark_to_listbox( PARAMETER_MARK, i+1, p->name );
        }
    p++;
    }

for ( i=1; i<=USER_AVG_VEL_COUNT; i++ )
    {
    if ( CurrentMarks.index(START_POS_MARK, i) == NO_INDEX )
        add_mark_to_listbox( START_POS_MARK, i, mark_name(START_POS_MARK, i) );

    if ( CurrentMarks.index(END_POS_MARK, i) == NO_INDEX )
        add_mark_to_listbox( END_POS_MARK, i, mark_name(END_POS_MARK, i) );
    }

for ( i=1; i<=USER_VEL_COUNT; i++ )
    if ( CurrentMarks.index(VEL_POS_MARK, i) == NO_INDEX )
        add_mark_to_listbox( VEL_POS_MARK, i, mark_name(VEL_POS_MARK, i) );

return TRUE;
}

/***********************************************************************
*                            ADD_NEW_MARKS                             *
***********************************************************************/
static void add_new_marks()
{
short i;

if ( !AddMarksDialogWindow )
    return;

for ( i=0; i<NofAddMarks; i++ )
    {
    if ( SendDlgItemMessage( AddMarksDialogWindow, ADD_MARKS_LISTBOX_ID, LB_GETSEL, i, 0L) )
        CurrentMarks.add( AddMarksList[i].parameter_type, AddMarksList[i].parameter_number );
    }
}

/***********************************************************************
*                        ADD_MARKS_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK add_marks_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        AddMarksDialogWindow = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_add_marks_listbox();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                add_new_marks();
                show_marks();

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                break;
            }

        return TRUE;

    case WM_DESTROY:
        AddMarksDialogWindow = 0;
        break;
    }

return FALSE;
}

/***********************************************************************
*                           MARKS_CONTEXT_HELP                         *
***********************************************************************/
static void marks_context_help( HWND cw, LPARAM lParam )
{
const int32 NOF_CONTROLS = 1;
const int32 NOF_STATIC_CONTROLS = 4;

static HELP_ENTRY control_help[] = {
    { MARKS_ADD_BUTTON, ADD_MARK_HELP }
    };

static HELP_ENTRY static_help[] = {
    { MARK_SOURCE_DESCRIPTION_STATIC, MARK_SOURCE_DESCRIPTION_HELP },
    { MARK_COLOR_CHOICE_STATIC,       MARK_COLOR_CHOICE_HELP       },
    { MARK_LINE_TYPE_STATIC,          MARK_LINE_TYPE_HELP          },
    { MARK_LABEL_STATIC,              MARK_LABEL_HELP              },
    };

int32 i;
POINT p;
HWND  dw;
HWND  w;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

dw = WindowInfo[PROFILE_MARKS_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    w = GetDlgItem( dw, static_help[i].id );
    if ( GetWindowRect(w, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            gethelp( HELP_CONTEXTPOPUP, static_help[i].context );
            return;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(dw, control_help[i].id) == cw )
        {
        gethelp( HELP_CONTEXTPOPUP, control_help[i].context );
        return;
        }
    }

get_overview_help();
}

/***********************************************************************
*                           PROFILE_MARKS_PROC                         *
***********************************************************************/
BOOL CALLBACK profile_marks_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;
LRESULT x;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_CONTEXTMENU:
        marks_context_help( (HWND) wParam, lParam );
        return TRUE;

    case WM_HELP:
        gethelp( HELP_CONTEXT, MARKS_SETUP_OVERVIEW_HELP );
        return TRUE;

    case WM_DBINIT:
        fill_color_list_box();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case MARKS_LISTBOX_ID:
                if ( cmd == LBN_SELCHANGE )
                    {
                    x = SendDlgItemMessage( hWnd, MARKS_LISTBOX_ID, LB_GETCURSEL, 0, 0L);
                    if ( x != LB_ERR )
                        {
                        CurrentSelectedMark = short( x );
                        show_current_mark();
                        }
                    }
                return TRUE;

            case MARKS_COLOR_LISTBOX_ID:
                if ( cmd == LBN_SELCHANGE )
                    {
                    x = SendDlgItemMessage( hWnd, id, LB_GETCURSEL, 0, 0L);
                    if ( x != LB_ERR )
                        CurrentMarks.array[CurrentSelectedMark].color = short( x );
                    }
                return TRUE;

            case MARKS_NO_LINE_BUTTON:
            case MARKS_VLINE_BUTTON:
            case MARKS_BASE_LINE_BUTTON:
                set_mark_line( id );
                return TRUE;

            case MARKS_LABEL_ID:
                GetDlgItemText( hWnd, id, CurrentMarks.array[CurrentSelectedMark].label, MARK_LABEL_LEN+1 );
                return TRUE;

            case MARKS_ADD_BUTTON:
                if ( !AddMarksDialogWindow )
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("ADD_MARKS"),
                        MainDialogWindow,
                       (DLGPROC) add_marks_dialog_proc );
                    }
                return TRUE;

            case MARKS_REMOVE_BUTTON:
                x = SendDlgItemMessage( hWnd, MARKS_LISTBOX_ID, LB_GETCURSEL, 0, 0L);
                if ( x != LB_ERR )
                    {
                    CurrentMarks.remove( short(x) );
                    show_marks();
                    }
                return TRUE;
            }

        return TRUE;

    }

return FALSE;
}

/***********************************************************************
*                    SHOW_ADVANCED_SETUP_CONTEXT_HELP                  *
***********************************************************************/
static void show_advanced_setup_context_help( LPARAM lParam )
{
const int32 NOF_STATIC_CONTROLS = 9;

static HELP_ENTRY staticbox[] = {
    { PLUNGER_DIAMETER_STATIC,   PLUNGER_DIAMETER_HELP  },
    { P1_TO_P3_STATIC,           P1_TO_P3_HELP          },
    { P2_TO_P3_STATIC,           P2_TO_P3_HELP          },
    { MIN_CSFS_POS_STATIC,       MIN_CSFS_POS_HELP      },
    { MIN_CSFS_VEL_STATIC,       MIN_CSFS_VEL_HELP      },
    { VEL_FOR_RISE_STATIC,       VEL_FOR_RISE_HELP      },
    { TIME_FOR_INTENS_STATIC,    TIME_FOR_INTENS_HELP   },
    { PRES_FOR_RESPONSE_STATIC,  PRES_FOR_RESPONSE_HELP },
    { BISCUIT_TIME_DELAY_STATIC, ADV_PART_OVERVIEW_HELP }
    };

int32 i;
POINT p;
HWND  bw;
HWND  sw;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

bw = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    sw = GetDlgItem( bw, staticbox[i].id );
    if ( GetWindowRect(sw, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            if ( staticbox[i].context == ADV_PART_OVERVIEW_HELP )
                break;

            gethelp( HELP_CONTEXTPOPUP, staticbox[i].context );
            return;
            }
        }
    }

gethelp( HELP_CONTEXT, ADV_PART_OVERVIEW_HELP );
}

/***********************************************************************
*                            ADVANCED_SETUP_PROC                       *
***********************************************************************/
BOOL CALLBACK advanced_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch (msg)
    {
    case WM_INITDIALOG:
        if ( !ShowEofFS )
            {
            ShowWindow( GetDlgItem(hWnd, END_OF_FS_VELOCITY_EDITBOX),  SW_HIDE );
            ShowWindow( GetDlgItem(hWnd, END_OF_FS_VELOCITY_STATIC),  SW_HIDE );
            }
        return TRUE;

    case WM_HELP:
        gethelp( HELP_CONTEXT, ADV_PART_OVERVIEW_HELP );
        return TRUE;

    case WM_CONTEXTMENU:
        show_advanced_setup_context_help( lParam );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            CREATE_DIALOGS                            *
***********************************************************************/
static void create_dialogs()
{
int i;

WindowInfo[BASIC_SETUP_TAB].procptr      = basic_setup_proc;
WindowInfo[PARAMETER_LIMITS_TAB].procptr = parameter_limits_proc;
WindowInfo[USER_DEFINED_TAB].procptr     = user_defined_proc;
WindowInfo[PROFILE_MARKS_TAB].procptr    = profile_marks_proc;
WindowInfo[ADVANCED_SETUP_TAB].procptr   = advanced_setup_proc;
WindowInfo[FASTRAK_CHANNELS_TAB].procptr = fastrak_channels_proc;
WindowInfo[SURETRAK_PROFILE_TAB].procptr = suretrak_profile_proc;
WindowInfo[PRESSURE_CONTROL_TAB].procptr = pressure_control_profile_proc;
WindowInfo[SETUP_SHEET_TAB].procptr      = setup_sheet_proc;

for ( i=0; i<TAB_COUNT; i++ )
    {
    WindowInfo[i].password_level = LOWEST_PASSWORD_LEVEL;
    WindowInfo[i].dialogwindow   = CreateDialog(
        MainInstance,
        DialogName[i],
        MainDialogWindow,
        WindowInfo[i].procptr );
    }
}

/***********************************************************************
*                         REPLACE_DISTANCE_VALUE                       *
***********************************************************************/
static STRING_CLASS & replace_distance_value( TCHAR * sorc, float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
i = round( x, 1.0 );

return replace_value( sorc, i );
}

/***********************************************************************
*                       REPLACE_X1_DISTANCE_VALUE                      *
***********************************************************************/
static STRING_CLASS & replace_x1_distance_value( TCHAR * sorc, float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
x /= 4.0;
i = round( x, 1.0 );

return replace_value( sorc, i );
}

/***********************************************************************
*                         REPLACE_VELOCITY_VALUE                       *
***********************************************************************/
static STRING_CLASS & replace_velocity_value( TCHAR * sorc, float new_value, BOOLEAN need_bit_30 )
{
static TCHAR setbit30[] = TEXT( "|H40000000" );
const int SLEN = sizeof(setbit30)/sizeof(TCHAR);

TCHAR s[MAX_INTEGER_LEN + SLEN + 1];
TCHAR * cp;
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_velocity( x );
x /= 4.0;
i = round( x, 1.0 );

cp = copystring( s, int32toasc((int32) i) );

if ( need_bit_30 )
    copystring( cp, setbit30 );

return replace_value( sorc, s );
}

/***********************************************************************
*                         REPLACE_VELOCITY_VALUE                       *
***********************************************************************/
static STRING_CLASS & replace_velocity_value( TCHAR * sorc, float new_value )
{
return replace_velocity_value( sorc, new_value, (BOOLEAN) FALSE );
}

/***********************************************************************
*                         X4_FROM_DISTANCE_VALUE                       *
***********************************************************************/
static unsigned x4_from_distance_value( float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
i = (int) round( x, 1.0 );
return (unsigned) i;
}

/***********************************************************************
*                         X1_FROM_DISTANCE_VALUE                       *
***********************************************************************/
static unsigned x1_from_distance_value( float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
x /= 4.0;
i = (int) round( x, 1.0 );
return (unsigned) i;
}

/***********************************************************************
*                              X1_VELOCITY                             *
***********************************************************************/
static unsigned x1_velocity( double x )
{
int    i;

x = CurrentPart.x4_from_velocity( x );
x /= 4.0;
i = round( x, 1.0 );

return (unsigned) i;
}

/***********************************************************************
*                              X1_VELOCITY                             *
***********************************************************************/
static unsigned x1_velocity( float new_value )
{
return x1_velocity( (double) new_value );
}

/***********************************************************************
*                              FTII_PERCENT                            *
static unsigned ftii_percent( double d )
{
union { unsigned u; short i; };

u = 0;
d *= 100.0;
i = (short) round( d, 1.0 );
u |= Bit31;

return u;
}
***********************************************************************/

/***********************************************************************
*                              FTII_PERCENT                            *
***********************************************************************/
static unsigned ftii_percent( double d )
{
unsigned u;

d *= 100.0;
u = (unsigned) round( d, 1.0 );
u |= Bit31;

return u;
}

/***********************************************************************
*                              FTII_PERCENT                            *
***********************************************************************/
static unsigned ftii_percent( float new_value )
{
return ftii_percent( (double) new_value );
}

/***********************************************************************
*                    UPDATE_SURETRAK_STEP_VALUES                       *
***********************************************************************/
static void update_suretrak_step_value( FTII_VAR_ARRAY & va, int vi )
{
int      step_number;
int      vn;
bool     have_low_impact;
double   d;
unsigned u;

SureTrakSetup.rewind();

vn              = ST_ACCEL_1_VN;
have_low_impact = false;

step_number = 0;
while ( true )
    {
    if ( step_number >= MAX_FTII_CONTROL_STEPS )
        break;

    if ( SureTrakSetup.next() )
        {
        if ( !have_low_impact )
            {
            if ( not_float_zero(SureTrakSetup.low_impact_value()) )
                have_low_impact = true;
            }

        /*
        -----
        Accel
        ----- */
        d = (double) SureTrakSetup.accel_value();
        if ( SureTrakSetup.accel_is_percent() )
            u = ftii_percent( d );
        else
            u = x1_from_distance_value( d );

        va[vi].set( vn, u );
        vi++;
        vn++;

        /*
        --------
        Velocity
        -------- */
        d = (double) SureTrakSetup.vel_value();
        if ( SureTrakSetup.vel_is_percent() )
            u = ftii_percent( d );
        else
            u = x1_velocity( d );

        if ( SureTrakSetup.have_vacuum_wait() )
            u |= Bit30;

        va[vi].set( vn, u );
        vi++;
        vn++;

        /*
        ------------
        End Position
        ------------ */
        u = x4_from_distance_value( SureTrakSetup.end_pos_value() );

        va[vi].set( vn, u );
        vi++;
        vn++;

        /*
        -------------
        Input to test
        ------------- */
        u = 0;
        if ( SureTrakSetup.have_vacuum_wait() )
            u = (unsigned) FTII_VACUUM_WAIT_INPUT;
        else if ( SureTrakSetup.have_low_impact_from_input() )
            u = (unsigned) FTII_LOW_IMPACT_INPUT;

        va[vi].set( vn, u );
        vi++;
        vn++;

        if ( have_low_impact )
            {
            va[vi++].set( vn++, (unsigned) 0 );
            d = (double) SureTrakSetup.low_impact_value();
            u = ftii_percent( d );
            va[vi++].set( vn++, (unsigned) u );
            va[vi++].set( vn++, (unsigned) 0 );
            va[vi++].set( vn++, (unsigned) 0 );
            step_number++;
            }
        }
    else
        {
        va[vi++].set( vn++, (unsigned) 0 );
        va[vi++].set( vn++, (unsigned) 0x7FFF );
        va[vi++].set( vn++, (unsigned) 0 );
        va[vi++].set( vn++, (unsigned) 0 );
        }
    step_number++;
    }
}

/***********************************************************************
*                     PRESSURE_TO_VOLTS_16BIT                          *
*             Convert a pressure setpoint to volts * 1000              *
* The setpoint can be a 16 bit positive or negative number which       *
* is then zero extended to 32 bits.                                    *
***********************************************************************/
unsigned pressure_to_volts_16bit( STRING_CLASS & sorc, double max_volts )
{
double max_pressure;
double min_pressure;
double min_volts;
double v;
union { unsigned u; unsigned short us; };

u = 0;

if ( sorc.contains(PercentChar) )
    {
    max_pressure = 100.0;
    min_pressure = -100.0;
    }
else
    {
    max_pressure = (double) analog_sensor_high_value( (short) PressureControl.sensor_number );
    min_pressure = - max_pressure;
    if ( max_pressure <= min_pressure )
        max_pressure = min_pressure + 1.0;
    }

min_volts = -max_volts;

v = sorc.real_value();
if ( v < min_pressure )
    v = min_pressure;
else if ( v > max_pressure )
    v = max_pressure;
v -= min_pressure;
v *= (max_volts - min_volts);
v *= 1000.0;
v /= (max_pressure - min_pressure);
v += min_volts*1000.0;

us = (unsigned short) v;

return u;
}

/***********************************************************************
*                     PRESSURE_TO_VOLTS_16BIT                          *
*             Convert a pressure setpoint to volts * 1000              *
* The setpoint can be a 16 bit positive or negative number which       *
* is then zero extended to 32 bits.                                    *
***********************************************************************/
unsigned pressure_to_volts_16bit( STRING_CLASS & sorc )
{
double max_volts;

/*
-------------------------------------------------------------------------------------
The voltage output range of the sensor does not apply here, I need the actual voltage
range, which is set by the radio buttons under the sensor drop down list box.
------------------------------------------------------------------------------------- */
max_volts = 5.0;

if ( PressureControl.voltage_output_range == PC_2_VOLT_RANGE )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == PC_10_VOLT_RANGE )
    max_volts = 10.0;

return pressure_to_volts_16bit( sorc, max_volts );
}

/***********************************************************************
*                      PERCENT_TO_UNSIGNED_VOLTS                       *
* This converts a string percent to an unsigned volts*1000.            *
* All 32 bits are used, so for +/- 10 volts -.01% = -.001 volts        *
* = -1 = 0xFFFFFFFF.                                                   *
***********************************************************************/
static unsigned percent_to_unsigned_volts( STRING_CLASS & sorc, double max_volts )
{
double d;

d = sorc.real_value();
d *= 100.0;
if ( (10.0 - max_volts) > 1.0 )
    {
    d *= max_volts;
    d /= 10.0;
    }

return (unsigned) round( d, 1.0 );
}

/***********************************************************************
*                      PERCENT_TO_UNSIGNED_VOLTS                       *
*            This uses the selected voltage output range.              *
***********************************************************************/
static unsigned percent_to_unsigned_volts( STRING_CLASS & sorc )
{
double max_volts;

max_volts = 5.0;

if ( PressureControl.voltage_output_range == PC_2_VOLT_RANGE )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == PC_10_VOLT_RANGE )
    max_volts = 10.0;

return percent_to_unsigned_volts( sorc, max_volts );
}

/***********************************************************************
*                           PERCENT_TO_VOLTS                           *
* Converts percent to volts using the current voltage output range     *
***********************************************************************/
double percent_to_volts( STRING_CLASS & sorc )
{
const double max_percent   =  100.0;
const double percent_range =  200.0;
double max_volts;
double volt_range;
double v;

/*
-------------------------------------------------------------------------------------
The voltage output range of the sensor does not apply here, I need the actual voltage
range, which is set by the radio buttons under the sensor drop down list box.
------------------------------------------------------------------------------------- */
max_volts = 5.0;

if ( PressureControl.voltage_output_range == PC_2_VOLT_RANGE )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == PC_10_VOLT_RANGE )
    max_volts = 10.0;

volt_range = 2 * max_volts;

v = sorc.real_value();
v += max_percent;
v *= volt_range;
v /= percent_range;
v -= max_volts;

return v;
}

/***********************************************************************
*                     UPDATE_FTII_CONTROL_PROGRAM                      *
* The limit_switch_mask is an array of 6 masks for the limit switches  *
* if this is NOT a suretrak2.                                          *
***********************************************************************/
static void update_ftii_control_program()
{
static int TSL_INDEX           = 0;
static int MSL_INDEX           = 1;
static int ESV_INDEX           = 2;
static int LS1_INDEX           = 3;
static int LS2_INDEX           = 4;
static int LS3_INDEX           = 5;
static int LS4_INDEX           = 6;
static int LS5_INDEX           = 7;
static int LS6_INDEX           = 8;
static int VEL_LOOP_GAIN_INDEX = 9;
static int FT_STOP_POS_INDEX   = 10;
static int BREAK_VEL_1_INDEX   = 11;
static int BREAK_VEL_2_INDEX   = 12;
static int BREAK_VEL_3_INDEX   = 13;
static int BREAK_GAIN_1_INDEX  = 14;
static int BREAK_GAIN_2_INDEX  = 15;
static int BREAK_GAIN_3_INDEX  = 16;
static int TIME_INTERVAL_MS_INDEX = 17;
static int NOF_TIME_SAMPLES_INDEX = 18;
static int BISCUIT_DELAY_MS_INDEX = 19;
static int PC_CONFIG_WORD_INDEX   = 20;
static int PC_PRES_LOOP_GAIN_INDEX   = 21;
static int PC_RETRACT_VOLTS_INDEX    = 22;
static int PC_PARK_VOLTS_INDEX       = 23;
static int PC_RAMP_1_INDEX           = 24;
static int PC_PRES_1_INDEX           = 25;
static int PC_HOLDTIME_1_INDEX       = 26;
static int PC_RAMP_2_INDEX           = 27;
static int PC_PRES_2_INDEX           = 28;
static int PC_HOLDTIME_2_INDEX       = 29;
static int PC_RAMP_3_INDEX           = 30;
static int PC_PRES_3_INDEX           = 31;
static int PC_HOLDTIME_3_INDEX       = 32;
static int PC_INTEGRAL_GAIN_INDEX    = 33;
static int PC_DERIVATIVE_GAIN_INDEX  = 34;
static int PC_RETCTL_DAC_INDEX       = 35; /* Retctl must be last */
static int PC_RETCTL_ARM_PRES_INDEX  = 36;
static int PC_RETCTL_PRES_INDEX      = 37;

static int vars[] = { TSL_VN, MSL_VN, ESV_VN,
 LS1_VN, LS2_VN, LS3_VN, LS4_VN, LS5_VN, LS6_VN,
 VEL_LOOP_GAIN_VN,
 FT_STOP_POS_VN,
 BREAK_VEL_1_VN,  BREAK_VEL_2_VN,  BREAK_VEL_3_VN,
 BREAK_GAIN_1_VN, BREAK_GAIN_2_VN, BREAK_GAIN_3_VN,
 TIME_INTERVAL_MS_VN, NOF_TIME_SAMPLES_VN,
 BISCUIT_DELAY_MS_VN,
 PC_CONFIG_WORD_VN,
 PC_PRES_LOOP_GAIN_VN,
 PC_RETRACT_VOLTS_VN,
 PC_PARK_VOLTS_VN,
 PC_RAMP_1_VN,
 PC_PRES_1_VN,
 PC_HOLDTIME_1_VN,
 PC_RAMP_2_VN,
 PC_PRES_2_VN,
 PC_HOLDTIME_2_VN,
 PC_RAMP_3_VN,
 PC_PRES_3_VN,
 PC_HOLDTIME_3_VN,
 PC_INTEGRAL_GAIN_VN,
 PC_DERIVATIVE_GAIN_VN,
 PC_RETCTL_DAC_VN,
 PC_RETCTL_ARM_PRES_VN,
 PC_RETCTL_PRES_VN
 };

const int nof_vars = sizeof(vars)/sizeof(int);

static int normal_gain_vars[]  = { BREAK_GAIN_1_VN, BREAK_GAIN_2_VN,  BREAK_GAIN_3_VN    };
static int cyclone_gain_vars[] = { BREAK_GAIN_1_VN, INTEGRAL_GAIN_VN, DERIVATIVE_GAIN_VN };

static int maskvn[] = { LS1_MASK_VN, LS2_MASK_VN, LS3_MASK_VN, LS4_MASK_VN, LS5_MASK_VN, LS6_MASK_VN };
const  int nof_masks = sizeof(maskvn)/sizeof(int);
const unsigned USE_ONLY_POSITIVE_VOLTAGES_MASK = 0x80;

int            mi;
int            vi;
int            i;
int            n;
unsigned       u;
unsigned       v;
float          fx;
double         x;
FTII_VAR_ARRAY va;
STRING_CLASS   s;
BOOLEAN        setpoint_not_zero;

if ( HavePressureControl )
    {
    n = nof_vars;
    if ( !PressureControl.retctl_is_enabled )
        n -= 3;
    }
else
    {
    n = PC_CONFIG_WORD_INDEX + 1;
    }
i = n;
if ( CurrentMachine.suretrak_controlled )
    i += ST_LAST_STEP_VN + 1;
mi = i;
i += nof_masks;
va.upsize( i );
va[TSL_INDEX].set( TSL_VN, x4_from_distance_value(CurrentPart.total_stroke_length) );
va[MSL_INDEX].set( MSL_VN, x4_from_distance_value(CurrentPart.min_stroke_length) );
va[ESV_INDEX].set( ESV_VN, x1_velocity(CurrentPart.eos_velocity) );

/*
--------------
Limit Switches
-------------- */
vi = LS1_INDEX;
for ( i=0; i<nof_masks; i++ )
    {
    fx = 0.0;
    if ( CurrentMachine.suretrak_controlled  )
        fx = SureTrakSetup.limit_switch_pos_value(i);
    else if ( i < LIMIT_SWITCH_COUNT )
        fx = CurrentPart.limit_switch_position[i];
    setpoint_not_zero = not_float_zero( fx );

    u  = 0;
    if ( setpoint_not_zero )
        u = CurrentLimitSwitchWire.mask(i);
    va[mi].set( maskvn[i], u );
    mi++;

    u  = 0x7FFF;
    if ( setpoint_not_zero )
        u = x4_from_distance_value( fx );
    va[vi].set( vars[vi], u );
    vi++;
    }

/*
------------------
Velocity Loop Gain
------------------ */
x = (double) SureTrakSetup.velocity_loop_gain();
x = round( x, 1.0 );
u = (unsigned) x;
va[VEL_LOOP_GAIN_INDEX].set( VEL_LOOP_GAIN_VN, u );

/*
----------------------------
Follow Through Stop Position
---------------------------- */
fx = SureTrakSetup.ft_stop_pos();
u  = 0x7FFF;
if ( not_float_zero(fx) )
    u = x4_from_distance_value( fx );
va[FT_STOP_POS_INDEX].set( FT_STOP_POS_VN, u );

va[BREAK_VEL_1_INDEX].set( BREAK_VEL_1_VN, x1_velocity(SureTrakSetup.gain_break_velocity(0)) );

if ( CurrentMachine.is_cyclone )
    {
    va[BREAK_VEL_2_INDEX].set( BREAK_VEL_2_VN, 6000 );
    va[BREAK_VEL_3_INDEX].set( BREAK_VEL_3_VN, 6000 );
    }
else
    {
    va[BREAK_VEL_2_INDEX].set( BREAK_VEL_2_VN, x1_velocity(SureTrakSetup.gain_break_velocity(1)) );
    va[BREAK_VEL_3_INDEX].set( BREAK_VEL_3_VN, x1_velocity(SureTrakSetup.gain_break_velocity(2)) );
    }

/*
------------------------------------------
Break gain, ingegral gain, derivative gain
------------------------------------------ */
vi = BREAK_GAIN_1_INDEX;
for (i=0; i<3; i++ )
    {
    if ( CurrentMachine.is_cyclone )
        vars[vi] = cyclone_gain_vars[i];
    else
        vars[vi] = normal_gain_vars[i];

    x = (double) SureTrakSetup.gain_break_gain(i);
    if ( i>0 && (CurrentMachine.is_cyclone) )
        {
        if ( i==1 && is_zero(x) )
            u = 1;
        else
            u = (unsigned) round( x, 1.0 );
        }
    else
        {
        x *= 1000.0;
        u = (unsigned) round( x, 1.0 );
        }
    va[vi].set( vars[vi], u );
    vi++;
    }

va[TIME_INTERVAL_MS_INDEX].set( TIME_INTERVAL_MS_VN, (unsigned) CurrentPart.ms_per_time_sample );
va[NOF_TIME_SAMPLES_INDEX].set( NOF_TIME_SAMPLES_VN, (unsigned) CurrentPart.nof_time_samples   );

/*
-----------------------------------------------------------------------------------
See if this machine uses biscuit time delay or absolute time to measure the biscuit
if absolute then zero the time delay as I am going to calculate the biscuit size.
----------------------------------------------------------------------------------- */
if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT )
    u = 0;
else
    u = (unsigned) CurrentPart.biscuit_time_delay*1000;
va[BISCUIT_DELAY_MS_INDEX].set( BISCUIT_DELAY_MS_VN, u );

/*
----------------
Pressure Control
---------------- */
if ( HavePressureControl )
    {
    u = 0;
    if ( PressureControl.is_enabled )
        {
        u = 1;
        v = PressureControl.voltage_output_range;
        v <<= 4;
        u |= v;
        if ( PressureControl.use_only_positive_voltages )
            u |= USE_ONLY_POSITIVE_VOLTAGES_MASK;
        /*
        --------------------------------------------------------
        The ls that arms the pressure control goes in bits 1,2,3
        Range = [LS1 = 0 to LS6 = 5]
        -------------------------------------------------------- */
        v = PressureControl.arm_intensifier_ls_number;
        v--;
        if ( v >= MAX_FTII_LIMIT_SWITCHES )
            v = 0;
        else
            v <<= 1;
        u |= v;
        }

    va[PC_CONFIG_WORD_INDEX].set(    PC_CONFIG_WORD_VN,    u );
    va[PC_PRES_LOOP_GAIN_INDEX].set( PC_PRES_LOOP_GAIN_VN, PressureControl.pressure_loop_gain.uint_value() );
    va[PC_INTEGRAL_GAIN_INDEX].set( PC_INTEGRAL_GAIN_VN, PressureControl.integral_gain.uint_value() );
    va[PC_DERIVATIVE_GAIN_INDEX].set( PC_DERIVATIVE_GAIN_VN, PressureControl.derivative_gain.uint_value() );

    if ( PressureControl.retctl_is_enabled )
        {
        /*
        --------------------------------------------------
        Set the DAC channel to be used for retract control
        -------------------------------------------------- */
        va[PC_RETCTL_DAC_INDEX].set( PC_RETCTL_DAC_VN, 4 );

        /*
        ------------------------------------------------------------------------------
        The Retract control arm pressure is stored as a voltage * 1000 but the type is
        just a number so I have to convert to an unsigned value myself.
        ------------------------------------------------------------------------------ */
        va[PC_RETCTL_ARM_PRES_INDEX].set( PC_RETCTL_ARM_PRES_VN, 0 );
        va[PC_RETCTL_ARM_PRES_INDEX] = percent_to_unsigned_volts( PressureControl.retctl_arm_pres, 10.0 );

        /*
        -------------------------------------------------------------------------------------------------
        At the present time the only pressure control is for a proportional valve, which will always be %
        ------------------------------------------------------------------------------------------------- */
        va[PC_RETCTL_PRES_INDEX].set( PC_RETCTL_PRES_VN, 0 );
        s = PressureControl.retctl_pres;
        set_percent( s, TRUE );
        u = pressure_to_volts_16bit( s, 10.0 );
        u |= Bit31;
        va[PC_RETCTL_PRES_INDEX] = u;
        }

    va[PC_RETRACT_VOLTS_INDEX].set( PC_RETRACT_VOLTS_VN, 0 );
    if ( PressureControl.proportional_valve )
        {
        x = percent_to_volts( PressureControl.retract_volts );
        }
    else
        {
        x = PressureControl.retract_volts.real_value();
        if ( x < -10.0 || x > 10.0 )
            x /= 1000.0;
        }
    va[PC_RETRACT_VOLTS_INDEX] = x;

    va[PC_PARK_VOLTS_INDEX].set( PC_PARK_VOLTS_VN, 0 );
    if ( PressureControl.proportional_valve )
        {
        x = percent_to_volts( PressureControl.park_volts );
        }
    else
        {
        x = PressureControl.park_volts.real_value();
        if ( x < -10.0 || x > 10.0 )
            x /= 1000.0;
        }
    va[PC_PARK_VOLTS_INDEX] = x;

    vi = PC_RAMP_1_INDEX;
    for ( i=0; i<PressureControl.nof_steps; i++ )
        {
        if ( PressureControl.step[i].is_goose_step )
            {
            s = PressureControl.step[i].ramp;
            if ( PressureControl.step[i].is_goose_percent )
                set_percent( s, TRUE );
            u = pressure_to_volts_16bit( s );
            u |= Bit31;
            if ( PressureControl.step[i].is_goose_percent )
                u |= Bit30;
            }
        else
            {
            u = PressureControl.step[i].ramp.uint_value();
            }
        va[vi].set( vars[vi], u );
        vi++;

        s = PressureControl.step[i].pressure;
        if ( PressureControl.step[i].is_percent )
            set_percent( s, TRUE );
        u = pressure_to_volts_16bit( s );
        if ( PressureControl.step[i].is_open_loop )
            {
            u |= Bit31;
            if ( PressureControl.step[i].is_percent )
                u |= Bit30;
            }
        va[vi].set( vars[vi], u );
        vi++;

        u = PressureControl.step[i].holdtime.uint_value();
        va[vi].set( vars[vi], u );
        vi++;
        }

    while( i<MAX_PRESSURE_CONTROL_STEPS )
        {
        u = 0;
        va[vi].set( vars[vi], u );
        vi++;

        u = 0x7FFF;
        va[vi].set( vars[vi], u );
        vi++;

        u = 0;
        va[vi].set( vars[vi], u );
        vi++;
        i++;
        }
    }
else
    {
    u = 0;
    va[PC_CONFIG_WORD_INDEX].set( PC_CONFIG_WORD_VN, u );
    }

/*
----------------------------------------------------------------------------
I only allocated memory for the suretrak steps if this machine is controlled
---------------------------------------------------------------------------- */
if ( CurrentMachine.suretrak_controlled )
    update_suretrak_step_value( va, n );

va.put( ftii_part_settings_name(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) );
}

/***********************************************************************
*                       CHECK_BREAK_GAIN_VALUES                        *
***********************************************************************/
static bool check_break_gain_values()
{
bool  have_no_zero_gains;
int   i;
float x;
HWND  w;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;
have_no_zero_gains = true;

for ( i=0; i<NOF_BREAK_GAIN_SETTINGS; i++ )
    {
    x = SureTrakSetup.gain_break_gain(i);
    if ( is_float_zero(x) )
        {
        if ( i==2 && CurrentMachine.is_cyclone )
            {
            /*
            ----------------------------------
            Derivative Gain default (i=2) is 0
            ---------------------------------- */
            continue;
            }
        x = 1.0;
        SureTrakSetup.set_gain_break_gain( i, x );
        set_text( w, BREAK_GAIN_1_EBOX+i, ascii_float(x) );
        have_no_zero_gains = false;
        }
    }

return have_no_zero_gains;
}

/***********************************************************************
*                      CHECK_MSL_OF_SURETRAK_PART                      *
***********************************************************************/
static void check_msl_of_suretrak_part()
{
int i;
int fast_shot_step;
int n;
float estimated_max_velocity;
float fast_shot_velocity;
float v;
double x;
HWND   w;
STRING_CLASS s1;
STRING_CLASS s2;

n = SureTrakSetup.nof_steps();
estimated_max_velocity = SureTrakParam.max_velocity();

fast_shot_step = 1;
fast_shot_velocity = -100.0;

SureTrakSetup.rewind();
for ( i=0; i<n; i++ )
    {
    SureTrakSetup.next();
    if ( SureTrakSetup.vel_is_percent() )
        v = estimated_max_velocity * SureTrakSetup.vel_value() / 100.0;
    else
        v =  SureTrakSetup.vel_value();

    if ( v > fast_shot_velocity )
        {
        fast_shot_velocity = v;
        fast_shot_step     = i;
        }
    }

if ( fast_shot_step > 0 && fast_shot_velocity > 0.0 )
    {
    SureTrakSetup.rewind();
    v = 0;
    i = 0;
    while ( SureTrakSetup.next() )
        {
        if ( i == (fast_shot_step-1) )
            {
            v = SureTrakSetup.end_pos_value();
            if ( CurrentPart.min_stroke_length < v )
                {
                SureTrakSetup.next();
                if ( is_empty(SureTrakSetup.end_pos_string()) )
                    v += CurrentPart.total_stroke_length;
                else
                    v += SureTrakSetup.end_pos_value();
                x = v;
                x /= 2.0;
                x = round( x, 0.01 );
                s2 = resource_string( MSL_TOO_SMALL_STRING );
                s1 = resource_string( RECOMMENDED_MSL_STRING );
                s1 += ascii_double( x );
                if ( MessageBox(MainWindow, s1.text(), s2.text(), MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK )
                    {
                    CurrentPart.min_stroke_length = (float) x;
                    w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;
                    set_text( w, MIN_STROKE_LEN_EDITBOX, ascii_double(x) );
                    }
                }
            break;
            }
        i++;
        }
    }
}

/***********************************************************************
*                        SAVE_VELOCITY_CHANGES                         *
***********************************************************************/
void save_velocity_changes()
{
int id;
HWND parent;
HWND w;
STRING_CLASS s;
BOOLEAN have_change;

have_change = FALSE;

parent = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

/*
--------------------------------------------------------------------------------
Reload the setups. The fact that I exist means the operator is allowed to change
nothing but the velocities. Make sure if any other changes were made that
I ignore them.
-------------------------------------------------------------------------------- */
if ( SureTrakSetup.find(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
    {
    id = VEL_1_EDITBOX;

    SureTrakSetup.rewind();
    while ( SureTrakSetup.next() )
        {
        w = GetDlgItem( parent, id );
        if ( StModList.contains(w) )
            {
            s.get_text( w );
            SureTrakSetup.set_vel( s.text() );
            have_change = TRUE;
            }
        id += CONTROLS_PER_STEP;
        }

    if ( have_change )
        {
        if ( SureTrakSetup.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
            {
            MessageBox( 0, resource_string(SAVING_VELOCITY_CHANGES_STRING), CurrentPart.name, MB_OK );
            refresh_suretrak_setup();
            update_ftii_control_program();
            send_new_monitor_setup_event();
            }
        }
    }
}

/***********************************************************************
*                               SAVE_CHANGES                           *
***********************************************************************/
static void save_changes()
{
int       i;
int       j;
int       pb;
short     n;
short     new_units;
short     old_units;
BITSETYPE new_type;
BITSETYPE old_type;
int       b;
bool      need_parameter_refresh;
bool      need_suretrak_refresh;
float     y;
HWND      w;
LONG      sensor_index;
TCHAR     sensor_description[ASENSOR_DESC_LEN+1];
STRING_CLASS s;
CHAXIS_LABEL_CLASS label;

temp_message( resource_string(SAVING_STRING) );

w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

/*
--------------
Distance Units
-------------- */
for ( i=0; i<NofDistanceButtons; i++ )
    {
    if ( is_checked(w, DistanceButton[i]) )
        {
        CurrentPart.distance_units = DistanceUnits[i];
        break;
        }
    }

/*
--------------
Velocity Units
-------------- */
for ( i=0; i<NofVelocityButtons; i++ )
    {
    if ( is_checked(w, VelocityButton[i]) )
        {
        CurrentPart.velocity_units = VelocityUnits[i];
        break;
        }
    }

need_suretrak_refresh = false;
y = ebox_float( w, TOTAL_STROKE_LEN_EDITBOX );
if ( not_float_zero(CurrentPart.total_stroke_length - y) )
    need_suretrak_refresh = true;
CurrentPart.total_stroke_length = ebox_float( w, TOTAL_STROKE_LEN_EDITBOX );
CurrentPart.min_stroke_length   = ebox_float( w, MIN_STROKE_LEN_EDITBOX   );
CurrentPart.eos_velocity        = ebox_float( w, EOS_VEL_EDITBOX          );

/*
----------------------
User Defined Positions
---------------------- */
w = WindowInfo[USER_DEFINED_TAB].dialogwindow;

if ( !(CurrentMachine.is_ftii && CurrentMachine.suretrak_controlled) )
    {
    for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
        {
        y = ebox_float( w, LIMIT_SWITCH_POSITION_1_EDITBOX+i);
        CurrentPart.limit_switch_position[i] = y;
        }
    /*
    -------------------------------------------------------------------------------------------
    The suretraksetup saves ls 5 and 6 to 1 and 2 of the ft1 positions. If this not a suretrak2
    I need to copy my new settings to these suretrak limit switch positions.
    ------------------------------------------------------------------------------------------- */
    i = 0;
    for (j=LIMIT_SWITCH_COUNT; j<MAX_FTII_LIMIT_SWITCHES; j++ )
        {
        s.get_text( w, LIMIT_SWITCH_POSITION_1_EDITBOX+i ); 
        SureTrakSetup.set_limit_switch_pos( s.text(), j );
        i++;
        }
    }

for ( i=0; i<USER_VEL_COUNT; i++ )
    CurrentPart.user_velocity_position[i] = ebox_float( w, VELOCITY_POSITION_1_EDITBOX+i );

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    CurrentPart.user_avg_velocity_start[i] = ebox_float( w, AVG_VELOCITY_START_1_EDITBOX+i );
    CurrentPart.user_avg_velocity_end[i]   = ebox_float( w, AVG_VELOCITY_END_1_EDITBOX+i );
    }

if ( CurrentMachine.is_ftii  )
    {
    if ( CurrentMachine.suretrak_controlled )
        {
        if ( HideSureTrakLimitSwitchWires )
            {
            CurrentLimitSwitchWire[0] = 29;
            CurrentLimitSwitchWire[1] = 30;
            CurrentLimitSwitchWire[2] = 31;
            CurrentLimitSwitchWire[3] = 32;
            CurrentLimitSwitchWire[4] = 26;
            CurrentLimitSwitchWire[5] = 27;
            }
        else
            {
            /*
            ----------------------------------------------------
            All six wires are now on the suretrak profile screen
            ---------------------------------------------------- */
            pb = LS1_WIRE_PB;
            for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
                {
                if ( s.get_text(SuretrakProfileWindow, pb) )
                    CurrentLimitSwitchWire[i] = s.uint_value();
                pb++;
                }
            }
        }
    else
        {
        /*
        -------------------------------------------------------------
        Four limit switches are available on the user position screen
        whis is the current value of w;
        ------------------------------------------------------------- */
        pb = LS_WIRE_1_PB;
        for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
            {
            if ( s.get_text(w, pb) )
                CurrentLimitSwitchWire[i] = s.uint_value();
            pb++;
            }
        /*
        ---------------------------------
        The remaining values are not used
        --------------------------------- */
        for ( i=LIMIT_SWITCH_COUNT; i<MAX_FTII_LIMIT_SWITCHES; i++ )
            {
            CurrentLimitSwitchWire[i] = NO_WIRE;
            }
        }

    CurrentLimitSwitchWire.put(CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    }

/*
-------------------
Advanced Part Setup
------------------- */
w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

CurrentPart.plunger_diameter       = ebox_float( w, PLUNGER_DIAMETER_EDITBOX );
CurrentPart.sleeve_fill_distance   = ebox_float( w, SLEEVE_FILL_DISTANCE_EDITBOX );
CurrentPart.runner_fill_distance   = ebox_float( w, RUNNER_FILL_DISTANCE_EDITBOX );
CurrentPart.csfs_min_position      = ebox_float( w, CSFS_MIN_POSITION_EDITBOX );
CurrentPart.csfs_min_velocity      = ebox_float( w, CSFS_MIN_VELOCITY_EDITBOX );
CurrentPart.csfs_rise_velocity     = ebox_float( w, CSFS_RISE_VELOCITY_EDITBOX );
CurrentPart.time_for_intens_pres   = ebox_float( w, TIME_FOR_INTENS_PRES_EDITBOX );
CurrentPart.pres_for_response_time = ebox_float( w, PRES_FOR_RESPONSE_TIME_EDITBOX );

y = ebox_float( w, BISCUIT_TIME_DELAY_EDITBOX );
if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT )
    y *= 1000.0;
CurrentPart.biscuit_time_delay = (int32) y;

/*
---------------------
FasTrak Channel Setup
--------------------- */
w = WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow;

/*
----------------------------
Save the channel axis labels
---------------------------- */
label.init( Nof_Editable_Channels );
for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    if ( NeedToCheckChannels5and7 && (i==4) )
        label.set( i, resource_string(LVDT_STRING) );
    else if ( NeedToCheckChannels5and7 && (i==6) )
        label.set( i, resource_string(CMD_STRING) );
    else if ( s.get_text(w, CHAN_1_AXIS_LABEL_EBOX+i) )
        label.set( i, s.text() );
    else
        label.set( i, EmptyString );
    }

label.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
label.cleanup();

for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    OriginalAnalogSensor[i] = CurrentPart.analog_sensor[i];
    if ( NeedToCheckChannels5and7 && (i==4 || i==6) )
        {
        CurrentPart.analog_sensor[i] = PLUS_MINUS_10_VOLT_SENSOR;
        }
    else
        {
        sensor_index = SendDlgItemMessage( w, FT_CHANNEL_1_LISTBOX+i, CB_GETCURSEL, 0, 0L );
        if ( sensor_index != CB_ERR )
            {
            SendDlgItemMessage( w, FT_CHANNEL_1_LISTBOX+i, CB_GETLBTEXT, sensor_index, (LPARAM) (LPCSTR) sensor_description );
            CurrentPart.analog_sensor[i] = analog_sensor_number( sensor_description );
            }
        }
    }

CurrentPart.head_pressure_channel = NO_FT_CHANNEL;
CurrentPart.rod_pressure_channel  = NO_FT_CHANNEL;
for ( b=HEAD_PRES_CHAN_1_BUTTON; b<HEAD_PRES_NO_CHAN_BUTTON; b++ )
    {
    if ( is_checked(w, b) )
        {
        CurrentPart.head_pressure_channel = 1 + b - HEAD_PRES_CHAN_1_BUTTON;
        break;
        }
    }

for ( b=ROD_PRES_CHAN_1_BUTTON; b<ROD_PRES_NO_CHAN_BUTTON; b++ )
    {
    if ( is_checked(w, b) )
        {
        CurrentPart.rod_pressure_channel = 1 + b - ROD_PRES_CHAN_1_BUTTON;
        break;
        }
    }

need_parameter_refresh = false;

/*
--------------
Pressure units
-------------- */
i = 0;

if ( CurrentMachine.impact_pres_type[POST_IMPACT_INDEX] & ROD_PRESSURE_CURVE )
    {
    if ( CurrentPart.rod_pressure_channel != NO_FT_CHANNEL )
        i = CurrentPart.rod_pressure_channel - 1;
    }
else
    {
    if ( CurrentPart.head_pressure_channel != NO_FT_CHANNEL )
        i = CurrentPart.head_pressure_channel - 1;
    }

new_units = analog_sensor_units( CurrentPart.analog_sensor[i] );
if ( new_units != analog_sensor_units(OriginalAnalogSensor[i]) )
    need_parameter_refresh = true;

if ( new_units != NO_UNITS )
    {
    /*
    ------------------------------------------------------------------------------------------------------------------
    The internal parameter pressure type units must be changed if the units of the head and rod pressures have changed
    ------------------------------------------------------------------------------------------------------------------ */
    CurrentPart.pressure_units = new_units;
    n = CurrentParam.count();
    for ( i=0; i<n; i++ )
        {
        if ( CurrentParam.parameter[i].vartype & PRESSURE_VAR && CurrentParam.parameter[i].input.type == INTERNAL_PARAMETER )
            CurrentParam.parameter[i].units = CurrentPart.pressure_units;
        }
    }

n = CurrentParam.count();

for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    {
    if ( OriginalAnalogSensor[i] != CurrentPart.analog_sensor[i] )
        {
        old_units = analog_sensor_units( OriginalAnalogSensor[i] );
        new_units = analog_sensor_units( CurrentPart.analog_sensor[i] );
        old_type  = analog_sensor_vartype( OriginalAnalogSensor[i] );
        new_type  = analog_sensor_vartype( CurrentPart.analog_sensor[i] );
        if ( old_units != new_units || old_type != new_type )
            {
            for ( int pi=0; pi<n; pi++ )
                {
                if ( CurrentParam.parameter[pi].input.type == FT_ANALOG_INPUT || CurrentParam.parameter[pi].input.type == FT_TARGET_INPUT || CurrentParam.parameter[pi].input.type == FT_EXTENDED_ANALOG_INPUT )
                    {
                    if ( CurrentFtAnalog.array[pi].channel == (i+1) )
                        {
                        need_parameter_refresh = true;
                        CurrentParam.parameter[pi].units   = new_units;
                        CurrentParam.parameter[pi].vartype = new_type;
                        }
                    }
                }
            }
        }
    }

update_good_shot_wire_setup();

/*
---------------------------------------------------
Check to see if the min stroke length is too small.
--------------------------------------------------- */
if ( HaveSureTrakControl )
    check_msl_of_suretrak_part();


CurrentPart.refresh_time_sample();   // The monitor setup may have changed the time sample
CurrentPart.save();
CurrentParam.save();
CurrentMarks.save();
CurrentFtAnalog.save(CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
--------------------------------------------------------------------------
Update the original analog sensor list so I will know if it changes again.
-------------------------------------------------------------------------- */
for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    OriginalAnalogSensor[i] = CurrentPart.analog_sensor[i];

/*
--------------------
Save the setup sheet
-------------------- */
w = WindowInfo[SETUP_SHEET_TAB].dialogwindow;
if ( SendDlgItemMessage( w, SETUP_EBOX, EM_GETMODIFY, 0, 0) )
    {
    s = setup_sheet_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    save_edit_control_to_file( s.text(), w, SETUP_EBOX );
    }

/*
-----------------------------
Save the parameter sort order
----------------------------- */
CurrentSort.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
----------------------------------------------------------------------------------------
I want to check the break gains even if no changes have been made to the suretrak setups
---------------------------------------------------------------------------------------- */
if ( CurrentMachine.is_ftii && CurrentMachine.suretrak_controlled )
    {
    if ( !check_break_gain_values() )
        {
        SureTrakHasChanged = TRUE;
        /*
        ---------------------------------------------------------
        I don't need to nag if this is an upgrade from ft2 to st2
        --------------------------------------------------------- */
        if ( !NeedToCheckChannels5and7 )
            {
            DialogBox(
                MainInstance,
                TEXT("ZERO_BREAK_GAIN_CHANGED_DIALOG"),
                MainDialogWindow,
                (DLGPROC) zero_break_gain_changed_dialog_proc );
            }
        }
    }

/*
-----------------
Sure-Trak Changes
----------------- */
if ( SureTrakHasChanged )
    {
    if ( !CurrentMachine.suretrak_controlled )
        {
        resource_message_box( MainInstance, CHANGE_MACHINE_SETUP_STRING, NOT_ST_MACHINE_STRING, MB_OK );
        CurrentMachine.suretrak_controlled = TRUE;
        }

    if ( !check_low_impact_values() )
        {
        show_low_impact_error();
        need_suretrak_refresh = true;
        }

    SureTrakHasChanged = FALSE;
    }

/*
------------------------------------------------------------------------------------------
I used to save the suretrak only if someting was changed but now I do it every time (6.51)
Unfortunately I used CurrentMachine.suretrak_controlled, which does not have to be set for
SureTrak1 systems. I changed this to using HaveSureTrakControl so I save whenever the 
st screen is visible (v6.54)
------------------------------------------------------------------------------------------ */
if ( HaveSureTrakControl )
    SureTrakSetup.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

if ( HaveMultipartRunlist )
    save_multipart_runlist();
else
    save_warmup_shot();

/*
--------------------------------------------
Clean up the mod lists for the suretrak page
-------------------------------------------- */
if ( StModList.count() > 0 )
    {
    if ( CurrentDialogNumber == SURETRAK_PROFILE_TAB )
        InvalidateRect( WindowInfo[CurrentDialogNumber].dialogwindow, 0, TRUE );
    StModList.remove_all();
    }

if ( HaveStModTbox.is_visible() )
    HaveStModTbox.hide();

if ( HavePressureControl )
    {
    get_new_pressure_control_steps();
    PressureControl.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    }
if ( need_suretrak_refresh )
    refresh_suretrak_setup();

if ( need_parameter_refresh )
    refresh_parameter_limits();

update_ftii_control_program();

send_new_monitor_setup_event();
}

/***********************************************************************
*                       SHOW_SELECT_PART_MESSAGE                       *
***********************************************************************/
static void show_select_part_message()
{
TCHAR s[MAX_STRING_LEN+1];
TCHAR t[MAX_STRING_LEN+1];

LoadString( MainInstance, CANT_DO_THAT_STRING, t, sizeof(t) );
LoadString( MainInstance, MUST_SELECT_STRING, s, sizeof(s) );
MessageBox( NULL, s, t, MB_OK );
}

/***********************************************************************
*                        SHOW_LAST_PART_MESSAGE                        *
***********************************************************************/
static void show_last_part_message()
{
TCHAR s[MAX_STRING_LEN+1];
TCHAR t[MAX_STRING_LEN+1];

LoadString( MainInstance, CANT_DO_THAT_STRING, t, sizeof(t) );
LoadString( MainInstance, LAST_PART_STRING, s, sizeof(s) );
MessageBox( NULL, s, t, MB_OK );
}

/***********************************************************************
*                              KILL_PART                               *
***********************************************************************/
static void kill_part( TCHAR * computer, TCHAR * machine, TCHAR * part )
{
DB_TABLE t;

t.open( plookup_dbname(computer, machine), PLOOKUP_RECLEN, PWL );
t.put_alpha( PLOOKUP_PART_NAME_OFFSET, part, PART_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    t.rec_delete();
t.close();

t.open( parts_dbname(computer, machine), PARTS_RECLEN, WL );
t.put_alpha( PARTS_PART_NAME_OFFSET, part, PART_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    t.rec_delete();
t.close();

kill_directory( madir_name(computer, machine, part) );
}

/***********************************************************************
*                         DELETE_CURRENT_PART                          *
***********************************************************************/
static void delete_current_part()
{
MACHINE_ENTRY * m;
INT             answer_button;

if ( !HaveCurrentPart )
    {
    show_select_part_message();
    return;
    }

if ( lstrcmp(CurrentMachine.current_part, CurrentPart.name) == 0 )
    {
    resource_message_box( MainInstance, THIS_IS_CURRENT_PART_STRING, CANT_DO_THAT_STRING, MB_OK );
    return;
    }

m = find_machine_entry( CurrentMachine.name );
if ( !m )
    return;

if ( m->partlist.count() < 2 )
    {
    show_last_part_message();
    return;
    }

/*
--------------------------------------------
See if this part is a warmup part for anyone
-------------------------------------------- */
TEXT_LIST_CLASS t;
WARMUP_CLASS    wc;
STRING_CLASS    s;

if ( get_current_partlist(t) )
    {
    t.rewind();
    while ( t.next() )
        {
        if ( !strings_are_equal(t.text(), CurrentPart.name) )
            {
            if ( wc.get(CurrentPart.computer, CurrentPart.machine, t.text()) )
                {
                if ( strings_are_equal(wc.partname, CurrentPart.name) )
                    {
                    s = resource_string( THIS_IS_A_WARMUP_PART_STRING );
                    s += t.text();
                    break;
                    }
                }
            }
        }
    }


if ( !s.isempty() )
    {
    MessageBox( MainWindow, s.text(), resource_string(CANT_DO_THAT_STRING), MB_OK | MB_SYSTEMMODAL );
    return;
    }

answer_button = resource_message_box( MainInstance, CONTINUE_STRING, ABOUT_TO_DELETE_STRING, MB_OKCANCEL | MB_ICONWARNING );
if ( answer_button != IDOK )
    return;

kill_part( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

reload_partlist( CurrentMachine.name );
fill_parts(0);
}

/***********************************************************************
*                      NEW_MACHINE_SETUP_CALLBACK                      *
***********************************************************************/
void new_machine_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
SendMessage( MainWindow, WM_NEWSETUP, 0, 0L );
}

/***********************************************************************
*                          MONITOR_THIS_PART                           *
***********************************************************************/
static void monitor_this_part()
{
DB_TABLE t;
HWND     w;
WORKER_LIST_CLASS worker;
STRING_CLASS file;
STRING_CLASS format;
STRING_CLASS oldpart;
STRING_CLASS s;
FILE_CLASS   f;

BOOLEAN have_monitor_log = FALSE;

file = get_ini_string( VisiTrakIniFile, ConfigSection, MonitorLogFileKey );
if ( file != unknown_string() )
    {
    format = get_ini_string( VisiTrakIniFile, ConfigSection, MonitorLogFormatKey );
    if ( file != unknown_string() )
        {
        worker.load();
        have_monitor_log = TRUE;
        }
    }

/*
--------------------------------------------
Save the current part into the machset table
-------------------------------------------- */
t.open( machset_dbname(CurrentMachine.computer), MACHSET_RECLEN, PWL );
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, CurrentMachine.name, MACHINE_NAME_LEN );
if ( t.get_next_key_match(1, WITH_LOCK) )
    {
    if ( have_monitor_log )
        oldpart = CurrentMachine.current_part;
    lstrcpy( CurrentMachine.current_part, CurrentPart.name );
    t.put_alpha( MACHSET_CURRENT_PART_OFFSET, CurrentPart.name, PART_NAME_LEN );
    t.rec_update();
    t.unlock_record();
    }
t.close();

Warmup.create_runlist( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
------------------------------------------------------------------------
Send a monitor setup event so the board monitor and suretrak will update
------------------------------------------------------------------------ */
poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentMachine.name );

/*
---------------------------------
Hide the monitor this part button
--------------------------------- */
w = GetDlgItem( MainDialogWindow, MONITOR_THIS_PART_BUTTON );
set_text( w, resource_string(THIS_IS_CURRENT_PART_STRING) );
EnableWindow( w, FALSE );

if ( have_monitor_log )
    {
    if ( log_file_string(s, format.text(), worker.full_name(CurrentMachine.opnumber), CurrentMachine.name, oldpart.text(), CurrentPart.name) )
        {
        if ( f.open_for_append(file.text()) )
            {
            f.writeline( s.text() );
            f.close();
            }
        }
    }
}

/***********************************************************************
*                            DO_NEWSETUP                               *
***********************************************************************/
static void do_newsetup()
{
BOOLEAN         had_current_part;
LISTBOX_CLASS   lb;
MACHINE_ENTRY * e;

had_current_part = HaveCurrentPart;
read_machines_and_parts();
HaveCurrentPart = FALSE;

if ( had_current_part )
    {
    e = find_machine_entry( CurrentMachine.name );
    if ( e )
        {
        lb.init( MainDialogWindow, MACHINE_LISTBOX_ID );
        fill_machine_cb( MainDialogWindow, MACHINE_LISTBOX_ID );
        if ( lb.setcursel(CurrentMachine.name) )
            {
            fill_part_listbox( MainDialogWindow, PART_LISTBOX_ID, e );
            lb.init( MainDialogWindow, PART_LISTBOX_ID );
            if ( lb.setcursel(CurrentPart.name) )
                HaveCurrentPart = TRUE;
            }
        }
    }

if ( !HaveCurrentPart )
    {
    lb.init( MainDialogWindow, MACHINE_LISTBOX_ID );
    lb.setcursel( 0 );
    fill_parts(0);
    }
}

/***********************************************************************
*                           MAIN_CONTEXT_HELP                          *
***********************************************************************/
static void main_context_help( HWND cw )
{
const int32 NOF_CONTROLS = 6;

static FULL_HELP_ENTRY control_help[] = {
    { CREATE_PART_BUTTON,            HELP_CONTEXT,      CREATE_NEW_PART_HELP     },
    { DELETE_PART_BUTTON,            HELP_CONTEXT,      DELETE_THIS_PART_HELP    },
    { COPY_TO_EXISTING_PARTS_BUTTON, HELP_CONTEXT,      COPY_TO_EXISTING_HELP    },
    { COPY_MULTIPLE_BUTTON,          HELP_CONTEXT,      COPY_TO_OTHER_HELP       },
    { MAIN_PRINT_BUTTON,             HELP_CONTEXTPOPUP, PART_SETUP_PRINTING_HELP },
    { RELOAD_BUTTON,                 HELP_CONTEXTPOPUP, RELOAD_SAVED_SETUP_HELP  }
    };

int32 i;

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(MainDialogWindow, control_help[i].id) == cw )
        {
        gethelp( control_help[i].type, control_help[i].context );
        return;
        }
    }

get_overview_help();
}

/***********************************************************************
*                    POSITION_SAVE_ALL_PARTS_DIALOG                    *
*                                                                      *
* The default position for the save_all_parts_dialog is the upper left *
* corner of the client rect of the main dialog. I want to offset this  *
* so I am centered in the main dialog.                                 *
*                                                                      *
***********************************************************************/
static void position_save_all_parts_dialog( HWND w )
{
RECTANGLE_CLASS rm;
RECTANGLE_CLASS r;
WINDOW_CLASS    wc;
long dx;
long dy;

wc = MainDialogWindow;
wc.get_client_rect( rm );

wc = w;
wc.get_move_rect( r );

dx = (rm.width() - r.width() )/2;
dy = (rm.height() - r.height() )/2;

wc.offset( dx, dy );
}

/***********************************************************************
*                    SAVE_ALL_PARTS_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK save_all_parts_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static LISTBOX_CLASS mlb;
static int           nof_machines;
static LISTBOX_CLASS plb;
static int           nof_parts;
static int mi;
static int pi;
static STRING_CLASS  s;
static PART_NAME_ENTRY ori;

int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        position_save_all_parts_dialog( hWnd );
        ori.set( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        mlb.init( MainDialogWindow, MACHINE_LISTBOX_ID );
        nof_machines = mlb.count();
        plb.init( MainDialogWindow, PART_LISTBOX_ID );
        nof_parts = 0;
        mi  = -1;
        pi  = -1;
        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_SAVE_NEXT_PART:
        if ( pi < 0 )
            {
            mi++;
            if ( mi >= nof_machines )
                {
                mlb.setcursel( ori.machine );
                fill_parts( ori.part );
                NeedToCheckChannels5and7 = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
                }
            s = mlb.item_text( mi );
            if ( machine_is_local(s.text()) )
                {
                mlb.setcursel( mi );
                pi = 0;
                fill_parts(pi);
                nof_parts = plb.count();
                }
            }
        if ( pi >= 0 )
            {
            if ( pi >= nof_parts )
                {
                pi = -1;
                }
            else
                {
                s = plb.item_text(pi);
                plb.setcursel( pi );
                get_current_part();
                save_changes();
                pi++;
                }
            }
        PostMessage( hWnd, WM_SAVE_NEXT_PART, 0, 0 );
        return TRUE;


    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                PostMessage( hWnd, WM_SAVE_NEXT_PART, 0, 0 );
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
*                            MainDialogProc                            *
***********************************************************************/
BOOL CALLBACK MainDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int            cmd;
int            id;
LPNMHDR        nm;
HWND           w;
HWND           fw;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {

    case WM_INITDIALOG:
        SaveChangesTitle.get_text( hWnd, SAVE_CHANGES_BUTTON );
        shrinkwrap( MainWindow, hWnd );
        return TRUE;

    case WM_NEW_DATA:
        /*
        --------------------------------------------------------------------
        This is used by the calculator to signal that p1 and p2 have changed
        -------------------------------------------------------------------- */
        refresh_advanced_setup();
        if ( CurrentDialogNumber != ADVANCED_SETUP_TAB )
            {
            TabCtrl_SetCurSel( TabControl, ADVANCED_SETUP_TAB );
            show_tab_control();
            w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;
            if ( w )
                {
                w = GetDlgItem( w, SLEEVE_FILL_DISTANCE_EDITBOX );
                SetFocus( w );
                }
            }
        return TRUE;

    case WM_COMMAND:
        if ( cmd == CBN_SELCHANGE && id == MACHINE_LISTBOX_ID )
            {
            fw = GetFocus();
            fill_parts(0);
            SetFocus( fw );
            return TRUE;
            }

        if ( cmd == LBN_SELCHANGE && id == PART_LISTBOX_ID )
            {
            fw = GetFocus();
            get_current_part();
            SetFocus( fw );
            return TRUE;
            }

        switch ( id )
            {
            case SAVE_CHANGES_BUTTON:
                if ( HaveCurrentPart )
                    {
                    if ( NeedToCallVelocityChanges )
                        save_velocity_changes();
                    else
                        save_changes();
                    }
                else
                    resource_message_box( MainInstance, NO_PART_SELECTED_STRING, CANT_DO_THAT_STRING, MB_OK );
                return TRUE;

            case MONITOR_THIS_PART_BUTTON:
                monitor_this_part();
                return TRUE;

            case SAVE_ALL_PARTS_BUTTON:
                CreateDialog(
                    MainInstance,
                    TEXT("SAVE_ALL_PARTS_DIALOG"),
                    MainWindow,
                    (DLGPROC) save_all_parts_dialog_proc );
                return TRUE;

            case RELOAD_BUTTON:
                get_current_part();
                return TRUE;

            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;

            case COPY_MULTIPLE_BUTTON:
                if ( !HaveCurrentPart )
                    show_select_part_message();
                else
                    {
                    CreateDialog(
                        MainInstance,
                        TEXT("COPY_MULTIPLE"),
                        MainWindow,
                        (DLGPROC) copy_multiple_dialog_proc );
                    }
                return TRUE;

            case COPY_TO_EXISTING_PARTS_BUTTON:
                if ( !HaveCurrentPart )
                    show_select_part_message();
                else if ( !CopyToDialogWindow )
                    {
                    CreateDialog(
                        MainInstance,
                        TEXT("COPYTODIALOG"),
                        MainWindow,
                        (DLGPROC) copy_to_dialog_proc );
                    }
                return TRUE;

            case MAIN_PRINT_BUTTON:
                print_part_setup();
                return TRUE;

            case CALCULATOR_BUTTON:
                if ( !HaveCurrentPart )
                    {
                    show_select_part_message();
                    }
                else
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("CALCULATOR_DIALOG"),
                        MainWindow,
                       (DLGPROC) calculator_dialog_proc );
                    }
                return TRUE;

            case DELETE_PART_BUTTON:
                delete_current_part();
                return TRUE;

            case CREATE_PART_BUTTON:
                if ( !HaveCurrentPart )
                    show_select_part_message();
                else
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("ADD_PART"),
                        MainWindow,
                        (DLGPROC) add_part_dialog_proc );
                    }
                return TRUE;

            }
        break;

    case WM_CONTEXTMENU:
        main_context_help( (HWND) wParam );
        return TRUE;

    case WM_NOTIFY:
        nm = (LPNMHDR) lParam;
        if ( nm->code == TCN_SELCHANGE )
            {
            w = GetFocus();
            show_tab_control();
            SetFocus(w);
            return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                              PAINT_ME                                *
***********************************************************************/
static void paint_me()
{
PAINTSTRUCT  ps;
HWND         w;

w = SureTrakPlotWindow.handle();

if ( !GetUpdateRect(w, NULL, FALSE) )
    return;

BeginPaint( w, &ps );

if ( MyFont )
    SelectObject( ps.hdc, MyFont );

if ( HaveSureTrakPlot )
    SureTrakPlot.paint( ps.hdc );

EndPaint( w, &ps );
}

/***********************************************************************
*                         SURETRAK_PLOT_PROC                           *
***********************************************************************/
LRESULT CALLBACK suretrak_plot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
switch (msg)
    {
    case WM_CREATE:
        SureTrakPlotWindow = hWnd;
        break;

    case WM_PAINT:
        paint_me();
        return 0;

    case WM_DBINIT:
        BringWindowToTop( hWnd );
        BringWindowToTop( hWnd );
        return 0;

    case WM_ACTIVATEAPP:
        if ( BOOL(wParam) )
            {
            if ( CurrentDialogNumber == SURETRAK_PROFILE_TAB || CurrentDialogNumber == PRESSURE_CONTROL_TAB )
                SureTrakPlotWindow.post( WM_DBINIT );
            }
        return 0;

    case WM_SETFOCUS:
        SetFocus( WindowInfo[CurrentDialogNumber].dialogwindow );
        return TRUE;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
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

enable_buttons();
if ( HavePressureControl )
    show_pres_controls( WindowInfo[PRESSURE_CONTROL_TAB].dialogwindow );
}

/***********************************************************************
*                              PLOT_MIN_CALLBACK                       *
***********************************************************************/
void plot_min_callback( TCHAR * topic, short item, HDDEDATA edata )
{
if ( !IsIconic(MainWindow) )
    CloseWindow( MainWindow );
}

/***********************************************************************
*                             TOGGLE_FOCUS                             *
***********************************************************************/
void toggle_focus()
{
HWND w;

w = GetFocus();
w = GetParent( w );

if ( w == MainDialogWindow )
    {

    if ( CurrentDialogNumber != NO_TAB )
        SetFocus( WindowInfo[CurrentDialogNumber].dialogwindow );
    }
else
    {
    SetFocus( MainDialogWindow );
    }
}

/***********************************************************************
*                          CHECK_FOR_AUTOSAVE                          *
***********************************************************************/
static void check_for_autosave()
{
NAME_CLASS s;
s.get_auto_save_all_file_name();
if ( s.file_exists() )
    {
    NeedToCheckChannels5and7 = TRUE;
    CreateDialog(
        MainInstance,
        TEXT("AUTO_SAVE_ALL_PARTS_DIALOG"),
        MainWindow,
        (DLGPROC) save_all_parts_dialog_proc );

    s.delete_file();
    }
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_NEWSETUP:
        do_newsetup();
        return 0;

    case WM_CONTEXTMENU:
    case WM_HELP:
        get_overview_help();
        return 0;

    case WM_EV_SHUTDOWN:
        SendMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        register_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX, new_machine_setup_callback );
        register_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX,   password_level_callback    );
        register_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX,   plot_min_callback          );
        get_current_password_level();
        fill_machines();
        SetFocus( GetDlgItem(MainDialogWindow, MACHINE_LISTBOX_ID) );
        check_for_autosave();
        return 0;

    case WM_COMMAND:
        switch ( id )
            {
            case OPEN_CHOICE:
                if ( CurrentDialogNumber == PARAMETER_LIMITS_TAB )
                    ShowWindow( ParamDialogWindow, SW_SHOW );
                return 0;

            case F10_KEY:
                toggle_focus();
                return 0;
            }
        break;

    case WM_SETFOCUS:
        if ( MainDialogWindow )
            SetFocus( GetDlgItem(MainDialogWindow, MACHINE_LISTBOX_ID) );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    case WM_SIZE:
    case WM_MOVE:
        position_current_dialog();
        break;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                            CREATE_MYFONT                             *
***********************************************************************/
static void create_myfont()
{
LOGFONT f;

f.lfHeight          = 14;
f.lfWidth           = 0;
f.lfEscapement      = 0;
f.lfOrientation     = 0;
f.lfWeight          = FW_DONTCARE;
f.lfItalic          = FALSE;
f.lfUnderline       = FALSE;
f.lfStrikeOut       = FALSE;
f.lfOutPrecision    = 0;
f.lfClipPrecision   = 0;
f.lfQuality         = DEFAULT_QUALITY;
f.lfPitchAndFamily  = 0;

if ( PRIMARYLANGID(GetUserDefaultLangID())==LANG_JAPANESE )
    {
    f.lfCharSet = SHIFTJIS_CHARSET;
    lstrcpy(f.lfFaceName,TEXT("MS –¾’©"));
    }
else
    {
    f.lfCharSet = ANSI_CHARSET;
    lstrcpy( f.lfFaceName, TEXT("Arial") );
    }

MyFont = CreateFontIndirect( &f );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( int cmd_show )
{
TCHAR * cp;
WNDCLASS wc;
COMPUTER_CLASS c;
RECT r;
int  width;
int  height;
int  i;
int  oldtab;
int  start_tab;

names_startup ();
c.startup();
read_machines_and_parts();
if ( machine_count(c.whoami()) < 1 )
    IsOfficeWorkstation = TRUE;
EboxBackgroundColor = (COLORREF) GetSysColor( COLOR_WINDOW) ;
EboxBackgroundBrush = CreateSolidBrush( EboxBackgroundColor );

/*
---------------------------------------------
See if this computer uses a multipart runlist
--------------------------------------------- */
HaveMultipartRunlist = boolean_from_ini( VisiTrakIniFile, ConfigSection, HaveMultipartRunlistKey );

/*
------------------------------------------------------------------------
Load a string to be used by the multipart runlist load and save routines
------------------------------------------------------------------------ */
NotUsedString = resource_string( NOT_USED_STRING );

HaveSureTrakControl = boolean_from_ini( VisiTrakIniFile, ConfigSection, HaveSureTrakKey );
HavePressureControl = boolean_from_ini( VisiTrakIniFile, ConfigSection, HavePressureControlKey );

cp = get_ini_string(EditPartIniFile, ConfigSection, VelocityChangesLevelKey );
if ( !unknown(cp) )
    VelocityChangesLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, SaveChangesLevelKey );
if ( !unknown(cp) )
    SaveChangesLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditOtherComputerKey );
if ( !unknown(cp) )
    EditOtherComputerLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditOtherCurrentPartKey );
if ( !unknown(cp) )
    EditOtherCurrentPartLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditOtherSuretrakKey );
if ( !unknown(cp) )
    EditOtherSuretrakLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditOtherCurrentSuretrakKey );
if ( !unknown(cp) )
    EditOtherCurrentSuretrakLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditPressureSetupLevelKey );
if ( !unknown(cp) )
    EditPressureSetupLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditPressureProfileLevelKey );
if ( !unknown(cp) )
    EditPressureProfileLevel = (short) asctoint32( cp );

HideSureTrakLimitSwitchWires = boolean_from_ini( EditPartIniFile, ConfigSection, HideSureTrakLimitSwitchWiresKey );

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MAKEINTRESOURCE(EDITPART_ICON) );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

wc.lpszClassName = SureTrakPlotWindowClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) suretrak_plot_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

register_choose_wire_control();

cp = resource_string( MAIN_WINDOW_TITLE_STRING );

MainWindow = CreateWindow(
    MyClassName,
    cp,
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    0, 0,                             // X,y
    640, 290,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("MAIN_DIALOG"),
    MainWindow,
    (DLGPROC) MainDialogProc );

create_myfont();
units_startup();
create_dialogs();
create_tab_control();
analog_sensor_startup();

ParamDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("PARAMETER_DIALOG"),
    MainWindow,
    (DLGPROC) ParamDialogProc );

/*
--------------------------------------------------------------------------
Make the suretrak plot window the same height as the parameter edit window
-------------------------------------------------------------------------- */
GetWindowRect( ParamDialogWindow, &r );
height = r.bottom - r.top;

/*
-------------------------------------
And the same width as the main window
------------------------------------- */
GetWindowRect( MainWindow, &r );
width = r.right - r.left;

cp = resource_string( SURETRAK_PLOT_WINDOW_TITLE_STRING );

CreateWindowEx(
    WS_EX_TOOLWINDOW,
    SureTrakPlotWindowClassName,
    cp,
    WS_POPUP | WS_DLGFRAME,
    0, 0,                             // X,y
    width, height,
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );

ShowWindow( ParamDialogWindow,  SW_HIDE );

start_tab = BASIC_SETUP_TAB;
if ( HaveSureTrakControl )
    {
    oldtab = asctoint32( get_ini_string(EditPartIniFile, ConfigSection, CurrentTabKey) );
    if ( oldtab == SURETRAK_PROFILE_TAB || oldtab == PRESSURE_CONTROL_TAB )
        start_tab = oldtab;
    }

i = asctoint32( get_ini_string(EditPartIniFile, ConfigSection, ParameterTabEditLevelKey) );
if ( i >= 0 )
    WindowInfo[PARAMETER_LIMITS_TAB].password_level = i;

/*
------------------------------------------------------------------------------------
At this point the current tab is NOTAB, I want to set it to either Basic or SureTrak
------------------------------------------------------------------------------------ */
TabCtrl_SetCurSel( TabControl, start_tab );
show_tab_control();
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
if ( MyFont )
    {
    DeleteObject( MyFont );
    MyFont = 0;
    }

if ( EboxBackgroundBrush )
    {
    DeleteObject( EboxBackgroundBrush );
    EboxBackgroundBrush = 0;
    }

cleanup_machines_and_parts();
unregister_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX   );
unregister_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX   );
unregister_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX );

put_ini_string( EditPartIniFile, ConfigSection, CurrentTabKey, int32toasc(CurrentDialogNumber) );
put_ini_string( EditPartIniFile, ConfigSection, LastMachineKey, CurrentPart.machine );
put_ini_string( EditPartIniFile, ConfigSection, LastPartKey,    CurrentPart.name );
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{

MSG  msg;
BOOL status;
TCHAR * cp;

MainInstance = this_instance;
cp = resource_string( MAIN_WINDOW_TITLE_STRING );
if ( is_previous_instance(MyClassName, cp) )
    return 0;

InitCommonControls();

LoadingSureTrakSteps = TRUE;
init( cmd_show );
LoadingSureTrakSteps = FALSE;

AccelHandle = LoadAccelerators( this_instance, AccelName );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status )   //  && MainDialogWindow )
        status = TranslateAccelerator( MainWindow, AccelHandle, &msg );

    if ( !status && MainDialogWindow )
        status = IsDialogMessage( MainDialogWindow, &msg );

    if ( !status && AddPartDialogWindow )
        status = IsDialogMessage( AddPartDialogWindow, &msg );

    if ( !status && CopyToDialogWindow )
        status = IsDialogMessage( CopyToDialogWindow, &msg );

    if ( !status && CopyMultipleDialogWindow )
        status = IsDialogMessage( CopyMultipleDialogWindow, &msg );

    if ( !status && AddMarksDialogWindow )
        status = IsDialogMessage( AddMarksDialogWindow, &msg );

    if ( !status && ParamDialogWindow )
        status = IsDialogMessage( ParamDialogWindow, &msg );

    if ( !status )
        {
        if ( CurrentDialogNumber != NO_TAB )
            {
            status = IsDialogMessage( WindowInfo[CurrentDialogNumber].dialogwindow, &msg );
            }
        }

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        }
    }

shutdown();
client_dde_shutdown();
analog_sensor_shutdown();
units_shutdown();
return(msg.wParam);
}
