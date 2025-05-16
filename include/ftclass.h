#ifndef _FTCLASS_
#define _FTCLASS_

typedef unsigned short BOARD_DATA;

const unsigned short LOW_FASTRAK_PORT_ADDRESS      = 0x220;
const unsigned short DEFAULT_SURETRAK_PORT_ADDRESS = 0x1C0;

const int FT_OK        = 1;
const int FT_BUSY      = 2;
const int FT_LOCKED_UP = 3;
const int FT_ERROR     = 5;

/*
-----------------------------
Machine types for ft_set_type
----------------------------- */
#define FT_SINGLE_MACHINE_TYPE 0
#define FT_MUX_MACHINE_TYPE    1

/*
---------------------------
FasTrak command definitions
--------------------------- */
const BOARD_DATA SET_BUSY                = 0x8000;
const BOARD_DATA DATA_COLL_CMD           = 0x8001;
const BOARD_DATA RESET_CMD               = 0x8002;
const BOARD_DATA EN_AT_INT               = 0x8003;
const BOARD_DATA SELF_TEST_CMD           = 0x8004;
const BOARD_DATA DIS_AT_INT              = 0x8005;
const BOARD_DATA CLR_ERR                 = 0x8006;
const BOARD_DATA CLR_DATA_READY          = 0x8007;
const BOARD_DATA OUT_TO_DAC              = 0x8008;
const BOARD_DATA CUR_POS                 = 0X8009;
const BOARD_DATA CUR_VEL                 = 0x800A;
const BOARD_DATA CUR_ANA1                = 0x800B;
const BOARD_DATA CUR_ANA2                = 0x800C;
const BOARD_DATA CUR_ANA3                = 0x800D;
const BOARD_DATA CUR_ANA4                = 0x800E;
const BOARD_DATA RAM_TEST                = 0x800F;
const BOARD_DATA TEST_DIO                = 0x8010;
const BOARD_DATA SET_REG_TST_FLG         = 0x8011;
const BOARD_DATA DATA_REG_WR_TST         = 0x8012;
const BOARD_DATA RESTOR_STATUS           = 0x8013;
const BOARD_DATA TRIG_INT                = 0x8014;
const BOARD_DATA CHANGE_MACH             = 0x8015;
const BOARD_DATA GET_MACH_NO             = 0x8016;
const BOARD_DATA OUT_TO_OPTO             = 0x8017; /* low byte only since we only have 8 outputs */
const BOARD_DATA LATCH_COUNTER           = 0x8018; /* latches the shot count for all 8 machines */
const BOARD_DATA CLR_COUNTER             = 0x8019; /* clear counters on mux */
const BOARD_DATA GET_COUNT               = 0x801A; /* get count from mux */
const BOARD_DATA SET_SCAN_MODE           = 0x801B;
const BOARD_DATA SET_MACH_MASK           = 0x801C;
const BOARD_DATA SET_BOARD_TYPE          = 0x801E; /* 0 == single, 1 == mux */
const BOARD_DATA GET_BOARD_TYPE          = 0x8020;
const BOARD_DATA GET_VERSION_NUMBER      = 0x8022;
const BOARD_DATA AUTO_COLLECTION         = 0x8023;
const BOARD_DATA SET_CRYSTAL_FREQ        = 0x8024;
const BOARD_DATA RD_CRYSTAL_FREQ         = 0x8025;
const BOARD_DATA CHECK_POS_XDCR          = 0x8026;
const BOARD_DATA ENBL_INPUT_POS_CHECKING = 0x8027;
const BOARD_DATA DIS_INPUT_POS_CHECKING  = 0x8028;
const BOARD_DATA CHK_INPUT_CHECK_STATUS  = 0x8029;
const BOARD_DATA NMI                     = 0x0800;

/*
-----------------
SureTrak Commands
----------------- */
const BOARD_DATA RD_CURRENT_DAC      = 0x8021;
const BOARD_DATA RD_INPUT_WORD       = 0x8022;
const BOARD_DATA RD_OUTPUT_WORD      = 0x8023;

const BOARD_DATA CUR_ANA5            = 0x802A;
const BOARD_DATA CUR_ANA6            = 0x802B;
const BOARD_DATA CUR_ANA7            = 0x802C;
const BOARD_DATA CUR_ANA8            = 0x802D;

