#define BOARD_NUMBER_COLUMN   0
#define TCP_ADDRESS_COLUMN    1
#define TCP_PORT_COLUMN       2
#define CONNECTED_COLUMN      3
#define MACHINE_NAME_COLUMN   4
#define PART_NAME_COLUMN      5
#define MONITOR_STATUS_COLUMN 6
#define NofCols               7
#define ALL_COLUMNS          10
#define ANALOG_VALUE_COUNT   24
#define NOF_OP_STATUS_ANALOGS 16

struct CONTROL_INFO {
    int  id;
    HWND w;
    };

#define NOT_SHUTTING_DOWN        0
#define SHUTTING_DOWN_MONITORING 1
#define SHUTTING_DOWN_SHOTSAVE   2
#define SHUTTING_DOWN_REMOTE     3
#define SHUTDOWN_COMPLETE        4

#ifdef _MAIN_
int     NofRows = 1;   /* Number of rows    in Grid */

HINSTANCE MainInstance;

WINDOW_CLASS CurrentWindow;
WINDOW_CLASS MainWindow;
WINDOW_CLASS AutoShotDisplayWindow;
WINDOW_CLASS ButtonDialog;
WINDOW_CLASS CurrentDacDialog;
WINDOW_CLASS CurrentValuesDialog;
WINDOW_CLASS GlobalParameterDialog;
WINDOW_CLASS ControlModsDialog;
WINDOW_CLASS SetNullDialog;
WINDOW_CLASS SetDacDialog;
WINDOW_CLASS TestAlarmOutputsDialog;
WINDOW_CLASS UploadFlashDialog;
WINDOW_CLASS ValveTestDialog;
WINDOW_CLASS HiresSensorTestDialog;
WINDOW_CLASS UploadAbortedDialog;

/*
-------------
Main Controls
------------- */
int  Char_Width  = 14;
int  Line_Height = 14;

CONTROL_INFO * MainPushButton    = 0;
CONTROL_INFO * AddressTbox       = 0;
CONTROL_INFO * PortTbox          = 0;
CONTROL_INFO * ConnectedTbox     = 0;
CONTROL_INFO * MachineTbox       = 0;
CONTROL_INFO * PartTbox          = 0;
CONTROL_INFO * MonitorStatusTbox = 0;

HWND          MainEbox = 0;
LISTBOX_CLASS MainListBox;

BOOLEAN TerminalIsVisible = FALSE;
bool    BoardMonitorIsRunning = false;
bool    ShotSaveIsRunning     = false;

BOOLEAN ClearAlarmsEachShot         = FALSE;
BOOLEAN ClearMonitorWireOnDown      = FALSE;
BOOLEAN ClearDownWireFromPopup      = FALSE;
BOOLEAN SaveAllAlarmShots           = FALSE;
BOOLEAN SubtractTimeoutFromAutoDown = FALSE;
BOOLEAN HasDigitalServoAmp          = FALSE;  /* Refers to CurrentBoard  */
BOOLEAN NeedErrorLog                = FALSE;
BOOLEAN IsHiresSensor               = FALSE;
BOOLEAN IsRemoteMonitor             = FALSE;
BOOLEAN AlsLogIsActive              = FALSE;
BOOLEAN LoggingSentCommands         = FALSE;

int      DownTimeAlarmWire  = NO_WIRE;
int      MonitorWire        = NO_WIRE;
int      CurrentBoardIndex  = NO_INDEX;

unsigned NoUploadWire     = NO_WIRE;
unsigned NoUploadWireMask = 0;
unsigned RetractWire        = NO_WIRE;
unsigned RetractWireMask    = 0x080000;

/*
------------------------------------------------
Machine and part of the currently selected board
------------------------------------------------ */
MACHINE_CLASS CurrentMachine;
PART_CLASS    CurrentPart;

STRING_CLASS ComputerName;
STRING_CLASS NoConnectState;
STRING_CLASS NoResponse;
STRING_CLASS NotMonitored;
STRING_CLASS NotConnected;
STRING_CLASS Connected;
STRING_CLASS Connecting;
STRING_CLASS ZeroStrokeLength;
STRING_CLASS UploadFailed;

/*
------------
Ft2 Timeouts
------------ */
unsigned UploadRetractTimeout    = 0;

/*
----------------
Connected states
---------------- */
unsigned NO_FT2_CONNECT_STATE = 0;
unsigned NOT_CONNECTED_STATE  = 1;
unsigned NOT_MONITORED_STATE  = 2;
unsigned CONNECTING_STATE     = 4;
unsigned CONNECTED_STATE      = 8;
unsigned ZERO_STROKE_LENGTH_STATE = 16;
unsigned UPLOAD_WAS_ABORTED_STATE = 32;

PART_ANALOG_SENSOR AnalogSensor;
int                HistoryShotCount = 50;


int   ShutdownState = NOT_SHUTTING_DOWN;

TCHAR ConfigSection[]             = TEXT( "Config" );
TCHAR CurrentValuesSection[]      = TEXT( "CurrentValues" );

