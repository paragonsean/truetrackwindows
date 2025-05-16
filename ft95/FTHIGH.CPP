#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\structs.h"
#include "..\include\ft.h"

extern POS_SENSOR_SETUP_DATA * utype;

/***********************************************************************
*                            FT_STARTUP                                *
*                     Win95 Version does nothing.                      *
***********************************************************************/
BOOLEAN ft_startup( void )
{
return TRUE;
}

/***********************************************************************
*                            FT_SHUTDOWN                               *
*                     Win95 Version does nothing.                      *
***********************************************************************/
void ft_shutdown( void )
{
}

/***********************************************************************
*                            FT_SET_GAIN                               *
*                                                                      *
*  Function set_gain is used to establish the gain settings for the    *
*  analog channels. Channel is an int from 1 to 4 and gain is an int   *
*  which is either 1, 2, 4, or 8. A one is returned if the operation   *
*  is sucessful.                                                       *
***********************************************************************/
int ft_set_gain( short channel, short gain )
{

static BOARD_DATA command[4] = { GN_CH1_SET_CMD, GN_CH2_SET_CMD, GN_CH3_SET_CMD, GN_CH4_SET_CMD };
static BOARD_DATA gain_setting[4][8] =
    {
    { 0x0000, 0x0004, 0, 0x0008, 0, 0, 0, 0x000C }, /* ch 1 */
    { 0x0001, 0x0005, 0, 0x0009, 0, 0, 0, 0x000D }, /* ch 2 */
    { 0x0002, 0x0006, 0, 0x000A, 0, 0, 0, 0x000E }, /* ch 3 */
    { 0x0003, 0x0007, 0, 0x000B, 0, 0, 0, 0x000F }  /* ch 4 */
    };

if ( channel < 1 || channel > 4 )
    return FT_ERROR;

if ( gain < 1 || gain > 8 )
    return FT_ERROR;

/*
-----------------------------------
Convert channel and gain to indexes
----------------------------------- */
channel--;
gain--;

return ft_set_parameter( gain_setting[channel][gain], command[channel] );
}

/***********************************************************************
*                               RD_GAIN                                *
*                           channel = [1,8]                            *
***********************************************************************/
int rd_gain( short channel, short * dest )
{
static BOARD_DATA command[8] = { GAIN_CH1, GAIN_CH2, GAIN_CH3, GAIN_CH4, GAIN_CH5, GAIN_CH6, GAIN_CH7, GAIN_CH8 };

int        status;
BOARD_DATA x;

*dest = 1;

if ( channel < 1 || channel > 8 )
     return FT_INVALID_CHANNEL;

channel--;
status = ft_read_parameter( command[channel], &x );
if (  status == FT_OK )
    {
    x &= 0x000C;
    switch ( x )
        {
        case 0x0000:
            *dest = 1;
            break;

        case 0x0004:
            *dest = 2;
            break;

        case 0x0008:
            *dest = 4;
            break;

        case 0x000C:
            *dest = 8;
            break;
        }
    }

return status;
}

/***********************************************************************
*                             VEL_COUNT                                *
***********************************************************************/
static BOARD_DATA vel_count( float velocity )
{

if ( velocity < utype->min_vel )
    velocity = utype->min_vel;

velocity = utype->vel_dividend / velocity;

return BOARD_DATA(velocity);
}

/***********************************************************************
*                            DIST_FROM_MARKS                           *
*                            Position Based                            *
***********************************************************************/
static inline float dist_from_marks( BOARD_DATA marks )
{
return float(marks) / utype->x1_pos_divisor;
}

/***********************************************************************
*                              DIST_COUNT                              *
*         Convert a distance to a time-based position count.           *
***********************************************************************/
static BOARD_DATA dist_count( float distance )
{

distance *= utype->x4_pos_divisor;

return BOARD_DATA( distance );
}

