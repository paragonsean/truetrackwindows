#define _CRTDBG_MAP_ALLOC
#include <windows.h>
#include <process.h>
#include <ddeml.h>
#include <crtdbg.h>

#include "..\include\visiparm.h"
#include "..\include\alarmsum.h"
#include "..\include\array.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\evclass.h"
#include "..\include\fileclas.h"
#include "..\include\stringcl.h"
#include "..\include\ftanalog.h"
#include "..\include\computer.h"
#include "..\include\fifo.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\nameclas.h"
#include "..\include\names.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\part_analog_sensor.h"
#include "..\include\rectclas.h"
#include "..\include\runlist.h"
#include "..\include\stddown.h"
#include "..\include\stpres.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\ftii.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

#include "extern.h"
#include "resource.h"
#include "shotsave.h"
#include "..\include\events.h"

const unsigned short FIRST_PACKET_NUMBER = 1;
const static int CTRL_PROG_NUMBER_LEN = 4;   /* Used to compare line numbers like .022 */

const BOARD_DATA VIS_AUTO_SHOT = 0x0400;
const BOARD_DATA VIS_TIME_OUT  = 0x0080;

static unsigned ALS_BITS            = 0x600E0000;
static unsigned FOLLOW_THROUGH_MASK = 0x40000;
static unsigned START_SHOT_MASK     = 0x20000;
static unsigned RETRACT_MASK        = 0x80000;
static unsigned ACCUMULATOR_MASK    = 0x40000000;
static unsigned SLOW_SHOT_MASK      = 0x20000000;

static HANDLE       SendEvent = 0;
static HANDLE       NewDataEvent = 0;

static bool     UseUploadRetractTimeout = false;
static bool     UseStartShotTimeout     = false;

static bool         Wsa_Is_Started = false;
static SOCKET       MySocket = INVALID_SOCKET;
static sockaddr_in  MyAddress;

static bool         Read_Thread_Is_Running     = false;
static bool         I_Am_Shutting_Down         = false;

static FIFO_CLASS   ReceivedAcks;
static FIFO_CLASS   AckList;
static FIFO_CLASS   NewData;

static FIFO_CLASS   SocketMessageStore;     /* Unused SOCKET_MESSAGEs */
static FIFO_CLASS   AckMessageStore;        /* Unused ACK_MESSAGEs    */

const  char AckChar  = '\006';
const  char CrChar   = '\r';
const  char NakChar  = '\025';
const  int  AckLen   = 2;
const  char PlusChar = '+';

const int      MAX_NAK_COUNT   = 3;
const int      MAX_MESSAGE_LEN = 1350;

struct ACK_MESSAGE {
    sockaddr_in a;
    char        s[AckLen];
    ACK_MESSAGE() { s[0]=AckChar; s[1]=CrChar; }
    };

struct SOCKET_MESSAGE {
    sockaddr_in a;
    char        s[MAX_MESSAGE_LEN+1];
    int         len;
    };

struct BOARD_EXTENDED_ANALOG_ENTRY {
    unsigned              mask;
    int                   channel;
    unsigned              trigger_type;
    BOOLEAN               waiting_for_value;
    EXTENDED_ANALOG_ENTRY ae;
    BOARD_EXTENDED_ANALOG_ENTRY() { mask=0; channel=NO_FT_CHANNEL; trigger_type=TRIGGER_WHEN_GREATER; waiting_for_value=FALSE; }
    };

class BOARD_EXTENDED_ANALOG_ARRAY {
    public:
    int n;
    BOARD_EXTENDED_ANALOG_ENTRY * array;

    BOARD_EXTENDED_ANALOG_ARRAY() { n = 0; array=0; }
    ~BOARD_EXTENDED_ANALOG_ARRAY();
    void clear();
    BOOLEAN init( int new_n );
    BOARD_EXTENDED_ANALOG_ENTRY & operator[]( int i );
    };

class FTII_BOARD_ENTRY {
    public:

    int             number;
    int             index;
    int             version;
    int             sub_version;
    int             shot_number;
    int             pc_retctl_dac;
    int             timer_frequency;
    bool            has_alarm;
    bool            has_warning;

    unsigned        alarm_bits;              /* Output bits mapped to alarms */
    unsigned        bits_currently_on;       /* Alarm + monitor wire + alarm_on_down */
    unsigned        config_word;
    unsigned        connect_state;
    unsigned        ms_at_last_contact;
    unsigned        do_not_upload_timeout;   /* ms = time when I can upload */
    unsigned        input_bits;

    int             down_state;
    int             down_timeout_seconds;
    TIME_CLASS      time_of_last_shot;
    BOARD_DATA      statusbits;

    STRING_CLASS    address;
    STRING_CLASS    port;
    STRING_CLASS    version_string;
    STRING_CLASS    upload_file_name;
    char            sample_type;    /* Type of binary data expected [P, T, C] */

    int             current_type;
    int             current_len;  /* Binary data only */
    int             chars_left_in_buffer;
    int             current_packet_number;
    char          * bp;
    char          * binarybuf;
    sockaddr_in     a;
    int             channel_mode;
    bool            do_not_upload;
    bool            have_response;   /* Used during upload to choose error message */
    bool            monitoring_was_stopped;
    bool            waiting_for_ack;
    bool            waiting_for_plus;
    bool            uploading;
    bool            using_plusses;
    bool            is_sending_op_data_packets;
    int             nak_count;
    int             bytes_uploaded;
    FIFO_CLASS      f;

    MACHINE_CLASS   m;
    PART_CLASS      part;
    PARAMETER_CLASS param;
    FTANALOG_CLASS  ftanalog;
    BOOLEAN         have_runlist;
    RUNLIST_CLASS   runlist;
    FTII_NET_DATA   shotdata;

    BOARD_EXTENDED_ANALOG_ARRAY extended_analog;

    FTII_BOARD_ENTRY();
    ~FTII_BOARD_ENTRY();
    void            check_inputs( TCHAR * sorc );
    void            check_for_extended_channel_value( TCHAR * sorc );
    void            clearwire( int wire_to_clear );
    void            get_alarm_bits();
    unsigned        get_parameter_alarm_bits();
    BOOLEAN         get_version_string();
    void            put_version_string();
    void            clear_all_alarm_bits();
    bool            upload_control_file();
    void            set_connect_state( unsigned new_state );
    void            set_extended_analog_bits();
    void            save_shot();
    void            add_binary_to_shotdata();
    void            get_config_word( TCHAR * cp );
    void            setwire( int wire_to_set );
    void            send_bits( unsigned bits, bool set_bits );
    void            send_config_word();
    bool            send_socket_message( TCHAR * sorc, bool need_to_set_event );
    bool            is_monitoring();
    };

FTII_BOARD_ENTRY * Boards    = 0;
int                NofBoards = 0;

static PART_ANALOG_SENSOR AnalogSensor;
static STDDOWN_CLASS      StdDown;

const TCHAR AsterixString[]  = TEXT( "*" );
const TCHAR CommaString[]  = TEXT( "," );
const TCHAR CrString[]     = TEXT( "\r" );
const TCHAR EqualsString[] = TEXT( "=" );
static TCHAR TimeoutString[]   = TEXT( "Timeout" );
const  TCHAR ConfigSetString[] = TEXT( "V313=H" );
static TCHAR IsControlSetString[] = TEXT( "V500=H" );
static TCHAR PrinceRodPitch[]  = TEXT( "100" );
static TCHAR PrinceSetString[] = TEXT( "V314=H" );
static TCHAR GetMSLString[]    = TEXT( "V319\r" );
static TCHAR GetVersionString[] = TEXT( "OV\r" );

static int PrinceCharOffset   = 13;          /* V314=H00000011 #Config word 2 */
static int PcRetctlDac4Offset =  9;          /* V314=H00010011 #Config word 2 */

const TCHAR BackslashChar    = TEXT( '\\'  );
const TCHAR CommaChar        = TEXT( ','  );
const TCHAR DecimalPointChar = TEXT( '.'  );
const TCHAR EqualsChar       = TEXT( '='  );
const TCHAR HChar            = TEXT( 'H'  );
const TCHAR NineChar         = TEXT( '9' );
const TCHAR NullChar         = TEXT( '\0' );
const TCHAR PoundChar        = TEXT( '#'  );
const TCHAR SpaceChar        = TEXT( ' '  );
const TCHAR TabChar          = TEXT( '\t' );
const TCHAR TwoChar          = TEXT( '2' );
const TCHAR ZeroChar         = TEXT( '0' );

static TCHAR EmptyString[]   = TEXT( "" );
static TCHAR NString[]       = TEXT( "N" );
static TCHAR YString[]       = TEXT( "Y" );
static TCHAR ChannelModeSetString[] = TEXT( "V429=" );
static TCHAR OpDataPacketString[]   = TEXT( "OC3="  );
static int   ChannelModeSetStringLen= 5;

static const TCHAR INPUT_BITS_RESPONSE[]    = TEXT( "O_ISW1=" );
static const int   INPUT_BITS_RESPONSE_SLEN = 7;

static char  ShutdownMessage[]      = "shutdown";
static int   ShutdownMessageLen     = 8;

static TCHAR  Turn_Off_Plusses[] = TEXT( "P0\r" );
static TCHAR  Turn_On_Plusses[]  = TEXT( "P1\r" );

extern HWND * Dnu;

static TCHAR GetTimerFrequencyString[] = TEXT( "V301\r" );
static TCHAR GetInputBitsString[]      = TEXT( "OI1\r"  );
static TCHAR GetAllAnalogInputs[]      = TEXT( "OAA\r"  );

double * calculate_ftii_parameters( FTII_PROFILE & f, MACHINE_CLASS & machine, PART_CLASS & part, PARAMETER_CLASS & param, PART_ANALOG_SENSOR & asensor, FTANALOG_CLASS & ftanalog );
TCHAR * resource_string( UINT resource_id );
void resource_message_box( UINT msg_id, UINT title_id );
void push_log_message( TCHAR * sorc );

/***********************************************************************
*                       ASCII_STRINGS_ARE_EQUAL                        *
***********************************************************************/
static BOOLEAN ascii_strings_are_equal( const char * s1, const char * s2, int slen )
{
while ( slen )
    {
    if ( *s1 != *s2 )
        return FALSE;
    if ( *s1 == '\0' )
        break;
    s1++;
    s2++;
    }

return TRUE;
}

/***********************************************************************
*                           ASCII_COPYSTRING                           *
***********************************************************************/
static void ascii_copystring( char * dest, const char * sorc, int maxlen )
{
while ( maxlen )
    {
    if ( *sorc == '\0' )
        break;
    *dest++ = *sorc++;
    maxlen--;
    }
*dest = '\0';
}

/***********************************************************************
*                           ASCII_COPYSTRING                           *
***********************************************************************/
static void ascii_copystring( char * dest, const char * sorc )
{

while ( true )
    {
    *dest = *sorc;
    if ( *dest == '\0' )
        break;
    dest++;
    sorc++;
    }
}

/***********************************************************************
*                          GET_SOCKET_MESSAGE                          *
***********************************************************************/
static SOCKET_MESSAGE * get_socket_message()
{
SOCKET_MESSAGE * p;

p = 0;
if ( SocketMessageStore.count() )
    p = (SOCKET_MESSAGE *) SocketMessageStore.pop();

if ( !p )
    p = new SOCKET_MESSAGE;

return p;
}

/***********************************************************************
*                           GET_ACK_MESSAGE                            *
***********************************************************************/
static ACK_MESSAGE * get_ack_message()
{
ACK_MESSAGE * p;

p = 0;
if ( AckMessageStore.count() )
    p = (ACK_MESSAGE *) AckMessageStore.pop();

if ( !p )
    p = new ACK_MESSAGE;

return p;
}

/***********************************************************************
*                         CLEAN_MESSAGE_STORES                         *
***********************************************************************/
static void clean_message_stores()
{
SOCKET_MESSAGE * sp;
ACK_MESSAGE    * ap;

while ( SocketMessageStore.count() )
    {
    sp = (SOCKET_MESSAGE *) SocketMessageStore.pop();
    delete sp;
    }

while ( AckMessageStore.count() )
    {
    ap = (ACK_MESSAGE *) AckMessageStore.pop();
    delete ap;
    }
}

#ifdef UNICODE
/***********************************************************************
*                        START_CHARS_ARE_EQUAL                         *
***********************************************************************/
bool start_chars_are_equal( const TCHAR * sorc, const char * s )
{
union {
char  c;
TCHAR t;
};

t = NullChar;

while ( true )
    {
    if ( *sorc == NullChar )
        return true;

    c = *s;

    if ( *sorc != t )
        return false;

    sorc++;
    s++;
    }
}
#endif

/***********************************************************************
*                        START_CHARS_ARE_EQUAL                         *
***********************************************************************/
bool start_chars_are_equal( const TCHAR * sorc, const TCHAR * s )
{
while ( true )
    {
    if ( *sorc == NullChar )
        return true;

    if ( *sorc != *s )
        return false;

    sorc++;
    s++;
    }
}

/***********************************************************************
*                             BOARD_INDEX                              *
***********************************************************************/
static int board_index( sockaddr_in & sorc )
{
int i;

for ( i=0; i<NofBoards; i++ )
    {
    if ( Boards[i].a.sin_addr.S_un.S_addr == sorc.sin_addr.S_un.S_addr )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                             BOARD_INDEX                              *
***********************************************************************/
static int board_index( const FTII_BOARD_ENTRY * board_to_find )
{
int i;

i = board_to_find - Boards;

if ( i>=0 && i<NofBoards )
    return i;

return NO_INDEX;
}

/***********************************************************************
*                             BOARD_INDEX                              *
***********************************************************************/
static int board_index( const TCHAR * machine_name )
{
int i;

for ( i=0; i<NofBoards; i++ )
    {
    if ( strings_are_equal(machine_name, Boards[i].m.name) )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                            FIND_BOARD                                *
***********************************************************************/
FTII_BOARD_ENTRY * find_board( sockaddr_in & sorc )
{
int i = board_index( sorc );

if ( i != NO_INDEX )
    return &Boards[i];

return 0;
}

/***********************************************************************
*                         CURRENT_BOARD_NUMBER                         *
***********************************************************************/
int current_board_number()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].number;

return NO_BOARD_NUMBER;
}

/***********************************************************************
*                         CURRENT_BOARD_INDEX                          *
***********************************************************************/
int current_board_index()
{
return board_index( CurrentMachine.name );
}

/***********************************************************************
*                        IS_SURETRAK_CONTROLLED                        *
*  This allows the main ft2 thread to check the type of machine.       *
***********************************************************************/
BOOLEAN is_suretrak_controlled( int i )
{
if ( i>=0 && i<NofBoards )
    return Boards[i].m.suretrak_controlled;

return FALSE;
}

/***********************************************************************
*                         CURRENT_CHANNEL_MODE                         *
***********************************************************************/
int current_channel_mode()
{
int bi;
bi = board_index( CurrentMachine.name );
if ( bi != NO_INDEX )
    return Boards[bi].channel_mode;

return CHANNEL_MODE_1;
}

/***********************************************************************
*                     CURRENT_VERSION_STRING                           *
***********************************************************************/
TCHAR * current_version_string()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].version_string.text();

return EmptyString;
}

