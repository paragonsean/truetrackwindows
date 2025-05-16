#ifndef _PART_CLASS_
#include "..\include\part.h"
#endif

#ifndef _DB_CLASS_
#include "..\include\dbclass.h"
#endif

#ifndef _VISI_PARAM_H_
#include "..\include\param.h"
#endif

#ifndef _VISI_MARKS_H_
#include "..\include\marks.h"
#endif

#ifndef _PROFILE_H_
#include "..\include\structs.h"
#endif

#ifndef _MACHINE_NAME_LIST_CLASS_
#include "..\include\machname.h"
#endif

#define MAX_SHOTS_TO_SHOW    10
#define MAX_PLOT_LABEL_LEN   18

/*
------------
Screen Types
------------ */
#define STANDARD_SCREEN_TYPE  1
#define ZABISH_SCREEN_TYPE    2
#define BOX_SCREEN_TYPE       3
#define TREND_SCREEN_TYPE     4
#define BIG_PARAM_SCREEN_TYPE 5
#define PROFILE_SCREEN_TYPE   6         /* Last used standard screen */
#define DOWNBAR_SCREEN_TYPE   7
#define STATUS_SCREEN_TYPE    8

/*
-------
Regions
------- */
#define BOTTOM_AXIS            0
#define LEFT_AXIS              1
#define RIGHT_AXIS             2
#define POS_BASED_ANALOG       3
#define POS_BASED_COUNT        4
#define TIME_BASED_ANALOG      5
#define TIME_BASED_COUNT       6
#define NOF_REGIONS            7

#define PRESSURE_DISPLAY_MASK (ROD_PRESSURE_CURVE | HEAD_PRESSURE_CURVE | DIFF_PRESSURE_CURVE)
#define CHANNEL_DISPLAY_MASK  (CHAN_1_ANALOG_CURVE | CHAN_2_ANALOG_CURVE | CHAN_3_ANALOG_CURVE | CHAN_4_ANALOG_CURVE)

struct REGION_ENTRY
    {
    RECT rect;
    BOOLEAN is_visible;
    };

struct PLOT_CURVE_ENTRY
    {
    short curve_index;
    TCHAR label[MAX_PLOT_LABEL_LEN+1];
    };

/*
----
Axes
---- */
#define NO_AXIS              -1
#define TIME_AXIS             0
#define Y_POS_AXIS            1
#define X_POS_AXIS            2
#define VELOCITY_AXIS         3
#define FIRST_ANALOG_AXIS     4
#define HEAD_PRES_AXIS        4
#define ROD_PRES_AXIS         5
#define DIFFERENTIAL_AXIS     6
#define LAST_ANALOG_AXIS      7

#define NOF_AXES              8

#define BACKGROUND_COLOR      8
#define BORDER_COLOR          9
#define GRID_COLOR           10
#define TEXT_COLOR           11
#define TREND_COLOR          12

#define NOF_COLORS           13

struct AXIS_ENTRY
    {
    float max;
    float min;
    BOOLEAN is_on_hold;
    BOOLEAN is_fixed;
    BOOLEAN is_clear;

    AXIS_ENTRY() { is_fixed = FALSE; is_on_hold = FALSE; is_clear = TRUE; min = 0.0; max = 0.0; };
    };

#ifndef _PROFILE_H_
#include "..\include\structs.h"
#endif

#define DATESTRING_DATE_OFFSET 5
#define DATESTRING_TIME_OFFSET 17
#define DATESTRING_LEN ( 5+ALPHADATE_LEN+2+ALPHATIME_LEN )

#define NOF_PARAMETER_LIST_TITLES 5

const unsigned int X4_COUNTS_PER_POS_MARK = DEF_COUNTS_PER_POS_MARK;

#ifdef _MAIN_
const TCHAR UnknownString[] = UNKNOWN;
REGION_ENTRY Region[NOF_REGIONS];
AXIS_ENTRY Axis[NOF_AXES];

