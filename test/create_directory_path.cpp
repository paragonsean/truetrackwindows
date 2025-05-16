#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\listbox.h"
#include "..\include\subs.h"

const TCHAR BackSlashChar     = TEXT( '\\' );
const TCHAR BackSlashString[] = TEXT( "\\" );
const TCHAR ColonChar         = TEXT( ':' );
const TCHAR NullChar          = TEXT( '\0' );

/***********************************************************************
*                        CREATE_DIRECTORY_PATH                         *
***********************************************************************/
bool create_directory_path( TCHAR * path, LISTBOX_CLASS & lb )
{
bool status;
TCHAR * s;
TCHAR * cp;
TCHAR * sp;
int   nof_dir_levels;

if ( !path )
    return false;

if ( lstrlen(path) < 1 )
    return false;

s = maketext( path );
if ( !s )
    return false;

remove_backslash_from_path( s );

cp = s;
if ( *cp == BackSlashChar )
    {
    while ( *cp == BackSlashChar )
        cp++;
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

status = true;

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
        /*
        if ( !CreateDirectory( s, NULL ) )
            {
            status = false;
            break;
            }
        */


        lb.add( s );
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

lb.add( s );

delete[] s;

return status;
}
