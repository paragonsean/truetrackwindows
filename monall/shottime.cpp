#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"

/***********************************************************************
*                               SHOTTIME                               *
***********************************************************************/
long shottime( TCHAR * computer, TCHAR * machine, TCHAR * part )
{
DB_TABLE t;
TCHAR this_part[PART_NAME_LEN+1];
SYSTEMTIME mytime;

init_systemtime_struct( mytime );

t.open( plookup_dbname(computer,machine), PLOOKUP_RECLEN, PFL );
while( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( this_part, PLOOKUP_PART_NAME_OFFSET, PART_NAME_LEN );
    if ( compare(part, this_part, PART_NAME_LEN) == 0 )
        {
        t.get_date( mytime, PLOOKUP_DATE_OFFSET );
        t.get_time( mytime, PLOOKUP_TIME_OFFSET );
        return sec_since1990( mytime );
        }
    }

return 0;
}
