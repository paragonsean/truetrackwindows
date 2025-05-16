#include <windows.h>

#include "..\include\strarray.h"
void remove_char( TCHAR * sorc );
void lower_case( TCHAR * s );
void upper_case( TCHAR * s );

TCHAR * findstring( const TCHAR * s_to_find, TCHAR * s_to_search );

static TCHAR NullChar = TEXT( '\0' );
static TCHAR NineChar = TEXT( '9' );
static TCHAR ZeroChar = TEXT( '0' );
TCHAR STRING_ARRAY_CLASS::emptystring[] = TEXT( "" );

/***********************************************************************
*                               FINDCHAR                               *
***********************************************************************/
static bool findchar( TCHAR c, const TCHAR * sorc )
{

while ( *sorc != NullChar )
    {
    if ( *sorc == c )
        return true;

    sorc++;
    }

return false;
}

/***********************************************************************
*                           STRING_ENTRY                               *
*                               EMPTY                                  *
***********************************************************************/
void STRING_ENTRY::empty( void )
{
if ( s )
    {
    delete [] s;
    s = 0;
    }
maxlen = 0;
}

/***********************************************************************
*                           STRING_ENTRY                               *
*                          ~STRING_ENTRY                               *
***********************************************************************/
STRING_ENTRY::~STRING_ENTRY()
{
empty();
}

