#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\listbox.h"
#include "..\include\sockcli.h"
#include "..\include\subs.h"

bool CLIENT_SOCKET_CLASS::wsa_is_started = false;

/***********************************************************************
*                           NEW_SOCKET_ENTRY                           *
***********************************************************************/
SOCKET_ENTRY * new_socket_entry( int buflen )
{
SOCKET_ENTRY * se;

if ( buflen )
    {
    se = new SOCKET_ENTRY;
    if ( se )
        {
        se->bp = new byte[buflen];
        if ( se->bp )
            {
            se->len = buflen;
            return se;
            }
        delete se;
        }
    }

return 0;
}

/***********************************************************************
*                          SOCKET_ENTRY_QUEUE                          *
***********************************************************************/
SOCKET_ENTRY_QUEUE::SOCKET_ENTRY_QUEUE()
{
first = 0;
InitializeCriticalSection( &my_critical_section );
}

/***********************************************************************
*                         ~SOCKET_ENTRY_QUEUE                          *
***********************************************************************/
SOCKET_ENTRY_QUEUE::~SOCKET_ENTRY_QUEUE()
{
clear();
DeleteCriticalSection( &my_critical_section );
}

/***********************************************************************
*                          SOCKET_ENTRY_QUEUE                          *
*                                CLEAR                                 *
***********************************************************************/
void  SOCKET_ENTRY_QUEUE::clear( void )
{
SOCKET_ENTRY * se;

while ( true )
    {
    se = pop();
    if ( !se )
        break;
    delete se;
    }
}

/***********************************************************************
*                          SOCKET_ENTRY_QUEUE                          *
*                                 PUSH                                 *
***********************************************************************/
void  SOCKET_ENTRY_QUEUE::push( SOCKET_ENTRY * sorc )
{
if ( !sorc )
    return;

EnterCriticalSection( &my_critical_section );

sorc->next = first;

first = sorc;

LeaveCriticalSection( &my_critical_section );
}

/***********************************************************************
*                          SOCKET_ENTRY_QUEUE                          *
*                                  POP                                 *
***********************************************************************/
SOCKET_ENTRY * SOCKET_ENTRY_QUEUE::pop( void )
{
SOCKET_ENTRY * prev;
SOCKET_ENTRY * sp;

sp   = 0;

if ( first )
    {
    prev = 0;

    EnterCriticalSection( &my_critical_section );

    sp = first;
    while ( sp->next )
        {
        prev = sp;
        sp   = sp->next;
        }

    if ( prev )
        prev->next = 0;
    else
        first = 0;

    LeaveCriticalSection( &my_critical_section );
    }

return sp;
}

/***********************************************************************
*                              READ_THREAD                             *
***********************************************************************/
DWORD CLIENT_SOCKET_CLASS::read_thread( void )
{
int    status;
int    bytes_needed;
SOCKET_ENTRY * se;

/*
------------------------------------------------------------------------
The receive buffer is used only to get the type (WORD) and length (int).
------------------------------------------------------------------------ */
const int RECVBUF_LEN = sizeof(WORD) + sizeof(int);
byte   recvbuf[RECVBUF_LEN];
byte * cp;

read_thread_is_running = true;

while ( true )
    {
    status = recv( my_socket, (char *) recvbuf, RECVBUF_LEN, 0 );
    if ( status == 0 || status == SOCKET_ERROR )
        break;

    se = new SOCKET_ENTRY;
    if ( se )
        {
        se->sorc_socket = my_socket;

        cp = recvbuf;

        memcpy( &se->type, cp, sizeof(se->type) );
        cp += sizeof( se->type );

        memcpy( &se->len,  cp, sizeof(se->len) );
        if ( se->len > 0 )
            {
            se->bp = new byte[se->len];
            if ( se->bp )
                {
                cp = se->bp;
                bytes_needed = se->len;
                while ( bytes_needed > 0 )
                    {
                    status = recv( my_socket, (char *) cp, bytes_needed, 0 );
                    if ( status == 0 || status == SOCKET_ERROR )
                        break;
                    bytes_needed -= status;
                    cp += status;
                    }
                recv_callback( se );
                se = 0;
                }
            }

        if ( se )
            {
            delete se;
            se = 0;
            }
        }
    else
        {
        break;
        }
    }

read_thread_is_running = false;
if ( !send_thread_is_running )
    {
    closesocket( my_socket );
    my_socket = INVALID_SOCKET;
    if ( close_complete_callback )
        close_complete_callback();
    }
else
    {
    SetEvent( send_event );
    }
return 0;
}

/***********************************************************************
*                               SEND_THREAD                            *
***********************************************************************/
DWORD CLIENT_SOCKET_CLASS::send_thread( void )
{
byte         * cp;
int            buflen;
SOCKET_ENTRY * se;

send_thread_is_running = true;

while ( true )
    {
    if ( WaitForSingleObject(send_event, INFINITE) == WAIT_FAILED )
        break;

    /*
    ----------------------------
    Cleanup before shutting down
    ---------------------------- */
    if ( i_am_closing || !read_thread_is_running )
        {
        if ( send_event )
            {
            CloseHandle( send_event );
            send_event = 0;
            }
        while ( (se = sendlist.pop()) )
            delete se;
        break;
        }

    while ( true )
        {
        se = sendlist.pop();
        if ( !se )
            break;

        if ( se->bp && se->len )
            {
            buflen = sizeof(se->type) + sizeof(se->len) + se->len;

            /*
            -----------------------------------------------------------
            Make sure my buffer is big enough to hold the type and blob
            ----------------------------------------------------------- */
            if ( sendbuflen < buflen )
                {
                if ( sendbuf )
                    delete[] sendbuf;

                sendbuf = new byte[buflen];
                if ( sendbuf )
                    sendbuflen = buflen;
                else
                    sendbuflen = 0;
                }

            /*
            -----------------------------------------
            Copy the type and blob to the send buffer
            ----------------------------------------- */
            if ( sendbuflen > 0 )
                {
                cp = sendbuf;
                memcpy( cp, &se->type, sizeof(se->type) );
                cp += sizeof( se->type );

                memcpy( cp, &se->len,  sizeof(se->len) );
                cp += sizeof( se->len );

                memcpy( cp, se->bp, se->len );

                ::send( my_socket, (char *)sendbuf, buflen, 0 );
                }
            }

        /*
        ----------------------------------------------------
        I own the socket_entry that I popped off of the list
        ---------------------------------------------------- */
        delete se;
        }
    }

send_thread_is_running = false;
if ( !read_thread_is_running )
    {
    closesocket( my_socket );
    my_socket = INVALID_SOCKET;
    if ( close_complete_callback )
        close_complete_callback();
    }
return 0;
}

