#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\ft.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\verrors.h"

#include "extern.h"
#include "boards.h"
#include "bid.h"
#include "resource.h"

static short BoardAtCurrentAddress = NO_BOARD;
static HARDWARE_BOARD_ENTRY Board[MAX_BOARDS];

static const TCHAR NoMachine[] = NO_MACHINE;

/*
--------------------------------
Pointer used by fastrak routines
-------------------------------- */
POS_SENSOR_SETUP_DATA * utype = 0;

ANALOG_SENSOR_SETUP_DATA * default_analog_sensor( void );
POS_SENSOR_SETUP_DATA    * find_pos_sensor( const TCHAR * rod_pitch, short dist_units, short vel_units, short mhz );

BOOLEAN append_downtime_record( TCHAR * computer, TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat );
void issue_downtime_event( TCHAR * computer, TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat );
void resource_message_box( UINT msg_id, UINT title_id );
TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                               WIREBIT                                *
***********************************************************************/
BITSETYPE wirebit( short wire_number )
{
BITSETYPE x;

x = 1;
wire_number--;
if ( wire_number > 0 )
    x <<= wire_number;

return  x;
}

/***********************************************************************
*                              CLEARWIRE                               *
*                          Wires from [1,16]                           *
***********************************************************************/
void HARDWARE_BOARD_ENTRY::clearwire( short wire_number )
{

BITSETYPE w;

w = wirebit( wire_number );

if ( dout_bits & w )
    {
    dout_bits &= ~w;
    need_dout_update = TRUE;
    }

}

/***********************************************************************
*                               SETWIRE                                *
*                          Wires from [1,16]                           *
***********************************************************************/
void HARDWARE_BOARD_ENTRY::setwire( short wire_number )
{

BITSETYPE w;

w = wirebit( wire_number );

if ( !(dout_bits & w) )
    {
    dout_bits |= w;
    need_dout_update = TRUE;
    }

}

/***********************************************************************
*                          NO_POSITION_SENSOR_DIALOG                   *
***********************************************************************/
BOOL CALLBACK no_position_sensor_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  i;
id  = LOWORD( wParam );
TEXT_LIST_CLASS * tp;

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

    case WM_INITDIALOG:
        tp = ( TEXT_LIST_CLASS *) lParam;
        tp->rewind();
        id = ROD_PITCH_TBOX;
        for ( i=0; i<4; i++ )
            {
            tp->next();
            set_text( hWnd, id, tp->text() );
            id++;
            }
        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           LOAD_SENSOR_POINTERS                       *
***********************************************************************/
BOOLEAN load_sensor_pointers( MUX_ENTRY & mux, short mhz )
{
static TEXT_LIST_CLASS tlist;
TCHAR rodpitch[RODPITCH_NAME_LEN+1];
short c;
short channel;
short sensor_number;

DB_TABLE t;
BOOLEAN status;

status = FALSE;
t.open( machset_dbname(ComputerName), MACHSET_RECLEN, PFL );
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, mux.machine, MACHINE_NAME_LEN );
t.reset_search_mode();
if ( t.get_next_key_match(1, NO_LOCK) )
    {
    t.get_alpha( rodpitch, MACHSET_RODPITCH_OFFSET, RODPITCH_NAME_LEN );
    status = TRUE;
    }
t.close();

if ( status )
    {
    status = FALSE;
    t.open( parts_dbname(ComputerName, mux.machine), PARTS_RECLEN, PFL );
    t.put_alpha( PARTS_PART_NAME_OFFSET, mux.part, PART_NAME_LEN );
    t.reset_search_mode();
    if ( t.get_next_key_match(1, NO_LOCK) )
        {
        mux.dist_units = t.get_short( PARTS_DISTANCE_UNITS_OFFSET );
        mux.vel_units  = t.get_short( PARTS_VELOCITY_UNITS_OFFSET );
        status = TRUE;
        }
    t.close();
    }

if ( status )
    {
    mux.psensor = find_pos_sensor( rodpitch, mux.dist_units, mux.vel_units, mhz );
    if ( !mux.psensor )
        {
        tlist.append( rodpitch );
        tlist.append( int32toasc((int32)mhz) );
        tlist.append( units_name(mux.dist_units) );
        tlist.append( units_name(mux.vel_units) );
        DialogBoxParam(
            MainInstance,
            TEXT("NO_POSITION_SENSOR_DIALOG"),
            MainWindow,
            no_position_sensor_dialog_proc,
            (LPARAM) &tlist
            );
        tlist.empty();
        status = FALSE;
        }
    }

