#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\iniclass.h"
#include "..\include\fifo.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

WINDOW_CLASS  DestWin;

const int     MAX_ASCII_LEN = 5000;
const unsigned short FIRST_PACKET_NUMBER = 1;

const unsigned char START_OF_BINARY_POS_SAMPLES       = 0;
const unsigned char START_OF_BINARY_TIME_SAMPLES      = 1;
const unsigned char START_OF_BINARY_PARAMETERS        = 2;
const unsigned char START_OF_BINARY_OSCILLOSCOPE_DATA = 3;
const unsigned char START_OF_BINARY_OP_STATUS_DATA    = 4;
const unsigned char START_OF_BINARY_TEXT_DATA         = 5;

const unsigned short DONT_ACK_BIT = 1;

static char   AsciiBuf[MAX_ASCII_LEN+1];
struct BINARY_HEADER {
    char           bchar;
    unsigned char  data_type;
    unsigned short flags;
    unsigned short data_set_num;
    unsigned short packet_num;
    unsigned short nof_packets;
    unsigned short nof_bytes;
    };

struct BOARD_ENTRY {
    int          current_type;
    int          current_len;  /* Binary data only */
    int          chars_in_buffer;
    int          current_packet_number;
    char       * bp;
    char       * binarybuf;
    sockaddr_in  a;
    bool         waiting_for_ack;
    int          nak_count;
    BOARD_ENTRY::BOARD_ENTRY();
    BOARD_ENTRY::~BOARD_ENTRY();
    };

BOARD_ENTRY MyBoard;

extern bool          ShuttingDown;
extern HWND          MainEbox;
extern LISTBOX_CLASS MainListBox;
extern char          START_OF_UPLOAD[];
extern char          END_OF_UPLOAD[];

static bool         Wsa_Is_Started = false;
static SOCKET       MySocket     = INVALID_SOCKET;
static sockaddr_in  SocketAddress;

static HANDLE       SendEvent = 0;
static HANDLE       NewDataEvent = 0;

static FIFO_CLASS   ReceivedAcks;
static FIFO_CLASS   AckList;
static FIFO_CLASS   SendData;
static FIFO_CLASS   NewData;

const char AckChar        = '\006';
const char CrChar         = '\r';
const char NakChar        = '\025';
const char NullChar       = '\0';
const char PlusChar       = '+';
const char QuestionChar   = '?';

const TCHAR UnderscoreString[] = TEXT("_");

const int AckLen          = 2;
const int MAX_NAK_COUNT   = 3;
const int MAX_MESSAGE_LEN = 1350;

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

/***********************************************************************
*                              BOARD_ENTRY                             *
***********************************************************************/
BOARD_ENTRY::BOARD_ENTRY()
{
current_type       = ASCII_TYPE;
current_len        = 0;
chars_in_buffer    = 0;
bp                 = 0;
binarybuf          = 0;

nak_count          = 0;
waiting_for_ack    = false;
}

/***********************************************************************
*                              BOARD_ENTRY                             *
***********************************************************************/
BOARD_ENTRY::~BOARD_ENTRY()
{
if ( binarybuf )
    {
    delete[] binarybuf;
    binarybuf = 0;
    }
}

/***********************************************************************
*                        START_CHARS_ARE_EQUAL                         *
***********************************************************************/
static bool start_chars_are_equal( const char * sorc, const char * s )
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
*                             post_data                                *
*                                                                      *
* I create a buffer and a struct to hold the info and send them        *
* all to the recv_window, who is responsible for deleting the          *
* buffers when finished with them.                                     *
*                                                                      *
***********************************************************************/
void post_data()
{
NEW_FTII_DATA * nd;
char          * cp;
int             len;

nd = new NEW_FTII_DATA;
if ( nd )
    {
    nd->sorc = 1;
    nd->type = MyBoard.current_type;
    if ( MyBoard.current_type == ASCII_TYPE )
        {
        len = lstrlen( AsciiBuf );
        if ( len > 0 )
            {
            cp = new char[len+1];
            if ( cp )
                {
                memcpy( cp, AsciiBuf, len );
                *(cp+len) = '\0';
                nd->buf = cp;
                nd->len = len;
                }
            }
        else
            {
            delete nd;
            nd = 0;
            }
        }
    else
        {
        /*
        ------------------------------------------------------------------------------
        This is binary data. Send it to the main window and reset myself to ascii mode
        ------------------------------------------------------------------------------ */
        if ( MyBoard.binarybuf && MyBoard.current_len > 0 )
            {
            nd->buf     = MyBoard.binarybuf;
            nd->len     = MyBoard.current_len;
            MyBoard.binarybuf = 0;
            }
        else
            {
            delete nd;
            nd = 0;
            }

        MyBoard.current_type = ASCII_TYPE;
        MyBoard.bp           = AsciiBuf;
        }

    if ( nd )
        DestWin.post( WM_NEW_DATA, 0, (LPARAM) nd );
    }
}

