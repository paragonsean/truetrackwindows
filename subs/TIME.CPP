#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

#define SEC_PER_YEAR   31536000L
#define SEC_PER_DAY    86400L
#define SEC_PER_HOUR   3600L
#define SEC_PER_MINUTE 60L

static const TCHAR ColonChar = TEXT( ':' );
static const TCHAR NullChar  = TEXT( '\0' );
static const TCHAR SlashChar = TEXT( '/' );
static const TCHAR ZeroChar  = TEXT( '0' );

static TCHAR * Days[] = {
    TEXT("Sun"),
    TEXT("Mon"),
    TEXT("Tue"),
    TEXT("Wed"),
    TEXT("Thr"),
    TEXT("Fri"),
    TEXT("Sat"),
    TEXT("???")
    };



/***********************************************************************
*                              IS_NUMERIC                              *
***********************************************************************/
BOOLEAN is_numeric( TCHAR c )
{
if ( IsCharAlpha(c) )
    return FALSE;

if ( IsCharAlphaNumeric(c) )
    return TRUE;

return FALSE;
}

/********************************************************************
*                             GETDAY                                *
*          Copy the three character abreviation. No NULL!           *
*   Sun = 0  Mon = 1  Tue = 2  Wed = 3  Thu = 4  Fri = 5  Sat = 6   *
********************************************************************/
void getday( TCHAR * dest, short day )
{

if ( (day < 0) || (day > 6) )
    day = 7;

copychars( dest, Days[day], 3 );
}

/***********************************************************************
*                       INIT_SYSTEMTIME_STRUCT                         *
***********************************************************************/
void init_systemtime_struct( SYSTEMTIME & t )
{

GetLocalTime( &t );
t.wMilliseconds = 0;

}

