#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"
#include "..\include\structs.h"
#include "..\include\wclass.h"

#include "extern.h"
#include "resource.h"

/***********************************************************************
*                        GET_CURRENT_PART_NAME                         *
* This assumes the computer name has already been put into             *
* SureTrakPart.computer.                                               *
***********************************************************************/
BOOLEAN get_suretrak_part_name( void )
{
DB_TABLE t;
bool     status;
bool     is_monitor;
HWND     w;
TCHAR    rod_pitch[RODPITCH_NAME_LEN+1];

status     = false;
is_monitor = false;

/*
-------------------------------------
Find the currently monitored machine.
------------------------------------- */
t.open( machset_dbname(SureTrakPart.computer), MACHSET_RECLEN, PFL );
while ( t.get_next_record(FALSE) )
    {
    if ( MA_MONITORING_ON & BITSETYPE(t.get_long(MACHSET_MONITOR_FLAGS_OFFSET)) )
        is_monitor = true;

    /*
    ---------------------------------------------------------------------------
    Load the first machine or load the currently monitored machine if different
    --------------------------------------------------------------------------- */
    if ( !status || is_monitor )
        {
        t.get_alpha( SureTrakPart.machine, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );
        t.get_alpha( SureTrakPart.part,    MACHSET_CURRENT_PART_OFFSET,  PART_NAME_LEN );
        t.get_alpha( rod_pitch,            MACHSET_RODPITCH_OFFSET,      RODPITCH_NAME_LEN );
        status = true;
        }

    if ( is_monitor )
        break;
    }
t.close();

t.open( parts_dbname(SureTrakPart.computer, SureTrakPart.machine), PARTS_RECLEN, PFL );
t.put_alpha( PARTS_PART_NAME_OFFSET, SureTrakPart.part, PART_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
    {
    DistanceUnits = t.get_short( PARTS_DISTANCE_UNITS_OFFSET );
    VelocityUnits = t.get_short( PARTS_VELOCITY_UNITS_OFFSET );
    }
t.close();

t.open( psensor_dbname(), PSENSOR_RECLEN, PFL );

t.put_alpha( PSENSOR_RODPITCH_OFFSET,   rod_pitch,        RODPITCH_NAME_LEN );
t.put_short( PSENSOR_DIST_UNITS_OFFSET, DistanceUnits,    UNITS_ID_LEN );
t.put_short( PSENSOR_VEL_UNITS_OFFSET,  VelocityUnits,    UNITS_ID_LEN );
t.put_short( PSENSOR_MHZ_OFFSET,        DEF_SURETRAK_MHZ, PSENSOR_MHZ_LEN );
t.reset_search_mode();

if ( t.get_next_key_match(4, NO_LOCK) )
    PosSensorVelocityDividend = t.get_double( PSENSOR_VEL_FACTOR_OFFSET );

t.close();

if ( status )
    {
    w = MainScreenWindow.handle();
    if ( w )
        {
        set_text( w, COMPUTER_NAME_TBOX, SureTrakPart.computer );
        set_text( w, MACHINE_NAME_TBOX,  SureTrakPart.machine  );
        set_text( w, PART_NAME_TBOX,     SureTrakPart.part     );
        }
    }

return status;
}