/***********************************************************************
*                           DO_ASCII_MESSAGE                           *
***********************************************************************/
void do_ascii_message( SOCKET_MESSAGE * mp )
{
int  n;
char * sorc;
BINARY_HEADER * bh;

MyBoard.current_type = ASCII_TYPE;

sorc = mp->s;
if ( *sorc == 'B' )
    {
    bh = (BINARY_HEADER *) sorc;
    sorc += sizeof( BINARY_HEADER );
    n  = (int) bh->nof_bytes;
    if ( n < 1 )
        return;
    }
else
    {
    n = mp->len;
    }

memcpy( AsciiBuf, sorc, n );
*(AsciiBuf+n) = '\0';

/*
-----------------------------------------
Post data doesn't affect the ascii buffer
----------------------------------------- */
post_data();
}

/***********************************************************************
*                         POST_BEGIN_MESSAGE                           *
***********************************************************************/
void post_begin_message( BINARY_HEADER * bh )
{
TCHAR s[100];
NEW_FTII_DATA * nd;
TCHAR * cp;

cp = 0;
switch( bh->data_type )
    {
    case START_OF_BINARY_POS_SAMPLES:
        cp = copystring( s, TEXT( "P_BEGIN_") );
        break;

    case START_OF_BINARY_TIME_SAMPLES:
        cp = copystring( s, TEXT( "T_BEGIN_") );
        break;

    case START_OF_BINARY_PARAMETERS:
        cp = copystring( s, TEXT( "C_BEGIN_") );
        break;
    }

if ( cp )
    {
    nd = new NEW_FTII_DATA;
    if ( nd )
        {
        nd->sorc = 1;
        nd->type = ASCII_TYPE;
        cp = copystring( cp, int32toasc((int32) bh->data_set_num) );
        cp = copystring( cp, UnderscoreString );
        cp = copystring( cp, int32toasc((int32) MyBoard.current_len) );

        nd->buf = maketext( s );
        nd->len = lstrlen( s );
        DestWin.post( WM_NEW_DATA, 0, (LPARAM) nd );
        }
    }
}

/***********************************************************************
*                         START_OF_BINARY_DATA                         *
***********************************************************************/
void start_of_binary_data( BINARY_HEADER * bh )
{
if ( MyBoard.binarybuf )
    {
    delete[] MyBoard.binarybuf;
    MyBoard.binarybuf = 0;
    }

MyBoard.current_len     = 0;
MyBoard.chars_in_buffer = 0;
switch( bh->data_type )
    {
    case START_OF_BINARY_POS_SAMPLES:
    case START_OF_BINARY_TIME_SAMPLES:
        MyBoard.chars_in_buffer = bh->nof_bytes * bh->nof_packets;
        MyBoard.current_type = SAMPLE_TYPE;
        break;

    case START_OF_BINARY_PARAMETERS:
        MyBoard.chars_in_buffer = bh->nof_bytes * bh->nof_packets;
        MyBoard.current_type = PARAMETERS_TYPE;
        break;

    case START_OF_BINARY_OP_STATUS_DATA:
        MyBoard.current_type = OP_STATUS_TYPE;
        MyBoard.chars_in_buffer = sizeof(FTII_OP_STATUS_DATA);
        break;

    case START_OF_BINARY_OSCILLOSCOPE_DATA:
        MyBoard.current_type = OSCILLOSCOPE_TYPE;
        MyBoard.chars_in_buffer = sizeof(FTII_OSCILLOSCOPE_DATA);
        break;
    }

if ( MyBoard.chars_in_buffer > 0 )
    {
    MyBoard.binarybuf             = new char[MyBoard.chars_in_buffer];
    MyBoard.bp                    = MyBoard.binarybuf;
    MyBoard.current_packet_number = 0;
    }
}