/***********************************************************************
*                                                                      *
*                      BOARD_DATA DIST_FROM_COUNT                      *
*                                                                      *
*   The count is the pos count during the time-based sampling.         *
*   There are 4 counts per mark so we use the x4_pos_divisor.          *
***********************************************************************/
static float dist_from_count( BOARD_DATA count )
{

float distance;

distance = float( count );

distance /= utype->x4_pos_divisor;

return distance;
}

/***********************************************************************
*                             SECONDS_COUNT                            *
***********************************************************************/
static BOARD_DATA seconds_count( float seconds )
{

seconds *= utype->crystal_freq * 1.0e6;
seconds /= 1048560.0;

if ( seconds > 65535.0 )
    seconds = 65535.0;

return BOARD_DATA( seconds );
}

/***********************************************************************
*                           SECONDS_FROM_COUNT                         *
***********************************************************************/
float seconds_from_count( BOARD_DATA count )
{
float seconds;

seconds = float( count );
seconds *= 1048560.0;

seconds /= utype->crystal_freq * 1.0e6;

return seconds;
}

/***********************************************************************
*                               SET_TYPE                               *
*                        0 = single, 1 = mux                           *
***********************************************************************/
int ft_set_type( BOARD_DATA type )
{
return ft_set_parameter( type, SET_BOARD_TYPE );
}

/***********************************************************************
*                        FT_CHANGE_MUX_CHANNEL                         *
*                             Mux [1,8]                                *
***********************************************************************/
int ft_change_mux_channel( short mux_channel )
{
return ft_set_parameter( BOARD_DATA(mux_channel), CHANGE_MACH );
}

/***********************************************************************
*                          FT_RD_MUX_CHANNEL                           *
***********************************************************************/
int ft_rd_mux_channel( short * dest )
{
BOARD_DATA mux_channel;
int        status;

*dest = 0;
status = ft_read_parameter( GET_MACH_NO, &mux_channel );
if ( status == FT_OK )
    *dest = short( mux_channel );

return status;
}

/***********************************************************************
*                           SET_TOT_STK_LEN                            *
***********************************************************************/
int set_tot_stk_len( float x )
{
return ft_set_parameter( dist_count(x), TOT_STK_LEN_SET );
}

/***********************************************************************
*                           SET_MIN_STK_LEN                            *
***********************************************************************/
int set_min_stk_len( float x )
{
return ft_set_parameter( dist_count(x), MIN_STK_LEN_SET );
}

/***********************************************************************
*                           SET_TIM_DEL_BISC                           *
***********************************************************************/
int set_tim_del_bisc( float seconds )
{
if ( seconds > 10.0 )
    seconds = 10.0;               /* limit to 10 seconds max */

return ft_set_parameter( seconds_count(seconds), TIM_DEL_BISC_SET );
}

/***********************************************************************
*                             SET_VEL_EOS                              *
***********************************************************************/
int set_vel_eos( float v )
{
return ft_set_parameter( vel_count(v), VEL_EOS_SET );
}

/***********************************************************************
*                           SET_MIN_VEL_EOS                            *
***********************************************************************/
int set_min_vel_eos( float v )
{
return ft_set_parameter( vel_count(v), SET_MIN_VEL_EOS );
}

/***********************************************************************
*                          SET_FILL_DISTANCE                           *
***********************************************************************/
int set_fill_distance( float x )
{
return ft_set_parameter( dist_count(x), SET_FILL_DIST );
}

/***********************************************************************
*                        SET_RUN_FILL_DISTANCE                         *
***********************************************************************/
int set_run_fill_distance( float x )
{
return ft_set_parameter( dist_count(x), SET_RUN_FILL_DIST );
}

/***********************************************************************
*                           SET_MIN_VEL_CSFS                           *
***********************************************************************/
int set_min_vel_csfs( float v )
{
return ft_set_parameter( vel_count(v), SET_MIN_VEL_FS );
}