if ( status )
    {
    /*
    --------------------------------------------------------------
    The list of channels is in the FTCHAN db in the part directory
    -------------------------------------------------------------- */
    t.open( ftchan_dbname(ComputerName, mux.machine, mux.part), FTCHAN_RECLEN, PFL );
    t.reset_search_mode();
    while ( t.get_next_record(NO_LOCK) )
        {
        channel       = t.get_short( FTCHAN_CHANNEL_NUMBER_OFFSET );
        sensor_number = t.get_short( FTCHAN_SENSOR_NUMBER_OFFSET );
        if ( channel > 0 && channel <= MAX_FT_CHANNELS )
            {
            channel--;
            mux.asensor[channel] = find_analog_sensor( sensor_number );
            if ( !mux.asensor[channel] )
                {
                error_message( LOAD_SENSOR_POINTERS, SENSOR_NOT_FOUND, channel );
                status = FALSE;
                }
            }
        }
    t.close();

    /*
    ------------------------------------------------------------------------------
    All channels must have a pointer, fill in any unused channels with the default
    ------------------------------------------------------------------------------ */
    for ( c=0; c<MAX_FT_CHANNELS; c++ )
        {
        if ( !mux.asensor[c] )
            {
            mux.asensor[c] = default_analog_sensor();
            if ( !mux.asensor[c] )
                {
                error_message( LOAD_SENSOR_POINTERS, SENSOR_NOT_FOUND, c );
                status = FALSE;
                }
            }
        }
    }

return status;
}

/***********************************************************************
*                        INIT_POS_SENSOR_POINTER                       *
***********************************************************************/
BOOLEAN init_pos_sensor_pointer( MUX_ENTRY & mux, short mhz )
{

BOOLEAN status;

status = load_sensor_pointers( mux, mhz );

if ( status )
    utype = mux.psensor;

return status;
}

/***********************************************************************
*                         SET_POS_SENSOR_POINTER                       *
***********************************************************************/
BOOLEAN BOARD_CLASS::set_pos_sensor_pointer( void )
{
short c;
HARDWARE_BOARD_ENTRY * b;
MUX_ENTRY            * m;

if ( currentboard >= 0 && currentboard < MAX_BOARDS )
    {
    b = Board + currentboard;
    c = b->current_mux - 1;
    if ( c >= 0 && c < MAX_MUX_CHANNELS )
        {
        m = b->mux + c;
        if ( m->psensor )
            {
            utype = m->psensor;
            return TRUE;
            }
        }
    }

return FALSE;
}

