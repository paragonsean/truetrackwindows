#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

/***********************************************************************
*                         GET_FILE_WRITE_TIME                          *
***********************************************************************/
BOOLEAN get_file_write_time( SYSTEMTIME & t, TCHAR * fname )
{
WIN32_FIND_DATA fdata;
HANDLE          fh;

fh = FindFirstFile( fname, &fdata );

if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;

FileTimeToSystemTime( &fdata.ftLastWriteTime, &t );
FindClose( fh );

return TRUE;
}
