#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

const TCHAR BackSlashChar     = TEXT( '\\' );
const TCHAR NullChar          = TEXT( '\0' );

/***********************************************************************
*                            FILE_TYPE                                 *
*                 0 not found, 1 normal, 2 directory                   *
***********************************************************************/
static int my_file_type( TCHAR * path )
{

DWORD   file_attributes;
int     i;
TCHAR * s;
BOOLEAN need_delete;

i = lstrlen( path );
i--;
if ( i < 1 )
    return FALSE;

if ( path[i] == BackSlashChar )
    {
    s = new TCHAR[i+1];
    if ( !s )
        return FALSE;
    copychars( s, path, i );
    *(s+i) = NullChar;
    need_delete = TRUE;
    }
else
    {
    s = path;
    need_delete = FALSE;
    }

file_attributes = GetFileAttributes( s );
if ( need_delete )
    delete[] s;

if ( file_attributes == 0xFFFFFFFF )
    return 0;

if ( file_attributes & FILE_ATTRIBUTE_DIRECTORY )
    return 2;


return 1;
}
