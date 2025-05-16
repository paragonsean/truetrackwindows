#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static TCHAR NullChar      = TEXT('\0' );
static TCHAR BackSlashChar = TEXT('\\');
static TCHAR ColonChar     = TEXT(':');

static TCHAR BackSlashString[] = TEXT( "\\" );
static TCHAR ExeString[]       = TEXT(".EXE");
static int   ExeStringLength = 4;

/***********************************************************************
*                       PATH_IS_DRIVE_ONLY                             *
*                                                                      *
*  Return TRUE if this is just the root directory of a drive.          *
***********************************************************************/
BOOLEAN path_is_drive_only( TCHAR * s )
{
int slen;

slen = lstrlen( s );
if ( slen < 2 || slen > 3 )
    return FALSE;

if ( IsCharAlpha(s[0]) )
    if ( s[1] == ColonChar )
        if ( s[2] == BackSlashChar || s[2] == NullChar )
            return TRUE;

return FALSE;
}

/***********************************************************************
*                       APPEND_BACKSLASH_TO_PATH                       *
*  Only if the last char is not a backslash or if the string is empty  *
***********************************************************************/
void append_backslash_to_path( TCHAR * s )
{
int n;

n = lstrlen( s );
if ( n > 0 )
    n--;

s += n;
if ( *s != BackSlashChar )
    {
    s++;
    *s = BackSlashChar;
    s++;
    *s = NullChar;
    }
}

/***********************************************************************
*                       APPEND_DIRECTORY_TO_PATH                       *
***********************************************************************/
void append_directory_to_path( TCHAR * dest, const TCHAR * sorc )
{
append_backslash_to_path( dest );
lstrcat( dest, sorc );
append_backslash_to_path( dest );
}

/***********************************************************************
*                       APPEND_FILENAME_TO_PATH                        *
***********************************************************************/
void append_filename_to_path( TCHAR * dest, const TCHAR * sorc )
{
append_backslash_to_path( dest );
lstrcat( dest, sorc );
}

/***********************************************************************
*                              CATPATH                                 *
* This is like append_filename_to_path but returns a pointer to the    *
* null character at the end of the resulting string;                   *
* If dest points to a null character I assume this points to the end   *
* of a string and I back up one character to check for the backslash.  *
***********************************************************************/
TCHAR * catpath( TCHAR * dest, const TCHAR * sorc )
{
if ( *dest == NullChar )
    dest--;
else
    dest += lstrlen(dest) - 1;

if ( *dest != BackSlashChar )
    {
    dest++;
    *dest = BackSlashChar;
    }
dest++;
while ( *sorc != NullChar )
    *dest++ = *sorc++;

*dest = NullChar;
return dest;
}

/***********************************************************************
*                      REMOVE_BACKSLASH_FROM_PATH                      *
***********************************************************************/
void remove_backslash_from_path( TCHAR * s )
{
int n;

if ( !s )
    return;

n = lstrlen( s );
if ( n > 0 )
    n--;

s += n;
if ( *s == BackSlashChar )
    *s = NullChar;

}

/***********************************************************************
*                          DIR_FROM_FILENAME                           *
***********************************************************************/
BOOLEAN dir_from_filename( TCHAR * s )
{
int     n;
TCHAR * cp;

if ( !s )
    return FALSE;

n = lstrlen( s );
n--;
if ( n <= 0 )
    return FALSE;

cp = s+n;

while ( TRUE )
    {
    if ( *cp == BackSlashChar || *cp == ColonChar )
        {
        if ( *cp == ColonChar )
            cp++;
        break;
        }

    if ( cp == s )
        return FALSE;

    cp--;
    }

*cp = NullChar;
return TRUE;
}

/***********************************************************************
*                        DIR_FROM_COMMAND_LINE                         *
* This assumes the file you are executing ends in exe and everything   *
* thereafter is extranious.                                            *
***********************************************************************/
BOOLEAN dir_from_command_line( TCHAR * s )
{
TCHAR * cp;
cp = findstring( ExeString, s );
if ( cp )
    {
    cp += ExeStringLength;
    *cp = NullChar;
    }

return dir_from_filename( s );
}

/***********************************************************************
*                           FILENAME_FROM_PATH                         *
* This returns a pointer to the location in the sorc string where the  *
* filename begins.                                                     *
***********************************************************************/
TCHAR * filename_from_path( TCHAR * path )
{
TCHAR * cp;

cp = path + lstrlen(path);
while ( cp > path )
    {
    if ( *cp == BackSlashChar )
        {
        cp++;
        break;
        }
    cp--;
    }

return cp;
}

/***********************************************************************
*                           CREATE_DIRECTORY                           *
***********************************************************************/
BOOLEAN create_directory( TCHAR * path )
{
BOOLEAN status;
TCHAR * s;
TCHAR * cp;
TCHAR * sp;
int   nof_dir_levels;

if ( !path )
    return FALSE;

if ( lstrlen(path) < 1 )
    return FALSE;

s = maketext( path );
if ( !s )
    return FALSE;

remove_backslash_from_path( s );
if ( directory_exists(path) )
    {
    delete[] s;
    return TRUE;
    }

cp = s;
if ( *cp == BackSlashChar )
    {
    while ( *cp == BackSlashChar )
        cp++;

    while ( *cp != NullChar && *cp != BackSlashChar )
        cp++;

    if ( *cp == BackSlashChar )
        {
        cp++;
        }
    else
        {
        /*
        --------------------------------------------------------------------
        Network names like \\gp6 are not directorys. There must be some name
        like \\gp6\v5.
        -------------------------------------------------------------------- */
        delete[] s;
        return FALSE;
        }
    }
else
    {
    if ( *(cp+1) == ColonChar && *(cp+2) == BackSlashChar )
        cp += 3;
    }

/*
-----------------------
Remember where to start
----------------------- */
sp = cp;

/*
--------------------------
Count the directory levels
-------------------------- */
nof_dir_levels = 0;
while ( *cp != NullChar )
    {
    if ( *cp == BackSlashChar )
        nof_dir_levels++;
    cp++;
    }

status = TRUE;

/*
---------------------------------------------------------
Check each level and try to create it if it doesn't exist
--------------------------------------------------------- */
cp = sp;
while ( nof_dir_levels > 0 )
    {
    replace_char_with_null( cp, BackSlashString );
    if ( !directory_exists(s) )
        {
        if ( !CreateDirectory( s, NULL ) )
            {
            status = FALSE;
            break;
            }
        }

    /*
    -----------------------------------
    Put the backslash back where it was
    ----------------------------------- */
    while ( *cp != NullChar )
        cp++;
    *cp = BackSlashChar;

    cp++;
    nof_dir_levels--;
    }

/*
---------------------------------------------
There should always be one more dir to create
--------------------------------------------- */
if ( *cp != NullChar )
    {
    if ( !CreateDirectory( s, NULL ) )
        status = FALSE;
    }

delete[] s;

return status;
}

/***********************************************************************
*                               EXECUTE                                *
***********************************************************************/
BOOLEAN execute( TCHAR * commandline )
{
TCHAR * dp;

dp = maketext( commandline );
if ( dp )
    {
    dir_from_command_line( dp );
    return execute_and_wait( commandline, dp, (DWORD) 0 );
    delete[] dp;
    }
return FALSE;
}
