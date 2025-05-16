#ifndef _SHIFT_CLASS_
#define _SHIFT_CLASS_

class SHIFT_CLASS
    {
    public:

    TIME_CLASS time;
    int32      number;

    SHIFT_CLASS();
    ~SHIFT_CLASS();
    BOOLEAN read( DB_TABLE & t, FIELDOFFSET date_offset, FIELDOFFSET shift_offset );
    BOOLEAN set_start_of_shift( const SYSTEMTIME & st, short new_shift_number );
    BOOLEAN operator<( const SHIFT_CLASS & sorc ) { return time < sorc.time; }
    BOOLEAN operator>( const SHIFT_CLASS & sorc ) { return time > sorc.time; }
    BOOLEAN operator==( const SHIFT_CLASS & sorc ){ return time == sorc.time; }
    void    operator=( const SHIFT_CLASS & sorc ) { time = sorc.time; number = sorc.number; }
    };

#endif
