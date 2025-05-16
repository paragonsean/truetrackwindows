#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\computer.h"
#include "..\include\updown.h"
#include "..\include\verrors.h"

#include "extern.h"
#include "resource.h"
#include "resrc1.h"

const int LB_LINE_LEN   = 2 *(DOW_NAME_LEN + 1) + SHIFT_LEN + 1 + ALPHATIME_LEN + 3;

static HWND    DialogWindow;

static TCHAR   ColonChar = TEXT(':');
static TCHAR   NullChar  = TEXT('\0');
static TCHAR   TabChar   = TEXT('\t');
static TCHAR   ZeroChar  = TEXT('0');

static BOOLEAN ProgramIsUpdating = FALSE;

const int32 NOF_DAYS = 7;
TCHAR * DayName[] = {
    TEXT( "Sunday" ),
    TEXT( "Monday" ),
    TEXT( "Tuesday" ),
    TEXT( "Wednesday" ),
    TEXT( "Thursday" ),
    TEXT( "Friday" ),
    TEXT( "Saturday" ),
    TEXT( "Unknown" )
    };

class EDIT_SHIFT_CLASS
{
public:

int32 shift_day;
int32 true_day;
int32 hours;
int32 minutes;
int32 shift_number;

void init( void ){}

EDIT_SHIFT_CLASS( void ) { init(); }

void    fill_editboxes( void );
TCHAR * lbline( void);
BOOLEAN read_editboxes( void );
BOOLEAN read( DB_TABLE & t );
BOOLEAN read( TCHAR * lb_sorc_line );
BOOLEAN saveall( void );
void    operator=(EDIT_SHIFT_CLASS & sorc );
};

/***********************************************************************
*                          DAY_NAME_FROM_INDEX                         *
***********************************************************************/
TCHAR * day_name_from_index( int32 i )
{
if ( i < 0 || i > NOF_DAYS )
    i = NOF_DAYS;

return DayName[i];
}

/***********************************************************************
*                          INDEX_FROM_DAY_NAME                         *
***********************************************************************/
int32 index_from_day_name( TCHAR * name )
{
int32 i;

for ( i=0; i<NOF_DAYS; i++ )
    {
    if ( strings_are_equal(DayName[i], name) )
        return i;
    }

return 0;
}

/***********************************************************************
*                             SET_TAB_STOPS                            *
***********************************************************************/
static void set_tab_stops( void )
{
const  int NOF_TABS  = 3;
static int tabs[NOF_TABS];
int        x;

x = LOWORD( GetDialogBaseUnits() );
x /= 2;

tabs[0] = ( DOW_NAME_LEN+ 4) * x;
tabs[1] = tabs[0] + (SHIFT_LEN + 4) * x;
tabs[2] = tabs[1] + (DOW_NAME_LEN + 4) * x;

SendDlgItemMessage( DialogWindow, SHIFT_LISTBOX, LB_SETTABSTOPS, (WPARAM) NOF_TABS, (LPARAM) tabs );
}

/***********************************************************************
*                           EDIT_SHIFT_CLASS::READ                          *
***********************************************************************/
BOOLEAN EDIT_SHIFT_CLASS::read( DB_TABLE & t )
{

TCHAR day_name[DOW_NAME_LEN+1];
SYSTEMTIME st;

t.get_alpha( day_name, SHIFT_DAY_OFFSET, DOW_NAME_LEN );
shift_day = index_from_day_name( day_name );
shift_number = t.get_long( SHIFT_NUMBER_OFFSET );
t.get_alpha( day_name, SHIFT_ACTUAL_DAY_OFFSET, DOW_NAME_LEN );
true_day = index_from_day_name( day_name );

t.get_time( st, SHIFT_START_TIME_OFFSET );

hours = st.wHour;
minutes = st.wMinute;

return TRUE;
}

