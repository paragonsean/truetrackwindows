#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "extern.h"
#include "resource.h"

/***********************************************************************
*                         SAVE_AS_MASTER_TRACE                         *
*       Save the currently displayed trace as the master trace.        *
***********************************************************************/
void save_as_master_trace( void )
{
TCHAR   dest[MAX_PATH+1];
TCHAR   sorc[MAX_PATH+1];

lstrcpy( sorc, profile_name(ComputerName, MachineName, PartName, ShotName) );
lstrcpy( dest, master_name(ComputerName, MachineName, PartName, CURRENT_MASTER_TRACE_NAME) );
if ( !file_exists(sorc) )
    {
    /*
    -----------------------------------------------
    I'm going to save a fastrak2 file master trace.
    Delete the old master trace if there is one
    ----------------------------------------------- */
    if ( file_exists(dest) )
        DeleteFile( dest );
    lstrcpy( sorc, profile_ii_name(ComputerName, MachineName, PartName, ShotName) );
    lstrcpy( dest, master_ii_name(ComputerName, MachineName, PartName, CURRENT_MASTER_TRACE_NAME) );
    }
if ( file_exists(dest) )
    {
    if ( resource_message_box(MainInstance, REPLACE_MASTER_STRING, MASTER_EXISTS_STRING, MB_SYSTEMMODAL | MB_YESNO | MB_ICONQUESTION) == IDNO )
        return;
    }

CopyFile( sorc, dest, FALSE );
}
