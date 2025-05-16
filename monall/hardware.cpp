#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\bitclass.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\events.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\param.h"
#include "..\include\stddown.h"
#include "..\include\structs.h"
#include "..\include\stringcl.h"
#include "..\include\ringcl.h"
#include "..\include\ft.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "extern.h"
#include "boards.h"

static STDDOWN_CLASS StdDown;

#define CANCEL_BUTTON             2
#define LOW_BIT_RADIO_BUTTON    110
#define HIGH_BIT_RADIO_BUTTON   265
#define LOW_MUX_CHANNEL_TEXT    300

const DWORD MS_TO_WAIT = 255;

extern BIT_CLASS Channel_A_Bits;
extern BIT_CLASS Channel_B_Bits;

extern STRING_CLASS CsvFileName;
extern int          CsvChannelIndex;
extern BOOLEAN      CsvEnable;

static TCHAR AutoShotMachineName[MACHINE_NAME_LEN+1] = NO_MACHINE;

static const TCHAR EmptyString[]        = TEXT( "" );
static const TCHAR NullChar             = TEXT( '\0' );
static const TCHAR CommaChar            = TEXT( ',' );
static const TCHAR DelimitChar          = TEXT( '\t' );
static const TCHAR PeriodChar           = TEXT( '.' );
static const TCHAR SpaceChar            = TEXT( ' ' );
static const TCHAR YChar                = TEXT( 'Y' );
static const TCHAR RightBracketString[] = TEXT( "]" );
static const TCHAR ConfigSection[]      = TEXT( "Config" );
static const TCHAR MonallIniFile[]      = TEXT( "monall.ini" );
static const TCHAR LockOnAlarmKey[]     = TEXT( "LockOnAlarm" );

