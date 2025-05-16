#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\names.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"
#include "..\include\genlist.h"
#include "..\include\verrors.h"

#include "extern.h"

/*
----------
State bits
---------- */
#define ACQUIRING      1
#define NOT_ACQUIRING  2
#define SKIPPING       4

struct SKIP_ENTRY
    {
    BITSETYPE state;
    BITSETYPE config;
    int32     max_saved;
    int32     skip_count;    /* Minutes or shots */
    int32     acquire_count;
    int32     current_count;
    FILETIME  last_time;
    };

struct MACH_SKIP_ENTRY
    {
    TCHAR      machine[MACHINE_NAME_LEN+1];
    SKIP_ENTRY parm;
    SKIP_ENTRY profile;
    };

GENERIC_LIST_CLASS SkipList;

/***********************************************************************
*                         FILL_SKIP_ENTRY                              *
***********************************************************************/
static void fill_skip_entry( SKIP_ENTRY * se, DB_TABLE & t, FIELDOFFSET fo )
{
SYSTEMTIME st;

se->acquire_count = t.get_long( fo + ACQUIRE_COUNT_OFFSET );
se->config        = t.get_long( fo + FLAGS_OFFSET );
se->max_saved     = t.get_long( fo + MAX_SAVED_OFFSET );
se->current_count = se->acquire_count;

GetLocalTime(&st);
SystemTimeToFileTime( &st, &se->last_time );

se->state = NOT_ACQUIRING;

if ( se->config & ACQUIRE_ALL_BIT )
    se->state = ACQUIRING;

else if ( se->config & (SKIP_TIME_BIT | SKIP_COUNT_BIT) )
    {
    if ( se->config & SKIP_TIME_BIT )
        se->skip_count = t.get_long( fo + TIME_SKIP_OFFSET );
    else
        se->skip_count = t.get_long( fo );

    if ( se->acquire_count > 0 )
        se->state = ACQUIRING;
    }

}

