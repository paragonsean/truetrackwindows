#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

/***********************************************************************
*                           CURRENT_PART_NAME                          *
***********************************************************************/
TCHAR * current_part_name( TCHAR * computer, TCHAR * machine )
{
static TCHAR part[PART_NAME_LEN+1];
DB_TABLE     t;
STRING_CLASS s;

lstrcpy( part, NO_PART );

s = machset_dbname( computer );

if ( file_exists(s.text()) )
    {
    t.open( s.text(), MACHSET_RECLEN, PFL );
    t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, machine, MACHINE_NAME_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        t.get_alpha( part, MACHSET_CURRENT_PART_OFFSET, PART_NAME_LEN );
    t.close();
    }

return part;
}

/***********************************************************************
*                        SET_CURRENT_PART_NAME                         *
***********************************************************************/
BOOLEAN set_current_part_name( TCHAR * co, TCHAR * ma, TCHAR * pa )
{
DB_TABLE t;
STRING_CLASS s;
BOOLEAN status;

status = FALSE;

s = machset_dbname(co);

if ( file_exists(s.text()) )
    {
    t.open( s.text(), MACHSET_RECLEN, PWL );
    t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, ma, MACHINE_NAME_LEN );
    if ( t.get_next_key_match(1, WITH_LOCK) )
        {
        t.put_alpha( MACHSET_CURRENT_PART_OFFSET, pa, PART_NAME_LEN );
        t.rec_update();
        t.unlock_record();
        status = TRUE;
        }
    t.close();
    }

return status;
}

/***********************************************************************
*                        SET_CURRENT_PART_NAME                         *
***********************************************************************/
BOOLEAN set_current_part_name( PART_NAME_ENTRY & p )
{
return set_current_part_name( p.computer, p.machine, p.part);
}
