#define _VISIPARM_H

#ifndef __BORLANDC__
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

typedef unsigned short BITSETYPE;
typedef unsigned short BOARD_DATA;
typedef long int int32;
typedef unsigned long int uint32;

#define WM_DBINIT         WM_USER+0x101
#define WM_DBCLOSE        WM_USER+0x102
#define WM_OVERLAY_CHANGE WM_USER+0x103
#define WM_DB_SAVE_DONE   WM_USER+0x104
#define WM_EV_SHUTDOWN    WM_USER+0x105
#define WM_NEWSETUP       WM_USER+0x106
#define WM_NEWSHOT        WM_USER+0x107
#define WM_NEWPART        WM_USER+0x108
#define WM_POKEMON        WM_USER+0x109
#define WM_STARTMON       WM_USER+0x110    /* Hardware <-> monall */
#define WM_STOPMON        WM_USER+0x111    /* Hardware <-> monall */
#define WM_NEWMAIL        WM_USER+0x112    /* Mailslot --> eventman */
#define WM_STATUS_CHANGE  WM_USER+0x113    /* SureTrak Status */
#define WM_NEW_DATA       WM_USER+0x114    /* Data update */
#define WM_HAVE_FT_VALUE  WM_USER+0x115    /* Monall <-- hardware */
#define WM_NEW_TC_CONFIG  WM_USER+0x116    /* Trend chart config  */
#define WM_NEW_DTR_CONFIG WM_USER+0x117    /* Downtime Barchart Config */
#define WM_F5_KEY         WM_USER+0x118
#define WM_NEW_CL_CONFIG  WM_USER+0x119    /* Trend chart control limit config  */
#define WM_NO_DATA        WM_USER+0x120    /* Demo monitor thread to main thread  */
#define WM_NEW_RT_DATA    WM_USER+0x121    /* Realtime ftii data from socket */
#define WM_NEW_FT2_STATE  WM_USER+0x122    /* Ft2 connect state changed */
#define WM_OPTO_CLICK     WM_USER+0x123    /* Opto control click notify */
#define WM_SAVE_NEXT_PART WM_USER+0x124    /* Save All Parts */

#define BITS_IN_BITSET 16

#define ACQUIRE_COUNT_LEN            6
#define ALPHADATE_LEN                10       /* 12/25/1996 */
#define ALPHATIME_LEN                8        /* 23:54:26   */
#define BACKUP_COMPUTER             TEXT( "BUP" )
#define BITSET_LEN                   5
#define BITSETYPE_LEN                5
#define BOARD_NUMBER_LEN             2
#define BOARD_DATA_HEX_LEN           4
#define CHANNEL_NUMBER_LEN           2
#define COLOR_NAME_LEN               12
#define COLORREF_LEN                 8
#define COMPUTER_NAME_LEN            3
#define COMPUTER_LIST_PASSWORD_LEVEL 10
#define DECIMAL_RADIX                10
#define DATE_FIELD_LEN               11      /* Time offset = date_offset + DATE_FIELD_LEN */
#define DOW_NAME_LEN                 9                     /*Wednesday*/
#define DOWNCAT_SYSTEM_CAT        TEXT( "00" )
#define DOWNCAT_UP_SUBCAT         TEXT( "11" )
#define DOWNCAT_PROG_EXIT_SUBCAT  TEXT( "00" )
#define DOWNCAT_PROG_START_SUBCAT TEXT( "01" )
#define DOWNCAT_NO_CONNECT_SUBCAT TEXT( "02" )             /* Ft2 */
#define DOWNCAT_UNSP_DOWN_SUBCAT  TEXT( "XX" )
#define DOWNCAT_NUMBER_LEN        2                        /*Ascii*/
#define DOWNCAT_NAME_LEN          15
#define DESC_LEN                  25
#define DEFAULT_ANALOG_SENSOR     23
#define DEFAULT_FT2_ANALOG_SENSOR 24
#define PLUS_MINUS_10_VOLT_SENSOR 25
#define DEFAULT_PART_NAME         TEXT( "DEFAULT" )
#define DEFAULT_PASSWORD_NAME     TEXT( "DEFAULT" )
#define DEFAULT_WS0_NAME          TEXT( "WS0" )
#define CURRENT_MASTER_TRACE_NAME TEXT( "CURRENT" )
#define DIFF_CURVE_NUMBER_LEN     3
#define DECIMAL_FT_ADDRESS_LEN    5
#define EDIT_OTHER_COMPUTER_LEVEL 6
#define EDIT_OTHER_CURRENT_PART_LEVEL 10
#define EDIT_OTHER_CURRENT_SURETRAK_LEVEL 50
#define ENGINEER_PASSWORD_LEVEL   5
#define FT_8_CHANEL_VERSION       26
#define FT_ADDRESS_INCREMENT      0x20
#define NOF_FTII_ANALOGS          8       /* Number of continuous analog channels on ftii board */
#define NOF_EXTENDED_ANALOGS      8       /* Number of extended analog channels on ftii board */
#define HEX_FT_ADDRESS_LEN        3
#define HEX_RADIX                 16
#define HIGH_FT_INPUT_WIRE        32      /* Digital inputs from opto */
#define HIGH_FT2_OUTPUT_WIRE      32      /* Digital outputs from opto */
#define HIGH_MONITOR_WIRE         8
#define HIGH_ST_INPUT_WIRE        32      /* Digital inputs from opto */
#define HIGH_WIRE                 16
#define IMPACT_POINTS_LEN         4
#define INPUT_NUMBER_LEN          3
#define LOWEST_PASSWORD_LEVEL   0
#define LOW_FT_INPUT_WIRE       17       /* Digital inputs from opto */
#define LOW_METAL_PRESSURE_DIFF_CURVE 50
#define LOW_MONITOR_WIRE        6
#define LOW_ST_INPUT_WIRE       17       /* Digital inputs from opto */
#define LOW_WIRE                1
#define MARK_LABEL_LEN          5
#define MACHINE_NAME_LEN        3
#define MAILSLOT_ID_LEN         (COMPUTER_NAME_LEN + 4)
#define MASTER_TRACE_NAME_LEN   8
#define MASTER_TRACE_SUFFIX     TEXT( ".MAS" )
#define MASTER_TRACE_II_SUFFIX  TEXT( ".MA2" )
#define MAX_SAVED_LEN           7
#define MHZ_LEN                 2
#define MINUTES_PER_DAY         1440
#define MS_PER_SECOND           1000
#define NOF_BREAK_GAIN_SETTINGS 3
#define NOF_VEL_GAIN_SETTINGS   3
#define OPERATOR_FIRST_NAME_LEN 15
#define OPERATOR_LAST_NAME_LEN  15
#define OPERATOR_NUMBER_LEN     8
#define PARAMETER_NAME_LEN      25
#define PARAMETER_NUMBER_LEN    3
#define PART_NAME_LEN           8
#define PASSWORD_LEN            20
#define PASSWORD_LEVEL_LEN      3
#define POST_IMPACT_SECONDS_LEN 5
#define PROFILE_NAME_SUFFIX     TEXT(".PRO")
#define PROFILE_II_SUFFIX       TEXT(".PR2")
#define PROFILE_BOTH_SUFFIXES   TEXT(".PR?")
#define RODPITCH_NAME_LEN       3
#define SEC_PER_MINUTE          60L
#define SEC_PER_HOUR            3600L
#define SEC_PER_DAY             86400L
#define SEC_PER_WEEK            604800L
#define SENSOR_NUMBER_LEN       3
#define SHIFT_LEN               2
#define SHOT_LEN                6
#define SINGLE_MACHINE_MUX_CHAN 1
#define SKIP_COUNT_LEN          6
#define SKIP_TIME_LEN           4
#define SYSTEM_PASSWORD_LEVEL   9
#define TCP_ADDRESS_LEN         15 /*"192.168.254.040"*/
#define TCP_PORT_LEN            5  /* ["0","65535"] */
#define TIMEOUT_SEC_LEN         4
#define ST_STEP_NUMBER_LEN      3
#define SURETRAK_ADDRESS        0x1C0
#define UNITS_ID_LEN            2
#define UNITS_LEN               7
#define VARTYPE_NAME_LEN        25
#define X4_COUNTS_PER_MARK      4
#define WIRE_LEN                2