/***********************************************************************
*                              RESET                                   *
***********************************************************************/
static BOOLEAN reset( MACH_SKIP_ENTRY * mse )
{

DWORD err;
DB_TABLE t;
t.open( machset_dbname(ComputerName), MACHSET_RECLEN, PFL );
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, mse->machine, MACHINE_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    {
    fill_skip_entry( &mse->profile, t, MACHSET_PROFILE_SKIP_COUNT_OFFSET );
    fill_skip_entry( &mse->parm,    t, MACHSET_PARAM_SKIP_COUNT_OFFSET );
    }
t.close();

err = t.get_global_error();
if ( err != VS_SUCCESS )
    {
    error_message( SKIP_RESET, DB_ERROR, short(err) );
    return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                              FIND                                    *
***********************************************************************/
static MACH_SKIP_ENTRY * find( TCHAR * machine_to_find )
{

static MACH_SKIP_ENTRY * mse = 0;

/*
-------------------------------
Find the entry for this machine
------------------------------- */
if ( !mse || lstrcmp(mse->machine, machine_to_find) != 0 )
    {
    SkipList.rewind();
    while ( TRUE )
        {
        mse = (MACH_SKIP_ENTRY *) SkipList.next();
        if ( !mse )
            break;
        if ( lstrcmp(mse->machine, machine_to_find) == 0 )
            break;
        }
    }

/*
-------------------------------------
If I didn't find an entry, create one
------------------------------------- */
if ( !mse )
    {
    mse = new MACH_SKIP_ENTRY;
    if ( mse )
        {
        lstrcpy(mse->machine, machine_to_find);
        reset( mse );
        if ( !SkipList.append( mse ) )
            {
            delete mse;
            mse = 0;
            error_message( SKIP_FIND, NO_MEM_ALLOC_ERROR );
            }
        }
    }

if ( !mse )
    error_message( SKIP_FIND, NO_MEM_ALLOC_ERROR );

return mse;
}

/***********************************************************************
*                         SKIP_RESET_MACHINE                           *
***********************************************************************/
BOOLEAN skip_reset_machine( TCHAR * machine )
{

MACH_SKIP_ENTRY * mse;

mse = find( machine );
if ( mse )
    return reset( mse );

return FALSE;
}

/***********************************************************************
*                            SKIP_STARTUP                              *
***********************************************************************/
BOOLEAN skip_startup( void )
{
return TRUE;
}

/***********************************************************************
*                            SKIP_SHUTDOWN                             *
***********************************************************************/
void skip_shutdown( void )
{

MACH_SKIP_ENTRY * mse;

SkipList.rewind();
while ( TRUE )
    {
    mse = (MACH_SKIP_ENTRY *) SkipList.next();
    if ( !mse )
        break;
    delete mse;
    }

SkipList.remove_all();
}

/***********************************************************************
*                       INCREMENT_SHOT_ENTRY                           *
***********************************************************************/
static BOOLEAN increment_shot_entry( SKIP_ENTRY * se, FILETIME time_of_shot )
{

BOOLEAN return_status;
int32    time_dif;
return_status = FALSE;

if ( se->state & ACQUIRING )
    {
    /*
    -----------------------
    I always save this shot
    ----------------------- */
    return_status = TRUE;

    /*
    ---------------------------------------
    See if it it time to switch to skipping
    --------------------------------------- */
    if ( !(se->config & ACQUIRE_ALL_BIT) )
        {
        if ( se->config & (SKIP_COUNT_BIT | SKIP_TIME_BIT) && se->skip_count > 0 )
            {
            se->current_count--;
            if ( !se->current_count )
                {
                se->current_count = se->skip_count;
                se->last_time     = time_of_shot;
                se->state &= ~BITSETYPE(ACQUIRING);
                se->state |= SKIPPING;
                }
            }
        }
    }
else if ( se->state & SKIPPING )
    {
    /*
    ---------------------------------------------------
    Leave return FALSE so I will skip this shot. See if
    it is time to switch to acquiring.
    --------------------------------------------------- */
    if ( se->config & SKIP_COUNT_BIT )
        {
        se->current_count--;
        }
    else if ( se->config & SKIP_TIME_BIT )
        {
        time_dif = seconds_difference( se->last_time, time_of_shot );

        if ( time_dif < 0 )
            time_dif *= -1;

        if ( time_dif > (long) se->skip_count )
            se->current_count = 0;
        }

    if ( !se->current_count )
        {
        se->current_count = se->acquire_count;
        se->state &= ~BITSETYPE(SKIPPING);
        se->state |= ACQUIRING;
        }
    }

return return_status;
}

/***********************************************************************
*                       SKIP_NEW_SHOT_NOTIFY                           *
*                                                                      *
*  Call this to notify me that a new shot has been received. I will    *
*  return with SAVE_PARM_DATA set if the parameter data should be      *
*  saved and with SAVE_PROFILE_DATA set if the profile should be       *
*  saved.                                                              *
*                                                                      *
***********************************************************************/
BITSETYPE skip_new_shot_notify( TCHAR * machine, FILETIME time_of_shot )
{

MACH_SKIP_ENTRY * mse;
BITSETYPE return_bits;

return_bits = 0;
mse         = find( machine );

if ( mse )
    {

    if ( increment_shot_entry(&mse->parm,   time_of_shot) )
        return_bits |= SAVE_PARM_DATA;

    if ( increment_shot_entry(&mse->profile, time_of_shot) )
        return_bits |= SAVE_PROFILE_DATA;
    }

return return_bits;
}

/***********************************************************************
*                         MAX_PARAMETERS_SAVED                         *
***********************************************************************/
int32 max_parameters_saved( TCHAR * machine )
{
MACH_SKIP_ENTRY * mse;

mse = find( machine );

if ( mse )
    return mse->parm.max_saved;

return 0;
}

/***********************************************************************
*                          MAX_PROFILES_SAVED                          *
***********************************************************************/
int32 max_profiles_saved( TCHAR * machine )
{
MACH_SKIP_ENTRY * mse;

mse = find( machine );

if ( mse )
    return mse->profile.max_saved;

return 0;
}

