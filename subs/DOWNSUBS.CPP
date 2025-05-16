#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\names.h"

static TCHAR   TabChar  = TEXT('\t');

/***********************************************************************
*                         MAKE_DOWNTIME_RECORD                         *
***********************************************************************/
BOOLEAN make_downtime_record( DB_TABLE & t, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat, TCHAR * op, TCHAR * part )
{

t.put_date(  DOWNTIME_DATE_OFFSET,       st );
t.put_time(  DOWNTIME_TIME_OFFSET,       st );
t.put_alpha( DOWNTIME_CAT_OFFSET,        cat,    DOWNCAT_NUMBER_LEN );
t.put_alpha( DOWNTIME_SUBCAT_OFFSET,     subcat, DOWNCAT_NUMBER_LEN );
t.put_alpha( DOWNTIME_OPERATOR_OFFSET,   op,     OPERATOR_NUMBER_LEN );
t.put_alpha( DOWNTIME_PART_NAME_OFFSET,  part,   PART_NAME_LEN );

return TRUE;
}

/***********************************************************************
*                      MAKE_DOWNTIME_EVENT_STRING                      *
* NOTE: !! The caller is responsible for deleting the string !!        *
***********************************************************************/
TCHAR * make_downtime_event_string( TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat, TCHAR * worker )
{
static const int DDE_LINE_LEN  = MACHINE_NAME_LEN + 1 + PART_NAME_LEN + 1 + ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 + 2*(DOWNCAT_NUMBER_LEN + 1) + 2*(DOWNCAT_NAME_LEN + 1) + OPERATOR_NUMBER_LEN;

TCHAR * buf;
TCHAR * cp;

buf = new TCHAR[DDE_LINE_LEN+1];
if ( !buf )
    return 0;

cp = buf;

cp = copy_w_char( cp, machine,       TabChar );
cp = copy_w_char( cp, part,          TabChar );
cp = copy_w_char( cp, alphadate(st), TabChar );
cp = copy_w_char( cp, alphatime(st), TabChar );
cp = copy_w_char( cp, cat,           TabChar );
cp = copy_w_char( cp, subcat,        TabChar );
cp = copy_w_char( cp, category_name(cat), TabChar );
cp = copy_w_char( cp, subcategory_name(cat, subcat), TabChar );
lstrcpy( cp, worker );

return buf;
}

/***********************************************************************
*                            CATEGORY_NAME                             *
***********************************************************************/
TCHAR * category_name( TCHAR * number_to_match )
{
static TCHAR name[DOWNCAT_NAME_LEN+1];
static TCHAR number[DOWNCAT_NUMBER_LEN+1];
STRING_CLASS s;
DB_TABLE     t;

lstrcpy( name, NO_DOWNCAT_NAME );

s = downcat_dbname();
if ( t.open(s.text(), DOWNCAT_RECLEN, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( number, DOWNCAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
        if ( compare(number_to_match, number, DOWNCAT_NUMBER_LEN) == 0 )
            {
            t.get_alpha( name,   DOWNCAT_NAME_OFFSET,   DOWNCAT_NAME_LEN   );
            break;
            }
        }
    t.close();
    }

return name;
}

/***********************************************************************
*                          SUBCATEGORY_NAME                            *
***********************************************************************/
TCHAR * subcategory_name( TCHAR * cat_to_match, TCHAR * subcat_to_match )
{
static TCHAR cat[DOWNCAT_NUMBER_LEN+1];
static TCHAR subcat[DOWNCAT_NUMBER_LEN+1];
static TCHAR name[DOWNCAT_NAME_LEN+1];
DB_TABLE t;
short    record_length;
STRING_CLASS s;

lstrcpy( name, NO_DOWNCAT_NAME );
s = downscatc_dbname();
if ( file_exists(s.text()) )
    {
    record_length = DOWNSCATC_RECLEN;
    }
else
    {
    record_length = DOWNSCAT_RECLEN;
    s = downscat_dbname();
    }

if ( t.open(s.text(), record_length, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( cat,    DOWNSCAT_CAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
        t.get_alpha( subcat, DOWNSCAT_SUB_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
        if ( compare(cat_to_match, cat, DOWNCAT_NUMBER_LEN) == 0 && compare(subcat_to_match, subcat, DOWNCAT_NUMBER_LEN) == 0 )
            {
            t.get_alpha( name, DOWNSCAT_SUB_NAME_OFFSET, DOWNCAT_NAME_LEN   );
            break;
            }
        }
    t.close();
    }

return name;
}

