#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static const TCHAR NullChar        = TEXT( '\0' );

/***********************************************************************
*                            ASCII_TO_UNICODE                          *
*   This assumes you are in ascii mode and want to convert an ascii    *
*   string into a unicode string. The unicode buffer is assumed to be  *
*   twice as long as the ascii buffer.                                 *
*                                                                      *
*   It is ok for the dest and sorc to be the same address as I copy    *
*   the last byte first.                                               *
***********************************************************************/
bool ascii_to_unicode( char * dest, char * sorc, unsigned int slen )
{
unsigned int dlen;

if ( !dest )
    return false;

if ( !sorc )
    return false;

dlen = slen * 2;

dest += dlen;
sorc += slen;

while ( slen )
    {
    dest--;
    *dest = NullChar;
    dest--;
    sorc--;
    *dest = *sorc;
    slen--;
    }

return true;
}

/***********************************************************************
*                                TOUNI                                 *
***********************************************************************/
bool touni( const TCHAR * file_to_convert )
{
DWORD   access_mode;
DWORD   share_mode;
DWORD   file_size;
DWORD   new_file_size;
DWORD   bytes_read;
DWORD   bytes_written;
TCHAR * buf;
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
    ascii_to_unicode( buf, buf, file_size );

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

delete[] buf;
return false;
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

cprintf( "\n\rThis program converts ascii files to unicode (REPLACING the original file." );

while ( 1 )
    {
    cprintf( "\n\rInput file to convert: " );
    cp = getline();
    if ( (*cp) == NullChar )
        break;

    cprintf( "Converting [%s]...", cp );
    touni( cp );
    cprintf( "\n\r" );
    }

return 0;
}


