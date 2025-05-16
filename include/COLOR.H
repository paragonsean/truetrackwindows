class COLORS_CLASS
    {
    public:

    COLORS_CLASS( void );
    ~COLORS_CLASS( void );

    COLORREF operator[]( short i );
    COLORREF operator[]( TCHAR * name );
    short    count( void );
    short    index( TCHAR * colorname );
    TCHAR *  name( short i );

    BOOLEAN  reload( void );
    };
