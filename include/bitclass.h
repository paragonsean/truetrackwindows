#ifndef _BIT_CLASS_
#define _BIT_CLASS_

class BIT_CLASS
    {
    private:

    unsigned int * ip;
    int32          nof_ints;
    int32          nof_bits;

    void cleanup( void );

    public:

    BIT_CLASS();
    BIT_CLASS( int32 max_bits );
    ~BIT_CLASS();
    int32 count( void ) { return nof_bits; } 
    void  clear( int32 bit_to_clear );
    void  clearall( void );
    bool  init( int32 max_bits, unsigned int * sorc );
    bool  init( int32 max_bits ) { return init(max_bits, 0); }
    void  set( int32 bit_to_set );
    bool  operator[]( int32 bit_to_check );
    bool  restore( int bytesize, unsigned int * sorc );
    int   save( unsigned int ** dest );
    };

#endif