#define MIN_FT_ADDRESS          0x220
#define MIN_FT_NUMBER           1
#define MIN_FT_CHANNEL          1
#define MIN_FREE_DISK_KB        3000

#define MAX_ANALOG_SENSORS      30
#define MAX_BOARDS              8
#define MAX_DOUBLE_LEN          40
#define MAX_FT_ADDRESS          0x340
#define MAX_FT_CHANNELS         8
#define MAX_FT2_CHANNELS        16
#define MAX_FT2_POINTS          14000
#define MAX_FTII_LIMIT_SWITCHES 6
#define MAX_CONTINUOUS_CHANNELS 4
#define MAX_CURVES              8
#define MAX_EQUATIONS           10
#define MAX_HEX_LEN              8
#define MAX_FLOAT_LEN           30
#define MAX_INI_STRING_LEN      60
#define MAX_LONG_INI_STRING_LEN 255
#define MAX_INTERNAL_PARMS      37
#define MAX_INTEGER_LEN         18
#define MAX_MULTIPART_RUNLIST_WIRES 3
#define MAX_MUX_CHANNELS        8
#define MAX_NAME_LEN            8    /*Max of co, ma, pa */
#define MAX_NUMBER_LEN          30   /* Max of hex, integer, float */
#define MAX_OPERATOR_NAME_LEN   15   /* Max of first, last, no. */
#define MAX_OVERLAY_SHOTS       10
#define MAX_PARMS               60   /* 37 Internal, 24 Other */
#define MAX_POINTS              5000
#define MAX_POLY_COEF           10
#define MAX_POSITION_SAMPLES    3000
#define MAX_STRING_LEN          120   /* String Table Entries */
#define MAX_ST_CHANNELS         4
#define MAX_ST_LIMIT_SWITCHES   4
#define MAX_TIME_SAMPLES        2000
#define MAX_YES_LEN             3
#define MIN_MS_PER_TIME_SAMPLE  1.0

#define USER_TIME_INT_COUNT      5
#define LIMIT_SWITCH_COUNT       4
#define USER_AVG_VEL_COUNT       4
#define USER_VEL_COUNT           4
#define FT_ONOFF_COUNT           4

#define NOT_A_NUMBER             TEXT( "NAN" )
#define NO_ANALOG_SENSOR_NUMBER  23
#define NO_BOARD_NUMBER          0
#define NO_COMPUTER             TEXT( "NOC" )
#define NO_CURVE_TYPE            0
#define NO_DIFF_CURVE_NUMBER     0
#define NO_DOWNCAT_NAME         TEXT( "UNKNOWN" )
#define NO_DOWNCAT_NUMBER       TEXT( "-1" )
#define NO_DOWN_TIMEOUT          0
#define NO_EQUATION_NUMBER       0
#define NO_FT_CHANNEL            0
#define NO_INDEX                -1
#define NO_MUX_CHANNEL           0
#define NO_OPERATOR_NUMBER      TEXT( "UNKNOWN0" )
#define NO_PARAMETER_INDEX      -1
#define NO_PARAMETER_NUMBER      0
#define NO_PARAMETER_TYPE        0
#define NO_PART                 TEXT( "NOPART" )
#define NO_PASSWORD_LEVEL       -1
#define NO_MACHINE              TEXT( "NOM" )
#define NO_MACHINE_TYPE          0
#define NO_RODPITCH             TEXT( "NOR" )
#define NO_SHIFT_NUMBER          -1
#define NO_SHOT_NUMBER           0
#define NO_STEP_NUMBER           0     /* Suretrak steps */
#define NO_UNITS                 0
#define NO_VARIABLE_TYPE         0
#define NO_WIRE                  0
#define UNKNOWN                 TEXT( "UNKNOWN" )

#define DEF_AUTO_SHOT_TIMEOUT     30
#define DEF_FT_MHZ                12
#define DEF_FT_ASENSOR            0
#define DEF_NOF_CAVITIES          1
#define DEF_NOF_TIME_SAMPLES      500
#define DEF_MS_PER_TIME_SAMPLE    1
#define DEF_CYCLES_PER_TIME_COUNT 4
#define DEF_COUNTS_PER_POS_MARK   4
#define DEF_SURETRAK_MHZ          12

// #define CYCLE_TIME_INDEX        5
#define V4_FIRMWARE_VERSION     19

#define RESULTS_DIRECTORY_NAME TEXT( "RESULTS" )
#define TAB_CHAR                9

#define MONITOR_CLASS_NAME   TEXT("MonAll")
#define MONITOR_WINDOW_TITLE TEXT("Board Monitor")

/*
-----------------
Alarm Limit Types
----------------- */
#define ALARM_MIN   0
#define ALARM_MAX   1
#define WARNING_MIN 2
#define WARNING_MAX 3
#define NOF_ALARM_LIMIT_TYPES 4

/*
----------------------
Parameter alarm states
---------------------- */
#define NO_ALARM_STATE   0
#define ALARM_STATE      1
#define WARNING_STATE    2

/*
--------------------
Digital Output Types
-------------------- */
#define ALARM_OUTPUT_TYPE        1
#define LIMIT_SWITCH_OUTPUT_TYPE 2
#define GOOD_SHOT_OUTPUT_TYPE    3

/*
-----------------
Skip return codes
----------------- */
#define SAVE_PARM_DATA    1
#define SAVE_PROFILE_DATA 2

/*
---------------
Parameter Types
--------------- */
#define INTERNAL_PARAMETER 1
#define FT_ANALOG_INPUT    2
#define FT_ON_OFF_INPUT    3
#define FT_TARGET_INPUT    4
#define FT_DIGITAL_INPUT   5
#define FT_PLC_INPUT       6
#define FT_EXTENDED_ANALOG_INPUT 7
#define FT2_ON_OFF_INPUT   8