/*
-----------------------------
Smoothing Setup (dconfig.cpp)
----------------------------- */
BOOLEAN Smoothing = FALSE;
BOOLEAN SmoothingChannel[MAX_FT_CHANNELS] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };
BOOLEAN SmoothingVelocity = FALSE;

/*
-------------------------------------------------------------------
The following varaibles are used by the dbar and dbarconfig screens
------------------------------------------------------------------- */
TCHAR DowntimeComputerName[COMPUTER_NAME_LEN+1];
TCHAR DowntimeMachineName[MACHINE_NAME_LEN+1];

TCHAR ComputerName[COMPUTER_NAME_LEN+1];
TCHAR MachineName[MACHINE_NAME_LEN+1];
TCHAR PartName[PART_NAME_LEN+1];
TCHAR ShotName[SHOT_LEN+1];
TCHAR NoPart[] = NO_PART;
float        PostImpactDisplayTime = 0.0;   /* Seconds */
MARKS_CLASS     CurrentMarks;
PARAMETER_CLASS CurrentParam;
PART_CLASS      CurrentPart;
PARAM_INDEX_CLASS CurrentSort;
MACHINE_CLASS   CurrentMachine;
FTANALOG_CLASS  CurrentFtAnalog;         /* FTII only */
                                         /*0123456789_123456789_1234*/
TCHAR DateString[DATESTRING_LEN+1] = TEXT("FRI  02/24/1995  13:54:32");

COLORREF AlarmColor      = RGB(255,   0,   0 );
COLORREF NoAlarmColor    = RGB(  0, 200, 100 );
COLORREF WarningColor    = RGB(255, 255,   0 );
COLORREF DialogTextColor = RGB(  0,   0,   0 );

COLORREF NormalColor[NOF_COLORS] =
    {
    RGB( 255, 255, 255 ),     /* Time       */
    RGB( 255, 255,   0 ),     /* Y Position */
    RGB(   0,   0, 255 ),     /* X Position */
    RGB( 255, 255, 255 ),     /* Velocity   */
    RGB( 255, 255, 250 ),     /* Analog 1   */
    RGB(   0, 255, 150 ),     /* Analog 2   */
    RGB(   0,   0, 255 ),     /* Analog 3   */
    RGB(   0, 255,   0 ),     /* Analog 4   */
    RGB(   0,   0,   0 ),     /* Background */
    RGB(  50,  50, 255 ),     /* Border     */
    RGB( 150, 150, 150 ),     /* Grid       */
    RGB( 200, 200, 200 ),     /* Text       */
    RGB( 200, 200, 200 )      /* Trend Background */
    };

COLORREF ReferenceColor[NOF_COLORS] =
    {
    RGB( 198,   0,   0 ),     /* Time       */
    RGB( 198,   0,   0 ),     /* Y Position */
    RGB( 198,   0,   0 ),     /* X Position */
    RGB( 198,   0,   0 ),     /* Velocity   */
    RGB( 198,  50,   0 ),     /* Analog 1   */
    RGB( 198,   0,   0 ),     /* Analog 2   */
    RGB( 198,   0,   0 ),     /* Analog 3   */
    RGB( 198,   0,   0 ),     /* Analog 4   */
    RGB(   0,   0,   0 ),     /* Background */
    RGB(  50,  50, 255 ),     /* Border     */
    RGB( 150, 150, 150 ),     /* Grid       */
    RGB( 200, 200, 200 ),     /* Text       */
    RGB( 200, 200, 200 )      /* Trend Background */
    };

COLORREF PrNormalColor[NOF_COLORS] =
    {
    RGB( 255, 255, 255 ),     /* Time       */
    RGB(  50, 200, 100 ),     /* Y Position */
    RGB(   0,   0, 255 ),     /* X Position */
    RGB(   0, 255,   0 ),     /* Velocity   */
    RGB( 255, 255, 250 ),     /* Analog 1   */
    RGB(   0, 255, 150 ),     /* Analog 2   */
    RGB(   0,   0, 255 ),     /* Analog 3   */
    RGB(   0, 255,   0 ),     /* Analog 4   */
    RGB( 255, 255, 255 ),     /* Background */
    RGB(  50,  50, 255 ),     /* Border     */
    RGB( 150, 150, 150 ),     /* Grid       */
    RGB( 200, 200, 200 ),     /* Text       */
    RGB( 200, 200, 200 )      /* Trend Background */
    };

