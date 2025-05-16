#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"

#include "socket_class.h"

BOOLEAN SOCKET_CLASS::wsa_is_started = FALSE;

/***********************************************************************
*                            SOCKET_CLASS                              *
*                                clear                                 *
*                                                                      *
* This function initializes the variables (except for the critical     *
* section and send_event) to a known state.                            *
*                                                                      *
***********************************************************************/
void SOCKET_CLASS::clear()
{
read_thread_is_running  = FALSE;
send_thread_is_running  = FALSE;
i_am_closing            = FALSE;
i_am_open               = FALSE;
mysocket                = INVALID_SOCKET;
close_complete_callback = 0;
have_new_data_event     = 0;
lastchar                = 0;
inptr                   = 0;
outptr                  = 0;
error                   = 0;
}

/***********************************************************************
*                            SOCKET_CLASS                              *
***********************************************************************/
SOCKET_CLASS::SOCKET_CLASS()
{
buf = 0;
clear();

InitializeCriticalSection( &my_critical_section );
send_event = CreateEvent( NULL, FALSE, FALSE, NULL );
}

/***********************************************************************
*                            ~SOCKET_CLASS                             *
***********************************************************************/
SOCKET_CLASS::~SOCKET_CLASS()
{
if ( send_event )
    {
    CloseHandle( send_event );
    send_event = 0;
    }

if ( buf )
    {
    delete[] buf;
    buf            = 0;
    lastchar       = 0;
    inptr          = 0;
    outptr         = 0;
    }

DeleteCriticalSection( &my_critical_section );
}

/***********************************************************************
*                            SOCKET_CLASS                              *
*                              startup                                 *
***********************************************************************/
void SOCKET_CLASS::startup()
{
const WORD SocketVersion = MAKEWORD(2,0);
WSADATA wsadata;

if ( !wsa_is_started )
    {
    WSAStartup( SocketVersion, &wsadata );
    wsa_is_started = TRUE;
    }
}

/***********************************************************************
*                            SOCKET_CLASS                              *
*                              shutdown                                *
***********************************************************************/
void SOCKET_CLASS::shutdown()
{
if ( wsa_is_started )
    {
    WSACleanup();
    wsa_is_started = FALSE;
    }
}

