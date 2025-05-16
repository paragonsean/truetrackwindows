#ifndef _DOWNTIME_BASE_CLASS_H_
#define _DOWNTIME_BASE_CLASS_H_

class DOWNTIME_BASE_CLASS
    {
    public:
    TCHAR cat[DOWNCAT_NUMBER_LEN+1];
    TCHAR subcat[DOWNCAT_NUMBER_LEN+1];
    COLORREF color;

    DOWNTIME_BASE_CLASS();
    DOWNTIME_BASE_CLASS( TCHAR * new_cat, TCHAR * new_subcat );
    void    operator=( const DOWNTIME_BASE_CLASS & sorc );
    BOOLEAN operator==(const DOWNTIME_BASE_CLASS & sorc );
    BOOLEAN operator!=(const DOWNTIME_BASE_CLASS & sorc );
    };

class DOWNTIME_RECORD_CLASS : public DOWNTIME_BASE_CLASS
    {
    public:

    TIME_CLASS tm;

    BOOLEAN get( DB_TABLE & t );
    void    operator=( const DOWNTIME_RECORD_CLASS & sorc );
    void    operator=( const DOWNTIME_BASE_CLASS & sorc ) { DOWNTIME_BASE_CLASS::operator=( sorc ); }
    };

#endif
