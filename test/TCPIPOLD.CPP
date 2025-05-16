#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

const WORD    SocketVersion = MAKEWORD(2,0);
const u_short TcpIpPort     = 40322;
char  TcpIpAddress[16]      = "192.168.0.2";

const int      BUF_LEN = 30000;
char           Buf[BUF_LEN];
const char     NullChar = '\0';

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

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
WSADATA     wsadata;
sockaddr_in a;
SOCKET      s;
int         i;
int         len;
int         x;
int         status;
HOSTENT   * hostname;
in_addr     address;
in_addr   * ap;
char      * cp;

if ( !WSAStartup(SocketVersion, &wsadata) )
    {
    s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( s != INVALID_SOCKET )
        {
        a.sin_family      = PF_INET;
        a.sin_port        = htons( TcpIpPort );
        a.sin_addr.s_addr = htonl( INADDR_ANY );

        status = bind( s, (sockaddr *) &a, sizeof(a) );
        if ( status != SOCKET_ERROR )
            {
            status = gethostname( Buf, 256 );
            if ( status != SOCKET_ERROR )
                {
                hostname = gethostbyname( Buf );
                cprintf( "\n\rHost name =[%s]\n\r", hostname->h_name );

                address = *(in_addr *)hostname->h_addr;
                cprintf( "\n\r[ %s ] [%d]\n\r", inet_ntoa(address), (int) hostname->h_length );

                i = 0;
                while ( true )
                    {
                    cp = hostname->h_aliases[i];
                    if ( !cp )
                        break;
                    cprintf( "\n\ralias[%d]=[%s]\n\r", i, cp );
                    i++;
                    }
                if ( i == 0 )
                    cprintf( "\n\rThere are no aliases" );

                i = 0;
                while ( true )
                    {
                    ap = (in_addr *)hostname->h_addr_list[i];
                    if ( !ap )
                        break;
                    address = *ap;
                    cprintf( "\n\raddress[%d] = [ %s ]\n\r", i, inet_ntoa(address) );
                    i++;
                    }

                len = sizeof( x );
                if ( getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&x, &len) == 0 )
                    {
                    cprintf( "\n\rSO_SNDBUF size=[%d] value = [ %d ]\n\r", len, x );
                    }

                if ( getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&x, &len) == 0 )
                    {
                    cprintf( "\n\rSO_RCVBUF size=[%d] value = [ %d ]\n\r", len, x );
                    }

                x = 131072;
                setsockopt( s, SOL_SOCKET, SO_RCVBUF, (char *)&x, len );

                len = sizeof( x );
                if ( getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&x, &len) == 0 )
                    {
                    cprintf( "\n\rSO_RCVBUF size=[%d] value = [ %d ]\n\r", len, x );
                    }


                }

            }
        closesocket( s );
        }

    WSACleanup();
    }

cprintf( "\n\rDone, press <Enter> to exit..." );
getline();
cprintf( "\n\r" );

return 0;
}

