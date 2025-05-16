const BOARD_DATA DEACTIVATE_CONTROL_REQUEST = 1;
const BOARD_DATA ACTIVATE_CONTROL_REQUEST   = 2;
const BOARD_DATA CONTROLLER_RESET_REQUEST   = 3;
const BOARD_DATA SET_PARAMETER_REQUEST      = 4;
const BOARD_DATA SET_VEL_DIVIDEND_REQUEST   = 5;
const BOARD_DATA SEND_DATA_REQUEST          = 6;
const BOARD_DATA CLEAR_COMMAND_PORT_REQUEST = 7;
const BOARD_DATA SEND_COMMAND_REQUEST       = 8;
const BOARD_DATA SEND_COMMAND_BUSY_REQUEST  = 9;
const BOARD_DATA NULL_VALVE_INIT_REQUEST    = 10;
const BOARD_DATA AUTO_NULL_REQUEST          = 11;
const BOARD_DATA SET_VARIABLE_REQUEST       = 12;
const BOARD_DATA READ_VARIABLE_REQUEST      = 13;
const BOARD_DATA VALVE_TEST_REQUEST         = 14;
const BOARD_DATA PROGRAM_DOWNLOAD_REQUEST   = 15;
const BOARD_DATA PROFILE_DOWNLOAD_REQUEST   = 16;
const BOARD_DATA SEND_DATA_COMMAND_BUSY_REQUEST  = 17;
const BOARD_DATA READ_CONTROL_PARAMETERS_REQUEST = 18;
const BOARD_DATA TRANSDUCER_TEST_REQUEST    = 19;
const BOARD_DATA RAM_TEST_REQUEST           = 20;
const BOARD_DATA OPTO_PORT_TEST_REQUEST     = 21;
const BOARD_DATA IO_PORT_TEST_REQUEST       = 22;
const BOARD_DATA NULL_VALVE_FINISH_REQUEST  = 23;

/*
-----------------------------------
SureTrak get/set parameter commands
----------------------------------- */

const BOARD_DATA CCLR_ERR            = 0x8004;
const BOARD_DATA COUT_TO_DAC         = 0x8005;
const BOARD_DATA TEST_C_DIO          = 0x8006;
const BOARD_DATA CSET_REG_TST_FLG    = 0x8007;
const BOARD_DATA CRAM_TEST           = 0x8008;
const BOARD_DATA CDATA_REG_WR_TST    = 0x800F;

const BOARD_DATA CSET_LS1            = 0x8016;

const BOARD_DATA RD_CONT_ERR_WORD    = 0x8024;   /* Fatal error number */
const BOARD_DATA START_VALVE_TEST    = 0x8025;
const BOARD_DATA WR_CONT_OPTO        = 0x8026;
const BOARD_DATA ZERO_DAC            = 0x8029;
const BOARD_DATA NULL_VALVE          = 0x802A;
const BOARD_DATA SET_PROG_DOWNLOADED = 0x802C;
const BOARD_DATA CHECK_XDCR          = 0x802B;

const BOARD_DATA SET_V_LOOP_GAIN      = 0xFFE4;
const BOARD_DATA SET_BK_VEL1          = 0xFFE5;
const BOARD_DATA SET_BKGN1            = 0xFFE8;
const BOARD_DATA SET_VEL_DIVIDEND     = 0xFFED;
const BOARD_DATA SET_COUNTS_PER_UNIT  = 0xFFF0;
const BOARD_DATA SET_P_CHECK_ZSPEED   = 0xFFEF;
const BOARD_DATA SET_NULL             = 0xFFF1;
const BOARD_DATA SET_LVDT_OFFSET      = 0xFFF2;
const BOARD_DATA SET_JS_SETTING       = 0xFFF3;
const BOARD_DATA SET_RETR_SETTING     = 0xFFF4;
const BOARD_DATA SET_FT_SETTING       = 0xFFF5;
const BOARD_DATA SET_VEL_MUL          = 0xFFF6;
const BOARD_DATA SET_SEN_DEV          = 0xFFF7;   /* V119 */
const BOARD_DATA SET_PRESS_CHK_ZSPEED = 0xFFF8;   /* V120 */
const BOARD_DATA SET_VEL_CHK_ZSPEED   = 0xFFF9;   /* V121 */
const BOARD_DATA SET_FT_STOP_POS      = 0xFFFA;

const BOARD_DATA VALVE_TEST_WIRE_BIT  = 0x0010;
const BOARD_DATA MANUAL_MODE_BIT      = 0x0020;

