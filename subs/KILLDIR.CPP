#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static TCHAR StarDotStar[] = TEXT( "*.*" );

/***********************************************************************
*                           EMPTY_DIRECTORY                            *
***********************************************************************/
BOOLEAN empty_directory( TCHAR * dirname, TCHAR * fname )
{
TCHAR s[MAX_PATH+1];
HANDLE fh;
WIN32_FIND_DATA fdata;
BOOLEAN status;
TCHAR * cp;

status = TRUE;

lstrcpy( s, dirname );
append_backslash_to_path( s );
cp = s + lstrlen(s);

lstrcat( s, fname );

fh = FindFirstFile( s, &fdata );

if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( TRUE )
        {
        if ( *fdata.cFileName != TEXT('.') )
            {
            lstrcpy( cp, fdata.cFileName );
            if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                status = kill_directory( s );
            else
                status = DeleteFile( s );

            if ( !status )
                break;
            }

        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );
    }

return status;
}

/***********************************************************************
*                           EMPTY_DIRECTORY                            *
***********************************************************************/
BOOLEAN empty_directory( TCHAR * dirname )
{
return empty_directory( dirname, StarDotStar );
}

/***********************************************************************
*                            KILL_DIRECTORY                            *
***********************************************************************/
BOOLEAN kill_directory( TCHAR * dirname )
{
TCHAR s[MAX_PATH+1];

if ( empty_directory(dirname, StarDotStar) )
    {
    lstrcpy( s, dirname );
    remove_backslash_from_path( s );
    RemoveDirectory( s );
    return TRUE;
    }

return FALSE;
}