double     RealMaxPosition                = 0.0;
short      MaxPosition                    = 0;
BOARD_DATA CurrentAnalog[MAX_FT_CHANNELS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
BOARD_DATA CurrentVelocity                = 0xFFFF;
BOARD_DATA CurrentPosition                = 0;
/*
-------------------------------------------
The following are for the thermometers only
------------------------------------------- */
BOARD_DATA MaxAnalog[MAX_FT_CHANNELS]     = { 0, 0, 0, 0, 0, 0, 0, 0 };
BOARD_DATA MaxVelocity                    = 0;
BOARD_DATA UMaxPosition                   = 0;

static short LastBoard = NO_BOARD_NUMBER;
static short LastMux   = NO_MUX_CHANNEL;

void get_shot_data( HARDWARE_BOARD_ENTRY * b );
void reset_machine_for_getdata( TCHAR * machine_to_find );
BOOLEAN append_downtime_record( TCHAR * computer, TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat );

struct HMACHINE_ENTRY
    {
    short board_number;
    short mux_channel;
    TCHAR name[MACHINE_NAME_LEN+1];
    TCHAR part[PART_NAME_LEN+1];
    BITSETYPE monitor_flags;
    long  downtime_timeout_seconds;
    };

/*
-----------------------------------------------------
List of buttons that are only on when in command mode
----------------------------------------------------- */
const UINT CommandModeButton[] = {
        READ_SHOT_PARAM_BUTTON,
        SET_DATA_COLL_MODE_BUTTON,
        XDCR_TEST_BUTTON,
        GET_PROM_VERSION_BUTTON,
        READ_CURRENT_VALUES_BUTTON,
        SETUP_PARAMS_BUTTON,
        COMMAND_AND_DATA_PORTS_BUTTON,
        CLEAR_ERROR_STATUS_BUTTON,
        CLEAR_DATA_READY_BUTTON,
        TEST_RAM_BUTTON,
        TEST_OPTO_BUTTON,
        TEST_IO_BUTTON
        };

const int32 NofCommandModeButtons = sizeof(CommandModeButton)/sizeof(UINT);
const int32 NofAlwaysOkButtons = 6;

/*
-----------------------------------------------------
List of buttons that are only on for multiplex boards
----------------------------------------------------- */
const UINT MultiplexButton[] = {
        LATCH_SHOT_COUNTERS_BUTTON,
        CLEAR_SHOT_COUNTERS_BUTTON,
        SET_SCAN_MODE_BUTTON
        };

const int32 NofMultiplexButtons = sizeof(MultiplexButton)/sizeof(UINT);

TCHAR * resource_string( UINT resource_id );
void resource_message_box( UINT msg_id, UINT title_id );
void udp_socket_startup();
void udp_socket_shutdown();

/***********************************************************************
*                         GET_EOS_UDP_ADDRESS                          *
***********************************************************************/
BOOLEAN get_eos_udp_address( sockaddr_in & dest,  TCHAR * computer, TCHAR * machine )
{
STRING_CLASS s;
FILE_CLASS   f;
BOOLEAN      status;
char         cbuf[TCP_ADDRESS_LEN+1];

status = FALSE;

dest.sin_family           = PF_INET;

s = eos_udp_address_name( computer, machine );
if ( file_exists(s.text()) )
    {
    f.open_for_read( s.text() );
    s = f.readline();
    if ( countchars(PeriodChar, s.text()) == 3 )
        {
        unicode_to_char( cbuf, s.text() );
        dest.sin_addr.S_un.S_addr = inet_addr( cbuf );
        s = f.readline();
        if ( !s.isempty() )
            {
            dest.sin_port = htons( (u_short) s.uint_value() );
            status = TRUE;
            }
        }
    f.close();
    }

return status;
}

/***********************************************************************
*                                VOLTS                                 *
***********************************************************************/
static TCHAR * volts( BOARD_DATA x )
{
static TCHAR buf[MAX_DOUBLE_LEN+1];
const double TEN     =    10.0;
const double DIVISOR = 16383.0;

double y;

y = double( x );
y *= TEN;
y /= DIVISOR;

double_to_tchar( buf, y, 2 );

return buf;
}

/***********************************************************************
*                        SHOW_COMMAND_MODE_MESSAGE                     *
***********************************************************************/
static void show_command_mode_message( void )
{
resource_message_box( NEED_CMD_MODE_STRING, CANT_COMPLY_STRING );
}

/***********************************************************************
*                          SHOW_MUX_ONLY_MESSAGE                       *
***********************************************************************/
static void show_mux_only_message( void )
{
resource_message_box( MUX_BOARD_ONLY_STRING, CANT_COMPLY_STRING );
}

/***********************************************************************
*                              SHOW_BITS                               *
***********************************************************************/
static void show_bits( HWND w, UINT radio_button_id, BOARD_DATA status, BOOLEAN with_enable )
{
short  i;
int    cmd;
HWND   bw;
WPARAM state;

for ( i=0; i<16; i++ )
    {
    if ( status & 0x0001 )
        {
        state = 1;
        cmd   = SW_SHOW;
        }
    else
        {
        state = 0;
        cmd   = SW_HIDE;
        }

    if ( Ioex4Window == w || HardwareStatusWindow == w )
        {
        bw = GetDlgItem( w, radio_button_id );
        if ( bw )
            ShowWindow( bw, cmd );
        }
    else
        {
        SendDlgItemMessage( w, radio_button_id, BM_SETCHECK, state, 0L );
        if ( with_enable )
            EnableWindow( GetDlgItem(w, radio_button_id), (BOOL) state );
        }
    radio_button_id++;
    status >>= 1;
    }

}

/***********************************************************************
*                              SHOW_BITS                               *
***********************************************************************/
static void show_bits( BOARD_CLASS & b )
{
short board_index;
short radio_button_id;
int   c;
int   mux_channel_text_id;

board_index = b.index();

if ( board_index >= 0 && board_index < MAX_BOARDS )
    {
    radio_button_id  = LOW_BIT_RADIO_BUTTON + board_index*20;
    show_bits( HardwareStatusWindow, radio_button_id, b.status(), FALSE );

    c = b.current_mux_channel();
    mux_channel_text_id = LOW_MUX_CHANNEL_TEXT + board_index;
    SetDlgItemInt( HardwareStatusWindow, mux_channel_text_id, c, FALSE );
    }

}

/***********************************************************************
*                      INIT_STATUS_BIT_RECTANGLES                      *
***********************************************************************/
static void init_status_bit_rectangles()
{
int   id;
int   mux_tbox_id;
int   board_index;
int   i;

mux_tbox_id = LOW_MUX_CHANNEL_TEXT;
for ( board_index=0; board_index<MAX_BOARDS; board_index++ )
    {
    set_text( HardwareStatusWindow, mux_tbox_id++, EmptyString );
    id  = LOW_BIT_RADIO_BUTTON + board_index*20;
    for ( i=0; i<16; i++ )
        ShowWindow( GetDlgItem(HardwareStatusWindow, id++), SW_HIDE );
    }

}

/***********************************************************************
*                           LOAD_ONE_MACHINE                           *
*                                                                      *
* This assumes the table is open and I am sitting on the correct rec.  *
*                                                                      *
***********************************************************************/
BOOLEAN load_one_machine( HMACHINE_ENTRY & m, DB_TABLE & t )
{
TCHAR sn[MAX_PATH+1];
DB_TABLE s;

m.monitor_flags = BITSETYPE( t.get_long(MACHSET_MONITOR_FLAGS_OFFSET) );
m.board_number  = t.get_short( MACHSET_FT_BOARD_NO_OFFSET );
t.get_alpha( m.name, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
t.get_alpha( m.part, MACHSET_CURRENT_PART_OFFSET, PART_NAME_LEN );
m.mux_channel = t.get_short( MACHSET_MUX_CHAN_OFFSET );
m.downtime_timeout_seconds = t.get_long( MACHSET_CYCLE_TIMEOUT_OFFSET );

/*
-------------------------------------------------
Make sure there is a semaphor db for this machine
------------------------------------------------- */
if ( m.monitor_flags & MA_MONITORING_ON )
    {
    copystring( sn, madir_name(ComputerName, m.name, SEMAPHOR_DB) );
    if ( !file_exists(sn) )
        s.create( sn );
    }

return TRUE;
}

/***********************************************************************
*                            LOAD_MACHINES                             *
***********************************************************************/
void load_machines( void )
{

DB_TABLE t;

HMACHINE_ENTRY m;
BOARD_CLASS b;
sockaddr_in address;

t.open( machset_dbname(ComputerName), MACHSET_RECLEN, PFL );
while ( t.get_next_record(FALSE) )
    {
    load_one_machine( m, t );
    if ( m.monitor_flags & MA_MONITORING_ON )
        {
        if ( b.find(m.board_number) )
            b.set_machine( m.mux_channel, m.name, m.part, m.downtime_timeout_seconds );
        /*
        ---------------------------------------------------
        See if there is a udp stamp machine at this address
        --------------------------------------------------- */
        if ( get_eos_udp_address(address, ComputerName, m.name) )
            HaveEosUdp = TRUE;
        }
    }


t.close();
}

/***********************************************************************
*                            SHOW_AUTO_SHOT                            *
***********************************************************************/
static void show_auto_shot( BOARD_CLASS & bc, short c )
{

HARDWARE_BOARD_ENTRY * b;

/*
-------------------------------
The mux channel should be [1,8]
------------------------------- */
if ( c < 1 || c > MAX_MUX_CHANNELS )
    return;
c--;

b = bc.board();
if ( b )
    {
    lstrcpyn( AutoShotMachineName, b->mux[c].machine, MACHINE_NAME_LEN+1 );
    if ( AutoShotDisplayWindow )
        {
        SetDlgItemText(AutoShotDisplayWindow, AUTO_SHOT_MACHINE_NUMBER_TEXTBOX, AutoShotMachineName );
        ShowWindow( AutoShotDisplayWindow, SW_SHOW );
        }

    }

}

/***********************************************************************
*                            HIDE_AUTO_SHOT                            *
***********************************************************************/
static void hide_auto_shot( BOARD_CLASS & bc, short c )
{

HARDWARE_BOARD_ENTRY * b;

b = bc.board();
if ( b )
    {
    if ( lstrcmp(AutoShotMachineName, b->mux[c].machine) )
        {
        lstrcpy( AutoShotMachineName, NO_MACHINE );
        if ( AutoShotDisplayWindow )
            {
            ShowWindow( AutoShotDisplayWindow, SW_HIDE );
            SetDlgItemText(AutoShotDisplayWindow, AUTO_SHOT_MACHINE_NUMBER_TEXTBOX, AutoShotMachineName );
            }
        }

    }

}

/***********************************************************************
*                            RESET_MACHINE                             *
***********************************************************************/
void reset_machine( TCHAR * machine_name )
{

BOOLEAN        have_machine;
short          current_board_number;
short          current_mux_channel;
DB_TABLE       t;
HMACHINE_ENTRY m;
BOARD_CLASS    b;

/* WaitForSingleObject( FasTrakMutex, INFINITE ); */

have_machine = FALSE;
t.open( machset_dbname(ComputerName), MACHSET_RECLEN, PFL );
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, machine_name, MACHINE_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    {
    load_one_machine( m, t );
    have_machine = TRUE;
    }
t.close();

if ( have_machine )
    {
    if ( b.locate(current_board_number, current_mux_channel, machine_name) )
        {
        if ( current_board_number != m.board_number || current_mux_channel != m.mux_channel || !(m.monitor_flags & MA_MONITORING_ON) )
            {
            /*
            ----------------------------------------------
            If the board has changed, turn off the old one
            ---------------------------------------------- */
            if ( b.find(current_board_number) )
                {
                b.command_mode();
                b.set_machine( current_mux_channel, NO_MACHINE, NO_PART );
                b.init_one_board();
                b.start_one_board();
                }
            }
        }

    if ( m.monitor_flags & MA_MONITORING_ON )
        if ( b.find(m.board_number) )
            {
            b.command_mode();
            b.set_machine( m.mux_channel, m.name, m.part, m.downtime_timeout_seconds );
            b.init_one_board();
            b.start_one_board();
            b.set_monitoring( TRUE );
            }

    skip_reset_machine( machine_name );
    reset_machine_for_getdata( machine_name );
    }


/* ReleaseMutex( FasTrakMutex ); */
}

/***********************************************************************
*                     CHECK_FOR_MACHINES_TO_RESET                      *
***********************************************************************/
static void check_for_machines_to_reset( void )
{
TCHAR * cp;

while ( TRUE )
    {
    cp = MachinesToReset.pop();
    if ( !cp )
        break;

    reset_machine( cp );
    }

}

/***********************************************************************
*                          RESET_ALL_BOARDS                            *
***********************************************************************/
void reset_all_boards( void )
{
BOARD_CLASS     b;

b.stop_all_boards();
b.startup();
load_machines();
b.init_all_boards();
b.start_all_boards();
}

/***********************************************************************
*                             ADD_BOARD_DATA                           *
***********************************************************************/
static TCHAR * add_board_data( TCHAR * dest, BOARD_DATA x )
{
const TCHAR ZeroChar = TEXT( '0' );

insalph( dest, (int32) x, BOARD_DATA_HEX_LEN, ZeroChar, HEX_RADIX );
dest += BOARD_DATA_HEX_LEN;

*dest = DelimitChar;
dest++;
return dest;
}

/***********************************************************************
*                   SEND_MACHINE_STATE_EVENT_STRING                    *
*                                                                      *
*                         "M01\t00F1\t00FF"                            *
***********************************************************************/
static void send_machine_state_event_string( TCHAR * machine, BOARD_DATA old_status, BOARD_DATA new_status )
{

static const int32 DDE_LINE_LEN = MACHINE_NAME_LEN + 1 + BOARD_DATA_HEX_LEN + 1 + BOARD_DATA_HEX_LEN + 1;

TCHAR * buf;
TCHAR * cp;

buf = maketext( DDE_LINE_LEN );
if ( !buf )
    {
    return;
    }

cp = buf;
cp = copy_w_char( cp, machine, DelimitChar );
cp = add_board_data( cp, old_status );
cp = add_board_data( cp, new_status );
cp--;
*cp = NullChar;

PostMessage( MainWindow, WM_POKEMON, (WPARAM) NEW_MACH_STATE_INDEX, (LPARAM) buf );
}

/***********************************************************************
*                      ENABLE_BOARD_RADIO_BUTTONS                      *
***********************************************************************/
static void enable_board_radio_buttons( void )
{

short       first_board_number;
short       n;
int32       i;
BOARD_CLASS b;
BOOL        have_board[MAX_BOARDS];
UINT        id;
HWND        w;

for ( i=0; i<MAX_BOARDS; i++ )
    have_board[i] = FALSE;

first_board_number = NO_BOARD_NUMBER;

while ( b.next() )
    {
    n = b.number();
    if ( n == NO_BOARD_NUMBER )
        break;

    if ( n == first_board_number )
        break;

    if ( first_board_number == NO_BOARD_NUMBER )
        first_board_number = n;

    n--;
    have_board[n] = TRUE;
    }

id = BOARD_1_RADIO;
for ( i=0; i<MAX_BOARDS; i++ )
    {
    w = Ioex4Window.control( id );
    if ( w )
        EnableWindow( w, have_board[i] );
    id++;
    }

}

/***********************************************************************
*                        SHOW_MULTIPLEX_BUTTONS                        *
***********************************************************************/
static void show_multiplex_buttons( BOARD_CLASS & b )
{
int32 i;
HWND  w;
BOOL  state;
HARDWARE_BOARD_ENTRY * hb;

w = Ioex4Window.handle();

hb    = 0;
state = FALSE;
if ( (b.status() & VIS_BUSY) == 0 )
    hb = b.board();

if ( hb )
    if ( hb->type & MUX_BOARD_TYPE )
        state = TRUE;

for ( i=0; i<NofMultiplexButtons; i++ )
    EnableWindow( GetDlgItem(w, MultiplexButton[i]), state );

}

/***********************************************************************
*                          SHOW_CURRENT_BOARD                          *
***********************************************************************/
static void show_current_board( BOARD_CLASS & b )
{
UINT  id;
UINT  xid;
int32 c;
int32 n;
HWND  w;
HARDWARE_BOARD_ENTRY * hb;
BOOLEAN                is_checked;
BOOLEAN                is_enabled;
TCHAR                * cp;

w = Ioex4Window.handle();

n = b.number();
if ( n == NO_BOARD_NUMBER )
    return;

CheckRadioButton( w, BOARD_1_RADIO, BOARD_8_RADIO, BOARD_1_RADIO + n - 1 );
set_text( w, BOARD_ADDRESS_EBOX, ultohex((unsigned long) b.address()) );

hb = b.board();
if ( hb )
    {
    id = SINGLE_BOARD_XBOX;
    if ( hb->type & MUX_BOARD_TYPE )
        id = MUX_BOARD_XBOX;
    CheckRadioButton( w, SINGLE_BOARD_XBOX, MUX_BOARD_XBOX, id );

    id = FOUR_MHZ_BOARD_RADIO;
    if ( hb->mhz == 12 )
        id = TWELVE_MHZ_BOARD_RADIO;
    CheckRadioButton( w, FOUR_MHZ_BOARD_RADIO, TWELVE_MHZ_BOARD_RADIO, id );

    c  = b.current_mux_channel();
    if ( c > 0 )
        c--;
    id = MUX_CHANNEL_1_RADIO + c;
    CheckRadioButton( w, MUX_CHANNEL_1_RADIO, MUX_CHANNEL_8_RADIO, id );

    set_text( w, IOEX4_MACHINE_TBOX, hb->mux[c].machine );
    set_text( w, IOEX4_PART_TBOX,    hb->mux[c].part    );

    id  = MUX_ENABLE_1_XBOX;
    xid = MUX_CHANNEL_1_RADIO;
    for ( c=0; c<MAX_MUX_CHANNELS; c++ )
        {
        cp = hb->mux[c].machine;
        if ( lstrcmp(cp, NO_MACHINE) != 0 )
            is_checked = TRUE;
        else
            is_checked = FALSE;
        is_enabled = is_checked;
        EnableWindow( GetDlgItem(w, xid), (BOOL) is_enabled );
        EnableWindow( GetDlgItem(w, id), (BOOL) is_enabled );
        set_checkbox( w, id, is_checked );
        id++;
        xid++;
        }

    show_bits( w, OPTO_BIT_0_XBOX, hb->dout_bits, FALSE );
    }

//show_multiplex_buttons( b );   these are not shown anymore
}

/***********************************************************************
*                          SET_IOEX4_BOARD                             *
***********************************************************************/
void set_ioex4_board( BOARD_CLASS & b )
{
UINT  id;
HWND  w;

w = Ioex4Window.handle();

for ( id=BOARD_1_RADIO; id<=BOARD_8_RADIO; id++ )
    {
    if ( is_checked( w, id ) )
        {
        if ( b.find(1+id-BOARD_1_RADIO) )
            {
            b.set_port_addresses();
            b.set_pos_sensor_pointer();
            show_current_board( b );
            show_bits( Ioex4Window.handle(), SCAN_MODE_BIT_TBOX, ft_read_status(), TRUE );
            }
        break;
        }
    }
}

/***********************************************************************
*                            SET_PARAM_TEXT                            *
***********************************************************************/
static void set_param_text( UINT id, float value )
{
set_text( FtSetupParamsWindow, id, ascii_float(value) );
}

/***********************************************************************
*                          FILL_SETUP_PARAMS                           *
*                                                                      *
*    The operator has just opened the setup parameters dialog.         *
*    I need to read the values from the board and fill the editboxes.  *
***********************************************************************/
static void fill_setup_params( BOARD_CLASS & b )
{
short        i;
short        n;
short        gain;
UINT         id;
UINT         first_id;
float        x;
BOARD_DATA   d;
MUX_ENTRY  * me;
STRING_CLASS s;

if ( !FtSetupParamsWindow )
    return;

b.set_pos_sensor_pointer();

get_version( &n );
if ( n >= FT_8_CHANEL_VERSION )
    n = MAX_FT_CHANNELS;
else
    n = MAX_CONTINUOUS_CHANNELS;

for ( i=0; i<n; i++ )
    {
    rd_gain( i+1, &gain );
    first_id = CHAN_1_GAIN_1_RADIO + 2*i;
    id = first_id;
    if ( gain > 1 )
        id++;
    CheckRadioButton( FtSetupParamsWindow, first_id, first_id+1, id );
    EnableWindow( GetDlgItem(FtSetupParamsWindow, id), TRUE );
    }

id = CHAN_1_GAIN_1_RADIO + 2*n;
while ( id <= CHAN_8_GAIN_2_RADIO )
    {
    set_checkbox( FtSetupParamsWindow, id, FALSE );
    EnableWindow( GetDlgItem(FtSetupParamsWindow, id), FALSE );
    id++;
    }

rd_total_stk_len( &x );
set_param_text( TOTAL_STROKE_LENGTH_EBOX, x );

rd_min_stk_len( &x );
set_param_text( MIN_STROKE_LENGTH_EBOX, x );

rd_tim_del_bisc( &x );
set_param_text( TIME_DELAY_BISCUIT_EBOX, x );

rd_vel_eos( &x );
set_param_text( VEL_FOR_EOS_EBOX, x );

rd_fill_distance( &x );
set_param_text( FILL_DISTANCE_EBOX, x );

rd_run_fill_distance( &x );
set_param_text( RUNNER_FILL_DISTANCE_EBOX, x );

rd_min_vel_csfs( &x );
set_param_text( VEL_FOR_CSFS_EBOX, x );

rd_vel_rise_time( &x );
set_param_text( VEL_FOR_RISE_TIME_EBOX, x );

rd_pos_csfs( &x );
set_param_text( MIN_POS_FOR_CSFS_EBOX, x );

for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
    {
    rd_limit_switch( i, &x );
    set_param_text( LS1_SETPOINT_EBOX+i, x );
    }

rd_delay( &d );
set_text( FtSetupParamsWindow, NOF_TIME_POINTS_EBOX, int32toasc((int32) d) );

rd_tim_coll_interval( &x );
set_param_text( TIME_BASED_MS_EBOX, x );

rd_time_out_period( &x );
set_param_text( CYCLE_START_TIMEOUT_EBOX, x );

rd_min_vel_eos( &x );
set_param_text( MIN_EOS_EBOX, x );

me = b.current_mux_entry();
if ( me )
    {
    s = units_name( me->dist_units );
    set_text( FtSetupParamsWindow, TOTAL_STROKE_UNITS_TBOX,  s.text() );
    set_text( FtSetupParamsWindow, MIN_STROKE_UNITS_TBOX,    s.text() );
    set_text( FtSetupParamsWindow, FILL_DISTANCE_UNITS_TBOX, s.text() );
    set_text( FtSetupParamsWindow, RUNNER_FILL_UNITS_TBOX,   s.text() );
    set_text( FtSetupParamsWindow, MIN_CSFS_POS_UNITS_TBOX,  s.text() );

    set_text( FtSetupParamsWindow, LS1_SETPOINT_UNITS_TBOX,  s.text() );
    set_text( FtSetupParamsWindow, LS2_SETPOINT_UNITS_TBOX,  s.text() );
    set_text( FtSetupParamsWindow, LS3_SETPOINT_UNITS_TBOX,  s.text() );
    set_text( FtSetupParamsWindow, LS4_SETPOINT_UNITS_TBOX,  s.text() );

    s = units_name( me->vel_units );
    set_text( FtSetupParamsWindow, VEL_FOR_EOS_UNITS_TBOX,   s.text() );
    set_text( FtSetupParamsWindow, VEL_FOR_CSFS_UNITS_TBOX,  s.text() );
    set_text( FtSetupParamsWindow, VEL_FOR_RISE_UNITS_TBOX,  s.text() );
    set_text( FtSetupParamsWindow, MIN_EOS_UNITS_TBOX,       s.text() );
    }

}

/***********************************************************************
*                              ADD_TO_LB                               *
***********************************************************************/
void add_to_lb( LISTBOX_CLASS & lb, TCHAR * name, TCHAR * data, short uid )
{
TCHAR   s[PARAMETER_NAME_LEN + 1 + MAX_FLOAT_LEN + 1 + UNITS_LEN + 1];
TCHAR * cp;

cp = copy_w_char( s,  name, DelimitChar );
cp = copy_w_char( cp, data, SpaceChar );
lstrcpy( cp, units_name( uid ) );
lb.add( s );
}

/***********************************************************************
*                           ADD_FLOAT_TO_LB                            *
***********************************************************************/
void add_float_to_lb( LISTBOX_CLASS & lb, PARAMETER_ENTRY * p, float x )
{
add_to_lb( lb, p->name, ascii_float(x), p->units );
}

/***********************************************************************
*                         ADD_BOARD_DATA_TO_LB                         *
***********************************************************************/
void add_board_data_to_lb( LISTBOX_CLASS & lb, PARAMETER_ENTRY * p, BOARD_DATA x )
{
add_to_lb( lb, p->name, int32toasc((int32) x), p->units );
}

/***********************************************************************
*                    FILL_COMPUTED_SHOT_PARAMETERS                     *
***********************************************************************/
void fill_computed_shot_parameters( BOARD_CLASS & b )
{
int32      i;
int32      n;
BOARD_DATA d;
float      x;
LISTBOX_CLASS     lb;
PARAMETER_CLASS   p;
MUX_ENTRY       * me;

if ( !FtShotParamsWindow )
    return;

lb.init( FtShotParamsWindow, SHOT_PARAM_LISTBOX );
lb.redraw_off();

me = b.current_mux_entry();
if ( me )
    {
    if ( p.find( ComputerName, me->machine, me->part) )
        {
        rd_eos_pos( &x );
        add_float_to_lb( lb, p.parameter + P3_POS_PARM,  x );

        rd_bisc_size( &x );
        add_float_to_lb( lb, p.parameter + BISCUIT_SIZE,  x );

        rd_cycle_tim( &x );
        add_float_to_lb( lb, p.parameter + CYCLE_TIME_PARM,  x );

        rd_p1( &x );
        add_float_to_lb( lb, p.parameter + P1_POS_PARM,  x );

        rd_p2( &x );
        add_float_to_lb( lb, p.parameter + P2_POS_PARM,  x );

        rd_csfs( &x );
        add_float_to_lb( lb, p.parameter + CSFS,  x );

        ft_read_parameter( FIL_TIM, &d );
        add_board_data_to_lb( lb, p.parameter + FILL_TIME,  d );

        ft_read_parameter( TOT_SHT_TIM, &d );
        add_board_data_to_lb( lb, p.parameter + TOTAL_SHOT_TIME,  d );

        ft_read_parameter( SS_TIM, &d );
        add_board_data_to_lb( lb, p.parameter + SLOW_SHOT_TIME,  d );

        ft_read_parameter( FS_TIM, &d );
        add_board_data_to_lb( lb, p.parameter + FAST_SHOT_TIME,  d );

        rd_rise_time( &x );
        add_float_to_lb( lb, p.parameter + RISE_TIME,  x );

        for ( i=0; i<5; i++ )
            {
            rd_time_interval( &x, i );
            add_float_to_lb( lb, p.parameter + USER_TIME_INT_ONE+i,  x );
            }

        n = 2*FT_ONOFF_COUNT;
        for ( i=0; i<n; i++ )
            {
            rd_on_off_input( &x, i );
            add_to_lb( lb, resource_string(ON_1_STRING+i), ascii_float(x), me->dist_units );
            }

        }
    }

lb.redraw_on();

}

/***********************************************************************
*                         DO_RAM_TEST_REQUEST                          *
***********************************************************************/
void do_ram_test_request( void )
{
const DWORD MsForRamTest =  5;
BOARD_DATA x;
STRING_CLASS s;

ft_send_cmd_wo_check( RAM_TEST );
Sleep( MsForRamTest );

while ( ft_read_status() & (unsigned short) VIS_BUSY )
    ;

x = ft_read_data();

if ( x == 0 )
    {
    s = resource_string( MainInstance, NO_RAM_ERRORS_STRING );
    }
else if ( x & 0x0001 )
    {
    s = resource_string( MainInstance, HIGH_RAM_ERROR_STRING );
    s += ultohex( (unsigned long) x );
    }
else
    {
    s = resource_string( MainInstance, LOW_RAM_ERROR_STRING );
    s += ultohex( (unsigned long) x );
    }

if ( Ioex4Window.handle() )
    set_text( Ioex4Window.handle(), FASTRAK_STATUS_TBOX, s.text() );
}

/***********************************************************************
*                      DO_OPTO_PORT_TEST_REQUEST                       *
***********************************************************************/
void do_opto_port_test_request( void )
{
const DWORD MsForOptoPortTest = 5;
BOARD_DATA x;
UINT       id;

ft_send_cmd_wo_check( TEST_DIO );

Sleep( MsForOptoPortTest );

while ( ft_read_status() & (unsigned short) VIS_BUSY )
    ;

x = ft_read_data();
if ( x == 0 )
    id = OPTO_PORT_OK_STRING;
else
    id = OPTO_PORT_ERROR_STRING;

if ( Ioex4Window.handle() )
    set_text( Ioex4Window.handle(), FASTRAK_STATUS_TBOX, resource_string(MainInstance, id) );
}

/***********************************************************************
*                        DO_IO_PORT_TEST_REQUEST                       *
***********************************************************************/
void do_io_port_test_request( void )
{
const BOARD_DATA MASK_ONE = 0x5555;
const BOARD_DATA MASK_TWO = 0xAAAA;
const DWORD MS_TO_WAIT    = 100;

UINT id;

id = IO_PORT_OK_STRING;

ft_send_data( MASK_ONE );
ft_send_cmd_wo_check( DATA_REG_WR_TST );
Sleep( MS_TO_WAIT );

if ( ft_read_status() != MASK_ONE || ft_read_data() != MASK_ONE )
    {
    id = IO_PORT_DATA_ERROR_STRING;
    }
else
    {
    ft_send_data( MASK_TWO );
    ft_send_cmd_wo_check( DATA_REG_WR_TST );
    Sleep( MS_TO_WAIT );

    if ( ft_read_status() != MASK_TWO || ft_read_data() != MASK_TWO )
        id = IO_PORT_DATA_ERROR_STRING;
    }

if ( id == IO_PORT_OK_STRING )
    {
    /*
    -------------------------------------
    Put card in mode to test command port
    ------------------------------------- */
    ft_send_cmd_wo_check( SET_REG_TST_FLG );
    Sleep( MS_TO_WAIT );

    /*
    ----------------
    Send bit pattern
    ---------------- */
    ft_send_cmd_wo_check( MASK_ONE );
    Sleep( MS_TO_WAIT );
    if ( ft_read_status() != MASK_ONE || ft_read_data() != MASK_ONE )
        id = IO_PORT_COMMAND_ERROR_STRING;

    ft_send_cmd_wo_check( MASK_TWO );
    Sleep( MS_TO_WAIT );
    if ( ft_read_status() != MASK_TWO || ft_read_data() != MASK_TWO )
        id = IO_PORT_COMMAND_ERROR_STRING;
    }

if ( Ioex4Window.handle() )
    set_text( Ioex4Window.handle(), FASTRAK_STATUS_TBOX, resource_string(MainInstance, id) );

/*
--------------------------
Send a NMI to restart card
-------------------------- */
ft_set_command_mode();
}

/***********************************************************************
*                          DO_TRANSDUCER_TEST                          *
***********************************************************************/
void do_transducer_test( void )
{
const DWORD MsForTransducerTest = 500;
const int32 NofChannelPoints = 504;

BOARD_DATA cmd;
BOARD_DATA x;
int32      i;

if ( ft_send_command(CHECK_POS_XDCR) == FT_OK )
    {
    Sleep( MsForTransducerTest );
    while ( ft_read_status() & (unsigned short) VIS_BUSY )
        ;
    }

cmd = 0x7800;

for ( i=0; i<NofChannelPoints; i++ )
    {
    ft_read_parameter( cmd, &x );
    if ( x & 0x0001)
        Channel_A_Bits.set( i );
    else
        Channel_A_Bits.clear( i );

    if ( x & 0x0020)
        Channel_B_Bits.set( i );
    else
        Channel_B_Bits.clear( i );
    cmd++;
    }

if ( TransducerTestWindow )
    InvalidateRect( TransducerTestWindow, 0, TRUE );

}

/***********************************************************************
*                            UL_FROM_HEX_EBOX                          *
***********************************************************************/
unsigned long ul_from_hex_ebox( HWND w, UINT id )
{
TCHAR buf[MAX_HEX_LEN+1];
unsigned long x;

x = 0;

if ( get_text(buf, w, id, MAX_HEX_LEN) )
    x = hextoul( buf );

return x;
}

/***********************************************************************
*                            DO_READ_DATA_PORT                         *
***********************************************************************/
static void do_read_data_port( void )
{
BOARD_DATA x;

if ( !FtCommandPortWindow )
    return;

x = ft_read_data();
set_text( FtCommandPortWindow, DATA_PORT_VALUE_EBOX, ultohex((unsigned long) x)  );
}

/***********************************************************************
*                            DO_SEND_DATA_PORT                         *
***********************************************************************/
static void do_send_data_port( void )
{
BOARD_DATA x;

if ( !FtCommandPortWindow )
    return;

x = ( BOARD_DATA ) ul_from_hex_ebox( FtCommandPortWindow, DATA_PORT_VALUE_EBOX );
ft_send_data( x );
}

/***********************************************************************
*                          DO_SEND_COMMAND_PORT                        *
***********************************************************************/
static void do_send_command_port( void )
{
BOARD_DATA x;

if ( !FtCommandPortWindow )
    return;

x = ( BOARD_DATA ) ul_from_hex_ebox( FtCommandPortWindow, COMMAND_VALUE_EBOX );

ft_send_command( x );
}

/***********************************************************************
*                             SET_MUX_TYPE                             *
***********************************************************************/
static void set_mux_type( BOARD_CLASS & b, int i )
{
HARDWARE_BOARD_ENTRY * hb;
BITSETYPE mask;
BOARD_DATA board_type;

hb = b.board();
if ( !hb )
    return;

mask = ~(MUX_BOARD_TYPE | SINGLE_MACHINE_BOARD_TYPE);

hb->type &= mask;

if ( i == SET_SINGLE_BOARD_REQUEST )
    {
    hb->type |= SINGLE_MACHINE_BOARD_TYPE;
    board_type = FT_SINGLE_MACHINE_TYPE;
    }
else
    {
    hb->type |= MUX_BOARD_TYPE;
    board_type = FT_MUX_MACHINE_TYPE;
    }

ft_set_type( board_type );
show_current_board( b );
Sleep( 1500 );
}

/***********************************************************************
*                         OK_FOR_MUX_COMMAND                           *
***********************************************************************/
BOOLEAN ok_for_mux_command( BOARD_CLASS & b )
{
HARDWARE_BOARD_ENTRY * hb;

if ( b.status() & VIS_BUSY )
    {
    show_command_mode_message();
    show_current_board( b );
    return FALSE;
    }

hb = b.board();
if ( !hb )
    return FALSE;

if ( (hb->type & MUX_BOARD_TYPE) == 0 )
    {
    show_mux_only_message();
    show_current_board( b );
    return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                           SET_MUX_CHANNEL                            *
***********************************************************************/
static void set_mux_channel( BOARD_CLASS & b )
{
UINT  id;
HWND  w;
MUX_ENTRY   * me;

if ( !ok_for_mux_command(b) )
    return;

w = Ioex4Window.handle();

for ( id=MUX_CHANNEL_1_RADIO; id<=MUX_CHANNEL_8_RADIO; id++ )
    {
    if ( is_checked( w, id ) )
        {
        ft_change_mux_channel( (short) 1 + id - MUX_CHANNEL_1_RADIO );
        b.read_current_mux_channel();
        me = b.current_mux_entry();
        if ( me )
            {
            set_text( w, IOEX4_MACHINE_TBOX, me->machine );
            set_text( w, IOEX4_PART_TBOX,    me->part           );
            }
        break;
        }
    }
}

/***********************************************************************
*                              SET_MUX_MASK                            *
***********************************************************************/
static void set_mux_mask( BOARD_CLASS & b )
{
UINT  id;
HWND  w;

BOARD_DATA x;
BOARD_DATA mask;

if ( !ok_for_mux_command(b) )
    return;

x    = 0;
mask = 1;

w = Ioex4Window.handle();

for ( id=MUX_ENABLE_1_XBOX; id<=MUX_ENABLE_8_XBOX; id++ )
    {
    if ( is_checked( w, id ) )
        x |= mask;
    mask <<= 1;
    }

set_mach_mask( x );
}

/***********************************************************************
*                          CHECK_FOR_COMMANDS                          *
***********************************************************************/
static void check_for_commands( BOARD_CLASS & b )
{
TCHAR    * cp;
int        i;
short      version;

while ( TRUE )
    {
    cp = FasTrakCommand.pop();
    if ( !cp )
        break;

    i = (int) hextoul( cp );
    switch ( i )
        {
        case DATA_COLLECTION_MODE_REQUEST:
            b.data_collection_mode();
            break;

        case COMMAND_MODE_REQUEST:
            b.command_mode();
            break;

        case SCAN_MODE_REQUEST:
            b.data_collection_mode();
            break;

        case SET_BOARD_REQUEST:
            set_ioex4_board( b );
            break;

        case SET_MUX_CHANNEL_REQUEST:
            set_mux_channel( b );
            break;

        case RESET_MAX_POSITION_REQUEST:
            UMaxPosition    = 0;
            MaxPosition     = 0;
            RealMaxPosition = 0.0;
            set_text( FtCurrentValuesWindow, MAX_POSITION_TBOX, TEXT( "0.0") );
            break;

        case FILL_SETUP_PARAMS_REQUEST:
            fill_setup_params( b );
            break;

        case FILL_SHOT_PARAMS_REQUEST:
            fill_computed_shot_parameters( b );
            break;

        case LATCH_COUNTERS_REQUEST:
            ft_send_cmd_busy( LATCH_COUNTER );
            break;

        case CLEAR_COUNTERS_REQUEST:
            ft_send_cmd_busy( CLR_COUNTER );
            break;

        case CLEAR_DATA_READY_BIT_REQUEST:
            ft_send_cmd_busy( CLR_DATA_READY );
            break;

        case RESET_REQUEST:
            ft_send_cmd_busy( RESET_CMD );
            break;

        case CLEAR_ERROR_BITS_REQUEST:
            ft_send_cmd_busy( CLR_ERR );
            break;

        case SET_MUX_BOARD_REQUEST:
            set_mux_type( b, i );
            break;

        case SET_SINGLE_BOARD_REQUEST:
            set_mux_type( b, i );
            break;

        case GET_VERSION_REQUEST:
            get_version( &version );
            set_text( Ioex4Window.handle(), FASTRAK_STATUS_TBOX, int32toasc((int32) version) );
            break;

        case SET_MUX_MASK_REQUEST:
            set_mux_mask( b );
            break;

        case RAM_TEST_REQUEST:
            do_ram_test_request();
            break;

        case OPTO_PORT_TEST_REQUEST:
            do_opto_port_test_request();
            break;

        case IO_PORT_TEST_REQUEST:
            do_io_port_test_request();
            break;

        case TRANSDUCER_TEST_REQUEST:
            do_transducer_test();
            break;

        case READ_DATA_PORT_REQUEST:
            do_read_data_port();
            break;

        case SEND_DATA_PORT_REQUEST:
            do_send_data_port();
            break;

        case CLEAR_COMMAND_PORT_REQUEST:
            ft_clear_cmd_port();
            break;

        case SEND_COMMAND_PORT_REQUEST:
            do_send_command_port();
            break;

        case CLEAR_OUTPUT_OPTOS_REQUEST:
            ft_send_data( 0 );
            ft_send_command( OUT_TO_OPTO );
            break;

        case SET_OUTPUT_OPTOS_REQUEST:
            ft_send_data( 0xff );
            ft_send_command( OUT_TO_OPTO );
            break;
        }
    }
}

/***********************************************************************
*                           UPDATE_THERMOMETER                         *
***********************************************************************/
static void update_thermometer( BOARD_DATA & max_value, BOARD_DATA current_value, int id )
{
if ( current_value > 0xF700 )
    current_value = 0;

if ( current_value > max_value )
    {
    max_value = current_value;
    SendDlgItemMessage( FtCurrentValuesWindow, id, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) current_value) );
    }

SendDlgItemMessage( FtCurrentValuesWindow, id, PBM_SETPOS, current_value, 0L );
}
/***********************************************************************
*                       INIT_ANALOG_THERMOMETERS                       *
***********************************************************************/
static void init_analog_thermometers( void )
{
int ch;
int id;

id = FT_A1_THERM;
for ( ch=0; ch<MAX_FT_CHANNELS; ch++ )
    {
    MaxAnalog[ch] = 4095;
    SendDlgItemMessage( FtCurrentValuesWindow, id, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );
    SendDlgItemMessage( FtCurrentValuesWindow, id, PBM_SETPOS, 0, 0L );
    id++;
    }
}

/**********************************************************************
*                            save_csv_file_info                       *
**********************************************************************/
void save_csv_file_info( double y )
{
FILE_CLASS   f;
TIME_CLASS   t;
STRING_CLASS s;
if ( !CsvFileName.isempty() )
    {
    if ( f.open_for_append(CsvFileName) )
        {
        t.get_local_time();
        s = t.text();
        s += CommaChar;
        s += y;
        f.writeline( s );
        f.close();
        }
    }
}

/***********************************************************************
*                        SHOW_CURRENT_FT_VALUES                        *
***********************************************************************/
void show_current_ft_values( BOARD_CLASS & b )
{
const BOARD_DATA COMMAND[MAX_FT_CHANNELS] = { CUR_ANA1, CUR_ANA2, CUR_ANA3, CUR_ANA4, CUR_ANA5, CUR_ANA6, CUR_ANA7, CUR_ANA8 };
const BOARD_DATA MASK                     = 0x0FFF;
const BOARD_DATA MAX_BOARD_DATA           = 0xFFFF;
static bool      first_time               = true;

short n;
short ch;
UINT  id;
double y;

MUX_ENTRY  * me;
BOARD_DATA   x;

if ( !FtCurrentValuesWindow )
    {
    first_time = true;
    return;
    }

if ( first_time )
    init_analog_thermometers();


if ( ft_read_status() & VIS_BUSY )
    return;

get_version( &n );
if ( n >= FT_8_CHANEL_VERSION )
    n = MAX_FT_CHANNELS;
else
    n = MAX_CONTINUOUS_CHANNELS;

me = b.current_mux_entry();
id = ANALOG_1_TBOX;
for ( ch=0; ch<n; ch++ )
    {
    if ( is_checked( FtCurrentValuesWindow, FT_A1_XBOX+ch) )
        {
        if ( ft_read_parameter(COMMAND[ch], &x) == FT_OK )
            {
            x &= MASK;
            if ( first_time || x != CurrentAnalog[ch] )
                {
                CurrentAnalog[ch] = x;
                if ( me )
                    {
                    y = (double) x;
                    y *= (double) 10.0;
                    y /= (double) 4095.0;
                    if ( me->asensor[ch]->gain > 1 )
                        y /= 2.0;
                    set_text( FtCurrentValuesWindow, VOLTS_1_TBOX+ch, fixed_string( y, 3, 2) );

                    y = (double) convert_analog( x, me->asensor[ch] );
                    set_text( FtCurrentValuesWindow, id, fixed_string( y, LeftAnalogPlaces, RightAnalogPlaces) );
                    if ( me->asensor[ch]->gain > 1 )
                        x /= 2;
                    update_thermometer( MaxAnalog[ch], x, FT_A1_THERM+ch );
                    if ( CsvEnable && ch == CsvChannelIndex )
                        save_csv_file_info( y );
                    }
                }
            }
        }
    id++;
    }

if ( first_time )
    {
    while ( id <= ANALOG_8_TBOX )
        {
        set_text( FtCurrentValuesWindow, id, EmptyString );
        id++;
        }
    }

if ( me )
    {
    if ( first_time )
        {
        set_text( FtCurrentValuesWindow, POSITION_UNITS_TBOX, units_name(me->dist_units) );
        set_text( FtCurrentValuesWindow, VELOCITY_UNITS_TBOX, units_name(me->vel_units)  );
        }

    if ( ft_read_parameter(CUR_POS, &x) == FT_OK )
        {
        if ( first_time || x != CurrentPosition )
            {
            CurrentPosition = x;
            y = (double) (short) x;
            y /= (double) me->psensor->x4_pos_divisor;
            set_text( FtCurrentValuesWindow, FT_CURRENT_POSITION_TBOX, fixed_string(y, LeftPosPlaces, RightPosPlaces) );
            update_thermometer( UMaxPosition, x, FT_POS_THERM );
            }

        if ( first_time || short(x) > MaxPosition )
            {
            MaxPosition = (short) x;
            y = (double) MaxPosition;
            y /= (double) me->psensor->x4_pos_divisor;
            RealMaxPosition = y;
            set_text( FtCurrentValuesWindow, MAX_POSITION_TBOX, fixed_string(y, LeftPosPlaces, RightPosPlaces) );
            }
        }

    if ( ft_read_parameter(CUR_VEL, &x) == FT_OK )
        {
        if ( first_time || x != CurrentVelocity )
            {
            CurrentVelocity = x;
            y = 0.0;
            if ( x )
                {
                y = (double) me->psensor->vel_dividend;
                y /= (double) x;
                update_thermometer( MaxVelocity, MAX_BOARD_DATA/x, FT_VEL_THERM );
                }
            set_text( FtCurrentValuesWindow, FT_CURRENT_VELOCITY_TBOX, fixed_string(y, LeftVelPlaces, RightVelPlaces) );
            }
        }
    }

if ( LastBoard != b.number() || LastMux != b.current_mux_channel() )
    {
    LastBoard  = b.number();
    LastMux    = b.current_mux_channel();
    MaxPosition     = 0;
    RealMaxPosition = 0.0;

    id = ANALOG_1_TBOX + n;
    while ( id <= ANALOG_8_TBOX )
        {
        set_text( FtCurrentValuesWindow, id, EmptyString );
        id++;
        }

    if ( me )
        {
        id = UNITS_1_TBOX;
        for ( ch=0; ch<n; ch++ )
            {
            set_text( FtCurrentValuesWindow, id, units_name(me->asensor[ch]->units) );
            id++;
            }

        for ( ch=n; ch<MAX_FT_CHANNELS; ch++ )
            {
            set_text( FtCurrentValuesWindow, id, EmptyString );
            id++;
            }
        }
    }

first_time = false;
}

/***********************************************************************
*                        SHOW_COMMAND_MODE_BUTTONS                     *
***********************************************************************/
static void show_command_mode_buttons( BOARD_CLASS & b )
{
int32 i;
HWND  w;
BOOL  state;
BOOL  mystate;

w = Ioex4Window.handle();

if ( b.status() & VIS_BUSY )
    state = FALSE;
else
    state = TRUE;

mystate = state;
for ( i=0; i<NofCommandModeButtons; i++ )
    {
    if ( state && i == NofAlwaysOkButtons && Ioex4GrayLevel > CurrentPasswordLevel )
        mystate = FALSE;
    EnableWindow( GetDlgItem(w, CommandModeButton[i]), mystate );
    }

if ( !state )
    for ( i=0; i<NofMultiplexButtons; i++ )
        EnableWindow( GetDlgItem(w, MultiplexButton[i]), state );

// show_multiplex_buttons( b ); these are not shown anymore
}

/***********************************************************************
*                          TURN_OFF_MONITOR_WIRE                       *
***********************************************************************/
void turn_off_monitor_wire( HARDWARE_BOARD_ENTRY * b )
{

if ( MonitorWire != NO_WIRE )
    b->clearwire( MonitorWire );

}

/***********************************************************************
*                         BOARD_MONITOR_THREAD                         *
***********************************************************************/
DWORD board_monitor_thread( int * notused )
{
static BOOLEAN ioex4_was_active = FALSE;

DWORD       status;
short       c;
BOARD_DATA  new_status;
BOARD_DATA  old_status;
BOARD_CLASS b;
SYSTEMTIME  st;
TIME_CLASS  tc;
long        now;
DSTAT_CLASS d;
HARDWARE_BOARD_ENTRY * hb;
MUX_ENTRY            * me;

BoardMonitorIsRunning = TRUE;

while ( TRUE )
    {
    if ( Ioex4IsActive )
        {
        if ( !ioex4_was_active )
            {
            MaxPosition     = 0;
            RealMaxPosition = 0.0;
            enable_board_radio_buttons();
            b.set_pos_sensor_pointer();
            show_current_board( b );
            }

        new_status = ft_read_status();
        b.set_status( new_status );
        if ( !ioex4_was_active )
            old_status = ~new_status;

        if ( new_status != old_status )
            {
            hb = b.board();
            if ( hb->type & MUX_BOARD_TYPE )
                {
                if ( (new_status & VIS_AUTO_SHOT) && !(old_status & VIS_AUTO_SHOT) )
                    {
                    b.read_current_mux_channel();
                    show_current_board( b );
                    }
                }
            show_bits( Ioex4Window.handle(), SCAN_MODE_BIT_TBOX, new_status, TRUE );

            if ( (old_status ^ new_status) & VIS_BUSY )
                show_command_mode_buttons( b );

            old_status = new_status;
            }

        ioex4_was_active = TRUE;
        check_for_commands( b );
        show_current_ft_values(b);
        if ( !FtCurrentValuesWindow )
            {
            LastBoard = NO_BOARD_NUMBER;
            LastMux   = NO_MUX_CHANNEL;
            }
        }
    else
        {
        if ( ioex4_was_active )
            {
            ioex4_was_active = FALSE;
            reset_all_boards();
            }
        else
            {
            check_for_machines_to_reset();
            }

        if ( b.next() )
            {
            /* WaitForSingleObject( FasTrakMutex, INFINITE ); */
            b.set_port_addresses();
            new_status = ft_read_status();
            old_status = b.status();
            if ( new_status != old_status )
                {
                if ( b.monitoring() )
                    {
                    if ( (new_status & VIS_AUTO_SHOT) && !(old_status & VIS_AUTO_SHOT) )
                        {
                        c = b.read_current_mux_channel();
                        if ( c != NO_MUX_CHANNEL )
                            show_auto_shot( b, c );
                        }
                    else if ( (old_status & VIS_AUTO_SHOT) && !(new_status & VIS_AUTO_SHOT) )
                        {
                        hide_auto_shot( b, b.current_mux_channel() );
                        }

                    if ( (new_status & VIS_DATA_READY) && !(old_status & VIS_DATA_READY) )
                        {
                        b.read_current_mux_channel();
                        b.set_pos_sensor_pointer();
                        get_shot_data( b.board() );
                        b.data_collection_mode();
                        new_status &= ~VIS_DATA_READY;
                        }
                    else if ( (new_status & VIS_TIME_OUT) && !(old_status & VIS_TIME_OUT) )
                        {
                        b.data_collection_mode();
                        }

                    c = b.current_mux_channel();
                    if ( c != NO_MUX_CHANNEL )
                        send_machine_state_event_string( b.board()->mux[c-1].machine, old_status, new_status );
                    }

                b.set_status( new_status );
                show_bits( b );
                }

            /* ReleaseMutex( FasTrakMutex ); */

            /*
            ------------------------------------------------------------
            If there was no status change for this board, check for down
            ------------------------------------------------------------ */
            if ( new_status == old_status )
                {
                hb = b.board();
                GetLocalTime(&st);
                now = sec_since1990( st );

                for ( c=0; c<MAX_MUX_CHANNELS; c++ )
                    {
                    me = &hb->mux[c];
                    if ( lstrcmp(me->machine, NO_MACHINE) != 0 )
                        {
                        if ( me->cycle_timeout_seconds > 0 )
                            {
                            if ( me->down_state == PROG_UP_STATE || me->down_state == MACH_UP_STATE || me->down_state == NO_DOWN_STATE )
                                {
                                if ( (now - me->time_of_last_shot) > me->cycle_timeout_seconds )
                                    {
                                    if ( SubtractTimeoutFromAutoDown )
                                        {
                                        tc.set( (unsigned long) now );
                                        tc -= (unsigned long) me->cycle_timeout_seconds;
                                        st = tc.system_time();
                                        }

                                    if ( StdDown.find( now ) )
                                        {
                                        d.set_cat( StdDown.cat() );
                                        d.set_subcat( StdDown.subcat() );
                                        }
                                    else
                                        {
                                        d.set_cat( DOWNCAT_SYSTEM_CAT );
                                        d.set_subcat( DOWNCAT_UNSP_DOWN_SUBCAT );
                                        }
                                    d.set_time( st );
                                    d.put( ComputerName, me->machine );
                                    me->down_state = d.down_state();
                                    append_downtime_record( ComputerName, me->machine, me->part, st, d.category(), d.subcategory() );

                                    if ( ClearMonitorWireOnDown )
                                        turn_off_monitor_wire( hb );

                                    if ( DownTimeAlarmWire != NO_WIRE )
                                        hb->setwire( DownTimeAlarmWire );
                                    }
                                }
                            else if ( hb->need_dout_update )
                                {
                                /*
                                ----------------------------------------
                                I am down but I need to change the state
                                of the opto bits. I probably want to
                                turn off the MonitorWire.
                                ---------------------------------------- */
                                b.command_mode();
                                b.data_collection_mode();
                                }
                            }
                        }
                    }
                }
            }
        }

    if ( ShuttingDown )
        {
        turn_off_monitor_wire( b.board() );
        b.shutdown();
        BoardMonitorIsRunning = FALSE;
        udp_socket_shutdown();
        PostMessage( MainWindow, WM_CLOSE, 0, 0L );
        break;
        }

    status = WaitForSingleObject( BoardMonitorEvent, MS_TO_WAIT );
    if ( status == WAIT_FAILED )
        resource_message_box( MainInstance,  UNABLE_TO_CONTINUE_STRING, SHOTSAVE_WAIT_FAIL_STRING,  MB_OK | MB_SYSTEMMODAL );
    }

return 0;
}

/***********************************************************************
*                      START_AQUIRING_DATA_THREAD                      *
***********************************************************************/
static void start_aquiring_data_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) board_monitor_thread, /* thread function       */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, SHOTSAVE_THREAD_STRING, MB_OK | MB_SYSTEMMODAL );

}