/***********************************************************************
*                           INIT_ONE_CHANNEL                           *
*              The ft board uses mux channels [1,8] = x+1.             *
*              Assume port is set, command mode.                       *
***********************************************************************/
BOOLEAN BOARD_CLASS::init_one_channel( short x )
{
int    bs;
short  firmware_version;
short  i;
short  n;
HARDWARE_BOARD_ENTRY * b;
BOARD_INITIALIZATION_DATA bid;

if ( currentboard < 0 || currentboard >= MAX_BOARDS )
    return FALSE;

b = Board + currentboard;

if ( !get_board_initialization_data(bid, ComputerName, b->mux[x].machine, b->mux[x].part) )
    return FALSE;

if ( !init_pos_sensor_pointer( b->mux[x], b->mhz) )
    return FALSE;

b->mux[x].ms_per_sample = bid.ms_per_time_based_sample;

if ( b->type & MUX_BOARD_TYPE && b->address != NO_BOARD_ADDRESS )
    ft_change_mux_channel( x+1 );

b->current_mux = x+1;

maxshort( bid.nof_time_samples, 1 );

/*
--------------------------------------------------------
If I can't talk to the board there is nothing else to do
-------------------------------------------------------- */
if ( b->address == NO_BOARD_ADDRESS )
    return FALSE;

bs = set_delay( bid.nof_time_samples );
if ( bs != FT_OK )
    {
    error_message( MSG_SET_DELAY, FT_BOARD_ERROR, (short) bs );
    resource_message_box( BOARD_NOT_STARTED_STRING, ERROR_IN_COMMUNICATION_STRING );
    return FALSE;
    }

bs = get_version( &firmware_version );
if ( bs != FT_OK )
     error_message( MSG_GET_VERSION, FT_BOARD_ERROR, (short) bs );

if ( firmware_version >= V4_FIRMWARE_VERSION )
    {
    bs = set_crystal_freq( float(b->mhz) );
    if ( bs != FT_OK )
         error_message( MSG_SET_CRYSTAL_FREQ, FT_BOARD_ERROR, (short) bs );

    bs = set_time_out_period( float(bid.auto_shot_timeout_seconds) );
    if ( bs != FT_OK )
         error_message( MSG_SET_TIME_OUT_PERIOD, FT_BOARD_ERROR, (short) bs );

    maxfloat( bid.ms_per_time_based_sample, MIN_MS_PER_TIME_SAMPLE );
    bs = set_tim_coll_interval( float(bid.ms_per_time_based_sample) );
    if ( bs != FT_OK )
         error_message( MSG_SET_TIM_COLL_INTERVAL, FT_BOARD_ERROR, (short) bs );
    }

bs = set_tot_stk_len( bid.total_stroke_len );
if ( bs != FT_OK )
     error_message( MSG_SET_TOT_STK_LEN, FT_BOARD_ERROR, (short) bs );

bs = set_min_stk_len( bid.min_stroke_len );
if ( bs != FT_OK )
     error_message( MSG_SET_MIN_STK_LEN, FT_BOARD_ERROR, (short) bs );

bs = set_tim_del_bisc( bid.biscuit_time_delay );
if ( bs != FT_OK )
     error_message( MSG_SET_TIM_DEL_BISC, FT_BOARD_ERROR, (short) bs );

bs = set_vel_eos( bid.eos_velocity );
if ( bs != FT_OK )
     error_message( MSG_SET_VEL_EOS, FT_BOARD_ERROR, (short) bs );

bs = set_fill_distance( bid.sleeve_fill_distance );
if ( bs != FT_OK )
     error_message( MSG_SET_FILL_DISTANCE, FT_BOARD_ERROR, (short) bs );

bs = set_run_fill_distance( bid.runner_fill_distance );
if ( bs != FT_OK )
     error_message( MSG_SET_RUN_FILL_DISTANCE, FT_BOARD_ERROR, (short) bs );

bs = set_min_vel_csfs( bid.min_csfs_vel );
if ( bs != FT_OK )
     error_message( MSG_SET_MIN_VEL_CSFS, FT_BOARD_ERROR, (short) bs );

bs = set_vel_rise_time( bid.rise_csfs_vel );
if ( bs != FT_OK )
     error_message( MSG_SET_VEL_RISE_TIME, FT_BOARD_ERROR, (short) bs );

bs = set_pos_csfs( bid.min_csfs_pos );
if ( bs != FT_OK )
     error_message( MSG_SET_POS_CSFS, FT_BOARD_ERROR, (short) bs );

for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
    {
    bs = set_limit_switch( i, bid.limits[i] );
    if ( bs != FT_OK )
         error_message( MSG_SET_LIM1_SETPT+i, FT_BOARD_ERROR, (short) bs );
    }

/*
-------------------------------------
Set the gain for each fastrak channel
------------------------------------- */
get_version( &n );
if ( n < FT_8_CHANEL_VERSION )
    n = MAX_CONTINUOUS_CHANNELS;
else
    n = MAX_FT_CHANNELS;

for ( i=0; i<n; i++ )
    ft_set_gain( i+1, b->mux[x].asensor[i]->gain );

bs = set_min_vel_eos( 0.0 );
if ( bs != FT_OK )
     error_message( MSG_SET_MIN_VEL_EOS, FT_BOARD_ERROR, (short) bs );

return TRUE;
}

