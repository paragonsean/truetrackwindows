#ifndef _SHOT_CLASS_
#define _SHOT_CLASS_

#ifndef _STRUCTS_
#include "..\include\structs.h"
#endif

#ifndef _TIME_CLASS_
#include "..\include\timeclas.h"
#endif

class SHOT_CLASS
    {
    public:
    TIME_CLASS time;
    int32      number;

    SHOT_CLASS() { number = NO_SHOT_NUMBER; }
    ~SHOT_CLASS() {}
    void       clear( void) { number = NO_SHOT_NUMBER; }
    void       operator=( const SHOT_CLASS & sorc );
    BOOLEAN    operator==( const SHOT_CLASS & sorc ) { return (time == sorc.time); }
    BOOLEAN    operator==( const TIME_CLASS & sorc ) { return (time == sorc); }
    BOOLEAN    operator==( const int32 sorc )        { return (number == sorc); }
    BOOLEAN    get_current( PART_NAME_ENTRY & p );
    BOOLEAN    get_from_graphlst( DB_TABLE & t );
    BOOLEAN    get_from_shotparm( DB_TABLE & t );
    };

#endif

