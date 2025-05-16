#ifndef _PROFILE_MERGE_CLASS_
#define _PROFILE_MERGE_CLASS_

#ifndef _DB_CLASS_
#include "..\include\dbclass.h"
#endif

#ifndef _SHOT_CLASS_
#include "..\include\shotclas.h"
#endif

class PROFILE_MERGE_CLASS
    {
    private:

    TCHAR    * dir;
    SHOT_CLASS current_shot;
    BOOLEAN    have_current_shot;

    DB_TABLE t;

    public:

    SHOT_CLASS min;
    SHOT_CLASS max;

    PROFILE_MERGE_CLASS();
    ~PROFILE_MERGE_CLASS();
    void    close( void ) { t.close(); }
    BOOLEAN have_shot( void ) { return have_current_shot; }
    BOOLEAN next( void );
    BOOLEAN get( PROFILE_MERGE_CLASS & sorc, int32 new_shot_number );
    BOOLEAN open_graphlst( TCHAR * directory_name, short table_lock_type );
    void    rewind( void );
    SHOT_CLASS & shot( void ) { return current_shot; }
    BOOLEAN get_min_and_max( void );
    BOOLEAN operator<( const PROFILE_MERGE_CLASS & sorc );
    BOOLEAN operator<( const TIME_CLASS & sorc );
    BOOLEAN operator==( const PROFILE_MERGE_CLASS & sorc );
    BOOLEAN operator==( const TIME_CLASS & sorc );
    };

class SHOTPARM_MERGE_CLASS
    {
    private:

    TCHAR    * dir;
    SHOT_CLASS current_shot;
    BOOLEAN    have_current_shot;

    DB_TABLE t;

    public:

    SHOT_CLASS min;
    SHOT_CLASS max;

    SHOTPARM_MERGE_CLASS();
    ~SHOTPARM_MERGE_CLASS();
    void    close( void ) { t.close(); }
    BOOLEAN have_shot( void ) { return have_current_shot; }
    BOOLEAN next( void );
    BOOLEAN get( SHOTPARM_MERGE_CLASS & sorc, int32 new_shot_number );
    BOOLEAN open_shotparm( TCHAR * directory_name, short table_lock_type );
    void    rewind( void );
    SHOT_CLASS & shot( void ) { return current_shot; }
    BOOLEAN get_min_and_max( void );
    BOOLEAN operator<( const SHOTPARM_MERGE_CLASS & sorc );
    BOOLEAN operator<( const TIME_CLASS & sorc );
    BOOLEAN operator==( const SHOTPARM_MERGE_CLASS & sorc );
    BOOLEAN operator==( const TIME_CLASS & sorc );
    };

class DOWNTIME_MERGE_CLASS
    {
    private:

    DOWNTIME_ENTRY d;
    BOOLEAN        have_current_downtime;

    DB_TABLE t;

    public:

    DOWNTIME_MERGE_CLASS();
    ~DOWNTIME_MERGE_CLASS();
    void    close( void ) { t.close(); }
    BOOLEAN have_downtime( void ) { return have_current_downtime; }
    BOOLEAN next( void );
    BOOLEAN add( const DOWNTIME_MERGE_CLASS & sorc );
    BOOLEAN open( TCHAR * directory_name, short table_lock_type );
    void    rewind( void );
    TIME_CLASS & time( void ) { return d.time; }
    BOOLEAN operator<( const DOWNTIME_MERGE_CLASS & sorc );
    BOOLEAN operator<( const TIME_CLASS & sorc );
    BOOLEAN operator==( const DOWNTIME_MERGE_CLASS & sorc );
    BOOLEAN operator==( const TIME_CLASS & sorc );
    };

#endif