/***********************************************************************
*                          SET_VEL_RISE_TIME                           *
***********************************************************************/
int set_vel_rise_time( float v )
{
return ft_set_parameter( vel_count(v), SET_VEL_RISE_TIME );
}

/***********************************************************************
*                             SET_POS_CSFS                             *
***********************************************************************/
int set_pos_csfs( float x )
{
return ft_set_parameter( dist_count(x), SET_L_MIN_CSFS );
}

/***********************************************************************
*                           SET_LIMIT_SWITCH                           *
*                          Switch number [0,3].                        *
***********************************************************************/
int set_limit_switch( short switch_number, float x )
{
static BOARD_DATA command[4] = { SET_LS1, SET_LS2, SET_LS3, SET_LS4 };

return ft_set_parameter( dist_count(x), command[switch_number] );
}

/***********************************************************************
*                               SET_DELAY                              *
*             Sets the amount of timed collection after eos            *
***********************************************************************/
int set_delay( short delay )
{
if ( delay < 25 )
    delay = 25;

if ( delay > 2000 )
    delay = 2000;

return ft_set_parameter( BOARD_DATA(delay), SET_DELAY );
}

/***********************************************************************
*                             GET_VERSION                              *
***********************************************************************/
int get_version( short * dest )
{

int status;
BOARD_DATA version;

*dest = 0;
status = ft_read_parameter( GET_VERSION_NUMBER, &version );
if ( status == FT_OK )
    *dest = short( version );

return status;
}

/***********************************************************************
*                               RD_DELAY                               *
***********************************************************************/
int rd_delay( BOARD_DATA * dest )
{
return ft_read_parameter( RD_DELAY, dest );
}

/***********************************************************************
*                           RD_CRYSTAL_FREQ                            *
***********************************************************************/
int rd_crystal_freq( float * freq )
{

int status;
BOARD_DATA i;

*freq = 0.0;

status = ft_read_parameter( RD_CRYSTAL_FREQ, &i );
if (status == FT_OK )
    {
    /*
    --------------
    Convert to mhz
    -------------- */
    i /= 1000;
    *freq = (float)i;
    }

return status;
}

/***********************************************************************
*                           SET_CRYSTAL_FREQ                           *
***********************************************************************/
int set_crystal_freq( float freq )
{
freq *= 1000.0;
return ft_set_parameter( BOARD_DATA(freq), SET_CRYSTAL_FREQ );
}

/***********************************************************************
*                           SET_MACH_MASK                              *
***********************************************************************/
int set_mach_mask( BOARD_DATA mask )
{
return ft_set_parameter( mask, SET_MACH_MASK );
}

/***********************************************************************
*                        SET_TIM_COLL_INTERVAL                         *
***********************************************************************/
int set_tim_coll_interval( float interval )
{

int status;
float mhz;

status = rd_crystal_freq( &mhz );
if ( status == FT_OK )
    {
    if ( mhz >= 11.9 )  /* If 12 MHz allow .2 mS minimum sample */
        {
        if ( interval < .2) interval = .2;      /* interval in mS */
        }
    else    /* Allow only 1.0 mS */
        {
        if ( interval < 1.0 ) interval = 1.0;
        }

    if ( interval > 80.0 ) interval = 80.0;
        interval *= 10.0;
    status = ft_set_parameter( BOARD_DATA(interval), SET_TIM_COLL_INTERVAL );
    }

return status;
}

/***********************************************************************
*                         SET_TIME_OUT_PERIOD                          *
*                          AUTO-SHOT TIMEOUT                           *
***********************************************************************/
int set_time_out_period( float seconds )
{

BOARD_DATA count;

if ( seconds < 5.0 )
    seconds = 0.0;

count = seconds_count( seconds );

if ( count < 10 )
    count = 0;

return ft_set_parameter( count, SET_TIME_OUT_PERIOD );
}

/***********************************************************************
*                               WR_OPTO                                *
***********************************************************************/
int wr_opto( BOARD_DATA bits )
{
return ft_set_parameter( bits, OUT_TO_OPTO );
}