COLORREF PrReferenceColor[NOF_COLORS] =
    {
    RGB( 198,   0,   0 ),     /* Time       */
    RGB( 198,   0,   0 ),     /* Y Position */
    RGB( 198,   0,   0 ),     /* X Position */
    RGB( 198,   0,   0 ),     /* Velocity   */
    RGB( 198,  50,   0 ),     /* Analog 1   */
    RGB( 198,   0,   0 ),     /* Analog 2   */
    RGB( 198,   0,   0 ),     /* Analog 3   */
    RGB( 198,   0,   0 ),     /* Analog 4   */
    RGB( 255, 255, 255 ),     /* Background */
    RGB(  50,  50, 255 ),     /* Border     */
    RGB( 150, 150, 150 ),     /* Grid       */
    RGB( 200, 200, 200 ),     /* Text       */
    RGB( 200, 200, 200 )      /* Trend Background */
    };

COLORREF * CurrentColor = NormalColor;

BOOLEAN  RealTime;
BOOLEAN  SingleMachineOnly     = FALSE;
BOOLEAN  HaveHeader;
BOOLEAN  OverlayMenuState;
BOOLEAN  ViewReferenceMenuState;
BOOLEAN  IsPressureDisplay     = TRUE;
BOOLEAN  ViewParametersDisplay = FALSE;
BOOLEAN  IsFullScreen          = FALSE;
BOOLEAN  DialogIsActive        = FALSE;
BOOLEAN  PrintingMultiplePlots = FALSE;
BOOLEAN  PartBasedDisplay      = FALSE;
BOOLEAN  SnapshotIsActive      = TRUE;

short    NofParms;
short    NofPoints;
short    TimeCurve;
short    PosCurve;
float    MaxPostPosition;
float    MinPostPosition;
float    Parms[MAX_PARMS];

PROFILE_HEADER Ph;
float          Points[MAX_CURVES][MAX_FT2_POINTS];

/*
--------------------------------
Width of the pen used for curves
-------------------------------- */
int CurvePenWidth = 1;

/*
-------------------------------------
Downtime categories and subcategories
------------------------------------- */
DOWNCAT_NAME_CLASS    Cats;
DOWNSCAT_NAME_CLASS   SubCats;

/*
-------
Windows
------- */
HINSTANCE MainInstance;
HWND   MainWindow     = 0;
HWND   ToolTipWindow  = 0;
HWND   MultipleShotDialogWindow = 0;
HWND   ExportAlarmsDialogWindow = 0;
HWND   ChooseDowntimeMachineWindow = 0;

WINDOW_CLASS XdcrWindow;
bool         HaveNewXdcrPart = true;

HFONT MyFont = NULL;
int   MyCharHeight;
int   MyCharWidth;

int   FirstDownCatButton = 0;
int   LastDownCatButton  = 0;


/*
--------------------------------------------------------------------------------------
I need to be able to tell the status screen to recalculate the column widths after the
header control is changed.
-------------------------------------------------------------------------------------- */
BOOLEAN NeedNewStatusScreenColumnWidths = FALSE;

/*
-------------------------------------------------------------------
Timer Id used by status screen to update the downtimes every minute
------------------------------------------------------------------- */
UINT StatusScreenTimerId = 0;

/*
----------------------------------------------------
If there is a timeout defined for the password level
I use the following timer to wake me up.
---------------------------------------------------- */
UINT PasswordLevelTimeoutId = 0;
UINT PasswordLevelTimeoutMs = 0;

FILETIME DateICanShowPressure;

short            HistoryShotCount = 50;

