#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static BOOLEAN     Wsa_Is_Started = FALSE;
static SOCKET      MySocket = INVALID_SOCKET;
static sockaddr_in MyAddress;

char * CBuf = 0;
int    CBufLen = 0;

/***********************************************************************
*                            SEND_UDP_MESSAGE                          *
***********************************************************************/
void send_udp_message( sockaddr_in & dest, TCHAR * message )
{
int  slen;
char * cp;

slen = lstrlen( message );
if ( slen > CBufLen )
    {
    if ( CBuf )
        {
        delete[] CBuf;
        CBuf    = 0;
        CBufLen = 0;
        }
    cp = new char[slen+1];
    if ( cp )
        {
        CBuf = cp;
        CBufLen = slen;
        }
    }

if ( CBuf )
    {
    unicode_to_char( CBuf, message );
    sendto( MySocket, CBuf, slen, 0, (sockaddr *) &dest, sizeof(sockaddr_in) );
    }
}

/***********************************************************************
*                          UDP_SOCKET_STARTUP                          *
***********************************************************************/
void udp_socket_startup()
{
const u_short my_port    = 20001;
const WORD SocketVersion = MAKEWORD(2,0);

char      buf[257];
WSADATA   wsadata;
int       status;
HOSTENT * hostname;
in_addr * ap;

if ( WSAStartup(SocketVersion, &wsadata) )
    {
    MessageBox( 0, TEXT("WSAStartup"), int32toasc(WSAGetLastError()), MB_OK );
    return;
    }

Wsa_Is_Started = TRUE;
MySocket = socket( PF_INET, SOCK_DGRAM, 0 );
if ( MySocket == INVALID_SOCKET )
    {
    MessageBox( 0, TEXT("socket"), int32toasc(WSAGetLastError()), MB_OK );
    return;
    }

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
        MyAddress.sin_addr = *ap;
    }
}

/***********************************************************************
*                         UDP_SOCKET_SHUTDOWN                          *
***********************************************************************/
void udp_socket_shutdown()
{
if ( MySocket != INVALID_SOCKET )
    {
    closesocket( MySocket );
    MySocket = INVALID_SOCKET;
    }

if ( Wsa_Is_Started )
    {
    Wsa_Is_Started = FALSE;
    WSACleanup();
    }

if ( CBuf )
    {
    CBufLen = 0;
    delete[] CBuf;
    CBuf = 0;
    }
}
