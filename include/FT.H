#ifndef _FASTRAK_
#define _FASTRAK_

typedef unsigned short BOARD_DATA;

/*
-------------------------------------------------------------
Number of parameters returned by ft_fill_shot_parameter_array
------------------------------------------------------------- */
#define NOF_FT_SHOT_PARAMETERS   26

#define LOW_FASTRAK_PORT_ADDRESS 0x220

#define FT_OK                             1
#define FT_BUSY                           2
#define FT_LOCKED_UP                      3
#define FT_ERROR                          5
#define FT_INVALID_CHANNEL                6
#define FT_BAD_BOARD_ADDRESS              7
#define FT_NOF_POINTS_OUT_OF_RANGE        8
#define FT_NOF_POST_POINTS_OUT_OF_RANGE   9
#define FT_ERROR_OUT_OF_STRUCTURES       10     // Driver io space maping error

/*
-----------------
Misc. definitions
----------------- */
#define DELAY_VAL 1000  /* value used in for loop time delay for waiting  */
#define DEGC 0          /* for valid data after sending command to 80c196 */
#define DEGF 1

/*
-----------------------------
Machine types for ft_set_type
----------------------------- */
#define FT_SINGLE_MACHINE_TYPE 0
#define FT_MUX_MACHINE_TYPE    1

/*
-------------------
Command definitions
------------------- */
#define SET_BUSY                0x8000
#define DATA_COLL_CMD           0x8001
#define RESET_CMD               0x8002
#define EN_AT_INT               0x8003
#define SELF_TEST_CMD           0x8004
#define DIS_AT_INT              0x8005
#define CLR_ERR                 0x8006
#define CLR_DATA_READY          0x8007
#define OUT_TO_DAC              0x8008
#define CUR_POS                 0X8009
#define CUR_VEL                 0x800A

#define CUR_ANA1                0x800B
#define CUR_ANA2                0x800C
#define CUR_ANA3                0x800D
#define CUR_ANA4                0x800E

#define RAM_TEST                0x800F
#define TEST_DIO                0x8010
#define SET_REG_TST_FLG         0x8011
#define DATA_REG_WR_TST         0x8012
#define RESTOR_STATUS           0x8013
#define TRIG_INT                0x8014
#define CHANGE_MACH             0x8015
#define GET_MACH_NO             0x8016
#define OUT_TO_OPTO             0x8017   /* low byte only since we only have 8 outputs */
#define LATCH_COUNTER           0x8018 /* latches the shot count for all 8 machines */
#define CLR_COUNTER             0x8019   /* clear counters on mux */
#define GET_COUNT               0x801A     /* get count from mux */
#define SET_SCAN_MODE           0x801B
#define SET_MACH_MASK           0x801C
#define SET_BOARD_TYPE          0x801E  /* 0 == single, 1 == mux */
#define GET_BOARD_TYPE          0x8020
#define GET_VERSION_NUMBER      0x8022
#define AUTO_COLLECTION         0x8023
#define SET_CRYSTAL_FREQ        0x8024
#define RD_CRYSTAL_FREQ         0x8025
#define CHECK_POS_XDCR          0x8026
#define ENBL_INPUT_POS_CHECKING 0x8027
#define DIS_INPUT_POS_CHECKING  0x8028
#define CHK_INPUT_CHECK_STATUS  0x8029
#define CUR_ANA5                0x802A
#define CUR_ANA6                0x802B
#define CUR_ANA7                0x802C
#define CUR_ANA8                0x802D
#define RD_CUR_INPUT_WORD       0x8031

#define NMI                     0x0800

/*
-------------------------------------------
Set parameter read back command definitions
------------------------------------------- */
#define TOT_STK_LEN          0x6800
#define MIN_STK_LEN          0x6801
#define TIM_DEL_BISC         0x6802
#define GAIN_CH1             0x6803
#define GAIN_CH2             0x6804
#define GAIN_CH3             0x6805
#define GAIN_CH4             0x6806
#define VEL_EOS              0x6807
#define RD_FILL_DIST         0x6808
#define RD_RUN_FILL_DIST     0x6809
#define RD_MIN_VEL_FS        0x680A
#define RD_VEL_RISE_TIME     0x680B
#define RD_L_MIN_CSFS        0x680C
#define RD_LS1               0x680D
#define RD_LS2               0x680E
#define RD_LS3               0x680F
#define RD_LS4               0x6810
#define RD_DELAY             0x6811 /* ms for timed collection [25, 2000] */
#define RD_MIN_VEL_EOS       0x6813
#define RD_TIM_COLL_INTERVAL 0x6814
#define RD_TIME_OUT_PERIOD   0x6815
#define GAIN_CH5             0x6816
#define GAIN_CH6             0x6817
#define GAIN_CH7             0x6818
#define GAIN_CH8             0x6819

