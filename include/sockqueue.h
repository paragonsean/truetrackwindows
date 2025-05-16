#ifndef _SOCKET_ENTRY_CLASS_
#define _SOCKET_ENTRY_CLASS_

class SOCKET_ENTRY
    {
    public:

    SOCKET_ENTRY * next;
    SOCKET sorc_socket;
    WORD   type;
    byte * bp;
    int    len;

    SOCKET_ENTRY() { next = 0; type = 0; bp = 0; len = 0; }
    SOCKET_ENTRY( int buflen );
    ~SOCKET_ENTRY() { if ( bp ) {delete[] bp; bp = 0;} }
    };

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

#endif
