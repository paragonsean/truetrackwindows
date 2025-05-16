#include <windows.h>

/***********************************************************************
*                               IS_EMPTY                               *
***********************************************************************/
BOOLEAN is_empty( const TCHAR * s )
{

if ( s )
    {
    while ( *s )
        {
        if ( *s != TEXT( ' ' ) )
            return FALSE;
        s++;
        }
    }

return TRUE;
}