/***********************************************************************
*                            RD_ANALOG_LPOS                            *
***********************************************************************/
int rd_analog_lpos( short channel, BOARD_DATA position, BOARD_DATA *ptr_result)
{

BOARD_DATA command;
static BOARD_DATA cmd_constant[5] = {0, 4096, 6144, 8192, 10240};
int status;

command = position + cmd_constant[channel];
status = ft_read_parameter( command, ptr_result );
(*ptr_result) &= 0x0FFF;

return status;
}

/***********************************************************************
*                                                                      *
*                            RD_ANALOG_TIM                             *
*                                                                      *
*  This function returns an BOARD_DATA corresponding to the analog     *
*  value  at the time in seconds specified and channel specified for   *
*  one of four analog channels                                         *
*                                                                      *
***********************************************************************/
int rd_analog_tim( short channel, BOARD_DATA seconds, BOARD_DATA * ptr_result )
{

BOARD_DATA command;
static BOARD_DATA cmd_constant[5] = {0, 16384, 18432, 20480, 22528};
int status;

/*
-------------------------------------------------
The time specification is imbedded in the command
------------------------------------------------- */
command = seconds + cmd_constant[channel];

status = ft_read_parameter( command, ptr_result );
(*ptr_result) &= 0x0FFF;

/*
------------------------------------------------------
we now have the value for the specified analog channel
at the specified time
------------------------------------------------------ */
return status;
}

/***********************************************************************
*                                                                      *
*                           RD_TOT_SHOT_TIME                           *
*                                                                      *
***********************************************************************/
int rd_tot_shot_time( BOARD_DATA * ptr_result )
{
int status;

status = ft_read_parameter( TOT_SHT_TIM, ptr_result );
if ( status != FT_OK )
    *ptr_result = 0;

return status;
}

/***********************************************************************
*                             LRD_EOS_POS                              *
***********************************************************************/
int lrd_eos_pos( BOARD_DATA * ptr_result )
{
return ft_read_parameter( EOS_POS, ptr_result );
}

/***********************************************************************
*                          LRD_INPUT_ON_POS1                           *
***********************************************************************/
int lrd_input_on_pos1( BOARD_DATA * ptr_result )
{
return ft_read_parameter(INPUT_ON_POS1, ptr_result );
}

/***********************************************************************
*                          LRD_INPUT_OFF_POS1                          *
***********************************************************************/
int lrd_input_off_pos1( BOARD_DATA * ptr_result )
{
return ft_read_parameter(INPUT_OFF_POS1, ptr_result );
}

/***********************************************************************
*                          LRD_INPUT_ON_POS2                           *
***********************************************************************/
int lrd_input_on_pos2( BOARD_DATA * ptr_result )
{
return ft_read_parameter(INPUT_ON_POS2, ptr_result );
}

/***********************************************************************
*                          LRD_INPUT_OFF_POS2                          *
***********************************************************************/
int lrd_input_off_pos2( BOARD_DATA * ptr_result )
{
return ft_read_parameter(INPUT_OFF_POS2, ptr_result );
}

/***********************************************************************
*                          LRD_INPUT_ON_POS3                           *
***********************************************************************/
int lrd_input_on_pos3( BOARD_DATA * ptr_result )
{
return ft_read_parameter(INPUT_ON_POS3, ptr_result );
}

/***********************************************************************
*                          LRD_INPUT_OFF_POS3                          *
***********************************************************************/
int lrd_input_off_pos3( BOARD_DATA * ptr_result )
{
return ft_read_parameter(INPUT_OFF_POS3, ptr_result );
}

/***********************************************************************
*                          LRD_INPUT_ON_POS4                           *
***********************************************************************/
int lrd_input_on_pos4( BOARD_DATA * ptr_result )
{
return ft_read_parameter(INPUT_ON_POS4, ptr_result );
}