/*
--------------
Variable Types
-------------- */
#define DISTANCE_VAR         1
#define POSITION_VAR         2
#define PRESSURE_VAR         4
#define TEMP_VAR             8
#define TIME_VAR            16
#define TIME_INTERVAL_VAR   32
#define VELOCITY_VAR        64
#define VOLUME_VAR         128
#define AREA_VAR           256
#define WORK_VAR           512
#define WEIGHT_VAR        1024
#define DENSITY_VAR       2048
#define DIGITAL_INPUT_VAR 4096
#define NOF_VARIABLE_TYPES  13

/*
-----------------------------------------------------------------------------------------------
Ft Analog Target Variable Types These cannot = POSITION_VAR or TIME_VAR. These are also 
used for ft_extended_analogs where TRIGGER_WHEN_GREATER means trigger when the input goes high. 
The same is true for the FT_ON_OFF_INPUT where TRIGGER_WHEN_GREATER means trigger when the wire
goes from 0 to 1.
----------------------------------------------------------------------------------------------- */
#define TRIGGER_WHEN_GREATER 1024
#define TRIGGER_WHEN_SMALLER 2048

/*
---------------------
Machine monitor flags
--------------------- */
#define MA_NO_MONITORING        0
#define MA_MONITORING_ON        1
#define MA_ALARM_MONITORING_ON  2
#define MA_HAS_DIGITAL_SERVO    4
#define MA_HAS_PRESSURE_CONTROL 8
#define MA_USE_TIME_TO_MEASURE_BISCUIT 16  /* Replace biscuit time delay with time to measure biscuit */

/*
--------------
Machine states
-------------- */
#define MA_NO_STATE          0
#define MA_IDLE_STATE        1
#define MA_TIMED_OUT_STATE   2
#define MA_AUTO_SHOT_STATE   4
#define MA_FT_TIME_OUT_STATE 8

/*
-----
Units
----- */
#define INCH_UNITS             1
#define CM_UNITS               2
#define FEET_UNITS             3
#define SECOND_UNITS           4
#define IPS_UNITS              5
#define FPM_UNITS              6
#define CMPS_UNITS             7
#define PSI_UNITS              8
#define BAR_UNITS              9
#define MM_UNITS               10
#define MPS_UNITS              11
#define DEG_F_UNITS            12
#define DEG_C_UNITS            13
#define VOLTS_UNITS            14
#define KG_PER_CM2_UNITS       15
#define MS_UNITS               16
#define IN_LB_UNITS            17
#define KJ_UNITS               18
#define LBS_UNITS              19
#define KG_UNITS               20
#define IN_CUBED_UNITS         21
#define CM_CUBED_UNITS         22
#define MM_CUBED_UNITS         23
#define IN_SQUARED_UNITS       24
#define CM_SQUARED_UNITS       25
#define MM_SQUARED_UNITS       26
#define LBS_PER_IN_CUBED_UNITS 27
#define G_PER_CM_CUBED_UNITS   28
#define INCHES_OF_HG_UNITS     29
#define DIGITAL_INPUT_UNITS    30
#define MEGAPA_UNITS           31
#define MILLIBAR_UNITS         32

/*
-----------
Board Types
----------- */
#define SINGLE_MACHINE_BOARD_TYPE   1
#define MUX_BOARD_TYPE              2
#define SURETRAK_BOARD_TYPE         4
#define FTII_BOARD_TYPE             8

/*
------------
MachineTypes
------------ */
#define COLD_CHAMBER_TYPE    1
#define HOT_CHAMBER_TYPE     2

/*
-----------
Curve Types
----------- */
#define TIME_CURVE          BITSETYPE( 1 )
#define POSITION_CURVE      BITSETYPE( 2 )
#define VELOCITY_CURVE      BITSETYPE( 4 )
#define ROD_PRESSURE_CURVE  BITSETYPE( 8 )
#define HEAD_PRESSURE_CURVE BITSETYPE( 16 )
#define CHAN_1_ANALOG_CURVE BITSETYPE( 32 )
#define CHAN_2_ANALOG_CURVE BITSETYPE( 64 )
#define CHAN_3_ANALOG_CURVE BITSETYPE( 128 )
#define CHAN_4_ANALOG_CURVE BITSETYPE( 256 )
#define DIFF_PRESSURE_CURVE BITSETYPE( 512 )
#define CHAN_5_ANALOG_CURVE BITSETYPE( 1024 )
#define CHAN_6_ANALOG_CURVE BITSETYPE( 2048 )
#define CHAN_7_ANALOG_CURVE BITSETYPE( 4096 )
#define CHAN_8_ANALOG_CURVE BITSETYPE( 8192 )

/*
-------------
Curve Indexes
------------- */
#define POSITION_CURVE_INDEX      0
#define TIME_CURVE_INDEX          1
#define VELOCITY_CURVE_INDEX      2

#define CHAN_1_ANALOG_CURVE_INDEX 3
#define CHAN_2_ANALOG_CURVE_INDEX 4
#define CHAN_3_ANALOG_CURVE_INDEX 5
#define CHAN_4_ANALOG_CURVE_INDEX 6
#define DIFFERENTIAL_CURVE_INDEX  7

/*
------------
Shot Regions
------------ */
#define PRE_IMPACT_REGION   1
#define POST_IMPACT_REGION  2

/*
-------------------------------
Parm/profile configuration bits
------------------------------- */
#define SKIP_COUNT_BIT   1
#define SKIP_TIME_BIT    2
#define ACQUIRE_ALL_BIT  4
#define ACQUIRE_NONE_BIT 8


#define FT_BOARD_TYPE            16
#define ASCII_FT_BOARD_TYPE     'F'

/*
-------------------
Ftanalog base types
------------------- */
#define ASCII_TIME_IND_VAR       'T'
#define ASCII_POS_IND_VAR        'P'
#define ASCII_GREATER_THAN_VAR   'G'
#define ASCII_SMALLER_THAN_VAR   'S'

/*
--------------------------
Internal Parameter indexes
-------------------------- */
#define FILL_TIME                0
#define TOTAL_SHOT_TIME          1
#define SLOW_SHOT_TIME           2
#define FAST_SHOT_TIME           3
#define RISE_TIME                4
#define CYCLE_TIME_PARM          5
#define USER_TIME_INT_ONE        6
#define BISCUIT_SIZE            11
#define CSFS                    12
#define INTENS_PRESSURE         13
#define INTENS_RESPONSE_TIME    14
#define EFF_PEAK_INTENS         15
#define IMPACT_PRES_TO_PEAK     16
#define INTENS_SQUEEZE_DIST     17
#define SLEEVE_FULL_VELOCITY    18
#define RUNNER_FULL_VELOCITY    19
#define AVG_FILL_VELOCITY       20
#define USER_VEL_1              21
#define USER_AVG_VEL_1          25
#define P1_POS_PARM             29
#define P2_POS_PARM             30
#define P3_POS_PARM             31
#define AVG_SLOW_SHOT_VELOCITY  32
#define AVG_FAST_SHOT_VELOCITY  33
#define FAST_SHOT_DISTANCE      34
#define AVG_GATE_VELOCITY       35
#define PRE_IMPACT_WORK         36

#define STATIC_ANALOG_PARAMETER 37   /* Change this if defined */

