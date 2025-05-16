#include <windows.h>

#include "..\include\visiparm.h"

#include "..\include\stringcl.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"

static TCHAR Cchar         = TEXT( 'c' );
static TCHAR Dchar         = TEXT( 'd' );
static TCHAR Ochar         = TEXT( 'o' );
static TCHAR Pchar         = TEXT( 'p' );
static TCHAR Mchar         = TEXT( 'm' );
static TCHAR Nchar         = TEXT( 'n' );
static TCHAR Tchar         = TEXT( 't' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR EmptyString[] = TEXT( "" );

/***********************************************************************
*                             LOG_FILE_STRING                          *
* This function returns TRUE if something was copied to the dest       *
* besides the format string.                                           *
***********************************************************************/
BOOLEAN log_file_string( STRING_CLASS & dest, TCHAR * format, TCHAR * owner, TCHAR * machine, TCHAR * old_part, TCHAR * new_part )
{
static TCHAR buf[] = TEXT("x");
TCHAR * fs;
TIME_CLASS   t;
TCHAR computer[MAX_COMPUTERNAME_LENGTH + 1];
DWORD computer_size = MAX_COMPUTERNAME_LENGTH + 1;
BOOLEAN status;

status = FALSE;
t.get_local_time();
GetComputerName( computer, &computer_size );

fs = format;
dest = EmptyString;
while ( true )
    {
    if ( *fs == Cchar )
        {
        dest += computer;
        status = TRUE;
        }
    else if ( *fs == Dchar )
        {
        dest += t.mmddyyyy();
        status = TRUE;
        }
    else if ( *fs == Tchar )
        {
        dest += t.hhmmss();
        status = TRUE;
        }
    else if ( *fs == Mchar )
        {
        dest += machine;
        status = TRUE;
        }
    else if ( *fs == Nchar )
        {
        dest += owner;
        status = TRUE;
        }
    else if ( *fs == Ochar )
        {
        dest += old_part;
        status = TRUE;
        }
    else if ( *fs == Pchar )
        {
        dest += new_part;
        status = TRUE;
        }
    else
        {
        *buf = *fs;
        dest += buf;
        }

    if ( *fs == NullChar )
        break;

    fs++;
    }

return status;
}
