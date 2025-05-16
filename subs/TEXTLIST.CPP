#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"

static TCHAR DotChar = TEXT( '.' );
static TCHAR NullChar = TEXT( '\0' );
static TCHAR StarDotStar[] = TEXT( "*.*" );

/***********************************************************************
*                            TEXT_LIST_CLASS                           *
***********************************************************************/
TEXT_LIST_CLASS::TEXT_LIST_CLASS( void )
{
current_entry = 0;
}

/***********************************************************************
*                                EMPTY                                 *
***********************************************************************/
void TEXT_LIST_CLASS::empty( void )
{
TCHAR * cp;

current_entry = 0;
tlist.rewind();
while ( TRUE )
    {
    cp = (TCHAR *) tlist.next();
    if ( !cp )
        break;
    delete[] cp;
    }

tlist.remove_all();
}

/***********************************************************************
*                                 NEXT                                 *
***********************************************************************/
BOOLEAN TEXT_LIST_CLASS::next( void )
{
current_entry = (TCHAR *) tlist.next();

if ( current_entry )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                                REMOVE                                *
***********************************************************************/
BOOLEAN TEXT_LIST_CLASS::remove( void )
{
if ( current_entry )
    {
    delete [] current_entry;
    current_entry = 0;
    tlist.remove();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               RELEASE                                *
*  This is like remove except the string is passed to the caller       *
*  who is responsible for deleting it.                                 *
***********************************************************************/
TCHAR * TEXT_LIST_CLASS::release()
{
TCHAR * ptr;

ptr = 0;
if ( current_entry )
    {
    ptr = current_entry;
    current_entry = 0;
    tlist.remove();
    }

return ptr;
}

/***********************************************************************
*                                 SORT                                 *
***********************************************************************/
BOOLEAN TEXT_LIST_CLASS::sort( void )
{
BOOLEAN status;

status = tlist.sort( compare_text );
rewind();

return status;
}

/***********************************************************************
*                        TEXT_LIST_CLASS::FIND                         *
***********************************************************************/
BOOLEAN TEXT_LIST_CLASS::find( TCHAR * s, int32 slen )
{

rewind();
while ( next() )
    {
    if ( compare(current_entry, s, slen) == 0 )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      TEXT_LIST_CLASS::FIND_EXACT                     *
***********************************************************************/
BOOLEAN TEXT_LIST_CLASS::find_exact( TCHAR * s )
{
if ( !s )
    return FALSE;

rewind();
while ( next() )
    {
    if ( compare(current_entry, s) == 0 )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               APPEND                                 *
***********************************************************************/
BOOLEAN TEXT_LIST_CLASS::append( TCHAR * sorc, int32 slen )
{
TCHAR * cp;
int     n;

if ( slen <= 0 )
    return FALSE;

/*
--------------------------------
Point slen at the last character
-------------------------------- */
slen--;

cp = sorc;
n  = 0;

/*
-------------------------------------------
Make n point to the last non-null character
------------------------------------------- */
while ( TRUE )
    {
    if ( *cp == NullChar )
        {
        n--;
        break;
        }

    if ( n == slen )
        break;

    n++;
    cp++;
    }

if ( n >= 0 )
    {
    n++;
    /*
    --------------------------------------------------
    N is now the number of non-null characters to copy
    -------------------------------------------------- */
    cp = maketext( n );
    if ( cp )
        {
        copychars( cp, sorc, n );
        *(cp+n) = NullChar;
        if ( tlist.append( cp ) )
            return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                                  =                                   *
***********************************************************************/
void TEXT_LIST_CLASS::operator=( TEXT_LIST_CLASS & sorc )
{

empty();
if ( sorc.count() > 0 )
    {
    sorc.rewind();
    while ( sorc.next() )
        append( sorc.text() );
    }
}

/***********************************************************************
*                           ~TEXT_LIST_CLASS                           *
***********************************************************************/
TEXT_LIST_CLASS::~TEXT_LIST_CLASS( void )
{
empty();
}

/***********************************************************************
*                                 SPLIT                                *
***********************************************************************/
bool TEXT_LIST_CLASS::split( const TCHAR * sorc, const TCHAR * separator )
{
TCHAR * copy;
TCHAR * cp;
TCHAR * sp;
int slen;

if ( !sorc )
    return false;

if ( !separator )
    return false;

slen = lstrlen( separator );
if ( !slen )
    return false;

empty();

copy = maketext( sorc );
cp = copy;
while ( cp )
    {
    sp = findstring( separator, cp );
    if ( sp )
        {
        *sp = NullChar;
        append( cp );
        cp = sp + slen;
        }
    else
        {
        if ( *cp != NullChar )
            append( cp );
        break;
        }
    }

if ( copy )
    delete[] copy;

return true;
}

/***********************************************************************
*                            GET_FILE_LIST                             *
***********************************************************************/
void TEXT_LIST_CLASS::get_file_list( const TCHAR * pattern_path )
{
HANDLE fh;
WIN32_FIND_DATA fdata;

fh = FindFirstFile( pattern_path, &fdata );
if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( TRUE )
        {
        if ( !(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            append( fdata.cFileName );

        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );
    }
}

/***********************************************************************
*                            GET_FILE_LIST                             *
***********************************************************************/
void TEXT_LIST_CLASS::get_file_list( const TCHAR * rootdir, const TCHAR * file_name_pattern )
{
TCHAR s[MAX_PATH+1];

lstrcpy( s, rootdir );
append_filename_to_path( s, file_name_pattern );

get_file_list( s );
}

/***********************************************************************
*                          GET_DIRECTORY_LIST                          *
***********************************************************************/
void TEXT_LIST_CLASS::get_directory_list( const TCHAR * rootdir )
{
TCHAR s[MAX_PATH+1];
HANDLE fh;
WIN32_FIND_DATA fdata;

lstrcpy( s, rootdir );
append_filename_to_path( s, StarDotStar );


fh = FindFirstFile( s, &fdata );
if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( TRUE )
        {
        if ( (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            {
            if ( *fdata.cFileName != DotChar )
                append( fdata.cFileName );
            }
        if ( !FindNextFile(fh, &fdata) )
            break;

        }

    FindClose( fh );
    }
}