/***********************************************************************
*                        GET_LOCK_ON_ALARM_MASK                        *
*  Make a mask to indicate alarm wires. If this is a mux system then   *
*  I check the alarm wires against this mask. If one is set then I     *
*  go into data collection mode instead of scan mode so I only monitor *
*  that machine until the alarm goes off.                              *
***********************************************************************/
static void get_lock_on_alarm_mask()
{

if ( *get_ini_string(MonallIniFile, ConfigSection, LockOnAlarmKey) == 'Y' )
    {
    LockOnAlarmMask = ~0;
    if ( DownTimeAlarmWire != NO_WIRE )
        LockOnAlarmMask &= !DownTimeAlarmWire;
    if ( MonitorWire != NO_WIRE )
        LockOnAlarmMask &= !MonitorWire;
    }

}

/***********************************************************************
*                           GET_MONITOR_WIRE                           *
* Get the monitor wire number from the monall.ini file. Make sure      *
* there is only one machine being monitored and that no one is         *
* using that wire already                                              *
***********************************************************************/
static void get_monitor_wire( void )
{
int32           i;
int32           j;
BOARD_CLASS     b;
short           wire_to_use;
TCHAR         * cp;
PART_NAME_ENTRY p;
PARAMETER_CLASS param;
STRING_CLASS    s;

DownTimeAlarmWire = NO_WIRE;
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DownTimeAlarmWire") );
if ( !unknown(cp) )
    {
    wire_to_use = asctoint32( cp );
    if ( wire_to_use > NO_WIRE )
        {
        if ( wire_to_use < LOW_MONITOR_WIRE || wire_to_use > HIGH_MONITOR_WIRE )
            {
            resource_message_box( MainInstance, UNABLE_TO_SET_DOWN_ALARM_WIRE_STRING, INVALID_WIRE_STRING, MB_OK | MB_SYSTEMMODAL );
            }
        else
            {
            DownTimeAlarmWire = wire_to_use;
            }
        }
    }

MonitorWire = NO_WIRE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("ClearMonitorWireOnDown") );
if ( *cp == YChar )
    ClearMonitorWireOnDown = TRUE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("ClearDownWireFromPopup") );