short            NofPlotCurves;
PLOT_CURVE_ENTRY PlotCurve[MAX_CONTINUOUS_CHANNELS];
BITSETYPE        PreImpactPlotCurveTypes;
BITSETYPE        PostImpactPlotCurveTypes;

short            NofOverlayShots;
short            CurrentPasswordLevel = 5;
short            EditStatusScreenLevel = 10;
PROFILE_NAME_ENTRY OverlayShot[MAX_OVERLAY_SHOTS];

MACHINE_NAME_LIST_CLASS MachList;

REALTIME_MACHINE_LIST_CLASS RealtimeMachineList;

/*
--------------
Ini file names
-------------- */
TCHAR DisplayIniFile[]              = TEXT( "display.ini" );
TCHAR EditPartIniFile[]             = TEXT( "editpart.ini" );
TCHAR MonallIniFile[]               = TEXT( "monall.ini" );
TCHAR MonallProgramName[]           = TEXT( "monall.exe" );
TCHAR SureTrakIniFile[]             = TEXT( "suretrak.ini" );
TCHAR VisiTrakIniFile[]             = TEXT( "visitrak.ini" );

TCHAR ButtonLevelsSection[]         = TEXT( "ButtonLevels" );
TCHAR ConfigSection[]               = TEXT( "Config" );
TCHAR MultipartSetupSection[]       = TEXT( "Multipart" );

TCHAR ClearDownWireFromPopupKey[]   = TEXT( "ClearDownWireFromPopup" );
TCHAR ClearMonitorWireOnDownKey[]   = TEXT( "ClearMonitorWireOnDown" );
TCHAR CsvShotFileNameKey[]          = TEXT( "CsvShotFileName" );
TCHAR CsvShotFileTypeKey[]          = TEXT( "CsvShotFileType" );
TCHAR CsvUseShotNumberKey[]         = TEXT( "CsvUseShotNumber" );
TCHAR CurrentFtValuesGrayLevelKey[] = TEXT( "CurrentFtValuesGrayLevel" );
TCHAR CurvePenWidthKey[]            = TEXT( "CurvePenWidth" );
TCHAR DisplaySecondsKey[]           = TEXT( "DisplaySeconds" );
TCHAR DownTimeAlarmWireKey[]        = TEXT( "DownTimeAlarmWire" );
TCHAR DowntimePurgeButtonKey[]      = TEXT( "DowntimePurgeButton" );
TCHAR EditControlButtonKey[]        = TEXT( "EditControlButton" );
TCHAR EditOperatorsButtonKey[]      = TEXT( "EditOperatorsButton" );
TCHAR EditOtherComputerKey[]        = TEXT( "EditOtherComputerLevel" );
TCHAR EditOtherCurrentPartKey[]     = TEXT( "EditOtherCurrentPartLevel" );
TCHAR EditOtherCurrentSuretrakKey[] = TEXT( "EditOtherCurrentSuretrakLevel" );
TCHAR EditOtherSuretrakKey[]        = TEXT( "EditOtherSuretrakLevel" );
TCHAR EditPressureSetupLevelKey[]   = TEXT( "EditPressureSetupLevel" );
TCHAR EditPressureProfileLevelKey[] = TEXT( "EditPressureProfileLevel" );
TCHAR EditStatusSceenLevelKey[]     = TEXT( "EditStatusScreenLevel" );
TCHAR ExitAllDefaultKey[]           = TEXT( "ExitAllDefault" );
TCHAR HaveTouchScreenKey[]          = TEXT( "HaveTouchScreen" );
TCHAR HaveMultipartRunlistKey[]     = TEXT( "HaveMultipartRunlist" );
TCHAR HaveSureTrakKey[]             = TEXT( "HaveSureTrak" );
TCHAR HideSureTrakLimitSwitchWiresKey[] = TEXT("HideSureTrakLimitSwitchWires");
TCHAR HighLimitTypeKey[]            = TEXT( "HighLimitType" );
TCHAR Ioex4GrayLevelKey[]           = TEXT( "Ioex4GrayLevel" );
TCHAR Ioex4MenuGrayLevelKey[]       = TEXT( "Ioex4MenuGrayLevel" );
TCHAR LastMachineKey[]              = TEXT( "LastMachine" );
TCHAR LastPartKey[]                 = TEXT( "LastPart" );
TCHAR LastShotKey[]                 = TEXT( "LastShot" );
TCHAR LockOnAlarmKey[]              = TEXT( "LockOnAlarm" );
TCHAR LowLimitTypeKey[]             = TEXT( "LowLimitType" );
TCHAR MachineBasedPlotDisplayKey[]  = TEXT( "MachineBasedPlotDisplay" );
TCHAR MainFontNameKey[]             = TEXT( "MainFontName" );
TCHAR MonitorWireKey[]              = TEXT( "MonitorWire" );
TCHAR MultipartBaseWireKey[]        = TEXT( "Wire" );
TCHAR NoNetworkKey[]                = TEXT( "NoNetwork" );

