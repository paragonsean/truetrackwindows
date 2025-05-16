#include <windows.h>
#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"

static TCHAR CrLf[]   = TEXT("\015\012");
static TCHAR LfChar   = TEXT('\012');
static TCHAR CrChar   = TEXT('\015');
static TCHAR NullChar = TEXT('\0');

/***********************************************************************
*                           CLEANUP_MEMORY                             *
***********************************************************************/
void FILE_CLASS::cleanup_memory()
{
if ( path )
    {
    delete[] path;
    path = 0;
    }

if ( buf )
    {
    delete[] buf;
    buf    = 0;
    buflen = 0;
    }
}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void FILE_CLASS::cleanup()
{
if ( fh != INVALID_HANDLE_VALUE )
    {
    CloseHandle( fh );
    fh = INVALID_HANDLE_VALUE;
    }

cleanup_memory();

last_status = FILE_NOT_OPEN;
last_error  = ERROR_SUCCESS;
current_position = 0;
}

/***********************************************************************
*                               FILE_CLASS                             *
***********************************************************************/
FILE_CLASS::FILE_CLASS()
{
fh          = INVALID_HANDLE_VALUE;
path        = 0;
buf         = 0;
buflen      = 0;
access_mode = 0;
share_mode  = 0;
current_position = 0;
last_status = FILE_NOT_OPEN;
last_error  = ERROR_SUCCESS;
}

/***********************************************************************
*                              ~FILE_CLASS                             *
***********************************************************************/
FILE_CLASS::~FILE_CLASS()
{
cleanup();
}

/***********************************************************************
*                                MAKE_PATH                             *
***********************************************************************/
BOOLEAN FILE_CLASS::make_path( TCHAR * sorc )
{
int32 n;

cleanup();

n = lstrlen( sorc );
if ( n < 1 )
    return FALSE;

path = maketext( n );
if ( !path )
    {
    last_status = NO_PATH_MEMORY;
    last_error  = GetLastError();
    return FALSE;
    }

lstrcpy( path, sorc );
return TRUE;
}