/***********************************************************************
*                           STRING_ARRAY_CLASS                         *
*                               EMPTY                                  *
***********************************************************************/
void STRING_ARRAY_CLASS::empty( void )
{
current->empty();
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                                FIND                                  *
***********************************************************************/
bool STRING_ARRAY_CLASS::find( TCHAR * sorc )
{
STRING_ENTRY * se;

if ( !sorc )
    return false;

se = current;
while ( true )
    {
    if ( lstrcmpi(current->s, sorc) == 0 )
        return true;

    if ( !next() )
        break;
    }

rewind();
while ( next() )
    {
    if ( current == se )
        break;

    if ( lstrcmpi(current->s, sorc) == 0 )
        return true;
    }

return false;
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                                FIND                                  *
***********************************************************************/
bool STRING_ARRAY_CLASS::find( int index_to_find )
{
if ( index_to_find < current_index )
    rewind();

if ( rewound )
    next();

do  {
    if ( current_index == index_to_find )
        return true;

    } while ( next() );

return false;
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                                PREV                                  *
***********************************************************************/
bool STRING_ARRAY_CLASS::prev( void )
{
int i;
i = current_index;
if ( i > 0 )
    {
    i--;
    return find( i );
    }

return false;
}

/***********************************************************************
*                             INSERT                                   *
***********************************************************************/
bool STRING_ARRAY_CLASS::insert( const TCHAR * sorc )
{
STRING_ENTRY * se;

/*
-----------------------------------------------------------
Add a string if this is not a new, empty STRING_ARRAY_CLASS
----------------------------------------------------------- */
if ( (current != first) || current->next || current->s )
    {
    se = new STRING_ENTRY;
    if ( !se )
        return false;

    se->next = current;

    if ( first == current )
        {
        first = se;
        current = se;
        current_index = 0;
        n++;
        }
    else
        {
        if ( prev() )
            {
            current->next = se;
            n++;
            next();
            }
        else
            {
            delete se;
            return false;
            }
        }
    }

operator=( sorc );
return true;
}

/***********************************************************************
*                             INSERT                                   *
***********************************************************************/
bool STRING_ARRAY_CLASS::insert( const STRING_ARRAY_CLASS & sorc )
{
return insert( sorc.current->s );
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                                 ADD                                  *
***********************************************************************/
bool STRING_ARRAY_CLASS::add( void )
{
while ( next() )
    ;

current->next = new STRING_ENTRY;
if ( next() )
    {
    n++;
    return true;
    }

return false;
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                                 ADD                                  *
***********************************************************************/
bool STRING_ARRAY_CLASS::add( const TCHAR * sorc )
{
bool status;

status = false;

/*
-------------------------------------------------------
Don't add a string if this is a new, empty STRING_ARRAY_CLASS
------------------------------------------------------- */
if ( (current == first) && !current->next && !current->s )
    status = true;

if ( !status )
    status = add();

if ( status )
    {
    operator=( sorc );
    return true;
    }

return false;
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                                 ADD                                  *
***********************************************************************/
bool STRING_ARRAY_CLASS::add( const STRING_ARRAY_CLASS & sorc )
{
return add( sorc.current->s );
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                                 LEN                                  *
***********************************************************************/
int STRING_ARRAY_CLASS::len()
{
if ( current->s )
    return (int) lstrlen(current->s);

return 0;
}

/***********************************************************************
*                                 ISZERO                               *
***********************************************************************/
bool STRING_ARRAY_CLASS::iszero()
{
const TCHAR badchars[] = TEXT("123456789");
const TCHAR okchars[]  = TEXT(" +-.");

TCHAR * cp;
bool    found_zero_char = false;

if ( !current->s )
    return false;

cp = current->s;
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
*                             IS_NUMERIC                               *
***********************************************************************/
bool STRING_ARRAY_CLASS::is_numeric()
{
const TCHAR okchars[] = TEXT(" +-.0123456789");
TCHAR * cp;
TCHAR   c;
bool has_number;

has_number = false;

if ( isempty() )
    return false;

cp = current->s;
while ( *cp != NullChar )
    {
    c = *cp;
    if ( !findchar(c, okchars) )
        return false;
    if ( c >= ZeroChar && c <= NineChar )
        has_number = true;
    cp++;
    }

return has_number;
}

/***********************************************************************
*                               ISEMPTY                                *
***********************************************************************/
bool STRING_ARRAY_CLASS::isempty()
{
if ( current->s )
    {
    if ( *current->s == NullChar )
        return true;
    return false;
    }
return true;
}

/***********************************************************************
*                                 CONTAINS                             *
***********************************************************************/
bool STRING_ARRAY_CLASS::contains( TCHAR c )
{
TCHAR * cp;

if ( !current->s )
    return false;

cp = current->s;
while ( *cp != NullChar )
    {
    if ( *cp == c )
        return true;
    cp++;
    }

return false;
}

/***********************************************************************
*                           STRING_ARRAY_CLASS                         *
*                             contains                                 *
***********************************************************************/
bool STRING_ARRAY_CLASS::contains( const TCHAR * s_to_find )
{
if ( s_to_find && current->s )
    {
    if ( findstring(s_to_find, current->s) )
        return true;
    }

return false;
}

/***********************************************************************
*                                 UPPERCASE                            *
***********************************************************************/
void STRING_ARRAY_CLASS::uppercase()
{
if ( current->s )
    upper_case( current->s );
}

/***********************************************************************
*                                 LOWERCASE                            *
***********************************************************************/
void STRING_ARRAY_CLASS::lowercase()
{
if ( current->s )
    lower_case( current->s );
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
***********************************************************************/
STRING_ARRAY_CLASS::STRING_ARRAY_CLASS()
{
first = current = new STRING_ENTRY;
current_index = 0;
n             = 1;
rewound       = true;
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
***********************************************************************/
STRING_ARRAY_CLASS::STRING_ARRAY_CLASS( const TCHAR * sorc )
{
first = current = new STRING_ENTRY;
current_index = 0;
n             = 1;
rewound       = true;
operator=( sorc );
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
***********************************************************************/
STRING_ARRAY_CLASS::STRING_ARRAY_CLASS( const STRING_ARRAY_CLASS & sorc )
{
first = current = new STRING_ENTRY;
current_index = 0;
n             = 1;
rewound       = true;
operator=( sorc );
}

/***********************************************************************
*                           ~STRING_ARRAY_CLASS                        *
***********************************************************************/
STRING_ARRAY_CLASS::~STRING_ARRAY_CLASS()
{
while ( first )
    {
    current = first;
    first   = first->next;
    delete current;
    }

n             = 0;
current       = 0;
current_index = 0;
rewound       = true;
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                                 INIT                                 *
***********************************************************************/
bool STRING_ARRAY_CLASS::init( int new_len )
{
TCHAR * cp;

current->empty();

if ( new_len <= 0 )
    return false;

current->s = new TCHAR[new_len+1];
if ( current->s )
    {
    current->maxlen = new_len;

    cp = current->s + new_len;
    while ( true )
        {
        *cp = NullChar;
        if ( cp == current->s )
            break;
        cp--;
        }

    return true;
    }

current->maxlen = 0;
return false;
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                                NEXT                                  *
***********************************************************************/
bool STRING_ARRAY_CLASS::next( void )
{
if ( rewound )
    {
    rewound = false;
    return true;
    }

if ( current->next )
    {
    current = current->next;
    current_index++;
    return true;
    }

return false;
}

/***********************************************************************
*                               REMOVE_ALL                             *
***********************************************************************/
void STRING_ARRAY_CLASS::remove_all()
{
STRING_ENTRY * se;
STRING_ENTRY * copy;

rewind();

n  = 1;
se = first->next;
first->next = 0;

while ( se )
    {
    copy = se;
    se = se->next;
    delete copy;
    }

first->empty();
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                               REMOVE                                 *
***********************************************************************/
bool STRING_ARRAY_CLASS::remove( void )
{
STRING_ENTRY * se;
STRING_ENTRY * prev;

/*
---------------------------
Don't delete the only entry
--------------------------- */
if ( !first->next )
    return false;

prev = 0;
se   = first;

while ( se )
    {
    if ( se == current )
        {
        se = current->next;

        if ( prev )
            {
            prev->next = se;
            if ( !se )
                {
                se = prev;
                current_index--;
                }
            }
        else
            {
            first = se;
            }

        delete current;
        current = se;
        n--;
        return true;
        }

    prev = se;
    se   = se->next;
    }

return false;
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                              OPERATOR[]                              *
***********************************************************************/
STRING_ARRAY_CLASS & STRING_ARRAY_CLASS::operator[]( int i )
{

find( i );

return *this;
}

/***********************************************************************
*                            STRING_ARRAY_CLASS                        *
*                               OPERATOR=                              *
***********************************************************************/
STRING_ARRAY_CLASS & STRING_ARRAY_CLASS::operator=( const TCHAR * sorc )
{
int nc;

if ( sorc == current->s )
    return (*this);

nc = 0;
if ( sorc )
    nc = lstrlen( sorc );

if ( nc <= 0 )
    {
    if ( current->s )
        *(current->s) = NullChar;
    }
else
    {
    if ( nc > current->maxlen )
        {
        if ( !init(nc) )
            return (*this);
        }
    lstrcpy( current->s, sorc );
    }

return (*this);
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
STRING_ARRAY_CLASS & STRING_ARRAY_CLASS::operator=( const STRING_ARRAY_CLASS & sorc )
{
return operator=( sorc.current->s );
}


/***********************************************************************
*                                 JOIN                                 *
***********************************************************************/
bool STRING_ARRAY_CLASS::join( const TCHAR * separator )
{
TCHAR        * cp;
STRING_ENTRY * se;
int nc;

nc = 0;
if ( separator )
    {
    nc = lstrlen( separator );
    if ( nc > 0 )
        nc *= count();
    else
        separator = 0;
    }

rewind();
while ( next() )
    nc += len();

if ( nc <= 0 )
    return false;

rewind();
next();

cp = current->s;

current->s = 0;
current->maxlen = 0;
init( nc );

if ( cp )
    {
    operator=( cp );
    delete[] cp;
    cp = 0;
    }

se = current->next;
while ( se )
    {
    operator+=(separator);
    operator+=(se->s);
    se = se->next;
    }

/*
--------------------------
Remove all the old entries
-------------------------- */
next();
while ( remove() )
    ;

return true;
}

/***********************************************************************
*                                 SPLIT                                *
***********************************************************************/
bool STRING_ARRAY_CLASS::split( const TCHAR * separator )
{

TCHAR * copy;
TCHAR * cp;
TCHAR * sp;
int slen;

if ( !current->s )
    return false;

if ( !separator )
    return false;

slen = lstrlen( separator );
if ( !slen )
    return false;

copy = current->s;
current->s = 0;
current->empty();

cp = copy;
while ( cp )
    {
    sp = findstring( separator, cp );
    if ( sp )
        {
        *sp = NullChar;
        add( cp );
        cp = sp + slen;
        }
    else
        {
        if ( *cp != NullChar )
            add( cp );
        break;
        }
    }
delete[] copy;

return true;
}

/***********************************************************************
*                              OPERATOR==                              *
***********************************************************************/
bool STRING_ARRAY_CLASS::operator==( const STRING_ARRAY_CLASS & sorc )
{

if ( !current->s )
    return false;

if ( !sorc.current->s )
    return false;

if ( lstrcmpi(current->s, sorc.current->s) == 0 )
    return true;

return false;
}

/***********************************************************************
*                              OPERATOR==                              *
***********************************************************************/
bool STRING_ARRAY_CLASS::operator==( const TCHAR * sorc )
{

if ( !current->s )
    return false;

if ( !sorc )
    return false;

if ( lstrcmpi(current->s, sorc) == 0 )
    return true;

return false;
}

/***********************************************************************
*                                  +=                                  *
***********************************************************************/
void STRING_ARRAY_CLASS::operator+=( const TCHAR * sorc )
{
int     nc;
TCHAR * dest;

if ( !sorc )
    return;

nc  = len();
nc += lstrlen( sorc );

if ( nc < 1 )
    return;

if ( nc > current->maxlen )
    {
    dest = new TCHAR[nc+1];
    if ( dest )
        {
        lstrcpy( dest, current->s );
        current->empty();
        current->s = dest;
        current->maxlen = nc;
        }
    else
        {
        return;
        }
    }

lstrcat( current->s, sorc );
}

/***********************************************************************
*                               cat_w_char                             *
***********************************************************************/
void STRING_ARRAY_CLASS::cat_w_char( const TCHAR * sorc, TCHAR c )
{
int   nc;
TCHAR * dest;

if ( !sorc )
    return;

nc = len();
nc++;
nc += lstrlen( sorc );

if ( nc < 1 )
    return;

if ( nc > current->maxlen )
    {
    dest = new TCHAR[nc+1];
    if ( dest )
        {
        lstrcpy( dest, current->s );
        current->empty();
        current->s = dest;
        current->maxlen = nc;
        }
    else
        {
        return;
        }
    }

nc = len();
dest = current->s;
dest += nc;
*dest = c;
dest++;
lstrcpy( dest, sorc );
}

/***********************************************************************
*                               cat_w_char                             *
***********************************************************************/
void STRING_ARRAY_CLASS::cat_w_char( const STRING_ARRAY_CLASS & sorc, TCHAR c )
{
cat_w_char( sorc.current->s, c );
}

/***********************************************************************
*                               cat_path                               *
***********************************************************************/
void STRING_ARRAY_CLASS::cat_path( const TCHAR * sorc )
{
const TCHAR backslash = TEXT( '\\' );

int   nc;
TCHAR * cp;

cp = current->s;
if ( !cp )
    return;

nc = lstrlen(cp);
if ( nc < 1 )
    return;
nc--;
cp += nc;

if ( *cp == backslash || *sorc == backslash )
    operator+=( sorc );
else
    cat_w_char( sorc, backslash );
}

/***********************************************************************
*                               cat_path                               *
***********************************************************************/
void STRING_ARRAY_CLASS::cat_path( const STRING_ARRAY_CLASS & sorc )
{
cat_path( sorc.current->s );
}

/***********************************************************************
*                                REVERSE                               *
***********************************************************************/
void STRING_ARRAY_CLASS::reverse( void )
{
TCHAR   c;
TCHAR * p1;
TCHAR * p2;

if ( !current->s )
    return;

p1 = current->s;
p2 = p1 + lstrlen(p1) - 1;
while ( p1 < p2 )
    {
    c = *p2;
    *p2-- = *p1;
    *p1++ = c;
    }
}

/***********************************************************************
*                                   +=                                 *
***********************************************************************/
void STRING_ARRAY_CLASS::operator+=( const STRING_ARRAY_CLASS & sorc )
{
operator+=( sorc.current->s );
}

#ifndef _CONSOLE

/***********************************************************************
*                                GETTEXT                               *
*                      Get the text from a control                     *
***********************************************************************/
bool STRING_ARRAY_CLASS::gettext( HWND w )
{
int nc;

nc = SendMessage( w, WM_GETTEXTLENGTH, 0, 0 );

if ( nc <= 0 )
    {
    current->empty();
    return true;
    }

if ( nc > current->maxlen )
    {
    if ( !init(nc) )
        return false;
    }

nc = SendMessage( w, WM_GETTEXT, nc+1, (LPARAM) current->s );
if ( nc > 0 )
    return true;

return false;
}

/***********************************************************************
*                                SETTEXT                               *
*                       Send the text to a control                     *
***********************************************************************/
bool STRING_ARRAY_CLASS::settext( HWND w )
{
TCHAR * cp;

if ( current->s )
    cp = current->s;
else
    cp = emptystring;

if ( SendMessage(w, WM_SETTEXT,  0, (LPARAM) cp) )
    return true;

return false;
}
#endif

/***********************************************************************
*                                 TRIM                                 *
*       Trim characters in charlist from the end of a string.          *
***********************************************************************/
void STRING_ARRAY_CLASS::trim( TCHAR * charlist )
{
TCHAR * cp;
TCHAR * bad;

cp = current->s;
if ( !cp )
    return;

while ( *cp != NullChar )
    cp++;

while ( *cp == NullChar && cp > current->s )
    {
    cp--;
    bad = charlist;
    while ( *bad )
        {
        if ( *cp == *bad )
            {
            *cp = NullChar;
            break;
            }
        bad++;
        }
    }
}

/***********************************************************************
*                                STRIP                                 *
*      Trim characters in charlist from both ends of a string.         *
***********************************************************************/
void STRING_ARRAY_CLASS::strip( TCHAR * charlist )
{
TCHAR * cp;
TCHAR * bad;
bool    have_bad;

cp = current->s;
if ( !cp )
    return;

have_bad = true;
while ( have_bad && (*cp != NullChar) )
    {
    bad = charlist;
    have_bad = false;
    while ( *bad )
        {
        if ( *cp == *bad )
            {
            remove_char( cp );
            have_bad = true;
            break;
            }
        bad++;
        }
    }

trim( charlist );
}