TCHAR ParameterTabEditLevelKey[]    = TEXT( "ParameterTabEditLevel" );
TCHAR ParameterWindowFontKey[]      = TEXT( "ParameterWindowFont" );
TCHAR PasswordLevelTimeoutKey[]     = TEXT( "PasswordLevelTimeout" );
TCHAR PasswordsAreEncriptedKey[]    = TEXT( "HideUnusedParams" );
TCHAR PartBasedDisplayKey[]         = TEXT( "PartBasedDisplay" );
TCHAR PrintParametersKey[]          = TEXT( "PrintParameters" );
TCHAR SaveAllPartsLevelKey[]        = TEXT( "SaveAllPartsLevel" );
TCHAR SaveAllAlarmShotsKey[]        = TEXT( "SaveAllAlarmShots" );
TCHAR SaveChangesLevelKey[]         = TEXT( "SaveChangesLevel" );
TCHAR SerialNumberKey[]             = TEXT( "SerialNumber" );
TCHAR ServiceToolsGrayLevelKey[]    = TEXT( "ServiceToolsGrayLevel" );
TCHAR SetupSheetLevelKey[]          = TEXT( "SetupSheetLevel" );
TCHAR ShowSetupSheetKey[]           = TEXT( "ShowSetupSheet" );
TCHAR SmoothFactorKey[]             = TEXT( "SmoothFactor" );
TCHAR SmoothNKey[]                  = TEXT( "SmoothN" );
TCHAR SmoothCurvesKey[]             = TEXT( "SmoothCurves" );
TCHAR SnapshotKey[]                 = TEXT( "SnapshotIsActive" );
TCHAR StExitLevelKey[]              = TEXT( "ExitEnableLevel" );
TCHAR SubtractTimeoutKey[]          = TEXT( "SubtractTimeout" );
TCHAR UseDowntimeEntryKey[]         = TEXT( "UseDowntimeEntry" );
TCHAR UseValveTestOutputKey[]       = TEXT( "UseValveTestOutput" );
TCHAR UseNullValveOutputKey[]       = TEXT( "UseNullValveOutput" );
TCHAR VelocityChangesLevelKey[]     = TEXT( "VelocityChangesLevel" );
TCHAR VersionKey[]                  = TEXT( "Version" );
#else

extern HWND    MainPlotWindow;
extern HWND    ChospartWindow;
extern HWND    ChosshotWindow;
extern HWND    ParameterWindow;
extern HWND    ZoomBoxWindow;
extern HWND    SmallZoomBoxWindow;
extern HWND    CornerParamDialogWindow;
extern HWND    ParamHistoryWindow;
extern HWND    ViewParametersDialogWindow;
extern HWND    MultipleShotDialogWindow;
extern HWND    ExportAlarmsDialogWindow;
extern HWND    ChooseDowntimeMachineWindow;

extern int     NofCornerParameters;
extern int     CornerParameters[];
extern short   MyScreenType;