/***********************************************************************
*                            SOCKET_CLASS                              *
*                             is_running                               *
***********************************************************************/
BOOLEAN SOCKET_CLASS::is_running()
{
if ( read_thread_is_running && send_thread_is_running )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            SOCKET_CLASS                              *
*                               close                                  *
***********************************************************************/
void SOCKET_CLASS::close()
{
if ( mysocket != INVALID_SOCKET && i_am_open )
    {
    EnterCriticalSection( &my_critical_section );

    closesocket( mysocket );
    i_am_open = FALSE;

    sendlist.empty();
    if ( buf )
        {
        delete[] buf;
        buf            = 0;
        lastchar       = 0;
        inptr          = 0;
        outptr         = 0;
        }

    LeaveCriticalSection( &my_critical_section );
    }

if ( close_complete_callback )
    close_complete_callback();
}

/***********************************************************************
*                            SOCKET_CLASS                              *
*                               close                                  *
***********************************************************************/
void SOCKET_CLASS::close( void (*new_close_complete_callback)() )
{
close_complete_callback = new_close_complete_callback;

i_am_closing = TRUE;
if ( send_thread_is_running )
    {
    SetEvent( send_event );
    }
else
    {
    close();
    }
}

/***********************************************************************
*                            SOCKET_CLASS                              *
*                            read_thread                               *
***********************************************************************/
DWORD SOCKET_CLASS::read_thread()
{
char * sorc;
int    len;
int chars_received;

read_thread_is_running = TRUE;

while ( true )
    {
    chars_received = recv( mysocket, recvbuf, RECVBUF_LEN, 0 );
    if ( chars_received == 0 || chars_received == SOCKET_ERROR )
        {
        if ( i_am_closing )
            break;

        if ( chars_received == SOCKET_ERROR )
            {
            error = WSAGetLastError();
            /*
            -------------------------------------------------------------------
            It is unlikely but possible that the send thread is already stopped
            ------------------------------------------------------------------- */
            if ( !send_thread_is_running )
                close();
            }

        break;
        }

    len = 1 + lastchar - inptr;

    sorc = recvbuf;
    if ( len > chars_received )
        len = chars_received;

    if ( len > 0 )
        {
        memcpy( inptr, sorc, len );
        if ( inptr+len > lastchar )
            {
            inptr = buf;
            chars_received -= len;
            if ( chars_received > 0 )
                {
                sorc += len;
                len = chars_received;
                memcpy( inptr, sorc, len );
                inptr += len;
                }
            }
        else
            {
            inptr += len;
            }

        /*
        -------------------------------------
        Tell my parent that there is new data
        ------------------------------------- */
        SetEvent( have_new_data_event );
        }
    }

read_thread_is_running = FALSE;
if ( send_thread_is_running )
    {
    SetEvent( send_event );
    }
/*
-------------------------------------------------------------------
else
    {
    close();
    }

This is the proper way to do this but it doesn't work with the ftII
The ftII doesn't close until I closesocket. I changed the send
thread to closesocket but now the read thread
gets a close message before closesocket returns and
I have time to set mysocket to INVALID
and so the closesocket gets called again.
I don't think I need to call close at all from here.
------------------------------------------------------------------- */

if ( !i_am_closing )
    SetEvent( have_new_data_event );

return 0;
}

/***********************************************************************
*                            SOCKET_CLASS                              *
*                            send_thread                               *
***********************************************************************/
DWORD SOCKET_CLASS::send_thread()
{
char * sorc;

send_thread_is_running = TRUE;

while ( true )
    {
    if ( WaitForSingleObject(send_event, INFINITE) == WAIT_FAILED )
        break;

    if ( !read_thread_is_running )
        break;

    while ( true )
        {
        sorc = 0;
        EnterCriticalSection( &my_critical_section );
        sendlist.rewind();
        if ( sendlist.next() )
            sorc = sendlist.release();
        LeaveCriticalSection( &my_critical_section );

        if ( !sorc )
            break;

        if ( !i_am_closing )
            ::send( mysocket, sorc, lstrlen(sorc), 0 );

        /*
        ----------------------------------------------------
        I own the socket_entry that I popped off of the list
        ---------------------------------------------------- */
        delete[] sorc;
        }

    if ( i_am_closing )
        break;

    }

send_thread_is_running = FALSE;

/*
------------------------------------------------------------------
ftII board doesn't close in response to them so I just close here.
The following two lines are the "proper" way to shutdown but the
::shutdown( mysocket, 1 );
if ( !read_thread_is_running )
------------------------------------------------------------------ */
    close();

return 0;
}

/***********************************************************************
*                            SOCKET_CLASS                              *
*                                send                                  *
***********************************************************************/
void SOCKET_CLASS::send( TCHAR * sorc )
{
if ( mysocket == INVALID_SOCKET )
    return;

EnterCriticalSection( &my_critical_section );

sendlist.append( sorc );

LeaveCriticalSection( &my_critical_section );

SetEvent( send_event );
}

/***********************************************************************
*                         START_READ_THREAD                            *
***********************************************************************/
DWORD start_read_thread( void * sp )
{
SOCKET_CLASS * s;

s = (SOCKET_CLASS *) sp;
return s->read_thread();
}

/***********************************************************************
*                          START_SEND_THREAD                           *
***********************************************************************/
DWORD start_send_thread( void * sp )
{
SOCKET_CLASS * s;

s = (SOCKET_CLASS *) sp;
return s->send_thread();
}

/***********************************************************************
*                            SOCKET_CLASS                              *
*                                open                                  *
***********************************************************************/
BOOLEAN SOCKET_CLASS::open( HANDLE new_data_notify_event, int buflen, char * ascii_tcp_ip_address, u_short port )

{
int    len;
int    status;
int    x;
DWORD  id;
HANDLE h;
sockaddr_in a;
linger      mylinger;

clear();

if ( !new_data_notify_event )
    return FALSE;

if ( buflen <= 0 )
    return FALSE;

have_new_data_event = new_data_notify_event;

if ( buf )
    delete[] buf;

buf = new char[buflen+1];

lastchar       = buf + buflen -1;
inptr          = buf;
outptr         = buf;

mysocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

if ( mysocket == INVALID_SOCKET )
    {
    error = WSAGetLastError();
    return FALSE;
    }

x = 131072;
len = sizeof( x );
if ( setsockopt( mysocket, SOL_SOCKET, SO_RCVBUF, (char *)&x, len) != 0 )
    MessageBox( 0, TEXT("Socket Error"), TEXT("setsockopt"), MB_OK );

/*
------------------------------------------------------
Set the socket to allow me to do the close handshaking
=> Hard close.
------------------------------------------------------ */
mylinger.l_onoff  = 1;
mylinger.l_linger = 0;

len = sizeof( mylinger );
if ( setsockopt( mysocket, SOL_SOCKET, SO_LINGER, (char *)&mylinger, len) != 0 )
    MessageBox( 0, TEXT("Socket Error"), TEXT("Set SO_LINGER"), MB_OK );

/*
-------------------------------
Connect to the address and port
------------------------------- */
a.sin_family           = AF_INET;
a.sin_port             = htons( port );
a.sin_addr.S_un.S_addr = inet_addr( ascii_tcp_ip_address );

status = connect( mysocket, (sockaddr *) &a, sizeof(a) );
if ( status == SOCKET_ERROR )
    {
    error = WSAGetLastError();
    return FALSE;
    }

i_am_open = TRUE;

h = CreateThread(
    NULL,                                         /* no security attributes        */
    0,                                            /* use default stack size        */
    (LPTHREAD_START_ROUTINE) start_read_thread,   /* thread function       */
    (void *) this,                                /* argument to thread function   */
    0,                                            /* use default creation flags    */
    &id );                                        /* returns the thread identifier */

if ( !h )
    return FALSE;

h = CreateThread(
    NULL,
    0,
    (LPTHREAD_START_ROUTINE) start_send_thread,
    (void *) this,
    0,
    &id );

if ( !h )
    return FALSE;

return TRUE;
}
