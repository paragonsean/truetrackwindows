#ifndef _FTII_H_
#define _FTII_H_

#define NO_FTII_VAR int(-1)
#define MAX_FTII_CONTROL_STEPS 20

/*
------------------------
Control Status Word V311
------------------------ */
#define FATAL_ERROR_BIT     unsigned(0x01)
#define WARNING_BIT         unsigned(0x02)
#define PROGRAM_LOADED_BIT  unsigned(0x04)
#define PROGRAM_RUNNING_BIT unsigned(0x08)
#define FREE_DAC_2_BIT      unsigned(0x10)
#define FREE_DAC_4_BIT      unsigned(0x20)

/*
---------------------------
Monitoring Status Word V312
--------------------------- */
#define CYCLE_START_BIT         unsigned(0x01)
#define SHOT_IN_PROGRESS_BIT    unsigned(0x02)
#define CURRENTLY_TIMING_BIT    unsigned(0x04)
#define PAST_MIN_STROKE_LEN_BIT unsigned(0x08)
#define ZSPEED_BIT              unsigned(0x10)
#define SHOT_COMPLETE_BIT       unsigned(0x20)
#define MONITOR_ENABLE_BIT      unsigned(0x40)
#define PAST_MIN_EOS_VEL_BIT    unsigned(0x80)

/*
----------------
Config Word V313
---------------- */
#define A17TO20_ON_BIT unsigned(0x10000)

/*
------------------
Warning Words V315
------------------ */
#define NO_WARNINGS               int(0)
#define VAC_ABORT_POS_TOO_LONG    int(1)
#define BLOCK_TIMEOUT_WARNING     int(2)
#define FILL_TEST_ABORT           int(3)
#define MASTER_ENABLE_DROPPED_OUT int(4)
#define VAC_ABORT_ZSPEED          int(5)
#define TEST_WARNING              int(10)

/*
----------------
Fatal Words V316
---------------- */
#define STACK_OVERFLOW          int(101)
#define BLOCK_TIMEOUT           int(2)
#define INVALID_BLOCK           int(3)
#define BLOCK_EXECUTION_ERROR   int(4)
#define PROGRAM_STOPPED_BY_USER int(5)
#define TEST_FATAL_ERROR        int(10)

/*
------------------
Channel Modes V429
------------------ */
static int CHANNEL_MODE_1 = 1;
static int CHANNEL_MODE_2 = 2;
static int CHANNEL_MODE_3 = 3;
static int CHANNEL_MODE_4 = 4;
static int CHANNEL_MODE_5 = 5;
static int CHANNEL_MODE_6 = 6;
static int CHANNEL_MODE_7 = 7;

/*
---------
Isw4 bits
--------- */
#define MASTER_ENABLE_BIT      unsigned(2)

/*
----------------
Variable Numbers
---------------- */
#define INTEGRAL_GAIN_VN     int(443)
#define CONFIG_WORD_2_VN     int(314)
#define TSL_VN               int(318)
#define MSL_VN               int(319)
#define BISCUIT_DELAY_MS_VN  int(321)
#define ESV_VN               int(322)
#define TIME_INTERVAL_MS_VN  int(328)
#define NOF_TIME_SAMPLES_VN  int(329)
#define CS_TIMEOUT_MS_VN     int(330)
#define VEL_LOOP_GAIN_VN     int(340)
#define BREAK_VEL_1_VN       int(341)
#define BREAK_VEL_2_VN       int(342)
#define BREAK_VEL_3_VN       int(343)
#define BREAK_GAIN_1_VN      int(344)
#define BREAK_GAIN_2_VN      int(345)
#define BREAK_GAIN_3_VN      int(346)
#define ZERO_SPEED_CHECK_VN  int(349)
#define NULL_DAC_OFFSET_VN   int(350)
#define NULL_DAC_2_OFFSET_VN int(404)
#define LVDT_DAC_OFFSET_VN   int(351)
#define LVDT_DAC_2_OFFSET_VN int(405)
#define JOG_SHOT_SETTING_VN  int(352)
#define FT_SETTING_VN        int(353)
#define FT_STOP_POS_VN       int(354)
#define FT_STOP_COMMAND_VN   int(502)
#define RETRACT_SETTING_VN   int(355)
#define LVDT_1_BOARD_LEVEL_OFFSET_VN int(361)
#define LVDT_2_BOARD_LEVEL_OFFSET_VN int(362)

