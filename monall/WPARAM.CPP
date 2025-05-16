#pragma warning( disable : 4018 )   // No warning on signed/unsigned compare

#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

static const short WRITE_PARAM_TIMEOUT = 150;   /* About 1/4 second */

#include "extern.h"

/***********************************************************************
*                           WRITE_PARAMETERS                           *
***********************************************************************/
BOOLEAN write_parameters( PROFILE_HEADER & ph, float * parameters, short nof_parameters, BITSETYPE save_flags )
{

int     i;
DB_TABLE t;
FIELDOFFSET fo;
float       value;
SYSTEMTIME shot_time;
DWORD      n;

FileTimeToSystemTime( &ph.time_of_shot, &shot_time );

t.open( shotparm_dbname(ComputerName, ph.machine_name, ph.part_name), SHOTPARM_RECLEN, WL, WRITE_PARAM_TIMEOUT );
t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, ph.shot_number, SHOT_LEN );
t.put_date( SHOTPARM_DATE_OFFSET, shot_time );
t.put_time( SHOTPARM_TIME_OFFSET, shot_time );
t.put_short( SHOTPARM_SAVE_FLAGS_OFFSET, (short) save_flags, SHOTPARM_SAVE_FLAGS_LEN );
fo = SHOTPARM_PARAM_1_OFFSET;
for ( i=0; i<MAX_PARMS; i++ )
    {
    value = 0.0;
    if ( i < nof_parameters )
        value = parameters[i];
    t.put_float( fo, value, SHOTPARM_FLOAT_LEN );
    fo += SHOTPARM_FLOAT_LEN+1;
    }
t.rec_append();

/*
----------------------------------------------------------------------
Check back 51 shots and see if I should delete the record.
Record the flags for write_profile so he doesn't have to look this up.
---------------------------------------------------------------------- */
n = t.nof_recs();
if ( n > (DWORD) HistoryShotCount )
    {
    n -= HistoryShotCount;
    /*
    --------------------------------------------------
    Subtract one more because the last record is n - 1
    -------------------------------------------------- */
    n--;
    t.goto_record_number(n);
    t.get_rec();

    save_flags = (BITSETYPE) t.get_short( SHOTPARM_SAVE_FLAGS_OFFSET );

    if ( !(save_flags & SAVE_PARM_DATA) )
        t.rec_delete();
    }

n = max_parameters_saved( ph.machine_name );
if ( n > 0 && n < t.nof_recs() )
    {
    t.goto_record_number( 0 );
    t.rec_delete();
    }

t.close();

if ( t.get_global_error() == VS_SUCCESS )
    return TRUE;

return FALSE;
}
