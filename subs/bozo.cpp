#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\machine.h"
#include "..\include\verrors.h"

const static TCHAR BackSlashChar = TEXT( '\\' );

/***********************************************************************
*                             CREATE_TABLE                             *
***********************************************************************/
static BOOL create_table( TCHAR * s )
{
DB_TABLE t;
TCHAR filename[MAX_PATH+1];

lstrcpy( filename, s );

if ( !file_exists(filename) )
    return t.create( filename );

return TRUE;
}

/***********************************************************************
*                           INIT_SAVE_COUNTS                           *
***********************************************************************/
void init_save_counts( SAVE_COUNTS * s )
{
s->skip_count   = 0;
s->acquire_count = 0;
s->max_saved    = 0;
s->skip_seconds = 0;
s->flags = ACQUIRE_NONE_BIT;
}

/***********************************************************************
*                         MACHINE_CLASS::INIT                          *
***********************************************************************/
void MACHINE_CLASS::init()
{
lstrcpy( current_part, NO_PART );
lstrcpy( computer,     NO_COMPUTER );
lstrcpy( name,         NO_MACHINE );
lstrcpy( rodpitch,     NO_RODPITCH );
lstrcpy( opnumber,     NO_OPERATOR_NUMBER );
init_save_counts( &profile );
init_save_counts( &params );
type                     = COLD_CHAMBER_TYPE;
ft_board_number          = NO_BOARD_NUMBER;
muxchan                  = NO_MUX_CHANNEL;
suretrak_controlled      = FALSE;
is_ftii                  = FALSE;
monitor_flags            = MA_NO_MONITORING;
autoshot_timeout_seconds = DEF_AUTO_SHOT_TIMEOUT;
cycle_timeout_seconds    = 0;
diff_curve_number        = NO_DIFF_CURVE_NUMBER;
impact_pres_type[PRE_IMPACT_INDEX]  = NO_CURVE_TYPE;
impact_pres_type[POST_IMPACT_INDEX] = NO_CURVE_TYPE;
impact_points[PRE_IMPACT_INDEX]     = 0;
impact_points[POST_IMPACT_INDEX]    = 0;
}

/***********************************************************************
*                     MACHINE_CLASS::MACHINE_CLASS                     *
***********************************************************************/
MACHINE_CLASS::MACHINE_CLASS()
{
init();
}

/***********************************************************************
*                            MACHINE_CLASS                             *
*                               ISEMPTY                                *
***********************************************************************/
BOOLEAN MACHINE_CLASS::isempty()
{
return strings_are_equal( name, NO_MACHINE );
}

/***********************************************************************
*                     MACHINE_CLASS::OPEN_READONLY                     *
***********************************************************************/
BOOLEAN MACHINE_CLASS::open_readonly( DB_TABLE & t, TCHAR * tname )
{
t.open( tname, MACHSET_RECLEN, PFL );
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, name, MACHINE_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    return TRUE;

t.close();
return FALSE;
}

