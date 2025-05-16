#ifndef _SETPOINT_
#define _SETPOINT_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif


struct SETPOINT_ENTRY
    {
    STRING_CLASS value;
    STRING_CLASS desc;
    };

class SETPOINT_CLASS
    {
    private:

    SETPOINT_ENTRY * array;
    int            n;

    public:

    SETPOINT_CLASS() { array=0; n=0; }
    ~SETPOINT_CLASS();

    SETPOINT_ENTRY & operator[]( int i );
    int     count() { return n; }
    void    cleanup();
    void    empty();
    BOOLEAN setsize( int new_n );

    int     get( TCHAR * path );
    BOOLEAN put( TCHAR * path );
    };

#endif