/***********************************************************************
*                          LRD_INPUT_OFF_POS4                          *
***********************************************************************/
int lrd_input_off_pos4( BOARD_DATA * ptr_result )
{
return ft_read_parameter(INPUT_OFF_POS4, ptr_result );
}


/***********************************************************************
*                            RD_ON_OFF_INPUT                           *
*                                                                      *
*                      The input number is [1,8];                      *
***********************************************************************/
int rd_on_off_input( float * dest, short input_number )
{
BOARD_DATA command;
BOARD_DATA x;
int status;

command = INPUT_ON_POS1 + input_number - 1;


status = ft_read_parameter( command, &x );
if ( status == FT_OK )
    *dest = dist_from_count( x );

return status;
}

/***********************************************************************
*                           LFILL_VEL_ARRAY                            *
*                                                                      *
*    Fill the passed array with position based raw velocity data.      *
*                                                                      *
***********************************************************************/
int lfill_vel_array( BOARD_DATA * dest, BOARD_DATA * pmin, BOARD_DATA * pmax, short n )
{
BOARD_DATA command;
int status;

status  = FT_OK;
command = 12289;
*pmax   = 0xffff;
*pmin   = 0;

while ( n )
    {
    status = ft_send_cmd_busy( command );
    if ( status != FT_OK )
        break;

    *dest = ft_read_data();

    if ( *dest < *pmax )  /* smaller number = higher velocity */
        *pmax = *dest;

    if ( *dest > *pmin )
        *pmin = *dest;

    dest++;
    command++;
    n--;
    }

return status;
}

/***********************************************************************
*                           LFILL_POS_ARRAY                            *
*                                                                      *
*        Fill the passed dest with up 2000 milliseconds worth          *
*        of raw position data after eos.                               *
*                                                                      *
***********************************************************************/
int lfill_pos_array( BOARD_DATA * dest, BOARD_DATA * pmin, BOARD_DATA * pmax, short n )
{

BOARD_DATA command;
int status;

status  = FT_OK;
command = 24577;
*pmin   = 0xffff;
*pmax   = 0;

while ( n )
    {
    status = ft_send_cmd_busy( command );
    if ( status != FT_OK )
        break;

    *dest = ft_read_data();
    if ( *dest > *pmax )
        *pmax = *dest;

    if ( *dest < *pmin )
        *pmin = *dest;

    dest++;
    command++;
    n--;
    }

return status;
}

/***********************************************************************
*                          FILL_ANALOG_ARRAY                           *
***********************************************************************/
static int fill_analog_array( BOARD_DATA * dest, BOARD_DATA * pmin, BOARD_DATA * pmax, BOARD_DATA command, short n )
{
int status;

/*
--------------------------------------------------------------
The command passed to me is the first. I need to add one to it
-------------------------------------------------------------- */
command++;
status  = FT_OK;
*pmin   = 0xffff;
*pmax   = 0;

while ( n )
    {
    status = ft_send_cmd_busy( command );
    if ( status != FT_OK )
        break;

    *dest = ft_read_data();

    /*
    -------------------------
    Mask off the upper 4 bits
    ------------------------- */
    *dest &= 0x0fff;

    if ( *dest > *pmax )
       *pmax = *dest;

    if ( *dest < *pmin )
       *pmin = *dest;

    dest++;
    command++;
    n--;
    }

return status;
}

/***********************************************************************
*                      LFILL_ANALOG_VS_POS_ARRAY                       *
*                                                                      *
*  Fill the passed array with position based analog data for the       *
*  channel selected.  Channel [1,4];                                   *
*                                                                      *
***********************************************************************/
int lfill_analog_vs_pos_array( BOARD_DATA * dest, BOARD_DATA * pmin, BOARD_DATA * pmax, short channel, short n )
{
static BOARD_DATA cmd_constant[5] = { 0, 4096, 6144, 8192, 10240 };

if ( channel < 1 || channel > MAX_FT_CHANNELS )
    return FT_ERROR;

return fill_analog_array( dest, pmin, pmax, cmd_constant[channel], n );
}