/*
-------------------------------------------
Set parameter read back command definitions
------------------------------------------- */
const BOARD_DATA TOT_STK_LEN             = 0x6800;
const BOARD_DATA MIN_STK_LEN             = 0x6801;
const BOARD_DATA TIM_DEL_BISC            = 0x6802;
const BOARD_DATA GAIN_CH1                = 0x6803;
const BOARD_DATA GAIN_CH2                = 0x6804;
const BOARD_DATA GAIN_CH3                = 0x6805;
const BOARD_DATA GAIN_CH4                = 0x6806;
const BOARD_DATA VEL_EOS                 = 0x6807;
const BOARD_DATA RD_FILL_DIST            = 0x6808;
const BOARD_DATA RD_RUN_FILL_DIST        = 0x6809;
const BOARD_DATA RD_MIN_VEL_FS           = 0x680A;
const BOARD_DATA RD_VEL_RISE_TIME        = 0x680B;
const BOARD_DATA RD_L_MIN_CSFS           = 0x680C;
const BOARD_DATA RD_LS1                  = 0x680D;
const BOARD_DATA RD_LS2                  = 0x680E;
const BOARD_DATA RD_LS3                  = 0x680F;
const BOARD_DATA RD_LS4                  = 0x6810;
const BOARD_DATA RD_DELAY                = 0x6811; /* Number of milliseconds for timed collection
                                                      from 25 to 2000 after eos */
const BOARD_DATA RD_MIN_VEL_EOS          = 0x6813;
const BOARD_DATA RD_TIM_COLL_INTERVAL    = 0x6814;
const BOARD_DATA RD_TIME_OUT_PERIOD      = 0x6815;

/*
----------------------------------------------------------
Read computed shot parameter / current value read commands
---------------------------------------------------------- */
const BOARD_DATA BISC_SIZE               = 0x7001;
const BOARD_DATA EOS_POS                 = 0x7002;
const BOARD_DATA CYCLE_TIME              = 0x7000;
const BOARD_DATA P1_POS                  = 0x7003;
const BOARD_DATA P2_POS                  = 0x7004;
const BOARD_DATA P3_POS                  = 0x7005;
const BOARD_DATA FIL_TIM                 = 0x7006;
const BOARD_DATA TOT_SHT_TIM             = 0x7007;
const BOARD_DATA SS_TIM                  = 0x7008;
const BOARD_DATA FS_TIM                  = 0x7009;
const BOARD_DATA RT_MILLI                = 0x700A;
const BOARD_DATA RT_MICRO                = 0x700B;
const BOARD_DATA POS_CSFS                = 0x700C;
const BOARD_DATA TIM1                    = 0x700D;
const BOARD_DATA TIM2                    = 0x700E;
const BOARD_DATA TIM3                    = 0x700F;
const BOARD_DATA TIM4                    = 0x7010;
const BOARD_DATA TIM5                    = 0x7011;
const BOARD_DATA INPUT_ON_POS1           = 0x7012;
const BOARD_DATA INPUT_OFF_POS1          = 0x7013;
const BOARD_DATA INPUT_ON_POS2           = 0x7014;
const BOARD_DATA INPUT_OFF_POS2          = 0x7015;
const BOARD_DATA INPUT_ON_POS3           = 0x7016;
const BOARD_DATA INPUT_OFF_POS3          = 0x7017;
const BOARD_DATA INPUT_ON_POS4           = 0x7018;
const BOARD_DATA INPUT_OFF_POS4          = 0x7019;

/*
----------------------
Parameter set commands
---------------------- */
const BOARD_DATA TOT_STK_LEN_SET         = 0xE800;
const BOARD_DATA MIN_STK_LEN_SET         = 0xE801;
const BOARD_DATA TIM_DEL_BISC_SET        = 0xE802;
const BOARD_DATA GN_CH1_SET_CMD          = 0xE803;
const BOARD_DATA GN_CH2_SET_CMD          = 0xE804;
const BOARD_DATA GN_CH3_SET_CMD          = 0xE805;
const BOARD_DATA GN_CH4_SET_CMD          = 0xE806;
const BOARD_DATA VEL_EOS_SET             = 0xE807;
const BOARD_DATA SET_FILL_DIST           = 0xE808;
const BOARD_DATA SET_RUN_FILL_DIST       = 0xE809;
const BOARD_DATA SET_MIN_VEL_FS          = 0xE80A;
const BOARD_DATA SET_VEL_RISE_TIME       = 0xE80B;
const BOARD_DATA SET_L_MIN_CSFS          = 0xE80C;
const BOARD_DATA SET_LS1                 = 0xE80D;
const BOARD_DATA SET_LS2                 = 0xE80E;
const BOARD_DATA SET_LS3                 = 0xE80F;
const BOARD_DATA SET_LS4                 = 0xE810;
const BOARD_DATA SET_DELAY               = 0xE811; /* Number of milliseconds for timed collection
                                                      from 25 to 2000 after eos */