/*
----------------------------
Read computed shot parameter
---------------------------- */
#define BISC_SIZE 0x7001
#define EOS_POS 0x7002
#define CYCLE_TIME 0x7000
#define P1_POS 0x7003
#define P2_POS 0x7004
#define P3_POS 0x7005
#define FIL_TIM 0x7006
#define TOT_SHT_TIM 0x7007
#define SS_TIM 0x7008
#define FS_TIM 0x7009
#define RT_MILLI 0x700A
#define RT_MICRO 0x700B
#define POS_CSFS 0x700C
#define TIM1 0x700D
#define TIM2 0x700E
#define TIM3 0x700F
#define TIM4 0x7010
#define TIM5 0x7011
#define INPUT_ON_POS1  0x7012
#define INPUT_OFF_POS1 0x7013
#define INPUT_ON_POS2  0x7014
#define INPUT_OFF_POS2 0x7015
#define INPUT_ON_POS3  0x7016
#define INPUT_OFF_POS3 0x7017
#define INPUT_ON_POS4  0x7018
#define INPUT_OFF_POS4 0x7019

/*
----------------------
Parameter set commands
---------------------- */
#define TOT_STK_LEN_SET       0xE800
#define MIN_STK_LEN_SET       0xE801
#define TIM_DEL_BISC_SET      0xE802
#define GN_CH1_SET_CMD        0xE803
#define GN_CH2_SET_CMD        0xE804
#define GN_CH3_SET_CMD        0xE805
#define GN_CH4_SET_CMD        0xE806
#define VEL_EOS_SET           0xE807
#define SET_FILL_DIST         0xE808
#define SET_RUN_FILL_DIST     0xE809
#define SET_MIN_VEL_FS        0xE80A
#define SET_VEL_RISE_TIME     0xE80B
#define SET_L_MIN_CSFS        0xE80C
#define SET_LS1               0xE80D
#define SET_LS2               0xE80E
#define SET_LS3               0xE80F
#define SET_LS4               0xE810
#define SET_DELAY             0xE811 /* ms for timed collection [25, 2000] */
#define SET_MIN_VEL_EOS       0xE813
#define SET_TIM_COLL_INTERVAL 0xE814
#define SET_TIME_OUT_PERIOD   0xE815

#define GN_CH5_SET_CMD        0xE816
#define GN_CH6_SET_CMD        0xE817
#define GN_CH7_SET_CMD        0xE818
#define GN_CH8_SET_CMD        0xE819

/*
-----------
Status bits
----------- */
#define VIS_BUSY             0x8000 /* busy */

#define VIS_ERROR            0x4000 /* error */

#define VIS_DATA_COLL_MODE   0x2000 /* data collection mode */

#define VIS_SPEED_ZERO       0x1000 /* zero speed (counter overflowed) */

#define VIS_DATA_READY       0x0800 /* Shot data is ready to get.
                                       An interrupt will also occur
                                       unless Pro-Viewer has not yet
                                       acknowledged a VIS_PLC_FAULT
                                       interrupt currently being serviced.
                                       This bit therefore must be checked
                                       just before leaving the PLC fault
                                       handling interrupt service routine. */

#define VIS_AUTO_SHOT        0x0400 /* auto shot will give the board
                                       to aquire shot data. */

#define VIS_POWER_UP         0x0200 /* power up */

#define VIS_INT_DISABLED     0x0100 /* interrupts are disabled */

#define VIS_TIME_OUT         0x0080 /* 30 second timeout after auto shot */

#define VIS_POS_TRANS_ERROR  0x0040 /* position transducer error
                                       Quadrature signal is bad. */

#define VIS_SELF_TEST_ERROR  0x0020 /* self test memory error */

#define VIS_INTERNAL_ERROR   0x0010 /* internal error */
                                    /* What are the possible errors? */

#define VIS_INVALID_COMMAND  0x0008 /* invalid command was sent */

#define VIS_CALC_ERROR  0x0004      /* parameter calculation error */

#define SCAN_MODE 0x0001            /* currently in scan mode */




/****************************************************************************
			    function prototypes
****************************************************************************/

