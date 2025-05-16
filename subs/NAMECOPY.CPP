#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static TCHAR NullChar       = TEXT( '\0' );
static TCHAR UnderscoreChar = TEXT( '_' );
static TCHAR DashChar       = TEXT( '-' );

/***********************************************************************
*                            PART_NAME_COPY                            *
*                                                                      *
*         Copy the part name from a string that may be bigger.         *
*                                                                      *
***********************************************************************/
TCHAR * part_name_copy( TCHAR * sorc )
{
static TCHAR mycopy[PART_NAME_LEN+1];
int i;
TCHAR c;

i = 0;
while ( TRUE )
    {
    if ( i == PART_NAME_LEN )
        break;

    c = sorc[i];

    /*
    -------------------------------------------
    Ok to copy if a is in { 0-9, A-Z, a-z, _, -  }
    ------------------------------------------- */
    if ( !IsCharAlphaNumeric(c) && c != UnderscoreChar && c != DashChar )
        break;

    mycopy[i] = c;
    i++;
    }

mycopy[i] = NullChar;

return mycopy;
}
