class TEXT_LEN_CLASS
    {
    private:

    HDC   dc;
    int32 h;
    int32 w;

    public:

    TEXT_LEN_CLASS() { dc = 0; h = 0; w = 0; }
    void  init( HDC new_dc ) { dc = new_dc; h = 0; w = 0; }
    void  check( TCHAR * s );
    int32 width( void ) { return w; }
    int32 height( void ) { return h; }
    };