/***********************************************************************
*                                 SEND                                 *
***********************************************************************/
bool CLIENT_SOCKET_CLASS::send( SOCKET_ENTRY * se )
{
sendlist.push( se );
SetEvent( send_event );
return true;
}

/***********************************************************************
*                                 SEND                                 *
***********************************************************************/
bool CLIENT_SOCKET_CLASS::send( WORD message_type, void * sorc, DWORD bytes_to_send )
{
SOCKET_ENTRY * se;

if ( !sorc || !bytes_to_send )
    return false;

se = new_socket_entry( bytes_to_send );
if ( se )
    {
    se->type        = message_type;
    se->sorc_socket = my_socket;
    memcpy( se->bp, sorc, se->len );
    return send( se );
    }

if ( se )
    delete se;

return false;
}

/***********************************************************************
*                                 CLOSE                                *
***********************************************************************/
bool CLIENT_SOCKET_CLASS::close( void (*new_close_complete_callback)(void) )
{
close_complete_callback = new_close_complete_callback;
i_am_closing = true;

if ( my_socket != INVALID_SOCKET )
    shutdown( my_socket, 1 );

SetEvent( send_event );
return true;
}

/***********************************************************************
*                       START_CLIENT_READ_THREAD                       *
***********************************************************************/
DWORD start_client_read_thread( void * sp )
{
CLIENT_SOCKET_CLASS * s;

s = (CLIENT_SOCKET_CLASS *) sp;
return s->read_thread();
}

/***********************************************************************
*                       START_CLIENT_SEND_THREAD                       *
***********************************************************************/
DWORD start_client_send_thread( void * sp )
{
CLIENT_SOCKET_CLASS * s;

s = (CLIENT_SOCKET_CLASS *) sp;
return s->send_thread();
}

/***********************************************************************
*                          CLIENT_SOCKET_CLASS                         *
*                                  OPEN                                *
***********************************************************************/
bool CLIENT_SOCKET_CLASS::open( void (*new_recv_callback)(SOCKET_ENTRY * se), char * ascii_tcp_ip_address, u_short port )
{
HANDLE h;
int    status;
sockaddr_in a;
DWORD       id;

i_am_closing = false;

/*
------------------------------------------
Save the function to which I send receives
------------------------------------------ */
recv_callback = new_recv_callback;

/*
---------------
Create a socket
--------------- */
my_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

if ( my_socket == INVALID_SOCKET )
    return false;

/*
-------------------------------
Connect to the address and port
------------------------------- */
a.sin_family           = AF_INET;
a.sin_port             = htons( port );
a.sin_addr.S_un.S_addr = inet_addr( ascii_tcp_ip_address );

status = connect( my_socket, (sockaddr *) &a, sizeof(a) );
if ( status == SOCKET_ERROR )
    return false;

send_event = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !send_event )
    return false;

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) start_client_read_thread,   /* thread function       */
    (void *) this,                                 /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    return false;

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) start_client_send_thread,   /* thread function       */
    (void *) this,                                 /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    return false;

return true;
}

/***********************************************************************
*                         CLIENT_SOCKET_CLASS                          *
***********************************************************************/
CLIENT_SOCKET_CLASS::CLIENT_SOCKET_CLASS()
{
i_am_closing           = false;
send_event             = 0;
my_socket              = INVALID_SOCKET;
sendbuf                = 0;
sendbuflen             = 0;
read_thread_is_running = false;
send_thread_is_running = false;
}
 
/***********************************************************************
*                        ~CLIENT_SOCKET_CLASS                          *
***********************************************************************/
CLIENT_SOCKET_CLASS::~CLIENT_SOCKET_CLASS()
{
if ( sendbuf )
    {
    delete[] sendbuf;
    sendbuf    = 0;
    sendbuflen = 0;
    }
}

/***********************************************************************
*                         CLIENT_SOCKET_CLASS                          *
*                       startup_windows_sockets                        *
***********************************************************************/
void CLIENT_SOCKET_CLASS::startup_windows_sockets( void )
{     
const WORD SocketVersion = MAKEWORD(2,0);
WSADATA wsadata;

if ( !wsa_is_started )
    {
    WSAStartup( SocketVersion, &wsadata );
    wsa_is_started = true;
    }
}

/***********************************************************************
*                         CLIENT_SOCKET_CLASS                          *
*                       cleanup_windows_sockets                        *
***********************************************************************/
void CLIENT_SOCKET_CLASS::cleanup_windows_sockets( void )
{

if ( wsa_is_started )
    {
    WSACleanup();
    wsa_is_started = false;
    }

}
