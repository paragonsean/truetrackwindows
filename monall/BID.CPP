#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\ft.h"
#include "..\include\subs.h"
#include "..\include\verrors.h"

#include "bid.h"

/***********************************************************************
*                    GET_BOARD_INITIALIZATION_DATA                     *
***********************************************************************/
BOOLEAN get_board_initialization_data( BOARD_INITIALIZATION_DATA & dest, TCHAR * computer, TCHAR * machine, TCHAR * part )
{

DB_TABLE t;
BOOLEAN  status;
short    i;
FIELDOFFSET offset;

status = FALSE;
t.open( machset_dbname(computer), MACHSET_RECLEN, PFL );
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, machine, MACHINE_NAME_LEN );
t.reset_search_mode();
if ( t.get_next_key_match(1, NO_LOCK) )
    {
    dest.auto_shot_timeout_seconds = (float) t.get_short( MACHSET_AUTO_SHOT_TIMEOUT_OFFSET );
    status = TRUE;
    }
t.close();

if ( !status )
    {
    error_message( GET_BOARD_INITIALIZATION_DATA, MACHINE_NOT_FOUND, machine );
    return FALSE;
    }

status = FALSE;
t.open( parts_dbname(computer, machine), PARTS_RECLEN, PFL );
t.put_alpha( PARTS_PART_NAME_OFFSET, part, PART_NAME_LEN );
t.reset_search_mode();
if ( t.get_next_key_match(1, NO_LOCK) )
    {
    dest.nof_time_samples          = t.get_short( PARTS_N_OF_TIME_SAMPLES_OFFSET );
    dest.total_stroke_len          = t.get_float( PARTS_TOTAL_STROKE_LEN_OFFSET );
    dest.min_stroke_len            = t.get_float( PARTS_MIN_STROKE_LEN_OFFSET );
    dest.biscuit_time_delay        = t.get_float( PARTS_BISCUIT_TIME_DELAY_OFFSET );
    dest.eos_velocity              = t.get_float( PARTS_EOS_VEL_OFFSET );
    dest.sleeve_fill_distance      = t.get_float( PARTS_SLEEVE_FILL_DIST_OFFSET );
    dest.runner_fill_distance      = t.get_float( PARTS_RUNNER_FILL_DIST_OFFSET );
    dest.min_csfs_vel              = t.get_float( PARTS_MIN_CSFS_VEL_OFFSET );
    dest.rise_csfs_vel             = t.get_float( PARTS_RISE_CSFS_VEL_OFFSET );
    dest.min_csfs_pos              = t.get_float( PARTS_MIN_CSFS_POS_OFFSET );
    offset = PARTS_LIMIT_SW_1_OFFSET;
    for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
        {
        dest.limits[i]             = t.get_float( offset );
        offset += PARTS_DB_FLOAT_LEN + 1;
        }
    dest.ms_per_time_based_sample  = t.get_float( PARTS_MS_PER_TIME_SAMPLE_OFFSET );
    status = TRUE;
    }
t.close();

if ( !status )
    error_message( GET_BOARD_INITIALIZATION_DATA, PART_NOT_FOUND, part );

return status;
}
