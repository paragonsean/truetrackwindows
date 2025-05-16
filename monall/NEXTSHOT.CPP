#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "extern.h"

/***********************************************************************
*                             NEXTSHOT                                 *
***********************************************************************/
BOOLEAN nextshot( PROFILE_HEADER & ph )
{

DB_TABLE   t;
SYSTEMTIME shot_time;
BOOLEAN    have_rec;
BOOLEAN    need_to_insert = FALSE;
 
FileTimeToSystemTime( &ph.time_of_shot, &shot_time );

/*
--------------------------------------------
If there is no plookup record, start at one.
-------------------------------------------- */
ph.shot_number = 0;

t.open( plookup_dbname(ComputerName,ph.machine_name), PLOOKUP_RECLEN, PWL );
t.put_alpha(PLOOKUP_PART_NAME_OFFSET, ph.part_name, PART_NAME_LEN);
t.reset_search_mode();
have_rec = t.get_next_key_match(1, TRUE);
if ( have_rec )
    ph.shot_number = t.get_long( PLOOKUP_LAST_SHOT_OFFSET );
else
    need_to_insert = t.goto_first_greater_than_record( 1 );
 
ph.shot_number++;
t.put_alpha(PLOOKUP_PART_NAME_OFFSET, ph.part_name, PART_NAME_LEN);
t.put_long( PLOOKUP_LAST_SHOT_OFFSET, ph.shot_number, SHOT_LEN );
t.put_date( PLOOKUP_DATE_OFFSET, shot_time );
t.put_time( PLOOKUP_TIME_OFFSET, shot_time );

if ( have_rec )
    {
    t.rec_update();
    t.unlock_record();
    }
else
    {
    if ( need_to_insert )
        t.rec_insert();
    else
        t.rec_append();
    }

t.close();

if ( t.get_global_error() == VS_SUCCESS )
    return TRUE;

return FALSE;
}
