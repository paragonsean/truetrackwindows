#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\names.h"

/***********************************************************************
*                         SET_LAST_SHOT_NUMBER                         *
***********************************************************************/
BOOLEAN set_last_shot_number( PART_NAME_ENTRY & p, int32 new_shot_number )
{
DB_TABLE t;
BOOLEAN status;

status = FALSE;

t.open( plookup_dbname(p.computer, p.machine), PLOOKUP_RECLEN, PWL );
t.put_alpha( PLOOKUP_PART_NAME_OFFSET, p.part, PART_NAME_LEN );
t.reset_search_mode();
if ( t.get_next_key_match(1, TRUE) )
    {
    t.put_long( PLOOKUP_LAST_SHOT_OFFSET, new_shot_number, SHOT_LEN );
    t.rec_update();
    t.unlock_record();
    status = TRUE;
    }

t.close();
return status;
}

/***********************************************************************
*                         SET_LAST_SHOT_NUMBER                         *
***********************************************************************/
BOOLEAN set_last_shot_number( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, int32 new_shot_number )
{
PART_NAME_ENTRY p;

p.set( computer_name, machine_name, part_name );
return set_last_shot_number( p, new_shot_number );

}