if ( *cp == YChar )
    ClearDownWireFromPopup = TRUE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("MonitorWire") );
if ( unknown(cp) )
    return;

wire_to_use = asctoint32( cp );

if (wire_to_use == NO_WIRE )
    return;

if ( wire_to_use < LOW_MONITOR_WIRE || wire_to_use > HIGH_MONITOR_WIRE )
    {
    resource_message_box( MainInstance, UNABLE_TO_SET_MONITOR_WIRE_STRING, INVALID_WIRE_STRING, MB_OK | MB_SYSTEMMODAL );
    return;
    }

b.rewind_machine_list();
cp = b.next_machine();
if ( !cp )
    {
    resource_message_box( MainInstance, UNABLE_TO_SET_MONITOR_WIRE_STRING, NOT_MONITORING_STRING, MB_OK | MB_SYSTEMMODAL );
    return;
    }

/*
--------------------------------
There should only be one machine
-------------------------------- */
if ( b.next_machine() )
    {
    resource_message_box( MainInstance, UNABLE_TO_SET_MONITOR_WIRE_STRING, MORE_THAN_ONE_MACHINE_STRING, MB_OK | MB_SYSTEMMODAL );
    return;
    }

/*
--------------------------------
Get the name of the current part
-------------------------------- */
lstrcpy( p.computer, ComputerName );
lstrcpy( p.machine, cp );
cp = current_part_name( p.computer, p.machine );
if ( !cp )
    {
    resource_message_box( MainInstance, UNABLE_TO_SET_MONITOR_WIRE_STRING, NO_CURRENT_PART_STRING, MB_OK | MB_SYSTEMMODAL );
    return;
    }

