#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"

static WORD DaysPerMonth[2][12] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }  }; /* Dec not needed */

static TCHAR * MonthAbreviation[13] = {
    { TEXT("???") },
    { TEXT("jan") }, { TEXT("feb") },
    { TEXT("mar") }, { TEXT("apr") },
    { TEXT("may") }, { TEXT("jun") },
    { TEXT("jul") }, { TEXT("aug") },
    { TEXT("sep") }, { TEXT("oct") },
    { TEXT("nov") }, { TEXT("dec") }
    };

const int NORMAL_YEAR_INDEX   = 0;
const int LEAP_YEAR_INDEX     = 1;
const int BASE_YEAR           = 1990;
const int BASE_WEEKDAY        = 1;        /* Jan 1, 1990 = Monday */

static const TCHAR SpaceChar = TEXT( ' ' );
static const TCHAR NullChar  = TEXT( '\0' );

int day_of_the_week( int m, int d, int y );

/***********************************************************************
*                              LEAP_INDEX                              *
***********************************************************************/
static int leap_index( int year )
{

if ( year%4 == 0 && year%100 != 0 )
    return LEAP_YEAR_INDEX;

if ( year%400 == 0 )
    return LEAP_YEAR_INDEX;

return NORMAL_YEAR_INDEX;
}

/***********************************************************************
*                           INIT_SYSTEMTIME                            *
***********************************************************************/
static void init_systemtime( SYSTEMTIME & st )
{

st.wYear         = 1990;
st.wMonth        = 1;
st.wDayOfWeek    = BASE_WEEKDAY;
st.wDay          = 1;
st.wHour         = 0;
st.wMinute       = 0;
st.wSecond       = 0;
st.wMilliseconds = 0;

}

/***********************************************************************
*                              TIME_CLASS                              *
*                                  -=                                  *
***********************************************************************/
void TIME_CLASS::operator-=( unsigned long sorc )
{

if ( sorc < n )
    n -= sorc;
else
    n = 0;

}

