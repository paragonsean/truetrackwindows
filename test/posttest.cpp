#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

const WORD    SocketVersion = MAKEWORD(2,0);
const u_short TcpIpPort     = 80;

const int      RECV_BUF_LEN = 3000;
char           RecvBuf[RECV_BUF_LEN];
const char     NullChar = '\0';

char Data[] ="<Machine_Setup>\r\n"
"  <Parameters>\r\n"
"    <parameter ParameterID=\"EOSV\" Description=\"Target End-Of-Shot Velocity\" Units=\"in/sec\" Precision=\"1\" HelValue=\"205\" HalEnabled=\"False\" HalValue=\"0\" HwlEnabled=\"False\" HwlValue=\"0\" TargetValueType=\"0\" TargetValue=\"0\" CurrentValueType=\"2\" CurrentValue=\"1\" LwlEnabled=\"False\" LwlValue=\"0\" LalEnabled=\"False\" LalValue=\"0\" LelValue=\"0\" />\r\n"
"    <parameter ParameterID=\"FIP\" Description=\"Final Intensifier Pressure\" Units=\"psi\" Precision=\"0\" HelValue=\"0\" HalEnabled=\"False\" HalValue=\"0\" HwlEnabled=\"False\" HwlValue=\"0\" TargetValueType=\"0\" TargetValue=\"0\" CurrentValueType=\"1\" CurrentValue=\"0\" LwlEnabled=\"False\" LwlValue=\"0\" LalEnabled=\"False\" LalValue=\"0\" LelValue=\"0\" />\r\n"
"  </Parameters>\r\n"
"  <Messages>\r\n"
"    <Fault Index=\"1\" State=\"Init\" Message=\"DCM BASE RESERVOIR FLUID LEVEL SWITCH FAULT\" />\r\n"
"  </Messages>\r\n"
"</Machine_Setup>\r\n";

//char Msg1[] = "POST /init.aspx HTTP/1.0\r\n";
char Msg1[] = "POST /cgi-bin/hello.py HTTP/1.1\r\n";
char Msg2[] = "Content-type: text/plain\r\n";
char Msg3[1024];

/***********************************************************************
*                                MYSEND                                *
***********************************************************************/
void mysend( SOCKET & s, char * sorc )
{
DWORD len;
len = lstrlen( sorc );
cprintf( "%s", sorc );
send( s, sorc, len, 0 );
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
sockaddr_in a;
WSADATA     wsadata;
int         status;
SOCKET      s;

if ( WSAStartup( SocketVersion, &wsadata) )
    {
    cprintf( "Unable to start sockets\n\r" );
    return 0;
    }

a.sin_family           = AF_INET;
a.sin_port             = htons( TcpIpPort );
//a.sin_addr.S_un.S_addr = inet_addr( "192.168.1.1" );
a.sin_addr.S_un.S_addr = inet_addr( "192.168.254.33" );

cprintf( "\n\rCreate socket...\n\r" );
s = socket( AF_INET, SOCK_STREAM, 0 );
if ( s != INVALID_SOCKET )
    {
    cprintf( "Connecting...\n\r" );
    if ( !connect(s, (sockaddr *) &a, sizeof(a)) )
        {
        mysend( s, Msg1 );
        mysend( s, Msg2 );

        status = lstrlen( Data );
        sprintf( Msg3, "Content-Length: %i\r\n\r\n", status );

        mysend( s, Msg3 );
        mysend( s, Data );

        cprintf( "Waiting for answer...\n\r" );
        status = recv( s, RecvBuf, RECV_BUF_LEN, 0 );
        if ( status != 0 && status != SOCKET_ERROR )
            {
            RecvBuf[status] = NullChar;
            }

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

