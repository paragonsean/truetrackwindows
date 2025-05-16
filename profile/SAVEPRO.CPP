#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "extern.h"

/***********************************************************************
*                    TOGGLE_PROFILE_SAVE_FLAGS                         *
*                                                                      *
*                                                                      *
*   If the profile is to be saved, turn off all saves else turn on     *
*   all saves.                                                         *
*                                                                      *
***********************************************************************/
void toggle_profile_save_flags( void )
{
const BITSETYPE flags = SAVE_PARM_DATA | SAVE_PROFILE_DATA;
DB_TABLE t;
BITSETYPE save_flags;

t.open( graphlst_dbname(ComputerName, MachineName, PartName), GRAPHLST_RECLEN, WL );

t.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, asctoint32(ShotName), SHOT_LEN );
if ( t.get_next_key_match(1, FALSE) )
    {
    save_flags  = (BITSETYPE) t.get_short( GRAPHLST_SAVE_FLAGS_OFFSET );
    if ( save_flags & SAVE_PROFILE_DATA )
        save_flags &= ~(flags);
    else
        save_flags |= flags;

    t.put_short( GRAPHLST_SAVE_FLAGS_OFFSET, save_flags, GRAPHLST_SAVE_FLAGS_LEN );
    t.rec_update();
    }
t.close();
}

