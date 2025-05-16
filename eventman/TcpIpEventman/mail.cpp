#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\computer.h"
#include "..\include\events.h"
#include "..\include\fifo.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"

#include "resource.h"
#include "extern.h"

static HANDLE ConnectEvent = 0;
static HANDLE SendEvent    = 0;

SOCKET ListenSocket;

static u_short DEFAULT_LISTENER_PORT = 20010;
static const int LEN_MSG_COUNT         = 4;      /*0032 a four character message is used to tell the number of chars to follow */
static const int SEND_MSG_PAD_COUNT    = 2;      /* Added to send messages so there will be enough room for the len_msg */
static BOOLEAN ShuttingDown   = FALSE;

static FIFO_CLASS ConnectData;
static FIFO_CLASS SendData;

class REMOTE_COMPUTER_ENTRY
    {
    public:
    STRING_CLASS name;
    BOOLEAN      is_online;
    DWORD        ms_at_last_check;
    REMOTE_COMPUTER_ENTRY(){ is_online = TRUE; ms_at_last_check = 0; }
    ~REMOTE_COMPUTER_ENTRY(){}
    };

static REMOTE_COMPUTER_ENTRY * RemoteComputer = 0;
static int NofRemoteComputers = 0;

static TCHAR TwoSpaces[]   = TEXT( "  " );
static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR CommaChar     = TEXT( ',' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR ZeroChar      = TEXT( '0' );

void show_message( const TCHAR * sorc );

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                         RECEIVE_ONE_MESSAGE                          *
* Because there is no guarentee that you'll get your message in        *
* one chunk it's easier to do it in a separate routine.                *
***********************************************************************/
BOOLEAN receive_one_message( TCHAR * dest, int nof_chars, SOCKET & sorc_socket )
{
int bytes_needed;
int bytes_received;
int total_bytes_needed;
int total_bytes_received;
TCHAR * cp;

bytes_needed = nof_chars * sizeof(TCHAR);
total_bytes_needed = bytes_needed;
total_bytes_received = 0;

cp = dest;

while ( true )
    {
    bytes_received = recv( sorc_socket, cp, bytes_needed, 0 );
    if ( bytes_received == SOCKET_ERROR )
        return FALSE;

    total_bytes_received += bytes_received;
    bytes_needed -= bytes_received;
    if ( bytes_needed <= 0 )
        break;

    cp += bytes_received;
    }

return TRUE;
}

/***********************************************************************
*                              CONNECT_THREAD                          *
***********************************************************************/
DWORD connect_thread( int * notused )
{
SOCKET * sp;
TCHAR  * s;
int      chars_needed;
TCHAR    lenstring[LEN_MSG_COUNT+1];

while ( TRUE )
    {
    if ( !HaveMailslots )
        break;

    if ( WaitForSingleObject(ConnectEvent, INFINITE) == WAIT_FAILED )
        resource_message_box( UNABLE_TO_CONTINUE_STRING, MAIL_MONITOR_WAIT_FAIL_STRING );

    if ( !HaveMailslots )
        break;

    while ( ConnectData.count() )
        {
        sp = (SOCKET *) ConnectData.pop();
        if ( sp )
            {
            /*
            ----------------------------------------------------------------
            The first four characters are the length of the following string
            ---------------------------------------------------------------- */
            if ( receive_one_message(lenstring, LEN_MSG_COUNT, *sp) )
                {
                lenstring[LEN_MSG_COUNT] = NullChar;
                chars_needed = asctoint32( lenstring );
                if ( chars_needed > 0 )
                    {
                    s = maketext( chars_needed );
                    if ( s )
                        {
                        if ( receive_one_message(s, chars_needed, *sp) )
                            {
                            *(s+chars_needed) = NullChar;
                            PostMessage( MainWindow, WM_NEWMAIL, 0, (LPARAM) s );
                            }
                        else
                            {
                            delete[] s;
                            }
                        }
                    }
                }
            closesocket( *sp );
            delete sp;
            }
        }
    }

if ( ConnectEvent )
    {
    CloseHandle( ConnectEvent );
    SendEvent = 0;
    }

if ( !SendEvent )
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                              LISTEN_THREAD                           *
***********************************************************************/
DWORD listen_thread( int * notused )
{
int         status;
sockaddr_in a;
SOCKET    * ap;

ListenSocket = socket( PF_INET, SOCK_STREAM, 0 );
if ( ListenSocket == INVALID_SOCKET )
    {
    HaveMailslots = FALSE;
    goto EndofListen;
    }

a.sin_family      = PF_INET;
a.sin_port        = DEFAULT_LISTENER_PORT;
a.sin_addr.s_addr = htonl(INADDR_ANY);

status = bind( ListenSocket, (sockaddr *) &a, sizeof(a) );
if ( status == SOCKET_ERROR )
    {
    HaveMailslots = FALSE;
    goto EndofListen;
    }

while ( true )
    {
    if ( !HaveMailslots )
        break;

    status = listen( ListenSocket, SOMAXCONN );
    if ( status == SOCKET_ERROR )
        HaveMailslots = FALSE;

    if ( !HaveMailslots )
        break;

    ap = new SOCKET;

    *ap = accept( ListenSocket, NULL, NULL );
    if ( *ap != SOCKET_ERROR )
        {
        ConnectData.push( ap );
        SetEvent( ConnectEvent );
        }
    }

EndofListen:

/*
---------------------------------------
I normally shut down the connect thread
--------------------------------------- */
if ( ConnectEvent )
    SetEvent( ConnectEvent );

if ( !SendEvent && !ConnectEvent )
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                         SET_COMPUTER_OFFLINE                         *
***********************************************************************/
static void set_computer_offline( STRING_CLASS computer )
{
STRING_CLASS s;
int i;

for ( i=0; i<NofRemoteComputers; i++ )
    {
    if ( RemoteComputer[i].name == computer )
        {
        RemoteComputer[i].is_online        = FALSE;
        RemoteComputer[i].ms_at_last_check = GetTickCount();
        if ( ListEvents )
            {
            s = computer;
            s += TEXT( " is OFF Line" );
            show_message( s.text() );
            }
        break;
        }
    }
}

/***********************************************************************
*                       CONVERT_TO_MAILSLOT_NAME                       *
* The mailslot name is really just the network name of the computer    *
* such as \\ws0. You pass this routine a directory and it makes sure   *
* there are two backslashes at the start, removes them,  and then      *
* nullifies the next backslash that it finds. The result is what is    *
* stored in the RemoteComputer list.                                   *
***********************************************************************/
BOOLEAN convert_to_mailslot_name( STRING_CLASS & s )
{
TCHAR * cp;

cp = s.text();

if ( *cp != BackslashChar )
    return FALSE;
cp++;

if ( *cp != BackslashChar )
    return FALSE;

cp++;

while ( *cp != BackslashChar && *cp != NullChar )
    cp++;

*cp = NullChar;

cp = s.text();

remove_char( cp );
remove_char( cp );

return TRUE;
}

/***********************************************************************
*                        CHECK_OFFLINE_COMPUTERS                       *
*                      I do this once every minute.                    *
***********************************************************************/
static void check_offline_computers()
{
//const DWORD DELTA_T = 300000UL;
const DWORD DELTA_T = 20000UL;
STRING_CLASS s;
int     i;
DWORD   rt;
DWORD   t;
show_message( "Checking Offline Computers..." );
t = GetTickCount();
for ( i=0; i<NofRemoteComputers; i++ )
    {
    s = RemoteComputer[i].name;
    if ( RemoteComputer[i].is_online )
        s += TEXT( " is ON Line" );
    else
        s += TEXT( " is OFF Line" );
    show_message( s.text() );

    if ( !RemoteComputer[i].is_online )
        {
        rt = RemoteComputer[i].ms_at_last_check;

        s = "t = ";
        s += (unsigned int) t;
        s += " rt = ";
        s += (unsigned int) rt;
        if ( t > rt )
            {
            s += " delta_t = ";
            s += (unsigned int) (t-rt);
            }
        show_message( s.text() );

        if ( t < rt || ((t-rt) > DELTA_T) )
            {
            RemoteComputer[i].is_online = TRUE;
            if ( ListEvents )
                {
                s = RemoteComputer[i].name;
                s += TEXT( " is ON Line" );
                show_message( s.text() );
                }
            }
        }
    }
}

/***********************************************************************
*                          RELOAD_REMOTE_COMPUTERS                     *
***********************************************************************/
static void reload_remote_computers()
{
COMPUTER_CLASS c;
STRING_CLASS   s;
TEXT_LIST_CLASS t;
TCHAR         * cp;
int             i;
int             n;

NofRemoteComputers = 0;
if ( RemoteComputer != 0 )
    {
    delete[] RemoteComputer;
    RemoteComputer = 0;
    }

c.rewind();
while ( c.next() )
    {
    s = c.directory();
    if ( convert_to_mailslot_name(s) )
        t.append( s.text() );
    }
n = t.count();
if ( n > 0 )
    {
    RemoteComputer = new REMOTE_COMPUTER_ENTRY[n];
    if ( RemoteComputer )
        {
        t.rewind();
        for ( i=0; i<n; i++ )
            {
            t.next();
            RemoteComputer[i].name = t.text();
            cp = t.text();
            RemoteComputer[i].is_online = TRUE;
            }
        NofRemoteComputers = n;
        }
    }
}

/***********************************************************************
*                             SHOW_STATUS                              *
***********************************************************************/
static void show_status()
{
int i;
STRING_CLASS s;

for ( i=0; i<NofRemoteComputers; i++ )
    {
    s = RemoteComputer[i].name;
    s += TEXT( "   " );
    if ( RemoteComputer[i].is_online )
        s += TEXT( "On Line" );
    else
        s += TEXT( "OFF Line" );
    show_message( s.text() );
    }
}

/***********************************************************************
*                               SEND_THREAD                            *
***********************************************************************/
DWORD send_thread( int * notused )
{
//const  DWORD MS_PER_MINUTE = 60000UL;
const  DWORD MS_PER_MINUTE = 6000UL;
static DWORD last_tick_count;
SOCKET send_socket;
STRING_CLASS msg;
STRING_CLASS computer;
TCHAR * cp;
TCHAR * s;
int     slen;
DWORD   current_tick_count;
BOOLEAN was_sent;
LPHOSTENT   hp;
sockaddr_in a;

a.sin_family           = PF_INET;
a.sin_port             = DEFAULT_LISTENER_PORT;

/*
-------------------------------------------------------------------
Initialize the ms counter that I use to check for offline computers
------------------------------------------------------------------- */
last_tick_count = GetTickCount();

while ( TRUE )
    {
    if ( !HaveMailslots )
        break;

    if ( WaitForSingleObject(SendEvent, INFINITE) == WAIT_FAILED )
        resource_message_box( UNABLE_TO_CONTINUE_STRING, MAIL_MONITOR_WAIT_FAIL_STRING );

    if ( !HaveMailslots )
        break;

    /*
    ------------------------------------
    Check offline computers every minute
    ------------------------------------ */
    current_tick_count = GetTickCount();
    if ( current_tick_count > last_tick_count )
        {
        if ( (current_tick_count - last_tick_count) > MS_PER_MINUTE )
            check_offline_computers();
        }
    last_tick_count = current_tick_count;

    while ( SendData.count() )
        {
        s = (TCHAR *) SendData.pop();
        if ( s )
            {
            cp = s + 2; /* The first two chars are spaces */
            if ( strings_are_equal(cp, ReloadRemoteComputers, lstrlen(ReloadRemoteComputers)) )
                {
                /*
                ---------------------------------------------------------------------------------------------
                This is a local command only. Since this thread is the only one using the RemoteComputer list
                I need to reload it here also.
                --------------------------------------------------------------------------------------------- */
                reload_remote_computers();
                }
            else if ( strings_are_equal(cp, ShowStatusRequest, lstrlen(ShowStatusRequest)) )
                {
                show_status();
                }
            else
                {
                /*
                -------------------------
                Put a line in the listbox
                ------------------------- */
                if ( ListEvents )
                    {
                    msg = "Send[";
                    msg += cp;
                    msg += "]";
                    show_message( msg.text() );
                    }


                /*
                ----------------
                Send the message
                ---------------- */
                cp = findchar( CommaChar, s );
                if ( cp )
                    {
                    *cp = NullChar;
                    cp++;
                    slen = lstrlen(cp) + 1;  /* Include the terminating null */
                    slen *= sizeof( TCHAR );

                    /*
                    -------------------------------------------------------
                    Save the computer name in case I need to set it offline
                    ------------------------------------------------------- */
                    computer = s+2;

                    was_sent = FALSE;
                    hp = gethostbyname( s+2 );
                    if ( hp )
                        {
                        send_socket = socket( PF_INET, SOCK_STREAM, 0 );
                        if ( send_socket != INVALID_SOCKET )
                            {
                            a.sin_addr = *(in_addr *)(hp->h_addr);
                            cp -= LEN_MSG_COUNT; /* I know there is room to backup because I padded the start */
                            insalph( cp, slen, LEN_MSG_COUNT, ZeroChar, DECIMAL_RADIX );
                            if ( connect(send_socket, (sockaddr *) &a, sizeof(a)) != SOCKET_ERROR )
                                {
                                slen += LEN_MSG_COUNT * sizeof(TCHAR);

                                if ( send(send_socket, (char *)cp, slen, 0) != SOCKET_ERROR )
                                    was_sent = TRUE;

                                }
                            closesocket( send_socket );
                            send_socket = INVALID_SOCKET;
                            }
                        }

                    if ( !was_sent )
                        set_computer_offline( computer );
                    }
                }
            delete[] s;
            }
        }
    }

while ( SendData.count() )
    {
    s = (TCHAR *) SendData.pop();
    if ( s )
        delete[] s;
    }

if ( SendEvent )
    {
    CloseHandle( SendEvent );
    SendEvent = 0;
    }

if ( send_socket != INVALID_SOCKET )
    {
    closesocket( send_socket );
    send_socket = INVALID_SOCKET;
    }

if ( !ConnectEvent )
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                             TO_MAILSLOT                              *
* This is called from eventman and is not in the send_thread.          *
* I pad the beginning of each dest name with two spaces so I will      *
* always have enough room to put the string length in the first four   *
* characters of the message. Mailslots are no longer used.             *
***********************************************************************/
void to_mailslot( TCHAR * dest, TCHAR * sorc )
{
TCHAR * cp;
TCHAR * s;
int32   slen;

if ( !HaveMailslots )
    return;

slen = 2 + lstrlen( dest ) + 1 + lstrlen( sorc );
s = maketext( slen );
if ( s )
    {
    cp = copystring( s, TwoSpaces );
    cp = copy_w_char( cp, dest, CommaChar );
    copystring( cp, sorc );
    SendData.push( s );
    SetEvent( SendEvent );
    }
}

/***********************************************************************
*                         SEND_MAILSLOT_EVENT                          *
* This is called from eventman and is not in the send_mailslot_thread  *
* I pad the beginning of each dest name with two spaces so I will      *
* always have enough room to put the string length in the first four   *
* characters of the message.                                           *
***********************************************************************/
void send_mailslot_event( TCHAR * topic, TCHAR * item, TCHAR * data )
{
int     count;
int     i;
int     slen;
TCHAR * s;
TCHAR * cp;

if ( !HaveMailslots )
    return;

if ( strings_are_equal(item,ItemList[DS_NOTIFY_INDEX].name) )
    {
    if ( HaveDataServer )
        {
        slen = 4; /* Room for two leading spaces + Room for commas in {topic,item,data} */
        slen += lstrlen(topic) + lstrlen(item) + lstrlen(data);

        s = maketext( slen );
        if ( s )
            {
            cp = copystring( s, TwoSpaces );
            cp = copy_w_char( cp,  topic, CommaChar );
            cp = copy_w_char( cp, item,  CommaChar );
            copystring( cp, data );
            to_mailslot( DataServerComputer.text(), s );
            delete[] s;
            }
        }
    return;
    }

count = 0;
for ( i=0; i<NofRemoteComputers; i++ )
    {
    if ( RemoteComputer[i].is_online )
        {
        slen = RemoteComputer[i].name.len();
        slen += 5; /* Room for two leading spaces + Room for commas in {computer,topic,item,data} */
        slen += lstrlen(topic) + lstrlen(item) + lstrlen(data);

        s = maketext( slen );
        if ( s )
            {
            cp = copystring( s, TwoSpaces );
            cp = copy_w_char( cp,  RemoteComputer[i].name.text(), CommaChar );
            cp = copy_w_char( cp, topic, CommaChar );
            cp = copy_w_char( cp, item,  CommaChar );
            copystring( cp, data );
            SendData.push( s );
            count++;
            }
        }
    }

if ( count > 0 )
    SetEvent( SendEvent );
}

/***********************************************************************
*                        STOP_MAILSLOT_THREADS                         *
***********************************************************************/
void stop_mailslot_threads()
{
HaveMailslots = FALSE;
closesocket( ListenSocket );
SetEvent( SendEvent );
}

/***********************************************************************
*                          START_MAILSLOT_THREADS                      *
***********************************************************************/
BOOLEAN start_mailslot_threads()
{
HANDLE h;
DWORD  id;

reload_remote_computers();

ConnectEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
SendEvent    = CreateEvent( NULL, FALSE, FALSE, NULL );

HaveMailslots = TRUE;

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) send_thread,          /* thread function               */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    {
    HaveMailslots = FALSE;
    return FALSE;
    }

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) connect_thread,       /* thread function               */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    {
    HaveMailslots = FALSE;
    return FALSE;
    }

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) listen_thread,        /* thread function               */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    {
    HaveMailslots = FALSE;
    return FALSE;
    }

return TRUE;
}