/*
--------------
SureTrak files
-------------- */
#define STPARAM_FILE              TEXT( "STPARAM.DAT" )
#define VTMASTER_FILE             TEXT( "VALRESP.SAV" )
#define CONTROL_PROGRAM_FILE      TEXT( "CONTROL.PRG" )
#define TEMP_CONTROL_PROGRAM_FILE TEXT( "CONTROL.TMP" )
#define CONTROL_MESSAGE_FILE      TEXT( "CONTROL.MSG" )
#define MESSAGE_FILE_SUFFIX       TEXT( "MSG" )

/*
---------------
Misc file names
--------------- */
#define AUTO_SAVE_ALL_FILE TEXT( "need_auto_save_all.txt" )
#define BACKUP_LIST_SLEN  (COMPUTER_NAME_LEN+1+MACHINE_NAME_LEN+1+PART_NAME_LEN+1+SHOT_LEN)
#define BAD_PART_SETTINGS_FILE TEXT( "ftii_part_setting.txt" )
#define CALC_FILENAME TEXT( "CALC.DAT" )
#define CONFIG_INI_FILE TEXT( "config.ini" )
#define EOS_UDP_ADDRESS_FILE TEXT( "eos_udp_address.txt" )
#define FTII_CONTROL_PROGRAM_FILE TEXT( "control.txt" )
#define FTII_CTRL_PROG_STEPS_FILE TEXT( "ftii_ctrl_prog_steps.txt" )
#define FTII_EDITOR_SETTINGS_FILE TEXT( "ftii_editor_settings.ini" )
#define FTII_GLOBAL_SETTINGS_FILE TEXT( "ftii_global_settings.txt" )
#define FTII_LIMIT_SWITCH_WIRES_FILE TEXT( "ftii_limit_switch_wires.dat" )
#define FTII_MACHINE_SETTINGS_FILE TEXT( "ftii_machine_settings.txt" )
#define FTII_PART_SETTINGS_FILE TEXT( "ftii_part_settings.txt" )
#define GOOD_SHOT_SETUP_FILE TEXT( "good_shot_setup.csv" )
#define MONALL_INI_FILE TEXT( "MONALL.INI" )
#define PARAMETER_SORT_ORDER_FILE TEXT( "parameter_sort_order.csv" )
#define PART_DISPLAY_INI_FILE TEXT( "PARTDISPLAY.INI" )
#define SETPOINT_CSV_FILE TEXT( "setpoint.csv" )
#define SMOOTH_FILE TEXT( "SMOOTH.DAT" )
#define STATUS_SCREEN_CONFIG_FILE_NAME TEXT( "status_screen_config.dat" )
#define TC_PARAMETERS_FILENAME TEXT( "TCPARAMS.DAT" )

/*
---------------------
Setpoint descriptions
--------------------- */
#define GOOD_SHOT_WIRE_DESC TEXT( "good shot wire number" )
#define GOOD_SHOT_IF_NO_WARNINGS_DESC TEXT( "good shot only if no warnings" )

/*
------------------------
Root directory databases
------------------------ */

/*        10        20        30        40        50        60        70        80        90        100       110       120       130       140
0123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_1
|   |                           |            |            |     |  |   |            |            |            |            |            |    |
123;"DescriptionDescriptionDes";123456789.12;123456789.12;12345;01;123;123456789.00;123456789.00;123456789.00;123456789.00;123456789.00;123  */
#define ASENSOR_DB                          TEXT( "ASENSOR.TXT" )
#define ASENSOR_DESC_LEN                     25
#define ASENSOR_NUMBER_LEN                   3
#define ASENSOR_POLY_EQ_LEN                  3
#define ASENSOR_GAIN_LEN                     3
#define ASENSOR_FLOAT_LEN                    12

#define ASENSOR_NUMBER_OFFSET                0
#define ASENSOR_DESC_OFFSET                  4
#define ASENSOR_HIGH_VALUE_OFFSET            32
#define ASENSOR_LOW_VALUE_OFFSET             45
#define ASENSOR_VARIABLE_TYPE_OFFSET         58
#define ASENSOR_UNITS_OFFSET                 64
#define ASENSOR_GAIN_OFFSET                  67
#define ASENSOR_MUL_OFFSET                   71
#define ASENSOR_SUB_OFFSET                   84
#define ASENSOR_DIV_OFFSET                   97
#define ASENSOR_ADD_OFFSET                   110
#define ASENSOR_MILLI_VOLT_OFFSET            123
#define ASENSOR_POLY_EQ_OFFSET               136
#define ASENSOR_RECLEN                       141

/*        10        20
0123456789_123456789_12345678
|              |   |   |    |
"123456789_12";123;123;123  */
#define COLOR_DB                           TEXT( "COLOR.TXT" )
#define COLOR_NAME_OFFSET                    0
#define COLOR_RED_OFFSET                     15
#define COLOR_GREEN_OFFSET                   19
#define COLOR_BLUE_OFFSET                    23
#define COLOR_RECLEN                         28

/*        10        20        30
0123456789_123456789_123456789_12345
|     |                            |
"C01";"123456789_123456789_12345"  */
#define COMPUTER_DB                         TEXT( "COMPUTER.TXT" )
#define TEMP_COMPUTER_DB                    TEXT( "TEMPCPUT.TXT" )
#define OLD_COMPUTER_DIR_LEN                 25
#define COMPUTER_NAME_OFFSET                 0
#define COMPUTER_DIR_OFFSET                  6
#define COMPUTER_RECLEN                      35
/*
Csv, computer name, path to root data directory
C01,\\W01\V5\DATA\C01\  */
#define COMPUTER_CSV                        TEXT( "COMPUTER.CSV" )
#define COMPUTER_CSV_MAXLEN                 (COMPUTER_NAME_LEN + 1 + MAX_PATH)

#define DCURVE_DB                           TEXT( "DCURVE.TXT" )
#define DCURVE_DESC_LEN                      25
#define DCURVE_NUMBER_LEN                    3
#define DCURVE_COEF_LEN                     12

/*        10        20        30        40        50
0123456789_123456789_123456789_123456789_123456789_123456789
|   |                           |            |             |
123;"123456789_123456789_12345";123456789.12;123456789.12  */
#define DCURVE_CURVE_NUMBER_OFFSET           0
#define DCURVE_DESC_OFFSET                   4
#define DCURVE_HEAD_PRES_OFFSET              32
#define DCURVE_ROD_PRES_OFFSET               45
#define DCURVE_RECLEN                        59

/*        10        20        30        40        50
0123456789_123456789_123456789_123456789_123456789_12
|   |                           |  |       |        |
  1;"Aluminium 380            ";22;0.09990;0.09040  */
#define DENSITY_DB                          TEXT( "DENSITY.TXT" )
#define DENSITY_FLOAT_LEN                   7
#define DENSITY_NAME_LEN                    25
#define DENSITY_NUMBER_LEN                  3

#define DENSITY_NUMBER_OFFSET               0
#define DENSITY_NAME_OFFSET                 4
#define DENSITY_UNITS_OFFSET                32
#define DENSITY_SOLID_OFFSET                35
#define DENSITY_LIQUID_OFFSET               43
#define DENSITY_RECLEN                      52