#ifdef _WINDOW_CLASS_
extern WINDOW_CLASS XdcrWindow;
#endif
extern bool    HaveNewXdcrPart;

extern const TCHAR UnknownString[];
extern REGION_ENTRY Region[];
extern AXIS_ENTRY Axis[];

extern BOOLEAN Smoothing;
extern BOOLEAN SmoothingChannel[];
extern BOOLEAN SmoothingVelocity;

extern TCHAR DowntimeMachineName[];
extern TCHAR DowntimeComputerName[];

extern TCHAR ComputerName[];
extern TCHAR MachineName[];
extern TCHAR PartName[];
extern TCHAR ShotName[];
extern TCHAR NoPart[];
extern float PostImpactDisplayTime;

extern MARKS_CLASS     CurrentMarks;
extern PARAMETER_CLASS CurrentParam;
extern PART_CLASS      CurrentPart;

#ifdef _PARAM_INDEX_CLASS_
extern PARAM_INDEX_CLASS CurrentSort;
#endif

#ifdef _MACHINE_CLASS_
extern MACHINE_CLASS   CurrentMachine;
#endif

#ifdef _FTANALOG_CLASS_
extern FTANALOG_CLASS  CurrentFtAnalog;
#endif

extern TCHAR DateString[];

extern COLORREF AlarmColor;
extern COLORREF NoAlarmColor;
extern COLORREF WarningColor;
extern COLORREF DialogTextColor;

extern COLORREF NormalColor[];
extern COLORREF ReferenceColor[];
extern COLORREF PrNormalColor[];
extern COLORREF PrReferenceColor[];

extern COLORREF * CurrentColor;

extern BOOLEAN  RealTime;
extern BOOLEAN  SingleMachineOnly;
extern BOOLEAN  HaveHeader;
extern BOOLEAN  OverlayMenuState;
extern BOOLEAN  ViewReferenceMenuState;

extern BOOLEAN  IsPressureDisplay;
extern BOOLEAN  ViewParametersDisplay;
extern BOOLEAN  IsFullScreen;
extern BOOLEAN  DialogIsActive;
extern BOOLEAN  PrintingMultiplePlots;
extern BOOLEAN  PartBasedDisplay;
extern BOOLEAN  SnapshotIsActive;

extern short    NofParms;
extern short    NofPoints;
extern short    TimeCurve;
extern short    PosCurve;
extern float    MaxPostPosition;
extern float    MinPostPosition;
extern float    Parms[MAX_PARMS];

extern PROFILE_HEADER     Ph;
extern float              Points[MAX_CURVES][MAX_FT2_POINTS];

extern int CurvePenWidth;

#ifdef _DOWNCAT_NAME_CLASS_
extern DOWNCAT_NAME_CLASS    Cats;
extern DOWNSCAT_NAME_CLASS   SubCats;
#endif

extern HINSTANCE          MainInstance;
extern HWND               MainWindow;
extern HWND               ToolTipWindow;

extern int   MyCharHeight;
extern int   MyCharWidth;
extern int   FirstDownCatButton;
extern int   LastDownCatButton;

#ifdef _STRING_CLASS_
extern STRING_CLASS ParameterListTitle[];
#endif

/*
------------------------------------
Display corner parameter limit types
------------------------------------ */
extern int LowLimitType;
extern int HighLimitType;

extern BOOLEAN NeedNewStatusScreenColumnWidths;
extern UINT StatusScreenTimerId;
extern UINT PasswordLevelTimeoutId;
extern UINT PasswordLevelTimeoutMs;

extern FILETIME DateICanShowPressure;

extern short              HistoryShotCount;
extern short              NofPlotCurves;
extern PLOT_CURVE_ENTRY   PlotCurve[MAX_CONTINUOUS_CHANNELS];
extern BITSETYPE          PreImpactPlotCurveTypes;
extern BITSETYPE          PostImpactPlotCurveTypes;
extern short              NofOverlayShots;
extern short              CurrentPasswordLevel;
extern short              EditStatusScreenLevel;

