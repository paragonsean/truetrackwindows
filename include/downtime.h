#ifndef _DOWNTIME_ENTRY_

#define _DOWNTIME_ENTRY_

struct DOWNTIME_ENTRY
    {
    TIME_CLASS time;
    TCHAR cat[DOWNCAT_NUMBER_LEN+1];
    TCHAR subcat[DOWNCAT_NUMBER_LEN+1];
    TCHAR worker[OPERATOR_NUMBER_LEN+1];
    TCHAR part[PART_NAME_LEN+1];

    DOWNTIME_ENTRY();
    BOOLEAN operator==( const DOWNTIME_ENTRY & sorc );
    BOOLEAN has_same_reason_as( const DOWNTIME_ENTRY & sorc );
    BOOLEAN has_same_cat_as( const DOWNTIME_ENTRY & sorc );
    BOOLEAN has_same_subcat_as( const DOWNTIME_ENTRY & sorc );
    BOOLEAN has_same_worker_as( const DOWNTIME_ENTRY & sorc );
    BOOLEAN has_same_part_as( const DOWNTIME_ENTRY & sorc );

    void    operator=( const DOWNTIME_ENTRY & sorc );

    BOOLEAN get( DB_TABLE & db );
    BOOLEAN put( DB_TABLE & db );
    };

#endif
