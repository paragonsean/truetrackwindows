#ifndef _UP_DOWN_CLASS_
#define _UP_DOWN_CLASS_

class UP_DOWN_CLASS
    {
    private:
    HWND    w;
    HWND    parent;

    public:
    UP_DOWN_CLASS( void ) { w = 0; parent = 0; }
    UP_DOWN_CLASS( HWND dialog_box, INT id );
    ~UP_DOWN_CLASS( void );

    BOOLEAN init( HWND dialog_box, INT id );
    short   getvalue( void );
    BOOLEAN setbuddy( INT buddy_id );
    void    setdecimal( void );
    void    sethex( void );
    BOOLEAN setincrement( short increment );
    void    setrange( short min_value, short max_value );
    short   setvalue( short new_value );
    };

#endif
