#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

/***********************************************************************
*                            PASSWORD_LEVEL                            *
***********************************************************************/
short password_level( TCHAR * pw )
{
DB_TABLE t;

short  level;
TCHAR  * cp;
TCHAR    name[PASSWORD_LEN+1];
short    reclen;

level = NO_PASSWORD_LEVEL;

cp = new_password_dbname();
if ( file_exists(cp) )
    {
    reclen = NEW_PASSWORD_RECLEN;
    }
else
    {
    cp = password_dbname();
    reclen = PASSWORD_RECLEN;
    }

if ( file_exists(cp) )
    {
    t.open( cp, reclen, PFL );
    while ( t.get_next_record(FALSE) )
        {
        t.get_alpha( name, PASSWORD_OFFSET, PASSWORD_LEN );
        if ( strings_are_equal(pw, name ) )
            {
            level = t.get_long( PASSWORD_LEVEL_OFFSET );
            break;
            }
        }

    t.close();
    }

return level;
}