/***********************************************************************
*                                TIME_CLASS                            *
*                                    -                                 *
***********************************************************************/
long TIME_CLASS::operator-( const TIME_CLASS & sorc )
{
long d;

if ( sorc.n > n )
    {
    d = (long) sorc.n - n;
    d = -d;
    }
else
    {
    d = (long) n - sorc.n;
    }

return d;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                            GET_LOCAL_TIME                            *
***********************************************************************/
void TIME_CLASS::get_local_time( void )
{
SYSTEMTIME t;

GetLocalTime( &t );
set( t );
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                 SET                                  *
***********************************************************************/
BOOLEAN TIME_CLASS::set( const SYSTEMTIME & t )
{
int i;
int leap;
int nof_days;
int nof_months;

n = 0UL;

for ( i=1990; i<t.wYear; i++ )
    {
    n += SECONDS_PER_YEAR;
    if ( leap_index(i) == LEAP_YEAR_INDEX )
        n += SECONDS_PER_DAY;
    }

leap = leap_index(t.wYear);
nof_months = t.wMonth - 1;
for ( i=0; i<nof_months; i++ )
    n += DaysPerMonth[leap][i] * SECONDS_PER_DAY;

nof_days = t.wDay - 1;
n += SECONDS_PER_DAY * nof_days;
n += SECONDS_PER_HOUR * (unsigned long) t.wHour;
n += SECONDS_PER_MINUTE * (unsigned long) t.wMinute;
n += (unsigned long) t.wSecond;

return TRUE;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                 SET                                  *
***********************************************************************/
BOOLEAN TIME_CLASS::set( const FILETIME & f )
{
SYSTEMTIME t;

FileTimeToSystemTime( &f, &t );
set( t );
return TRUE;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                              SYSTEM_TIME                             *
***********************************************************************/
SYSTEMTIME & TIME_CLASS::system_time( void )
{
int days_this_month;
unsigned long mysec;
unsigned long sec_this_period;
int           i;
int           leap;
static SYSTEMTIME st;

init_systemtime( st );

mysec = 0;

while ( TRUE )
    {
    leap  = leap_index( st.wYear );

    sec_this_period = SECONDS_PER_YEAR;
    if ( leap == LEAP_YEAR_INDEX )
        sec_this_period += SECONDS_PER_DAY;

    mysec += sec_this_period;
    if ( n < mysec )
        {
        mysec -= sec_this_period;
        for ( i=0; i<12; i++ )
            {
            days_this_month = DaysPerMonth[leap][i];
            sec_this_period = days_this_month * SECONDS_PER_DAY;
            mysec += sec_this_period;
            if ( n < mysec )
                {
                mysec -= sec_this_period;
                st.wMonth = i + 1;
                for ( i=0; i<days_this_month; i++ )
                    {
                    mysec += SECONDS_PER_DAY;
                    if ( n < mysec )
                        {
                        mysec -= SECONDS_PER_DAY;
                        st.wDay = i + 1;
                        for ( i=0; i<24; i++ )
                            {
                            mysec += SECONDS_PER_HOUR;
                            if ( n < mysec )
                                {
                                mysec -= SECONDS_PER_HOUR;
                                st.wHour = i;
                                for ( i=0; i<60; i++ )
                                    {
                                    mysec += SECONDS_PER_MINUTE;
                                    if ( n < mysec )
                                        {
                                        mysec -= SECONDS_PER_MINUTE;
                                        st.wMinute = i;
                                        st.wSecond = n - mysec;
                                        break;
                                        }
                                    }
                                break;
                                }
                            }
                        break;
                        }
                    }
                break;
                }
            }
        break;
        }

    st.wYear++;
    }

st.wDayOfWeek = day_of_the_week( (int) st.wMonth, (int) st.wDay, (int) st.wYear );

return st;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                               FILE_TIME                              *
***********************************************************************/
FILETIME & TIME_CLASS::file_time( void )
{
SYSTEMTIME st;
static FILETIME ft;
st = system_time();

SystemTimeToFileTime( &st, &ft );
return ft;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                TEXT                                  *
***********************************************************************/
TCHAR * TIME_CLASS::text( void )
{
static TCHAR s[ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1];
SYSTEMTIME   st;
TCHAR      * cp;

*s = NullChar;
st = system_time();

cp = copy_w_char( s, alphadate(st), SpaceChar );
lstrcpy( cp, alphatime(st) );

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                               0123456789                             *
*                               12/31/04                               *
*                                MMDDYY                                *
***********************************************************************/
TCHAR * TIME_CLASS::mmddyy( void )
{
static TCHAR s[ALPHADATE_LEN + 1];
SYSTEMTIME   st;

st = system_time();
lstrcpy( s, alphadate(st) );
s[6] = s[8];
s[7] = s[9];
s[8] = NullChar;

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                               MMDDYYYY                               *
***********************************************************************/
TCHAR * TIME_CLASS::mmddyyyy( void )
{
static TCHAR s[ALPHADATE_LEN + 1];
SYSTEMTIME   st;

st = system_time();
lstrcpy( s, alphadate(st) );

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                  MM                                  *
***********************************************************************/
TCHAR * TIME_CLASS::mm()
{
TCHAR * s;

s = mmddyyyy();
s[2] = NullChar;

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                 MMM                                  *
***********************************************************************/
TCHAR * TIME_CLASS::mmm()
{
SYSTEMTIME   st;
int i;

st = system_time();
i  = st.wMonth;
if ( i<1 || i>12 )
    i = 0;

return MonthAbreviation[i];
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                  DD                                  *
***********************************************************************/
TCHAR * TIME_CLASS::dd()
{
TCHAR * s;

s = mmddyyyy();
s += 3;
s[2] = NullChar;

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                  YY                                  *
      12/19/2002
***********************************************************************/
TCHAR * TIME_CLASS::yy()
{
TCHAR * s;

s = mmddyyyy();
s += 8;
s[2] = NullChar;

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                 YYYY                                 *
***********************************************************************/
TCHAR * TIME_CLASS::yyyy()
{
TCHAR * s;

s = mmddyyyy();
s += 6;

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                HHMMSS                                *
***********************************************************************/
TCHAR * TIME_CLASS::hhmmss( void )
{
static TCHAR s[ALPHATIME_LEN + 1];
SYSTEMTIME   st;

st = system_time();
lstrcpy( s, alphatime(st) );

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                 HHMM                                 *
***********************************************************************/
TCHAR * TIME_CLASS::hhmm()
{
TCHAR * s;
s = hhmmss();
s[5] = NullChar;

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                  HH                                  *
***********************************************************************/
TCHAR * TIME_CLASS::hh()
{
TCHAR * s;
s = hhmmss();
s[2] = NullChar;

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                  MI                                  *
***********************************************************************/
TCHAR * TIME_CLASS::mi()
{
TCHAR * s;
s = hhmmss();
s += 3;
s[2] = NullChar;

return s;
}

/***********************************************************************
*                              TIME_CLASS                              *
*                                  =                                   *
*                          10/23/1998 12:34:54                         *
***********************************************************************/
void TIME_CLASS::operator=( const TCHAR * sorc )
{
SYSTEMTIME   st;

init_systemtime( st );

if ( lstrlen(sorc) >= ALPHADATE_LEN )
    extmdy( st, sorc );

sorc += ALPHADATE_LEN;
if ( lstrlen(sorc) > ALPHATIME_LEN )
    {
    sorc++;
    exthms( st, sorc );
    }

set( st );
}

/***********************************************************************
*                              TIME_CLASS                              *
*                               SET_TIME                               *
***********************************************************************/
void TIME_CLASS::set_time( const TCHAR * sorc )
{
SYSTEMTIME   st;

st = system_time();
exthms( st, sorc );
set( st );
}
