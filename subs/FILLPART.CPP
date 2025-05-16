#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"

static TCHAR NullChar  = TEXT( '\0' );
static TCHAR SpaceChar = TEXT( ' ' );
static TCHAR TabChar   = TEXT( '\t' );

/***********************************************************************
*                            FILL_PART_LISTBOX                         *
***********************************************************************/
void fill_part_listbox( HWND w, int box, TCHAR * computer, TCHAR * machine )
{
NAME_CLASS    s;
DB_TABLE      t;
TCHAR         name[PART_NAME_LEN+1];
HCURSOR       old_cursor;
LISTBOX_CLASS lb;

if ( !lb.init(w, box) )
    return;

s = parts_dbname( computer, machine );
if ( !s.file_exists() )
    return;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

lb.redraw_off();
lb.empty();

t.open( s.text(), PARTS_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( name, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
    lb.add( name );
    }
t.close();

lb.setcursel( 0 );
lb.redraw_on();

SetCursor( old_cursor );
}

/***********************************************************************
*                      MAKE_PART_AND_COUNTS_STRING                     *
***********************************************************************/
TCHAR * make_part_and_counts_string( TCHAR * computer, TCHAR * machine, TCHAR * part, TCHAR separator, BOOLEAN need_parms  )
{
static TCHAR buf[PART_NAME_LEN+1+SHOT_LEN+1+SHOT_LEN+5];

NAME_CLASS s;
DB_TABLE   t;
int32      n;
TCHAR    * cp;

cp = buf;
copyfromstr( cp, part, PART_NAME_LEN );

cp += PART_NAME_LEN;
*cp++ = separator;

if ( need_parms )
    {
    n = 0;

    s = shotparm_dbname( computer, machine, part );
    if ( s.file_exists() )
        {
        if (  t.open(s.text(), SHOTPARM_RECLEN, PFL) )
            {
            n = t.nof_recs();
            t.close();
            }
        }

    if ( separator == TabChar )
        {
        cp = copy_w_char( cp, int32toasc(n), separator );
        }
    else
        {
        insalph( cp, n,  SHOT_LEN, SpaceChar, DECIMAL_RADIX );
        cp += SHOT_LEN;
        *cp++ = separator;
        }
    }

n = 0;
s = graphlst_dbname( computer, machine, part );
if ( s.file_exists() )
    {
    if ( t.open(s.text(), GRAPHLST_RECLEN, PFL) )
        {
        n = t.nof_recs();
        t.close();
        }
    }

if ( separator == TabChar )
    {
    copy_w_char( cp, int32toasc(n), NullChar );
    }
else
    {
    insalph( cp, n,  SHOT_LEN, SpaceChar, DECIMAL_RADIX );
    cp += SHOT_LEN;
    *cp = NullChar;
    }

return buf;
}

/***********************************************************************
*                     FILL_PARTS_AND_COUNTS_LISTBOX                    *
***********************************************************************/
static void fill_parts_and_counts_listbox( HWND w, int box, TCHAR * computer, TCHAR * machine, TCHAR separator, BOOLEAN need_parms )
{
NAME_CLASS    s;
DB_TABLE      t;
TCHAR         name[PART_NAME_LEN+1];
TCHAR  *      cp;
HCURSOR       old_cursor;
LISTBOX_CLASS lb;

if ( !lb.init(w, box) )
    return;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

lb.redraw_off();
lb.empty();
SendDlgItemMessage( w, box, LB_RESETCONTENT, TRUE, 0L );
SendDlgItemMessage( w, box, WM_SETREDRAW, 0,  0l );

s = parts_dbname( computer, machine );
if ( s.file_exists() )
    {
    if ( t.open(s.text(), PARTS_RECLEN, PFL) )
        {
        while ( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( name, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
            cp = make_part_and_counts_string( computer, machine, name, separator, need_parms );
            if ( cp )
                lb.add( cp );
            }
        t.close();
        }
    }

lb.setcursel( 0 );
lb.redraw_on();

SetCursor( old_cursor );
}

/***********************************************************************
*                    FILL_PARTS_AND_COUNTS_LISTBOX                     *
***********************************************************************/
void fill_parts_and_counts_listbox( HWND w, int box, TCHAR * computer, TCHAR * machine, TCHAR separator )
{
fill_parts_and_counts_listbox( w, box, computer, machine, separator, TRUE );
}

/***********************************************************************
*                     FILL_PARTS_AND_COUNTS_LISTBOX                    *
***********************************************************************/
void fill_parts_and_counts_listbox( HWND w, int box, TCHAR * computer, TCHAR * machine )
{
fill_parts_and_counts_listbox( w, box, computer, machine, SpaceChar, TRUE );
}

/***********************************************************************
*                   FILL_PARTS_AND_SHOT_COUNT_LISTBOX                  *
***********************************************************************/
void fill_parts_and_shot_count_listbox( HWND w, int box, TCHAR * computer, TCHAR * machine )
{
fill_parts_and_counts_listbox( w, box, computer, machine, TabChar, FALSE );
}
