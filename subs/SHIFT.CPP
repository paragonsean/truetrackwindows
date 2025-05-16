#include <windows.h>
#include <time.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\timeclas.h"
#include "..\include\shiftcl.h"
#include "..\include\subs.h"

const short SATURDAY = 6;
const short SUNDAY   = 0;

struct SHIFT
    {
    short day;
    short shift;
    short start_minutes;             /* Minutes since Sunday 00:00 */
    };

struct SHIFT * Shifts    = 0;
short          NofShifts = 0;

static TCHAR * Days[] = {
 TEXT("SU"),
 TEXT("MO"),
 TEXT("TU"),
 TEXT("WE"),
 TEXT("TH"),
 TEXT("FR"),
 TEXT("SA")
 };

/*
-----------------------------------------
Variables for detecting shift change time
----------------------------------------- */
static short CurrentShiftDow    = -1;
static short CurrentShiftNumber = -1;
static TCHAR NullChar = TEXT( '\0' );

/***********************************************************************
*                      ADD_SECONDS_TO_SYSTEMTIME                       *
***********************************************************************/
static BOOLEAN add_seconds_to_systemtime( SYSTEMTIME & st, int32 seconds )
{
TIME_CLASS t;;
unsigned long u;
BOOLEAN is_negative;

is_negative = FALSE;
if ( seconds < 0 )
    {
    is_negative = TRUE;
    seconds = -seconds;
    }

u = seconds;
t.set( st );
if ( is_negative )
    t -= u;
else
    t += u;

st = t.system_time();

return TRUE;
};

/***********************************************************************
*                               TOMORROW                               *
***********************************************************************/
static short tomorrow( short today )
{

today++;
if ( today > SATURDAY )
    today = SUNDAY;

return today;
}

/***********************************************************************
*                              YEASTERDAY                              *
***********************************************************************/
static short yeasterday( short today )
{

today--;
if ( today < SUNDAY )
    today = SATURDAY;

return today;
}

/***********************************************************************
*                         MINUTES_PAST_SUNDAY                          *
***********************************************************************/
short minutes_past_sunday( const SYSTEMTIME & sorc )
{
return short( sorc.wDayOfWeek * MINUTES_PER_DAY + sorc.wHour*60 + sorc.wMinute );
}

/***********************************************************************
*                           DOW_FROM_STRING                            *
*   Sun = 0  Mon = 1  Tue = 2  Wed = 3  Thu = 4  Fri = 5  Sat = 6      *
*   No Match = Sunday                                                  *
***********************************************************************/
short dow_from_string( TCHAR * day )
{
short i;
TCHAR s[3];

/*
----------------------------------------------------
I only need the first two characters of the day name
---------------------------------------------------- */
s[0] = day[0];
s[1] = day[1];
s[2] = NullChar;

upper_case( s );

for ( i=0; i<7; i++ )
    if ( lstrcmp(s,Days[i]) == 0 )
        return i;

return 0;
}

/***********************************************************************
*                            SHIFT_FROM_REC                            *
***********************************************************************/
static void shift_from_rec( short * dest_for_actual_dow, short i, DB_TABLE & t )
{
short      dow;
TCHAR      dayname[DOW_NAME_LEN+1];
SYSTEMTIME st;

/*
-------------------------------------------------
The day of the week that is kept is the shift day
------------------------------------------------- */
t.get_alpha( dayname, SHIFT_DAY_OFFSET, DOW_NAME_LEN );
Shifts[i].day   = dow_from_string( dayname );
Shifts[i].shift = t.get_short( SHIFT_NUMBER_OFFSET );

/*
----------------------------------------------------
The start_minutes are minutes since 00:00 on Sunday.
---------------------------------------------------- */
t.get_time( st, SHIFT_START_TIME_OFFSET );
Shifts[i].start_minutes = 60 * st.wHour + st.wMinute;

t.get_alpha( dayname, SHIFT_ACTUAL_DAY_OFFSET, DOW_NAME_LEN );
dow = dow_from_string( dayname );

Shifts[i].start_minutes += dow * MINUTES_PER_DAY;

/*
----------------------------------------------------------
Copy the actual day of the week if the caller requested it
---------------------------------------------------------- */
if ( dest_for_actual_dow )
    *dest_for_actual_dow = dow;

}