/***********************************************************************
*                      BOARD_CLASS::INIT_ONE_BOARD                     *
***********************************************************************/
BOOLEAN BOARD_CLASS::init_one_board( void )
{

BOARD_DATA monitor_bits;
BOARD_DATA board_type;
short      n;
short      x;
int        bs;
int        ds;
HARDWARE_BOARD_ENTRY * b;
BOOLEAN    status;

status = TRUE;

if ( currentboard < 0 || currentboard >= MAX_BOARDS )
    return FALSE;

/*
---------------------------------------------
Make a pointer to the current board structure
--------------------------------------------- */
b = Board + currentboard;

set_port_addresses();
bs = ft_set_command_mode();
if ( bs != FT_OK )
    {
    STRING_CLASS s;
    s = resource_string( FT_BOARD_STRING );
    s += int32toasc( currentboard + 1 );
    s += resource_string( AT_ADDRESS_STRING );
    s += ultohex( (unsigned long) b->address );
    MessageBox( MainWindow, resource_string(COMMAND_MODE_ERR_STRING), s.text(), MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL );

    b->address = NO_BOARD_ADDRESS;
    b->status  = NO_BOARD_STATUS;
    b->state   = NO_BOARD_STATE;

    status = FALSE;
    return status;
    }

/*
-----------------------------------
Set the board to single or mux type
----------------------------------- */
n            = 0;
monitor_bits = 0;
board_type   = FT_SINGLE_MACHINE_TYPE;

if ( b->type & MUX_BOARD_TYPE )
    board_type = FT_MUX_MACHINE_TYPE;

if ( b->address != NO_BOARD_ADDRESS )
    {
    bs = ft_set_type( board_type );
    if ( bs != FT_OK )
        {
        error_message( MSG_FT_SET_TYPE, FT_BOARD_ERROR, (short) bs );

        STRING_CLASS s;

        s = resource_string( FT_BOARD_STRING );
        s += int32toasc( currentboard + 1 );
        s += resource_string( AT_ADDRESS_STRING );
        s += ultohex( (unsigned long) b->address );

        MessageBox( MainWindow, resource_string(BOARD_TYPE_ERR_STRING), s.text(), MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL );

        b->address = NO_BOARD_ADDRESS;
        b->status  = NO_BOARD_STATUS;
        b->state   = NO_BOARD_STATE;

        status = FALSE;
        return status;
        }
    }

b->dout_bits        = 0;
b->need_dout_update = TRUE;

/*
--------------------------------------
Count the mux channels to be monitored
-------------------------------------- */
for ( x=0; x<MAX_MUX_CHANNELS; x++ )
    {
    if ( lstrcmp(b->mux[x].machine, NoMachine) != 0 )
        {
        monitor_bits |= (1 << x);
        n++;
        if ( !init_one_channel(x) )
            {
            status = FALSE;
            }

        /*
        ------------------------------------------------------------------------
        Turn on the monitor wire if the machine is up or if there is no downtime
        ------------------------------------------------------------------------ */
        if ( MonitorWire != NO_WIRE )
            {
            ds = b->mux[x].down_state;
            if ( !ClearMonitorWireOnDown || ds == PROG_UP_STATE || ds == MACH_UP_STATE || ds == NO_DOWN_STATE )
                b->dout_bits |= wirebit( MonitorWire );
            }
        }

    if ( board_type == FT_SINGLE_MACHINE_TYPE )
        break;
    }

b->state &= ~(MONITORING_STATE | SINGLE_SCAN_STATE);

if ( n > 0 )
    {
    b->state |= MONITORING_STATE;
    b->state &= ~SINGLE_SCAN_STATE;

    if ( board_type == FT_MUX_MACHINE_TYPE )
        {
        if ( b->address != NO_BOARD_ADDRESS )
            set_mach_mask( monitor_bits );
        if ( n == 1 )
            b->state |= SINGLE_SCAN_STATE;
        }
    }

if ( !status )
    {
    STRING_CLASS s;
    STRING_CLASS s1;
    s = resource_string( FT_BOARD_STRING );
    s += TEXT( " " );
    s += int32toasc( currentboard +1  );
    s1 = resource_string( NO_BOARD_INIT_STRING );
    MessageBox( MainWindow, s1.text(), s.text(), MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
    }

return status;
}

/***********************************************************************
*                     BOARD_CLASS::INIT_ALL_BOARDS                     *
***********************************************************************/
BOOLEAN BOARD_CLASS::init_all_boards( void )
{

short i;
BOOLEAN status;

status = TRUE;
for ( i=0; i<MAX_BOARDS; i++ )
    {
    if ( Board[i].address != NO_BOARD_ADDRESS )
        {
        currentboard = i;
        if ( !init_one_board() )
            status = FALSE;
        }
    }

return status;
}

/***********************************************************************
*                             LOAD_BOARDS                              *
***********************************************************************/
static void load_boards( TCHAR * computer_name )
{

DB_TABLE t;
short c;
short i;
short x;

/*
----------------------------
Initialize all of the boards
---------------------------- */
for ( i=0; i<MAX_BOARDS; i++ )
    {
    Board[i].address = NO_BOARD_ADDRESS;
    Board[i].status  = NO_BOARD_STATUS;
    for ( x=0; x<MAX_MUX_CHANNELS; x++ )
        {
        lstrcpy( Board[i].mux[x].machine, NoMachine );
        Board[i].mux[x].dist_units            = NO_UNITS;
        Board[i].mux[x].vel_units             = NO_UNITS;
        Board[i].mux[x].psensor               = 0;
        Board[i].mux[x].ms_per_sample         = 1;
        Board[i].mux[x].down_state            = NO_DOWN_STATE;
        Board[i].mux[x].cycle_timeout_seconds = 0;
        Board[i].mux[x].time_of_last_shot     = 0;
        for ( c=0; c<MAX_FT_CHANNELS; c++ )
            Board[i].mux[x].asensor[c] = 0;
        }
    }

/*
-----------------------------------------
Fill in the data from the boards database
----------------------------------------- */
t.open( boards_dbname(computer_name), BOARDS_RECLEN, PFL );
while ( t.get_next_record(FALSE) )
    {
    i = t.get_short( BOARDS_NUMBER_OFFSET );
    i--;
    if ( i >= 0 && i < MAX_BOARDS )
        {
        Board[i].address = (unsigned short) t.get_short( BOARDS_ADDRESS_OFFSET );
        Board[i].type    = (BITSETYPE)      t.get_long(  BOARDS_TYPE_OFFSET );
        Board[i].mhz     =                  t.get_short( BOARDS_MHZ_OFFSET );
        Board[i].status  = NO_BOARD_STATUS;
        Board[i].state   = NO_BOARD_STATE;
        Board[i].dout_bits = 0;
        Board[i].need_dout_update = FALSE;
        Board[i].cycles_per_time_count = t.get_short( BOARDS_CYCLES_PER_TIME_COUNT_OFFSET );
        Board[i].counts_per_pos_mark   = t.get_short( BOARDS_COUNTS_PER_POS_MARK_OFFSET );
        Board[i].current_mux           = NO_MUX_CHANNEL;
        Board[i].monitoring            = FALSE;
        }
    }
t.close();

}

/***********************************************************************
*                          BOARD_CLASS::BOARD                          *
***********************************************************************/
HARDWARE_BOARD_ENTRY * BOARD_CLASS::board( void )
{
if ( currentboard >= 0 && currentboard < MAX_BOARDS )
    return Board + currentboard;

return 0;
}

/***********************************************************************
*                     BOARD_CLASS::CURRENT_MUX_ENTRY                   *
***********************************************************************/
MUX_ENTRY * BOARD_CLASS::current_mux_entry( void )
{
short                  c;
MUX_ENTRY            * me;
HARDWARE_BOARD_ENTRY * hb;

me = 0;

hb = board();
if ( hb )
    {
    c = hb->current_mux - 1;
    if ( c >= 0 && c < MAX_MUX_CHANNELS )
        me = hb->mux + c;
    }

return me;
}

/***********************************************************************
*                         BOARD_CLASS::MONITORING                      *
***********************************************************************/
BOOLEAN BOARD_CLASS::monitoring( void )
{
if ( currentboard >= 0 && currentboard < MAX_BOARDS )
    return Board[currentboard].monitoring;

return FALSE;
}


/***********************************************************************
*                         CURRENT_MUX_CHANNEL                          *
***********************************************************************/
short BOARD_CLASS::current_mux_channel( void )
{
if ( currentboard >= 0 && currentboard < MAX_BOARDS )
    return Board[currentboard].current_mux;

return NO_MUX_CHANNEL;
}

/***********************************************************************
*                       READ_CURRENT_MUX_CHANNEL                       *
*             This assumes the ports have already been set.            *
***********************************************************************/
short BOARD_CLASS::read_current_mux_channel( void )
{

short c;
BOARD_DATA current_board_status;

if ( currentboard >= 0 && currentboard < MAX_BOARDS )
    {
    if ( Board[currentboard].type & SINGLE_MACHINE_BOARD_TYPE )
        return SINGLE_MACHINE_MUX_CHAN;

    current_board_status = ft_read_status();
    if ( current_board_status & VIS_BUSY )
        {
        /*
        -------------------------------------------------------
        If the board is in scan mode, Greg puts the current mux
        channel in the data register. Otherwise I'm monitoring
        just one machine and the current mux is constant.
        ------------------------------------------------------- */
        if ( current_board_status & SCAN_MODE )
            c = (short) ft_read_data();
        else
            c = Board[currentboard].current_mux;
        }
    else if ( ft_rd_mux_channel(&c) != FT_OK )
        c = NO_MUX_CHANNEL;

    if ( c <= 0 || c > MAX_MUX_CHANNELS )
        c = NO_MUX_CHANNEL;

    if ( c != NO_MUX_CHANNEL )
        {
        Board[currentboard].current_mux = c;
        return c;
        }
    }

return NO_MUX_CHANNEL;
}

/***********************************************************************
*                      BOARD_CLASS::SET_MONITORING                     *
***********************************************************************/
BOOLEAN BOARD_CLASS::set_monitoring( BOOLEAN new_monitoring_state )
{
if ( currentboard >= 0 && currentboard < MAX_BOARDS )
    {
    /*
    --------------------------------------------------------
    Make sure there is a board if I am turning monitoring on
    -------------------------------------------------------- */
    if ( new_monitoring_state )
        if ( Board[currentboard].address == NO_BOARD_ADDRESS )
            return FALSE;
    Board[currentboard].monitoring = new_monitoring_state;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         BOARD_CLASS::STARTUP                         *
***********************************************************************/
BOOLEAN BOARD_CLASS::startup( void )
{
load_boards( ComputerName );
return TRUE;
}

/***********************************************************************
*                        BOARD_CLASS::SHUTDOWN                         *
***********************************************************************/
void BOARD_CLASS::shutdown( void )
{

short i;
short c;
MUX_ENTRY * me;
SYSTEMTIME  st;
DSTAT_CLASS d;

for ( i=0; i<MAX_BOARDS; i++ )
    {
    if ( Board[i].state != NO_BOARD_STATE )
        {
        if ( Board[i].address != NO_BOARD_ADDRESS )
            {
            currentboard = i;
            set_port_addresses();
            ft_set_command_mode();
            wr_opto( 0 );
            }

        for ( c=0; c<MAX_MUX_CHANNELS; c++ )
            {
            me = &Board[i].mux[c];
            if ( lstrcmp(me->machine, NoMachine) != 0 )
                {
                if ( me->down_state == PROG_UP_STATE || me->down_state == MACH_UP_STATE || me->down_state == NO_DOWN_STATE || me->cycle_timeout_seconds == 0 )
                    {
                    d.set_cat( DOWNCAT_SYSTEM_CAT );
                    d.set_subcat( DOWNCAT_PROG_EXIT_SUBCAT );
                    GetLocalTime(&st);
                    d.set_time( st );
                    d.put( ComputerName, me->machine );
                    me->down_state = d.down_state();
                    if ( me->cycle_timeout_seconds > 0 )
                        append_downtime_record( ComputerName, me->machine, me->part, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
                    else
                        issue_downtime_event( ComputerName, me->machine, me->part, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
                    }
                }
            }
        }
    }

}

/***********************************************************************
*                          BOARD_CLASS::FIND                           *
* For everyone else, boards go from 1 to 8. For me they go from 0 to 7.*
***********************************************************************/
BOOLEAN BOARD_CLASS::find( short board_number )
{
short i;

i = board_number;
i--;
if ( i >= 0 && i < MAX_BOARDS )
    {
    if ( Board[i].address != NO_BOARD_ADDRESS )
        {
        currentboard = i;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                          BOARD_CLASS::NEXT                           *
***********************************************************************/
BOOLEAN BOARD_CLASS::next( void )
{
short i;

i = currentboard;

while ( TRUE )
    {
    i++;
    if ( i >= MAX_BOARDS )
        {
        /*
        -----------------------------------------------------------
        If I started at -1 then there are no boards at this machine
        ----------------------------------------------------------- */
        if ( currentboard == NO_BOARD )
            break;

        /*
        --------------------------------------
        Otherwise start again at the 0th board
        -------------------------------------- */
        i = 0;
        }

    if ( Board[i].address != NO_BOARD_ADDRESS )
        {
        currentboard = i;
        return TRUE;
        }

    if ( i == currentboard )
        break;
    }

return FALSE;
}

/***********************************************************************
*                   BOARD_CLASS::SET_PORT_ADDRESSES                    *
***********************************************************************/
void BOARD_CLASS::set_port_addresses( void )
{

if ( currentboard != NO_BOARD )
    if ( currentboard != BoardAtCurrentAddress )
        {
        BoardAtCurrentAddress = currentboard;
        ft_setports( Board[currentboard].address );
        }
}

/***********************************************************************
*                       BOARD_CLASS::ADDRESS                           *
***********************************************************************/
BOARD_DATA BOARD_CLASS::address( void )
{
if ( currentboard >= 0 && currentboard < MAX_BOARDS )
    return Board[currentboard].address;

return NO_BOARD_ADDRESS;
}

/***********************************************************************
*                         BOARD_CLASS::NUMBER                          *
***********************************************************************/
short BOARD_CLASS::number( void )
{
return currentboard + 1;
}

/***********************************************************************
*                         BOARD_CLASS::STATUS                          *
***********************************************************************/
BOARD_DATA BOARD_CLASS::status( void )
{
if ( currentboard >= 0 && currentboard < MAX_BOARDS )
    return Board[currentboard].status;

return NO_BOARD_STATUS;
}

/***********************************************************************
*                      BOARD_CLASS::SET_STATUS                         *
***********************************************************************/
void BOARD_CLASS::set_status( BOARD_DATA new_status )
{
if ( currentboard >= 0 && currentboard < MAX_BOARDS )
    Board[currentboard].status = new_status;
}

/***********************************************************************
*                       BOARD_CLASS::SET_MACHINE                       *
***********************************************************************/
void BOARD_CLASS::set_machine( short mux_channel, TCHAR * machine_name, TCHAR * part_name, long new_cycle_timeout_seconds )
{
HARDWARE_BOARD_ENTRY * b;
DB_TABLE    t;
MUX_ENTRY * me;
SYSTEMTIME  st;
DSTAT_CLASS d;

if ( currentboard < 0 || currentboard >= MAX_BOARDS )
    return;

b = Board + currentboard;

mux_channel--;
if ( mux_channel < 0 || mux_channel >= MAX_MUX_CHANNELS )
    return;

me = &b->mux[mux_channel];

lstrcpyn( me->machine, machine_name, MACHINE_NAME_LEN+1 );
lstrcpyn( me->part,    part_name,    PART_NAME_LEN+1 );

if ( lstrcmp(me->machine, NoMachine) != 0 && lstrcmp(me->part, NO_PART) != 0 )
    {
    GetLocalTime(&st);

    d.get( ComputerName, machine_name );
    if ( d.down_state() == PROG_DOWN_STATE || d.down_state() == NO_DOWN_STATE || new_cycle_timeout_seconds == 0 )
        {
        /*
        -------------------------------------------
        Replace the program down with program start
        ------------------------------------------- */
        d.set_cat( DOWNCAT_SYSTEM_CAT );
        d.set_subcat( DOWNCAT_PROG_START_SUBCAT );
        d.set_time( st );
        d.put( ComputerName, machine_name );
        if ( new_cycle_timeout_seconds > 0 )
            append_downtime_record( ComputerName, machine_name, part_name, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_START_SUBCAT );
        else
            issue_downtime_event( ComputerName, machine_name, part_name, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_START_SUBCAT );

        /*
        ----------------------------------------------------------------
        The machine was up when I left so give it a chance for one cycle
        ---------------------------------------------------------------- */
        }

    me->time_of_last_shot = sec_since1990( st );
    me->down_state            = d.down_state();
    me->cycle_timeout_seconds = new_cycle_timeout_seconds;
    }
}

/***********************************************************************
*                       BOARD_CLASS::SET_MACHINE                       *
***********************************************************************/
void BOARD_CLASS::set_machine( short mux_channel, TCHAR * machine_name, TCHAR * part_name )
{

set_machine( mux_channel, machine_name, part_name, 0 );

}

/***********************************************************************
*                      BOARD_CLASS::COMMAND_MODE                       *
***********************************************************************/
BOOLEAN BOARD_CLASS::command_mode( void )
{
set_port_addresses();

if ( ft_set_command_mode() == FT_OK )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                 BOARD_CLASS::DATA_COLLECTION_MODE                    *
***********************************************************************/
BOOLEAN BOARD_CLASS::data_collection_mode( void )
{
bool need_scan_mode;
short err;

HARDWARE_BOARD_ENTRY * b;

if ( currentboard < 0 || currentboard >= MAX_BOARDS )
    return FALSE;

set_port_addresses();

b = Board + currentboard;

if ( b->need_dout_update )
    {
    if ( wr_opto(b->dout_bits) == FT_OK )
        b->need_dout_update = FALSE;
    }

need_scan_mode = false;

if ( b->type & MUX_BOARD_TYPE && !(b->state & SINGLE_SCAN_STATE) )
    {
    /*
    -----------------------------------------------------------------------------------------
    I still want data collection mode if this machine is in alarm and the LockOnAlarm is true
    ----------------------------------------------------------------------------------------- */
    if ( !(b->dout_bits & LockOnAlarmMask) )
        need_scan_mode = true;
    }

if ( need_scan_mode )
    {
    err = ft_set_scan_mode();
    }
else
    {
    err = ft_set_data_coll_mode();
    }

if ( err == FT_OK )
    return TRUE;

error_message( DATA_COLLECTION_MODE, FT_BOARD_ERROR, err );
return FALSE;
}

/***********************************************************************
*                    BOARD_CLASS::START_ALL_BOARDS                     *
***********************************************************************/
BOOLEAN BOARD_CLASS::start_all_boards( void )
{

short i;

for ( i=0; i<MAX_BOARDS; i++ )
    {
    if ( Board[i].state & MONITORING_STATE )
        {
        currentboard = i;
        Board[i].monitoring = TRUE;
        data_collection_mode();
        }
    }

return TRUE;
}

/***********************************************************************
*                     BOARD_CLASS::START_ONE_BOARD                     *
***********************************************************************/
BOOLEAN BOARD_CLASS::start_one_board( void )
{

if ( Board[currentboard].state & MONITORING_STATE )
    data_collection_mode();

return TRUE;
}

/***********************************************************************
*                    BOARD_CLASS::STOP_ALL_BOARDS                      *
***********************************************************************/
BOOLEAN BOARD_CLASS::stop_all_boards( void )
{

short i;

for ( i=0; i<MAX_BOARDS; i++ )
    {
    if ( Board[i].state & MONITORING_STATE )
        {
        currentboard = i;
        command_mode();
        }
    }

return TRUE;
}

/***********************************************************************
*                         BOARD_CLASS::LOCATE                          *
*                                                                      *
*  Locate the board and channel that a machine is on. Do NOT move      *
*  to that board, just locate it.                                      *
*                                                                      *
***********************************************************************/
BOOLEAN BOARD_CLASS::locate( short & board_number, short & channel_number, TCHAR * machine_to_locate )
{
short i;
short j;

for ( i=0; i<MAX_BOARDS; i++ )
    for ( j=0; j<MAX_MUX_CHANNELS; j++ )
        if ( compare( Board[i].mux[j].machine, machine_to_locate, MACHINE_NAME_LEN) == 0 )
            {
            board_number   = i+1;
            channel_number = j+1;
            return TRUE;
            }

return FALSE;
}

/***********************************************************************
*                           REFRESH_DOWNTIME_STATE                     *
***********************************************************************/
void BOARD_CLASS::refresh_downtime_state( DOWN_DATA_CLASS & dd )
{
short       i;
short       j;
int         ds;
DSTAT_CLASS d;
MUX_ENTRY * me;
BOOLEAN     was_up;
BOOLEAN     is_up;

for ( i=0; i<MAX_BOARDS; i++ )
    {
    for ( j=0; j<MAX_MUX_CHANNELS; j++ )
        {
        me = &Board[i].mux[j];
        if ( strings_are_equal( me->machine, dd.machine_name(), MACHINE_NAME_LEN) )
            {
            d.set_cat( dd.category() );
            d.set_subcat( dd.subcategory() );

            if ( me->cycle_timeout_seconds > 0 )
                {
                ds = me->down_state;
                if ( ds == PROG_UP_STATE || ds == MACH_UP_STATE || ds == NO_DOWN_STATE )
                    was_up = TRUE;
                else
                    was_up = FALSE;

                ds = d.down_state();
                if ( ds == PROG_UP_STATE || ds == MACH_UP_STATE || ds == NO_DOWN_STATE )
                    is_up = TRUE;
                else
                    is_up = FALSE;

                if ( DownTimeAlarmWire != NO_WIRE )
                    {
                    if ( was_up && !is_up )
                        {
                        Board[i].setwire( DownTimeAlarmWire );
                        }
                    else if ( is_up && !was_up )
                        {
                        Board[i].clearwire( DownTimeAlarmWire );
                        }
                    else if ( !is_up && !was_up && ClearDownWireFromPopup && ds == HUMAN_DOWN_STATE )
                        {
                        Board[i].clearwire( DownTimeAlarmWire );
                        }
                    }

                if ( MonitorWire != NO_WIRE && ClearMonitorWireOnDown )
                    {
                    /*
                    --------------------------------------------
                    See if the MonitorWire needs to change state
                    -------------------------------------------- */
                    if ( was_up && !is_up )
                        Board[i].clearwire( MonitorWire );
                    else if ( is_up && !was_up )
                        Board[i].setwire( MonitorWire );
                    }
                }

            me->down_state = d.down_state();
            return;
            }
        }
    }

}

/***********************************************************************
*                               NEXT_MACHINE                           *
***********************************************************************/
TCHAR * BOARD_CLASS::next_machine( void )
{
short i;
short c;
TCHAR * cp;

i = machine_list_board;
c = machine_list_mux;

if ( i == NO_BOARD )
    {
    i = 0;
    c = 0;
    }
else
    {
    c++;
    }

while ( TRUE )
    {
    if ( c >= MAX_MUX_CHANNELS )
        {
        i++;
        c = 0;
        }

    if ( i >= MAX_BOARDS )
        return 0;

    if ( Board[i].address == NO_BOARD_ADDRESS )
        return 0;

    cp = Board[i].mux[c].machine;

    if ( lstrcmp(cp, NoMachine) != 0 )
        {
        machine_list_board = i;
        machine_list_mux   = c;
        return cp;
        }
    else
        c++;
    }
}
