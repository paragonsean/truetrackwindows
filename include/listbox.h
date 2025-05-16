#ifndef _LISTBOX_CLASS_
#define _LISTBOX_CLASS_

class LISTBOX_CLASS
    {
    private:
    HWND    w;
    BOOLEAN is_extendedsel;
    int     c;       /* Command index */
    int     buflen;
    TCHAR * buf;
    BOOLEAN check_buf_size( int slen );
    BOOLEAN check_buf_size( TCHAR * s );
    BOOLEAN check_item_size( INT i );
    bool    convert_tab_array_to_dialog_units( INT * tabs, int n );

    public:
    LISTBOX_CLASS( void ) { w = 0; c = 0; buf = 0; buflen = 0; is_extendedsel = FALSE; }
    ~LISTBOX_CLASS( void );
    INT     add( TCHAR * s );
    INT     count( void );
    INT     current_index( void );
    void    empty( void );
    BOOLEAN findprefix( TCHAR * s );
    int     get_item_data( INT item_index );
    int     get_item_data();
    INT     get_select_list( INT ** dest );
    HWND    handle( void ) { return w; }
    BOOLEAN init( HWND hWnd );
    BOOLEAN init( HWND dialog_box, INT id );
    INT     insert( INT i, TCHAR * s );
    TCHAR * item_text( INT i );
    BOOLEAN is_selected( INT item_index );
    void    redraw_off( void );
    void    redraw_on( void );
    void    refresh();
    BOOLEAN remove( INT x );
    BOOLEAN replace( TCHAR * s );
    void    select_all( WPARAM is_set );
    void    select_all( void ) { select_all( TRUE ); }
    INT     select_count( void );
    INT     selected_index( void );
    TCHAR * selected_text( void );
    BOOLEAN setcursel( INT i );
    BOOLEAN setcursel( TCHAR * s );
    BOOLEAN set_current_index( INT new_index );
    bool    set_item_data( INT item_index, int value );
    bool    set_item_data( int value );
    void    set_select( INT item_index, BOOLEAN is_selected );
    bool    set_pixel_tabs( INT * tabs, int n );
    bool    set_tabs( INT first_static_control, INT last_static_control );
    bool    set_tabs( HWND header_control_handle );
    BOOLEAN set_top_index( INT new_index  );
    INT     top_index( void );
    void    unselect_all( void ) { select_all( FALSE ); }
    INT     visible_row_count( void );
    };

#endif
