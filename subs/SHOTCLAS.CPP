#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"
#include "..\include\names.h"
#include "..\include\timeclas.h"
#include "..\include\shotclas.h"
#include "..\include\subs.h"

/***********************************************************************
*                             SHOT_CLASS                               *
*                                  =                                   *
***********************************************************************/
void  SHOT_CLASS::operator=( const SHOT_CLASS & sorc )
{
time   = sorc.time;
number = sorc.number;
}

/***********************************************************************
*                             SHOT_CLASS                               *
*                             GET_CURRENT                              *
***********************************************************************/
BOOLEAN SHOT_CLASS::get_current( PART_NAME_ENTRY & p )
{
DB_TABLE   t;
TCHAR    * tn;
SYSTEMTIME mytime;
BOOLEAN    status;

status      = FALSE;
init_systemtime_struct( mytime );

tn = plookup_dbname( p.computer, p.machine );
if ( file_exists(tn) )
    {
    t.open( tn, PLOOKUP_RECLEN, PFL );
    t.put_alpha( PLOOKUP_PART_NAME_OFFSET, p.part, PART_NAME_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        {
        number = t.get_long( PLOOKUP_LAST_SHOT_OFFSET );
        t.get_date( mytime, PLOOKUP_DATE_OFFSET );
        t.get_time( mytime, PLOOKUP_TIME_OFFSET );
        time.set( mytime );
        status = TRUE;
        }
    t.close();
    }

return status;
}

/***********************************************************************
*                              SHOT_CLASS                              *
*                           GET_FROM_GRAPHLST                          *
***********************************************************************/
BOOLEAN SHOT_CLASS::get_from_graphlst( DB_TABLE & t )
{
SYSTEMTIME mytime;
BOOLEAN    status;

status = FALSE;
init_systemtime_struct( mytime );

number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );

if ( t.get_date(mytime, GRAPHLST_DATE_OFFSET) )
    {
    if ( t.get_time( mytime, GRAPHLST_TIME_OFFSET) )
        {
        time.set( mytime );
        status = TRUE;
        }
    }

return status;
}

/***********************************************************************
*                              SHOT_CLASS                              *
*                           GET_FROM_SHOTPARM                          *
***********************************************************************/
BOOLEAN SHOT_CLASS::get_from_shotparm( DB_TABLE & t )
{
SYSTEMTIME mytime;
BOOLEAN    status;

status = FALSE;
init_systemtime_struct( mytime );

number = t.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );

if ( t.get_date(mytime, SHOTPARM_DATE_OFFSET) )
    {
    if ( t.get_time( mytime, SHOTPARM_TIME_OFFSET) )
        {
        time.set( mytime );
        status = TRUE;
        }
    }

return status;
}

