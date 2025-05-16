#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"

/***********************************************************************
*                                                                      *
*                            GET_CURRENT_SHOT                          *
*                                                                      *
***********************************************************************/
int32 get_current_shot( TCHAR * computer, TCHAR * machine, TCHAR * part )
{
DB_TABLE t;
int32   shotnumber;
NAME_CLASS s;

shotnumber = NO_SHOT_NUMBER;
s = plookup_dbname( computer,machine );
if ( s.file_exists() )
    {
    if ( t.open(s.text(), PLOOKUP_RECLEN, PFL) )
        {
        t.put_alpha(PLOOKUP_PART_NAME_OFFSET, part, PART_NAME_LEN);
        t.reset_search_mode();
        if ( t.get_next_key_match(1, FALSE) )
            {
            shotnumber = t.get_long( PLOOKUP_LAST_SHOT_OFFSET );
            }

        t.close();
        }
    }

return shotnumber;
}
