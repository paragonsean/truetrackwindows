#include <windows.h>

static TCHAR CrLf[] = TEXT("\015\012");

/***********************************************************************
*                              WRITELINE                               *
***********************************************************************/
BOOL writeline( HANDLE fh, TCHAR * sorc )
{

BOOL status;
DWORD bytes_to_write;
DWORD bytes_written;

bytes_to_write = lstrlen( sorc ) * sizeof( TCHAR );
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
    status = FALSE;

return status;
}
