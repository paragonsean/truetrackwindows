#ifndef _WINDOW_LIST_CLASS_
#define _WINDOW_LIST_CLASS_

class WINDOW_LIST_CLASS
    {
    public:
    HWND * list;
    int    n;
    int    max_n;
    int    x;              /* Current Index */
    WINDOW_LIST_CLASS() { list=0; n=0; max_n=0; x=-1; }
    ~WINDOW_LIST_CLASS();
    int   count() { return n; };
    int   current_index() { return x; }
    bool  contains( HWND w );
    void  destroy();
    void  empty();
    void  enable( BOOL is_enabled );
    bool  find( HWND w );
    int   last_index() { return n-1; }
    bool  setsize( int new_max_n );
    bool  copy( const WINDOW_LIST_CLASS & sorc );
    HWND & operator[]( int i );
    bool  append( const WINDOW_LIST_CLASS & sorc );
    bool  append( HWND w );
    HWND  handle();
    void  hide();
    HWND  next();
    void  offset( POINT p );
    bool  remove( HWND dead_window );
    bool  remove( const WINDOW_LIST_CLASS & sorc );
    void  remove_all();
    void  refresh();
    void  rewind() { x = -1; }
    void  show();
    void  show( bool visible );
    };

#endif
