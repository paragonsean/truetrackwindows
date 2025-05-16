#ifndef _WINDOW_CLASS_
#define _WINDOW_CLASS_

class WINDOW_CLASS
    {
    private:

    HWND w;

    public:

    WINDOW_CLASS() { w=0; }
    virtual ~WINDOW_CLASS() {}
    void operator=( const HWND sorc ) { w = sorc; }
    void operator=(  const WINDOW_CLASS & sorc ) { w = sorc.w; }
    bool operator==( const WINDOW_CLASS & sorc ) { return (w == sorc.w); }
    bool operator==( const HWND sorc ) { return (w == sorc); }
    bool operator!=( const WINDOW_CLASS & sorc ) { return (w != sorc.w); }
    bool operator!=( const HWND sorc ) { return (w != sorc); }
    BOOL add_style( LONG style_bits_to_add );
    HWND handle( void ) { return w; }
    HWND control( UINT id ) { if (w) return GetDlgItem(w, id); else return 0; }
    BOOL is_enabled();
    BOOL is_visible( void );
    BOOL is_minimized( void );
    void close( void ) { post( WM_CLOSE ); }
    void destroy( void );
    void enable( bool is_enabled );
    void enable( BOOLEAN is_enabled );
    void enable() { enable( true ); }
    void disable() { enable( false ); }
    void minimize( void ) { if ( w ) CloseWindow( w ); }
    void move( int x, int y, int width, int height, BOOL need_repaint );
    void move( int x, int y, int width, int height ) { move( x, y, width, height, true ); }
    void move( int x, int y, BOOL need_repaint );
    void move( int x, int y ) { move( x, y, TRUE ); }
    void move( RECT & r );
    void offset( long dx, long dy );
    void offset( POINT & p ) { offset( p.x, p.y ); }
    BOOL getrect( RECT & r );
    BOOL get_client_rect( RECT & r ) { if (w) return GetClientRect(w,&r); else return FALSE; }
    HDC  get_dc() { if (w) return GetDC(w); else return 0; }
    BOOL get_move_rect( RECT & r );
    void show( int cmd );
    void show( void ) { show( SW_SHOW ); }
    void hide( void ) { show( SW_HIDE ); }
    void show( bool visible ) { if ( visible ) show( SW_SHOW ); else show( SW_HIDE ); }
    void post( UINT msg, WPARAM wParam, LPARAM lParam );
    void post( UINT msg );
    void refresh( void );
    int  release_dc( HDC dc ) { if (w) return ReleaseDC(w,dc); else return 0; }
    BOOL remove_style( LONG style_bits_to_remove );
    void send( UINT msg, WPARAM wParam, LPARAM lParam );
    void send( UINT msg );
    void set_title( TCHAR * new_title );
    void set_focus( void ) { if ( w ) SetFocus(w); }
    void set_focus( UINT id ) { if ( w ) SetFocus( GetDlgItem(w,id) ); }
    void set_userdata( LONG new_value ) { if ( w ) SetWindowLong( w, GWL_USERDATA, new_value ); }
    void shrinkwrap( HWND sorc );
    LONG userdata();
    LONG style();
    };

class WINDOW_ARRAY_CLASS
    {
    public:
    WINDOW_CLASS * list;
    int    n;
    int    max_n;
    int    x;              /* Current Index */

    WINDOW_ARRAY_CLASS() { list=0; n=0; max_n=0; x=-1; }
    ~WINDOW_ARRAY_CLASS();
    int   count() { return n; }
    bool  contains( HWND w );
    bool  contains( WINDOW_CLASS w ) { return contains(w.handle()); }
    void  empty();
    bool  add( HWND w );
    bool  remove( HWND w );
    WINDOW_CLASS & operator[]( int i );
    HWND  handle() { if ( x >= 0 ) return list[x].handle(); else return 0; }
    bool  next();
    void  remove_all();
    void  rewind() { x = -1; }
    bool  setsize( int new_max_n );
    };

#endif

