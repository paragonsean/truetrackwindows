#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"

/***********************************************************************
*                       GET_TIME_CLASS_FROM_DB                         *
*              (assume time field comes right after date)              *
***********************************************************************/
BOOLEAN get_time_class_from_db( TIME_CLASS & tc, DB_TABLE & db, FIELDOFFSET offset )
{
SYSTEMTIME st;
BOOLEAN status;

status = FALSE;
if ( db.get_date(st, offset) )
    {
    offset += DATE_FIELD_LEN;
    if ( db.get_time(st, offset) )
        {
        tc.set( st );
        status = TRUE;
        }
    }
 
return status;
}
 
