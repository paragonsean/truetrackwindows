#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dateclas.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"
#include "..\include\genlist.h"
#include "..\include\alarmsum.h"
#include "..\include\names.h"
#include "..\include\shiftcl.h"
#include "..\include\subs.h"

static const int32 ALL_SHIFTS = -1;
static const WORD  ALL_YEARS  = 1601;
static const TCHAR NullChar   = TEXT( '\0' );

/***********************************************************************
*                              ADD                                     *
***********************************************************************/
ALARM_COUNTS_ENTRY * ALARM_SUMMARY_CLASS::add( int32 new_parameter_number )
{
int i;
ALARM_COUNTS_ENTRY * ae;
ALARM_COUNTS_ENTRY * nae;

nae = new ALARM_COUNTS_ENTRY;
if ( nae )
    {
    nae->parameter_number = new_parameter_number;
    for ( i=0; i<NOF_ALARM_LIMIT_TYPES; i++ )
        nae->counts[i] = 0;

    /*
    -------------------------------------------
    Add to the list ordered by parameter number
    ------------------------------------------- */
    alarmlist.rewind();
    while ( TRUE )
        {
        ae = (ALARM_COUNTS_ENTRY *) alarmlist.next();
        if ( !ae )
            {
            alarmlist.append( nae );
            break;
            }

        if ( ae->parameter_number > new_parameter_number )
            {
            alarmlist.insert( nae );
            break;
            }

        }
    }

return nae;
}

/***********************************************************************
*                                 FIND                                 *
***********************************************************************/
ALARM_COUNTS_ENTRY * ALARM_SUMMARY_CLASS::find( int32 parameter_number_to_find  )
{
ALARM_COUNTS_ENTRY * ae;

alarmlist.rewind();
while ( TRUE )
    {
    ae = (ALARM_COUNTS_ENTRY *) alarmlist.next();
    if ( !ae )
        break;

    if ( ae->parameter_number == parameter_number_to_find )
        break;
    }

return ae;
}

/***********************************************************************
*                                COUNT                                 *
***********************************************************************/
int32 ALARM_SUMMARY_CLASS::count( int32 parameter_number, int32 alarm_type )
{
ALARM_COUNTS_ENTRY * ae;
int32 n;

n = 0;
if ( alarm_type >= 0 && alarm_type < NOF_ALARM_LIMIT_TYPES )
    {
    ae = find( parameter_number );
    if ( ae )
        n = ae->counts[alarm_type];
    }

return n;
}

/***********************************************************************
*                                EMPTY                                 *
***********************************************************************/
void ALARM_SUMMARY_CLASS::empty( void )
{

ALARM_COUNTS_ENTRY * ae;

alarmlist.rewind();
while ( TRUE )
    {
    ae = (ALARM_COUNTS_ENTRY *) alarmlist.next();
    if ( !ae )
        break;
    delete ae;
    }

alarmlist.remove_all();

total_shots          = 0;
total_alarms_count   = 0;
total_warnings_count = 0;
}

/***********************************************************************
*                            ALARM_SUMMARY_CLASS                       *
***********************************************************************/
ALARM_SUMMARY_CLASS::ALARM_SUMMARY_CLASS()
{

lstrcpy( pe.computer, NO_COMPUTER );
lstrcpy( pe.machine,  NO_MACHINE  );
lstrcpy( pe.part,     NO_PART     );

total_shots          = 0;
total_alarms_count   = 0;
total_warnings_count = 0;

start_shift = 1;
end_shift = 1;
}

/***********************************************************************
*                           ~ALARM_SUMMARY_CLASS                       *
***********************************************************************/
ALARM_SUMMARY_CLASS::~ALARM_SUMMARY_CLASS()
{
empty();
}

/***********************************************************************
*                              SET_PART                                *
***********************************************************************/
void ALARM_SUMMARY_CLASS::set_part( TCHAR * new_computer, TCHAR * new_machine,  TCHAR * new_part )