#ifdef __cplusplus
    extern "C" {
#endif

/*
------------------
Low level commands
------------------ */
void           ft_clear_cmd_port( void );
void           ft_clear_interrupt( void );
unsigned short ft_board_address( void );
DWORD          ft_read_all_data( unsigned short portid, BOARD_DATA * dest, DWORD destbytes );
BOARD_DATA     ft_read_data( void );
BOARD_DATA     ft_read_status( void );
BOOLEAN        ft_send_cmd_wo_check( BOARD_DATA command );
BOOLEAN        ft_send_data( BOARD_DATA data );
void           ft_setports( unsigned short base_port_id );
void           ft_set_clear_cmd_wait( int new_wait_count );

/*
---------------------------
Intermediate level commands
--------------------------- */
int     ft_read_parameter( BOARD_DATA parameter, BOARD_DATA * ptr_result );
int     ft_send_command( BOARD_DATA command );
int     ft_send_cmd_busy( BOARD_DATA command );
int     ft_set_command_mode( void );
int     ft_set_data_coll_mode( void );
int     ft_set_parameter( BOARD_DATA data, BOARD_DATA command );
int     ft_set_scan_mode( void );
BOOLEAN ft_wait_for_busy_bit_clear( void );

/*
-------------------
High level commands
------------------- */
BOOLEAN ft_startup( void );
void ft_shutdown( void );
int convert( int channel, BOARD_DATA val, float * ptr_result );
int ft_change_mux_channel( short mux_channel );
int ft_rd_mux_channel( short * mux_channel );
int ft_set_gain( short channel, short gain );
int ft_set_type( BOARD_DATA type );
int get_version( short * dest );
int lfill_analog_vs_pos_array(  BOARD_DATA * dest, BOARD_DATA * pmin, BOARD_DATA * pmax, short channel, short n );
int lfill_analog_vs_time_array( BOARD_DATA * dest, BOARD_DATA * pmin, BOARD_DATA * pmax, short channel, short n );
int lfill_pos_array( BOARD_DATA * dest, BOARD_DATA * pmin, BOARD_DATA * pmax, short n );
int lfill_vel_array( BOARD_DATA * dest, BOARD_DATA * pmin, BOARD_DATA * pmax, short n );
int lrd_analog_pos(int channel, BOARD_DATA position, BOARD_DATA *ptr_val_analog );
int lrd_analog_tim(int channel, BOARD_DATA seconds, BOARD_DATA *ptr_val_analog);
int lrd_bisc_size( BOARD_DATA * ptr_result );
int lrd_csfs( BOARD_DATA * ptr_result );
int lrd_cycle_tim( BOARD_DATA *ptr_result );
int lrd_eos_pos( BOARD_DATA * ptr_result );
int lrd_p1( BOARD_DATA * ptr_result );
int lrd_p2( BOARD_DATA * ptr_result );
int lrd_p3( BOARD_DATA * ptr_result );
int lrd_pos( int ms, BOARD_DATA *ptr_pos );
int rd_time_interval( float * dest, int i );
int lrd_vel( BOARD_DATA position, BOARD_DATA *ptr_vel );
int new_rd_fill_time( BOARD_DATA *ptr_result, BOARD_DATA p2 );
int rd_analog_lpos( short channel, BOARD_DATA position, BOARD_DATA *ptr_result);
int rd_analog_pos(int channel, double position, BOARD_DATA *ptr_result);
int rd_analog_tim( short channel, BOARD_DATA seconds, BOARD_DATA * ptr_result );
int rd_bisc_size( float *ptr_result );
int rd_crystal_freq( float * freq );
int rd_csfs( float * dest );
int rd_cycle_tim(float *ptr_result);
int rd_delay( BOARD_DATA * dest );
int rd_eos_pos( float *ptr_result );
int rd_fast_shot_time( BOARD_DATA * ptr_result );
int rd_fill_distance( float * dest );
int rd_fill_time( BOARD_DATA * ptr_result );
int rd_gain( short channel, short * dest );
int rd_limit_switch( short switch_number, float * dest );
int rd_min_stk_len( float *dest );
int rd_min_vel_csfs( float * dest );
int rd_min_vel_eos( float * dest );
int rd_on_off_input( float * dest, short input_number );
int rd_p1(float *ptr_result);
int rd_p2(float *ptr_result);
int rd_p3(float *ptr_result);
int rd_pos(int seconds, float *ptr_result);
int rd_pos_csfs( float * dest );
int rd_rise_time( float * dest );
int rd_run_fill_distance( float * dest );
int rd_slow_shot_time( BOARD_DATA * ptr_result );
int rd_time_out_period( float * dest );
int rd_tim_coll_interval( float * dest );
int rd_tim1_interval(float *ptr_result);
int rd_tim2_interval(float *ptr_result);
int rd_tim3_interval(float *ptr_result);
int rd_tim4_interval(float *ptr_result);
int rd_tim5_interval(float *ptr_result);
int rd_tim_del_bisc( float * dest );
int rd_total_stk_len( float *dest );
int rd_tot_shot_time( BOARD_DATA * ptr_result );
int rd_vel(double position, float *ptr_result);
int rd_vel_eos( float *dest );
int rd_vel_lpos(BOARD_DATA position, float *ptr_result);
int rd_vel_rise_time( float * dest );
float seconds_from_count( BOARD_DATA count );
int set_crystal_freq( float freq );
int set_delay( short delay );
int set_fill_distance( float x );
int set_limit_switch( short switch_number, float x );
int set_mach_mask( BOARD_DATA mask );
int set_min_stk_len( float x );
int set_min_vel_csfs( float v );
int set_min_vel_eos(float fval);
int set_pos_csfs( float x );
int set_run_fill_distance( float x );
int set_tim_coll_interval( float ms );
int set_tim_del_bisc( float seconds );
int set_time_out_period( float seconds );
int set_tot_stk_len( float x );
int set_vel_eos( float v );
int set_vel_rise_time( float v );
int wr_opto( BOARD_DATA bits );

#ifdef __cplusplus
    }
#endif

#endif