/*        10        20
0123456789_123456789_1234
|    |                  |
"12";"DieCast Machine"  */
#define DOWNCAT_DB                          TEXT( "DOWNCAT.TXT" )
#define DOWNCAT_NUMBER_OFFSET               0
#define DOWNCAT_NAME_OFFSET                 5
#define DOWNCAT_RECLEN                      24

/*        10        20
0123456789_123456789_123456789
|    |    |                  |
"01";"12";"DieCast Machine"  */
#define DOWNSCAT_DB                         TEXT( "DOWNSCAT.TXT" )
#define DOWNSCAT_CAT_NUMBER_OFFSET           0
#define DOWNSCAT_SUB_NUMBER_OFFSET           5
#define DOWNSCAT_SUB_NAME_OFFSET            10
#define DOWNSCAT_RECLEN                     29

/*        10        20        30
0123456789_123456789_123456789_12345678     Note: the first three fields are the same length as DOWNSCAT.
|    |    |                 |bbggrr   |
"01";"12";"DieCast Machine";"FF002F"  */
#define DOWNSCATC_DB                         TEXT( "Downscatc.txt" )
#define DOWNSCATC_COLOR_LEN                  6
#define DOWNSCATC_COLOR_OFFSET              28
#define DOWNSCATC_RECLEN                    38

/*        10        20        30
0123456789_123456789_123456789_1
|   |            |             |
"Y";123456789_12;123456789_12  */
#define FIXPRES_DB                           TEXT( "FIXPRES.TXT" )
#define FIXCHAN_DB                           TEXT( "FIXCHAN.TXT" )
#define FIXAXIS_FLOAT_LEN                   12

#define FIXAXIS_ENABLED_OFFSET               0
#define FIXAXIS_MIN_OFFSET                   4
#define FIXAXIS_MAX_OFFSET                  17
#define FIXAXIS_RECLEN                      31

/*        10        20        30        40
0123456789_123456789_123456789_123456789_12345678
|          |                 |                  |
"12345678";"Alexander      ";"Allezielaskiewi"  */

#define OPERATOR_DB                         TEXT( "OPERATOR.TXT" )
#define OPERATOR_NUMBER_OFFSET               0
#define OPERATOR_FIRST_NAME_OFFSET           11
#define OPERATOR_LAST_NAME_OFFSET            29
#define OPERATOR_RECLEN                      48

/*        10        20        30        40
0123456789_123456789_123456789_123456789_12345
|   |     |                           |      |
123;12345;"123456789_123456789_12345";12345  */
#define PARMNAME_DB                         TEXT( "PARMNAME.TXT" )
#define PARMNAME_NUMBER_OFFSET               0
#define PARMNAME_MACH_TYPE_OFFSET            4
#define PARMNAME_NAME_OFFSET                 10
#define PARMNAME_TYPE_OFFSET                 38
#define PARMNAME_RECLEN                      45

/*        10        20
0123456789_123456789_12345678
|   |                       |
123;"0123456789_123456789"  */
#define PASSWORD_DB                          TEXT( "PASSWORD.TXT" )
#define PASSWORD_LEVEL_OFFSET                0
#define PASSWORD_OFFSET                      4
#define PASSWORD_RECLEN                      28

/*        10        20        30        40        50       60
0123456789_123456789_123456789_123456789_123456789_123456789_1
|   |                      |                                 |
123;"0123456789_123456789";"0123456789_123456789_123456789"  */
#define PASSWORD_OWNER_LEN                   30
#define NEW_PASSWORD_DB                      TEXT( "ADD.TXT" )
#define PASSWORD_OWNER_OFFSET                27
#define NEW_PASSWORD_RECLEN                  61

/*
          10        20        30        40        50        60        70        80        90        100       110       120       130       140       150       160
0123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123
|   |                           |            |            |            |            |            |            |            |            |            |             |
001;"Polynomial Equation #2345";0.0000000000;0.0000000000;0.0000000000;0.0000000000;0.0000000000;0.0000000000;0.0000000000;0.0000000000;0.0000000000;0.0000000000  */
#define POLYEQ_DB                           TEXT( "POLYEQ.TXT" )

#define POLYEQ_COEF_LEN                      12
#define POLYEQ_DESC_LEN                      25
#define POLYEQ_NUMBER_LEN                    3

#define POLYEQ_NUMBER_OFFSET                 0
#define POLYEQ_DESC_OFFSET                   4
#define POLYEQ_COEF_0_OFFSET                 32
#define POLYEQ_COEF_1_OFFSET                 45
#define POLYEQ_COEF_2_OFFSET                 58
#define POLYEQ_COEF_3_OFFSET                 71
#define POLYEQ_COEF_4_OFFSET                 84
#define POLYEQ_COEF_5_OFFSET                 97
#define POLYEQ_COEF_6_OFFSET                 110
#define POLYEQ_COEF_7_OFFSET                 123
#define POLYEQ_COEF_8_OFFSET                 136
#define POLYEQ_COEF_9_OFFSET                 149
#define POLYEQ_RECLEN                        163

/*        10        20        30        40        50        60        70        80        90
0123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_1234
|     |  |  |   |            |        |        |        |        |                            |
"20P";01;02;123;123456789.12;1234567.;1234567.;1234567.;12345678;"123456789_123456789_12345"  */
#define PSENSOR_DB                          TEXT( "PSENSOR.TXT" )
#define PSENSOR_MHZ_LEN                      3
#define PSENSOR_VEL_FACTOR_LEN               12
#define PSENSOR_FLOAT_LEN                    8   /* All except vel_factor */

#define PSENSOR_RODPITCH_OFFSET              0
#define PSENSOR_DIST_UNITS_OFFSET            6
#define PSENSOR_VEL_UNITS_OFFSET             9
#define PSENSOR_MHZ_OFFSET                   12
#define PSENSOR_VEL_FACTOR_OFFSET            16
#define PSENSOR_X1_POS_DIVISOR_OFFSET        29
#define PSENSOR_X4_POS_DIVISOR_OFFSET        38
#define PSENSOR_MIN_VEL_OFFSET               47
#define PSENSOR_DIST_PER_MARK_OFFSET         56
#define PSENSOR_DESC_OFFSET                  65
#define PSENSOR_RECLEN                       94

#define RODPITCH_DB                         TEXT( "RODPITCH.TXT" )

#define RODPITCH_NAME_OFFSET                 0
#define RODPITCH_RECLEN                      7

#define SCRAPCAT_DB                         TEXT( "SCRAPCAT.TXT" )

/*        10        20        30
0123456789_123456789_1234567890123
|    |    |          |         |
"01";"12";"23:59:00";"23:59:00"  */
#define STDDOWN_DB                         TEXT( "STDDOWN.TXT" )
#define STDDOWN_CAT_NUMBER_OFFSET           0
#define STDDOWN_SUB_NUMBER_OFFSET           5
#define STDDOWN_START_TIME_OFFSET          10
#define STDDOWN_END_TIME_OFFSET            21
#define STDDOWN_RECLEN                     33

/*        10        20        30
0123456789_123456789_123456789_1234567
|           |  |           |         |
"Wednesday";01;"Wednesday";12:00:00  */
#define SHIFT_DB                            TEXT( "SHIFT.TXT" )