{
lstrcpy( pe.computer, new_computer );
lstrcpy( pe.machine,  new_machine  );
lstrcpy( pe.part,     new_part     );
}

/***********************************************************************
*                   ALARM_SUMMARY_CLASS::GET_COUNTS                    *
***********************************************************************/
BOOLEAN ALARM_SUMMARY_CLASS::get_counts( SYSTEMTIME & date_to_find, int32 shift_to_find, TCHAR * fname )
{
DB_TABLE             t;
ALARM_COUNTS_ENTRY * ae;
SYSTEMTIME           mydate;
int32                pn;
int32                myshift;
BOOLEAN              status;
BOOLEAN              first_time;

/*
--------------------------------
Initialize mydate time variables
-------------------------------- */
mydate = date_to_find;

/*
--------------------------
Forget any existing counts
-------------------------- */
empty();
first_time = TRUE;

if ( !file_exists(fname) )
    return FALSE;

if ( t.open( fname, ALARMSUM_RECLEN, PFL) )
    {
    status = FALSE;

    if ( date_to_find.wYear != ALL_YEARS )
        {
        t.put_date( ALARMSUM_DATE_OFFSET,  date_to_find );
        status = t.get_next_key_match( 1, NO_LOCK );
        }

    if ( status )
        {
        if ( shift_to_find != ALL_SHIFTS )
            {
            status = FALSE;
            while ( TRUE )
                {
                t.get_date( mydate, ALARMSUM_DATE_OFFSET );
                if ( !samedate( mydate, date_to_find ) )
                    break;

                myshift = t.get_long( ALARMSUM_SHIFT_OFFSET );
                if ( myshift == shift_to_find )
                    {
                    status = TRUE;
                    break;
                    }

                if ( !t.get_next_record(NO_LOCK) )
                    break;
                }
            }
        }
    else
        {
        status = t.get_next_record( NO_LOCK );
        }

    if ( status )
        {
        while ( TRUE )
            {
            t.get_date( mydate, ALARMSUM_DATE_OFFSET );
            myshift = t.get_long( ALARMSUM_SHIFT_OFFSET );

            /*
            -----------------------
            Remember the first date
            ----------------------- */
            if ( first_time )
                {
                first_time = FALSE;
                start.set( mydate );
                start_shift = myshift;
                }

            if ( date_to_find.wYear != ALL_YEARS )
                {

                if ( !samedate( mydate, date_to_find ) )
                    break;
                }


            if ( shift_to_find != ALL_SHIFTS )
                {
                if ( myshift != shift_to_find )
                    break;
                }

            end.set( mydate );
            end_shift = myshift;

            /*
            ----------------------------------------------------------
            If there is an existing entry, add to it.
            Otherwise create a new entry for this parameter and append
            it to the list.
            ---------------------------------------------------------- */
            pn = t.get_long( ALARMSUM_PARAMETER_NUMBER_OFFSET );

            if ( pn == TOTALS_PARAMETER_NUMBER )
                {
                total_shots          += t.get_long( ALARMSUM_TOTAL_COUNT_OFFSET );
                total_alarms_count   += t.get_long( ALARMSUM_TOTAL_ALARM_COUNT_OFFSET );
                total_warnings_count += t.get_long( ALARMSUM_TOTAL_WARNING_COUNT_OFFSET );
                }
            else
                {
                ae = find( pn );
                if ( !ae )
                    {
                    ae = add( pn );
                    }

                if ( ae )
                    {
                    ae->counts[ALARM_MIN]   += t.get_long( ALARMSUM_LOW_ALARM_COUNT_OFFSET );
                    ae->counts[ALARM_MAX]   += t.get_long( ALARMSUM_HIGH_ALARM_COUNT_OFFSET );
                    ae->counts[WARNING_MIN] += t.get_long( ALARMSUM_LOW_WARNING_COUNT_OFFSET );
                    ae->counts[WARNING_MAX] += t.get_long( ALARMSUM_HIGH_WARNING_COUNT_OFFSET );
                    }
                }

            if ( !t.get_next_record(NO_LOCK) )
                break;
            }
        }

    t.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                   ALARM_SUMMARY_CLASS::GET_COUNTS                    *
***********************************************************************/
BOOLEAN ALARM_SUMMARY_CLASS::get_counts( SYSTEMTIME & date_to_find, int32 shift_to_find )
{
return get_counts( date_to_find, shift_to_find, alarmsum_dbname(pe.computer, pe.machine, pe.part) );
}
 
/***********************************************************************
*                             GET_COUNTS                               *
***********************************************************************/
BOOLEAN ALARM_SUMMARY_CLASS::get_counts( void )
{

SYSTEMTIME mydate;
int32      myshift;

init_systemtime_struct( mydate );

mydate.wYear = ALL_YEARS;
myshift      = ALL_SHIFTS;

return get_counts( mydate, myshift, alarmsum_dbname(pe.computer, pe.machine, pe.part) );
}

/***********************************************************************
*                              GET_COUNTS                              *
***********************************************************************/
BOOLEAN ALARM_SUMMARY_CLASS::get_counts( SYSTEMTIME & date_to_find )
{
int32 myshift;

myshift      = ALL_SHIFTS;
return get_counts( date_to_find, myshift, alarmsum_dbname(pe.computer, pe.machine, pe.part) );
}

/***********************************************************************
*                              SET_COUNTS                              *
*     Value is the array of parameter values from a single shot.       *
***********************************************************************/
void ALARM_SUMMARY_CLASS::set_counts( float * parameter, PARAMETER_CLASS * p )
{
const BITSETYPE ALARM_MIN_BIT   = 1;
const BITSETYPE ALARM_MAX_BIT   = 2;
const BITSETYPE WARNING_MIN_BIT = 4;
const BITSETYPE WARNING_MAX_BIT = 8;

int32                i;
BITSETYPE            mask;
PARAMETER_ENTRY    * pe;
ALARM_COUNTS_ENTRY * ae;
BOOLEAN              have_alarm;
BOOLEAN              have_warning;

have_alarm   = FALSE;
have_warning = FALSE;

for ( i=0; i<MAX_PARMS; i++ )
    {
    pe = p->parameter + i;
    if ( pe->limits[ALARM_MIN].value == pe->limits[ALARM_MAX].value && pe->limits[WARNING_MIN].value == pe->limits[WARNING_MAX].value )
        continue;

    mask = 0;

    if ( pe->limits[ALARM_MIN].value != pe->limits[ALARM_MAX].value )
        {
        if ( parameter[i] < pe->limits[ALARM_MIN].value )
            {
            have_alarm = TRUE;
            mask |= ALARM_MIN_BIT;
            }

        if ( parameter[i] > pe->limits[ALARM_MAX].value )
            {
            have_alarm = TRUE;
            mask |= ALARM_MAX_BIT;
            }
        }

    if ( pe->limits[WARNING_MIN].value != pe->limits[WARNING_MAX].value )
        {
        if ( parameter[i] < pe->limits[WARNING_MIN].value )
            {
            have_warning = TRUE;
            mask |= WARNING_MIN_BIT;
            }

        if ( parameter[i] > pe->limits[WARNING_MAX].value )
            {
            have_warning = TRUE;
            mask |= WARNING_MAX_BIT;
            }
        }

    if ( mask )
        {
        ae = find( i+1 );
        if ( !ae )
            ae = add( i+1 );

        if ( ae )
            {
            if ( mask & ALARM_MIN_BIT )
                ae->counts[ALARM_MIN]++;
            if ( mask & ALARM_MAX_BIT )
                ae->counts[ALARM_MAX]++;
            if ( mask & WARNING_MIN_BIT )
                ae->counts[WARNING_MIN]++;
            if ( mask & WARNING_MAX_BIT )
                ae->counts[WARNING_MAX]++;
            }
        }
    }

total_shots++;

if ( have_alarm )
    total_alarms_count++;

if ( have_warning )
    total_warnings_count++;
}

/***********************************************************************
*                                 SAVE                                 *
***********************************************************************/
void ALARM_SUMMARY_CLASS::save( const SYSTEMTIME & date_to_save, int32 shift_to_save, BOOLEAN is_update, TCHAR * path )
{
/*
------------
Action types
------------ */
const int32 APPEND_ALL = 1;
const int32 INSERT_ALL = 2;
const int32 SAME_SHIFT = 3;
const int32 SKIP_REC   = 4;
const int32 ALL_DONE   = 5;

DB_TABLE             t;
ALARM_COUNTS_ENTRY * ae;
int32                i;
int32                sum;
int32                action;

int32                my_parameter_number;
SHIFT_CLASS          my_shift;

int32                this_parameter_number;
SHIFT_CLASS          this_shift;

ae = 0;

if ( !file_exists(path) )
    t.create( path );

action = SAME_SHIFT;
if ( t.open( path, ALARMSUM_RECLEN, WL) )
    {
    t.put_date( ALARMSUM_DATE_OFFSET,  date_to_save );
    if ( ! t.goto_first_equal_or_greater_record(1) )
        {
        action = APPEND_ALL;
        /*
        -----------------------------------------------
        This is now an update, there is nothing to read
        ----------------------------------------------- */
        is_update = TRUE;
        }
    else
        {
        t.get_current_record( NO_LOCK );
        }

    my_parameter_number = TOTALS_PARAMETER_NUMBER;                 /* Must be zero */
    my_shift.set_start_of_shift( date_to_save, shift_to_save );
    alarmlist.rewind();

    while ( TRUE )
        {
        if ( action == SAME_SHIFT || action == SKIP_REC )
            {
            /*
            ------------------------------------------
            See if this record is still the same shift
            ------------------------------------------ */
            this_shift.read( t, ALARMSUM_DATE_OFFSET, ALARMSUM_SHIFT_OFFSET );
            if ( this_shift > my_shift )
                action = INSERT_ALL;
            else if ( this_shift < my_shift )
                action = SKIP_REC;
            else
                action = SAME_SHIFT;
            }

        if ( action == SAME_SHIFT )
            {
            this_parameter_number = t.get_long( ALARMSUM_PARAMETER_NUMBER_OFFSET );
            if ( this_parameter_number < my_parameter_number )
                action = SKIP_REC;
            }
        else
            {
            this_parameter_number = NO_PARAMETER_NUMBER;
            }

        if ( action != SKIP_REC )
            {
            t.put_long( ALARMSUM_PARAMETER_NUMBER_OFFSET, my_parameter_number, PARAMETER_NUMBER_LEN );
            t.put_long( ALARMSUM_SHIFT_OFFSET, shift_to_save, SHIFT_LEN );
            t.put_date( ALARMSUM_DATE_OFFSET,  date_to_save );

            if ( my_parameter_number == TOTALS_PARAMETER_NUMBER )
                {
                /*
                ---------------
                Save the totals
                --------------- */

                i = total_shots;
                if ( !is_update )
                    i += t.get_long( ALARMSUM_TOTAL_COUNT_OFFSET );
                t.put_long( ALARMSUM_TOTAL_COUNT_OFFSET, i, ALARMSUM_COUNTS_LEN );

                i = total_alarms_count;
                if ( !is_update )
                    i += t.get_long( ALARMSUM_TOTAL_ALARM_COUNT_OFFSET );
                t.put_long( ALARMSUM_TOTAL_ALARM_COUNT_OFFSET, i, ALARMSUM_COUNTS_LEN );

                i = total_warnings_count;
                if ( !is_update )
                    i += t.get_long( ALARMSUM_TOTAL_WARNING_COUNT_OFFSET );
                t.put_long( ALARMSUM_TOTAL_WARNING_COUNT_OFFSET, i, ALARMSUM_COUNTS_LEN );

                t.put_long( ALARMSUM_HIGH_ALARM_COUNT_OFFSET, 0, ALARMSUM_COUNTS_LEN  );
                }
            else
                {
                if ( ae )
                    {
                    i = ae->counts[ALARM_MIN];
                    if ( !is_update && my_parameter_number == this_parameter_number )
                        i += t.get_long( ALARMSUM_LOW_ALARM_COUNT_OFFSET );
                    t.put_long( ALARMSUM_LOW_ALARM_COUNT_OFFSET, i, ALARMSUM_COUNTS_LEN );

                    i = ae->counts[ALARM_MAX];
                    if ( !is_update && my_parameter_number == this_parameter_number )
                        i += t.get_long( ALARMSUM_HIGH_ALARM_COUNT_OFFSET );
                    t.put_long( ALARMSUM_HIGH_ALARM_COUNT_OFFSET, i, ALARMSUM_COUNTS_LEN );

                    i = ae->counts[WARNING_MIN];
                    if ( !is_update && my_parameter_number == this_parameter_number )
                        i += t.get_long( ALARMSUM_LOW_WARNING_COUNT_OFFSET );
                    t.put_long( ALARMSUM_LOW_WARNING_COUNT_OFFSET, i, ALARMSUM_COUNTS_LEN );

                    i = ae->counts[WARNING_MAX];
                    if ( !is_update && my_parameter_number == this_parameter_number )
                        i += t.get_long( ALARMSUM_HIGH_WARNING_COUNT_OFFSET );
                    t.put_long( ALARMSUM_HIGH_WARNING_COUNT_OFFSET, i, ALARMSUM_COUNTS_LEN );
                    }
                }

            switch( action )
                {
                case APPEND_ALL:
                    t.rec_append();
                    break;

                case INSERT_ALL:
                    t.rec_insert();
                    break;

                case SAME_SHIFT:
                    if ( my_parameter_number == this_parameter_number )
                        t.rec_update();
                    else
                        t.rec_insert();
                    break;

                default:
                    break;
                }

            while ( TRUE )
                {
                ae = (ALARM_COUNTS_ENTRY *) alarmlist.next();
                if ( !ae )
                    {
                    action = ALL_DONE;
                    break;
                    }

                sum = 0;
                for ( i=0; i<NOF_ALARM_LIMIT_TYPES; i++ )
                    sum += ae->counts[i];

                if ( sum != 0 )
                    break;
                }

            if ( action == ALL_DONE )
                break;

            my_parameter_number = ae->parameter_number;
            }

        if ( action == SAME_SHIFT || action == SKIP_REC || action == INSERT_ALL )
            {
            if ( !t.get_next_record(NO_LOCK) )
                {
                action = APPEND_ALL;
                is_update = TRUE;
                }
            }
        }

    t.close();
    }

}

/***********************************************************************
*                                 SAVE                                 *
***********************************************************************/
void ALARM_SUMMARY_CLASS::save(  const SYSTEMTIME & date_to_save, int32 shift_to_save, BOOLEAN is_update )
{
save( date_to_save, shift_to_save, is_update, alarmsum_dbname(pe.computer, pe.machine, pe.part) );
}

/***********************************************************************
*                          ALARM_SUMMARY_CLASS                         *
*                                 SAVE                                 *
***********************************************************************/
void ALARM_SUMMARY_CLASS::save(  const SYSTEMTIME & date_to_save, int32 shift_to_save )
{
save( date_to_save, shift_to_save, FALSE, alarmsum_dbname(pe.computer, pe.machine, pe.part) );
}

