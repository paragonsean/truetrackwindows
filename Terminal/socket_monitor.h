#ifndef _SOCKET_MONITOR_
#define _SOCKET_MONITOR_

const int MAX_ASCII_LEN = 128;

class SOCKET_MONITOR : public SOCKET_CLASS
    {
    private:

    char   asciibuf[MAX_ASCII_LEN+1];
    int    current_type;
    int    current_len;  /* Binary data only */
    int    chars_needed; /* Binary data only */
    char * bp;
    char * binarybuf;
    bool   skipping;
    int    skip_count;

    WINDOW_CLASS destwin;
    WINDOW_CLASS stopwin;
    HANDLE       myevent;
    bool         shutting_down;
    bool         monitor_is_running;

    public:

    SOCKET_MONITOR();
    ~SOCKET_MONITOR();
    bool  begin_monitoring( WINDOW_CLASS & notification_window  );
    DWORD board_monitor_thread();
    void  close_socket( void (*new_close_complete_callback)() );
    void  do_ascii_message();
    bool  is_monitoring() { return monitor_is_running; }
    int   extract_nof_bytes_from_line();
    bool  open_socket( char * ascii_tcp_ip_address, u_short port );
    void  post_data();
    void  stop_monitoring();
    void  stop_monitoring( WINDOW_CLASS & win_to_close ) { stopwin = win_to_close; stop_monitoring(); }
    };

#endif
