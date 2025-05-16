#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static const TCHAR BackSlashChar = TEXT('\\');
static const TCHAR NullChar      = TEXT('\0');
static const TCHAR ColonChar     = TEXT(':');
static TCHAR DriveName[3];

/***********************************************************************
*                          GET_DRIVE_NAME                              *
***********************************************************************/
BOOLEAN get_drive_name( TCHAR * path )
{

DriveName[0] = NullChar;

/*
--------------------------
Null strings are not valid
-------------------------- */
if ( !path )
    return FALSE;

if ( path[0] == NullChar )
    return FALSE;

DriveName[0] = path[0];
DriveName[1] = path[1];
DriveName[2] = NullChar;

return TRUE;
}

/***********************************************************************
*                             DRIVE_EXISTS                             *
***********************************************************************/
BOOLEAN drive_exists( TCHAR * path )
{
UINT  drive_type;

if ( path[1] != ColonChar )
    return FALSE;

if ( !get_drive_name(path) )
    return FALSE;

drive_type = GetDriveType( DriveName );

/*
---------------------------
1 => root dir doesn't exist
--------------------------- */
if ( drive_type == 1 )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                             IS_REMOVABLE                             *
***********************************************************************/
BOOLEAN is_removable( TCHAR * path )
{
UINT  drive_type;

if ( path[1] != ColonChar )
    return FALSE;

if ( !get_drive_name(path) )
    return FALSE;

drive_type = GetDriveType( DriveName );

if ( drive_type == DRIVE_REMOVABLE )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           DISK_IS_IN_DRIVE                           *
***********************************************************************/
BOOLEAN disk_is_in_drive( TCHAR * path )
{
DWORD status;

if ( is_removable(path) )
    {
    SetErrorMode( SEM_FAILCRITICALERRORS );
    status = GetFileAttributes( DriveName );
    SetErrorMode( 0 );
    if( status == 0xFFFFFFFF )
        return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                            FILE_TYPE                                 *
*                 0 not found, 1 normal, 2 directory                   *
***********************************************************************/
static int file_type( TCHAR * path )
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

/***********************************************************************
*                            FILE_EXISTS                               *
***********************************************************************/
BOOLEAN file_exists( TCHAR * path )
{
if ( file_type(path) == 1 )
    return TRUE;
return FALSE;
}

/***********************************************************************
*                          DIRECTORY_EXISTS                            *
***********************************************************************/
BOOLEAN directory_exists( TCHAR * path )
{
if ( disk_is_in_drive(path) )
    {
    if ( file_type(path) == 2 )
        return TRUE;
    }

return FALSE;
}


/***********************************************************************
*                              FILE_SIZE                               *
***********************************************************************/
int32 file_size( TCHAR * path )
{
WIN32_FIND_DATA fdata;
HANDLE          fh;
int32           size;

fh = FindFirstFile( path, &fdata );
FindClose( fh );

if ( fh != INVALID_HANDLE_VALUE )
    size = (int32) fdata.nFileSizeLow;
else
    size = -1;

return size;
}