/***********************************************************************
*                       CURRENT_UPLOAD_FILE_NAME                       *
***********************************************************************/
TCHAR * current_upload_file_name()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].upload_file_name.text();

return EmptyString;
}

/***********************************************************************
*                         CURRENT_VERSION                              *
***********************************************************************/
int current_version()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].version;

return 0;
}

/***********************************************************************
*                       CURRENT_SUB_VERSION                            *
***********************************************************************/
int current_sub_version()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].sub_version;

return 0;
}

/***********************************************************************
*                             ADD_BOARD_DATA                           *
***********************************************************************/
static TCHAR * add_board_data( TCHAR * dest, BOARD_DATA x )
{
insalph( dest, (int32) x, BOARD_DATA_HEX_LEN, ZeroChar, HEX_RADIX );
dest += BOARD_DATA_HEX_LEN;

*dest = TabChar;
dest++;
return dest;
}

/***********************************************************************
*                               WIREBIT                                *
***********************************************************************/
unsigned wirebit( int wire_number )
{
unsigned x;

x = 1;
wire_number--;
if ( wire_number > 0 )
    x <<= wire_number;

return x;
}

/***********************************************************************
*                   SEND_MACHINE_STATE_EVENT_STRING                    *
*                                                                      *
*                         "M01\t00F1\t00FF"                            *
***********************************************************************/
static void send_machine_state_event_string( TCHAR * machine, BOARD_DATA old_status, BOARD_DATA new_status )
{
const int DDE_LINE_LEN = MACHINE_NAME_LEN + 1 + BOARD_DATA_HEX_LEN + 1 + BOARD_DATA_HEX_LEN + 1;

TCHAR * buf;
TCHAR * cp;

buf = maketext( DDE_LINE_LEN );
if ( !buf )
    return;

cp = buf;
cp = copy_w_char( cp, machine, TabChar );
cp = add_board_data( cp, old_status );
cp = add_board_data( cp, new_status );
cp--;
*cp = NullChar;

MainWindow.post( WM_POKEMON, (WPARAM) NEW_MACH_STATE_INDEX, (LPARAM) buf );
}

/***********************************************************************
*                        UPDATE_MONITOR_STATUS                         *
***********************************************************************/
static void update_monitor_status( FTII_BOARD_ENTRY * board, TCHAR * s )
{
static TCHAR CycleString[] = TEXT( "Cycle" );
static BOOLEAN CycleStartIsVisible = FALSE;

HWND     w;
int      i;
BOARD_DATA old_status;

i = board_index( board );

if ( i != NO_INDEX )
    {
    if ( i < NofRows )
        set_text( MonitorStatusTbox[i].w, s );

    old_status = board->statusbits;
    if ( findstring( CycleString, s) )
        {
        w = AutoShotDisplayWindow.control( AUTO_SHOT_MACHINE_NUMBER_TBOX );
        set_text( w, board->m.name );
        AutoShotDisplayWindow.show();
        CycleStartIsVisible = TRUE;
        board->statusbits |= VIS_AUTO_SHOT;
        board->statusbits &= ~VIS_TIME_OUT;
        }
    else
        {
        if ( CycleStartIsVisible )
            {
            CycleStartIsVisible = FALSE;
            AutoShotDisplayWindow.hide();
            }

        if ( findstring(TimeoutString, s) )
            {
            board->statusbits &= ~VIS_AUTO_SHOT;
            board->statusbits |=  VIS_TIME_OUT;
            }
        else
            {
            board->statusbits &= ~(VIS_AUTO_SHOT | VIS_TIME_OUT);
            }
        }

    if ( old_status != board->statusbits )
        send_machine_state_event_string( board->m.name, old_status, board->statusbits );
    }
}

/***********************************************************************
*                    BOARD_EXTENDED_ANALOG_ARRAY                       *
***********************************************************************/
BOARD_EXTENDED_ANALOG_ARRAY::~BOARD_EXTENDED_ANALOG_ARRAY()
{
if ( array )
    {
    n = 0;
    delete[] array;
    array = 0;
    }
}

/***********************************************************************
*                    BOARD_EXTENDED_ANALOG_ARRAY                       *
*                              clear                                   *
***********************************************************************/
void BOARD_EXTENDED_ANALOG_ARRAY::clear()
{
if ( array )
    {
    n = 0;
    delete[] array;
    array = 0;
    }
}

/***********************************************************************
*                    BOARD_EXTENDED_ANALOG_ARRAY                       *
*                              init                                    *
***********************************************************************/
BOOLEAN BOARD_EXTENDED_ANALOG_ARRAY::init( int new_n )
{
int i;

if ( new_n != n )
    {
    clear();
    if ( new_n > 0 )
        {
        array = new BOARD_EXTENDED_ANALOG_ENTRY[new_n];
        if ( array )
            n = new_n;
        else
            return FALSE;
        }
    }
else
    {
    for ( i=0; i<n; i++ )
        {
        array[i].mask = 0;
        array[i].ae.parameter_number = NO_PARAMETER_NUMBER;
        }
    }

return TRUE;
}