/***********************************************************************
*                                 OPEN                                 *
***********************************************************************/
BOOLEAN FILE_CLASS::open( TCHAR * sorc, DWORD create_flags )
{
const DWORD OPEN_WAIT_MS       = 10;  /* MS to wait between open tries */
const int   OPEN_TIMEOUT_COUNT = 10;  /* Max tries to open file  */
const int   LAST_TIMEOUT_TO_DOUBLE = 7; /* => double three times 20, 40, 80ms */
int time_out;
DWORD ms;

if ( !make_path(sorc) )
    return FALSE;

time_out = OPEN_TIMEOUT_COUNT;
ms       = OPEN_WAIT_MS;
while ( true )
    {
    fh = CreateFile( path, access_mode, share_mode, NULL, create_flags, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
    if ( fh == INVALID_HANDLE_VALUE )
        {
        last_status = BAD_FILE_OPEN;
        last_error  = GetLastError();
        if ( last_error != ERROR_SHARING_VIOLATION )
            break;
        }
    else
        {
        last_status = OK_STATUS;
        last_error  = ERROR_SUCCESS;
        break;
        }

    time_out--;
    if ( !time_out )
        break;

    Sleep( ms );
    if ( time_out >= LAST_TIMEOUT_TO_DOUBLE )
        ms *= 2;
    }

if ( last_status == OK_STATUS )
    return TRUE;

cleanup_memory();
return FALSE;
}

/***********************************************************************
*                              OPEN_FOR_WRITE                          *
***********************************************************************/
BOOLEAN FILE_CLASS::open_for_write( TCHAR * sorc )
{
share_mode  = 0; /*FILE_SHARE_READ | FILE_SHARE_WRITE; */
access_mode = GENERIC_WRITE;

return open( sorc, CREATE_ALWAYS );
}

/***********************************************************************
*                              OPEN_FOR_WRITE                          *
***********************************************************************/
BOOLEAN FILE_CLASS::open_for_write( STRING_CLASS & sorc )
{
return open_for_write( sorc.text() );
}

/***********************************************************************
*                              OPEN_FOR_READ                           *
***********************************************************************/
BOOLEAN FILE_CLASS::open_for_read( TCHAR * sorc )
{
share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
access_mode = GENERIC_READ;

return open( sorc, OPEN_EXISTING );
}

/***********************************************************************
*                              OPEN_FOR_READ                           *
***********************************************************************/
BOOLEAN FILE_CLASS::open_for_read( STRING_CLASS & sorc )
{
return open_for_read( sorc.text() );
}

/***********************************************************************
*                             OPEN_FOR_APPEND                          *
***********************************************************************/
BOOLEAN FILE_CLASS::open_for_append( TCHAR * sorc )
{
share_mode  = 0;
access_mode = GENERIC_WRITE;

if ( open(sorc, OPEN_ALWAYS) )
    {
    SetFilePointer( fh, 0, 0, FILE_END );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             OPEN_FOR_APPEND                          *
***********************************************************************/
BOOLEAN FILE_CLASS::open_for_append( STRING_CLASS & sorc )
{
return open_for_append( sorc.text() );
}

/***********************************************************************
*                              OPEN_FOR_UPDATE                         *
***********************************************************************/
BOOLEAN FILE_CLASS::open_for_update( TCHAR * sorc )
{
share_mode  = FILE_SHARE_READ;
access_mode = GENERIC_READ | GENERIC_WRITE;

return open( sorc, OPEN_EXISTING );
}

/***********************************************************************
*                              OPEN_FOR_UPDATE                         *
***********************************************************************/
BOOLEAN FILE_CLASS::open_for_update( STRING_CLASS & sorc )
{
return open_for_update( sorc.text() );
}

/***********************************************************************
*                                 CLOSE                                *
***********************************************************************/
void FILE_CLASS::close()
{
cleanup();
}

/***********************************************************************
*                               SET_EOF                                *
***********************************************************************/
BOOLEAN FILE_CLASS::set_eof()
{
if ( fh != INVALID_HANDLE_VALUE )
    {
    if ( SetEndOfFile(fh) )
        return TRUE;

    last_status = FAIL_EOF;
    last_error  = GetLastError();
    }

return FALSE;
}

/***********************************************************************
*                                EMPTY                                 *
***********************************************************************/
BOOLEAN FILE_CLASS::empty()
{
if ( fh != INVALID_HANDLE_VALUE )
    {
    if ( setpos(0) )
        {
        if ( set_eof() )
            return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                               SETPOS                                 *
***********************************************************************/
BOOLEAN FILE_CLASS::setpos( int32 char_position )
{
int32 byte_position;
DWORD status;

if ( fh != INVALID_HANDLE_VALUE )
    {
    byte_position = char_position * sizeof(TCHAR);

    status = SetFilePointer( fh, byte_position, 0, FILE_BEGIN );
    if ( status != 0xFFFFFFFF )
        {
        current_position = char_position;
        last_status = OK_STATUS;
        return TRUE;
        }
    else
        {
        last_status = SETPOS_ERROR;
        last_error  = GetLastError();
        }
    }

return FALSE;
}

/***********************************************************************
*                            SET_BYTE_POS                              *
***********************************************************************/
BOOLEAN FILE_CLASS::set_byte_pos( int32 byte_position )
{
DWORD status;

if ( fh != INVALID_HANDLE_VALUE )
    {
    status = SetFilePointer( fh, byte_position, 0, FILE_BEGIN );
    if ( status != 0xFFFFFFFF )
        {
        current_position = byte_position*sizeof(TCHAR);
        last_status = OK_STATUS;
        return TRUE;
        }
    else
        {
        last_status = SETPOS_ERROR;
        last_error  = GetLastError();
        }
    }

return FALSE;
}

/***********************************************************************
*                             WRITEBINARY                              *
***********************************************************************/
BOOLEAN FILE_CLASS::writebinary( void * sorc, DWORD bytes_to_write )
{
BOOL status;
DWORD bytes_written;

if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;

if ( !sorc )
    bytes_to_write = 0;

bytes_written  = bytes_to_write;
status         = TRUE;

if ( bytes_to_write > 0 )
    status = WriteFile( fh, sorc, bytes_to_write, &bytes_written, 0 );

if ( !status || bytes_written != bytes_to_write )
    {
    last_status = WRITE_ERROR;
    last_error  = GetLastError();
    status = FALSE;
    }

return status;
}

/***********************************************************************
*                             WRITELINE                                *
***********************************************************************/
BOOLEAN FILE_CLASS::writeline( TCHAR * sorc )
{

BOOL status;
DWORD bytes_to_write;
DWORD bytes_written;

if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;

if ( sorc )
    bytes_to_write = lstrlen( sorc ) * sizeof( TCHAR );
else
    bytes_to_write = 0;

bytes_written  = bytes_to_write;
status         = TRUE;

if ( bytes_to_write > 0 )
    status = WriteFile( fh, sorc, bytes_to_write, &bytes_written, 0 );

if ( status && bytes_written == bytes_to_write )
    {
    bytes_to_write = lstrlen( CrLf ) * sizeof( TCHAR );
    status = WriteFile( fh, CrLf, bytes_to_write, &bytes_written, 0 );
    }
else
    {
    last_status = WRITE_ERROR;
    last_error  = GetLastError();
    status = FALSE;
    }

return status;
}

/***********************************************************************
*                             WRITELINE                                *
***********************************************************************/
BOOLEAN FILE_CLASS::writeline( STRING_CLASS & sorc )
{
return writeline( sorc.text() );
}

/***********************************************************************
*                               READBINARY                             *
*                     Return the number of bytes read.                 *
***********************************************************************/
DWORD FILE_CLASS::readbinary( void * dest, DWORD bytes_to_read )
{
BOOL    status;
DWORD   bytes_read;

bytes_read = 0;

if ( fh == INVALID_HANDLE_VALUE )
    return bytes_read;

if ( last_status != OK_STATUS )
    return bytes_read;

status = ReadFile( fh, dest, bytes_to_read, &bytes_read, 0 );
if ( status && bytes_read != bytes_to_read )
    {
    last_status = END_OF_FILE;
    last_error  = ERROR_SUCCESS;
    }

if ( !status )
    {
    last_status = READ_ERROR;
    last_error  = GetLastError();
    }
current_position += bytes_read;
return bytes_read;
}

/***********************************************************************
*                               READLINE                               *
***********************************************************************/
TCHAR * FILE_CLASS::readline()
{

BOOL    status;
DWORD   bytes_to_read;
DWORD   bytes_read;
int32   slen;
int32   n;
TCHAR * cp;

if ( fh == INVALID_HANDLE_VALUE )
    return 0;

if ( last_status == END_OF_FILE )
    return 0;

bytes_to_read = sizeof( TCHAR );
slen  = 0;
cp    = buf;
while ( TRUE )
    {
    /*
    ----------------------------------------------------------------
    If I have reached the end of the buffer or if there is no buffer
    then double the size (or create a new one).
    ---------------------------------------------------------------- */
    if ( buflen <= (DWORD) slen )
        {
        n = slen * 2;

        if ( n > MAX_LINE_LEN )
            {
            last_status = LINE_TOO_LONG;
            return 0;
            }

        if ( n < 1 )
            n = 80;

        cp = maketext( n );
        if ( !cp )
            {
            last_status = NO_BUFFER_MEMORY;
            return 0;
            }

        if ( buf )
            {
            copychars( cp, buf, slen );
            delete[] buf;
            }
        buf    = cp;
        buflen = n;
        cp     = buf + slen;
        }

    status = ReadFile( fh, cp, bytes_to_read, &bytes_read, 0 );
    if ( status && bytes_read > 0 )
        {
        current_position += bytes_read;

        if ( *cp == LfChar )
            break;

        if ( *cp != CrChar )
            {
            cp++;
            slen++;
            }
        }
    else
        {
        last_status = END_OF_FILE;
        last_error  = ERROR_SUCCESS;
        break;
        }
    }

*cp = NullChar;

if ( last_status == END_OF_FILE && cp == buf )
    return 0;

return buf;
}

/***********************************************************************
*                               NOF_LINES                              *
* Count the number of line feeds. Add one if there are chars after the *
* last lf.                                                             *
***********************************************************************/
int FILE_CLASS::nof_lines()
{
const int   BUFSIZE = 32768;
const DWORD bytes_to_read = (DWORD) BUFSIZE * sizeof(TCHAR);
TCHAR * cp;
TCHAR   buf[BUFSIZE];
DWORD   bytes_read;
DWORD   pos;
int     count;
int     i;
int     n;
BOOL    status;
BOOLEAN have_extra_chars;

if ( fh == INVALID_HANDLE_VALUE )
    return 0;

pos = getpos();
setpos( 0 );

have_extra_chars = FALSE;
count = 0;
while( TRUE )
    {

    status = ReadFile( fh, buf, bytes_to_read, &bytes_read, 0 );
    if ( !status || bytes_read == 0 )
        break;
    cp = buf;
    n  = bytes_read / sizeof( TCHAR );
    for ( i=0; i<n; i++ )
        {
        if ( *cp == LfChar )
            {
            count++;
            have_extra_chars = FALSE;
            }
        else
            {
            have_extra_chars = TRUE;
            }

        cp++;
        }

    if ( bytes_read != bytes_to_read )
        break;
    }

if ( have_extra_chars )
    count++;

setpos( pos );

return count;
}

/***********************************************************************
*                           GET_CREATION_TIME                          *
* I was using the creation time (argument 1) but this only worked      *
* on the local drive (not on network drives) so now I use the last     *
* write time, which is the only one that is sent on a network file.    *
* This only works on open_for_read because you must use the            *
* GENERIC_READ flag.                                                   *
***********************************************************************/
BOOLEAN FILE_CLASS::get_creation_time( FILETIME * dest )
{

if ( fh != INVALID_HANDLE_VALUE )
    {
    if ( GetFileTime(fh, 0, 0, dest) )
        {
        return TRUE;
        }
    }

return FALSE;
}