/***********************************************************************
*                               IS_TIME                                *
***********************************************************************/
BOOLEAN is_time( TCHAR * s )
{
int32   i;
int32   slen;
TCHAR * cp;
TCHAR   c;

slen = lstrlen( s );
if ( slen < 1 )
    return FALSE;

/*
----------------------------------
Only numeric and ':' chars allowed
---------------------------------- */
cp = s;
while ( TRUE )
    {
    c = *cp;
    if ( c == NullChar )
        break;

    if ( !is_numeric(c) && c != ColonChar )
        return FALSE;

    cp++;
    }

/*
------------------------------------------
Check that the first field is a valid hour
------------------------------------------ */
i = asctoint32( s );
if ( i > 23 )
    return FALSE;

if ( slen > 2 )
    {
    /*
    --------------------------------------------------
    There must be a ':' if there are more than 2 chars
    -------------------------------------------------- */
    cp = findchar( ColonChar, s );
    if ( !cp )
        return FALSE;

    if ( (cp-s) > 2 )
        return FALSE;

    /*
    -----------------------
    Go to the minutes field
    ----------------------- */
    s = cp + 1;

    /*
    ----------------------------------------
    Check that this is a valid minutes field
    ---------------------------------------- */
    i = asctoint32( s );
    if ( i > 59 )
        return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                                EXTHMS                                *
*                               HH:MM:SS                               *
***********************************************************************/
BOOLEAN exthms( SYSTEMTIME & t, const TCHAR * s )
{
const TCHAR * cp;

t.wHour         = extshort( s, 2 );
t.wMinute       = 0;
t.wSecond       = 0;
t.wMilliseconds = 0;

cp = findchar( ColonChar, s );
if ( cp )
    {
    s = cp + 1;
    t.wMinute = extshort( s, 2 );
    cp = findchar( ColonChar, s );
    if ( cp )
        {
        s = cp + 1;
        t.wSecond = extshort( s, 2 );
        }
    }

return TRUE;
}

/***********************************************************************
*                                EXTMDY                                *
*                              MM/DD/YYYY                              *
***********************************************************************/
BOOLEAN extmdy( SYSTEMTIME & t, const TCHAR * s )
{
int32 i;
TCHAR c;

t.wMonth = extshort( s, 2 );

for ( i=0; i<3; i++ )
    {
    c = *s;
    s++;
    if ( c == SlashChar )
        break;
    }

t.wDay  = extshort( s, 2 );

for ( i=0; i<3; i++ )
    {
    c = *s;
    s++;
    if ( c == SlashChar )
        break;
    }

t.wYear = extshort( s, 4 );

t.wDayOfWeek = day_of_the_week( (int) t.wMonth, (int) t.wDay, (int) t.wYear );

return TRUE;
}

/***********************************************************************
*                               EXTMMDDYY                              *
*                               MM/DD/YY                               *
***********************************************************************/
BOOLEAN extmmddyy( SYSTEMTIME & t, const TCHAR * s )
{

t.wMonth = extshort( s, 2 );

s += 3;
t.wDay  = extshort( s, 2 );

s += 3;
t.wYear = extshort( s, 2 );

if ( t.wYear < 50 )
    t.wYear += 2000;
else
    t.wYear += 1900;

t.wDayOfWeek = day_of_the_week( (int) t.wMonth, (int) t.wDay, (int) t.wYear );

return TRUE;
}

/***********************************************************************
*                              ALPHADATE                               *
*                              MM/DD/YYYY                              *
***********************************************************************/
TCHAR * alphadate( const SYSTEMTIME & t )
{

static TCHAR s[ALPHADATE_LEN+1];
TCHAR * cp;

cp = s;

insalph( cp, t.wMonth,  2, TEXT('0'), DECIMAL_RADIX );
cp += 2;
*cp++ = TEXT( '/' );

insalph( cp, t.wDay,  2, TEXT('0'), DECIMAL_RADIX );
cp += 2;
*cp++ = TEXT( '/' );

insalph( cp, t.wYear, 4, TEXT('0'), DECIMAL_RADIX );
*(s+ALPHADATE_LEN) = TEXT('\0');

return s;
}

/***********************************************************************
*                              ALPHATIME                               *
*                               HH:MM:SS                               *
***********************************************************************/
TCHAR * alphatime( const SYSTEMTIME & t )
{

static TCHAR s[ALPHATIME_LEN+1];
TCHAR * cp;

cp = s;

insalph( cp, t.wHour,  2, TEXT('0'), DECIMAL_RADIX );
cp += 2;
*cp++ = ColonChar;

insalph( cp, t.wMinute,  2, TEXT('0'), DECIMAL_RADIX );
cp += 2;
*cp++ = ColonChar;

insalph( cp, t.wSecond, 2, TEXT('0'), DECIMAL_RADIX );

*(s+ALPHATIME_LEN) = NullChar;

return s;
}

/***********************************************************************
*                           FIX_ALPHATIME                              *
*                             HH:MM:SS                                 *
* This takes a string (assumed to be at long enough for HH:MM:SS and   *
* fixes it to fit the pattern, inserting zeros as necessary.           *
***********************************************************************/
void fix_alphatime( TCHAR * sorc )
{
SYSTEMTIME t;

exthms( t, sorc );
lstrcpy( sorc, alphatime(t) );
}

/***********************************************************************
*                                HHMM                                  *
*                               HH:MM                                  *
***********************************************************************/
TCHAR * hhmm( int32 hours, int32 minutes )
{
static TCHAR s[ALPHATIME_LEN+1];
TCHAR * cp;

cp = s;

insalph( cp, hours, 2, ZeroChar, DECIMAL_RADIX );
cp += 2;
*cp++ = ColonChar;

insalph( cp, minutes, 2, ZeroChar, DECIMAL_RADIX );
cp += 2;

*cp = NullChar;

return s;
}

/***********************************************************************
*                             IS_LEAP_YEAR                             *
***********************************************************************/
BOOLEAN is_leap_year( WORD year )
{

if ( year%4 == 0 && year%100 != 0 )
    return TRUE;

if ( year%400 == 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            DAYS_IN_MONTH                             *
*                 Jan=1  Dec=12     Year = 2004, 2005                  *
***********************************************************************/
WORD days_in_month( WORD month, WORD year )
{

int leap;
int i;

static WORD days[2][12] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };

if ( is_leap_year(year) )
    leap = 1;
else
    leap = 0;

/*
-----------------
Months begin at 1
----------------- */
i = month - 1;

return days[leap][i];
}

/***********************************************************************
*                            DAYS_IN_MONTH                             *
***********************************************************************/
WORD days_in_month( SYSTEMTIME & st )
{
return days_in_month( st.wMonth, st.wYear );
}

/***********************************************************************
*                                DOFYR                                 *
*                                                                      *
*   day    = [1,31]    return = [0,364] regular                        *
*   month  = [1,12]             [0,365] leap                           *
*   year  >= 1950                                                      *
***********************************************************************/
WORD dofyr( WORD month, WORD day, WORD year )
{

int leap;
int i;

static WORD days[2][11] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30 }  }; /* Dec not needed */

if ( is_leap_year(year) )
    leap = 1;
else
    leap = 0;

/*
-------------------------------
Both days and months begin at 1
------------------------------- */
month--;
day--;

if ( month > 11 )
    month = 11;

for ( i=0; i<month; i++ )
    day += days[leap][i];

return day;
}

/***********************************************************************
*                                DOFYR                                 *
***********************************************************************/
WORD dofyr( const SYSTEMTIME & st )
{
return dofyr( st.wMonth, st.wDay, st.wYear );
}

/***********************************************************************
*                               SAMEDATE                               *
***********************************************************************/
BOOLEAN samedate( const SYSTEMTIME & s1, const SYSTEMTIME & s2 )
{

if ( s1.wYear != s2.wYear )
    return FALSE;

if ( s1.wMonth != s2.wMonth )
    return FALSE;

if ( s1.wDay != s2.wDay )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            SEC_SINCE1990                             *
***********************************************************************/
long sec_since1990( SYSTEMTIME & t )
{

long seconds;
WORD y;

seconds = 0L;

for ( y=1990; y<t.wYear; y++ )
    {
    seconds += SEC_PER_YEAR;
    if ( is_leap_year(y) )
        seconds += SEC_PER_DAY;
    }

seconds += long(dofyr(t.wMonth, t.wDay, t.wYear)) * SEC_PER_DAY;
seconds += long(t.wHour) * SEC_PER_HOUR;
seconds += long(t.wMinute) * SEC_PER_MINUTE;
seconds += long(t.wSecond);

return seconds;
}

/***********************************************************************
*                        CURRENT_SEC_SINCE1990                         *
***********************************************************************/
long current_sec_since1990( void )
{
SYSTEMTIME t;

GetLocalTime( &t );
return sec_since1990( t );
}

/***********************************************************************
*                          SECONDS_DIFFERENCE                          *
***********************************************************************/
int32 seconds_difference( SYSTEMTIME & t1, SYSTEMTIME & t2 )
{
long s1;
long s2;
s1 = sec_since1990( t1 );
s2 = sec_since1990( t2 );

return int32(s1-s2);
}

/***********************************************************************
*                          SECONDS_DIFFERENCE                          *
***********************************************************************/
int32 seconds_difference( FILETIME & t1, FILETIME & t2 )
{

SYSTEMTIME s1;
SYSTEMTIME s2;
BOOL status;

status = FileTimeToSystemTime( &t1, &s1 );
if ( status )
    status = FileTimeToSystemTime( &t2, &s2 );

if ( status )
   {
   return seconds_difference( s1, s2 );
   };

return 0;
}
