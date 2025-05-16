const BOARD_DATA COMMAND_MODE_REQUEST         = 1;
const BOARD_DATA DATA_COLLECTION_MODE_REQUEST = 2;
const BOARD_DATA AUTO_COLLECTION_MODE_REQUEST = 3;
const BOARD_DATA SCAN_MODE_REQUEST            = 4;
const BOARD_DATA RESET_MAX_POSITION_REQUEST   = 6;
const BOARD_DATA SET_BOARD_REQUEST            = 7;
const BOARD_DATA FILL_SETUP_PARAMS_REQUEST    = 8;
const BOARD_DATA FILL_SHOT_PARAMS_REQUEST     = 9;
const BOARD_DATA CLEAR_DATA_READY_BIT_REQUEST = 10;
const BOARD_DATA CLEAR_ERROR_BITS_REQUEST     = 11;
const BOARD_DATA SET_MUX_BOARD_REQUEST        = 12;
const BOARD_DATA SET_SINGLE_BOARD_REQUEST     = 13;
const BOARD_DATA SET_MUX_CHANNEL_REQUEST      = 14;
const BOARD_DATA SET_MUX_MASK_REQUEST         = 15;
const BOARD_DATA GET_VERSION_REQUEST          = 16;
const BOARD_DATA RAM_TEST_REQUEST             = 17;
const BOARD_DATA IO_PORT_TEST_REQUEST         = 18;
const BOARD_DATA OPTO_PORT_TEST_REQUEST       = 19;
const BOARD_DATA TRANSDUCER_TEST_REQUEST      = 20;
const BOARD_DATA READ_DATA_PORT_REQUEST       = 21;
const BOARD_DATA SEND_DATA_PORT_REQUEST       = 22;
const BOARD_DATA CLEAR_COMMAND_PORT_REQUEST   = 23;
const BOARD_DATA SEND_COMMAND_PORT_REQUEST    = 24;
const BOARD_DATA CLEAR_COUNTERS_REQUEST       = 25;
const BOARD_DATA LATCH_COUNTERS_REQUEST       = 26;
const BOARD_DATA RESET_REQUEST                = 27;
const BOARD_DATA CLEAR_OUTPUT_OPTOS_REQUEST   = 28;
const BOARD_DATA SET_OUTPUT_OPTOS_REQUEST     = 29;

#ifdef _MAIN_

HINSTANCE MainInstance;
HWND      MainWindow;

HWND         HardwareStatusWindow  = 0;
HWND         ManualControlWindow   = 0;
HWND         AutoShotDisplayWindow = 0;
HWND         FtCurrentValuesWindow = 0;
HWND         FtSetupParamsWindow   = 0;
HWND         FtShotParamsWindow    = 0;
HWND         TransducerTestWindow  = 0;
HWND         FtCommandPortWindow   = 0;
WINDOW_CLASS Ioex4Window;

HMENU MainMenu = 0;

short     CurrentPasswordLevel     = 5;
short     Ioex4GrayLevel           = 0;
short     Ioex4MenuGrayLevel       = SYSTEM_PASSWORD_LEVEL;
short     CurrentFtValuesGrayLevel = SYSTEM_PASSWORD_LEVEL;
short     EpiSkipCount             = 5;

int32     RightVelPlaces = 2;
int32     LeftVelPlaces  = 3;
int32     RightPosPlaces = 2;
int32     LeftPosPlaces  = 4;
int32     RightAnalogPlaces = 2;
int32     LeftAnalogPlaces  = 4;

BOOLEAN   Ioex4IsActive = FALSE;
BOOLEAN   HaveSureTrakControl = FALSE;
BOOLEAN   ShowCurrentFtValuesOnly = FALSE;
HANDLE    FasTrakMutex;
HANDLE    BoardMonitorEvent;

TCHAR     ComputerName[COMPUTER_NAME_LEN+1];

TCHAR     StringTerminator = 0;
TCHAR     NoComputer[] = NO_COMPUTER;

BOOLEAN   BoardMonitorIsRunning  = FALSE;
BOOLEAN   ShotSaveIsRunning      = FALSE;
BOOLEAN   ShuttingDown           = FALSE;
BOOLEAN   ClearMonitorWireOnDown = FALSE;
BOOLEAN   SubtractTimeoutFromAutoDown = FALSE;
BOOLEAN   ClearDownWireFromPopup = FALSE;
BOOLEAN   HaveEosUdp             = FALSE;
BITSETYPE LockOnAlarmMask        = 0;

TCHAR     NewShotMachineName[MACHINE_NAME_LEN+1];
TCHAR     NewShotNumber[SHOT_LEN+1];

short     DownTimeAlarmWire = NO_WIRE;
short     MonitorWire       = NO_WIRE;
short     HistoryShotCount  = 50;

RING_CLASS MachinesToReset;
RING_CLASS FasTrakCommand;

#else
extern HINSTANCE MainInstance;
extern HWND      MainWindow;
extern HWND      HardwareStatusWindow;
extern HWND      ManualControlWindow;
extern HWND      AutoShotDisplayWindow;
extern HWND      FtCurrentValuesWindow;
extern HWND      FtSetupParamsWindow;
extern HWND      FtShotParamsWindow;
extern HWND      TransducerTestWindow;
extern HWND      FtCommandPortWindow;

extern HMENU     MainMenu;

#ifdef _WINDOW_CLASS_
extern WINDOW_CLASS Ioex4Window;
#endif

extern short     CurrentPasswordLevel;
extern short     Ioex4GrayLevel;
extern short     Ioex4MenuGrayLevel;
extern short     CurrentFtValuesGrayLevel;
extern short     EpiSkipCount;

extern int32     RightVelPlaces;
extern int32     LeftVelPlaces;
extern int32     RightPosPlaces;
extern int32     LeftPosPlaces;
extern int32     RightAnalogPlaces;
extern int32     LeftAnalogPlaces;

extern BOOLEAN   Ioex4IsActive;
extern BOOLEAN   HaveSureTrakControl;
extern BOOLEAN   ShowCurrentFtValuesOnly;

extern HANDLE    FasTrakMutex;
extern HANDLE    BoardMonitorEvent;

extern TCHAR     ComputerName[];
extern TCHAR     StringTerminator;
extern TCHAR     NoComputer[];

extern BOOLEAN   BoardMonitorIsRunning;
extern BOOLEAN   ShotSaveIsRunning;
extern BOOLEAN   ShuttingDown;
extern BOOLEAN   ClearMonitorWireOnDown;
extern BOOLEAN   SubtractTimeoutFromAutoDown;
extern BOOLEAN   ClearDownWireFromPopup;
extern BOOLEAN   HaveEosUdp;

#ifdef _VISIPARM_H
extern BITSETYPE LockOnAlarmMask;
#endif
extern TCHAR     NewShotMachineName[];
extern TCHAR     NewShotNumber[];
extern short     DownTimeAlarmWire;
extern short     MonitorWire;
extern short     HistoryShotCount;

#ifdef _RING_CLASS_
extern RING_CLASS MachinesToReset;
extern RING_CLASS FasTrakCommand;
#endif

#endif