/***********************************************************************
*                      LFILL_ANALOG_VS_TIME_ARRAY                      *
***********************************************************************/
int lfill_analog_vs_time_array( BOARD_DATA * dest, BOARD_DATA * pmin, BOARD_DATA * pmax, short channel, short n )
{
static BOARD_DATA cmd_constant[5] = {0, 16384, 18432, 20480, 22528};

if ( channel < 1 || channel > MAX_FT_CHANNELS )
    return FT_ERROR;

return fill_analog_array( dest, pmin, pmax, cmd_constant[channel], n );
}

/***********************************************************************
*                             RD_CYCLE_TIM                             *
***********************************************************************/
int rd_cycle_tim( float * dest )
{

int status;
BOARD_DATA x;

*dest = 0.0;

status = ft_read_parameter( CYCLE_TIME, &x );
if ( status == FT_OK )
    *dest = seconds_from_count( x );

return status;
}

/***********************************************************************
*                           RD_TIME_INTERVAL                           *
*                                                                      *
*                       i = [0, USER_TIME_INT_COUNT)                   *
*                                                                      *
***********************************************************************/
int rd_time_interval( float * dest, int i )
{

static BOARD_DATA command[] = { TIM1, TIM2, TIM3, TIM4, TIM5 };
BOARD_DATA x;
int        status;

*dest   = 0.0;
status = FT_ERROR;

if ( i >= 0 && i < USER_TIME_INT_COUNT )
    {
    status = ft_read_parameter( command[i], &x );
    if ( status == FT_OK )
        *dest = seconds_from_count( x );
    }

return status;
}

/***********************************************************************
*                             RD_BISC_SIZE                             *
***********************************************************************/
int rd_bisc_size( float * dest )
{
int status;
BOARD_DATA x;

*dest = 0.0;

status = ft_read_parameter( BISC_SIZE, &x );
if ( status == FT_OK )
     *dest = dist_from_count( x );

return status;
}

/***********************************************************************
*                            READ_DISTANCE                             *
***********************************************************************/
static int read_distance( BOARD_DATA command, float * dest )
{
int        status;
BOARD_DATA x;

*dest = 0.0;

status = ft_read_parameter( command, &x );
if ( status == FT_OK )
    *dest = dist_from_count( x );

return status;
}

/***********************************************************************
*                            READ_MARK_DISTANCE                        *
***********************************************************************/
static int read_mark_distance( BOARD_DATA command, float * dest )
{
int        status;
BOARD_DATA x;

*dest = 0.0;

status = ft_read_parameter( command, &x );
if ( status == FT_OK )
    *dest = dist_from_marks( x );

return status;
}

/***********************************************************************
*                              RD_EOS_POS                              *
***********************************************************************/
int rd_eos_pos( float * dest )
{
return read_mark_distance( EOS_POS, dest );
}

/***********************************************************************
*                                RD_P1                                 *
***********************************************************************/
int rd_p1( float * dest )
{
return read_distance( P1_POS, dest );
}

/***********************************************************************
*                                RD_P2                                 *
***********************************************************************/
int rd_p2( float * dest )
{
return read_distance( P2_POS, dest );
}

/***********************************************************************
*                               RD_CSFS                                *
***********************************************************************/
int rd_csfs( float * dest )
{
return read_mark_distance( POS_CSFS, dest );
}

/***********************************************************************
*                           RD_TOTAL_STK_LEN                           *
***********************************************************************/
int rd_total_stk_len( float *dest )
{
return read_distance( TOT_STK_LEN, dest );
}

/***********************************************************************
*                            RD_MIN_STK_LEN                            *
***********************************************************************/
int rd_min_stk_len( float *dest )
{
return read_distance( MIN_STK_LEN, dest );
}

