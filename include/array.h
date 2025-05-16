#ifndef _ARRAY_CLASS_
#define _ARRAY_CLASS_

class FLOAT_ARRAY_CLASS
    {
    private:

    float * p;
    int32   n;
    int32   x;
    BOOLEAN rewound;

    public:

    FLOAT_ARRAY_CLASS() { p = 0; n = 0; x = 0; rewound = FALSE; }
    ~FLOAT_ARRAY_CLASS();
    void    cleanup( void );
    int32   count( void ) { return n; }
    BOOLEAN rewind_to( int32 new_index );
    int32   index( void ) { return x; }
    virtual BOOLEAN init( int32 nof_floats );
    BOOLEAN next( void );
    void    rewind( void ) { x = 0; rewound = TRUE; }
    virtual void set( float new_value ) { if ( p ) p[x] = new_value; }
    float   value( void ) { if ( p ) return p[x]; else return 0.0; }
    };

class DOUBLE_ARRAY_CLASS
    {
    private:

    double * p;
    int32    n;

    public:

    DOUBLE_ARRAY_CLASS() { p = 0; n = 0; }
    ~DOUBLE_ARRAY_CLASS();
    double & operator[]( int32 i );
    void    cleanup( void );
    int     count() { return (int) n; }
    void    move( DOUBLE_ARRAY_CLASS & sorc );
    virtual BOOLEAN init( int32 nof_floats );
    virtual BOOLEAN init( int32 sorc_n, double * sorc );
    void    operator=( const DOUBLE_ARRAY_CLASS & sorc );
    };

class INT32_ARRAY_CLASS
    {
    private:
    int32 * p;
    int32   nset;            /* Number of elements that have been set */
    int32   n;
    int32   x;
    BOOLEAN rewound;

    public:
    INT32_ARRAY_CLASS() { p = 0; n = 0; nset=0; x = 0; rewound = FALSE; }
    ~INT32_ARRAY_CLASS();

    void    operator=( const INT32_ARRAY_CLASS & sorc );
    BOOLEAN operator==(const INT32_ARRAY_CLASS & sorc );
    BOOLEAN operator!=(const INT32_ARRAY_CLASS & sorc );
    int32 & operator[]( int32 i );

    void    cleanup( void );
    int32   count( void ) { return n; }
    int32   set_count( void ) { return nset; }

    BOOLEAN is_set( void ) { if ( x < nset ) return TRUE; return FALSE; }
    BOOLEAN rewind_to( int32 new_index );
    int32   index( void ) { return x; }
    virtual BOOLEAN init( int32 nof_ints );
    virtual void left_shift( int32 new_value );
    int32   max_value( void );
    int32   min_value( void );
    BOOLEAN next( void );
    BOOLEAN read( TCHAR * fname );
    void    rewind( void ) { x = 0; rewound = TRUE; }
    virtual void set( int32 new_value ) { if ( p ) p[x] = new_value; if ( x >= nset ) nset = x+1; }
    int32   value( void ) { if ( p ) return p[x]; else return 0; }
    BOOLEAN write( TCHAR * fname );
    };

class INT_ARRAY_CLASS
    {
    private:
    int * p;
    int   n;

    public:
    INT_ARRAY_CLASS() { p = 0; n = 0; }
    ~INT_ARRAY_CLASS();
    void   cleanup( void );
    bool   contains( int sorc );
    int    count( void ) { return n; }
    int  & operator[]( int i );
    };

#endif
