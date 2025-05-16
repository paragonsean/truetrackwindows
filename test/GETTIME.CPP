#include <windows.h>
#include <iostream.h>
#include <time.h>
#include <winsock.h>

int main( void )
{

time_t t;
SOCKET      s;
sockaddr_in a;
hostent   * h;
WSADATA     wsaData;

if ( WSAStartup(0X101, &wsaData) )
    {
    cout << "Unable to initialize WinSock library.\n";
    return 0;
    }

h = gethostbyname( "http://www.be.com" );
if ( h == NULL )
    {
    cout << "Cannot resolve host name.\n";
    WSACleanup();
    return 0;
    }

a.sin_family = AF_INET;
a.sin_port   = htons( 37 );
memcpy( &(a.sys_addr.s_addr), h->h_addr, sizeof(int) );

s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
if ( s == INVALID_SOCKET )
    {
    cout << "Cannot create socket: " << WSAGetLastError() << '\n';
    WSACleanup();
    return 0;
    }

if ( connect(s, (sockaddr *) &a, sizeof(a)) )
    {
    cout << "Cannot establish connection: " << WSAGetLastError() << '\n';
    WSACleanup();
    return 0;
    }

if ( recv(s, (char *)&t, 4, 0) != 4 )
    {
    cout << "Unable to optain time.\n"
    }
else
    {
    t = ntohl(t) - 2208988800L;
    cout << asctime(localtime(&t));
    }

closesocket(s);
WSACleanup();

}