#define OSCIL_TRIGGER_VN     int(390)
#define OSCIL_CHANNEL_VN     int(391)
#define OSCIL_US_VN          int(392)
#define CHANNEL_MODE_VN      int(429)
#define DERIVATIVE_GAIN_VN   int(212)

#define ST_ACCEL_1_VN          int(0)
#define ST_VELOCITY_1_VN       int(1)
#define ST_END_POS_1_VN        int(2)
#define ST_INPUT_T0_TEST_1_VN  int(3)
#define ST_NEXT_STEP_VN_OFFSET int(4)
#define ST_LAST_STEP_VN        int(79)

/*
--------------------------
Pressure Control Variables
-------------------------- */
#define PC_CONFIG_WORD_VN      int(80)
#define PC_PRES_LOOP_GAIN_VN   int(81)
#define PC_ARM_SETTING_VN      int(82)  //No longer used 2/8/08
#define PC_INITIAL_PRES_VN     int(83)  //No longer used 2/8/08
#define PC_RETRACT_VOLTS_VN    int(84)
#define PC_PARK_VOLTS_VN       int(85)
#define PC_INTEGRAL_GAIN_VN    int(87)
#define PC_DERIVATIVE_GAIN_VN  int(88)
#define PC_RAMP_1_VN           int(90)
#define PC_PRES_1_VN           int(91)
#define PC_HOLDTIME_1_VN       int(92)
#define PC_RAMP_2_VN           int(93)
#define PC_PRES_2_VN           int(94)
#define PC_HOLDTIME_2_VN       int(95)
#define PC_RAMP_3_VN           int(96)
#define PC_PRES_3_VN           int(97)
#define PC_HOLDTIME_3_VN       int(98)
#define PC_RETCTL_DAC_VN       int(449)
#define PC_RETCTL_ARM_PRES_VN  int(450)
#define PC_RETCTL_PRES_VN      int(451)

#define LS1_VN                 int(366)
#define LS2_VN                 int(367)
#define LS3_VN                 int(368)
#define LS4_VN                 int(369)
#define LS5_VN                 int(370)
#define LS6_VN                 int(371)

#define LS1_MASK_VN            int(378)
#define LS2_MASK_VN            int(379)
#define LS3_MASK_VN            int(380)
#define LS4_MASK_VN            int(381)
#define LS5_MASK_VN            int(382)
#define LS6_MASK_VN            int(383)
#define VALVE_TYPES_1_VN       int(414)
#define VALVE_TYPES_2_VN       int(415)
#define DEMODULATOR_PHASES_VN  int(430)
#define DIGITAL_POTS_VN        int(431)

#define MinLowImpactPercentKey TEXT("MinLowImpactPercent")
#define LS1DescriptionKey      TEXT("LS1Description")
#define LS2DescriptionKey      TEXT("LS2Description")
#define LS3DescriptionKey      TEXT("LS3Description")
#define LS4DescriptionKey      TEXT("LS4Description")
#define LS5DescriptionKey      TEXT("LS5Description")
#define LS6DescriptionKey      TEXT("LS6Description")
#define MaxVelocityKey         TEXT("MaxVelocity")
#define VacuumLimitSwitchKey   TEXT("VacuumLimitSwitch")