/*
-------------------------------------------------
See if this part uses the monitor wire for alarms
------------------------------------------------- */
lstrcpy( p.part, cp );
if ( param.find( p.computer, p.machine, p.part) )
    {
    for ( i=0; i<MAX_PARMS; i++ )
        {
        for ( j=0; j<NOF_ALARM_LIMIT_TYPES; j++ )
            {
            if ( param.parameter[i].limits[j].wire_number == wire_to_use )
                {
                s = resource_string( WIRE_USED_BY_ALARM_STRING );
                s += param.parameter[i].name;
                s += RightBracketString;
                MessageBox( NULL, resource_string(UNABLE_TO_SET_MONITOR_WIRE_STRING), s.text(), MB_OK | MB_SYSTEMMODAL );
                return;
                }
            }
        }
    }

MonitorWire = wire_to_use;
}

/***********************************************************************
*                          HARDWARESTATUSPROC                          *
***********************************************************************/
BOOL CALLBACK HardwareStatusProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static const COLORREF GreenColor = RGB( 0, 255, 0 );
static HBRUSH BackgroundBrush = 0;
static bool firstime = true;

int id;
BOARD_CLASS b;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        BackgroundBrush = CreateSolidBrush ( GreenColor );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_SHOWWINDOW:
        if ( ((BOOL) wParam) && lParam == 0 )   //wParam is TRUE if show, lParam is 0 of caused by ShowWindow
            {
            if ( firstime )
                firstime = false;
            else
                shrinkwrap( MainWindow, hWnd );
            }
        break;

    case WM_DESTROY:
        if ( BackgroundBrush )
            {
            DeleteObject( BackgroundBrush );
            BackgroundBrush = 0;
            }
        break;

    case WM_CTLCOLORSTATIC:
        HWND ws;
        ws = (HWND) lParam;
        id = GetDlgCtrlID( ws );
        if ( id >= LOW_BIT_RADIO_BUTTON && id <= HIGH_BIT_RADIO_BUTTON )
            {
            SetTextColor ((HDC) wParam, GreenColor );
            SetBkColor ((HDC) wParam, GreenColor );
            return (int) BackgroundBrush;
            }
        break;

    case WM_DBINIT:
        /* WaitForSingleObject( FasTrakMutex, INFINITE ); */
        init_status_bit_rectangles();
        StdDown.startup();
        b.startup();
        load_machines();
        if ( HaveEosUdp )
            {
            udp_socket_startup();
            }
        get_monitor_wire();
        if ( b.init_all_boards() )
            b.start_all_boards();
        get_lock_on_alarm_mask();
        /* ReleaseMutex( FasTrakMutex ); */
        start_aquiring_data_thread();
        return TRUE;

    case WM_COMMAND:
        switch (id)
            {
            case CANCEL_BUTTON:
               ShowWindow( HardwareStatusWindow, SW_HIDE );
               return TRUE;
            }
        break;

    }

return FALSE;
}
