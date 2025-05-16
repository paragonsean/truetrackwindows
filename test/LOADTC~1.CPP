/***********************************************************************
*                 LOAD_LISTBOX_WITH_TCPIP_ADDRESS_LIST                 *
***********************************************************************/
void load_listbox_with_tcpip_address_list( HWND w, INT id )
{
LISTBOX_CLASS lb;

sockaddr_in a;
SOCKET      s;
int         i;
int         status;
HOSTENT   * hostname;
in_addr     address;
in_addr   * ap;

const int   BUFLEN = 256;
char        buf[BUFLEN];

lb.init( w, id );
s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
if ( s != INVALID_SOCKET )
    {
    a.sin_family      = PF_INET;
    a.sin_port        = htons( TcpIpPort );
    a.sin_addr.s_addr = htonl( INADDR_ANY );

    status = bind( s, (sockaddr *) &a, sizeof(a) );
    if ( status != SOCKET_ERROR )
        {
        status = gethostname( buf, BUFLEN );
        if ( status != SOCKET_ERROR )
            {
            hostname = gethostbyname( buf );
            address = *(in_addr *)hostname->h_addr;

            i = 0;
            while ( true )
                {
                ap = (in_addr *)hostname->h_addr_list[i];
                if ( !ap )
                    break;
                address = *ap;
                lb.add( inet_ntoa(address) );
                i++;
                }
            }
        }
    closesocket( s );
    }
}