TCHAR FtiiIniFile[]               = TEXT( "ftii.ini" );
TCHAR MonallIniFile[]             = TEXT( "monall.ini" );
TCHAR SureTrakIniFile[]           = TEXT( "suretrak.ini" );
TCHAR VisiTrakIniFile[]           = TEXT( "visitrak.ini" );

TCHAR AlsLogFile[]                =  TEXT( "C:\\io_log_file.txt" );
TCHAR AlsLogIsActiveKey[]         = TEXT( "NewLog" );
TCHAR ClearAlarmsEachShotKey[]    = TEXT( "ClearAlarmsEachShot" );
TCHAR ClearDownWireFromPopupKey[] = TEXT( "ClearDownWireFromPopup" );
TCHAR ClearMonitorWireOnDownKey[] = TEXT( "ClearMonitorWireOnDown" );
TCHAR CurrentChannelModeKey[]     = TEXT( "CurrentChannelMode" );
TCHAR DownTimeAlarmWireKey[]      = TEXT( "DownTimeAlarmWire" );
TCHAR ExitEnableLevelKey[]        = TEXT( "ExitEnableLevel" );
TCHAR FlashVersionKey[]           = TEXT( "FlashVersion" );
TCHAR HavePressureControlKey[]    = TEXT( "HavePressureControl" );
TCHAR HaveSureTrakKey[]           = TEXT( "HaveSureTrak" );
TCHAR IsHiresSensorKey[]          = TEXT( "IsHiresSensor" );
TCHAR LogSentCommandsKey[]        = TEXT( "LogSentCommands" );
TCHAR LastNKey[]                  = TEXT( "LastN" );
TCHAR MonitorWireKey[]            = TEXT( "MonitorWire" );
TCHAR NoUploadWireKey[]         = TEXT( "NoUploadWire" );    /* Use this wire instead of cycle start message */
TCHAR RetractWireKey[]            = TEXT( "RetractWire" );       /* Use this wire instead of At home message */
TCHAR SaveAllAlarmShotsKey[]      = TEXT( "SaveAllAlarmShots" );
TCHAR SentCommandsLogFile[]       = TEXT( "C:\\sent_commands_log_file.txt" );
TCHAR ServiceToolsGrayLevelKey[]  = TEXT( "ServiceToolsGrayLevel" );
TCHAR SubtractTimeoutKey[]        = TEXT( "SubtractTimeout" );

TCHAR UploadRetractTimeoutKey[]   = TEXT( "UploadRetractTimeout" );
TCHAR UseNullValveOutputKey[]       = TEXT( "UseNullValveOutput" );
TCHAR UseValveTestOutputKey[]       = TEXT( "UseValveTestOutput" );

TCHAR StartControlPgmCmd[]   = TEXT(".G");
TCHAR EnableShotXmitCmd[]    = TEXT( "EXMIT" );
TCHAR DisableShotXmitCmd[]   = TEXT( "DXMIT" );
int   ShotXmitCmdLen         = 5;

TCHAR START_OF_UPLOAD[]      = TEXT( "START_OF_UPLOAD" );
TCHAR END_OF_UPLOAD[]        = TEXT( "END_OF_UPLOAD"   );
TCHAR RESET_MACHINE_CMD[]    = TEXT( "RESET_MACHINE"   );
TCHAR CLEAR_DO_NOT_UPLOAD_CMD[] = TEXT( "CLEAR_DO_NOT_UPLOAD" );

TCHAR DONT_SEND_OP_DATA_PACKETS[] = TEXT( "OC3=0" );
TCHAR SEND_OP_DATA_PACKETS[]      = TEXT( "OC3=2" );

COLORREF NormalCellColor     = RGB( 255, 255, 255 );
COLORREF GreenColor          = RGB(   0, 255,   0 );
COLORREF OrangeColor         = RGB( 255,  50,  35 );
COLORREF RedColor            = RGB( 255,   0,   0 );

FTII_OP_STATUS_DATA CurrentOpStatus;
bool                CurrentOpStatusIsEmpty = true;
unsigned            CurrentAnalogValue[ANALOG_VALUE_COUNT] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

BLOCK_MESSAGE_CLASS StepMessage;

#else

extern       int NofRows;

extern HINSTANCE MainInstance;

#ifdef _WINDOW_CLASS_
extern WINDOW_CLASS CurrentWindow;
extern WINDOW_CLASS MainWindow;
extern WINDOW_CLASS AutoShotDisplayWindow;
extern WINDOW_CLASS ButtonDialog;
extern WINDOW_CLASS CurrentDacDialog;
extern WINDOW_CLASS CurrentValuesDialog;
extern WINDOW_CLASS GlobalParameterDialog;
extern WINDOW_CLASS ControlModsDialog;
extern WINDOW_CLASS SetNullDialog;
extern WINDOW_CLASS SetDacDialog;
extern WINDOW_CLASS TestAlarmOutputsDialog;
extern WINDOW_CLASS UploadFlashDialog;
extern WINDOW_CLASS ValveTestDialog;
#endif

extern int  Char_Width;
extern int  Line_Height;