const BOARD_DATA SET_MIN_VEL_EOS         = 0xE813;
const BOARD_DATA SET_TIM_COLL_INTERVAL   = 0xE814;
const BOARD_DATA SET_TIME_OUT_PERIOD     = 0xE815;

/*
-----------
Status bits
----------- */
const BOARD_DATA  VIS_BUSY            = 0x8000; /* busy */
const BOARD_DATA  VIS_ERROR           = 0x4000; /* error */
const BOARD_DATA CVIS_CONTROL_ERR     = 0x4000; /* Control error */
const BOARD_DATA  VIS_DATA_COLL_MODE  = 0x2000; /* data collection mode */
const BOARD_DATA CVIS_CONTROL_ACTIVE  = 0x2000; /* Control Active */
const BOARD_DATA  VIS_SPEED_ZERO      = 0x1000; /* zero speed (counter overflowed) */
const BOARD_DATA  VIS_DATA_READY      = 0x0800; /* Shot data is ready to get.
                                                  An interrupt will also occur
                                                  unless Pro-Viewer has not yet
                                                  acknowledged a VIS_PLC_FAULT
                                                  interrupt currently being serviced.
                                                  This bit therefore must be checked
                                                  just before leaving the PLC fault
                                                  handling interrupt service routine. */

const BOARD_DATA CVIS_SHOT_IN_PROGRESS= 0x0800;/* Shot in progress */

const BOARD_DATA  VIS_AUTO_SHOT       = 0x0400;
const BOARD_DATA  VIS_POWER_UP        = 0x0200; /* Power up */
const BOARD_DATA  VIS_INT_DISABLED    = 0x0100; /* Interrupts are disabled */
const BOARD_DATA  VIS_TIME_OUT        = 0x0080; /* 30 second timeout after auto shot */
const BOARD_DATA  VIS_POS_TRANS_ERROR = 0x0040; /* Position transducer error, quadrature signal is bad */
const BOARD_DATA  VIS_SELF_TEST_ERROR = 0x0020; /* Self test memory error */
const BOARD_DATA CVIS_MEMORY_ERROR    = 0x0020; /* Self test memory error */
const BOARD_DATA  VIS_INTERNAL_ERROR  = 0x0010; /* Internal error */
const BOARD_DATA  VIS_INVALID_COMMAND = 0x0008; /* Invalid command was sent */
const BOARD_DATA  VIS_CALC_ERROR      = 0x0004; /* Parameter calculation error */
const BOARD_DATA  SCAN_MODE           = 0x0001; /* Currently in scan mode */

class BOARD_CONTROL_CLASS
    {
    private:

    static int FtClearCmdWait;

    unsigned short data_port;
    unsigned short command_status_port;
    unsigned short interrupt_port;
    unsigned short clear_command_port;

    void short_wait( void );

    public:

    BOARD_CONTROL_CLASS();
    ~BOARD_CONTROL_CLASS();
    void       clear_command( void );
    void       clear_interrupt( void );
    BOARD_DATA read_data( void );
    int        read_parameter( BOARD_DATA parameter, BOARD_DATA * dest );
    BOARD_DATA read_status( void );
    int        send_command( BOARD_DATA command );
    int        send_command_busy( BOARD_DATA command );
    void       send_command_wo_check( BOARD_DATA command );
    BOOLEAN    send_data( BOARD_DATA data );
    void       setports( unsigned short base_port_id );
    void       set_wait_count( int new_wait_count ){ if ( new_wait_count > 0 ) FtClearCmdWait = new_wait_count; }
    int        set_command_mode( void );
    int        set_parameter( BOARD_DATA parameter, BOARD_DATA sorc );
    void       shutdown( void );
    BOOLEAN    startup( void );
    BOOLEAN    wait_for_busy_bit_clear( void );
    BOOLEAN    wait_for_busy_bit_set( void );
    };

#endif