/***********************************************************************
*                             SHIFT_INDEX                              *
***********************************************************************/
static short shift_index( const SYSTEMTIME & now )
{
short i;
short mps;

if ( !NofShifts )
    return NO_INDEX;

mps = minutes_past_sunday( now );

for ( i=0; i<NofShifts; i++ )
    {
    if ( mps >= Shifts[i].start_minutes && mps < Shifts[i+1].start_minutes )
        return i;
    }

/*
--------------------------------------------------------------------------------
If the first shift on sunday starts on saturday then the minutes for this are in
the NofShifts entry.
-------------------------------------------------------------------------------- */
if ( mps >= Shifts[i].start_minutes )
    return NofShifts;

/*
--------------------------------------------------------------------
If this is before the first time, this is the last shift of saturday
otherwise this is the first shift of Sunday.
-------------------------------------------------------------------- */
i = 0;
if ( mps < Shifts[0].start_minutes )
    i = NofShifts - 1;

return i;
}

/***********************************************************************
*                             SHIFT_NUMBER                             *
***********************************************************************/
short shift_number( short * shift_dow, const SYSTEMTIME & now )
{
short i;

i = shift_index( now );
if ( i == NO_INDEX )
    {
    if ( shift_dow )
        *shift_dow = now.wDayOfWeek;
    return 1;
    }

if ( shift_dow )
    *shift_dow = Shifts[i].day;

return Shifts[i].shift;
}

/***********************************************************************
*                            START_OF_SHIFT                            *
***********************************************************************/
SYSTEMTIME & start_of_shift( const SYSTEMTIME & now )
{
static SYSTEMTIME st;
short i;
short mps;
int32 seconds;

st = now;
st.wSecond = 0;
st.wMilliseconds = 0;

i = shift_index( now );
if ( i != NO_INDEX )
    {
    mps = Shifts[i].start_minutes - minutes_past_sunday( st );
    seconds = mps;
    /*
    --------------------------------------------------------------------------------
    If this is positive then this should be yeasterday and I need to subtract 7 days
    -------------------------------------------------------------------------------- */
    if ( seconds > 0 )
        seconds -= 7*MINUTES_PER_DAY;
    seconds *= 60;
    add_seconds_to_systemtime( st, seconds );
    }

return st;
}

/***********************************************************************
*                      GET_SHIFT_NUMBER_AND_DATE                       *
***********************************************************************/
void get_shift_number_and_date( short * shift, SYSTEMTIME * shift_date, const SYSTEMTIME & true_date )
{

short true_dow;
short dow;

true_dow = true_date.wDayOfWeek;

/*
-------------------------------
Get the shift and shift weekday
------------------------------- */
*shift = shift_number( &dow, true_date );

/*
---------------------------------------------------------------
Set the shift date and adjust it if the weekday is not the same
--------------------------------------------------------------- */
*shift_date = true_date;

if ( dow != true_dow )
    {
    if ( dow == tomorrow(true_dow) )
        add_seconds_to_systemtime( *shift_date, SEC_PER_DAY );
    else if ( dow == yeasterday(true_dow) )
        add_seconds_to_systemtime( *shift_date, -SEC_PER_DAY );
    }

}