extern CONTROL_INFO * MainPushButton;
extern CONTROL_INFO * AddressTbox;
extern CONTROL_INFO * PortTbox;
extern CONTROL_INFO * ConnectedTbox;
extern CONTROL_INFO * MachineTbox;
extern CONTROL_INFO * PartTbox;
extern CONTROL_INFO * MonitorStatusTbox;

extern HWND          MainEbox;

#ifdef _LISTBOX_CLASS_
extern LISTBOX_CLASS MainListBox;
#endif

extern BOOLEAN TerminalIsVisible;
extern bool    BoardMonitorIsRunning;
extern bool    ShotSaveIsRunning;

extern BOOLEAN ClearAlarmsEachShot;
extern BOOLEAN ClearMonitorWireOnDown;
extern BOOLEAN ClearDownWireFromPopup;
extern BOOLEAN SaveAllAlarmShots;
extern BOOLEAN SubtractTimeoutFromAutoDown;
extern BOOLEAN HasDigitalServoAmp;
extern BOOLEAN NeedErrorLog;
extern BOOLEAN IsHiresSensor;
extern BOOLEAN IsRemoteMonitor;
extern BOOLEAN AlsLogIsActive;
extern BOOLEAN LoggingSentCommands;

extern int      DownTimeAlarmWire;
extern int      MonitorWire;
extern int      CurrentBoardIndex;

extern unsigned NoUploadWire;
extern unsigned NoUploadWireMask;
extern unsigned RetractWire;
extern unsigned RetractWireMask;

#ifdef _MACHINE_CLASS_
extern MACHINE_CLASS CurrentMachine;
#endif

#ifdef _PART_CLASS_
extern PART_CLASS CurrentPart;
#endif

#ifdef _STRING_CLASS_
extern STRING_CLASS ComputerName;
extern STRING_CLASS NoConnectState;
extern STRING_CLASS NoResponse;
extern STRING_CLASS NotMonitored;
extern STRING_CLASS NotConnected;
extern STRING_CLASS Connected;
extern STRING_CLASS Connecting;
extern STRING_CLASS ZeroStrokeLength;
extern STRING_CLASS UploadFailed;
#endif

extern unsigned UploadRetractTimeout;

extern unsigned NO_FT2_CONNECT_STATE;
extern unsigned NOT_CONNECTED_STATE;
extern unsigned NOT_MONITORED_STATE;
extern unsigned CONNECTING_STATE;
extern unsigned CONNECTED_STATE;
extern unsigned ZERO_STROKE_LENGTH_STATE;
extern unsigned UPLOAD_WAS_ABORTED_STATE;

#ifdef _PLOT_ANALOG_SENSOR_
extern PART_ANALOG_SENSOR AnalogSensor;
#endif

extern int HistoryShotCount;
extern int ShutdownState;

extern TCHAR ConfigSection[];
extern TCHAR CurrentValuesSection[];

extern TCHAR FtiiIniFile[];
extern TCHAR MonallIniFile[];
extern TCHAR SureTrakIniFile[];
extern TCHAR VisiTrakIniFile[];

extern TCHAR AlsLogFile[];
extern TCHAR ClearDownWireFromPopupKey[];
extern TCHAR ClearMonitorWireOnDownKey[];
extern TCHAR CurrentChannelModeKey[];
extern TCHAR DownTimeAlarmWireKey[];
extern TCHAR ExitEnableLevelKey[];
extern TCHAR FlashVersionKey[];
extern TCHAR HavePressureControlKey[];
extern TCHAR HaveSureTrakKey[];
extern TCHAR IsHiresSensorKey[];
extern TCHAR LastNKey[];
extern TCHAR MonitorWireKey[];
extern TCHAR NoUploadWireKey[];
extern TCHAR RetractWireKey[];
extern TCHAR SaveAllAlarmShotsKey[];
extern TCHAR SentCommandsLogFile[];
extern TCHAR ServiceToolsGrayLevelKey[];
extern TCHAR SubtractTimeoutKey[];
extern TCHAR UseNullValveOutputKey[];
extern TCHAR UseValveTestOutputKey[];

extern TCHAR StartControlPgmCmd[];
extern TCHAR EnableShotXmitCmd[];
extern TCHAR DisableShotXmitCmd[];
extern int   ShotXmitCmdLen;

extern TCHAR START_OF_UPLOAD[];
extern TCHAR END_OF_UPLOAD[];
extern TCHAR RESET_MACHINE_CMD[];
extern TCHAR CLEAR_DO_NOT_UPLOAD_CMD[];

extern TCHAR DONT_SEND_OP_DATA_PACKETS[];
extern TCHAR SEND_OP_DATA_PACKETS[];

extern COLORREF NormalCellColor;
extern COLORREF GreenColor;
extern COLORREF OrangeColor;
extern COLORREF RedColor;

#ifdef _FTII_SHOT_DATA_
extern FTII_OP_STATUS_DATA CurrentOpStatus;
extern bool                CurrentOpStatusIsEmpty;
#endif

#ifdef _BLOCK_MESSAGE_CLASS_
extern BLOCK_MESSAGE_CLASS StepMessage;
#endif

extern unsigned CurrentAnalogValue[20];

#endif