extern PROFILE_NAME_ENTRY OverlayShot[MAX_OVERLAY_SHOTS];

extern MACHINE_NAME_LIST_CLASS MachList;

#ifdef _REALTIME_MACHINE_LIST_CLASS_
extern REALTIME_MACHINE_LIST_CLASS RealtimeMachineList;
#endif

extern TCHAR DisplayIniFile[];
extern TCHAR EditPartIniFile[];
extern TCHAR MonallIniFile[];
extern TCHAR MonallProgramName[];
extern TCHAR SureTrakIniFile[];
extern TCHAR VisiTrakIniFile[];

extern TCHAR ButtonLevelsSection[];
extern TCHAR ConfigSection[];
extern TCHAR MultipartSetupSection[];

extern TCHAR ClearDownWireFromPopupKey[];
extern TCHAR ClearMonitorWireOnDownKey[];
extern TCHAR CsvShotFileNameKey[];
extern TCHAR CsvShotFileTypeKey[];
extern TCHAR CsvUseShotNumberKey[];
extern TCHAR CurrentFtValuesGrayLevelKey[];
extern TCHAR CurvePenWidthKey[];
extern TCHAR DisplaySecondsKey[];
extern TCHAR DownTimeAlarmWireKey[];
extern TCHAR DowntimePurgeButtonKey[];
extern TCHAR EditControlButtonKey[];
extern TCHAR EditOperatorsButtonKey[];
extern TCHAR EditOtherComputerKey[];
extern TCHAR EditOtherCurrentPartKey[];
extern TCHAR EditOtherCurrentSuretrakKey[];
extern TCHAR EditOtherSuretrakKey[];
extern TCHAR EditPressureSetupLevelKey[];
extern TCHAR EditPressureProfileLevelKey[];
extern TCHAR EditStatusSceenLevelKey[];
extern TCHAR ExitAllDefaultKey[];
extern TCHAR HaveTouchScreenKey[];
extern TCHAR HaveMultipartRunlistKey[];
extern TCHAR HaveSureTrakKey[];
extern TCHAR HideSureTrakLimitSwitchWiresKey[];
extern TCHAR HighLimitTypeKey[];
extern TCHAR Ioex4GrayLevelKey[];
extern TCHAR Ioex4MenuGrayLevelKey[];
extern TCHAR LastMachineKey[];
extern TCHAR LastPartKey[];
extern TCHAR LastShotKey[];
extern TCHAR LockOnAlarmKey[];
extern TCHAR LowLimitTypeKey[];
extern TCHAR MachineBasedPlotDisplayKey[];
extern TCHAR MainFontNameKey[];
extern TCHAR MonitorWireKey[];
extern TCHAR MultipartBaseWireKey[];
extern TCHAR NoNetworkKey[];
extern TCHAR ParameterTabEditLevelKey[];
extern TCHAR ParameterWindowFontKey[];
extern TCHAR PasswordLevelTimeoutKey[];
extern TCHAR PasswordsAreEncriptedKey[];
extern TCHAR PartBasedDisplayKey[];
extern TCHAR PrintParametersKey[];
extern TCHAR SaveAllAlarmShotsKey[];
extern TCHAR SaveAllPartsLevelKey[];
extern TCHAR SaveChangesLevelKey[];
extern TCHAR ServiceToolsGrayLevelKey[];
extern TCHAR SetupSheetLevelKey[];
extern TCHAR ShowSetupSheetKey[];
extern TCHAR SmoothFactorKey[];
extern TCHAR SmoothNKey[];
extern TCHAR SmoothCurvesKey[];
extern TCHAR SnapshotKey[];
extern TCHAR StExitLevelKey[];
extern TCHAR SubtractTimeoutKey[];
extern TCHAR UseDowntimeEntryKey[];
extern TCHAR UseValveTestOutputKey[];
extern TCHAR UseNullValveOutputKey[];
extern TCHAR VelocityChangesLevelKey[];
extern TCHAR VersionKey[];

#endif
