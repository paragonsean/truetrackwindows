#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\timeclas.h"
#include "..\include\downtime.h"
#include "..\include\subs.h"

/***********************************************************************
*                            DOWNTIME_ENTRY                            *
***********************************************************************/
DOWNTIME_ENTRY::DOWNTIME_ENTRY()
{
lstrcpy( cat,    NO_DOWNCAT_NUMBER  );
lstrcpy( subcat, NO_DOWNCAT_NUMBER  );
lstrcpy( worker, NO_OPERATOR_NUMBER );
lstrcpy( part,   NO_PART            );
}

/***********************************************************************
*                            DOWNTIME_ENTRY                            *
*                          HAS_SAME_REASON_AS                          *
***********************************************************************/
BOOLEAN DOWNTIME_ENTRY::has_same_reason_as( const DOWNTIME_ENTRY & sorc )
{
if ( lstrcmp(cat, sorc.cat) != 0 )
    return FALSE;

if ( lstrcmp(subcat, sorc.subcat) != 0 )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            DOWNTIME_ENTRY                            *
*                            HAS_SAME_CAT_AS                           *
***********************************************************************/
BOOLEAN DOWNTIME_ENTRY::has_same_cat_as( const DOWNTIME_ENTRY & sorc )
{
if ( lstrcmp(cat, sorc.cat) != 0 )
    return FALSE;
return TRUE;
}
 
/***********************************************************************
*                            DOWNTIME_ENTRY                            *
*                          HAS_SAME_SUBCAT_AS                          *
***********************************************************************/
BOOLEAN DOWNTIME_ENTRY::has_same_subcat_as( const DOWNTIME_ENTRY & sorc )
{
if ( lstrcmp(subcat, sorc.subcat) != 0 )
    return FALSE;
 
return TRUE;
}
 
/***********************************************************************
*                            DOWNTIME_ENTRY                            *
*                          HAS_SAME_WORKER_AS                          *
***********************************************************************/
BOOLEAN DOWNTIME_ENTRY::has_same_worker_as( const DOWNTIME_ENTRY & sorc )
{
if ( lstrcmp(worker, sorc.worker) != 0 )
    return FALSE;
 
return TRUE;
}

/***********************************************************************
*                            DOWNTIME_ENTRY                            *
*                           HAS_SAME_PART_AS                           *
***********************************************************************/
BOOLEAN DOWNTIME_ENTRY::has_same_part_as( const DOWNTIME_ENTRY & sorc )
{
if ( lstrcmp(part, sorc.part) != 0 )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            DOWNTIME_ENTRY                            *
*                                   =                                  *
***********************************************************************/
void DOWNTIME_ENTRY::operator=( const DOWNTIME_ENTRY & sorc )
{
time = sorc.time;
lstrcpy( cat,    sorc.cat    );
lstrcpy( subcat, sorc.subcat );
lstrcpy( worker, sorc.worker );
lstrcpy( part,   sorc.part   );
}
 
/***********************************************************************
*                            DOWNTIME_ENTRY                            *
*                                  ==                                  *
***********************************************************************/
BOOLEAN DOWNTIME_ENTRY::operator==( const DOWNTIME_ENTRY & sorc )
{
if ( time != sorc.time )
    return FALSE;
 
if ( lstrcmp(cat, sorc.cat) != 0 )
    return FALSE;
 
if ( lstrcmp(subcat, sorc.subcat) != 0 )
    return FALSE;
 
if ( lstrcmp(worker, sorc.worker) != 0 )
    return FALSE;
 
if ( lstrcmp(part, sorc.part) != 0 )
    return FALSE;
 
return TRUE;
}
 
/***********************************************************************
*                            DOWNTIME_ENTRY                            *
*                                  GET                                 *
***********************************************************************/
BOOLEAN DOWNTIME_ENTRY::get( DB_TABLE & db )
{
BOOLEAN status;
 
status = get_time_class_from_db( time, db, DOWNTIME_DATE_OFFSET );
 
if ( status )
    status = db.get_alpha( cat,    DOWNTIME_CAT_OFFSET,       DOWNCAT_NUMBER_LEN );
 
if ( status )
    status = db.get_alpha( subcat, DOWNTIME_SUBCAT_OFFSET,    DOWNCAT_NUMBER_LEN );
 
if ( status )
    status = db.get_alpha( worker, DOWNTIME_OPERATOR_OFFSET,  OPERATOR_NUMBER_LEN );
 
if ( status )
    status = db.get_alpha( part,   DOWNTIME_PART_NAME_OFFSET, PART_NAME_LEN );
 
return status;
}

/***********************************************************************
*                            DOWNTIME_ENTRY                            *
*                                  PUT                                 *
***********************************************************************/
BOOLEAN DOWNTIME_ENTRY::put( DB_TABLE & db )
{
BOOLEAN status;
 
status = db.put_date( DOWNTIME_DATE_OFFSET, time.system_time() );
 
if ( status )
    status = db.put_time( DOWNTIME_TIME_OFFSET, time.system_time() );
 
if ( status )
    status = db.put_alpha( DOWNTIME_CAT_OFFSET, cat, DOWNCAT_NUMBER_LEN );
 
if ( status )
    status = db.put_alpha( DOWNTIME_SUBCAT_OFFSET, subcat, DOWNCAT_NUMBER_LEN );
 
if ( status )
    status = db.put_alpha( DOWNTIME_OPERATOR_OFFSET, worker, OPERATOR_NUMBER_LEN );
 
if ( status )
    status = db.put_alpha( DOWNTIME_PART_NAME_OFFSET, part, PART_NAME_LEN );
 
return status;
}