/***********************************************************************
*                            SHIFTS_STARTUP                            *
***********************************************************************/
BOOLEAN shifts_startup( void )
{

BOOLEAN all_ok;
short   actual_dow;
short   i;
DB_TABLE t;
SYSTEMTIME st;

all_ok    = FALSE;

NofShifts = 0;
if ( t.open(shift_dbname(), SHIFT_RECLEN, PFL) )
    {
    NofShifts = t.nof_recs();
    if ( NofShifts )
        {
        /*
        ---------------------------------------
        Allocate the Shifts array (n+1 records)
        --------------------------------------- */
        Shifts = new SHIFT[NofShifts+1];

        all_ok = TRUE;

        /*
        ---------------------
        Read the first record
        --------------------- */
        if ( t.get_next_record(NO_LOCK) )
            {
            shift_from_rec( &actual_dow, 0, t );

            /*
            ---------------------------------------------------------------
            The final record (record n if I start at 0) is used only
            to check for the end of shift and for the day if Sunday's first
            shift starts on Saturday.
            --------------------------------------------------------------- */
            if ( actual_dow == SATURDAY )
                {
                /*
                --------------------------------------------------------
                If the current time is greater than the nth start minute
                then the shift is the first on Sunday.
                -------------------------------------------------------- */
                Shifts[NofShifts].start_minutes = Shifts[0].start_minutes;
                Shifts[0].start_minutes = 0;
                }
            else
                Shifts[NofShifts].start_minutes = 7 * MINUTES_PER_DAY;

            /*
            --------------------------------------------------------
            The day and shift of the nth record will only be used if
            Sunday's first shift starts on Saturday.
            -------------------------------------------------------- */
            Shifts[NofShifts].day   = SUNDAY;
            Shifts[NofShifts].shift = Shifts[0].shift;
            }
        else
            all_ok = FALSE;

        /*
        ----------------------------------------
        Read the rest of the records  (1 to n-1)
        ---------------------------------------- */
        for ( i=1; i<NofShifts; i++ )
            {
            if ( all_ok && t.get_next_record(NO_LOCK) )
                shift_from_rec( &actual_dow, i, t );
            else
                all_ok = FALSE;
            }
        }

    t.close();
    }

/*
--------------------------------------
Initialize the current shift variables
-------------------------------------- */
GetLocalTime(&st);
CurrentShiftNumber = shift_number( &CurrentShiftDow, st );

return all_ok;
}

/***********************************************************************
*                           SHIFTS_SHUTDOWN                            *
***********************************************************************/
void shifts_shutdown( void )
{

if ( Shifts )
    delete[] Shifts;

}

/***********************************************************************
*                        CHECK_FOR_SHIFT_CHANGE                        *
***********************************************************************/
void check_for_shift_change( void )
{
short shift;
short dow;
SYSTEMTIME st;

GetLocalTime(&st);
shift = shift_number( &dow, st );
if ( shift != CurrentShiftNumber || dow != CurrentShiftDow )
    {
    CurrentShiftNumber = shift;
    CurrentShiftDow    = dow;

    /*
    -------------------------------
    Issue new shift event if needed
    ------------------------------- */
    }

}

/***********************************************************************
*                             SHIFT_CLASS                              *
***********************************************************************/
SHIFT_CLASS::SHIFT_CLASS()
{
}

/***********************************************************************
*                            ~SHIFT_CLASS                              *
***********************************************************************/
SHIFT_CLASS::~SHIFT_CLASS()
{
}

/***********************************************************************
*                             SHIFT_CLASS                              *
*                          SET_START_OF_SHIFT                          *
***********************************************************************/
BOOLEAN SHIFT_CLASS::set_start_of_shift( const SYSTEMTIME & new_st, short new_shift_number )
{
short i;
short days_back;
short mps;
SYSTEMTIME st;

st = new_st;

for ( i=0; i<NofShifts; i++ )
    {
    if ( st.wDayOfWeek == Shifts[i].day && new_shift_number == Shifts[i].shift )
        {
        mps = Shifts[i].start_minutes;
        days_back = st.wDayOfWeek;

        if ( i == 0 && Shifts[NofShifts].day == SATURDAY )
            {
            mps = Shifts[NofShifts].start_minutes;
            days_back += 7;
            }

        st.wMilliseconds = 0;
        st.wSecond       = 0;
        st.wMinute       = 0;
        st.wHour         = 0;

        time.set( st );

        time -= days_back * SEC_PER_DAY;
        time += mps * SEC_PER_MINUTE;

        number = new_shift_number;

        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                             SHIFT_CLASS                              *
*                                 READ                                 *
***********************************************************************/
BOOLEAN SHIFT_CLASS::read( DB_TABLE & t, FIELDOFFSET date_offset, FIELDOFFSET shift_offset )
{
SYSTEMTIME st;

st.wMilliseconds = 0;
st.wSecond       = 0;
st.wMinute       = 0;
st.wHour         = 12;

t.get_date( st, date_offset );
number = t.get_long( shift_offset );

if ( t.get_last_error() == VS_SUCCESS )
    {
    return set_start_of_shift( st, number );
    }

return FALSE;
}