/***********************************************************************
*                            NEW_DATA_THREAD                           *
***********************************************************************/
DWORD new_data_thread( int * notused )
{
const DWORD wait_ms = INFINITE;

STRING_CLASS     s;
SOCKET_MESSAGE * mp;
BINARY_HEADER  * bh;
int              n;
char           * cp;

while ( true )
    {
    if ( WaitForSingleObject(NewDataEvent, wait_ms) == WAIT_FAILED )
        break;

    if ( ShuttingDown )
        break;

    while ( NewData.count() )
        {
        mp = (SOCKET_MESSAGE *) NewData.pop();
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
                do_ascii_message( mp );
                }
            else
                {
                if ( bh->packet_num == FIRST_PACKET_NUMBER )
                    start_of_binary_data( bh );

                if ( int(bh->packet_num) == MyBoard.current_packet_number  )
                    {
                    s = TEXT("Packet " );
                    s += MyBoard.current_packet_number;
                    s += TEXT(" repeated" );
                    MainListBox.add( s.text() );
                    }
                if ( int(bh->packet_num) == (MyBoard.current_packet_number+1)  )
                    {
                    n = (int) bh->nof_bytes;
                    if ( n > 0 && MyBoard.chars_in_buffer > 0 )
                        {
                        if ( n > MyBoard.chars_in_buffer )
                            n = MyBoard.chars_in_buffer;
                        cp += sizeof( BINARY_HEADER );
                        memcpy( MyBoard.bp, cp, n );
                        MyBoard.chars_in_buffer -= n;
                        MyBoard.current_len     += n;
                        if ( MyBoard.chars_in_buffer > 0 )
                            MyBoard.bp += n;
                        }
                    }
                else if ( int(bh->packet_num) > (MyBoard.current_packet_number+1)  )
                    {
                    s = TEXT("**** Packet " );
                    s += MyBoard.current_packet_number+1;
                    s += TEXT(" missing ***" );
                    MainListBox.add( s.text() );
                    }

                MyBoard.current_packet_number = (int) bh->packet_num;

                if ( bh->packet_num==bh->nof_packets || MyBoard.chars_in_buffer==0 )
                    {
                    post_begin_message( bh );
                    post_data();
                    }
                }
            }
        else
            {
            /*
            ----------
            Ascii data
            ---------- */
            do_ascii_message( mp );
            }

        delete mp;
        }
    }

return 0;
}

/***********************************************************************
*                            DO_ACK_MESSAGE                            *
*           I have recieved an ack or nak from the board.              *
***********************************************************************/
static void do_ack_message( SOCKET_MESSAGE * mp )
{
bool message_is_complete;

SOCKET_MESSAGE * dead_mp;

if ( !MyBoard.waiting_for_ack )
    return;

message_is_complete = false;

if ( mp->s[0] == NakChar )
    {
    MainListBox.add( "*** Nak Received ***" );
    MyBoard.nak_count++;
    if ( MyBoard.nak_count < MAX_NAK_COUNT )
        MyBoard.waiting_for_ack = false;
    else
        message_is_complete = true;
    }
else
    {
    message_is_complete = true;
    }

if ( message_is_complete )
    {
    dead_mp = (SOCKET_MESSAGE *) SendData.pop();
    delete dead_mp;

    MyBoard.waiting_for_ack = false;
    MyBoard.nak_count       = 0;
    }
}

/***********************************************************************
*                          CHECK_FOR_TIMEOUTS                          *
*    This just nak's any board that is waiting for an ack or nak.      *
***********************************************************************/
static void check_for_timeouts()
{
SOCKET_MESSAGE * dead_mp;

if ( MyBoard.waiting_for_ack )
    {
    MyBoard.nak_count++;
    if ( MyBoard.nak_count < MAX_NAK_COUNT )
        {
        MainListBox.add( "*** TimeOut Resend ***" );
        MyBoard.waiting_for_ack = false;
        }
    else
        {
        dead_mp = (SOCKET_MESSAGE *) SendData.pop();
        delete dead_mp;

        MyBoard.waiting_for_ack = false;
        MyBoard.nak_count       = 0;
        }
    }
}

