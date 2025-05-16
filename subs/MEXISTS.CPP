#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"

/***********************************************************************
*                            MACHINE_EXISTS                            *
***********************************************************************/
BOOLEAN machine_exists( TCHAR * computer_to_find, TCHAR * name_to_find )
{
BOOLEAN  status;
TCHAR    s[MAX_PATH+1];
DB_TABLE t;

status = FALSE;
lstrcpy( s, machset_dbname(computer_to_find) );
if ( file_exists(s) )
    {
    t.open( s, MACHSET_RECLEN, PFL );
    t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, name_to_find, MACHINE_NAME_LEN );
    status = t.get_next_key_match( 1, NO_LOCK );
    t.close();
    }

return status;
}