#define SHIFT_DAY_OFFSET                     0
#define SHIFT_NUMBER_OFFSET                 12
#define SHIFT_ACTUAL_DAY_OFFSET             15
#define SHIFT_START_TIME_OFFSET             27
#define SHIFT_RECLEN                        37

#define SOFTVERS_DB                         TEXT( "SOFTVERS.TXT" )
#define SOFTVERS_VERSION_OFFSET             0
#define SOFTVERS_RECLEN                     10

/*        10        20        30
0123456789_123456789_123456789_123
|  |     |         |             |
 1;    1;"IN     ";123456789.12  */
#define UNITS_DB                            TEXT( "UNITS.TXT" )
#define UNITS_FLOAT_LEN                      12

#define UNITS_ID_OFFSET                      0
#define UNITS_VARIABLE_TYPE_OFFSET           3
#define UNITS_NAME_OFFSET                    9
#define UNITS_SEC_PER_VEL_OFFSET             19
#define UNITS_RECLEN                         33

/*        10        20        30
0123456789_123456789_123456789_12345
|     |                            |
    1;"DISTANCE_VAR             "  */
#define VARTYPE_DB                          TEXT( "VARTYPE.TXT" )
#define VARTYPE_VALUE_OFFSET                 0
#define VARTYPE_NAME_OFFSET                  6
#define VARTYPE_RECLEN                       35

/*        10        20
0123456789_123456789_
|  |  |             |
 1; 1;123456789.12  */
#define VELCVT_DB                           TEXT(  "VELCVT.TXT" )
#define VELCVT_DIST_ID_OFFSET                 0
#define VELCVT_VELOCITY_ID_OFFSET             3
#define VELCVT_MULTIPLIER_OFFSET              6    /*  Convert dist/sec to vel */
#define VELCVT_RECLEN                        20

/*
----------------------------
Computer Directory Databases
----------------------------
          10        20
0123456789_123456789_12345678
|  |     |     |   |   |    |
 1;  544;    1; 12;  4;  4  */

#define BOARDS_DB                           TEXT( "BOARDS.TXT" )
#define BOARDS_DB_MHZ_LEN                    3
#define BOARDS_DB_CYCLES_LEN                 3
#define BOARDS_DB_COUNTS_LEN                 3

#define BOARDS_NUMBER_OFFSET                 0
#define BOARDS_ADDRESS_OFFSET                3
#define BOARDS_TYPE_OFFSET                   9
#define BOARDS_MHZ_OFFSET                    15
#define BOARDS_CYCLES_PER_TIME_COUNT_OFFSET  19
#define BOARDS_COUNTS_PER_POS_MARK_OFFSET    23
#define BOARDS_RECLEN                        28


/*        10        20        30
0123456789_123456789_123456789_
|  |                 |
 1;"192.168.254.40 ";"20000"  */
#define FTII_BOARDS_DB                       TEXT( "Ftii_boards.txt" )
#define FTII_BOARDS_NUMBER_OFFSET             0
#define FTII_BOARDS_ADDRESS_OFFSET            3
#define FTII_BOARDS_PORT_OFFSET              21
#define FTII_BOARDS_RECLEN                   30

/*        10        20        30        40        50        60        70        80        90        100       110       120       130       140       150       160
0123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_1
|     |     |  |  |   |          |     |      |      |    |     |       |      |      |    |     |       |     |    |    |   |    |    |     |     |           |
"M02";    2; 1; 1;"N";"VISITRAK";"10P";    10;     1; 600;    2;    100;123456;     1;  10;    1;    100;    1;  30;   0;  0;  10;  10;    1;    2;"OPER0003"  */

#define MACHSET_DB                          TEXT(  "MACHSET.TXT" )

/*
-----------------------------
Offsets from skip count field
----------------------------- */
#define ACQUIRE_COUNT_OFFSET  7
#define TIME_SKIP_OFFSET     14
#define FLAGS_OFFSET         19
#define MAX_SAVED_OFFSET     25

#define MACHSET_MACHINE_NAME_OFFSET          0
#define MACHSET_MACHINE_TYPE_OFFSET          6
#define MACHSET_FT_BOARD_NO_OFFSET           12
#define MACHSET_MUX_CHAN_OFFSET              15
#define MACHSET_ST_BOARD_OFFSET              18
#define MACHSET_CURRENT_PART_OFFSET          22
#define MACHSET_RODPITCH_OFFSET              33
#define MACHSET_PROFILE_SKIP_COUNT_OFFSET    39
#define MACHSET_PROFILE_ACQUIRE_COUNT_OFFSET 46
#define MACHSET_PROFILE_TIME_SKIP_OFFSET     53
#define MACHSET_PROFILE_FLAGS_OFFSET         58
#define MACHSET_PROFILE_MAX_SAVED_OFFSET     64
#define MACHSET_PARAM_SKIP_COUNT_OFFSET      72
#define MACHSET_PARAM_ACQUIRE_COUNT_OFFSET   79
#define MACHSET_PARAM_TIME_SKIP_OFFSET       86
#define MACHSET_PARAM_FLAGS_OFFSET           91
#define MACHSET_PARAM_MAX_SAVED_OFFSET       97
#define MACHSET_MONITOR_FLAGS_OFFSET         105
#define MACHSET_AUTO_SHOT_TIMEOUT_OFFSET     111
#define MACHSET_CYCLE_TIMEOUT_OFFSET         116
#define MACHSET_DIFF_CURVE_NUMBER_OFFSET     121
#define MACHSET_POSITION_IMPACT_PTS_OFFSET   125
#define MACHSET_TIME_IMPACT_PTS_OFFSET       130
#define MACHSET_PRE_PRES_TYPE_OFFSET         135
#define MACHSET_POST_PRES_TYPE_OFFSET        141
#define MACHSET_OPERATOR_NUMBER_OFFSET       147
#define MACHSET_RECLEN                       159

/*
---------------------------
Machine directory databases
--------------------------- */

/*
          10        20        30        40        50        60        70        80        90        100       110       120       130       140       150       160       170       180       190       200       210       220       230       240       250       260       270       280       290
0123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123
|          |  |  |  |        |        |       |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |       |        |     |     |   |  |   |
"VISITRAK";01;02;03;4.100000;4.000000;      3;1.000000;2.000000;3.000000;5.000000;6.000000;4.000000;1.000000;2.000000;3.000000;4.000000;99.00000;99.00000;99.00000;99.00000;1.000000;3.000000;5.000000;7.000000;2.000000;4.000000;6.000000;9.000000;1.000000;      1;2.000000;    1; 1000;"Y"; 1; 1  */
#define PARTS_DB                            TEXT( "PARTS.TXT" )
#define PARTS_DB_FLOAT_LEN                   8
#define PARTS_DB_INT32_LEN                   7
#define PARTS_DB_SHORT_LEN                   5