/***********************************************************************
*                             READ_SECONDS                             *
***********************************************************************/
static int read_seconds( BOARD_DATA command, float * dest )
{
int        status;
BOARD_DATA x;

*dest = 0.0;

status = ft_read_parameter( command, &x );
if ( status == FT_OK )
    *dest = seconds_from_count( x );

return status;
}

/***********************************************************************
*                           RD_TIM_DEL_BISC                            *
***********************************************************************/
int rd_tim_del_bisc( float * dest )
{
return read_seconds( TIM_DEL_BISC, dest );
}

/***********************************************************************
*                             READ_VELOCITY                            *
***********************************************************************/
static int read_velocity( BOARD_DATA command, float * dest )
{
int        status;
BOARD_DATA x;

*dest = 0.0;

status = ft_read_parameter( command, &x );
if ( status == FT_OK )
    {
    if ( x )
        *dest = utype->vel_dividend / (float) x;
    }

return status;
}

/***********************************************************************
*                              RD_VEL_EOS                              *
***********************************************************************/
int rd_vel_eos( float *dest )
{
return read_velocity( VEL_EOS, dest );
}

/***********************************************************************
*                            RD_MIN_VEL_EOS                            *
***********************************************************************/
int rd_min_vel_eos( float * dest )
{
return read_velocity( RD_MIN_VEL_EOS, dest );
}

/***********************************************************************
*                           RD_FILL_DISTANCE                           *
***********************************************************************/
int rd_fill_distance( float * dest )
{
return read_distance( RD_FILL_DIST, dest );
}

/***********************************************************************
*                       RD_RUN_FILL_DISTANCE                           *
***********************************************************************/
int rd_run_fill_distance( float * dest )
{
return read_distance( RD_RUN_FILL_DIST, dest );
}

/***********************************************************************
*                           RD_MIN_VEL_CSFS                            *
***********************************************************************/
int rd_min_vel_csfs( float * dest )
{
return read_velocity( RD_MIN_VEL_FS, dest );
}

/***********************************************************************
*                           RD_VEL_RISE_TIME                           *
***********************************************************************/
int rd_vel_rise_time( float * dest )
{
return read_velocity( RD_VEL_RISE_TIME, dest );
}

/***********************************************************************
*                             RD_POS_CSFS                              *
***********************************************************************/
int rd_pos_csfs( float * dest )
{
return read_distance( RD_L_MIN_CSFS, dest );
}

/***********************************************************************
*                         RD_TIM_COLL_INTERVAL                         *
***********************************************************************/
int rd_tim_coll_interval( float * dest )
{
int        status;
BOARD_DATA x;

*dest = 0.0;

status = ft_read_parameter( RD_TIM_COLL_INTERVAL, &x );
if ( status == FT_OK )
    {
    *dest = (float) x;
    *dest /= 10.0;       /* Convert to ms */
    }

return status;
}

/***********************************************************************
*                          RD_TIME_OUT_PERIOD                          *
***********************************************************************/
int rd_time_out_period( float * dest )
{
return read_seconds( RD_TIME_OUT_PERIOD, dest );
}

/***********************************************************************
*                            RD_LIMIT_SWITCH                           *
*                          Switch number [0,3].                        *
***********************************************************************/
int rd_limit_switch( short switch_number, float * dest )
{
static BOARD_DATA command[4] = { RD_LS1, RD_LS2, RD_LS3, RD_LS4 };

*dest = 0.0;

if ( switch_number>=0 && switch_number<4 )
    return read_distance( command[switch_number], dest );

return FALSE;
}

/***********************************************************************
*                             RD_RISE_TIME                             *
***********************************************************************/
int rd_rise_time( float * dest )
{
int status;
BOARD_DATA  x;

status = ft_read_parameter( RT_MILLI, &x );
if ( status == FT_OK )
    {
    *dest = (float) x;
    status = ft_read_parameter( RT_MICRO, &x );
    if ( status == FT_OK )
        *dest += float(x)/ 1000.0;
    }

return status;
}

