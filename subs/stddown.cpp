#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\stddown.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

/***********************************************************************
*                         STDDOWN_CLASS                                *
*                            startup                                   *
***********************************************************************/
void STDDOWN_CLASS::startup()
{
STRING_CLASS s;
DB_TABLE t;
int i;

cleanup();

s = stddown_dbname();
if ( file_exists(s.text()) )
    {
    if ( t.open(s.text(), STDDOWN_RECLEN, PFL) )
        {
        n = t.nof_recs();
        if ( n > 0 )
            {
            p = new STDDOWN_ENTRY[n];
            if ( p )
                {
                i = 0;
                while( t.get_next_record(NO_LOCK) )
                    {
                    t.get_alpha( p[i].category,    STDDOWN_CAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                    t.get_alpha( p[i].subcategory, STDDOWN_SUB_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                    t.get_alpha( p[i].startime,    STDDOWN_START_TIME_OFFSET, ALPHATIME_LEN );
                    t.get_alpha( p[i].endtime,     STDDOWN_END_TIME_OFFSET,   ALPHATIME_LEN );
                    i++;
                    }
                }
            }
        t.close();
        }
    }

if ( !p )
    n = 0;

current_index = 0;
}

/***********************************************************************
*                         STDDOWN_CLASS                                *
*                             find                                     *
***********************************************************************/
bool STDDOWN_CLASS::find( long now )
{
long  s1;
long  s2;
SYSTEMTIME t;

GetLocalTime( &t );

int i;
for ( i=0; i<n; i++ )
    {
    exthms( t, p[i].startime );
    s1 = sec_since1990( t );

    exthms( t, p[i].endtime );
    s2 = sec_since1990( t );

    if ( s1 <= now && now <= s2 )
        {
        current_index = i;
        return true;
        }
    }
return false;
}

/***********************************************************************
*                         STDDOWN_CLASS                                *
*                              cat                                     *
***********************************************************************/
TCHAR * STDDOWN_CLASS::cat()
{
static TCHAR es[] = TEXT("");
if ( p )
    return p[current_index].category;

return es;
}

/***********************************************************************
*                         STDDOWN_CLASS                                *
*                            subcat                                    *
***********************************************************************/
TCHAR * STDDOWN_CLASS::subcat()
{
static TCHAR es[] = TEXT("");

if ( p )
    return p[current_index].subcategory;

return es;
}