/***********************************************************************
*                         EDIT_SHIFT_CLASS::READ                            *
***********************************************************************/
BOOLEAN EDIT_SHIFT_CLASS::read( TCHAR * lb_sorc_line )
{
TCHAR s[LB_LINE_LEN+1];
TCHAR * cp;
SYSTEMTIME st;

lstrcpyn( s, lb_sorc_line, LB_LINE_LEN+1 );

cp = s;
if ( replace_tab_with_null(cp) )
    {
    shift_day = index_from_day_name( cp );
    cp = nextstring( cp );
    if ( replace_tab_with_null(cp) )
        {
        shift_number = asctoint32( cp );
        cp = nextstring( cp );
        if ( replace_tab_with_null(cp) )
            {
            true_day = index_from_day_name( cp );
            cp = nextstring( cp );
            if ( exthms(st, cp) )
                {
                hours = st.wHour;
                minutes = st.wMinute;
                return TRUE;
                }
            }
        }
    }

return FALSE;
}

/***********************************************************************
*                            READ_EDITBOXES                            *
***********************************************************************/
BOOLEAN EDIT_SHIFT_CLASS::read_editboxes( void )
{
int32 i;
TCHAR buf[MAX_INTEGER_LEN+1];

true_day = shift_day;

for ( i=SUNDAY_RADIO; i<=SATURDAY_RADIO; i++ )
    {
    if ( is_checked(DialogWindow,i) )
        {
        true_day = i-SUNDAY_RADIO;
        break;
        }
    }

get_text( buf, DialogWindow, SHIFT_NAME_EBOX, MAX_INTEGER_LEN );
shift_number = asctoint32( buf );

get_text( buf, DialogWindow, SHIFT_HOUR_EBOX, MAX_INTEGER_LEN );
hours = asctoint32( buf );

get_text( buf, DialogWindow, SHIFT_MINUTE_EBOX, MAX_INTEGER_LEN );
minutes = asctoint32( buf );

return TRUE;
}

/***********************************************************************
*                            TWO_CHAR_INT32                            *
***********************************************************************/
static TCHAR * two_char_int32( int32 i )
{
static TCHAR s[2+1];
TCHAR * cp;

cp = s;

insalph( cp, i, 2, ZeroChar, DECIMAL_RADIX );
cp += 2;
*cp = NullChar;

return s;
}

/***********************************************************************
*                            FILL_EDITBOXES                            *
***********************************************************************/
void EDIT_SHIFT_CLASS::fill_editboxes( void )
{
set_text( DialogWindow, SHIFT_NAME_EBOX,   int32toasc(shift_number) );
set_text( DialogWindow, SHIFT_HOUR_EBOX,   two_char_int32(hours)    );
set_text( DialogWindow, SHIFT_MINUTE_EBOX, two_char_int32(minutes)  );

if ( true_day >= 0 && true_day < NOF_DAYS )
    CheckRadioButton( DialogWindow, SUNDAY_RADIO, SATURDAY_RADIO, SUNDAY_RADIO+true_day );
}

/***********************************************************************
*                           EDIT_SHIFT_CLASS::LBLINE                        *
***********************************************************************/
TCHAR * EDIT_SHIFT_CLASS::lbline( void )
{
static TCHAR s[LB_LINE_LEN+1];
TCHAR * cp;

cp = s;

cp = copy_w_char( cp, day_name_from_index(shift_day), TabChar );
cp = copy_w_char( cp, int32toasc(shift_number), TabChar );
cp = copy_w_char( cp, day_name_from_index(true_day), TabChar );
cp = copystring(  cp, hhmm(hours, minutes) );

return s;
}

/***********************************************************************
*                             OPERATOR =                               *
***********************************************************************/
void EDIT_SHIFT_CLASS::operator=(EDIT_SHIFT_CLASS & sorc )
{
shift_day    = sorc.shift_day;
true_day     = sorc.true_day;
hours        = sorc.hours;
minutes      = sorc.minutes;
shift_number = sorc.shift_number;
}

