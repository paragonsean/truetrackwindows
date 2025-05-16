#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

static TCHAR BackSlashChar     = TEXT( '\\' );
static TCHAR CommaChar         = TEXT( ',' );
static TCHAR CrChar            = TEXT( '\r' );
static TCHAR LfChar            = TEXT( '\n' );
static TCHAR NullChar          = TEXT( '\0' );
static TCHAR SpaceChar         = TEXT( ' ' );
static TCHAR ZeroChar          = TEXT( '0' );

static TCHAR BackSlashString[] = TEXT( "\\" );
static TCHAR FalseString[]     = TEXT( "N" );
static TCHAR TrueString[]      = TEXT( "Y" );

TCHAR STRING_CLASS::emptystring[] = TEXT( "" );

/***********************************************************************
*                               EMPTY                                  *
***********************************************************************/
void STRING_CLASS::empty()
{
if ( s )
    {
    delete[] s;
    s      = 0;
    maxlen = 0;
    }
}

/***********************************************************************
*                                NULL                                  *
***********************************************************************/
void STRING_CLASS::null()
{
if ( s )
    *s = NullChar;
}

/***********************************************************************
*                              ACQUIRE                                 *
* slen is the max length not counting the NULL character at the end    *
***********************************************************************/
BOOLEAN STRING_CLASS::acquire( TCHAR * sorc, int slen )
{
empty();

if ( sorc )
    {
    maxlen = slen;
    if ( maxlen > 0 )
        {
        s = sorc;
        *(s+maxlen) = NullChar;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                              ACQUIRE                                 *
***********************************************************************/
BOOLEAN STRING_CLASS::acquire( TCHAR * sorc )
{
empty();

if ( sorc )
    {
    maxlen = lstrlen( sorc );
    if ( maxlen > 0 )
        {
        s = sorc;
        return TRUE;
        }
    }

maxlen = 0;
return FALSE;
}

/***********************************************************************
*                              GET_ASCII                               *
*            If this is not unicode this is the same as =              *
***********************************************************************/
BOOLEAN STRING_CLASS::get_ascii( char * sorc )
{
const char AsciiNull = '\0';

union {
char  c;
TCHAR t;
};

TCHAR * cp;
char  * ap;
int n;

/*
----------------------------------
Get the length of the ascii string
---------------------------------- */
n = 0;
ap = sorc;
while ( *ap != AsciiNull )
    {
    ap++;
    n++;
    }

/*
----------------
Convert to TCHAR
---------------- */
if ( n > 0 )
    {
    if ( init(n) )
        {
        cp = s;
        while ( true )
            {
            t = NullChar;
            c = *sorc;
            *cp = t;
            if (*sorc == AsciiNull )
                break;
            cp++;
            sorc++;
            }
        return TRUE;
        }
    }
return FALSE;
}

/***********************************************************************
*                              RELEASE                                 *
* This is used to give my current buffer to someone else, who is       *
* then responsible for deleting it.                                    *
***********************************************************************/
TCHAR * STRING_CLASS::release()
{
TCHAR * p;

p      = s;
s      = 0;
maxlen = 0;

return p;
}

/***********************************************************************
*                                 LEN                                  *
***********************************************************************/
int32 STRING_CLASS::len()
{
if ( s )
    {
    return (int32) lstrlen(s);
    }

return 0;
}

/***********************************************************************
*                            STRING_CLASS                              *
***********************************************************************/
STRING_CLASS::STRING_CLASS()
{
s      = 0;
maxlen = 0;
}

/***********************************************************************
*                            STRING_CLASS                              *
***********************************************************************/
STRING_CLASS::STRING_CLASS( const TCHAR * sorc )
{
s      = 0;
maxlen = 0;
operator=( sorc );
}

/***********************************************************************
*                            STRING_CLASS                              *
***********************************************************************/
STRING_CLASS::STRING_CLASS( const STRING_CLASS & sorc )
{
s      = 0;
maxlen = 0;
operator=( sorc );
}

/***********************************************************************
*                           ~STRING_CLASS                              *
***********************************************************************/
STRING_CLASS::~STRING_CLASS()
{
empty();
}

/***********************************************************************
*                              NEXT_FIELD                              *
***********************************************************************/
bool STRING_CLASS::next_field( TCHAR separator )
{
TCHAR * cp;
TCHAR * dest;
TCHAR * ep;

if ( !s )
    return false;

/*
---------------------------------------------------------------------
If I find a separator then this is the first time I have been called.
--------------------------------------------------------------------- */
cp = s;
while ( *cp != NullChar )
    {
    if ( *cp == separator )
        break;
    cp++;
    }

if ( *cp == separator )
    {
    /*
    -------------------------------
    End the string at the separator
    ------------------------------- */
    *cp = NullChar;

    /*
    -------------------------------------------------------------
    Make sure that everything after the end of the string is null
    ------------------------------------------------------------- */
    ep = s + maxlen;
    *ep = NullChar;

    cp++;
    while ( *cp != NullChar )
        cp++;

    while ( cp < ep )
        {
        *cp = NullChar;
        cp++;
        }
    return true;
    }
else
    {
    ep = s + maxlen;
    if ( cp < ep )
        {
        cp++;
        if ( *cp != NullChar && cp < ep )
            {
            /*
            -------------------------------------------------
            Copy the string beginning at cp down to the start
            ------------------------------------------------- */
            dest = s;
            while ( cp <= ep )
                *dest++ = *cp++;
            while ( dest < ep )
                *dest++ = NullChar;
            /*
            --------------------------------
            If there is a separator, null it
            -------------------------------- */
            cp = s;
            while ( cp < ep )
                {
                if ( *cp == separator )
                    *cp = NullChar;
                if ( *cp == NullChar )
                    break;
                cp++;
                }
            return true;
            }
        }
    }
return false;
}

/***********************************************************************
*                              NEXT_FIELD                              *
***********************************************************************/
bool STRING_CLASS::next_field()
{
return next_field( CommaChar );
}

/***********************************************************************
*                             STRING_CLASS                             *
*                               GET_TEXT                               *
*                      Read the text from a control                    *
***********************************************************************/
bool STRING_CLASS::get_text( HWND w )
{
int32 n;

n = GetWindowTextLength( w );

if ( n <= 0 )
    {
    if ( s )
        *s = NullChar;
    return true;
    }

if ( n > maxlen )
    {
    if ( !init(n) )
        return false;
    }

return ::get_text(s, w, n) != 0 ;
}

/***********************************************************************
*                             STRING_CLASS                             *
*                               SET_TEXT                               *
*                       Send the text to a control                     *
***********************************************************************/
bool STRING_CLASS::set_text( HWND w )
{
TCHAR * cp;
if ( s )
    cp = s;
else
    cp = emptystring;

return ::set_text(w, cp) != 0;
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOLEAN STRING_CLASS::init( int32 new_len )
{
empty();

if ( new_len <= 0 )
    return FALSE;

s = new TCHAR[new_len+1];
if ( s )
    {
    maxlen = new_len;
    new_len++;
    new_len *= sizeof(TCHAR);
    memset( s, 0, new_len );
    return TRUE;
    }

maxlen = 0;
return FALSE;
}

/***********************************************************************
*                             BOOLEAN_VALUE                            *
***********************************************************************/
BOOLEAN STRING_CLASS::boolean_value()
{
if ( s )
    {
    if ( *s == TrueString[0] )
        return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( BOOLEAN sorc )
{
if ( sorc )
    operator=( TrueString );
else
    operator=( FalseString );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( int n )
{
if ( maxlen < MAX_INTEGER_LEN )
    {
    if ( !init(MAX_INTEGER_LEN) )
        return;
    }

int32toasc( s, (int32) n, DECIMAL_RADIX );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( unsigned int u )
{
if ( maxlen < MAX_INTEGER_LEN )
    {
    if ( !init(MAX_INTEGER_LEN) )
        return;
    }

ultoascii( s, u, DECIMAL_RADIX );
}

/***********************************************************************
*                                    =                                 *
***********************************************************************/
void STRING_CLASS::operator=( double d )
{
if ( maxlen < MAX_DOUBLE_LEN )
    {
    if ( !init(MAX_DOUBLE_LEN) )
        return;
    }

rounded_double_to_tchar( s, d );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( TCHAR sorc )
{
if ( s )
    *s = NullChar;

operator+=( sorc );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( const TCHAR * sorc )
{
int32 n;

if ( sorc == s )
    return;

n = 0;
if ( sorc )
    n = lstrlen( sorc );

if ( n <= 0 )
    {
    /*
    -------------------------------------------
    Don't delete the string, just make it len 0
    ------------------------------------------- */
    if ( s )
        *s = NullChar;
    return;
    }

if ( n > maxlen )
    {
    if ( !init(n) )
        return;
    }

lstrcpy( s, sorc );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( const STRING_CLASS & sorc )
{
operator=( sorc.s );
}

/***********************************************************************
*                               COMPARE                                *
*         -1 if I am < sorc, 0 if equal, +1 if I am > sorc             *
**********************************************************************/
int STRING_CLASS::compare( TCHAR * sorc )
{
if ( !s && !sorc )
    return 0;

if ( !s )
    return -1;

if ( !sorc )
    return 1;

return (int) ::compare( s, sorc );
}

/***********************************************************************
*                              OPERATOR<                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator<( const TCHAR * sorc )
{

if ( !s )
    return FALSE;

if ( !sorc )
    return FALSE;

if ( lstrcmpi(s, sorc) < 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR<                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator<( const STRING_CLASS & sorc )
{

if ( !s )
    return FALSE;

if ( !sorc.s )
    return FALSE;

if ( lstrcmpi(s, sorc.s) < 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR<                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator>( const TCHAR * sorc )
{

if ( !s )
    return FALSE;

if ( !sorc )
    return FALSE;

if ( lstrcmpi(s, sorc) > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR<                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator>( const STRING_CLASS & sorc )
{

if ( !s )
    return FALSE;

if ( !sorc.s )
    return FALSE;

if ( lstrcmpi(s, sorc.s) > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR==                              *
***********************************************************************/
BOOLEAN STRING_CLASS::operator==( const STRING_CLASS & sorc )
{
if ( !s )
    return FALSE;

if ( !sorc.s )
    return FALSE;

if ( lstrcmpi(s, sorc.s) == 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR==                              *
***********************************************************************/
BOOLEAN STRING_CLASS::operator==( const TCHAR * sorc )
{

if ( !s )
    return FALSE;

if ( !sorc )
    return FALSE;

if ( lstrcmpi(s, sorc) == 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                                 upsize                               *
* This function checks to see if s is big enough. If not, a new        *
* string is allocated and the old one copied to it. This is only       *
* used to make a string bigger. False means the allocation failed.     *
***********************************************************************/
BOOLEAN STRING_CLASS::upsize( int32 new_len )
{
TCHAR * dest;

if ( new_len <= maxlen )
    return TRUE;

new_len++;

dest = new TCHAR[new_len];
if ( !dest )
    return FALSE;

memset( dest, 0, new_len*sizeof(TCHAR) );

new_len--;

if ( s )
    {
    lstrcpy( dest, s );
    empty();
    }

s      = dest;
maxlen = new_len;

return TRUE;
}

/***********************************************************************
*                               cat_w_char                             *
*             Append a string followed with a character                *
***********************************************************************/
void STRING_CLASS::cat_w_char( const TCHAR * sorc, TCHAR c )
{
int32   n;
TCHAR * dest;

if ( !sorc )
    return;

n = len();
n++;
n += lstrlen( sorc );

if ( n < 1 )
    return;

if ( upsize(n) )
    {
    lstrcat( s, sorc );

    dest  = s + n;
    *dest = NullChar;
    dest--;
    *dest = c;
    }
}

/***********************************************************************
*                               cat_path                               *
***********************************************************************/
void STRING_CLASS::cat_path( const TCHAR * sorc )
{
int32   n;
int32   sorc_len;
TCHAR * cp;
bool    need_backslash;

if ( !sorc )
    return;

sorc_len = lstrlen( sorc );
if ( sorc_len < 1 )
    return;

n = len();
if ( n > 0 )
    {
    /*
    -----------------------------------------------------
    Point cp at the last character of the existing string
    ----------------------------------------------------- */
    cp = s;
    cp += n;
    cp--;

    need_backslash = ( (*cp != BackSlashChar) && (*sorc != BackSlashChar) );

    n += sorc_len;
    if ( need_backslash )
        n++;
    upsize( n );
    if ( need_backslash )
        operator+=( BackSlashString );
    }

operator+=( sorc );
}

/***********************************************************************
*                                  +=                                  *
***********************************************************************/
void STRING_CLASS::operator+=( const TCHAR sorc )
{
int32 n;

n = len();
n++;

if ( upsize(n) )
    {
    n--;
    s[n] = sorc;
    n++;
    s[n] = NullChar;
    }
}

/***********************************************************************
*                                  +=                                  *
***********************************************************************/
void STRING_CLASS::operator+=( const TCHAR * sorc )
{
int32   n;

if ( !sorc )
    return;

n  = len();
n += lstrlen( sorc );

if ( n < 1 )
    return;

if ( upsize(n) )
    lstrcat( s, sorc );
}

/***********************************************************************
*                                   +=                                 *
***********************************************************************/
void STRING_CLASS::operator+=( const STRING_CLASS & sorc )
{
operator+=( sorc.s );
}

/***********************************************************************
*                                   +=                                 *
***********************************************************************/
void STRING_CLASS::operator+=( BOOLEAN sorc )
{
if ( sorc )
    operator+=( TrueString );
else
    operator+=( FalseString );
}

/***********************************************************************
*                                    +=                                *
***********************************************************************/
void STRING_CLASS::operator+=( int n )
{
int slen;
int newlen;

slen   = len();
newlen = slen;
newlen += MAX_INTEGER_LEN;

if ( newlen > maxlen )
    {
    if ( !upsize(newlen) )
        return;
    }

int32toasc( s+slen, (int32) n, DECIMAL_RADIX );
}

/***********************************************************************
*                                    +=                                *
***********************************************************************/
void STRING_CLASS::operator+=( double d )
{
int slen;
int newlen;

slen   = len();
newlen = slen;
newlen += MAX_DOUBLE_LEN;

if ( newlen > maxlen )
    {
    if ( !upsize(newlen) )
        return;
    }

rounded_double_to_tchar( s+slen, d );
}

/***********************************************************************
*                                    +=                                *
***********************************************************************/
void STRING_CLASS::operator+=( unsigned int u )
{
int slen;
int newlen;

slen   = len();
newlen = slen;
newlen += MAX_INTEGER_LEN;

if ( newlen > maxlen )
    {
    if ( !upsize(newlen) )
        return;
    }
ultoascii( s+slen, u, DECIMAL_RADIX );
}

/***********************************************************************
*                                ISEMPTY                               *
***********************************************************************/
BOOLEAN STRING_CLASS::isempty()
{
if ( !s )
    return TRUE;

if ( *s == NullChar )
    return TRUE;

return is_empty( s );
}

/***********************************************************************
*                                 ISZERO                               *
***********************************************************************/
bool STRING_CLASS::iszero()
{
const TCHAR badchars[] = TEXT("123456789");
const TCHAR okchars[]  = TEXT(" +-.");

TCHAR * cp;
bool    found_zero_char = false;

if ( !s )
    return false;

cp = s;
while ( *cp != NullChar )
    {
    if ( *cp == ZeroChar )
        found_zero_char = true;
    else if ( findchar(*cp, badchars) )
        return false;
    else if ( !findchar(*cp, okchars) )
        break;
    cp++;
    }

return found_zero_char;
}

/***********************************************************************
*                              REAL_VALUE                              *
* I use strtod in extdouble so I have to make sure there are no alpha  *
* characters.                                                          *
***********************************************************************/
double STRING_CLASS::real_value()
{
double  d;
int     n;
TCHAR * cp;

d = 0.0;

if ( s )
    {
    n = 0;
    cp = s;
    while ( *cp != NullChar )
        {
        if ( !is_real_char(*cp) )
            break;
        n++;
        cp++;
        }
    if ( n )
        d =  extdouble(s, n);
    }

return d;
}

/***********************************************************************
*                                  FIND                                *
***********************************************************************/
TCHAR * STRING_CLASS::find( TCHAR c )
{
TCHAR * cp;

if ( s )
    {
    cp = s;
    while ( *cp != NullChar )
        {
        if ( *cp == c )
            return cp;
        cp++;
        }
    }

return 0;
}

/***********************************************************************
*                           STRING_CLASS                               *
*                               find                                   *
***********************************************************************/
TCHAR * STRING_CLASS::find( const TCHAR * s_to_find )
{
TCHAR * cp;

cp = 0;
if ( s_to_find && s )
    cp = findstring( s_to_find, s );

return cp;
}

/***********************************************************************
*                                 CONTAINS                             *
***********************************************************************/
bool STRING_CLASS::contains( TCHAR c )
{
if ( find(c) )
    return true;

return false;
}

/***********************************************************************
*                                 UPPERCASE                            *
***********************************************************************/
void STRING_CLASS::uppercase()
{
if ( s )
    upper_case( s );
}

/***********************************************************************
*                                 LOWERCASE                            *
***********************************************************************/
void STRING_CLASS::lowercase()
{
if ( s )
    lower_case( s );
}

/***********************************************************************
*                           STRING_CLASS                               *
*                             contains                                 *
***********************************************************************/
bool STRING_CLASS::contains( const TCHAR * s_to_find )
{
if ( find(s_to_find) )
    return true;

return false;
}

/***********************************************************************
*                               REPLACE                                *
*        Returns TRUE if the text was found and replaced.              *
*        FALSE means the text was not found.                           *
***********************************************************************/
BOOLEAN STRING_CLASS::replace( const TCHAR * oldtext, const TCHAR * newtext )
{
int32 delta;
int32 old_len;
int32 new_len;
int32 slen;
TCHAR * cp;

if ( !s )
    return FALSE;

cp = findstring( oldtext, s );
if ( !cp )
    return FALSE;

old_len = lstrlen( oldtext );
new_len = lstrlen( newtext );

delta = new_len - old_len;
if ( delta > 0 )
    {
    slen = len() + delta;
    if ( slen > maxlen )
        {
        if ( !upsize( slen) )
            return FALSE;
        cp = findstring( oldtext, s );
        if ( !cp )
            return FALSE;
        }
    move_string_forward( cp, delta );
    }
else if ( delta < 0 )
    {
    copystring( cp+new_len, cp+old_len );
    }

copychars( cp, newtext, new_len );
return TRUE;
}

/***********************************************************************
*                              STRIP_CRLF                              *
*         Strip any cr or lf chars from the end of the string          *
***********************************************************************/
void STRING_CLASS::strip_crlf()
{
TCHAR * cp;
int     n;

if ( !s )
    return;

n = len();
if ( n < 1 )
    return;

cp = s + n -1;

while ( true )
    {
    if ( (*cp != CrChar) && (*cp != LfChar) )
        break;

    *cp = NullChar;
    if ( cp == s )
        break;

    cp--;
    }
}

/***********************************************************************
*                        NULL_FIRST_CONTROL_CHAR                       *
* Replace the first control char with a null, else null *(s + maxlen). *
***********************************************************************/
void STRING_CLASS::null_first_control_char()
{
TCHAR * cp;
TCHAR * ep;

if ( !s )
    return;

cp = s;
ep = s + maxlen;

while ( cp < ep )
    {
    if ( *cp < SpaceChar )
        break;
    cp++;
    }

*cp = NullChar;
}

/***********************************************************************
*                                 STRIP                                *
*         Strip spaces and control chars from both ends.               *
***********************************************************************/
void STRING_CLASS::strip()
{
TCHAR * cp;
TCHAR * ep;
int     n;

if ( !s )
    return;

n = len();
if ( n < 1 )
    return;

cp = s + n -1;

while ( true )
    {
    if ( (*cp > SpaceChar) )
        break;

    *cp = NullChar;
    if ( cp == s )
        return;

    cp--;
    }

ep = cp;
while ( cp > s )
    {
    cp--;
    if ( *cp > SpaceChar )
        ep = cp;
    }

/*
----------------------------------------
Ep points to the first non-control char.
---------------------------------------- */
if ( cp < ep )
    {
    while ( true )
        {
        *cp = *ep;
        if ( *ep == NullChar )
            break;
        cp++;
        ep++;
        }
    }
}

/***********************************************************************
*                                 rjust                                *
*  Right justify the string padding on the left with the passed char.  *
***********************************************************************/
BOOLEAN STRING_CLASS::rjust( int32 new_len, TCHAR c )
{
int32 current_len;
TCHAR * sorc;
TCHAR * dest;

current_len = len();
if ( current_len >= new_len )
    return TRUE;

if ( !upsize(new_len) )
    return FALSE;

/*
-----------------------
Move the string forward
----------------------- */
sorc = s + current_len;
dest = s + new_len;
while ( true )
    {
    *dest = *sorc;
    if ( sorc == s )
        break;
    sorc--;
    dest--;
    }

/*
--------------------------------------
Fill the start with the fill character
-------------------------------------- */
while ( dest > s )
    {
    dest--;
    *dest = c;
    }

return TRUE;
}

/***********************************************************************
*                                 rjust                                *
***********************************************************************/
BOOLEAN STRING_CLASS::rjust( int32 new_len )
{
return rjust( new_len, SpaceChar );
}
