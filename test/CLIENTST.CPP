#include <windows.h>
#include <stdio.h>
#include <conio.h>

typedef unsigned short BOARD_DATA;
typedef long int int32;

#include "..\boardman\boardman.h"
#include "..\include\visiparm.h"
#include "..\include\subs.h"

const WORD    SocketVersion = MAKEWORD(2,0);
const u_short TcpIpPort     = 40322;

const int      RECV_BUF_LEN = 30000;
char           RecvBuf[RECV_BUF_LEN];
const char     NullChar = '\0';
 
BOARD_DATA CurrentBoardNumber = 1;

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
TCHAR * getline( void )
{
static TCHAR buf[103];
buf[0] = 100;

cgets( buf );

return buf+2;
}

SOCKET      s;
BOARD_DATA  d[10];
char        SendBuf[100];

/***********************************************************************
*                            SEND_TO_BOARD                             *
***********************************************************************/
void send_to_board( int n )
{
static WORD type = BOARD_DATA_SOCKET_ENTRY_TYPE;
DWORD   len;
char  * cp;

len = n * sizeof( BOARD_DATA );

cp = SendBuf;

memcpy( cp, &type, sizeof(type) );
cp += sizeof( type );

memcpy( cp, &len,  sizeof(len) );
cp += sizeof( len );

memcpy( cp, d, len );

len += sizeof(type);
len += sizeof(len);

cprintf( "Send...\n\r" );
send( s, SendBuf, len, 0 );

int i;
for ( i=0; i< int(len); i++ )
    cprintf( "%2d %4x\n\r", i, (int) (unsigned char) SendBuf[i] );

cprintf( "\n\r" );

}


/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
sockaddr_in a;
WSADATA     wsadata;
char      * cp;
char      * cp1;
int         status;
BOARD_DATA * bp;

if ( WSAStartup( SocketVersion, &wsadata) )
    {
    cprintf( "Unable to start sockets\n\r" );
    return 0;
    }

a.sin_family           = AF_INET;
a.sin_port             = htons( TcpIpPort );
a.sin_addr.S_un.S_addr = inet_addr( "192.168.0.2" );

cprintf( "\n\rCreate socket...\n\r" );
s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
if ( s != INVALID_SOCKET )
    {
    cprintf( "Connecting...\n\r" );
    if ( !connect(s, (sockaddr *) &a, sizeof(a)) )
        {
        cprintf( "Setting Port Address.\n\r" );

        d[COMMAND_INDEX] = SET_PORT_ADDRESS;
        d[BOARD_INDEX]   = 1;
        d[DATA_INDEX]    = 0x1C0;
 
        send_to_board( 3 );
 
        d[COMMAND_INDEX] = SET_PORT_ADDRESS;
        d[BOARD_INDEX]   = 2;
        d[DATA_INDEX]    = 0x220;

        send_to_board( 3 );

        while ( true )
            {
            cp = getline();

            if ( *cp == 'r' )
                {
                d[COMMAND_INDEX] = GET_BOARD_STATUS;
                d[BOARD_INDEX]   = CurrentBoardNumber;
                cprintf( "Asking for status...\n\r" );
                send_to_board( 2 );
                }
            else if ( *cp == 'b' || *cp == 'B' )
                {
                *cp = 'b';
                CurrentBoardNumber = (BOARD_DATA) hextoul( cp+1 );
                if ( CurrentBoardNumber < 1 || CurrentBoardNumber > 2 )
                    CurrentBoardNumber = 2;
                }
            else if ( *cp == 'm' )
                {
                d[COMMAND_INDEX] = SET_COMMAND_MODE;
                d[BOARD_INDEX]   = CurrentBoardNumber;
                cprintf( "Asking for COMMAND_MODE...\n\r" );
                send_to_board( 2 );
                }
            else if ( *cp == 'd' )
                {
                d[COMMAND_INDEX] = SET_DATA_COLLECTION_MODE;
                d[BOARD_INDEX]   = CurrentBoardNumber;
                cprintf( "Asking for DATA_COLLECTION_MODE...\n\r" );
                send_to_board( 2 );
                }
            else if ( *cp == 'p' || *cp == 'P' )
                {
                *cp = 'p';
                d[COMMAND_INDEX] = SET_BOARD_PARAMETER;
                d[BOARD_INDEX]   = CurrentBoardNumber;
                cp1 = cp+1;
                d[DATA_INDEX]    = (BOARD_DATA) hextoul( cp1 );
                while ( *cp1 != ' ' )
                    cp1++;
                while ( *cp1 == ' ' )
                    cp1++;
                d[DATA_INDEX+1]  = (BOARD_DATA) hextoul( cp1 );
                cprintf( "Asking for SET_BOARD_PARAMETER...\n\r" );
                send_to_board( 4 );
                }
            else if ( *cp == 's' || *cp == 'S' )
                {
                *cp = 's';
                d[COMMAND_INDEX] = SEND_COMMAND_BUSY;
                d[BOARD_INDEX]   = CurrentBoardNumber;
                d[DATA_INDEX]    = (BOARD_DATA) hextoul( cp+1 );
                cprintf( "Asking for SEND_COMMAND_BUSY...\n\r" );
                send_to_board( 3 );
                }
            else if ( *cp == 't' || *cp == 'T' )
                {
                *cp = 't';
                d[COMMAND_INDEX] = GET_SHOT_DATA;
                d[BOARD_INDEX]   = CurrentBoardNumber;
                cprintf( "Asking for GET_SHOT_DATA...\n\r" );
                send_to_board( 2 );
                }
            else if ( *cp >= '0' && *cp <= '9' )
                {
                d[COMMAND_INDEX] = GET_BOARD_PARAMETER;
                d[BOARD_INDEX]   = CurrentBoardNumber;
                d[DATA_INDEX]    = (BOARD_DATA) hextoul( cp );
                cprintf( "Asking for parameter 0x%4X...\n\r", d[DATA_INDEX] );
                send_to_board( 3 );
                }
            else
                break;

            if ( *cp != 'm' && *cp != 'd' && *cp != 'p' && *cp != 's' && *cp != 'b' )
                {
                cprintf( "Waiting for answer...\n\r" );
                status = recv( s, RecvBuf, RECV_BUF_LEN, 0 );
                if ( status != 0 && status != SOCKET_ERROR )
                    {
                    RecvBuf[status] = NullChar;
                    bp = (BOARD_DATA *) (RecvBuf+6);
                    cprintf( "[ %4X %4X %4X %4X\n\r", (int) bp[0], (int) bp[1],(int) bp[2], (int) bp[3] );
                    }
                }
            }

        cprintf( "shutdown...\n\r" );
        shutdown( s, 1 );

        cprintf( "Closesocket...\n\r" );
        closesocket(s);
        }
    else
        {
        cprintf( "connect error %d \n\r", (int) WSAGetLastError() );
        }
    }
else
    {
    cprintf( "socket error %d \n\r", (int) WSAGetLastError() );
    }

cprintf( "WSACleanup...\n\r" );
WSACleanup();

return 0;
}