/***********************************************************************
*                         EDIT_SHIFT_CLASS::SAVEALL                      *
***********************************************************************/
BOOLEAN EDIT_SHIFT_CLASS::saveall( void )
{
DB_TABLE       t;
INT            i;
INT            nof_rows;
LISTBOX_CLASS  lb;
SYSTEMTIME     st;

lb.init( DialogWindow, SHIFT_LISTBOX );

nof_rows = lb.count();
if ( nof_rows <= 0 )
    return FALSE;

init_systemtime_struct( st );

if ( t.open(shift_dbname(), SHIFT_RECLEN, FL) )
    {
    t.empty();
    for ( i=0; i<nof_rows; i++ )
        {
        if ( read(lb.item_text(i)) )
            {
            t.put_alpha( SHIFT_DAY_OFFSET, day_name_from_index(shift_day), DOW_NAME_LEN );
            t.put_long( SHIFT_NUMBER_OFFSET, shift_number, SHIFT_LEN );
            t.put_alpha( SHIFT_ACTUAL_DAY_OFFSET, day_name_from_index(true_day), DOW_NAME_LEN );
            st.wHour = hours;
            st.wMinute = minutes;
            st.wSecond = 0;
            t.put_time( SHIFT_START_TIME_OFFSET, st );
            t.rec_append();
            }
        }
    t.close();
    }

return FALSE;
}