#define PARTS_PART_NAME_OFFSET               0
#define PARTS_DISTANCE_UNITS_OFFSET          11
#define PARTS_VELOCITY_UNITS_OFFSET          14
#define PARTS_PRESSURE_UNITS_OFFSET          17
#define PARTS_TOTAL_STROKE_LEN_OFFSET        20
#define PARTS_MIN_STROKE_LEN_OFFSET          29
#define PARTS_BISCUIT_TIME_DELAY_OFFSET      38
#define PARTS_EOS_VEL_OFFSET                 46
#define PARTS_SLEEVE_FILL_DIST_OFFSET        55
#define PARTS_RUNNER_FILL_DIST_OFFSET        64
#define PARTS_MIN_CSFS_VEL_OFFSET            73
#define PARTS_RISE_CSFS_VEL_OFFSET           82
#define PARTS_MIN_CSFS_POS_OFFSET            91
#define PARTS_LIMIT_SW_1_OFFSET              100
#define PARTS_LIMIT_SW_2_OFFSET              109
#define PARTS_LIMIT_SW_3_OFFSET              118
#define PARTS_LIMIT_SW_4_OFFSET              127
#define PARTS_USER_VEL_1_OFFSET              136
#define PARTS_USER_VEL_2_OFFSET              145
#define PARTS_USER_VEL_3_OFFSET              154
#define PARTS_USER_VEL_4_OFFSET              163
#define PARTS_AVG_START_1_OFFSET             172
#define PARTS_AVG_START_2_OFFSET             181
#define PARTS_AVG_START_3_OFFSET             190
#define PARTS_AVG_START_4_OFFSET             199
#define PARTS_AVG_END_1_OFFSET               208
#define PARTS_AVG_END_2_OFFSET               217
#define PARTS_AVG_END_3_OFFSET               226
#define PARTS_AVG_END_4_OFFSET               235
#define PARTS_PLUNGER_DIA_OFFSET             244
#define PARTS_TIME_FOR_INTENS_PRES_OFFSET    253
#define PARTS_PRES_FOR_RESP_TIME_OFFSET      261
#define PARTS_MS_PER_TIME_SAMPLE_OFFSET      270
#define PARTS_N_OF_TIME_SAMPLES_OFFSET       276
#define PARTS_PRODUCTION_ENABLED             282
#define PARTS_HEAD_CHANNEL_OFFSET            286
#define PARTS_ROD_CHANNEL_OFFSET             289
#define PARTS_RECLEN                         293

/*        10        20        30
0123456789_123456789_123456789_123456789
|          |      |          |         |
"VISITRAK";123456;04/12/1995;23:59:00  */
#define PLOOKUP_DB                          TEXT( "PLOOKUP.TXT" )
#define PLOOKUP_PART_NAME_OFFSET             0
#define PLOOKUP_LAST_SHOT_OFFSET             11
#define PLOOKUP_DATE_OFFSET                  18
#define PLOOKUP_TIME_OFFSET                  29
#define PLOOKUP_RECLEN                       39

/*        10        20
0123456789_123456789_12345678
|      |          |         |
"0000";04/12/1995;23:59:00  */
#define DSTAT_CODE_LEN                       4

#define DSTAT_DB                            TEXT( "DSTAT.TXT" )
#define DSTAT_CODE_OFFSET                    0
#define DSTAT_DATE_OFFSET                    7
#define DSTAT_TIME_OFFSET                    18
#define DSTAT_RECLEN                         28

/*        10        20
0123456789_123456789_123
|          |           |
"0000FF2C";"VISITRAK"  */
#define RUNLIST_DB                         TEXT( "RUNLIST.TXT" )

#define RUNLIST_BITS_OFFSET                 0    // MAX_HEX_LEN = 8
#define RUNLIST_PART_NAME_OFFSET           11
#define RUNLIST_RECLEN                     23

#define SEMAPHOR_DB                         TEXT( "SEMAPHOR.TXT" )
#define SEMAPHOR_RECLEN                      5

/*
-----------------------------------
Machine results directory databases
----------------------------------- */
/*        10        20        30        40        50
0123456789_123456789_123456789_123456789_123456789_123
|          |        |    |    |          |           |
04/12/1995;23:59:00;"01";"01";"12345678";"VisiTrak"  */

#define DOWNTIME_DB                         TEXT( "DOWNTIME.TXT" )
#define DOWNTIME_DATE_OFFSET                 0
#define DOWNTIME_TIME_OFFSET                 11
#define DOWNTIME_CAT_OFFSET                  20
#define DOWNTIME_SUBCAT_OFFSET               25
#define DOWNTIME_OPERATOR_OFFSET             30
#define DOWNTIME_PART_NAME_OFFSET            41
#define DOWNTIME_RECLEN                      53

/*
------------------------
Part directory databases
------------------------ */
#define CTRLIMIT_DB                         TEXT( "CTRLIMIT.TXT" )
#define CTRLIMIT_FLOAT_LEN                    12

/*        10        20        30
0123456789_123456789_123456789_1
|   |            |             |
012;123456789.12;123456789.12  */

#define CTRLIMIT_PARM_NUMBER_OFFSET           0
#define CTRLIMIT_LOW_LIMIT_OFFSET             4
#define CTRLIMIT_HIGH_LIMIT_OFFSET           17
#define CTRLIMIT_RECLEN                      31

#define FTANALOG_CSV                        TEXT( "FTANALOG.CSV" )
#define FTANALOG_DB                         TEXT( "FTANALOG.TXT" )
#define FTANALOG_FLOAT_LEN                    12

/*        10        20
0123456789_123456789_12345678
|   |  |   |            |   |
012;12;"P";123456789.12;12  */
#define FTANALOG_INPUT_NUMBER_OFFSET         0
#define FTANALOG_CHANNEL_NUMBER_OFFSET       4
#define FTANALOG_SAMPLE_TYPE_OFFSET          7
#define FTANALOG_SAMPLE_VALUE_OFFSET         11
#define FTANALOG_UNITS_OFFSET                24
#define FTANALOG_RECLEN                      28

/*
012345678
|  |    |
12;123  */
#define FTCHAN_DB                           TEXT( "FTCHAN.TXT" )
#define FTCHAN_CHANNEL_NUMBER_OFFSET         0
#define FTCHAN_SENSOR_NUMBER_OFFSET          3
#define FTCHAN_RECLEN                        8

/*        10        20        30        40        50        60        70        80        90        100       110
0123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123
|   |  |   |     |  |                           |            |  |            |  |            |  |            |   |
123;12;123;12345;01;"DescriptionDescriptionDes";123456789_12;LW;123456789_12;HW;123456789_12;LA;123456789_12;HA  */
#define PARMLIST_DB                         TEXT( "PARMLIST.TXT" )
#define PARMLIST_INPUT_TYPE_LEN              2
#define PARMLIST_FLOAT_LEN                   12

#define PARMLIST_PARM_NUMBER_OFFSET          0
#define PARMLIST_INPUT_TYPE_OFFSET           4
#define PARMLIST_INPUT_NUMBER_OFFSET         7
#define PARMLIST_VARIABLE_TYPE_OFFSET        11
#define PARMLIST_UNITS_OFFSET                17
#define PARMLIST_DESC_OFFSET                 20
#define PARMLIST_LOW_WARN_LIMIT_OFFSET       48
#define PARMLIST_LOW_WARN_WIRE_OFFSET        61
#define PARMLIST_HIGH_WARN_LIMIT_OFFSET      64
#define PARMLIST_HIGH_WARN_WIRE_OFFSET       77
#define PARMLIST_LOW_ALARM_LIMIT_OFFSET      80
#define PARMLIST_LOW_ALARM_WIRE_OFFSET       93
#define PARMLIST_HIGH_ALARM_LIMIT_OFFSET     96
#define PARMLIST_HIGH_ALARM_WIRE_OFFSET     109

