#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\listbox.h"
#include "..\include\stringcl.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"

const char CrChar   = '\r';
const char NullChar = '\0';
const char PlusChar = '+';

/*
-------------------------------------------------------------------------------------
Note: This server acts like the altera, in that it reflects single character messages
back to the sender.

Received messages are left in the buffer until a carriage return is received. This is
replaced by a null and the message handler is called.
------------------------------------------------------------------------------------- */

CRITICAL_SECTION My_Critical_Section;
SOCKET           MySocket = INVALID_SOCKET;
SOCKET           Listener;
BOOLEAN          ShuttingDown = FALSE;
sockaddr_in      My_Address;
TEXT_LIST_CLASS  SendList;
HANDLE           SendEvent = 0;
BOOLEAN          Read_Thread_Is_Running = FALSE;
BOOLEAN          Send_Thread_Is_Running = FALSE;

extern LISTBOX_CLASS MainListBox;

static BOOLEAN begin_listening();
void send( TCHAR * sorc );
void shutdown_callback();
void socket_message_callback( char * sorc );

/***********************************************************************
*                           SHUTDOWN_SERVER                            *
***********************************************************************/
void shutdown_server()
{
EnterCriticalSection( &My_Critical_Section );

if ( MySocket != INVALID_SOCKET )
    {
    closesocket( MySocket );
    MySocket = INVALID_SOCKET;
    }

LeaveCriticalSection( &My_Critical_Section );

if ( ShuttingDown )
    {
    if ( Listener != INVALID_SOCKET )
        {
        MainListBox.add( "Closing down the listerer" );
        closesocket( Listener );
        Listener = INVALID_SOCKET;
        MainListBox.add( "Listener is closed" );
        }

    if ( SendEvent )
        {
        CloseHandle( SendEvent );
        SendEvent = 0;
        }

    DeleteCriticalSection( &My_Critical_Section );
    shutdown_callback();
    }
else
    {
    begin_listening();
    }
}

/***********************************************************************
*                              READ_THREAD                             *
***********************************************************************/
DWORD read_thread( int * notused )
{

const int RECVBUF_LEN = 32768;
static char recvbuf[RECVBUF_LEN];

int    n;
int    destlen;
char * dest;
char * s;

Read_Thread_Is_Running = TRUE;

dest    = recvbuf;
destlen = RECVBUF_LEN;

while ( TRUE )
    {
    n = recv( MySocket, dest, destlen, 0 );
    if ( n == 0 || n == SOCKET_ERROR )
        break;

    /*
    -------------------------------------------------------------
    Reduce the buffer length by the number of characters received
    ------------------------------------------------------------- */
    destlen -= n;

    while ( n )
        {
        n--;
        if ( *dest == CrChar || *dest == NullChar )
            {
            *dest = NullChar;

            s = maketext( recvbuf );
            if ( s )
                socket_message_callback( s );
            if ( n > 0 )
                memcpy( recvbuf, dest, n );
            dest = recvbuf+n;
            destlen = RECVBUF_LEN - n;
            }
        else
            {
            dest++;
            }
        }
    }

Read_Thread_Is_Running = FALSE;
if ( Send_Thread_Is_Running )
    {
    SetEvent( SendEvent );
    }
else
    {
    shutdown_server();
    }

return 0;
}

/***********************************************************************
*                               SEND_THREAD                            *
***********************************************************************/
DWORD send_thread( int * notused )
{
char         * sorc;

Send_Thread_Is_Running = TRUE;

while ( TRUE )
    {
    if ( WaitForSingleObject(SendEvent, INFINITE) == WAIT_FAILED )
        break;

    /*
    ----------------------------
    Cleanup before shutting down
    ---------------------------- */
    if ( ShuttingDown || !Read_Thread_Is_Running )
        break;

    while ( TRUE )
        {
        sorc = 0;
        EnterCriticalSection( &My_Critical_Section );
        SendList.rewind();
        if ( SendList.next() )
            sorc = SendList.release();
        LeaveCriticalSection( &My_Critical_Section );

        if ( !sorc )
            break;

        send( MySocket, sorc, lstrlen(sorc)+1, 0 );

        /*
        ----------------------------------------------------
        I own the socket_entry that I popped off of the list
        ---------------------------------------------------- */
        delete[] sorc;
        }
    }

Send_Thread_Is_Running = FALSE;
shutdown( MySocket, 1 );
if ( !Read_Thread_Is_Running )
    shutdown_server();

return 0;
}

