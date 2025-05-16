#ifndef _SOCKET_CLASS_
#define _SOCKET_CLASS_

const int RECVBUF_LEN = 32768;

class SOCKET_CLASS
    {
    private:

    static BOOLEAN wsa_is_started;

    BOOLEAN          read_thread_is_running;
    BOOLEAN          send_thread_is_running;
    BOOLEAN          i_am_open;
    BOOLEAN          i_am_closing;              /* Tell send thread to close down */
    int              error;
    CRITICAL_SECTION my_critical_section;
    char             recvbuf[RECVBUF_LEN];
    TEXT_LIST_CLASS  sendlist;
    SOCKET           mysocket;
    HANDLE           send_event;
    HANDLE           have_new_data_event;

    void (*close_complete_callback)();

    void close();
    void clear();

    public:

    char * buf;
    char * lastchar;           /* Last character of buffer */
    char * inptr;              /* The next position that will be written to (maintained by me) */
    char * outptr;             /* The next position that will be read from (maintained by parent) */

    SOCKET_CLASS();
    virtual ~SOCKET_CLASS();
    void    close( void (*new_close_complete_callback)() );
    BOOLEAN operator==( const SOCKET sorc ) { return (sorc == mysocket); }
    BOOLEAN is_running();
    int     last_error() { return error; }
    BOOLEAN open( HANDLE new_data_notify_event, int buflen, char * ascii_tcp_ip_address, u_short port );
    DWORD   read_thread();
    void    send( TCHAR * sorc );
    DWORD   send_thread();
    SOCKET  socket_id() { return mysocket; }
    static void shutdown();
    static void startup();
    };

#endif
