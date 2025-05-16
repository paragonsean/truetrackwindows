#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"

static const TCHAR NullChar        = TEXT( '\0' );
static const TCHAR BackSlashChar   = TEXT( '\\' );
static const TCHAR ColonChar       = TEXT( ':' );
static TCHAR DefaultDirectory[]    = TEXT( "C:\\" );

/***********************************************************************
*                            GET_FILE_LIST                             *
***********************************************************************/
void get_file_list( TEXT_LIST_CLASS & dest, const TCHAR * rootdir, const TCHAR * file_name_pattern )
{
TCHAR s[MAX_PATH_NAME_LEN+1];
HANDLE fh;
WIN32_FIND_DATA fdata;

dest.empty();

lstrcpy( s, rootdir );
append_filename_to_path( s, file_name_pattern );

fh = FindFirstFile( s, &fdata );
if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( TRUE )
        {
        if ( !(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            dest.append( fdata.cFileName );

        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );
    }
}

/***********************************************************************
*                                TOUNI                                 *
***********************************************************************/
bool touni( TCHAR * file_to_convert )
{
DWORD   access_mode;
DWORD   share_mode;
DWORD   file_size;
DWORD   new_file_size;
DWORD   bytes_read;
DWORD   bytes_written;
TCHAR * buf;
TCHAR * sorc;
TCHAR * dest;
HANDLE  fh;

share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
access_mode = GENERIC_READ;

fh = CreateFile( file_to_convert, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh == INVALID_HANDLE_VALUE )
    return false;

file_size = GetFileSize( fh, 0 );
if ( file_size == 0 )
    {
    CloseHandle( fh );
    return true;
    }

new_file_size = file_size * 2;

buf = new TCHAR[new_file_size];
if ( !buf )
    {
    CloseHandle( fh );
    return false;
    }

if ( ReadFile( fh, buf, file_size, &bytes_read, 0)  )
    {
    dest = buf + new_file_size;
    sorc = buf + file_size;
    while ( sorc > buf )
        {
        dest--;
        *dest = NullChar;
        dest--;
        sorc--;
        *dest = *sorc;
        }

    CloseHandle( fh );

    share_mode  = 0;
    access_mode = GENERIC_WRITE;

    fh = CreateFile( file_to_convert, access_mode, share_mode, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
    if ( fh == INVALID_HANDLE_VALUE )
        return false;

    WriteFile( fh, buf, new_file_size, &bytes_written, 0 );
    CloseHandle( fh );
    fh = INVALID_HANDLE_VALUE;
    if ( new_file_size == bytes_written )
        return true;
    }
else
    {
    CloseHandle( fh );
    }

return false;
}

/***********************************************************************
*                         CONVERT_ONE_PATTERN                          *
***********************************************************************/
void convert_one_pattern( TCHAR * pattern )
{

TEXT_LIST_CLASS fname;

// char  buf[MAX_PATH];
//  DWORD buf_size = MAX_PATH;


get_file_list( fname, ".", pattern );

fname.rewind();
while ( fname.next() )
    {
    cprintf( "\n\r%s...", fname.text() );
    touni( fname.text() );
    }
}

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
TCHAR * getline( void )
{
static TCHAR buf[103];
buf[0] = 100;

cgets( buf );

return buf+2;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
TCHAR * cp;

// char  buf[MAX_PATH];
//  DWORD buf_size = MAX_PATH;

convert_one_pattern( "*.TXT" );
convert_one_pattern( "*.DAT" );
convert_one_pattern( "*.MSG" );
convert_one_pattern( "*.PRG" );

cprintf( "\n\rPress enter to exit " );
cp = getline();

/*
#define ERROR_FILE_NOT_FOUND             2L
#define ERROR_PATH_NOT_FOUND             3L

cp = getline();
if ( cp )
    {
    if ( lstrlen(cp) > 0 )
        {
        SchemaFile = cp;
        SchemaFile.cat_path( SchemaName );

        get_file_list( cp );

        cprintf( "\n\r" );
        }
    }
*/
return 0;
}

