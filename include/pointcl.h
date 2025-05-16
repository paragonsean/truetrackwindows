#ifndef _POINT_ARRAY_CLASS_
#define _POINT_ARRAY_CLASS_

class POINT_ARRAY_CLASS
    {
    private:

    POINT * p;
    int     n;
    int     i;

    public:

    POINT_ARRAY_CLASS() { p=0; n=0; i=-1; }
    POINT_ARRAY_CLASS( const POINT_ARRAY_CLASS & sorc );
    ~POINT_ARRAY_CLASS();
    int  count( void ) { return n; }
    int  index( void ) { return i; }
    void rewind( void ) { i=-1; }
    void cleanup( void );
    bool next( void );
    bool set( long new_x, long new_y );
    POINT * pointer( void ) { return p; }
    POINT * release_array( void );
    bool init( int new_n );
    POINT & operator[]( int i );
    POINT_ARRAY_CLASS & operator=( const POINT_ARRAY_CLASS & sorc );
    POINT & value( void ) { return operator[](i); }
    };

#endif
