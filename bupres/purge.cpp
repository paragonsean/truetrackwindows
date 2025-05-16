#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

static TCHAR TwoChar = TEXT( '2' );

/***********************************************************************
*                                PURGE                                 *
***********************************************************************/
void purge( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )
{
static TCHAR all_profiles[] = TEXT( "*.pr?" );

DB_TABLE t;
int32    i;
int32    shot_number;
int32    nof_profiles;
BOOL     need_plookup_update;
TCHAR  * cp;
NAME_CLASS s;

if ( !(BackupType & WITH_PURGE) )
    return;

need_plookup_update = FALSE;

/*
------------------
Purge the profiles
------------------ */
if ( BackupType & SAVE_LAST_50 )
    {
    s.get_part_results_dir_file_name( computer_name, machine_name, part_name, GRAPHLST_DB );
    if ( t.open( s.text(), GRAPHLST_RECLEN, FL) )
        {
        nof_profiles = t.nof_recs();
        nof_profiles -= int32( HistoryShotCount );

        if ( nof_profiles > 0 )
            {
            for ( i=0; i<nof_profiles; i++ )
                {
                if ( t.get_next_record(NO_LOCK) )
                    {
                    shot_number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
                    s = profile_name( computer_name, machine_name, part_name, shot_number );
                    if ( !s.delete_file() )
                        {
                        cp = s.text();
                        cp += s.len();
                        cp--;
                        *cp = TwoChar;
                        s.delete_file();
                        }
                    }
                }
            }

        t.reset_search_mode();
        t.get_next_record(NO_LOCK);
        t.rec_delete( nof_profiles );

        t.close();
        }
    }
else
    {
    s.get_part_results_dir_file_name( computer_name, machine_name, part_name, 0 );
    empty_directory( s.text(), all_profiles );
    s.get_part_results_dir_file_name( computer_name, machine_name, part_name, GRAPHLST_DB );
    s.delete_file();
    t.create( s.text() );
    need_plookup_update = TRUE;
    }

/*
--------------------
Purge the parameters
-------------------- */
s.get_part_results_dir_file_name( computer_name, machine_name, part_name, SHOTPARM_DB );
if ( BackupType & SAVE_LAST_50 )
    {
    if ( t.open( s.text(), SHOTPARM_RECLEN, FL) )
        {
        nof_profiles = t.nof_recs();
        nof_profiles -= int32( HistoryShotCount );

        if ( nof_profiles > 0 )
            {
            t.get_next_record(NO_LOCK);
            t.rec_delete( nof_profiles );
            }
        t.close();
        }
    }
else
    {
    s.delete_file();
    t.create( s.text() );
    }

if ( BackupType & ALARM_DATA )
    {
    s.get_part_results_dir_file_name( computer_name, machine_name, part_name, ALARMSUM_DB );
    if ( s.file_exists() )
        {
        t.open( s.text(), ALARMSUM_RECLEN, FL );
        t.empty();
        t.close();
        }
    }

/*
--------------
Update plookup
-------------- */
if ( need_plookup_update )
    {
    i = 0;
    if ( NextShotNumber > 0 )
        i = NextShotNumber - 1;
    set_last_shot_number( computer_name, machine_name, part_name, i );
    }
}