/***********************************************************************
*                               SEND                                   *
***********************************************************************/
void send( TCHAR * sorc )
{
EnterCriticalSection( &My_Critical_Section );

SendList.append( sorc );

LeaveCriticalSection( &My_Critical_Section );

SetEvent( SendEvent );
}

/***********************************************************************
*                           START_MY_SOCKET                            *
***********************************************************************/
BOOLEAN start_my_socket()
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                   /* no security attributes        */
    0,                                      /* use default stack size        */
    (LPTHREAD_START_ROUTINE) read_thread,   /* thread function       */
    0,                                      /* argument to thread function   */
    0,                                      /* use default creation flags    */
    &id );                                  /* returns the thread identifier */

if ( !h )
    return FALSE;

h = CreateThread(
    NULL,
    0,
    (LPTHREAD_START_ROUTINE) send_thread,
    0,
    0,
    &id );

if ( !h )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            LISTEN_THREAD                             *
***********************************************************************/
DWORD server_listen_thread( int * notused )
{
int    status;

Listener = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
if ( Listener != INVALID_SOCKET )
    {
    status = bind( Listener, (SOCKADDR *) &My_Address, sizeof(My_Address) );
    if ( status == 0 )
        {
        status = listen( Listener, 1 );
        if ( status == 0 )
            {
            /*
            ----------------------------------
            Wait for someone to try to connect
            ---------------------------------- */
            MySocket = accept( Listener, 0, 0 );
            if ( MySocket != INVALID_SOCKET )
                start_my_socket();
            }
        }
    }

/*
--------------------------------------
I only accept one connection at a time
-------------------------------------- */
closesocket( Listener );
Listener = INVALID_SOCKET;

return 0;
}

/***********************************************************************
*                             get_my_address                           *
***********************************************************************/
void get_my_address( char * dest )
{
int         status;
HOSTENT   * hostname;
in_addr     address;
in_addr   * ap;
char        buf[256];
char      * sorc;

status = gethostname( buf, 255 );
if ( status != SOCKET_ERROR )
    {
    hostname = gethostbyname( buf );
    address  = *(in_addr *)hostname->h_addr;
    ap       = (in_addr *)hostname->h_addr_list[0];
    if ( ap )
        {
        address = *ap;
        sorc    = inet_ntoa( address );
        while ( *sorc )
            *dest++ = *sorc++;
        *dest = '\0';
        }
    }
}

/***********************************************************************
*                           BEGIN_LISTENING                            *
***********************************************************************/
static BOOLEAN begin_listening()
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                           /* no security attributes        */
    0,                                              /* use default stack size        */
    (LPTHREAD_START_ROUTINE) server_listen_thread,  /* thread function       */
    0,                                              /* argument to thread function   */
    0,                                              /* use default creation flags    */
    &id );                                          /* returns the thread identifier */

if ( !h )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                           SOCKETS_SHUTDOWN                           *
***********************************************************************/
void sockets_shutdown()
{
ShuttingDown = TRUE;
if ( Send_Thread_Is_Running )
    {
    SetEvent( SendEvent );
    }
else
    {
    shutdown_server();
    }
}

/***********************************************************************
*                             SOCKETS_STARTUP                          *
***********************************************************************/
BOOLEAN sockets_startup( char * tcp_address, u_short port )
{
const WORD  SocketVersion = MAKEWORD(2,0);
WSADATA  wsa_data;

if ( WSAStartup(SocketVersion, &wsa_data) != 0 )
    return FALSE;

InitializeCriticalSection( &My_Critical_Section );

SendEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !SendEvent )
    return FALSE;

if ( *tcp_address == '\0' )
    get_my_address( tcp_address );

My_Address.sin_family           = AF_INET;
My_Address.sin_port             = htons( port );
My_Address.sin_addr.S_un.S_addr = inet_addr( tcp_address );

return begin_listening();
}
