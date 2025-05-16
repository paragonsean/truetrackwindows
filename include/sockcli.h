#ifndef _SOCKET_CLIENT_CLASS_
#define _SOCKET_CLIENT_CLASS_

class SOCKET_CLIENT_CLASS;

struct SOCKET_ENTRY
    {
    SOCKET_ENTRY * next;
    SOCKET sorc_socket;
    WORD   type;
    byte * bp;
    int    len;

    SOCKET_ENTRY() { next = 0; type = 0; bp = 0; len = 0; }
    ~SOCKET_ENTRY() { if ( bp ) {delete[] bp; bp = 0;} }
    };

SOCKET_ENTRY * new_socket_entry( int buflen );

class SOCKET_ENTRY_QUEUE
    {
    private:

    CRITICAL_SECTION my_critical_section;
    SOCKET_ENTRY * first;

    public:

    void           clear( void );
    void           push( SOCKET_ENTRY * sorc );
    SOCKET_ENTRY * pop( void );

    SOCKET_ENTRY_QUEUE();
    ~SOCKET_ENTRY_QUEUE();
    };

class CLIENT_SOCKET_CLASS
    {
    private:
    static bool wsa_is_started;

    bool      read_thread_is_running;
    bool      send_thread_is_running;
    bool      i_am_closing;

    int       max_socket_buf_len;
    HANDLE    send_event;
    byte    * sendbuf;
    int       sendbuflen;
    SOCKET_ENTRY_QUEUE sendlist;
 
    void (*recv_callback)( SOCKET_ENTRY * se );
    void (*close_complete_callback)( void );

    public:

    SOCKET    my_socket;

    CLIENT_SOCKET_CLASS();
   ~CLIENT_SOCKET_CLASS();
    DWORD   read_thread( void );
    DWORD   send_thread( void );
    bool    operator==( SOCKET s ) { return my_socket == s; }
    bool open( void (*new_recv_callback)(SOCKET_ENTRY * se), char * ascii_tcp_ip_address, u_short port );
    bool close( void (*close_complete_callback)(void) );
    bool send( SOCKET_ENTRY * se );
    bool send( WORD message_type, void * buf, DWORD bytes_to_send );
    bool is_running( void ) { if (read_thread_is_running && send_thread_is_running) return true; else return false; }
    void startup_windows_sockets( void );
    void cleanup_windows_sockets( void );
    };

#endif
