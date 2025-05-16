#ifndef _FILE_CLASS_
#define _FILE_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

const DWORD OK_STATUS        =  0;
const DWORD BAD_FILE_OPEN    =  1;
const DWORD FILE_NOT_FOUND   =  2;
const DWORD NO_BUFFER_MEMORY =  3;
const DWORD LINE_TOO_LONG    =  4;
const DWORD NO_PATH_MEMORY   =  5;
const DWORD EMPTY_ERROR      =  6;
const DWORD SETPOS_ERROR     =  7;
const DWORD FILE_NOT_OPEN    =  8;
const DWORD WRITE_ERROR      =  9;
const DWORD END_OF_FILE      = 10;
const DWORD READ_ERROR       = 11;
const DWORD FAIL_EOF         = 12;

const int32 MAX_LINE_LEN     = 32768;

class FILE_CLASS
    {
    private:

    TCHAR  * path;
    TCHAR  * buf;
    DWORD  access_mode;
    DWORD  share_mode;
    DWORD  current_position;
    DWORD  buflen;
    DWORD  last_error;
    DWORD  last_status;
    HANDLE fh;

    public:

    FILE_CLASS();
    ~FILE_CLASS();
    void    cleanup_memory();
    void    cleanup();
    void    close();
    BOOLEAN empty();
    DWORD   error()  { return last_error;  }
    DWORD   getpos() { return current_position; }
    BOOLEAN get_creation_time( FILETIME * dest );
    BOOLEAN make_path( TCHAR * sorc );
    int     nof_lines();
    BOOLEAN open( TCHAR * sorc, DWORD create_flags );
    BOOLEAN open_for_append( TCHAR * sorc );
    BOOLEAN open_for_append( STRING_CLASS & sorc );
    BOOLEAN open_for_read(   TCHAR * sorc );
    BOOLEAN open_for_read(   STRING_CLASS & sorc );
    BOOLEAN open_for_write(  TCHAR * sorc );
    BOOLEAN open_for_write(  STRING_CLASS & sorc );
    BOOLEAN open_for_update( TCHAR * sorc );
    BOOLEAN open_for_update( STRING_CLASS & sorc );
    DWORD   readbinary( void * dest, DWORD nof_bytes );
    TCHAR * readline();
    BOOLEAN set_byte_pos( int32 byte_position );
    BOOLEAN set_eof();
    BOOLEAN setpos( int32 char_position );
    BOOLEAN rewind() { return setpos( 0 ); }
    DWORD   status() { return last_status; }
    BOOLEAN writebinary( void * sorc, DWORD nof_bytes );
    BOOLEAN writeline( TCHAR * sorc );
    BOOLEAN writeline( STRING_CLASS & sorc );
    };

#endif
