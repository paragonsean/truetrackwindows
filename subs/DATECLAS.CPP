#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

const static TCHAR NullChar = TEXT( '\0' );

class DATE_CLASS
    {
    private:

    SYSTEMTIME st;

    TCHAR      s[ALPHADATE_LEN+1];

    public:

    DATE_CLASS();
    WORD         year( void )  { return st.wYear;  }
    WORD         month( void ) { return st.wMonth; }
    WORD         day( void )   { return st.wDay;   }
    WORD         dow( void )   { return st.wDayOfWeek; }
    TCHAR      * text( void )  { return s; }
    void operator=( const DATE_CLASS & sorc );
    void operator=( const SYSTEMTIME & sorc );
    BOOLEAN      operator==( const DATE_CLASS & sorc );

    SYSTEMTIME * system_time( void );
    };

/***********************************************************************
*                               DATE_CLASS                             *
***********************************************************************/
DATE_CLASS::DATE_CLASS()
{
st.wYear         = 1601;
st.wMonth        = 1;
st.wDay          = 1;
st.wHour         = 0;
st.wMinute       = 0;
st.wSecond       = 0;
st.wMilliseconds = 0;
st.wDayOfWeek    = 0;
*s               = NullChar;
}

/***********************************************************************
*                              OPERATOR=                               *
***********************************************************************/
void DATE_CLASS::operator=( const SYSTEMTIME & sorc )
{
st.wYear         = sorc.wYear;
st.wMonth        = sorc.wMonth;
st.wDay          = sorc.wDay;
st.wHour         = 0;
st.wMinute       = 0;
st.wSecond       = 0;
st.wMilliseconds = 0;
st.wDayOfWeek    = day_of_the_week( (int) st.wMonth, (int) st.wDay, (int) st.wYear );

lstrcpy( s, alphadate(st) );
}

/***********************************************************************
*                              OPERATOR=                               *
***********************************************************************/
void DATE_CLASS::operator=( const DATE_CLASS & sorc )
{
operator=( sorc.st );
}

/***********************************************************************
*                              OPERATOR==                              *
***********************************************************************/
BOOLEAN DATE_CLASS::operator==( const DATE_CLASS & sorc )
{

if ( st.wYear != sorc.st.wYear )
    return FALSE;

if ( st.wMonth != sorc.st.wMonth )
    return FALSE;

if ( st.wDay != sorc.st.wDay )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                              SYSTEM_TIME                             *
***********************************************************************/
SYSTEMTIME * DATE_CLASS::system_time( void )
{
return &st;
}