const BOARD_DATA SET_ACCEL1           = 0xFF81;     /* V1  */
const BOARD_DATA SET_ACCEL2           = 0xFF88;     /* V8  */
const BOARD_DATA SET_ACCEL3           = 0xFF8F;     /* V15 */
const BOARD_DATA SET_ACCEL4           = 0xFF96;     /* V22 */
const BOARD_DATA SET_ACCEL5           = 0xFF9D;     /* V29 */
const BOARD_DATA SET_ACCEL6           = 0xFFA4;     /* V36 */

const BOARD_DATA SET_GOOSE1           = 0xFF82;     /* V2  */
const BOARD_DATA SET_GOOSE2           = 0xFF89;     /* V9  */
const BOARD_DATA SET_GOOSE3           = 0xFF90;     /* V16 */
const BOARD_DATA SET_GOOSE4           = 0xFF97;     /* V23 */
const BOARD_DATA SET_GOOSE5           = 0xFF9E;     /* V30 */
const BOARD_DATA SET_GOOSE6           = 0xFFA5;     /* V37 */

const BOARD_DATA SET_POS1            = 0xFF83;     /* V3  */
const BOARD_DATA SET_POS2            = 0xFF8A;     /* V10 */
const BOARD_DATA SET_POS3            = 0xFF91;     /* V17 */
const BOARD_DATA SET_POS4            = 0xFF98;     /* V24 */
const BOARD_DATA SET_POS5            = 0xFF9F;     /* V31 */
const BOARD_DATA SET_POS6            = 0xFFA6;     /* V38 */

const BOARD_DATA SET_VEL1            = 0xFF80;     /* V0  */
const BOARD_DATA SET_VEL2            = 0xFF87;     /* V7  */
const BOARD_DATA SET_VEL3            = 0xFF8E;     /* V14 */
const BOARD_DATA SET_VEL4            = 0xFF95;     /* V21 */
const BOARD_DATA SET_VEL5            = 0xFF9C;     /* V28 */
const BOARD_DATA SET_VEL6            = 0xFFA3;     /* V35 */

const BOARD_DATA SET_VAC1            = 0xFF86;     /* V6  */
const BOARD_DATA SET_VAC2            = 0xFF8D;     /* V13 */
const BOARD_DATA SET_VAC3            = 0xFF94;     /* V20 */
const BOARD_DATA SET_VAC4            = 0xFF9B;     /* V27 */
const BOARD_DATA SET_VAC5            = 0xFFA2;     /* V34 */
const BOARD_DATA SET_VAC6            = 0xFFA9;     /* V41 */

const BOARD_DATA SET_LICMD1          = 0xFF84;     /* V4  */
const BOARD_DATA SET_LICMD2          = 0xFF8B;     /* V11 */
const BOARD_DATA SET_LICMD3          = 0xFF92;     /* V18 */
const BOARD_DATA SET_LICMD4          = 0xFF99;     /* V25 */
const BOARD_DATA SET_LICMD5          = 0xFFA0;     /* V32 */
const BOARD_DATA SET_LICMD6          = 0xFFA7;     /* V39 */

const BOARD_DATA SET_LI_IO1          = 0xFF85;     /* V5  */
const BOARD_DATA SET_LI_IO2          = 0xFF8C;     /* V12 */
const BOARD_DATA SET_LI_IO3          = 0xFF93;     /* V19 */
const BOARD_DATA SET_LI_IO4          = 0xFF9A;     /* V26 */
const BOARD_DATA SET_LI_IO5          = 0xFFA1;     /* V33 */
const BOARD_DATA SET_LI_IO6          = 0xFFA8;     /* V40 */

const BOARD_DATA SET_RETR            = 0xFFB2;     /* V50 */
const BOARD_DATA SET_FT              = 0xFFB3;     /* V51 */
const BOARD_DATA SET_JS              = 0xFFB4;     /* V52 */

const int32      NofValveTestPoints  = 500;

#ifdef _MAIN_

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MainScreenWindow;
WINDOW_CLASS ValveTestWindow;
HWND         CurrentAnalogWindow     = 0;
HWND         CurrentParametersWindow = 0;
HWND         PromNumberWindow        = 0;
HWND         DataPortWindow          = 0;
HWND         RamBlockWindow          = 0;
HWND         LoadStatusWindow        = 0;
HWND         NullValveWindow         = 0;
HWND         ServiceToolsWindow      = 0;
HWND         TransducerTestWindow    = 0;
HWND         VariablesWindow         = 0;

int32        RamBlockNumber          = -1;

double       PosSensorVelocityDividend = 150000;
short        DistanceUnits       = INCH_UNITS;
short        VelocityUnits       = IPS_UNITS;
short        CurrentPasswordLevel  = 5;
short        ServiceToolsGrayLevel = 20;
short        ExitEnableLevel       = 0;

