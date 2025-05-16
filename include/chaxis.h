#ifndef _CHAXIS_LABEL_CLASS_
#define _CHAXIS_LABEL_CLASS_

class CHAXIS_LABEL_CLASS
    {
    private:
    TCHAR ** label;
    int      n;

    public:
    CHAXIS_LABEL_CLASS() { label=0; n=0; }
    ~CHAXIS_LABEL_CLASS();
    BOOLEAN init( int max_n );
    void   cleanup( void );
    int    count( void ) { return n; }
    TCHAR * operator[]( int i );
    BOOLEAN set( int i, TCHAR * sorc );
    BOOLEAN get( TCHAR * cn, TCHAR * mn, TCHAR * pn );
    BOOLEAN put( TCHAR * cn, TCHAR * mn, TCHAR * pn );
    };

#endif
