#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"

const int SUNDAY   = 0;
const int SATURDAY = 6;

/***********************************************************************
*                            start_of_shift                            *
*                                                                      *
* You pass me a time and I figure which shift you are in and return    *
* the time of the start of that shift.                                 *
***********************************************************************/
SYSTEMTIME & start_of_shift( const SYSTEMTIME & now )
{
static SYSTEMTIME st;

DB_TABLE   t;
TCHAR      day[DOW_NAME_LEN+1];
int        today;
int        yeasterday;
int        tomorrow;

int        dow;
int        last_dow;
SYSTEMTIME last_st;
TIME_CLASS rectime;
TIME_CLASS nowtime;
BOOLEAN    have_today;

have_today = FALSE;

nowtime.set( now );
st      = now;
last_st = now;

today    = now.wDayOfWeek;
last_dow = today;

yeasterday = today - 1;
if ( yeasterday < SUNDAY )
    yeasterday = SATURDAY;

tomorrow = today + 1;
if ( tomorrow > SATURDAY )
    tomorrow = SUNDAY;

if ( t.open(shift_dbname(), SHIFT_RECLEN, PFL) )
    {
    /*
    -------------------------------------------------
    Find the first record that is after me (timewise)
    ------------------------------------------------- */
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( day, SHIFT_ACTUAL_DAY_OFFSET, DOW_NAME_LEN );
        dow = (int) dow_from_string( day );

        if ( have_today && dow == tomorrow )
            break;

        if ( dow == today )
            {
            have_today = TRUE;
            t.get_time( st, SHIFT_START_TIME_OFFSET );
            rectime.set( st );
            if ( nowtime < rectime )
                break;
            }

        /*
        -----------------------------------------------------------------------------------------------
        Remember the time as this will be the start of the shift when I get to a record that is greater
        ----------------------------------------------------------------------------------------------- */
        last_dow = dow;
        last_st  = st;
        }
    t.close();
    }

rectime.set( last_st );
if ( last_dow == yeasterday )
    {
    rectime -= SECONDS_PER_DAY;
    st = rectime.system_time();
    st.wHour = last_st.wHour;
    st.wMinute = last_st.wMinute;
    st.wSecond = last_st.wSecond;
    st.wMilliseconds = 0;
    }

return st;
}
