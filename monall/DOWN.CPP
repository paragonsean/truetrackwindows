#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\events.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "extern.h"

/***********************************************************************
*                           CURRENT_OPERATOR                           *
***********************************************************************/
TCHAR * current_operator( TCHAR * computer, TCHAR * machine )
{
static TCHAR opnumber[OPERATOR_NUMBER_LEN+1];
DB_TABLE t;

lstrcpy( opnumber, NO_OPERATOR_NUMBER );

if ( t.open(machset_dbname(computer), MACHSET_RECLEN, PFL) )
    {
    t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, machine, MACHINE_NAME_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        t.get_alpha( opnumber, MACHSET_OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
    t.close();
    }

return opnumber;
}

/***********************************************************************
*                        ISSUE_DOWNTIME_EVENT                          *
***********************************************************************/
void issue_downtime_event( TCHAR * computer, TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat )
{
TCHAR * cp;
TCHAR * worker;

worker = current_operator( computer, machine );

cp = make_downtime_event_string( machine, part, st, cat, subcat, worker );
if ( cp )
    PostMessage( MainWindow, WM_POKEMON, (WPARAM) DOWN_DATA_INDEX, (LPARAM) cp );
}

/***********************************************************************
*                        APPEND_DOWNTIME_RECORD                        *
***********************************************************************/
BOOLEAN append_downtime_record( TCHAR * computer, TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat )
{
DB_TABLE t;
TCHAR * worker;
BOOLEAN status;

status = FALSE;

worker = current_operator( computer, machine );

if ( t.open(downtime_dbname(computer, machine), DOWNTIME_RECLEN, PWL) )
    {
    make_downtime_record( t, st, cat, subcat, worker, part );
    t.rec_append();
    t.close();
    status = TRUE;
    }

issue_downtime_event( computer, machine, part, st, cat, subcat );

return status;
}