/***********************************************************************
*                            MACHINE_CLASS                             *
*                           READ_MACHSET_DB                            *
***********************************************************************/
BOOLEAN MACHINE_CLASS::read_machset_db( TCHAR * fname )
{
DB_TABLE t;
TCHAR *  cp;
TCHAR *  ftiiname;

if ( open_readonly(t, fname) )
    {
    type             = BITSETYPE( t.get_long(MACHSET_MACHINE_TYPE_OFFSET)  );
    monitor_flags    = BITSETYPE( t.get_long(MACHSET_MONITOR_FLAGS_OFFSET) );

    ft_board_number     = t.get_short(   MACHSET_FT_BOARD_NO_OFFSET );
    muxchan             = t.get_short(   MACHSET_MUX_CHAN_OFFSET );
    suretrak_controlled = t.get_boolean( MACHSET_ST_BOARD_OFFSET );

    autoshot_timeout_seconds = t.get_short( MACHSET_AUTO_SHOT_TIMEOUT_OFFSET );
    cycle_timeout_seconds    = t.get_short( MACHSET_CYCLE_TIMEOUT_OFFSET );

    profile.skip_count   = t.get_long( MACHSET_PROFILE_SKIP_COUNT_OFFSET );
    profile.acquire_count = t.get_long( MACHSET_PROFILE_ACQUIRE_COUNT_OFFSET );
    profile.max_saved    = t.get_long( MACHSET_PROFILE_MAX_SAVED_OFFSET );
    profile.skip_seconds = t.get_long( MACHSET_PROFILE_TIME_SKIP_OFFSET );
    profile.flags        = t.get_long( MACHSET_PROFILE_FLAGS_OFFSET );

    params.skip_count   = t.get_long( MACHSET_PARAM_SKIP_COUNT_OFFSET );
    params.acquire_count = t.get_long( MACHSET_PARAM_ACQUIRE_COUNT_OFFSET );
    params.max_saved    = t.get_long( MACHSET_PARAM_MAX_SAVED_OFFSET );
    params.skip_seconds = t.get_long( MACHSET_PARAM_TIME_SKIP_OFFSET );
    params.flags        = t.get_long( MACHSET_PARAM_FLAGS_OFFSET );

    impact_points[PRE_IMPACT_INDEX]     = t.get_short( MACHSET_POSITION_IMPACT_PTS_OFFSET );
    impact_points[POST_IMPACT_INDEX]    = t.get_short( MACHSET_TIME_IMPACT_PTS_OFFSET );

    impact_pres_type[PRE_IMPACT_INDEX]  = BITSETYPE( t.get_long(MACHSET_PRE_PRES_TYPE_OFFSET) );
    impact_pres_type[POST_IMPACT_INDEX] = BITSETYPE( t.get_long(MACHSET_POST_PRES_TYPE_OFFSET) );

    t.get_alpha( rodpitch,     MACHSET_RODPITCH_OFFSET,      RODPITCH_NAME_LEN );
    t.get_alpha( opnumber,     MACHSET_OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
    t.get_alpha( current_part, MACHSET_CURRENT_PART_OFFSET,  PART_NAME_LEN );

    diff_curve_number = t.get_short( MACHSET_DIFF_CURVE_NUMBER_OFFSET );
    t.close();

    /*
    ------------------------------------------------------------
    If my board is in the ftii table then this is a ftii machine
    ------------------------------------------------------------ */
    is_ftii = FALSE;
    ftiiname = maketext( fname, 10 );
    if ( ftiiname )
        {
        cp = ftiiname + lstrlen( ftiiname );
        while ( cp > ftiiname )
            {
            cp--;
            if ( *cp == BackSlashChar )
                {
                cp++;
                copystring( cp, FTII_BOARDS_DB );
                if ( file_exists(ftiiname) )
                    {
                    if ( t.open(ftiiname, FTII_BOARDS_RECLEN, PFL) )
                        {
                        while ( t.get_next_record(FALSE) )
                            {
                            if ( ft_board_number == t.get_short(FTII_BOARDS_NUMBER_OFFSET) )
                                {
                                is_ftii = TRUE;
                                break;
                                }
                            }
                        t.close();
                        }
                    }
                break;
                }
            }
        delete[] ftiiname;
        }
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         MACHINE_CLASS::FIND                          *
***********************************************************************/
BOOLEAN MACHINE_CLASS::find( TCHAR * computer_to_find, TCHAR * name_to_find )
{
/*
-------------------------------------------
It's possible to call using my own strings.
Don't copy from myself.
------------------------------------------- */
if ( computer != computer_to_find )
    lstrcpy( computer, computer_to_find );

if ( name != name_to_find )
    lstrcpy( name,     name_to_find );

return read_machset_db( machset_dbname(computer) );
}

/***********************************************************************
*                 MACHINE_CLASS::REFRESH_MONITOR_FLAGS                 *
***********************************************************************/
BOOLEAN MACHINE_CLASS::refresh_monitor_flags( void )
{
DB_TABLE t;

if ( open_readonly(t, machset_dbname(computer)) )
    {
    monitor_flags = BITSETYPE( t.get_long(MACHSET_MONITOR_FLAGS_OFFSET) );
    t.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         MACHINE_CLASS::SAVE                          *
***********************************************************************/
BOOLEAN MACHINE_CLASS::save()
{
TCHAR    this_machine[MACHINE_NAME_LEN+1];
BOOLEAN  is_new_machine;
BOOLEAN  need_to_insert;
BOOLEAN  status;
int      compare_result;
DB_TABLE t;

is_new_machine = TRUE;
need_to_insert = FALSE;

t.open( machset_dbname(computer), MACHSET_RECLEN, PWL );
while( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( this_machine, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
    compare_result = compare( name, this_machine, MACHINE_NAME_LEN );
    if ( compare_result == 0 )
        {
        t.lock_record();
        is_new_machine = FALSE;
        break;
        }
    else if ( compare_result < 0 )
        {
        need_to_insert = TRUE;
        break;
        }
    }

t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, name, MACHINE_NAME_LEN );

t.put_long( MACHSET_MACHINE_TYPE_OFFSET,  type,          BITSETYPE_LEN );
t.put_long( MACHSET_MONITOR_FLAGS_OFFSET, monitor_flags, BITSETYPE_LEN );
t.put_short( MACHSET_FT_BOARD_NO_OFFSET, ft_board_number, BOARD_NUMBER_LEN );
t.put_short( MACHSET_MUX_CHAN_OFFSET,    muxchan,         CHANNEL_NUMBER_LEN );
t.put_boolean( MACHSET_ST_BOARD_OFFSET, suretrak_controlled );
t.put_short( MACHSET_AUTO_SHOT_TIMEOUT_OFFSET, autoshot_timeout_seconds, TIMEOUT_SEC_LEN );
t.put_short( MACHSET_CYCLE_TIMEOUT_OFFSET,     cycle_timeout_seconds,    TIMEOUT_SEC_LEN );

t.put_long( MACHSET_PROFILE_SKIP_COUNT_OFFSET,    profile.skip_count,    SKIP_COUNT_LEN );
t.put_long( MACHSET_PROFILE_ACQUIRE_COUNT_OFFSET, profile.acquire_count, ACQUIRE_COUNT_LEN );
t.put_long( MACHSET_PROFILE_MAX_SAVED_OFFSET,     profile.max_saved,     MAX_SAVED_LEN );
t.put_long( MACHSET_PROFILE_TIME_SKIP_OFFSET,     profile.skip_seconds,  SKIP_TIME_LEN );
t.put_long( MACHSET_PROFILE_FLAGS_OFFSET,         profile.flags,         BITSETYPE_LEN );

t.put_long( MACHSET_PARAM_SKIP_COUNT_OFFSET,    params.skip_count,    SKIP_COUNT_LEN );
t.put_long( MACHSET_PARAM_ACQUIRE_COUNT_OFFSET, params.acquire_count, ACQUIRE_COUNT_LEN );
t.put_long( MACHSET_PARAM_MAX_SAVED_OFFSET,     params.max_saved,     MAX_SAVED_LEN );
t.put_long( MACHSET_PARAM_TIME_SKIP_OFFSET,     params.skip_seconds,  SKIP_TIME_LEN );
t.put_long( MACHSET_PARAM_FLAGS_OFFSET,         params.flags,         BITSETYPE_LEN );

t.put_short( MACHSET_POSITION_IMPACT_PTS_OFFSET, impact_points[PRE_IMPACT_INDEX],  IMPACT_POINTS_LEN );
t.put_short( MACHSET_TIME_IMPACT_PTS_OFFSET,     impact_points[POST_IMPACT_INDEX], IMPACT_POINTS_LEN );

t.put_long( MACHSET_PRE_PRES_TYPE_OFFSET,  impact_pres_type[PRE_IMPACT_INDEX],  BITSETYPE_LEN );
t.put_long( MACHSET_POST_PRES_TYPE_OFFSET, impact_pres_type[POST_IMPACT_INDEX], BITSETYPE_LEN );

t.put_alpha( MACHSET_RODPITCH_OFFSET,        rodpitch,     RODPITCH_NAME_LEN );
t.put_alpha( MACHSET_OPERATOR_NUMBER_OFFSET, opnumber,     OPERATOR_NUMBER_LEN );
t.put_alpha( MACHSET_CURRENT_PART_OFFSET,    current_part, PART_NAME_LEN );

t.put_short( MACHSET_DIFF_CURVE_NUMBER_OFFSET, diff_curve_number,  DIFF_CURVE_NUMBER_LEN );

if ( is_new_machine )
    {
    if ( need_to_insert )
        status = t.rec_insert();
    else
        status = t.rec_append();
    }
else
    {
    status = t.rec_update();
    t.unlock_record();
    }

t.close();

if ( is_new_machine )
    {
    /*
    ----------------------------
    Create the machine directory
    ---------------------------- */
    if ( create_directory(codir_name(computer, name)) )
        {
        create_table( parts_dbname(computer, name)   );
        create_table( plookup_dbname(computer, name) );
        create_table( dstat_dbname(computer, name) );
        create_table( madir_name(computer, name, SEMAPHOR_DB) );
        }

    /*
    ------------------------------------
    Create the machine results directory
    ------------------------------------ */
    if ( create_directory(madir_name(computer, name, RESULTS_DIRECTORY_NAME)) )
        {
        create_table( downtime_dbname(computer, name) );
        }
    }

return status;
}

/***********************************************************************
*                             MACHINE_CLASS                            *
*                                  =                                   *
***********************************************************************/
void MACHINE_CLASS::operator=( const MACHINE_CLASS & sorc )
{
copystring( name,         sorc.name         );
copystring( computer,     sorc.computer     );
copystring( current_part, sorc.current_part );
copystring( rodpitch,     sorc.rodpitch     );
copystring( opnumber,     sorc.opnumber     );

type = sorc.type;
ft_board_number = sorc.ft_board_number;
muxchan = sorc.muxchan;
is_ftii = sorc.is_ftii;
suretrak_controlled = sorc.suretrak_controlled;

profile.skip_count    = sorc.profile.skip_count;
profile.acquire_count = sorc.profile.acquire_count;
profile.max_saved     = sorc.profile.max_saved;
profile.skip_seconds  = sorc.profile.skip_seconds;
profile.flags         = sorc.profile.flags;

params.skip_count    = sorc.params.skip_count;
params.acquire_count = sorc.params.acquire_count;
params.max_saved     = sorc.params.max_saved;
params.skip_seconds  = sorc.params.skip_seconds;
params.flags         = sorc.params.flags;

monitor_flags            = sorc.monitor_flags;
autoshot_timeout_seconds = sorc.autoshot_timeout_seconds;
cycle_timeout_seconds    = sorc.cycle_timeout_seconds;
diff_curve_number        = sorc.diff_curve_number;

impact_pres_type[PRE_IMPACT_INDEX]  = sorc.impact_pres_type[PRE_IMPACT_INDEX];
impact_pres_type[POST_IMPACT_INDEX] = sorc.impact_pres_type[POST_IMPACT_INDEX];

impact_points[PRE_IMPACT_INDEX]     = sorc.impact_points[PRE_IMPACT_INDEX];
impact_points[POST_IMPACT_INDEX]    = sorc.impact_points[POST_IMPACT_INDEX];

smooth.n      = sorc.smooth.n;
smooth.factor = sorc.smooth.factor;
}