#include <windows.h>
#include <ddeml.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static TCHAR NullChar           = TEXT( '\0' );
static TCHAR SpaceChar          = TEXT( ' ' );

/***********************************************************************
*                           GET_STARTUP_PART                           *
***********************************************************************/
BOOLEAN get_startup_part( TCHAR * machine, TCHAR * part )
{
TCHAR * commandline;
TCHAR * cp;
TCHAR * ep;
int     slen;

lstrcpy( machine, NO_MACHINE );
lstrcpy( part,    NO_PART    );

commandline = GetCommandLine();

cp = findchar( SpaceChar, commandline, lstrlen(commandline) );
if ( cp )
    {
    while ( *cp == SpaceChar )
        cp++;
    ep = findchar( SpaceChar, cp, lstrlen(cp) );
    if ( ep )
        {
        slen = ep - cp;
        if ( slen > 0 )
            {
            copychars( machine, cp, slen );
            *(machine+slen) = NullChar;
            ep++;
            if ( lstrlen(ep) > 0 )
                {
                lstrcpyn( part, ep, PART_NAME_LEN+1);
                return TRUE;
                }
            }
        }
    }

return FALSE;
}