int32        RightVelPlaces = 2;
int32        LeftVelPlaces  = 3;
int32        RightPosPlaces = 2;
int32        LeftPosPlaces  = 4;
int32        RightAnalogPlaces = 2;
int32        LeftAnalogPlaces  = 4;

PART_NAME_ENTRY      SureTrakPart;
SURETRAK_SETUP_CLASS SureTrakSetup;
SURETRAK_PARAM_CLASS SureTrakParameters;
RING_CLASS           SureTrakCommand;
BLOCK_MESSAGE_CLASS  SureTrakBlockMessage;

BOOLEAN              NeedRunlistCheck = FALSE;
BOOLEAN              HaveRunlist      = FALSE;
RUNLIST_CLASS        Runlist;

HANDLE       SureTrakMonitorEvent;
BOARD_DATA   SureTrakRawPosition;
BOARD_DATA   SureTrakStatus;
BOARD_DATA   SureTrakDacWord;
BOARD_DATA   SureTrakInputWord;
BOARD_DATA   SureTrakOutputWord;

BOARD_DATA   SureTrakAnalogValue[MAX_ST_CHANNELS] = { 0, 0, 0, 0 };
BOARD_DATA   SureTrakVelocity = 0xFFFF;

BOARD_DATA   SureTrakBlockNumber;
BOARD_DATA   SureTrakFatalErrorNumber;
BOOLEAN      SureTrakHasProgram;
BOOLEAN      SureTrakMonitorIsRunning;
BOOLEAN      NeedMonitorUpdate = TRUE;
BOOLEAN      ShuttingDown;
uint32       RamBlock[2048];
FLOAT_POINT  MasterData[NofValveTestPoints];
FLOAT_POINT  ValveTestData[NofValveTestPoints];
bool         Downloading = false;

#else

extern HINSTANCE    MainInstance;
#ifdef _WINDOW_CLASS_
extern WINDOW_CLASS MainWindow;
extern WINDOW_CLASS MainScreenWindow;
extern WINDOW_CLASS ValveTestWindow;
#endif
extern HWND         CurrentAnalogWindow;
extern HWND         CurrentParametersWindow;
extern HWND         PromNumberWindow;
extern HWND         DataPortWindow;
extern HWND         RamBlockWindow;
extern HWND         LoadStatusWindow;
extern HWND         NullValveWindow;
extern HWND         ServiceToolsWindow;
extern HWND         TransducerTestWindow;
extern HWND         VariablesWindow;
extern int32        RamBlockNumber;
extern double       PosSensorVelocityDividend;
extern short        DistanceUnits;
extern short        VelocityUnits;
extern short        CurrentPasswordLevel;
extern short        ServiceToolsGrayLevel;
extern short        ExitEnableLevel;

extern int32     RightVelPlaces;
extern int32     LeftVelPlaces;
extern int32     RightPosPlaces;
extern int32     LeftPosPlaces;
extern int32     RightAnalogPlaces;
extern int32     LeftAnalogPlaces;

#ifdef _PLOT_CLASS_
extern FLOAT_POINT  MasterData[];
extern FLOAT_POINT  ValveTestData[];
#endif

#ifdef _STRUCTS_
extern PART_NAME_ENTRY  SureTrakPart;
#endif

#ifdef _SURETRAK_SETUP_CLASS_
extern SURETRAK_SETUP_CLASS SureTrakSetup;
#endif

#ifdef _SURETRAK_PARAM_CLASS_
extern SURETRAK_PARAM_CLASS SureTrakParameters;
#endif

#ifdef _RING_CLASS_
extern RING_CLASS   SureTrakCommand;
#endif

#ifdef _BLOCK_MESSAGE_CLASS_
extern BLOCK_MESSAGE_CLASS  SureTrakBlockMessage;
#endif

#ifdef _RUNLIST_CLASS_
extern BOOLEAN       NeedRunlistCheck;
extern BOOLEAN       HaveRunlist;
extern RUNLIST_CLASS Runlist;
#endif

extern HANDLE       SureTrakMonitorEvent;
extern BOARD_DATA   SureTrakRawPosition;
extern BOARD_DATA   SureTrakInputWord;
extern BOARD_DATA   SureTrakOutputWord;
extern BOARD_DATA   SureTrakAnalogValue[];
extern BOARD_DATA   SureTrakVelocity;
extern BOARD_DATA   SureTrakStatus;
extern BOARD_DATA   SureTrakDacWord;
extern BOARD_DATA   SureTrakBlockNumber;
extern BOARD_DATA   SureTrakFatalErrorNumber;
extern BOOLEAN      SureTrakHasProgram;
extern BOOLEAN      SureTrakMonitorIsRunning;
extern BOOLEAN      NeedMonitorUpdate;
extern BOOLEAN      ShuttingDown;
extern uint32       RamBlock[];
extern bool         Downloading;

#endif

