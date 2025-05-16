#include <windows.h>

#define LF_CHAR   TEXT('\012')
#define CR_CHAR   TEXT('\015')

/***********************************************************************
*                               READLINE                               *
*                                                                      *
*     Buflen is the length of the buffer NOT counting the NULL.        *
*                                        ~~~                           *
*     Returns the number of characters read not counting the NULL      *
*     and not counting the CR or LF (which are stripped).              *
***********************************************************************/
short readline( TCHAR * dest, HANDLE fh, short buflen )
{

BOOL status;
short count;
DWORD bytes_to_read;
DWORD bytes_read;

bytes_to_read = sizeof( TCHAR );
count = 0;
while ( TRUE )
    {
    status = ReadFile( fh, dest, bytes_to_read, &bytes_read, 0 );
    if ( status && bytes_read > 0 )
        {
        if ( *dest == LF_CHAR )
            break;

        if ( *dest != CR_CHAR && count < buflen )
            {
            count++;
            dest++;
            }
        }
    else
        break;
    }

*dest = TEXT( '\0' );

return count;
}