/*
----------
Data types
---------- */
const unsigned char START_OF_BINARY_POS_SAMPLES       = 0;
const unsigned char START_OF_BINARY_TIME_SAMPLES      = 1;
const unsigned char START_OF_BINARY_PARAMETERS        = 2;
const unsigned char START_OF_BINARY_OSCILLOSCOPE_DATA = 3;
const unsigned char START_OF_BINARY_OP_STATUS_DATA    = 4;
const unsigned char START_OF_BINARY_TEXT_DATA         = 5;
const unsigned char START_OF_IO_CHANGE_DATA           = 6;  /* Version 5 */
const unsigned char START_OF_SINGLE_ANALOG_DATA       = 7;  /* Version 5 */
const unsigned char START_OF_BLOCK_ANALOG_DATA        = 8;  /* Version 5 */
const unsigned char CONNECTION_LOST                   = 100;
const unsigned short DONT_ACK_BIT = 1;

struct BINARY_HEADER {
    char           bchar;
    unsigned char  data_type;
    unsigned short flags;
    unsigned short data_set_num;
    unsigned short packet_num;
    unsigned short nof_packets;
    unsigned short nof_bytes;
    };

class FTII_VAR
{
public:
int vn;
unsigned x;
FTII_VAR() { vn = NO_FTII_VAR; x=0; }
FTII_VAR( int new_vn, unsigned new_value ) { vn=new_vn; x = new_value; }
virtual ~FTII_VAR() {}
bool append_value( STRING_CLASS & dest );
bool is_same_variable( const TCHAR * sorc );
bool makeline( STRING_CLASS & dest );
virtual bool get( const TCHAR * sorc );
virtual bool put( STRING_CLASS & dest );
int      ival() { return int(x); }
unsigned uval() { return x; }
double   realval();
virtual void set( int new_vn, unsigned new_value ) { vn = new_vn; x=new_value; }
bool operator==( const FTII_VAR & sorc ) { return (vn == sorc.vn && x == sorc.x); }
virtual void operator=( const TCHAR * sorc );
virtual void operator=( const FTII_VAR & sorc ) { vn = sorc.vn; x = sorc.x; }
virtual void operator=( int new_value ) { x = (unsigned) new_value; }
virtual void operator=( unsigned new_value ) { x = new_value; }
virtual void operator=( double new_value );
};

class FTII_VAR_ARRAY : public FTII_VAR
{
private:

int        maxn;
int        ci;
int        n;
FTII_VAR * p;

public:

FTII_VAR_ARRAY() { maxn=0; n=0; p=0; ci=NO_INDEX; }
~FTII_VAR_ARRAY();
void     cleanup();
int      count() { return n; }
int      current_index() { return ci; }
void     empty() { n = 0; }
bool     find( int vn_to_find );
bool     find( TCHAR * sorc );
bool     get( TCHAR * filename );
bool     get_from( TCHAR * filename );
bool     put( TCHAR * filename );
bool     put_into( TCHAR * filename );
FTII_VAR & operator[]( int i );
void     operator=( TCHAR * sorc );
void     operator=( const FTII_VAR_ARRAY & sorc );
void     operator=( const FTII_VAR & sorc );
void     operator=( int new_value );
void     operator=( unsigned new_value );
void     operator=( double new_value );
void     set( int new_vn, unsigned new_value );
void     rewind() { ci=NO_INDEX; }
bool     prev();
bool     next();
bool     remove();
bool     remove( int i );
bool     upsize( int new_n );
};

class FTII_LIMIT_SWITCH_WIRE_CLASS
{
private:
unsigned wire[MAX_FTII_LIMIT_SWITCHES];
public:
TCHAR *  ascii_mask( int index );
TCHAR *  ascii_wire( int index );
unsigned mask( int index );
unsigned & operator[]( int index );
void     empty();
BOOLEAN  get( TCHAR * computer, TCHAR * machine, TCHAR * part );
BOOLEAN  put( TCHAR * computer, TCHAR * machine, TCHAR * part );
};

#endif
