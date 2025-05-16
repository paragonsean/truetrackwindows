#ifndef _STRING_ARRAY_CLASS_
#define _STRING_ARRAY_CLASS_

#ifndef _VISIPARM_H
typedef long int int32;
#endif

#ifndef _SUBS_H_
int32   asctoint32( const TCHAR * sorc );
float   extfloat( const TCHAR * sorc );
double  extdouble( const TCHAR * sorc );
TCHAR * int32toasc( int32 n );
void    matchcase( TCHAR * dest, const TCHAR * sorc );
TCHAR * ultoascii( unsigned long n );
#endif

class STRING_ENTRY
    {
    public:

    STRING_ENTRY * next;
    int            maxlen;
    TCHAR        * s;
    STRING_ENTRY() { next = 0; s = 0; maxlen = 0; }
    ~STRING_ENTRY();
    void empty( void );
    };

class STRING_ARRAY_CLASS
    {
    private:

    STRING_ENTRY * first;
    STRING_ENTRY * current;
    int            current_index;
    int            n;
    bool           rewound;
    static TCHAR emptystring[];

    public:

    STRING_ARRAY_CLASS();
    STRING_ARRAY_CLASS( const TCHAR * sorc );
    STRING_ARRAY_CLASS( const STRING_ARRAY_CLASS & sorc );
    ~STRING_ARRAY_CLASS();

    bool add( void );
    bool add( const TCHAR * sorc );
    bool add( const STRING_ARRAY_CLASS & sorc );

    void cat_w_char( const TCHAR * sorc, TCHAR c );
    void cat_w_char( const STRING_ARRAY_CLASS & sorc, TCHAR c );

    void cat_path( const TCHAR * sorc );
    void cat_path( const STRING_ARRAY_CLASS & sorc );

    bool insert( const TCHAR * sorc );
    bool insert( const STRING_ARRAY_CLASS & sorc );

    bool find( TCHAR * sorc );
    bool find( int index_to_find );

    void empty( void );
    bool init( int new_len );
    bool isempty();
    bool is_numeric();
    bool iszero();
    bool contains( TCHAR c );
    bool contains( const TCHAR * s_to_find );
    bool contains( const STRING_ARRAY_CLASS & sorc ) { return contains(sorc.current->s); }
    int  len();
    void lowercase();
    void matchcase( const TCHAR * sorc ) { if ( current->s && sorc ) ::matchcase( current->s, sorc ); }
    bool next( void );
    bool prev( void );
    int  count( void ) { return n; }
    bool remove( void );
    void remove_all();
    void rewind( void ) { current = first; current_index = 0; rewound = true; }
    bool split( const TCHAR * separator );
    bool join( const TCHAR * separator );
    bool join( void ) { return join(0); }
    void reverse();
    void strip( TCHAR * charlist );
    void trim(  TCHAR * charlist);

    int     int_value( void ) { if (current->s) return (int) asctoint32(current->s); else return 0; }
    float   float_value( void ) { if (current->s) return extfloat(current->s); else return 0.0; }
    double  real_value( void ) { if (current->s) return extdouble(current->s); else return 0.0; }

    STRING_ARRAY_CLASS & operator=( const TCHAR * sorc );
    STRING_ARRAY_CLASS & operator=( const STRING_ARRAY_CLASS & sorc );

    void operator+=( const TCHAR * sorc );
    void operator+=( const STRING_ARRAY_CLASS & sorc );
    void operator+=( int sorc ) { operator+=(int32toasc(sorc)); }
    void operator+=( unsigned int sorc ) { operator+=(ultoascii((unsigned long)sorc)); }

    bool operator==(const STRING_ARRAY_CLASS & sorc );
    bool operator==(const TCHAR * sorc );

    STRING_ARRAY_CLASS & operator[]( int i );

    TCHAR * text( void ) { if (current->s) return current->s; else return emptystring; }
    void uppercase();

    #ifndef _CONSOLE
    bool gettext( HWND w );
    bool gettext( HWND w, int id ) { return STRING_ARRAY_CLASS::gettext( GetDlgItem(w, id) ); }
    bool settext( HWND w );
    bool settext( HWND w, int id ) { return STRING_ARRAY_CLASS::settext( GetDlgItem(w, id) ); }
    #endif
    };

#endif
