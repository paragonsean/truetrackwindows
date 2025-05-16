
class VIRTUAL_LISTBOX_CLASS
    {
    private:

    int   nof_lines;
    int   top_line;
    int   current_line;
    int   lines_per_page;

    DB_TABLE t;
    LISTBOX_CLASS lb;
    HWND          sbw;

    TCHAR * (*lbline)( DB_TABLE & t );

    public:

    VIRTUAL_LISTBOX_CLASS( void );
    int32   count( void ) { return nof_lines; }
    BOOLEAN startup( TCHAR * fname, short reclen, HWND parent, UINT listbox_id, UINT scrollbar_id, TCHAR * (*lbline_func)( DB_TABLE & t ) );
    void    shutdown( void );
    void    do_select( void );
    void    do_vscroll( WPARAM wParam );
    int     do_keypress( int id, int cmd );
    void    fill_listbox( void );
    void    goto_end( void );
    void    goto_start( void );
    void    set_scrollbar( void );
    void    set_scrollbar_pos( void );
    void    scroll_down_one_line( void );
    void    scroll_down_one_page( void );
    void    scroll_up_one_line( void );
    void    scroll_up_one_page( void );
    };
 
