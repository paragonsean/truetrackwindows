#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\stringcl.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

static TCHAR CommaChar = TEXT( ',' );
static TCHAR NullChar  = TEXT( '\0' );
static TCHAR SpaceChar = TEXT( ' ' );

/***********************************************************************
*                             write_olmsted_file                       *
***********************************************************************/
BOOLEAN write_olmsted_file( STRING_CLASS & destfile, STRING_CLASS & sorcfile )
{
FTII_FILE_DATA  ft;
FTII_ARRAY_DATA fa;

BOOLEAN    status;
FILE_CLASS f;
int        i;
int        j;
TCHAR    * cp;
TCHAR      buf[100];

status = FALSE;

if ( !ft.get(sorcfile) )
    return FALSE;

fa = ft;

if ( f.open_for_write(destfile.text()) )
    {
    copystring( buf, int32toasc(ft.np-1) );
    rjust( buf, 5, SpaceChar );
    buf[5] = NullChar;
    f.writeline( buf );

    i = 1;
    while ( i<fa.np )
        {
        copystring( buf, int32toasc(fa.position[i+3]) );
        rjust( buf, 5, SpaceChar );
        buf[5] = CommaChar;
        cp = buf+6;
        for ( j=0; j<4; j++ )
            {
            copystring( cp, ultoascii((unsigned long) fa.timer[i]) );
            rjust( cp, 10, SpaceChar );
            cp += 10;
            if ( j < 3 )
                *cp++ = CommaChar;
            i++;
            }
        *cp = NullChar;
        f.writeline( buf );
        }
    while ( i<fa.n )
        {
        copystring( buf, int32toasc(fa.position[i]) );
        rjust( buf, 5, SpaceChar );
        buf[5] = CommaChar;
        cp = buf+6;

        copystring( cp, ultoascii((unsigned long) fa.timer[i]) );
        rjust( cp, 10, SpaceChar );
        cp += 10;
        *cp = NullChar;
        f.writeline( buf );
        i++;
        }

    f.close();
    status = TRUE;
    }

return status;
}