/***********************************************************************
*                    BOARD_EXTENDED_ANALOG_ARRAY                       *
*                               []                                     *
***********************************************************************/
BOARD_EXTENDED_ANALOG_ENTRY & BOARD_EXTENDED_ANALOG_ARRAY::operator[]( int i )
{
static BOARD_EXTENDED_ANALOG_ENTRY empty;

if ( array && i>=0 && i< n )
    return array[i];

return empty;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
***********************************************************************/
FTII_BOARD_ENTRY::FTII_BOARD_ENTRY()
{
index                  = 0;
version                = 0;
sub_version            = 0;
connect_state          = NO_FT2_CONNECT_STATE;
pc_retctl_dac          = 0;
ms_at_last_contact     = 0;
channel_mode           = CHANNEL_MODE_1;
uploading              = false;
using_plusses          = false;
bytes_uploaded         = 0;
current_type           = ASCII_TYPE;
current_len            = 0;
chars_left_in_buffer   = 0;
current_packet_number  = 0;
bp                     = 0;
binarybuf              = 0;
a.sin_family           = PF_INET;
a.sin_port             = 0;
a.sin_addr.S_un.S_addr = 0;
number                 = NO_BOARD_NUMBER;
nak_count              = 0;
do_not_upload          = false;
do_not_upload_timeout  = 0;
input_bits             = 0;
monitoring_was_stopped = false;
waiting_for_ack        = false;
waiting_for_plus       = false;
is_sending_op_data_packets = false;
statusbits             = 0;
shot_number            = 0;
down_state             = NO_DOWN_STATE;
down_timeout_seconds   = 0;
time_of_last_shot      = 0;
timer_frequency        = 16666667;
config_word            = 0x3110FFFF;
alarm_bits             = 0;
bits_currently_on      = 0;
has_alarm              = false;
has_warning            = false;
have_runlist           = FALSE;
have_response          = false;
}

/***********************************************************************
*                          ~FTII_BOARD_ENTRY                           *
***********************************************************************/
FTII_BOARD_ENTRY::~FTII_BOARD_ENTRY()
{
if ( binarybuf )
    {
    delete[] binarybuf;
    binarybuf = 0;
    }
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                           SET_CONNECT_STATE                          *
***********************************************************************/
void FTII_BOARD_ENTRY::set_connect_state( unsigned new_state )
{
int      i;
unsigned b;

b = 0;
if ( connect_state != new_state )
    {
    b = connect_state & UPLOAD_WAS_ABORTED_STATE;
    connect_state = new_state;
    if ( (new_state & CONNECTING_STATE) || (new_state & NOT_CONNECTED_STATE) )
        connect_state |= b;
    i = board_index( this );
    /*
    ------------------------------------------------------------------------------------
    If the last upload was aborted, tell the main proc so he can ask a human to reupload
    ------------------------------------------------------------------------------------ */
    if ( new_state & CONNECTED_STATE )
        new_state |= b;
    MainWindow.post( WM_NEW_FT2_STATE, (WPARAM) i, (LPARAM) new_state );
    }
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                           SEND_CONFIG_WORD                           *
***********************************************************************/
void FTII_BOARD_ENTRY::send_config_word()
{
const int max_slen = 16;
STRING_CLASS s;

s.upsize( max_slen );
s = ConfigSetString;

s += ultohex( (unsigned long) config_word );
s += CrString;
send_socket_message( s.text(), true );
}


/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                          GET_VERSION_STRING                          *
* This function retrieves the last saved version string from the       *
* ftii_editor_settings.ini file. It does not set the version and       *
* sub_version variables as these are used to tell if the board         *
* has sent a version string.                                           *
***********************************************************************/
BOOLEAN FTII_BOARD_ENTRY::get_version_string()
{
TCHAR      c;
NAME_CLASS s;
INI_CLASS  ini;

s.get_ft2_editor_settings_file_name( m.computer, m.name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(FlashVersionKey) )
    {
    s = ini.get_string();
    c = *s.text();
    if ( is_numeric(c) )
        {
        version_string.acquire( s.release() );
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                          PUT_VERSION_STRING                          *
***********************************************************************/
void FTII_BOARD_ENTRY::put_version_string()
{
NAME_CLASS s;
INI_CLASS  ini;

s.get_ft2_editor_settings_file_name( m.computer, m.name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
ini.put_string( FlashVersionKey, version_string.text() );
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                            IS_MONITORING                             *
***********************************************************************/
bool FTII_BOARD_ENTRY::is_monitoring()
{
if ( m.monitor_flags & MA_MONITORING_ON )
    return true;

return false;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                           get_config_word                            *
*   This assumes that the number is in hex and begins with 'H'.        *
***********************************************************************/
void FTII_BOARD_ENTRY::get_config_word( TCHAR * sorc )
{
TCHAR * cp;

cp = findchar( HChar, sorc );
if ( cp )
    {
    cp++;
    config_word = (unsigned) hextoul( cp );
    }
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                        add_binary_to_shotdata                        *
***********************************************************************/
void FTII_BOARD_ENTRY::add_binary_to_shotdata()
{
int nof_samples;

switch ( current_type )
    {
    case SAMPLE_TYPE:
        /*
        ---------------------------------------------------
        The length is in bytes but I need number of samples
        --------------------------------------------------- */
        nof_samples = current_len / sizeof( FTII_SAMPLE );
        if ( sample_type == 'P' )
            {
            time_of_last_shot.get_local_time();
            shotdata.set_shot_time( time_of_last_shot.file_time() );
            shotdata.set_position_based_points( (FTII_SAMPLE *) binarybuf, nof_samples );
            }
        else
            {
            shotdata.set_time_based_points( (FTII_SAMPLE *) binarybuf, nof_samples );
            }
        binarybuf = 0;
        break;

    case PARAMETERS_TYPE:
        shotdata.set_parameters( (FTII_PARAMETERS *) binarybuf );
        binarybuf = 0;
        save_shot();
        break;
    }
}

/***********************************************************************
*                 CHECK_FOR_EXTENDED_CHANNEL_VALUE                     *
*              The sorc is a line like "O_CH12=xxxx"                   *
***********************************************************************/
void FTII_BOARD_ENTRY::check_for_extended_channel_value( TCHAR * sorc )
{
int i;
TCHAR * cp;

for ( i=0; i<extended_analog.n; i++ )
    {
    if ( extended_analog[i].waiting_for_value )
        {
        cp = sorc + 4;
        if ( extended_analog[i].channel == asctoint32(cp) )
            {
            cp = findchar( EqualsChar, cp );
            if ( cp )
                {
                cp++;
                extended_analog[i].ae.value = (unsigned short) hextoul(cp);
                }
            extended_analog[i].waiting_for_value = FALSE;
            }
        }
    }
}

/***********************************************************************
*                               BIT_STATE                              *
***********************************************************************/
static TCHAR * bit_state( unsigned x )
{
static TCHAR On[] = TEXT(" ON ");
static TCHAR Off[] = TEXT(" OFF");
if ( x )
    return On;
return Off;
}

/***********************************************************************
*                                MAKE_LINE                             *
***********************************************************************/
static void make_line( STRING_CLASS & s, unsigned x, TIME_CLASS & t )
{
s += bit_state( x );
s += TEXT( " at time: " );
s += t.hhmmss();
}

/***********************************************************************
*                           WRITE_ALS_BIT_CHANGES                      *
***********************************************************************/
static void write_als_bit_changes( unsigned new_bits, unsigned old_bits )
{
FILE_CLASS f;
TIME_CLASS t;
STRING_CLASS s;

if ( !AlsLogIsActive )
    return;

t.get_local_time();

if ( !f.open_for_append(AlsLogFile) )
    return;

if ( (new_bits & RETRACT_MASK) != (old_bits & RETRACT_MASK) )
    {
    s = TEXT( "Retract        INPUT" );
    make_line( s, (new_bits & RETRACT_MASK), t );
    f.writeline( s.text() );
    }
if ( (new_bits & START_SHOT_MASK) != (old_bits & START_SHOT_MASK) )
    {
    s = TEXT( "Start Shot     INPUT" );
    make_line( s, (new_bits & START_SHOT_MASK), t );
    f.writeline( s.text() );
    }
if ( (new_bits & FOLLOW_THROUGH_MASK) != (old_bits & FOLLOW_THROUGH_MASK) )
    {
    s = TEXT( "Follow Through INPUT" );
    make_line( s, (new_bits & FOLLOW_THROUGH_MASK), t );
    f.writeline( s.text() );
    }

if ( (new_bits & ACCUMULATOR_MASK) != (old_bits & ACCUMULATOR_MASK) )
    {
    s = TEXT( "Accumulator    INPUT" );
    make_line( s, (new_bits & ACCUMULATOR_MASK), t );
    f.writeline( s.text() );
    }

if ( (new_bits & SLOW_SHOT_MASK) != (old_bits & SLOW_SHOT_MASK) )
    {
    s = TEXT( "Slow Shot      INPUT" );
    make_line( s, (new_bits & SLOW_SHOT_MASK), t );
    f.writeline( s.text() );
    }

f.close();
}

/***********************************************************************
*                            WRITE_SENT_COMMAND                        *
***********************************************************************/
static void write_sent_command( int board_number, char * sorc )
{
FILE_CLASS f;
TIME_CLASS t;
STRING_CLASS s;
STRING_CLASS as;

if ( !as.get_ascii(sorc) )
    return;

as.strip_crlf();

t.get_local_time();

if ( !f.open_for_append(SentCommandsLogFile) )
    return;

s = board_number;
s += SpaceChar;
s += t.hhmmss();
s += SpaceChar;
s += as;

f.writeline( s.text() );
f.close();
}

/***********************************************************************
*                            CHECK_INPUTS                              *
***********************************************************************/
void FTII_BOARD_ENTRY::check_inputs( TCHAR * sorc )
{
static TCHAR getanalog[] = TEXT( "OAXX\r" );
TCHAR * cp;
TCHAR * buf;
int      i;
BOOLEAN  need_message;
unsigned additional_ms;
unsigned mask;
unsigned u;

u = hextoul( sorc );

if ( have_runlist )
    {
    cp = runlist.find_part( u );

    if ( *cp != NullChar && !strings_are_equal(cp, part.name) )
        {
        if ( part_exists( part.computer, part.machine, cp) )
            {
            set_current_part_name( part.computer, part.machine, cp );
            buf = maketext( part.machine );
            if ( buf )
                MainWindow.post( WM_POKEMON, (WPARAM) MONITOR_SETUP_INDEX, (LPARAM) buf );
            }
        else
            {
            MessageBox( 0, resource_string(NO_WARMUP_PART_STRING), cp, MB_OK | MB_SYSTEMMODAL );
            }
        }
    }

/*
--------------------------------------------------------------------------------------
If the NoUploadMask is set then I am using an input instead of the cycle start message
The bit must be clear in the saved input_bits before I disable uploads.
-------------------------------------------------------------------------------------- */
if ( m.suretrak_controlled && NoUploadWireMask && !do_not_upload )
    {
    if ( (u & NoUploadWireMask) && !(u & input_bits) )
        {
        do_not_upload = true;
        if ( UseStartShotTimeout )
            {
            do_not_upload_timeout = GetTickCount();
            additional_ms = m.autoshot_timeout_seconds;
            additional_ms += 5;
            additional_ms *= 1000;
            do_not_upload_timeout += additional_ms;
            }
        set_text( Dnu[index], AsterixString );
        }
    }

if ( do_not_upload && UseUploadRetractTimeout )
    {
    if ( u & RetractWireMask )
        {
        /*
        ------------------------------------------
        Wait 2 more seconds for the rod to retract
        before beginning an upload.
        ------------------------------------------ */
        do_not_upload_timeout = GetTickCount();
        do_not_upload_timeout += 2000;
        }
    }

/*
--------------------------------------------------------------------
See if any of the extended analog inputs have changed. If so, send a
OAy command to get the analog value for that channel..
-------------------------------------------------------------------- */
for ( i=0; i<extended_analog.n; i++ )
    {
    need_message = FALSE;
    if ( !extended_analog[i].waiting_for_value )
        {
        mask = extended_analog[i].mask;
        if ( extended_analog[i].trigger_type == TRIGGER_WHEN_GREATER )
            {
            if ( (mask & u) && !(mask & input_bits ) )
                need_message = TRUE;
            }
        else
            {
            if ( !(mask & u) && (mask & input_bits ) )
                need_message = TRUE;
            }
        }

    if ( need_message )
        {
        extended_analog[i].waiting_for_value = TRUE;
        cp = getanalog + 2;
        cp = copystring( cp, int32toasc((int32)extended_analog[i].channel) );
        copystring( cp, CrString );
        send_socket_message( getanalog, true );
        }
    }

if ( (u & ALS_BITS) != (input_bits & ALS_BITS) )
    write_als_bit_changes( u, input_bits );

input_bits = u;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                         SEND_SOCKET_MESSAGE                          *
***********************************************************************/
bool FTII_BOARD_ENTRY::send_socket_message( TCHAR * sorc, bool need_to_set_event )
{
SOCKET_MESSAGE * mp;
union
{
char c;
TCHAR t;
};
int i;

mp = get_socket_message();

mp->a.sin_family           = PF_INET;
mp->a.sin_port             = a.sin_port;
mp->a.sin_addr.S_un.S_addr = a.sin_addr.S_un.S_addr;

if ( sorc )
    {
    if ( start_chars_are_equal(OpDataPacketString, sorc) )
        {
        if ( *(sorc+4) == ZeroChar )
            is_sending_op_data_packets = false;
        else if ( *(sorc+4) == TwoChar )
            is_sending_op_data_packets = true;
        }

    /*
    ------------------------------------------
    Copy the tchar message to the char buffer.
    ------------------------------------------ */
    mp->len = lstrlen( sorc );
    if ( mp->len > MAX_MESSAGE_LEN )
        mp->len = MAX_MESSAGE_LEN;
    for ( i=0; i<mp->len; i++ )
        {
        t = sorc[i];
        mp->s[i] = c;
        };
    mp->s[mp->len] = '\0';
    }
else
    {
    /*
    -----------
    Null Packet
    ----------- */
    *(mp->s) = '\0';
    mp->len  = 0;
    }

f.push( mp );
if ( need_to_set_event )
    SetEvent( SendEvent );

return true;
}

/***********************************************************************
*                     SET_EXTENDED_ANALOG_BITS                         *
************************************************************************/
void FTII_BOARD_ENTRY::set_extended_analog_bits()
{
int i;
int n;
int nof_params;
int wire_number;
int input_type;

/*
-----------------------------------------------------------------
Count the number of extended analogs that use inputs for triggers
----------------------------------------------------------------- */
n = 0;
nof_params = param.count();
for ( i=0; i<nof_params; i++ )
    {
    if ( param.parameter[i].input.type == FT_EXTENDED_ANALOG_INPUT )
        {
        if ( ftanalog.array[i].ind_value.int_value() > 0 )
            n++;
        }
    }

if ( !n )
    return;

extended_analog.init( n );
n = 0;
for ( i=0; i<nof_params; i++ )
    {
    input_type = param.parameter[i].input.type;

    if ( input_type == FT_EXTENDED_ANALOG_INPUT )
        {
        wire_number = ftanalog.array[i].ind_value.int_value();
        if ( wire_number > 0 )
            {
            extended_analog[n].mask = wirebit( wire_number );
            extended_analog[n].ae.parameter_number = i+1;
            extended_analog[n].trigger_type = (unsigned) ftanalog.array[i].ind_var_type;
            extended_analog[n].channel      = (int)      ftanalog.array[i].channel;
            n++;
            }
        }
    }
}

/***********************************************************************
*          FTII_BOARD_ENTRY::GET_PARAMETER_ALARM_BITS                  *
***********************************************************************/
unsigned FTII_BOARD_ENTRY::get_parameter_alarm_bits()
{
int i;
int j;
unsigned bits;
PARAMETER_ENTRY * p;

bits = 0;
for ( i=0; i<MAX_PARMS; i++ )
    {
    p = param.parameter + i;
    if ( p->input.type != NO_PARAMETER_TYPE )
        {
        if ( p->has_warning_limits() )
            {
            for ( j=WARNING_MIN; j<=WARNING_MAX; j++ )
                {
                if ( p->limits[j].wire_number != NO_WIRE )
                    bits |= wirebit( p->limits[j].wire_number );
                }
            }
        if ( p->has_alarm_limits() )
            {
            for ( j=ALARM_MIN; j<=ALARM_MAX; j++ )
                {
                if ( p->limits[j].wire_number != NO_WIRE )
                    bits |= wirebit( p->limits[j].wire_number );
                }
            }
        }
    }

return bits;
}

/***********************************************************************
*                   FTII_BOARD_ENTRY::GET_ALARM_BITS                   *
***********************************************************************/
void FTII_BOARD_ENTRY::get_alarm_bits()
{
alarm_bits = get_parameter_alarm_bits();

if ( part.good_shot_wire != NO_WIRE )
    alarm_bits |= wirebit( part.good_shot_wire );
}

/***********************************************************************
*                        CLEAR_ALL_ALARM_BITS                          *
*  This searches for all the alarm wires I am controlling and turns    *
*  them off.                                                           *
************************************************************************/
void FTII_BOARD_ENTRY::clear_all_alarm_bits()
{
send_bits( alarm_bits, false );
bits_currently_on &= ~alarm_bits;
}

/***********************************************************************
*                              SEND_BITS                               *
* I was going to change (26-Sep-05) so that I send a separate          *
* message for each bit. I've commented out the changes as I decided    *
* that change was unneccessary but I may change my mind.               *
*                                                                      *
* Note: this does not affect the bits_currenly_on, as this is most     *
* often used to send that value.                                       *
************************************************************************/
void FTII_BOARD_ENTRY::send_bits( unsigned bits, bool set_bits )
{
static TCHAR OnString[]  = TEXT("OO1=" );
static TCHAR OffString[] = TEXT("OO1=~" );
TCHAR s[30];
TCHAR * cp;
//TCHAR * dest;
//unsigned u;
//int      i;

if ( set_bits )
    cp = copystring( s, OnString );
else
    cp = copystring( s, OffString );

cp = copystring( cp, ultohex((unsigned long) bits) );

if ( NeedErrorLog )
    push_log_message( s );

cp = copystring( cp, CrString );
send_socket_message( s, true );

/*
-------------------------------------------------------------
u = 1;
for ( i=0; i<32; i++ )
    {
    if ( u & bits )
        {
        cp = copystring( dest, ultohex((unsigned long) u) );
        cp = copystring( cp, CrString );
        send_socket_message( s, true );

        }
    u <<= 1;
    }
------------------------------------------------------------- */
}

/***********************************************************************
*                             CLEARWIRE                                *
***********************************************************************/
void FTII_BOARD_ENTRY::clearwire( int wire_to_clear )
{
unsigned u;
u = wirebit( wire_to_clear );
send_bits( u, false  );
bits_currently_on &= ~u;
}

/***********************************************************************
*                              SETWIRE                                 *
***********************************************************************/
void FTII_BOARD_ENTRY::setwire( int wire_to_set )
{
unsigned u;
u = wirebit( wire_to_set );
send_bits( u, true  );
bits_currently_on |= u;
}

/***********************************************************************
*                           CHECK_ONE_ALARM                            *
***********************************************************************/
void check_one_alarm( bool & is_alarm, unsigned & setbits, float value, float low, short low_wire, float high, float high_wire )
{
/*
----------------------------------------------------
If high and low are the same I should not check this
---------------------------------------------------- */
if ( not_float_zero(high-low) )
    {
    if ( value < low )
        {
        if ( not_float_zero(low-value) )
            {
            is_alarm = true;
            if ( low_wire != NO_WIRE )
                setbits |= wirebit(low_wire);
            }
        }
    else if ( value > high )
        {
        if ( not_float_zero(value-high) )
            {
            is_alarm = true;
            if ( high_wire != NO_WIRE )
                setbits |= wirebit(high_wire);
            }
        }
    }
}

/***********************************************************************
*                             CHECK_ALARMS                             *
***********************************************************************/
static void check_alarms( SYSTEMTIME & st, FTII_BOARD_ENTRY * board, double * dp )
{
static float fp[MAX_PARMS];
short      i;
short      shift_number;
SYSTEMTIME shift_date;
unsigned   clearbits;
unsigned   setbits;
bool       this_is_a_good_shot;

ALARM_SUMMARY_CLASS as;
PARAMETER_CLASS   * p;
PARAMETER_ENTRY   * pe;

/*
    sp[0].value += CommaString;
--------------------------------------------
Do nothing if alarms are not being monitored
-------------------------------------------- */
if ( !board->is_monitoring() )
    return;

if ( !(board->m.monitor_flags & MA_ALARM_MONITORING_ON) )
    return;

p = &board->param;

if ( !p )
    return;

get_shift_number_and_date( &shift_number, &shift_date, st );

board->has_alarm   = false;
board->has_warning = false;

/*
----------------------------------------------------
Use the array of floats to send to the alarm summary
---------------------------------------------------- */
for ( i=0; i<MAX_PARMS; i++ )
    fp[i] = (float) dp[i];

as.set_part( board->part.computer, board->part.machine, board->part.name );
as.set_counts( fp, p );
as.save( shift_date, (int32) shift_number );

setbits = 0;

for ( i=0; i<MAX_PARMS; i++ )
    {
    pe = p->parameter + i;
    if ( pe->input.type != NO_PARAMETER_TYPE )
        {
        check_one_alarm( board->has_alarm,   setbits, fp[i], pe->limits[ALARM_MIN].value,   pe->limits[ALARM_MIN].wire_number,   pe->limits[ALARM_MAX].value,   pe->limits[ALARM_MAX].wire_number );
        check_one_alarm( board->has_warning, setbits, fp[i], pe->limits[WARNING_MIN].value, pe->limits[WARNING_MIN].wire_number, pe->limits[WARNING_MAX].value, pe->limits[WARNING_MAX].wire_number );
        }
    }

if ( board->part.good_shot_wire != NO_WIRE )
    {
    this_is_a_good_shot = true;
    if ( board->has_alarm )
        this_is_a_good_shot = false;
    if ( board->part.good_shot_requires_no_warnings && board->has_warning )
        this_is_a_good_shot = false;
    if ( this_is_a_good_shot )
        setbits |= wirebit(board->part.good_shot_wire);
    }

board->bits_currently_on &= ~board->alarm_bits;
board->bits_currently_on |= setbits;
if ( board->bits_currently_on )
    board->send_bits( board->bits_currently_on, true );

clearbits = board->alarm_bits ^ setbits;
if ( clearbits )
    board->send_bits( clearbits, false );
}

/***********************************************************************
*                           CURRENT_OPERATOR                           *
***********************************************************************/
TCHAR * current_operator( TCHAR * computer, TCHAR * machine )
{
static TCHAR opnumber[OPERATOR_NUMBER_LEN+1];
NAME_CLASS n;
DB_TABLE   t;

n.get_machset_file_name( computer );

lstrcpy( opnumber, NO_OPERATOR_NUMBER );

if ( t.open(n.text(), MACHSET_RECLEN, PFL) )
    {
    t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, machine, MACHINE_NAME_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        t.get_alpha( opnumber, MACHSET_OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
    t.close();
    }

return opnumber;
}

/***********************************************************************
*                        ISSUE_DOWNTIME_EVENT                          *
***********************************************************************/
void issue_downtime_event( TCHAR * computer, TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat )
{
TCHAR * cp;
TCHAR * worker;

worker = current_operator( computer, machine );

cp = make_downtime_event_string( machine, part, st, cat, subcat, worker );
if ( cp )
    MainWindow.post( WM_POKEMON, (WPARAM) DOWN_DATA_INDEX, (LPARAM) cp );
}

/***********************************************************************
*                        APPEND_DOWNTIME_RECORD                        *
***********************************************************************/
BOOLEAN append_downtime_record( TCHAR * computer, TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat )
{
NAME_CLASS n;
DB_TABLE   t;
TCHAR    * worker;
BOOLEAN    status;

status = FALSE;

worker = current_operator( computer, machine );

n.get_machine_results_dir_file_name( computer, machine, DOWNTIME_DB );

if ( t.open(n.text(), DOWNTIME_RECLEN, PWL) )
    {
    make_downtime_record( t, st, cat, subcat, worker, part );
    t.rec_append();
    t.close();
    status = TRUE;
    }

issue_downtime_event( computer, machine, part, st, cat, subcat );

return status;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                              SAVE_SHOT                               *
***********************************************************************/
void FTII_BOARD_ENTRY::save_shot()
{
double           * dp;
FTII_FILE_DATA     filedata;
FTII_PROFILE       fp;
SYSTEMTIME         st;
PART_NAME_ENTRY    pn;
DOUBLE_ARRAY_CLASS pa;
DSTAT_CLASS        d;
int                i;
unsigned           u;

/*
--------------------------------------------------------------------------------------
If either the position data or time data are missing I must have lost some packets and
can't save the file.
-------------------------------------------------------------------------------------- */
if ( !shotdata.position_sample || !shotdata.time_sample )
    return;

pn.set( m.computer, m.name, m.current_part );

update_monitor_status( this, resource_string(SAVING_SHOT_STRING) );
FileTimeToSystemTime( &shotdata.shot_time, &st );
filedata = shotdata;
if ( extended_analog.n > 0 )
    {
    filedata.set_nof_extended_analogs( extended_analog.n );
    if ( filedata.extended_analog )
        {
        for ( i=0; i<extended_analog.n; i++ )
            {
            filedata.extended_analog[i] = extended_analog[i].ae;
            extended_analog[i].ae.value = 0;   /* Zero the value so it won't show up next time. */
            }
        }
    }

fp = filedata;
dp = calculate_ftii_parameters( fp, m, part, param, AnalogSensor, ftanalog );
if ( dp )
    {
    check_alarms( st, this, dp );
    pa.init( MAX_PARMS, dp );
    dp = 0;
    /*
    -----------------------------------------------
    This will transfer the data from filedata and a
    ----------------------------------------------- */
    add_to_shotsave( has_alarm, has_warning, pn, filedata, pa );
    SetEvent( ShotSaveEvent );
    }
shotdata.clear();

if ( down_state != MACH_UP_STATE )
    {
    d.set_cat( DOWNCAT_SYSTEM_CAT );
    d.set_subcat( DOWNCAT_UP_SUBCAT );
    d.set_time( st );
    d.put( part.computer, part.machine );
    down_state = d.down_state();

    /*
    -----------------------------------------------------------
    Only write to the downtime file if the cycle timeout is set
    ----------------------------------------------------------- */
    if ( down_timeout_seconds > 0 )
        append_downtime_record( part.computer, part.machine, part.name, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT );
    else
        issue_downtime_event( part.computer, part.machine, part.name, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT );

    if ( down_timeout_seconds > 0 )
        {
        /*
        -------------------------------------------------
        This was a good shot, turn on the monitoring wire
        ------------------------------------------------- */
        if ( MonitorWire != NO_WIRE && ClearMonitorWireOnDown )
            {
            u = wirebit( MonitorWire );
            if ( !(bits_currently_on & u) )
                setwire( MonitorWire );
            }

        if ( DownTimeAlarmWire != NO_WIRE )
            {
            u = wirebit( DownTimeAlarmWire );
            if ( bits_currently_on & u )
                clearwire( DownTimeAlarmWire );
            }
        }
    }

update_monitor_status( this, resource_string(READY_STRING) );
}

/***********************************************************************
*                      TRUNCATE_CONTROL_FILE_LINE                      *
*             Don't send comments or spaces before comments            *
***********************************************************************/
static void truncate_control_file_line( TCHAR * line )
{
TCHAR * pp;

pp = findchar( PoundChar, line );
if ( pp )
    {
    while ( pp > line )
        {
        pp--;
        if ( *pp != SpaceChar && *pp != TabChar )
            {
            pp++;
            break;
            }
        }
    *pp = NullChar;
    }
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                          UPLOAD_CONTROL_FILE                         *
***********************************************************************/
bool FTII_BOARD_ENTRY::upload_control_file()
{
const unsigned BIT_0_MASK = 1;

TCHAR * cp;
NAME_CLASS      nc;
STRING_CLASS    s;
FILE_CLASS      f;
FTII_VAR_ARRAY  globals;
FTII_VAR_ARRAY  partvars;
FTII_VAR_ARRAY  machvars;
TEXT_LIST_CLASS t;
unsigned        x;
int             i;
bool            is_prince_machine;

is_prince_machine = false;
if ( strings_are_equal(m.rodpitch, PrinceRodPitch) )
    is_prince_machine = true;

nc.get_ft2_global_settings_file_name( part.computer, part.machine );
globals.get( nc.text() );

/*
-------------------------------
Read the park step and alt step
------------------------------- */
nc.get_ft2_ctrl_prog_steps_file_name( part.computer, part.machine );
if ( f.open_for_read(nc.text()) )
    {
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;
        if ( *cp == DecimalPointChar )
            t.append( cp );
        }
    f.close();
    }

nc.get_ft2_part_settings_file_name( part.computer, part.machine, part.name );
partvars.get( nc.text() );

nc.get_ft2_machine_settings_file_name( part.computer, part.machine );
if ( file_exists(nc.text()) )
    {
    machvars.get( nc.text() );
    /*
    -------------------------------------------------------
    Make sure there is no V314 in the machine variable list
    ------------------------------------------------------- */
    if ( machvars.find(CONFIG_WORD_2_VN) )
        machvars.remove();
    }
else if ( machvars.count() > 0 )
    {
    machvars.empty();
    }
x = m.autoshot_timeout_seconds;
if ( x > 0 )
    {
    x *= 1000;
    i = machvars.count();
    machvars.upsize( i+1 );
    machvars[i].set( CS_TIMEOUT_MS_VN, x );
    }

/*
-------------------------------------------------------------
Save the file name so I can display it on the realtime screen
------------------------------------------------------------- */
upload_file_name = ft2_control_program_name( version, m );

nc.get_exe_dir_file_name( part.computer, upload_file_name.text() );

if ( f.open_for_read(nc.text()) )
    {
    send_socket_message( START_OF_UPLOAD, false );
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;

        truncate_control_file_line( cp );

        if ( *cp != NullChar && *cp != PoundChar )
            {
            /*
            ---------------------------------------------------------
            If this is a global or part variable, use the local value
            --------------------------------------------------------- */
            if ( globals.find(cp) )
                {
                globals.makeline( s );
                }
            else if ( partvars.find(cp) )
                {
                partvars.makeline( s );
                }
            else if ( machvars.find(cp) )
                {
                machvars.makeline( s );
                }
            else
                {
                s = cp;

                /*
                -------------------------------------------------------------
                Check to see if this is a program line that I want to replace
                ------------------------------------------------------------- */
                if ( *cp == DecimalPointChar && *(cp+1) >= ZeroChar && *(cp+1) <= NineChar )
                    {
                    t.rewind();
                    while ( t.next() )
                        {
                        if ( strings_are_equal( cp, t.text(), CTRL_PROG_NUMBER_LEN) )
                            {
                            s = t.text();
                            break;
                            }
                        }
                    }
                }

            /*
            -----------------------------------
            If this is the config word, save it
            ----------------------------------- */
            if ( s.contains(ConfigSetString) )
                {
                get_config_word( s.text() );
                }
            else if ( start_chars_are_equal(PrinceSetString, s.text()) )
                {
                if ( is_prince_machine )
                    {
                    if ( s.len() >= PrinceCharOffset )
                        {
                        cp = s.text();
                        cp += PrinceCharOffset;
                        *cp = TEXT('5');
                        }
                    }

                if ( pc_retctl_dac == 4 )
                    {
                    cp = s.text();
                    cp += PcRetctlDac4Offset;
                    *cp = TEXT('1');
                    }
                }

            else if ( s.contains(IsControlSetString) )
                {
                cp = s.text();
                cp = findchar( HChar, cp );
                if ( cp )
                    {
                    cp++;
                    x = (unsigned) hextoul( cp );
                    if ( m.suretrak_controlled )
                        x |= BIT_0_MASK;
                    else
                        x &= ~BIT_0_MASK;
                    *cp = NullChar;
                    s += ultohex( (unsigned long) x );
                    }
                }
            if ( !s.isempty() )
                {
                s += CrString;
                send_socket_message( s.text(), false );
                }
            }
        }

    f.close();
    send_socket_message( END_OF_UPLOAD, false );
    SetEvent( SendEvent );
    return true;
    }

return false;
}

/***********************************************************************
*                       UPDATE_ONE_BOARD_DISPLAY                       *
***********************************************************************/
static void update_one_board_display( int row, TCHAR * monitor_status_text )
{
if ( row>=0 && row<NofRows )
    {
    set_text( MainPushButton[row].w,    int32toasc(Boards[row].number) );
    set_text( AddressTbox[row].w,       Boards[row].address.text()     );
    set_text( PortTbox[row].w,          Boards[row].port.text()        );
    set_text( ConnectedTbox[row].w,     NoConnectState.text()          );
    set_text( MachineTbox[row].w,       Boards[row].m.name             );
    set_text( PartTbox[row].w,          Boards[row].m.current_part     );
    set_text( MonitorStatusTbox[row].w, monitor_status_text            );
    }
}

/***********************************************************************
*                          DISPLAY_BOARD_INFO                          *
***********************************************************************/
void display_board_info()
{
int row;

for ( row=0; row<NofBoards; row++ )
    update_one_board_display( row, EmptyString );
}

/***********************************************************************
*                        POSITION_AUTO_SHOT_DISPLAY                    *
***********************************************************************/
void position_auto_shot_display()
{
RECT r;
INT  w;
INT  h;

AutoShotDisplayWindow.get_move_rect( r );

w = r.right - r.left;
h = r.bottom - r.top;

GetClientRect( GetDesktopWindow(), &r );

if ( r.top == 0 )
    r.top = -1;

r.left = r.right - w;
r.bottom = r.top + h;

SetWindowPos( AutoShotDisplayWindow.handle(), HWND_TOPMOST,
              r.left, r.top, r.right-r.left, r.bottom-r.top,
              SWP_HIDEWINDOW );
}

/***********************************************************************
*                          AUTOSHOTDISPLAYPROC                         *
***********************************************************************/
BOOL CALLBACK AutoShotDisplayProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static HBRUSH InputOnBackgroundBrush = 0;
HWND ws;
int  id;

switch (msg)
    {
    case WM_WINDOWPOSCHANGING:
        return TRUE;

    case WM_INITDIALOG:
        AutoShotDisplayWindow = hWnd;
        InputOnBackgroundBrush = CreateSolidBrush ( GreenColor );
        AutoShotDisplayWindow.post( WM_DBINIT );
        return TRUE;

    case WM_DESTROY:
        if ( InputOnBackgroundBrush )
            {
            DeleteObject( InputOnBackgroundBrush );
            InputOnBackgroundBrush = 0;
            }
        break;

    case WM_CTLCOLORSTATIC:
        ws = (HWND) lParam;
        id = GetDlgCtrlID( ws );
        if ( id == AUTO_SHOT_MACHINE_NUMBER_TBOX || id == AUTO_SHOT_LABEL_TBOX )
            {
            if ( IsWindowEnabled(ws) )
                {
                SetBkColor ((HDC) wParam, GreenColor );
                return (int) InputOnBackgroundBrush;
                }
            }
        break;

    case WM_DBINIT:
        position_auto_shot_display();
        MainWindow.set_focus();
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*              LOAD_THE_CURRENT_MACHINE_AT_THIS_BOARD                  *
***********************************************************************/
void load_the_current_machine_at_this_board( FTII_BOARD_ENTRY & dest )
{
STRING_CLASS machine_name;
STRING_CLASS new_machine_name;
STRING_CLASS old_part_name;
STRING_CLASS fn;
NAME_CLASS   s;
DB_TABLE     t;
int          i;
int          n;
DSTAT_CLASS  d;
bool         is_different_machine;
bool         monitoring_is_on;
TIME_CLASS   tc;
INI_CLASS    ini;
PRESSURE_CONTROL_CLASS pc;

new_machine_name.upsize( MACHINE_NAME_LEN );
machine_name.upsize( MACHINE_NAME_LEN );

/*
---------------
Turn off alarms
--------------- */
if ( dest.alarm_bits && !IsRemoteMonitor )
    {
    if ( dest.m.monitor_flags & MA_ALARM_MONITORING_ON )
        dest.clear_all_alarm_bits();
    }

if ( IsRemoteMonitor && (dest.number == CurrentMachine.ft_board_number) )
    {
    monitoring_is_on = true;
    machine_name = CurrentMachine.name;
    }
else
    {
    monitoring_is_on = false;
    s.get_machset_file_name( ComputerName.text() );
    t.open( s.text(), MACHSET_RECLEN, PWL );
    while( t.get_next_record(NO_LOCK) )
        {
        n = (int) t.get_short( MACHSET_FT_BOARD_NO_OFFSET );
        if ( dest.number == n )
            {
            if ( MA_MONITORING_ON & BITSETYPE(t.get_long(MACHSET_MONITOR_FLAGS_OFFSET)) )
                monitoring_is_on = true;

            t.get_alpha( new_machine_name.text(), MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );

            if ( monitoring_is_on || machine_name.isempty() || new_machine_name==dest.m.name )
                {
                machine_name = new_machine_name;
                if ( monitoring_is_on )
                    break;
                }
            }
        }
    t.close();
    if ( IsRemoteMonitor )
        monitoring_is_on = false;
    }

tc.get_local_time();
is_different_machine = false;

if ( IsRemoteMonitor )
    {
    dest.m.monitor_flags = 0;
    }
else
    {
    if ( machine_name != dest.m.name )
        is_different_machine = true;
    }

if ( dest.is_monitoring() )
    {
    d.get( dest.m.computer, dest.m.name );

    if ( !monitoring_is_on || is_different_machine )
        {
        if ( !d.is_down() )
            {
            d.set_cat( DOWNCAT_SYSTEM_CAT );
            d.set_subcat( DOWNCAT_PROG_EXIT_SUBCAT );
            d.set_time( tc.system_time() );
            d.put( dest.m.computer, dest.m.name );
            if ( dest.down_timeout_seconds > 0 )
                append_downtime_record( dest.m.computer, dest.m.name, dest.m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
            else
                issue_downtime_event(   dest.m.computer, dest.m.name, dest.m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
            }
        dest.send_socket_message( 0, true );
        if ( !monitoring_is_on && !is_different_machine )
            {
            /*
            ------------------------------
            Monitoring is being turned off
            ------------------------------ */
            dest.monitoring_was_stopped = true;
            }
        }
    }

dest.down_timeout_seconds = 0;
dest.extended_analog.clear();

if ( !machine_name.isempty() )
    {
    old_part_name = dest.part.name;
    if ( dest.m.find(ComputerName.text(), machine_name.text()) )
        {
        if ( dest.m.cycle_timeout_seconds > 0 )
            dest.down_timeout_seconds = (int) dest.m.cycle_timeout_seconds;

        if ( IsRemoteMonitor )
            {
            if ( machine_name == CurrentMachine.name )
                {
                dest.m.monitor_flags |= BITSETYPE( MA_MONITORING_ON );
                monitoring_is_on = true;
                }
            else
                {
                dest.m.monitor_flags &= BITSETYPE( ~MA_MONITORING_ON );
                monitoring_is_on = false;
                }
            }

        d.get( dest.m.computer, dest.m.name );
        dest.down_state = d.down_state();

        dest.part.find(     ComputerName.text(), machine_name.text(), dest.m.current_part );
        dest.param.find(    ComputerName.text(), machine_name.text(), dest.m.current_part );
        dest.ftanalog.load( ComputerName.text(), machine_name.text(), dest.m.current_part );
        dest.get_alarm_bits();
        dest.have_runlist = dest.runlist.get( ComputerName.text(), machine_name.text() );
        if ( dest.have_runlist )
            {
            for ( i=0; i<dest.runlist.count(); i++ )
                {
                if ( !part_exists(ComputerName.text(), machine_name.text(), dest.runlist[i].partname) )
                    MessageBox( 0, resource_string(NO_WARMUP_PART_STRING), dest.runlist[i].partname, MB_OK | MB_SYSTEMMODAL );
                }
            }
        dest.shotdata.set_us_per_time_sample( dest.part.ms_per_time_sample * 1000 );
        dest.set_extended_analog_bits();

        if ( is_different_machine )
            dest.monitoring_was_stopped = false;

        if ( pc.find(ComputerName.text(), machine_name.text(), dest.m.current_part) )
            {
            if ( pc.retctl_is_enabled )
                dest.pc_retctl_dac = 4;
            else
                dest.pc_retctl_dac = 0;
            }
        /*
        ------------------------------------
        Don't monitor if this is the NEWPART
        ------------------------------------ */
        if ( is_float_zero(dest.part.min_stroke_length) || is_float_zero(dest.part.total_stroke_length) )
            {
            dest.m.monitor_flags &= BITSETYPE(~MA_MONITORING_ON);
            monitoring_is_on = false;
            if ( !IsRemoteMonitor || (machine_name == CurrentMachine.name) )
                dest.send_socket_message( 0, true );
            }

        if ( monitoring_is_on )
            {
            /*
            -------------------------
            Get the last channel_mode
            ------------------------- */
            s.get_ft2_editor_settings_file_name( ComputerName.text(), machine_name.text() );
            ini.set_file( s.text() );
            ini.set_section( ConfigSection );
            if ( s.file_exists() )
                {
                if ( ini.find(CurrentChannelModeKey) )
                    dest.channel_mode = ini.get_int();
                }
            else
                {
                if ( dest.m.suretrak_controlled )
                    dest.channel_mode = 7;
                else
                    dest.channel_mode = 5;
                ini.put_int( CurrentChannelModeKey, dest.channel_mode );
                }

            /*
            -----------------------------------------------
            Make sure the results directory databases exist
            ----------------------------------------------- */
            if ( is_different_machine )
                {
                fn = alarmsum_dbname( dest.part.computer, dest.part.machine, dest.part.name );
                t.ensure_existance( fn.text() );
                fn = graphlst_dbname( dest.part.computer, dest.part.machine, dest.part.name );
                t.ensure_existance( fn.text() );
                fn = shotparm_dbname( dest.part.computer, dest.part.machine, dest.part.name );
                t.ensure_existance( fn.text() );
                }

            if ( d.down_state()==PROG_DOWN_STATE || d.down_state()==NO_DOWN_STATE || dest.down_timeout_seconds==0 )
                {
                /*
                -------------------------------------------
                Replace the program down with program start
                ------------------------------------------- */
                d.set_cat( DOWNCAT_SYSTEM_CAT );
                d.set_subcat( DOWNCAT_PROG_START_SUBCAT );
                d.set_time( tc.system_time() );
                d.put( dest.m.computer, dest.m.name );
                dest.down_state = d.down_state();
                dest.time_of_last_shot = tc;
                if ( dest.down_timeout_seconds > 0 )
                    append_downtime_record( dest.m.computer, dest.m.name, dest.m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_START_SUBCAT );
                else
                    issue_downtime_event(   dest.m.computer, dest.m.name, dest.m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_START_SUBCAT );
                }

            if ( dest.connect_state & CONNECTED_STATE )
                {
                if ( MonitorWire != NO_WIRE )
                    {
                    if ( !ClearMonitorWireOnDown || dest.down_state == MACH_UP_STATE || dest.down_state == PROG_UP_STATE )
                        dest.setwire( MonitorWire );
                    }
                }
            }
        }
    }
}

/***********************************************************************
*                           LOAD_FTII_BOARDS                           *
* This is called by the main program to load the boards once the       *
* initialization is complete.                                          *
***********************************************************************/
int load_ftii_boards()
{
NAME_CLASS s;
DB_TABLE   t;
int        i;
int        n;
char       cbuf[TCP_ADDRESS_LEN+1];

if ( Boards )
    {
    delete[] Boards;
    Boards    = 0;
    NofBoards = 0;
    }

s.get_ft2_boards_file_name( ComputerName.text() );
if ( file_exists(s.text()) )
    {
    t.open( s.text(), FTII_BOARDS_RECLEN, PFL );
    n = t.nof_recs();
    if ( n > 0 )
        {
        Boards = new FTII_BOARD_ENTRY[n];
        if ( Boards )
            {
            NofBoards = 0;
            while ( t.get_next_record(FALSE) )
                {
                if ( NofBoards < n )
                    {
                    Boards[NofBoards].number = t.get_short( FTII_BOARDS_NUMBER_OFFSET );

                    Boards[NofBoards].address.upsize( TCP_ADDRESS_LEN );
                    t.get_alpha( Boards[NofBoards].address.text(), FTII_BOARDS_ADDRESS_OFFSET, TCP_ADDRESS_LEN );

                    Boards[NofBoards].port.upsize( TCP_PORT_LEN );
                    t.get_alpha( Boards[NofBoards].port.text(), FTII_BOARDS_PORT_OFFSET,    TCP_PORT_LEN );

                    Boards[NofBoards].connect_state = NO_FT2_CONNECT_STATE;

                    Boards[NofBoards].a.sin_family           = PF_INET;
                    Boards[NofBoards].a.sin_port             = htons( ((u_short)Boards[NofBoards].port.uint_value()) );
                    unicode_to_char( cbuf, Boards[NofBoards].address.text() );
                    Boards[NofBoards].a.sin_addr.S_un.S_addr = inet_addr( cbuf );

                    NofBoards++;
                    }
                }
            }
        }
    t.close();
    }

if ( NofBoards > 0 )
    {
    for ( i=0; i<NofBoards; i++ )
        {
        load_the_current_machine_at_this_board( Boards[i] );
        Boards[i].index = i;
        }
    }

return NofBoards;
}

/***********************************************************************
*                        DISCONNECT_FROM_BOARDS                        *
***********************************************************************/
static void disconnect_from_boards()
{
int          i;
DSTAT_CLASS  d;
TIME_CLASS   tc;
unsigned     mask;

tc.get_local_time();
for ( i=0; i<NofBoards; i++ )
    {
    if ( Boards[i].is_monitoring() )
        {
        if ( Boards[i].connect_state & CONNECTED_STATE )
            {
            mask = Boards[i].alarm_bits;
            if ( MonitorWire != NO_WIRE )
                mask |= wirebit( MonitorWire );

            if ( DownTimeAlarmWire != NO_WIRE )
                mask |= wirebit( DownTimeAlarmWire );

            if ( mask )
                Boards[i].send_bits( mask, false );

            Boards[i].bits_currently_on = 0;

            if ( Boards[i].is_sending_op_data_packets )
                Boards[i].send_socket_message( DONT_SEND_OP_DATA_PACKETS, true );

            Boards[i].send_socket_message( 0, false );
            }

       /*
       -------------------------------------------------------------------------------
       If I am running the warmup part, restore the main part name as the current part
       ------------------------------------------------------------------------------- */
       if ( Boards[i].have_runlist )
           {
           if ( Boards[i].runlist.count() > 1 )
               {
               if ( !strings_are_equal(Boards[i].runlist[0].partname, Boards[i].part.name) )
                   set_current_part_name( Boards[i].part.computer, Boards[i].part.machine, Boards[i].runlist[0].partname );
               }
           }
        d.get( Boards[i].m.computer, Boards[i].m.name );
        if ( d.down_state()==PROG_UP_STATE || d.down_state()==MACH_UP_STATE || d.down_state()==NO_DOWN_STATE || Boards[i].down_timeout_seconds==0 )
            {
            d.set_cat( DOWNCAT_SYSTEM_CAT );
            d.set_subcat( DOWNCAT_PROG_EXIT_SUBCAT );
            d.set_time( tc.system_time() );
            d.put( Boards[i].m.computer, Boards[i].m.name );
            if ( Boards[i].down_timeout_seconds > 0 )
                append_downtime_record( Boards[i].m.computer, Boards[i].m.name, Boards[i].m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
            else
                issue_downtime_event(   Boards[i].m.computer, Boards[i].m.name, Boards[i].m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
            }
        }
    }
}

/***********************************************************************
*                    CONTINUE_CONNECTING_ONE_BOARD                     *
***********************************************************************/
static void continue_connecting_one_board( int i )
{
static TCHAR set_input_bitmask_string[] = TEXT( "V427=H" );
static STRING_CLASS s;
unsigned u;
int      ds;
int      j;

/*
--------------------------------------------------------------
If the monitoring was stopped and then restarted I am assuming
it was because the office workstation was monitoring so I do
not want to reupload unlessed forced to.
-------------------------------------------------------------- */
//if ( !Boards[i].monitoring_was_stopped )
  //  Boards[i].send_socket_message( Turn_On_Plusses, true );

/*
-----------------------------------------
Tell the board which bits I am monitoring
----------------------------------------- */
u = 0;
if ( Boards[i].have_runlist )
    u = Boards[i].runlist.bitmask();
if ( Boards[i].m.suretrak_controlled )
    u |= RetractWireMask;
u |= NoUploadWireMask;
for ( j=0; j<Boards[i].extended_analog.n; j++ )
    u |= Boards[i].extended_analog[j].mask;

/*
--------------------------
Add bits for Al's Log File
-------------------------- */
if ( AlsLogIsActive )
    u |= ALS_BITS;

s = set_input_bitmask_string;
s += ultohex( (unsigned long) u );
s += CrString;
Boards[i].send_socket_message( s.text(), true );
Boards[i].send_socket_message( GetInputBitsString, true );

if ( !Boards[i].monitoring_was_stopped )
    {
    if ( !Boards[i].upload_control_file() )
        update_monitor_status( &Boards[i], resource_string(NO_UPLOAD_FILE_STRING) );
    //Boards[i].send_socket_message( Turn_Off_Plusses, true );
    }

Boards[i].monitoring_was_stopped = false;

Boards[i].clear_all_alarm_bits();
if ( MonitorWire != NO_WIRE && Boards[i].down_timeout_seconds > 0 )
    {
    ds = Boards[i].down_state;
    if ( !ClearMonitorWireOnDown || ds == PROG_UP_STATE || ds == MACH_UP_STATE || ds == NO_DOWN_STATE )
        {
        Boards[i].setwire( MonitorWire );
        }
    }
}

/***********************************************************************
*                          CONNECT_ONE_BOARD                           *
***********************************************************************/
static void connect_one_board( int i )
{
static TCHAR set_input_bitmask_string[] = TEXT( "V427=H" );
static STRING_CLASS s;
unsigned u;
bool     not_connected;

not_connected = true;
if ( Boards[i].connect_state & CONNECTED_STATE )
    not_connected = false;

if ( Boards[i].is_monitoring() )
    {
    /*
    ----------------------------------------------------------------------------
    I use the following to decide which error message to display if I don't talk
    ---------------------------------------------------------------------------- */
    Boards[i].have_response = false;

    Boards[i].version = 0;
    Boards[i].set_connect_state( CONNECTING_STATE );
    if ( not_connected )
        Boards[i].send_socket_message( GetTimerFrequencyString, false );
    Boards[i].send_socket_message( GetVersionString, true );
    }
else
    {
    u = NOT_MONITORED_STATE;
    if ( is_float_zero(Boards[i].part.min_stroke_length) || is_float_zero(Boards[i].part.total_stroke_length) )
        u |= ZERO_STROKE_LENGTH_STATE;
    Boards[i].set_connect_state( u );
    }
}

/***********************************************************************
*                           CONNECT_TO_BOARDS                          *
*  This is only run at startup so I check here for the remote machine  *
*  and make sure it is not uploaded the first time.                    *
***********************************************************************/
static void connect_to_boards()
{
int i;

for ( i=0; i<NofBoards; i++ )
    {
    if ( IsRemoteMonitor && (Boards[i].number == CurrentMachine.ft_board_number) )
        Boards[i].monitoring_was_stopped = true;
    connect_one_board( i );
    }
}

/***********************************************************************
*                      CHECK_FOR_CYCLE_START                           *
***********************************************************************/
static bool check_for_cycle_start( FTII_BOARD_ENTRY * board,  TCHAR * sorc )
{
static TCHAR  CYCLE_START[] = TEXT("Cycle start");
static TCHAR  TIMEOUT[]     = TEXT("timeout");

TCHAR * cp;
unsigned u;

if ( !board )
    return false;

cp = findstring( CYCLE_START, sorc );
if ( cp )
    {
    if ( findstring(TIMEOUT, cp) )
        {
        update_monitor_status( board, resource_string(TIMEOUT_STRING) );
        board->do_not_upload = false;
        set_text( Dnu[board->index], EmptyString );
        }
    else
        {
        update_monitor_status( board, resource_string(CYCLE_START_STRING) );
        if ( board->part.good_shot_wire != NO_WIRE )
            {
            board->clearwire( board->part.good_shot_wire );
            }

        if ( ClearAlarmsEachShot )
            {
            u = board->get_parameter_alarm_bits();
            if ( u )
                board->send_bits( u, false );
            }

        if ( !NoUploadWireMask && board->m.suretrak_controlled )
            {
            board->do_not_upload = true;
            if ( UseStartShotTimeout )
                {
                board->do_not_upload_timeout = GetTickCount();
                u = board->m.autoshot_timeout_seconds;
                u += 5;
                u *= 1000;
                board->do_not_upload_timeout += u;
                }
            set_text( Dnu[board->index], AsterixString );
            }
        }
    return true;
    }

return false;
}

/***********************************************************************
*                             DO_ASCII_MESSAGE                         *
***********************************************************************/
static void do_ascii_message( int bindex, TCHAR * sorc )
{
static const TCHAR ANALOG_RESPONSE[] = TEXT( "O_CH" );
static const int  ANALOG_RESPONSE_SLEN = 4;

static const TCHAR TIMER_FREQ_RESPONSE[] = TEXT( "V301_" );
static const int   TIMER_FREQ_SLEN       = 5;

static const TCHAR AT_HOME_RESPONSE[]    = TEXT( "R_At home" );
static const TCHAR VERSION_RESPONSE[]    = TEXT( "R:6#emc_" );
static const TCHAR VERSION_RESPONSE2[]   = TEXT( "R:6#cyc_" );  /* Cyclone II board returns this */
static const int   VERSION_RESPONSE_SLEN = 8;

static const TCHAR FATAL_OR_WARNING_RESPONSE[] = TEXT( "_Cont." );
static const int   FATAL_OR_WARNING_RESPONSE_SLEN = 6;

FTII_BOARD_ENTRY * board;
LPARAM lParam;
WPARAM wParam;
TCHAR * cp;
TCHAR * commap;
bool    matched_message;
int     old_version;

board = Boards + bindex;

matched_message = check_for_cycle_start( board,  sorc );

/*
----------------------------------------
See if this is a timer frequency message
---------------------------------------- */
if ( !matched_message )
    {
    cp = findstring( TIMER_FREQ_RESPONSE, sorc );
    if ( cp )
        {
        cp += TIMER_FREQ_SLEN;
        board->timer_frequency = asctoint32( cp );
        matched_message = true;
        }
    }

if ( !matched_message )
    {
    cp = findstring( INPUT_BITS_RESPONSE, sorc );
    if ( cp )
        {
        cp += INPUT_BITS_RESPONSE_SLEN;
        board->check_inputs( cp );
        matched_message = true;
        }
    }

if ( !matched_message && board->do_not_upload )
    {
    cp = findstring( AT_HOME_RESPONSE, sorc );
    if ( cp )
        {
        board->do_not_upload = false;
        matched_message = true;
        set_text( Dnu[board->index], EmptyString );
        }
    }

if ( !matched_message )
    {
    cp = findstring( VERSION_RESPONSE, sorc );
    if ( !cp )
        cp = findstring( VERSION_RESPONSE2, sorc );
    if ( cp )
        {
        cp += VERSION_RESPONSE_SLEN;
        commap = findchar( CommaChar, cp );
        if ( commap )
            {
            *commap = NullChar;
            if ( board->version_string != cp )
                {
                board->version_string = cp;
                board->put_version_string();
                }
            *commap = CommaChar;
            }
        old_version = board->version;
        board->version = asctoint32( cp );
        cp += 2;
        board->sub_version = asctoint32( cp );
        matched_message = true;
        /*
        ----------------------------------------------------------------
        I now wait for the version message to come back before uploading
        ---------------------------------------------------------------- */
        if ( old_version == 0 )
            continue_connecting_one_board( bindex );
        }
    }

if ( !matched_message )
    {
    /*
    ----------------------------------------------------------------------------------------
    I always need to check to see if an analog message is in response to an extended channel
    request.
    ---------------------------------------------------------------------------------------- */
    if ( strings_are_equal(sorc, ANALOG_RESPONSE, ANALOG_RESPONSE_SLEN) )
        {
        board->check_for_extended_channel_value( sorc );
        matched_message = true;
        }
    }

if ( !matched_message )
    {
    cp = sorc;
    cp++;
    if ( strings_are_equal(cp, FATAL_OR_WARNING_RESPONSE, FATAL_OR_WARNING_RESPONSE_SLEN) )
        {
        cp += FATAL_OR_WARNING_RESPONSE_SLEN;
        cp++;
        update_monitor_status( board, cp );
        // matched_message = true;
        }
    }

/*
---------------------------------------------------------------------
If this is the board that has the current machine then send all ascii
data to the main window in case the terminal is on.
The main window will delete the buffer.
--------------------------------------------------------------------- */
if ( bindex == CurrentBoardIndex )
    {
    lParam = (LPARAM) sorc;
    wParam = (WPARAM) ASCII_TYPE;
    MainWindow.post( WM_NEW_RT_DATA, wParam, lParam );
    }
else
    {
    delete[] sorc;
    }
}

/***********************************************************************
*                           DO_ASCII_MESSAGE                           *
***********************************************************************/
static void do_ascii_message( int bindex, SOCKET_MESSAGE * mp )
{
static TCHAR lfchar = TEXT('\n');
char * sorc;
TCHAR * dest;
int    n;
BINARY_HEADER  * bh;

sorc = mp->s;
if ( *sorc == 'B' )
    {
    bh = (BINARY_HEADER *) mp->s;
    n = (int) bh->nof_bytes;
    sorc += sizeof( BINARY_HEADER );
    }
else
    {
    n = mp->len;
    }

if ( n < 1 )
    return;

/*
----------------------------------
See if this is just a plus message
---------------------------------- */
if ( *sorc == PlusChar )
    {
    Boards[bindex].waiting_for_plus = false;
    SetEvent( SendEvent );
    return;
    }

dest = maketext_from_char( sorc, n );
if ( dest )
    {
    n--;
    if ( dest[n] == lfchar )
        dest[n] = NullChar;
    do_ascii_message( bindex, dest );
    }
}

/***********************************************************************
*                             DO_NEWSETUP                              *
***********************************************************************/
void do_newsetup( TCHAR * machine_name )
{
int i;
MACHINE_CLASS m;
bool was_sending_op_data_packets;

if ( m.find(ComputerName.text(), machine_name) )
    {
    if ( m.is_ftii )
        {
        for ( i=0; i<NofBoards; i++ )
            {
            if ( Boards[i].number == m.ft_board_number )
                {
                /*
                ------------------------------------------------------------------------
                If this board is not being monitored and that hasn't changed, do nothing
                ------------------------------------------------------------------------ */
                if ( !Boards[i].is_monitoring() && !(m.monitor_flags & MA_MONITORING_ON) )
                    return;

                if ( i < NofRows )
                    {
                    if ( Boards[i].is_monitoring() && !(Boards[i].connect_state & ZERO_STROKE_LENGTH_STATE) )
                        set_text( MonitorStatusTbox[i].w, resource_string(NEW_SETUP_STRING) );
                    }
                was_sending_op_data_packets = false;
                if ( Boards[i].is_sending_op_data_packets )
                    {
                    was_sending_op_data_packets = true;
                    Boards[i].send_socket_message( DONT_SEND_OP_DATA_PACKETS, true );
                    }
                load_the_current_machine_at_this_board( Boards[i] );
                connect_one_board( i );
                if ( Boards[i].is_monitoring() )
                    {
                    if ( was_sending_op_data_packets )
                        Boards[i].send_socket_message( SEND_OP_DATA_PACKETS, true );
                    if ( Boards[i].monitoring_was_stopped )
                        set_text( MonitorStatusTbox[i].w, resource_string(RESTARTED_STRING) );
                    }
                skip_reset_machine( Boards[i].m.name );
                if ( i < NofRows )
                    {
                    set_text( MachineTbox[i].w, Boards[i].m.name         );
                    set_text( PartTbox[i].w,    Boards[i].m.current_part );
                    }
                break;
                }
            }
        }
    }
}

/***********************************************************************
*                            CHECK_FOR_DOWN                            *
***********************************************************************/
void check_for_down()
{

int        i;
int        ds;
int        my_timeout_seconds;
int        seconds_since_last_shot;
TIME_CLASS now;
TIME_CLASS basetime;
DSTAT_CLASS  d;

basetime.get_local_time();

for ( i=0; i<NofBoards; i++ )
    {
    my_timeout_seconds = Boards[i].down_timeout_seconds;
    if ( Boards[i].is_monitoring() &&  my_timeout_seconds > 0 )
        {
        now = basetime;
        ds = Boards[i].down_state;
        if ( ds == PROG_UP_STATE || ds == MACH_UP_STATE || ds == NO_DOWN_STATE )
            {
            seconds_since_last_shot = now - Boards[i].time_of_last_shot;

            if ( seconds_since_last_shot > my_timeout_seconds )
                {
                if ( SubtractTimeoutFromAutoDown )
                    now -= (unsigned long) my_timeout_seconds;

                if ( StdDown.find((long) now.current_value()) )
                    {
                    d.set_cat( StdDown.cat() );
                    d.set_subcat( StdDown.subcat() );
                    }
                else
                    {
                    d.set_cat( DOWNCAT_SYSTEM_CAT );
                    d.set_subcat( DOWNCAT_UNSP_DOWN_SUBCAT );
                    }

                d.set_time( now.system_time() );
                d.put( Boards[i].part.computer, Boards[i].part.machine );
                Boards[i].down_state = d.down_state();
                append_downtime_record( Boards[i].part.computer, Boards[i].part.machine, Boards[i].part.name, now.system_time(), d.category(), d.subcategory() );

                if ( MonitorWire != NO_WIRE && ClearMonitorWireOnDown )
                    Boards[i].clearwire( MonitorWire );

                if ( DownTimeAlarmWire != NO_WIRE )
                    Boards[i].setwire( DownTimeAlarmWire );
                }
            }
        }
    }
}

/***********************************************************************
*                            DO_ACK_MESSAGE                            *
*           I have recieved an ack or nak from the board.              *
***********************************************************************/
static void do_ack_message( SOCKET_MESSAGE * mp )
{
bool message_is_complete;

SOCKET_MESSAGE * dead_mp;
int i;
unsigned new_state;

i = board_index( mp->a );
if ( i == NO_INDEX )
    return;

if ( !Boards[i].waiting_for_ack )
    return;

message_is_complete = false;

if ( mp->s[0] == NakChar )
    {
    if ( TerminalIsVisible )
        MainListBox.add( resource_string(NAK_RECEIVED_STRING) );
    Boards[i].nak_count++;
    Boards[i].waiting_for_ack  = false;
    Boards[i].waiting_for_plus = false;

    if ( Boards[i].nak_count >= MAX_NAK_COUNT )
        {
        new_state = NOT_CONNECTED_STATE;
        if ( Boards[i].uploading )
            {
            Boards[i].uploading      = false;
            Boards[i].bytes_uploaded = 0;
            /*
            -------------------------------------------------------------------
            The upload failed. Flush the que and make a note so I can try again
            ------------------------------------------------------------------- */
            new_state |= UPLOAD_WAS_ABORTED_STATE;
            while ( Boards[i].f.count() )
                {
                dead_mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                SocketMessageStore.push( dead_mp );
                }
            }
        Boards[i].set_connect_state( new_state );
        }
    }
else
    {
    message_is_complete = true;
    }

if ( message_is_complete )
    {
    dead_mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
    SocketMessageStore.push( dead_mp );

    Boards[i].waiting_for_ack = false;
    Boards[i].nak_count       = 0;
    }
}

/***********************************************************************
*                          CHECK_FOR_TIMEOUTS                          *
*    This just nak's any board that is waiting for an ack or nak.      *
***********************************************************************/
static void check_for_timeouts()
{
int i;
unsigned         new_state;
SOCKET_MESSAGE * mp;

for ( i=0; i<NofBoards; i++ )
    {
    if ( Boards[i].waiting_for_ack )
        {
        Boards[i].nak_count++;
        Boards[i].waiting_for_ack  = false;
        Boards[i].waiting_for_plus = false;
        if ( Boards[i].nak_count >= MAX_NAK_COUNT )
            {
            /*
            ---------------------------------------------------------------
            If the packet sent was a null packet then I don't want to leave
            it on the stack.
            --------------------------------------------------------------- */
            mp = (SOCKET_MESSAGE *) Boards[i].f.peek();
            if ( mp )
                {
                if ( mp->len == 0 )
                    {
                    mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                    SocketMessageStore.push( mp );
                    }
                }
            new_state               = NOT_CONNECTED_STATE;
            Boards[i].nak_count     = 0;
            Boards[i].using_plusses = false;
            if ( Boards[i].uploading || (Boards[i].version == 0) )
                {
                Boards[i].uploading      = false;
                Boards[i].bytes_uploaded = 0;
                if ( Boards[i].have_response )
                    update_monitor_status( &Boards[i], UploadFailed.text() );
                else
                    update_monitor_status( &Boards[i], NoResponse.text() );
                /*
                -------------------------------------------------------------------
                The upload failed. Flush the que and make a note so I can try again
                ------------------------------------------------------------------- */
                new_state |= UPLOAD_WAS_ABORTED_STATE;
                while ( Boards[i].f.count() )
                    {
                    mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                    SocketMessageStore.push( mp );
                    }
                }
            Boards[i].ms_at_last_contact = GetTickCount();
            Boards[i].set_connect_state( new_state );
            }
        }
    }
}

/***********************************************************************
*                    CONVERT_IO_CHANGE_TO_ASCII                        *
***********************************************************************/
void convert_io_change_to_ascii( int bindex, SOCKET_MESSAGE * mp )
{
char * cp;
unsigned int * xp;
TCHAR * s;
TCHAR * sp;

cp = mp->s;
cp += sizeof( BINARY_HEADER );
xp = (unsigned int *) cp;
s = maketext( INPUT_BITS_RESPONSE, 8 );
if ( s )
    {
    sp = s + INPUT_BITS_RESPONSE_SLEN;
    copystring( sp, ultohex(*xp) );
    do_ascii_message( bindex, s );
    }
}

/***********************************************************************
*                         START_OF_BINARY_DATA                         *
***********************************************************************/
void start_of_binary_data( int i, BINARY_HEADER * bh )
{
if ( Boards[i].binarybuf )
    {
    delete[] Boards[i].binarybuf;
    Boards[i].binarybuf = 0;
    }

Boards[i].current_len     = 0;
Boards[i].chars_left_in_buffer = 0;
switch( bh->data_type )
    {
    case START_OF_BINARY_POS_SAMPLES:
    case START_OF_BINARY_TIME_SAMPLES:
        Boards[i].chars_left_in_buffer = bh->nof_bytes * bh->nof_packets;
        Boards[i].current_type = SAMPLE_TYPE;
        Boards[i].sample_type  = 'P';
        if ( bh->data_type == START_OF_BINARY_TIME_SAMPLES )
            Boards[i].sample_type = 'T';
        break;

    case START_OF_BINARY_PARAMETERS:
        Boards[i].chars_left_in_buffer = bh->nof_bytes * bh->nof_packets;
        Boards[i].current_type = PARAMETERS_TYPE;
        break;

    case START_OF_BINARY_OP_STATUS_DATA:
        Boards[i].current_type = OP_STATUS_TYPE;
        Boards[i].chars_left_in_buffer = sizeof(FTII_OP_STATUS_DATA);
        break;

    case START_OF_BINARY_OSCILLOSCOPE_DATA:
        Boards[i].current_type = OSCILLOSCOPE_TYPE;
        Boards[i].chars_left_in_buffer = sizeof(FTII_OSCILLOSCOPE_DATA);
        break;
    }

if ( Boards[i].chars_left_in_buffer > 0 )
    {
    Boards[i].binarybuf             = new char[Boards[i].chars_left_in_buffer];
    Boards[i].bp                    = Boards[i].binarybuf;
    Boards[i].current_packet_number = 0;
    }
}

/***********************************************************************
*                            NEW_DATA_THREAD                           *
***********************************************************************/
void new_data_thread( void * notused )
{
const DWORD wait_ms = 2000;
static bool finished = false;

STRING_CLASS     s;
SOCKET_MESSAGE * mp;
BINARY_HEADER  * bh;
int              n;
char           * cp;
int              i;
LPARAM           lParam;
WPARAM           wParam;

while ( true )
    {
    if ( WaitForSingleObject(NewDataEvent, wait_ms) == WAIT_FAILED )
        break;

    while ( NewData.count() )
        {
        mp = (SOCKET_MESSAGE *) NewData.pop();

        if ( ascii_strings_are_equal(ShutdownMessage, mp->s, ShutdownMessageLen) )
            {
            SocketMessageStore.push( mp );
            mp = 0;
            finished = true;
            break;
            }

        i = board_index( mp->a );
        if ( i != NO_INDEX )
            {
            cp = mp->s;
            if ( *cp == 'B' )
                {
                /*
                -----------
                Binary data
                ----------- */
                bh = (BINARY_HEADER *) cp;
                if ( bh->data_type == START_OF_BINARY_TEXT_DATA )
                    {
                    do_ascii_message( i, mp );
                    }
                else if ( bh->data_type == START_OF_IO_CHANGE_DATA )
                    {
                    convert_io_change_to_ascii( i, mp );
                    }
                else if ( bh->data_type == START_OF_SINGLE_ANALOG_DATA || bh->data_type == START_OF_BLOCK_ANALOG_DATA )
                    {
                    }
                else
                    {
                    if ( bh->packet_num == FIRST_PACKET_NUMBER )
                        start_of_binary_data( i, bh );

                    if ( int(bh->packet_num) == Boards[i].current_packet_number  )
                        {
                        if ( TerminalIsVisible )
                            {
                            s = resource_string( PACKET_STRING );
                            s += Boards[i].current_packet_number;
                            s += resource_string( REPEATED_STRING );
                            s += Boards[i].m.name;
                            MainListBox.add( s.text() );
                            }
                        /*
                        ---------------------------------------------------------
                        I've already done this so make the packet_number invalide
                        --------------------------------------------------------- */
                        bh->packet_num = 0;
                        }
                    else if ( int(bh->packet_num) == (Boards[i].current_packet_number+1)  )
                        {
                        n = (int) bh->nof_bytes;
                        if ( n > 0 && Boards[i].chars_left_in_buffer > 0 )
                            {
                            if ( n > Boards[i].chars_left_in_buffer )
                                n = Boards[i].chars_left_in_buffer;
                            cp += sizeof( BINARY_HEADER );
                            memcpy( Boards[i].bp, cp, n );
                            Boards[i].chars_left_in_buffer -= n;
                            Boards[i].current_len     += n;
                            if ( Boards[i].chars_left_in_buffer > 0 )
                                Boards[i].bp += n;
                            }
                        Boards[i].current_packet_number = (int) bh->packet_num;
                        }
                    else if ( int(bh->packet_num) > (Boards[i].current_packet_number+1)  )
                        {
                        if ( TerminalIsVisible )
                            {
                            s = TEXT("**** ");
                            s += resource_string( PACKET_STRING );
                            s += Boards[i].current_packet_number+1;
                            s += resource_string( MISSING_STRING );
                            s += Boards[i].m.name;
                            s += TEXT(" ***" );
                            MainListBox.add( s.text() );
                            }
                        update_monitor_status( &Boards[i], resource_string(PACKET_ERROR_STRING) );
                        /*
                        -------------------------------------------------------------------------------------
                        I've missed a packet. Mark this with an invalid number so I won't use it for anything
                        ------------------------------------------------------------------------------------- */
                        bh->packet_num = 0;
                        }


                    if ( bh->packet_num==bh->nof_packets )
                        {
                        switch ( Boards[i].current_type )
                            {
                            case SAMPLE_TYPE:
                            case PARAMETERS_TYPE:
                                Boards[i].add_binary_to_shotdata();
                                break;

                            case OP_STATUS_TYPE:
                            case OSCILLOSCOPE_TYPE:
                                if ( i == CurrentBoardIndex )
                                    {
                                    lParam = (LPARAM) Boards[i].binarybuf;
                                    wParam = (WPARAM) Boards[i].current_type;
                                    MainWindow.post( WM_NEW_RT_DATA, wParam, lParam );
                                    }
                                else
                                    {
                                    delete[] Boards[i].binarybuf;
                                    }
                                Boards[i].binarybuf = 0;
                                break;
                            }
                        Boards[i].current_type = ASCII_TYPE;
                        }
                    }
                }
            else
                {
                /*
                ----------
                Ascii data
                ---------- */
                do_ascii_message( i, mp );
                }
            }

        SocketMessageStore.push( mp );
        }

    if ( finished )
        break;

    check_for_down();
    }

while ( NewData.count() )
    {
    mp = (SOCKET_MESSAGE *) NewData.pop();
    delete mp;
    }

if ( NewDataEvent )
    {
    CloseHandle( NewDataEvent );
    NewDataEvent = 0;
    }
}

/***********************************************************************
*                           GET_CHANNEL_MODE                           *
*     i is the board index, sorc is the channel_mode set string        *
*                                                                      *
*  New with version 6.44: If I am uploading then I want to change      *
*  the channel mode to the current channel mode. Otherwise I want to   *
*  change the current mode to the one being sent. This assumes the     *
*  only time the channel mode will be set outside of an upload is by   *
*  toggling the realtime channel 17 checkbox.                          *
***********************************************************************/
void get_channel_mode( int i, char * sorc )
{
int current_mode;
int new_mode;
TCHAR buf[31];
TCHAR * cp;

char_to_tchar( buf, sorc, 30 );
cp = buf + ChannelModeSetStringLen;

new_mode     = asctoint32( cp );
current_mode = Boards[i].channel_mode;

if ( new_mode == current_mode )
    return;

if ( current_mode < CHANNEL_MODE_1 || current_mode > CHANNEL_MODE_7 )
    {
    Boards[i].channel_mode = new_mode;
    return;
    }

if ( new_mode >= CHANNEL_MODE_1 && new_mode <= CHANNEL_MODE_7 )
    {
    if ( Boards[i].uploading )
        {
        sorc += ChannelModeSetStringLen;
        *sorc = '0';
        *sorc += current_mode;
        }
    else
        {
        Boards[i].channel_mode = new_mode;
        }
    }
}

/***********************************************************************
*                              SEND_THREAD                             *
***********************************************************************/
void send_thread( void * notused )
{
const  DWORD POST_NAK_WAIT_MS   = 3000;
const  DWORD CONNECT_TIMEOUT_MS = 3001;
const  DWORD ACK_WAIT_MS        = 1500;
const  DWORD PING_TIMEOUT_MS    = 20000;
const  DWORD SHUTTING_DOWN_MS   = 200;
static DWORD wait_ms            = CONNECT_TIMEOUT_MS;

static bool  waiting_for_read_shutdown     = false;
static bool  waiting_for_new_data_shutdown = false;

ACK_MESSAGE    * ap;
SOCKET_MESSAGE * mp;
DWORD            status;
int              i;
unsigned         delta;
unsigned         ms;
static unsigned  last_check_time;
bool             ok_to_send;   /* Used to trap a start of download when shot is in progress, st only */

ap = 0;
mp = 0;
last_check_time = GetTickCount();
while ( true )
    {
    status = WaitForSingleObject( SendEvent, wait_ms );
    if ( status == WAIT_FAILED )
        break;

    if ( waiting_for_read_shutdown )
        {
        if ( !Read_Thread_Is_Running )
            {
            waiting_for_new_data_shutdown = true;
            waiting_for_read_shutdown = false;
            if ( NewDataEvent )
                {
                mp = get_socket_message();
                if ( mp )
                    {
                    ascii_copystring( mp->s, ShutdownMessage );
                    mp->len = ShutdownMessageLen;
                    NewData.push( mp );
                    SetEvent( NewDataEvent );
                    }
                else
                    {
                    break;
                    }
                }
            else
                break;
            }
        continue;
        }
    else if ( waiting_for_new_data_shutdown )
        {
        if ( !NewDataEvent )
            break;
        continue;
        }

    /*
    -------------------
    Send any acks I owe
    ------------------- */
    while ( AckList.count() )
        {
        ap = (ACK_MESSAGE *) AckList.pop();
        sendto( MySocket, ap->s, AckLen, 0, (sockaddr *) &ap->a, sizeof(sockaddr_in) );
        AckMessageStore.push( ap );
        ap = 0;
        }

    /*
    ---------------------------------------
    Handle any acks or naks I have received
    --------------------------------------- */
    while ( ReceivedAcks.count() )
        {
        mp = (SOCKET_MESSAGE *) ReceivedAcks.pop();
        do_ack_message( mp );
        SocketMessageStore.push( mp );
        mp = 0;
        }

    /*
    ----------------------------------------------------------
    If I timed out and there is anyone still waiting then they
    have timed out.
    ---------------------------------------------------------- */
    ms = GetTickCount();
    delta = 0;
    if ( ms > last_check_time )
        delta = ms - last_check_time;
    if ( (ms < last_check_time) || (delta >= wait_ms ) )
        {
        check_for_timeouts();
        last_check_time = ms;
        }

    wait_ms = CONNECT_TIMEOUT_MS;
    ms      = GetTickCount();
    for ( i=0; i<NofBoards; i++ )
        {
        /*
        ----------------------------------------------
        I can't Upload while the shot is in progress
        ---------------------------------------------- */
        if ( Boards[i].do_not_upload )
            {
            if ( Boards[i].do_not_upload_timeout && (ms > Boards[i].do_not_upload_timeout) )
                {
                Boards[i].do_not_upload = false;
                set_text( Dnu[i], EmptyString );
                }
            }

        if ( Boards[i].do_not_upload && !I_Am_Shutting_Down )
            wait_ms = ACK_WAIT_MS;

        if ( Boards[i].waiting_for_ack || Boards[i].waiting_for_plus )
            {
            wait_ms = ACK_WAIT_MS;
            }
        else if ( Boards[i].connect_state & NOT_CONNECTED_STATE )
            {
            if ( !I_Am_Shutting_Down && Boards[i].is_monitoring() )
                {
                /*
                -----------------------------------------------------------
                If I haven't tried in a while, ping it to see if it is back
                ----------------------------------------------------------- */
                if ( ms < Boards[i].ms_at_last_contact )
                    {
                    Boards[i].ms_at_last_contact = ms;
                    }
                else
                    {
                    delta = ms - Boards[i].ms_at_last_contact;
                    if ( delta > PING_TIMEOUT_MS )
                        {
                        Boards[i].set_connect_state( CONNECTING_STATE );
                        Boards[i].ms_at_last_contact = ms;
                        if ( Boards[i].f.count() == 0 )
                            Boards[i].send_socket_message( GetMSLString, true );
                        }
                    }
                }
            }
        else
            {
            if ( Boards[i].f.count() )
                {
                /*
                ---------------------------------------------------------
                I use delta to wait 3 seconds if I have been nak'd.
                This prevents me from banging away when the board is busy
                --------------------------------------------------------- */
                delta = POST_NAK_WAIT_MS;
                if ( Boards[i].nak_count > 0 )
                    {
                    if ( ms > Boards[i].ms_at_last_contact )
                        delta = ms - Boards[i].ms_at_last_contact;
                    }

                /*
                ----------------------------------------------
                I can't upload while the shot is in progress
                ---------------------------------------------- */
                if ( Boards[i].do_not_upload )
                    {
                    if (  Boards[i].do_not_upload_timeout && ( ms > Boards[i].do_not_upload_timeout) )
                        {
                        Boards[i].do_not_upload = false;
                        update_monitor_status( &Boards[i], TEXT(" 0 Seconds") );
                        set_text( Dnu[i], EmptyString );
                        }
                    }

                if ( delta >= POST_NAK_WAIT_MS )
                    {
                    mp = (SOCKET_MESSAGE *) Boards[i].f.peek();
                    ok_to_send = true;

                    if ( start_chars_are_equal(START_OF_UPLOAD, mp->s) )
                        {
                        /*
                        ------------------------------------------------
                        Don't start an upload if the do_not_upload is on
                        ------------------------------------------------ */
                        if ( Boards[i].do_not_upload )
                            {
                            ok_to_send = false;
                            }
                        else
                            {
                            Boards[i].uploading      = true;
                            Boards[i].bytes_uploaded = 0;
                            mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                            SocketMessageStore.push( mp );
                            mp = 0;
                            update_monitor_status( &Boards[i], resource_string(BEGIN_UPLOAD_STRING) );
                            }
                        }
                    else if ( start_chars_are_equal(END_OF_UPLOAD, mp->s) )
                        {
                        Boards[i].uploading      = false;
                        Boards[i].bytes_uploaded = 0;
                        mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                        SocketMessageStore.push( mp );
                        mp = 0;
                        update_monitor_status( &Boards[i], resource_string(UPLOAD_COMPLETE_STRING) );
                        }

                    /*
                    -------------------------------------------------------------------------------
                    If I get here and mp is null then I must have just received a file start or end
                    ------------------------------------------------------------------------------- */
                    if ( !mp && Boards[i].f.count() )
                        mp = (SOCKET_MESSAGE *) Boards[i].f.peek();

                    if ( mp && ok_to_send )
                        {
                        if ( start_chars_are_equal(Turn_On_Plusses, mp->s) )
                            {
                            Boards[i].using_plusses    = true;
                            Boards[i].waiting_for_plus = false;
                            }
                        else if ( start_chars_are_equal(Turn_Off_Plusses, mp->s) )
                            {
                            Boards[i].using_plusses    = false;
                            Boards[i].waiting_for_plus = false;
                            }
                        else if ( start_chars_are_equal(ChannelModeSetString, mp->s) )
                            {
                            get_channel_mode( i, mp->s );
                            }

                        if ( Boards[i].uploading )
                            {
                            //if ( Boards[i].bytes_uploaded > 0 )
                              //  Sleep(1);
                            Boards[i].bytes_uploaded += mp->len;
                            //update_monitor_status( &Boards[i], int32toasc((int32)Boards[i].bytes_uploaded) );
                            }
                        if ( LoggingSentCommands )
                            write_sent_command( Boards[i].number, mp->s );

                        sendto( MySocket, mp->s, mp->len, 0, (sockaddr *) &mp->a, sizeof(sockaddr_in) );
                        Boards[i].waiting_for_ack = true;
                        if ( Boards[i].using_plusses )
                            Boards[i].waiting_for_plus = true;
                        wait_ms = ACK_WAIT_MS;
                        mp = 0;
                        }
                    }
                }
            else if ( Boards[i].connect_state & CONNECTED_STATE )
                {
                if ( !I_Am_Shutting_Down )
                    {
                    /*
                    --------------------------------------------------------------------------------
                    If I haven't talked to the board in a while, ping it to see if it is still there
                    -------------------------------------------------------------------------------- */
                    if ( ms < Boards[i].ms_at_last_contact )
                        {
                        Boards[i].ms_at_last_contact = ms;
                        }
                    else
                        {
                        delta = ms - Boards[i].ms_at_last_contact;
                        if ( delta > PING_TIMEOUT_MS )
                            {
                            Boards[i].ms_at_last_contact = ms;
                            Boards[i].send_socket_message( GetMSLString, true );
                            }
                        }
                    }
                }
            }
        }

    if ( I_Am_Shutting_Down && wait_ms == CONNECT_TIMEOUT_MS )
        {
        wait_ms = SHUTTING_DOWN_MS;

        if ( !waiting_for_read_shutdown && !waiting_for_new_data_shutdown )
            {
            waiting_for_read_shutdown = true;
            sendto( MySocket, ShutdownMessage, ShutdownMessageLen+1, 0, (sockaddr *) &MyAddress, sizeof(sockaddr_in) );
            }
        }
    }

if ( MySocket != INVALID_SOCKET )
    {
    closesocket( MySocket );
    MySocket = INVALID_SOCKET;
    }

if ( SendEvent )
    {
    CloseHandle( SendEvent );
    SendEvent = 0;
    }

if ( Boards )
    {
    delete[] Boards;
    Boards    = 0;
    NofBoards = 0;
    }

if ( Wsa_Is_Started )
    {
    WSACleanup();
    Wsa_Is_Started = false;
    }

clean_message_stores();

BoardMonitorIsRunning = false;
}

/***********************************************************************
*                               SEND_ACK                               *
***********************************************************************/
static void send_ack( sockaddr_in & dest )
{
ACK_MESSAGE * ap;

ap = get_ack_message();
ap->a = dest;
AckList.push( ap );
SetEvent( SendEvent );
}

/***********************************************************************
*                             READ_THREAD                              *
***********************************************************************/
void read_thread( void * notused )
{
STRING_CLASS     s;
SOCKET_MESSAGE * mp;
BINARY_HEADER  * bh;
int              alen;
int              i;
char           * cp;

Read_Thread_Is_Running = true;

while ( true )
    {
    mp = get_socket_message();
    alen = sizeof( sockaddr_in );
    mp->len = recvfrom( MySocket, mp->s, MAX_MESSAGE_LEN, 0, (sockaddr *) &mp->a, &alen );

    if ( mp->len == 0 || mp->len == SOCKET_ERROR || !SendEvent )
        {
        delete mp;
        mp = 0;
        break;
        }

    if ( ascii_strings_are_equal(ShutdownMessage, mp->s, ShutdownMessageLen) )
        {
        delete mp;
        mp = 0;
        break;
        }

    i = board_index( mp->a );
    if ( i != NO_INDEX )
        {
        Boards[i].have_response = true;
        if ( !(Boards[i].connect_state & CONNECTED_STATE) )
            {
            if ( !(Boards[i].connect_state & NOT_MONITORED_STATE) )
                Boards[i].set_connect_state( CONNECTED_STATE );
            }

        Boards[i].ms_at_last_contact = GetTickCount();
        }

    if ( mp->len == AckLen )
        {
        if ( mp->s[0]==AckChar || mp->s[0]==NakChar )
            {
            ReceivedAcks.push( mp );
            SetEvent( SendEvent );
            mp = 0;
            }
        }

    if ( mp )
        {
        if ( mp->len <= 0 )
            {
            SocketMessageStore.push( mp );
            mp = 0;
            }
        else
            {
            cp = mp->s;
            if ( *cp == 'B' )
                {
                /*
                -----------------------------------------------------------------------
                Only send an ack if this is binary data and the DONT_ACK_BIT is not set
                ----------------------------------------------------------------------- */
                bh = (BINARY_HEADER *) cp;
                if ( !(bh->flags & DONT_ACK_BIT) )
                    send_ack( mp->a );
                }
            NewData.push( mp );
            SetEvent( NewDataEvent );
            mp = 0;
            }
        }
    }

Read_Thread_Is_Running = false;
}

/***********************************************************************
*                             START_SOCKET                             *
***********************************************************************/
static void start_socket()
{
const u_short my_port    = 20001;
const WORD SocketVersion = MAKEWORD(2,0);

char      buf[257];
WSADATA   wsadata;
int       status;
HOSTENT * hostname;
in_addr * ap;

if ( WSAStartup(SocketVersion, &wsadata) )
    MessageBox( 0, TEXT("WSAStartup"), int32toasc(WSAGetLastError()), MB_OK );
else
    Wsa_Is_Started = true;

if ( Wsa_Is_Started )
    {
    MySocket = socket( PF_INET, SOCK_DGRAM, 0 );
    if ( MySocket == INVALID_SOCKET )
        {
        MessageBox( 0, TEXT("socket"), int32toasc(WSAGetLastError()), MB_OK );
        }
    else
        {
        MyAddress.sin_family           = PF_INET;
        MyAddress.sin_port             = htons( my_port );
        MyAddress.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
        status = bind( MySocket, (sockaddr *) &MyAddress, sizeof(MyAddress) );
        if ( status == SOCKET_ERROR )
            MessageBox( 0, TEXT("bind"), int32toasc(WSAGetLastError()), MB_OK );

        status = gethostname( buf, 256 );
        if ( status != SOCKET_ERROR )
            {
            hostname = gethostbyname( buf );
            ap = (in_addr *)hostname->h_addr_list[0];
            if ( ap )
                {
                MyAddress.sin_addr = *ap;
                //MainListBox.add( inet_ntoa(MyAddress.sin_addr) );
                }
            }
        }
    }
}

/***********************************************************************
*                            BEGIN_MONITORING                          *
***********************************************************************/
bool begin_monitoring()
{
unsigned long h;

SendEvent    = CreateEvent( NULL, FALSE, FALSE, NULL );
NewDataEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

AnalogSensor.get();
StdDown.startup();
start_socket();

h = _beginthread( new_data_thread, 0, NULL );
if ( h == -1 )
    return false;

h = _beginthread( send_thread, 0, NULL );
if ( h == -1 )
    return false;

h = _beginthread( read_thread, 0, NULL );
if ( h == -1 )
    return false;

BoardMonitorIsRunning = true;
connect_to_boards();

return true;
}

/***********************************************************************
*                            STOP_MONITORING                           *
***********************************************************************/
void stop_monitoring()
{
disconnect_from_boards();
I_Am_Shutting_Down = true;
SetEvent( SendEvent );
}

/***********************************************************************
*                             SEND_COMMAND                             *
***********************************************************************/
void send_command( TCHAR * machine_name, TCHAR * command, bool need_to_set_event )
{
int i;
bool is_reset_machine_command;
MACHINE_CLASS      m;
FTII_BOARD_ENTRY * b;

is_reset_machine_command = false;
if ( command )
    is_reset_machine_command = start_chars_are_equal( RESET_MACHINE_CMD, command );

i = board_index( machine_name );
if ( i == NO_INDEX )
    {
    /*
    --------------------------------------------
    See if I am to start monitoring this machine
    -------------------------------------------- */
    if ( is_reset_machine_command )
        {
        if ( m.find(ComputerName.text(), machine_name) )
            {
            if ( m.monitor_flags & MA_MONITORING_ON )
                {
                do_newsetup( machine_name );
                i = board_index( machine_name );
                if ( i != NO_INDEX )
                    update_one_board_display( i, resource_string(NEW_SETUP_STRING) );
                }
            }
        }
    return;
    }

b = Boards + i;

if ( (b->connect_state & NOT_CONNECTED_STATE) )
    b->set_connect_state( CONNECTING_STATE );

if ( command )
    {
    if ( is_reset_machine_command )
        {
        do_newsetup( machine_name );
        return;
        }
    b->send_socket_message( command, need_to_set_event );

    if ( start_chars_are_equal(StartControlPgmCmd, command) && MonitorWire != NO_WIRE )
        b->setwire( MonitorWire );
    }
}

/***********************************************************************
*                           UPDATE_DOWNTIME                            *
* This is called to update the downtime state from an event from the   *
* popup downtime window.                                               *
***********************************************************************/
void update_downtime( DOWN_DATA_CLASS dd )
{
int ds;
int i;
FTII_BOARD_ENTRY * b;
DSTAT_CLASS d;
bool was_up;

i = board_index( dd.machine_name() );
if ( i == NO_INDEX )
    return;

b = Boards + i;

if ( ( b->connect_state & NOT_MONITORED_STATE) )
    return;

if ( b->down_timeout_seconds < 1 )
    return;

d.set_cat( dd.category() );
d.set_subcat( dd.subcategory() );

if ( b->down_state == d.down_state() )
    return;

if ( ClearDownWireFromPopup && DownTimeAlarmWire != NO_WIRE )
    {
    was_up = false;

    ds = b->down_state;
    if ( ds == PROG_UP_STATE || ds == MACH_UP_STATE || ds == NO_DOWN_STATE )
        was_up = true;

    ds  = d.down_state();

    /*
    ---------------------------------
    If this wire is set then clear it
    --------------------------------- */
    if ( !was_up && ds == HUMAN_DOWN_STATE )
        b->clearwire( DownTimeAlarmWire );
    }

b->down_state = d.down_state();
}