/***********************************************************************
*                              SEND_THREAD                             *
***********************************************************************/
DWORD send_thread( int * notused )
{
const  DWORD ACK_WAIT_MS = 1500;
static DWORD wait_ms = INFINITE;
static int   bytes_uploaded = 0;
static bool  uploading      = false;

ACK_MESSAGE    * ap;
SOCKET_MESSAGE * mp;
DWORD            status;

ap = 0;
mp = 0;
while ( true )
    {
    status = WaitForSingleObject( SendEvent, wait_ms );
    if ( status == WAIT_FAILED )
        break;

    if ( ShuttingDown )
        break;

    /*
    -------------------
    Send any acks I owe
    ------------------- */
    while ( AckList.count() )
        {
        ap = (ACK_MESSAGE *) AckList.pop();
        sendto( MySocket, ap->s, AckLen, 0, (sockaddr *) &ap->a, sizeof(sockaddr_in) );
        delete ap;
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
        delete mp;
        mp = 0;
        }

    /*
    ----------------------------------------------------------
    If I timed out and there is anyone still waiting then they
    have timed out.
    ---------------------------------------------------------- */
    if ( status == WAIT_TIMEOUT )
        check_for_timeouts();

    wait_ms = INFINITE;
    if ( MyBoard.waiting_for_ack )
        {
        wait_ms = ACK_WAIT_MS;
        }
    else
        {
        if ( SendData.count() )
            {
            mp = (SOCKET_MESSAGE *) SendData.peek();

            if ( strings_are_equal(START_OF_UPLOAD, mp->s) )
                {
                uploading = true;
                bytes_uploaded = 0;
                mp = (SOCKET_MESSAGE *) SendData.pop();
                delete mp;
                mp = 0;
                MainListBox.add( TEXT("Begin Upload...") );
                }
            else if ( strings_are_equal(END_OF_UPLOAD, mp->s) )
                {
                uploading = false;
                bytes_uploaded = 0;
                mp = (SOCKET_MESSAGE *) SendData.pop();
                delete mp;
                mp = 0;
                MainListBox.add( TEXT("Upload Complete") );
                }

            /*
            --------------------------------------------------------------------------------
            If I get there and mp is null then I must have just received a file start or end
            -------------------------------------------------------------------------------- */
            if ( !mp && SendData.count() )
                mp = (SOCKET_MESSAGE *) SendData.peek();

            if ( mp )
                {
                if ( uploading )
                    {
                    bytes_uploaded += mp->len;
                    set_text( MainEbox, int32toasc((int32)bytes_uploaded) );
                    }
                sendto( MySocket, mp->s, mp->len, 0, (sockaddr *) &mp->a, sizeof(sockaddr_in) );
                MyBoard.waiting_for_ack = true;
                mp = 0;
                wait_ms = ACK_WAIT_MS;
                }
            }
        }
    }

return 0;
}

/***********************************************************************
*                               SEND_ACK                               *
***********************************************************************/
static void send_ack( sockaddr_in & dest )
{
ACK_MESSAGE * ap;

ap = new ACK_MESSAGE;
ap->a = dest;
AckList.push( ap );
SetEvent( SendEvent );
}

/***********************************************************************
*                             READ_THREAD                              *
***********************************************************************/
DWORD read_thread( int * notused )
{
STRING_CLASS     s;
SOCKET_MESSAGE * mp;
int              alen;
BINARY_HEADER  * bh;
char           * cp;

while ( true )
    {
    if ( ShuttingDown )
        break;

    mp = new SOCKET_MESSAGE;
    alen = sizeof( sockaddr_in );
    mp->len = recvfrom( MySocket, mp->s, MAX_MESSAGE_LEN, 0, (sockaddr *) &mp->a, &alen );

    if ( ShuttingDown )
        {
        delete mp;
        mp = 0;
        break;
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
        if ( mp->len > 0 )
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
        else
            {
            delete mp;
            mp = 0;
            break;
            }
        }
    }

return 0;
}

