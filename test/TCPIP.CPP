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
int         i;
int         status;
HOSTENT   * hostname;
in_addr     address;
in_addr   * ap;

if ( !WSAStartup(SocketVersion, &wsadata) )
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
            ap = (in_addr *)hostname->h_addr_list[i];
            if ( !ap )
                break;
            address = *ap;
            cprintf( "\n\raddress[%d] = [ %s ]\n\r", i, inet_ntoa(address) );
            i++;
            }

        }

    WSACleanup();
    }

cprintf( "\n\rDone, press <Enter> to exit..." );
getline();
cprintf( "\n\r" );

return 0;
}

