#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "category.h"

static TCHAR EmptyString[] = TEXT( "" );
static TCHAR TabChar       = TEXT( '\t' );

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void CATEGORY_CLASS::cleanup( void )
{

if ( cat )
    {
    delete[] cat;
    cat = 0;
    n   = 0;
    }

}

/***********************************************************************
*                           ~CATEGORY_CLASS                            *
***********************************************************************/
CATEGORY_CLASS::~CATEGORY_CLASS( void )
{
cleanup();
}

/***********************************************************************
*                               READ                                   *
***********************************************************************/
BOOLEAN CATEGORY_CLASS::read( void )
{
DB_TABLE t;
int      nof_categories;
BOOLEAN  status;

status = FALSE;
if ( t.open(downcat_dbname(), DOWNCAT_RECLEN, PFL) )
    {
    nof_categories = t.nof_recs();
    if ( nof_categories > 0 )
        {
        cat = new CATEGORY_ENTRY[nof_categories];
        if ( cat )
            {
            status = TRUE;
            n      = 0;
            while( t.get_next_record(NO_LOCK) )
                {
                t.get_alpha( cat[n].number, DOWNCAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                t.get_alpha( cat[n].name,   DOWNCAT_NAME_OFFSET,   DOWNCAT_NAME_LEN   );
                n++;
                if ( n >= nof_categories )
                    break;
                }
            }
        }
    t.close();
    }

return status;
}

/***********************************************************************
*                                 SAVE                                 *
*                                                                      *
* I assume each listbox line starts with cat no and name, separated    *
* by tabs.                                                             *
***********************************************************************/
BOOLEAN CATEGORY_CLASS::save( LISTBOX_CLASS & lb )
{
DB_TABLE       t;
INT            i;
INT            nof_rows;
TCHAR          last_cat[DOWNCAT_NUMBER_LEN+1];
CATEGORY_ENTRY ce;
TCHAR        * cp;

nof_rows = lb.count();
if ( nof_rows <= 0 )
    return FALSE;

lstrcpy( last_cat, NO_DOWNCAT_NUMBER );

if ( t.open(downcat_dbname(), DOWNCAT_RECLEN, FL) )
    {
    t.empty();
    for ( i=0; i<nof_rows; i++ )
        {
        cp = extfield( ce.number, lb.item_text(i), TabChar, DOWNCAT_NUMBER_LEN );
        if ( lstrcmp(last_cat, ce.number) != 0 )
            {
            lstrcpy( last_cat, ce.number );
            extfield( ce.name, cp, TabChar, DOWNCAT_NAME_LEN );
            t.put_alpha( DOWNCAT_NUMBER_OFFSET, ce.number, DOWNCAT_NUMBER_LEN );
            t.put_alpha( DOWNCAT_NAME_OFFSET,   ce.name,   DOWNCAT_NAME_LEN );
            t.rec_append();
            }
        }
    t.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                                NAME                                  *
***********************************************************************/
TCHAR * CATEGORY_CLASS::name( TCHAR * number_to_find )
{
int i;
for ( i=0; i<n; i++ )
    {
    if ( lstrcmp(cat[i].number, number_to_find) == 0 )
        return cat[i].name;
    }

return EmptyString;
}