#define PARMLIST_RECLEN                     113

/*        10        20        30`
0123456789_123456789_123456789_12345678
|   |   |     |              |        |
"P";001;12345;"      YELLOW";"VacOn"  */
#define MARKLIST_DB                         TEXT( "MARKLIST.TXT" )
#define MARKLIST_PARM_TYPE_OFFSET           0
#define MARKLIST_PARM_NUMBER_OFFSET         4
#define MARKLIST_SYMBOL_OFFSET              8
#define MARKLIST_COLOR_OFFSET               14
#define MARKLIST_LABEL_OFFSET               29
#define MARKLIST_RECLEN                     38

/*        10        20        30        40        50
0123456789_123456789_123456789_123456789_123456789_123
|          |          |          |          |   |    |
"12345678";"12345678";"12345678";"12345678";"Y";"N"  */
#define STSTEPS_FLOAT_LEN                   8             /* Includes room for % */
#define STSTEPS_DB                          TEXT( "STSTEPS.TXT" )
#define STSTEPS_ACCEL_OFFSET                 0
#define STSTEPS_VEL_OFFSET                   11
#define STSTEPS_END_POS_OFFSET               22
#define STSTEPS_LOW_IMPACT_OFFSET            33
#define STSTEPS_VACUUM_WAIT_OFFSET           44
#define STSTEPS_LOW_IMPACT_FROM_INPUT_OFFSET 48
#define STSTEPS_RECLEN                       53

/*        10        20        30        40        50        60        70        80        90       100       110
0123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_1234567
|          |          |          |          |        |        |        |        |        |        |        |         |
"12345678";"12345678";"12345678";"12345678";    1.00;    1.00;    2.00;    2.00;    3.00;    3.00;  100.00;  100.00  */
#define STLIMITS_FLOAT_LEN                   8             /* Includes room for % */
#define STLIMITS_DB                         TEXT( "STLIMITS.TXT" )
#define STLIMITS_POSITION_OFFSET             0
#define STLIMITS_RECLEN                      117

#define STLIMITS_DATA                       TEXT( "STLIMITS.DAT" )
#define STPART_DATA                         TEXT( "STPART.DAT" )
#define STPRES_DATA                         TEXT( "STPRES.DAT" )
#define CHAXIS_LABEL_DATA_FILE              TEXT( "chaxislabel.dat" )
#define GPVAR_DB                            TEXT( "GPVAR.CSV" )
#define SUBPARTS_DB                         TEXT( "SUBPARTS.TXT" )
#define SETUP_SHEET_FILE                    TEXT( "SetupSheet.txt" )

/*        10        20
0123456789_123456789_1234567
|   |   |      |           |
"Y";"Y";"  31";"VISITRAK"  */
#define WARMUP_DB                            TEXT( "WARMUP.TXT" )
#define WARMUP_WIRE_LEN                      4

#define WARMUP_ENABLED_OFFSET              0
#define WARMUP_RUN_IF_ON_OFFSET            4
#define WARMUP_WIRE_OFFSET                 8
#define WARMUP_PART_NAME_OFFSET           15
#define WARMUP_RECLEN                     27

/*
--------------------------
Result directory databases
-------------------------- */

/*        10        20
0123456789_123456789_12345678
|      |          |        |
000001;04/12/1995;23:59:00;1;

 30      40        50        60        70        80        90
9_123456789_123456789_123456789_123456789_123456789_123456789_123
|            |            |            |            |
123456789_12;123456789_12;123456789_12;123456789_12;123456789_12;
...
      750       760       770       780       790       800       810
456789_123456789_123456789_123456789_123456789_123456789_123456789_
|            |            |            |            |
123456789_12;123456789_12;123456789_12;123456789_12;123456789_12  */

#define SHOTPARM_DB                         TEXT( "SHOTPARM.TXT" )
#define SHOTPARM_SAVE_FLAGS_LEN              1
#define SHOTPARM_FLOAT_LEN                   12

#define SHOTPARM_SHOT_NUMBER_OFFSET          0
#define SHOTPARM_DATE_OFFSET                 7
#define SHOTPARM_TIME_OFFSET                 18
#define SHOTPARM_SAVE_FLAGS_OFFSET           27
#define SHOTPARM_PARAM_1_OFFSET              29
#define SHOTPARM_PARAM_60_OFFSET             796
#define SHOTPARM_RECLEN                      810

/*        10        20        30
0123456789_123456789_123456789_
|      |          |             |
000001;04/12/1995;23:59:00;1  */
#define GRAPHLST_DB                         TEXT( "GRAPHLST.TXT" )
#define GRAPHLST_SAVE_FLAGS_LEN              1

#define GRAPHLST_SHOT_NUMBER_OFFSET          0
#define GRAPHLST_DATE_OFFSET                 7
#define GRAPHLST_TIME_OFFSET                 18
#define GRAPHLST_SAVE_FLAGS_OFFSET           27
#define GRAPHLST_RECLEN                      30

#define TEMP_GRAPHLST_DB                    TEXT( "TEMPGLST.TXT" )
#define TEMP_SHOTPARM_DB                    TEXT( "TEMPSHOT.TXT" )

/*        10        20        30        40
0123456789_123456789_123456789_123456789_1234567
|          |  |   |      |      |      |
04/12/1995;01;001;   123;   123;   123;   123  */

#define ALARMSUM_DB                         TEXT( "ALARMSUM.TXT" )
#define ALARMSUM_COUNTS_LEN                  6

#define ALARMSUM_DATE_OFFSET                 0
#define ALARMSUM_SHIFT_OFFSET                11
#define ALARMSUM_PARAMETER_NUMBER_OFFSET     14
#define ALARMSUM_LOW_WARNING_COUNT_OFFSET    18
#define ALARMSUM_TOTAL_COUNT_OFFSET          18

#define ALARMSUM_HIGH_WARNING_COUNT_OFFSET   25
#define ALARMSUM_TOTAL_WARNING_COUNT_OFFSET  25

#define ALARMSUM_LOW_ALARM_COUNT_OFFSET      32
#define ALARMSUM_TOTAL_ALARM_COUNT_OFFSET    32

#define ALARMSUM_HIGH_ALARM_COUNT_OFFSET     39
#define ALARMSUM_RECLEN                      47

/*        10        20        30
0123456789_123456789_123456789_123
|     |     |          |
"C01";"E20";"VISITRAK";"000001"  */
#define BACKUP_LIST_DB                        TEXT( "backup_list.txt" )
#define BACKUP_LIST_CN_OFFSET                 0
#define BACKUP_LIST_MN_OFFSET                 6
#define BACKUP_LIST_PN_OFFSET                12
#define BACKUP_LIST_SN_OFFSET                23
#define BACKUP_LIST_RECLEN                   33