/***********************************************************************
*                             START_SOCKET                             *
***********************************************************************/
static void start_socket()
{
const u_short my_port    = 20001;
const WORD SocketVersion = MAKEWORD(2,0);
WSADATA wsadata;
int     status;
static sockaddr_in a;

if ( WSAStartup(SocketVersion, &wsadata) )
    MessageBox( 0, "WSAStartup", int32toasc(WSAGetLastError()), MB_OK );
else
    Wsa_Is_Started = true;

if ( Wsa_Is_Started )
    {
    NewDataEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    SendEvent    = CreateEvent( NULL, FALSE, FALSE, NULL );

    MySocket = socket( PF_INET, SOCK_DGRAM, 0 );
    if ( MySocket == INVALID_SOCKET )
        {
        MessageBox( 0, "socket", int32toasc(WSAGetLastError()), MB_OK );
        }
    else
        {
        a.sin_family           = PF_INET;
        a.sin_port             = htons( my_port );
        a.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
        status = bind( MySocket, (sockaddr *) &a, sizeof(a) );
        if ( status == SOCKET_ERROR )
            MessageBox( 0, "bind", int32toasc(WSAGetLastError()), MB_OK );
        }
    }
}

/***********************************************************************
*                           BEGIN_MONITORING                           *
***********************************************************************/
bool begin_monitoring( WINDOW_CLASS & notification_window )
{
HANDLE h;
DWORD  id;

DestWin = notification_window;

start_socket();

h = CreateThread(
    NULL,                                       /* no security attributes        */
    0,                                          /* use default stack size        */
    (LPTHREAD_START_ROUTINE) new_data_thread,   /* thread function       */
    0,                                          /* argument to thread function   */
    0,                                          /* use default creation flags    */
    &id );                                      /* returns the thread identifier */

if ( !h )
    return false;

h = CreateThread(
    NULL,                                       /* no security attributes        */
    0,                                          /* use default stack size        */
    (LPTHREAD_START_ROUTINE) send_thread,       /* thread function       */
    0,                                          /* argument to thread function   */
    0,                                          /* use default creation flags    */
    &id );                                      /* returns the thread identifier */

if ( !h )
    return false;

h = CreateThread(
    NULL,                                       /* no security attributes        */
    0,                                          /* use default stack size        */
    (LPTHREAD_START_ROUTINE) read_thread,       /* thread function       */
    0,                                          /* argument to thread function   */
    0,                                          /* use default creation flags    */
    &id );                                      /* returns the thread identifier */

if ( !h )
    return false;


return true;
}

/***********************************************************************
*                         SET_CURRENT_ADDRESS                          *
***********************************************************************/
bool set_current_address( char * new_address, u_short new_port )
{
SocketAddress.sin_family           = PF_INET;
SocketAddress.sin_port             = htons( new_port );
SocketAddress.sin_addr.S_un.S_addr = inet_addr( new_address );
return true;
}

/***********************************************************************
*                        SET_SOCKET_SEND_EVENT                         *
***********************************************************************/
void set_socket_send_event()
{
SetEvent( SendEvent );
}

/***********************************************************************
*                           SEND_SOCKET_MESSAGE                        *
***********************************************************************/
bool send_socket_message( char * sorc, bool need_to_set_event )
{
SOCKET_MESSAGE * mp;

if ( ShuttingDown )
    return false;

mp = new SOCKET_MESSAGE;

mp->a.sin_family           = PF_INET;
mp->a.sin_port             = SocketAddress.sin_port;
mp->a.sin_addr.S_un.S_addr = SocketAddress.sin_addr.S_un.S_addr;

lstrcpy( mp->s, sorc );
mp->len = lstrlen( mp->s );

SendData.push( mp );
if ( need_to_set_event )
    SetEvent( SendEvent );

return true;
}

/***********************************************************************
*                            STOP_MONITORING                           *
***********************************************************************/
void stop_monitoring()
{
SOCKET_MESSAGE * mp;
ACK_MESSAGE    * ap;

if ( MySocket != INVALID_SOCKET )
    {
    closesocket( MySocket );
    MySocket = INVALID_SOCKET;
    }

if ( Wsa_Is_Started )
    {
    WSACleanup();
    Wsa_Is_Started = false;
    }

if ( SendEvent )
    {
    CloseHandle( SendEvent );
    SendEvent = 0;
    }

if ( NewDataEvent )
    {
    CloseHandle( NewDataEvent );
    NewDataEvent = 0;
    }

while ( SendData.count() )
    {
    mp = (SOCKET_MESSAGE *) SendData.pop();
    delete mp;
    }

while ( ReceivedAcks.count() )
    {
    mp = (SOCKET_MESSAGE *) ReceivedAcks.pop();
    delete mp;
    }

while ( AckList.count() )
    {
    ap = (ACK_MESSAGE *) AckList.pop();
    delete ap;
    }
}