/***********************************************************************
*                              DO_SELECT                               *
***********************************************************************/
static void do_select()
{
LISTBOX_CLASS lb;
EDIT_SHIFT_CLASS d;

lb.init( DialogWindow, SHIFT_LISTBOX );

d.read( lb.selected_text() );

ProgramIsUpdating = TRUE;

d.fill_editboxes();

ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                             FILL_LISTBOX                             *
***********************************************************************/
static void  fill_listbox( void )
{
EDIT_SHIFT_CLASS d;
LISTBOX_CLASS  lb;
DB_TABLE       t;

lb.init( DialogWindow, SHIFT_LISTBOX );
lb.empty();

if ( t.open(shift_dbname(), SHIFT_RECLEN, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        d.read( t );
        lb.add( d.lbline() );
        }
    t.close();
    }
lb.setcursel( 0 );
do_select();
}

/***********************************************************************
*                        REFRESH_LISTBOX_ENTRY                         *
***********************************************************************/
static void refresh_listbox_entry()
{
EDIT_SHIFT_CLASS d;
LISTBOX_CLASS  lb;

if ( ProgramIsUpdating )
    return;

lb.init( DialogWindow, SHIFT_LISTBOX );
d.read( lb.selected_text() );
d.read_editboxes();
lb.replace( d.lbline() );
}

/***********************************************************************
*                              DO_INSERT                               *
***********************************************************************/
static void do_insert()
{
EDIT_SHIFT_CLASS d1;
EDIT_SHIFT_CLASS d2;
LISTBOX_CLASS  lb;
INT            i;
INT            last_index;

lb.init( DialogWindow, SHIFT_LISTBOX );

last_index = lb.count() - 1;
i          = lb.current_index();
d2.read( lb.selected_text() );
if ( i < last_index )
    {
    if ( i > 0 )
        {
        d1.read( lb.item_text(i-1) );
        if ( d1.shift_day != d2.shift_day )
            {
            d1.shift_number++;
            d2 = d1;
            }
        else if ( i < last_index )
            {
            d1 = d2;
            i++;
            d2.read( lb.item_text(i) );
            if ( d1.shift_day != d2.shift_day )
                {
                d1.shift_number++;
                d2 = d1;
                }
            }
        }
    i = lb.insert( i, d2.lbline() );
    }
else
    {
    d2.shift_number++;
    i = lb.add( d2.lbline() );
    }
lb.setcursel( i );
do_select();
}

/***********************************************************************
*                                DO_SAVE                               *
***********************************************************************/
static void do_save( void )
{
EDIT_SHIFT_CLASS d;

d.saveall();
resource_message_box( MainInstance, RESTART_MONITOR_STRING, SHIFT_CHANGES_STRING );

}

/***********************************************************************
*                              DO_REMOVE                               *
***********************************************************************/
static void do_remove()
{
EDIT_SHIFT_CLASS    d1;
EDIT_SHIFT_CLASS    d2;
LISTBOX_CLASS  lb;
INT            i;
INT            last_index;
BOOLEAN        is_only_entry;
lb.init( DialogWindow, SHIFT_LISTBOX );

is_only_entry = TRUE;

i = lb.current_index();
last_index = lb.count() - 1;

d1.read( lb.selected_text() );
if ( i > 0 )
    {
    d2.read( lb.item_text(i-1) );
    if ( d2.shift_day == d1.shift_day )
        is_only_entry = FALSE;
    }

if ( is_only_entry && i < last_index )
    {
    d2.read( lb.item_text(i+1) );
    if ( d2.shift_day == d1.shift_day )
        is_only_entry = FALSE;
    }

if ( is_only_entry )
    {
    resource_message_box( MainInstance, MUST_HAVE_ONE_STRING, CANNOT_COMPLY_STRING );
    return;
    }

if ( last_index > 0 )
    {
    i = lb.current_index();
    lb.remove( i );
    last_index--;
    if ( i > last_index )
        i = last_index;
    lb.setcursel( i );
    do_select();
    }
}

/***********************************************************************
*                             COPY_THIS_DAY                            *
***********************************************************************/
static void copy_this_day( void )
{
EDIT_SHIFT_CLASS    d1;
EDIT_SHIFT_CLASS    d2;
EDIT_SHIFT_CLASS  * d;
LISTBOX_CLASS  lb;
INT            count;
INT            current_index;
INT            first_index;
INT            i;
INT            n;

lb.init( DialogWindow, SHIFT_LISTBOX );
current_index = lb.current_index();
d1.read( lb.selected_text() );

/*
----------------------------------------
Count the number of entries for this day
---------------------------------------- */
count = 0;
first_index = -1;
n = lb.count();
for ( i=0; i<n; i++ )
    {
    d2.read( lb.item_text(i) );
    if ( d2.shift_day == d1.shift_day )
        {
        count++;
        if ( first_index == -1 )
            first_index = i;
        }
    }

d = new EDIT_SHIFT_CLASS[count];
if ( d )
    {
    n = first_index;
    for ( i=0; i<count; i++ )
        {
        d[i].read( lb.item_text(n) );
        n++;
        }

    n = d1.shift_day;

    for ( i=0; i<count; i++ )
        {
        d[i].shift_day -= n;
        d[i].true_day  -= n;
        if ( d[i].true_day  < 0 )
            d[i].true_day += NOF_DAYS;
        }

    lb.redraw_off();
    lb.empty();

    for ( n=0; n<NOF_DAYS; n++ )
        {
        for ( i=0; i<count; i++ )
            {
            lb.add( d[i].lbline() );
            d[i].shift_day++;
            d[i].true_day++;
            if ( d[i].true_day >= NOF_DAYS )
                d[i].true_day -= NOF_DAYS;
            }
        }

    n = lb.count();
    if ( current_index >= n )
        current_index = n - 1;

    lb.setcursel( current_index );
    delete[] d;

    lb.redraw_on();
    do_select();
    }

}

/***********************************************************************
*                         EDIT_SHIFTS_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK edit_shifts_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        DialogWindow = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        limit_text( hWnd, SHIFT_NAME_EBOX, SHIFT_LEN );
        set_tab_stops();
        fill_listbox();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SUNDAY_RADIO:
            case MONDAY_RADIO:
            case TUESDAY_RADIO:
            case WEDNESDAY_RADIO:
            case THURSDAY_RADIO:
            case FRIDAY_RADIO:
            case SATURDAY_RADIO:
                refresh_listbox_entry();
                return TRUE;

            case SHIFT_NAME_EBOX:
            case SHIFT_HOUR_EBOX:
            case SHIFT_MINUTE_EBOX:
                if ( cmd == EN_CHANGE )
                    refresh_listbox_entry();
                return TRUE;

            case SHIFT_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_select();
                    return TRUE;
                    }
                break;

            case COPY_THIS_DAY_BUTTON:
                copy_this_day();
                return TRUE;

            case NEW_SHIFT_BUTTON:
                do_insert();
                return TRUE;

            case REMOVE_SHIFT_BUTTON:
                do_remove();
                return TRUE;

            case SAVE_CHANGES_BUTTON:
                do_save();

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}
