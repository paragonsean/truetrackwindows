#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

static TCHAR NullChar = TEXT( '\0' );

/***********************************************************************
*                             PART_EXISTS                              *
***********************************************************************/
BOOLEAN part_exists( TCHAR * computer_to_find, TCHAR * machine_to_find, TCHAR * name_to_find )
{
BOOLEAN    status;
NAME_CLASS s;
DB_TABLE   t;

status = FALSE;
s.get_parts_file_name( computer_to_find, machine_to_find );
if ( !s.file_exists() )
    return FALSE;

if ( t.open(s.text(), PARTS_RECLEN, PFL) )
    {
    t.put_alpha( PARTS_PART_NAME_OFFSET, name_to_find, PART_NAME_LEN );
    status = t.get_next_key_match( 1, NO_LOCK );
    t.close();
    }

return status;
}

/***********************************************************************
*                             PART_EXISTS                              *
***********************************************************************/
BOOLEAN part_exists( PART_NAME_ENTRY & p )
{
return part_exists( p.computer, p.machine, p.part );
}

/***********************************************************************
*                              NOT_A_PART                              *
*  Return TRUE if the name is empty or is "NOPART".                    *
***********************************************************************/
BOOLEAN not_a_part( TCHAR * pa )
{
const TCHAR nopart[] = NO_PART;

if ( *pa == NullChar )
    return TRUE;

if ( compare(nopart, pa) == 0 )
    return TRUE;

return FALSE;
}