#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\dstat.h"
#include "..\include\v5help.h"

#include "category.h"
#include "resource.h"

#define _MAIN_
#include "..\include\events.h"

static const int CODE_LEN = 2 * DOWNCAT_NUMBER_LEN;

HACCEL  AccelHandle;
TCHAR   AccelName[] = TEXT("EDITDOWN_HOT_KEYS");

HINSTANCE MainInstance;
HWND   MainWindow;
HWND   EditDownWindow;

int32  CurrentPasswordLevel = LOWEST_PASSWORD_LEVEL;

static const int DDE_LINE_LEN  = MACHINE_NAME_LEN + 1 + PART_NAME_LEN + 1 + ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 + 2*(DOWNCAT_NUMBER_LEN + 1) + 2*(DOWNCAT_NAME_LEN + 1) + OPERATOR_NUMBER_LEN;
static const int LB_LINE_LEN   = ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 + 2*(DOWNCAT_NUMBER_LEN + 1) + 2*(DOWNCAT_NAME_LEN+1) + PART_NAME_LEN + 1 + OPERATOR_NUMBER_LEN+1;
static const int NOF_EDITBOXES = CODE_EDITBOX - DATE_EDITBOX + 1;

TCHAR MyComputer[COMPUTER_NAME_LEN+1] = TEXT("C01");
TCHAR MyMachine[MACHINE_NAME_LEN+1]   = TEXT("M04");
TCHAR * NotFoundError = 0;
DB_TABLE T;

static TIME_CLASS OriginalTime;
static BOOLEAN    IsInsert   = FALSE;
static BOOLEAN    HaveChange = FALSE;

static int NofLines     = 10000;
static int TopLine      = 0;
static int CurrentLine  = 0;
static int LinesPerPage = 10;

TCHAR MyClassName[]      = TEXT("EditDown");
TCHAR MainHelpFileName[] = TEXT( "v5help.hlp" );
STRING_CLASS MyWindowTitle;

static TCHAR   NullChar = TEXT('\0');
static TCHAR   TabChar  = TEXT('\t');

static WNDPROC OldEditProc[NOF_EDITBOXES];

CATEGORY_CLASS Category;
SUBCAT_CLASS   SubCategory;

static BOOLEAN ProgramIsUpdating = FALSE;

class EDITDOWN_CLASS : public TIME_CLASS
{
private:

TCHAR      category[DOWNCAT_NUMBER_LEN+1];
TCHAR      subcategory[DOWNCAT_NUMBER_LEN+1];
TCHAR      part[PART_NAME_LEN+1];
TCHAR      worker[OPERATOR_NUMBER_LEN+1];

void init( void ){ *category = NullChar; *subcategory = NullChar; *part=NullChar; *worker=NullChar; }

public:

        EDITDOWN_CLASS( void );
void    operator=( const TIME_CLASS & sorc ) { *((TIME_CLASS *)this) = sorc; }
BOOLEAN operator==( const TIME_CLASS & sorc ) { return (*((TIME_CLASS *)this) == sorc); }
TCHAR * lbline( void);
TCHAR * ddeline( void );
void    fill_dstat( DSTAT_CLASS & d );
void    fill_editboxes( void );
BOOLEAN read_editboxes( void );
BOOLEAN read( DB_TABLE & t );
BOOLEAN read( TCHAR * lb_sorc_line );
BOOLEAN put_record( void );
};

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( TCHAR * fname, UINT helptype, DWORD context )
{
TCHAR path[MAX_PATH+1];

if ( get_exe_directory(path) )
    {
    lstrcat( path, fname );
    WinHelp( MainWindow, path, helptype, context );
    }

}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( UINT helptype, DWORD context )
{
gethelp( MainHelpFileName, helptype, context );
}

/***********************************************************************
*                                 CODE                                 *
***********************************************************************/
static TCHAR * code( TCHAR * cat, TCHAR * subcat )
{
static TCHAR s[CODE_LEN+1];
TCHAR * cp;

cp = s;
lstrcpy( cp, cat );
cp += lstrlen(cp);
lstrcpy( cp, subcat );

return s;
}

/***********************************************************************
*                               EXTCODE                                *
***********************************************************************/
static BOOLEAN extcode( TCHAR * category, TCHAR * subcategory, TCHAR * mycode )
{

if ( lstrlen(mycode) >= DOWNCAT_NUMBER_LEN )
    {
    copychars( category, mycode, DOWNCAT_NUMBER_LEN );
    mycode += DOWNCAT_NUMBER_LEN;
    }
else
    {
    copychars( category, NO_DOWNCAT_NUMBER, DOWNCAT_NUMBER_LEN );
    }

*( category+DOWNCAT_NUMBER_LEN) = NullChar;


if ( lstrlen(mycode) >= DOWNCAT_NUMBER_LEN )
    copychars( subcategory, mycode, DOWNCAT_NUMBER_LEN );
else
    copychars( subcategory, NO_DOWNCAT_NUMBER, DOWNCAT_NUMBER_LEN );
*( subcategory+DOWNCAT_NUMBER_LEN) = NullChar;

return TRUE;
}

/***********************************************************************
*                            FILL_CATBOXES                             *
***********************************************************************/
static void fill_catboxes( void )
{
LISTBOX_CLASS lb;
TCHAR mycode[CODE_LEN+1];
TCHAR cat[DOWNCAT_NUMBER_LEN+1];
TCHAR subcat[DOWNCAT_NUMBER_LEN+1];

get_text( mycode, EditDownWindow, CODE_EDITBOX, CODE_LEN );
extcode( cat, subcat, mycode );

Category.find( cat );
lb.init( EditDownWindow, CAT_COMBOBOX );
lb.setcursel( Category.name() );

lb.init( EditDownWindow, SUB_CAT_COMBOBOX );
lb.empty();
SubCategory.rewind();
while ( SubCategory.next() )
    {
    if ( lstrcmp(SubCategory.cat(), cat) == 0 )
        lb.add( SubCategory.name() );
    }

if ( SubCategory.find(cat, subcat) )
    lb.setcursel( SubCategory.name() );
}

/***********************************************************************
*                    EDITDOWN_CLASS::EDITDOWN_CLASS                    *
***********************************************************************/
EDITDOWN_CLASS::EDITDOWN_CLASS( void )
{
lstrcpy( category,    DOWNCAT_SYSTEM_CAT );
lstrcpy( subcategory, DOWNCAT_UP_SUBCAT  );
lstrcpy( part,        NO_PART            );
lstrcpy( worker,      NO_OPERATOR_NUMBER );
}

/***********************************************************************
*                    EDITDOWN_CLASS::READ_EDITBOXES                    *
***********************************************************************/
BOOLEAN EDITDOWN_CLASS::read_editboxes( void )
{
SYSTEMTIME st;
LISTBOX_CLASS lb;
TCHAR mydate[ALPHADATE_LEN+1];
TCHAR mytime[ALPHATIME_LEN+1];
TCHAR mycode[CODE_LEN+1];

init_systemtime_struct( st );

if ( get_text(mydate, EditDownWindow, DATE_EDITBOX, ALPHADATE_LEN) )
    extmdy( st, mydate );

if ( get_text(mytime, EditDownWindow, TIME_EDITBOX, ALPHATIME_LEN) )
    exthms( st, mytime );

set( st );

get_text(mycode, EditDownWindow, CODE_EDITBOX, CODE_LEN );
extcode( category, subcategory, mycode );

lb.init( EditDownWindow, PART_COMBOBOX );
lstrcpy( part, lb.selected_text() );

lb.init( EditDownWindow, OPERATOR_COMBOBOX );
lstrcpy( worker, lb.selected_text() );

return TRUE;
}

/***********************************************************************
*                    EDITDOWN_CLASS::FILL_EDITBOXES                    *
***********************************************************************/
void EDITDOWN_CLASS::fill_editboxes( void )
{
LISTBOX_CLASS lb;
SYSTEMTIME    st;

st = system_time();

OriginalTime.set( st );

set_text( EditDownWindow, DATE_EDITBOX, alphadate(st) );
set_text( EditDownWindow, TIME_EDITBOX, alphatime(st) );
set_text( EditDownWindow, CODE_EDITBOX, code(category, subcategory) );

fill_catboxes();

lb.init( EditDownWindow, PART_COMBOBOX );
lb.setcursel( part );

lb.init( EditDownWindow, OPERATOR_COMBOBOX );
lb.setcursel( worker );
}

/***********************************************************************
*                         EDITDOWN_CLASS::READ                         *
***********************************************************************/
BOOLEAN EDITDOWN_CLASS::read( DB_TABLE & t )
{
SYSTEMTIME st;

init_systemtime_struct( st );

t.get_date( st, DOWNTIME_DATE_OFFSET );
t.get_time( st, DOWNTIME_TIME_OFFSET );

set( st );

t.get_alpha( category,    DOWNTIME_CAT_OFFSET,       DOWNCAT_NUMBER_LEN );
t.get_alpha( subcategory, DOWNTIME_SUBCAT_OFFSET,    DOWNCAT_NUMBER_LEN );
t.get_alpha( part,        DOWNTIME_PART_NAME_OFFSET, PART_NAME_LEN );
t.get_alpha( worker,      DOWNTIME_OPERATOR_OFFSET,  OPERATOR_NUMBER_LEN );

return TRUE;
}

/***********************************************************************
*                              FILL_DSTAT                              *
***********************************************************************/
void EDITDOWN_CLASS::fill_dstat( DSTAT_CLASS & d )
{
d.set_cat( category );
d.set_subcat( subcategory );
d.set_time( system_time() );
}

/***********************************************************************
*                         EDITDOWN_CLASS::READ                         *
***********************************************************************/
BOOLEAN EDITDOWN_CLASS::read( TCHAR * lb_sorc_line )
{
TCHAR      s[LB_LINE_LEN+1];
TCHAR    * cp;
SYSTEMTIME st;

lstrcpyn( s, lb_sorc_line, LB_LINE_LEN+1 );

init();

cp = s;

/*
----
Date
---- */
replace_tab_with_null( cp );
init_systemtime_struct( st );
extmdy( st, cp );

/*
----
Time
---- */
cp = nextstring( cp );
replace_tab_with_null( cp );
exthms( st, cp );

set( st );

/*
--------
Category
-------- */
cp = nextstring( cp );
replace_tab_with_null( cp );
lstrcpyn( category, cp, DOWNCAT_NUMBER_LEN+1 );

/*
-----------
Subcategory
----------- */
cp = nextstring( cp );
replace_tab_with_null( cp );
lstrcpyn( subcategory, cp, DOWNCAT_NUMBER_LEN+1 );

/*
----------------------
Skip the category name
---------------------- */
cp = nextstring( cp );
replace_tab_with_null( cp );

/*
-------------------------
Skip the subcategory name
------------------------- */
cp = nextstring( cp );
replace_tab_with_null( cp );

/*
---------
Part Name
--------- */
cp = nextstring( cp );
replace_tab_with_null( cp );
lstrcpyn( part, cp, PART_NAME_LEN+1 );

/*
-------------
Operator Name
------------- */
cp = nextstring( cp );
lstrcpyn( worker, cp, OPERATOR_NUMBER_LEN+1 );

return TRUE;
}

/***********************************************************************
*                        EDITDOWN_CLASS::LBLINE                        *
***********************************************************************/
TCHAR * EDITDOWN_CLASS::lbline( void )
{
static TCHAR buf[LB_LINE_LEN+1];
TCHAR * cp;

cp = buf;

cp = copy_w_char( cp, alphadate(system_time()), TabChar );
cp = copy_w_char( cp, alphatime(system_time()), TabChar );
cp = copy_w_char( cp, category,      TabChar );
cp = copy_w_char( cp, subcategory,   TabChar );

Category.find( category );
cp = copy_w_char( cp, Category.name(), TabChar );

SubCategory.find( category, subcategory );
cp = copy_w_char( cp, SubCategory.name(), TabChar );

cp = copy_w_char( cp, part, TabChar );
lstrcpy( cp, worker );

return buf;
}

/***********************************************************************
*                        EDITDOWN_CLASS::DDELINE                       *
***********************************************************************/
TCHAR * EDITDOWN_CLASS::ddeline( void )
{
static TCHAR buf[DDE_LINE_LEN+1];
TCHAR * cp;

cp = buf;

cp = copy_w_char( cp, MyMachine, TabChar );
cp = copy_w_char( cp, part, TabChar );
cp = copy_w_char( cp, alphadate(system_time()), TabChar );
cp = copy_w_char( cp, alphatime(system_time()), TabChar );
cp = copy_w_char( cp, category,      TabChar );
cp = copy_w_char( cp, subcategory,   TabChar );

Category.find( category );
cp = copy_w_char( cp, Category.name(), TabChar );

SubCategory.find( category, subcategory );
cp = copy_w_char( cp, SubCategory.name(), TabChar );

lstrcpy( cp, worker );

return buf;
}

/***********************************************************************
*                             PUT_RECORD                               *
***********************************************************************/
BOOLEAN EDITDOWN_CLASS::put_record( void )
{
return make_downtime_record( T, system_time(), category, subcategory, worker, part );
}

/***********************************************************************
*                          ENABLE_PURGE_BUTTON                         *
***********************************************************************/
static void enable_purge_button( void )
{
TCHAR * cp;
int32   level_needed;

cp = get_ini_string( TEXT("visitrak.ini"), TEXT("ButtonLevels"), TEXT("DowntimePurgeButton") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    {
    level_needed = asctoint32( cp );
    if ( CurrentPasswordLevel >= level_needed )
        EnableWindow( GetDlgItem( EditDownWindow,PURGE_BUTTON), TRUE );

    }
}


/***********************************************************************
*                          PASSWORD_LEVEL_CALLBACK                     *
***********************************************************************/
void password_level_callback( TCHAR * topic, short item, HDDEDATA edata )
{
TCHAR buf[MAX_INTEGER_LEN+3];
DWORD bytes_copied;
DWORD bufsize = sizeof( buf );

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_copied = DdeGetData( edata, (LPBYTE) buf, bufsize, 0 );
if ( bytes_copied > 0 )
    {
    *(buf+MAX_INTEGER_LEN) = NullChar;
    fix_dde_name( buf );
    CurrentPasswordLevel = asctoint32( buf );
    enable_purge_button();
    }
}

/***********************************************************************
*                             SET_TAB_STOPS                            *
***********************************************************************/
static void set_tab_stops( void )
{
const  int NOF_TABS  = 7;
static int tabs[NOF_TABS];
int        x;

x = LOWORD( GetDialogBaseUnits() );
x /= 2;

tabs[0] = (ALPHADATE_LEN + 1) * x;
tabs[1] = tabs[0] + (ALPHATIME_LEN + 1) * x;
tabs[2] = tabs[1] + (DOWNCAT_NUMBER_LEN+1) * x;
tabs[3] = tabs[2] + (DOWNCAT_NUMBER_LEN + 3) * x;
tabs[4] = tabs[3] + (DOWNCAT_NAME_LEN + 4) * x;
tabs[5] = tabs[4] + (DOWNCAT_NAME_LEN + 4) * x;
tabs[6] = tabs[5] + (PART_NAME_LEN + 2) * x;

SendDlgItemMessage( EditDownWindow, DOWNTIME_LISTBOX, LB_SETTABSTOPS, (WPARAM) NOF_TABS, (LPARAM) tabs );
}

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
static TCHAR * getline( int i )
{
EDITDOWN_CLASS e;

if ( T.goto_record_number(i) )
    {
    T.get_current_record( NO_LOCK );
    e.read( T );
    return e.lbline();
    }

return NotFoundError;
}

/***********************************************************************
*                             FILL_LISTBOX                             *
***********************************************************************/
static void fill_listbox( void )
{
int i;
int lastline;
LISTBOX_CLASS lb;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
lb.empty();

lastline = TopLine + LinesPerPage - 1;
if ( lastline >= NofLines )
    lastline = NofLines - 1;

i = TopLine;
while ( i <= lastline )
    {
    lb.add( getline(i) );
    i++;
    }

if ( CurrentLine >= TopLine && CurrentLine < (TopLine+LinesPerPage) )
    lb.setcursel( CurrentLine - TopLine );
}

/***********************************************************************
*                            SET_SCROLLBAR                             *
***********************************************************************/
static void set_scrollbar( void )
{
HWND w;
SCROLLINFO si;

w = GetDlgItem( EditDownWindow, DOWNTIME_SCROLBAR );

si.cbSize = sizeof(si);
si.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;
si.nMin   = 0;
si.nMax   = NofLines - 1;
si.nPage  = LinesPerPage;
si.nPos   = TopLine;

SetScrollInfo( w, SB_CTL, &si, TRUE );
}

/***********************************************************************
*                            SET_SCROLLBAR_POS                         *
***********************************************************************/
static void set_scrollbar_pos( void )
{
HWND           w;
SCROLLINFO    si;

w = GetDlgItem( EditDownWindow, DOWNTIME_SCROLBAR );

si.cbSize = sizeof(si);
si.fMask  = SIF_POS;
si.nPos   = TopLine;
SetScrollInfo( w, SB_CTL, &si, TRUE );
}

/***********************************************************************
*                             OK_TO_SCROLL                             *
***********************************************************************/
BOOLEAN ok_to_scroll( void )
{
if ( !HaveChange )
    return TRUE;

if ( resource_message_box(MainInstance, CONTINUE_SCROLL_STRING, SCROLL_CHANGES_STRING, MB_YESNO) == IDYES )
    {
    HaveChange = FALSE;
    IsInsert   = FALSE;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         SCROLL_DOWN_ONE_LINE                         *
***********************************************************************/
void scroll_down_one_line( void )
{
int i;
int lastline;

if ( !ok_to_scroll() )
    return;

i        = TopLine + LinesPerPage - 1;
lastline = NofLines - 1;

if ( i < lastline )
    {
    TopLine++;
    set_scrollbar_pos();
    fill_listbox();
    }

}

/***********************************************************************
*                          SCROLL_UP_ONE_LINE                          *
***********************************************************************/
void scroll_up_one_line( void )
{

if ( !ok_to_scroll() )
    return;

if ( TopLine > 0 )
    {
    TopLine--;
    set_scrollbar_pos();
    fill_listbox();
    }

}

/***********************************************************************
*                          SCROLL_UP_ONE_PAGE                          *
***********************************************************************/
void scroll_up_one_page( void )
{
int i;

if ( !ok_to_scroll() )
    return;

i = TopLine - LinesPerPage;
maxint( i, 0 );
if ( i < TopLine )
    {
    TopLine = i;
    set_scrollbar_pos();
    fill_listbox();
    }
}

/***********************************************************************
*                        SCROLL_DOWN_ONE_PAGE                          *
***********************************************************************/
void scroll_down_one_page( void )
{
int i;

if ( !ok_to_scroll() )
    return;

i = TopLine + LinesPerPage;
minint( i, (NofLines - LinesPerPage) );
if ( i > TopLine )
    {
    TopLine = i;
    set_scrollbar_pos();
    fill_listbox();
    }

}

/***********************************************************************
*                              DO_SCROLL                               *
***********************************************************************/
static void do_scroll( WPARAM wParam )
{
int scroll_code;
int pos;

scroll_code = (int) LOWORD( wParam );
pos         = (int) HIWORD( wParam );

switch ( scroll_code )
    {
    case SB_LINEDOWN:
        scroll_down_one_line();
        break;

    case SB_LINEUP:
        scroll_up_one_line();
        break;

    case SB_PAGEDOWN:
        scroll_down_one_page();
        break;

    case SB_PAGEUP:
        scroll_up_one_page();
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        TopLine = pos;
        if ( scroll_code == SB_THUMBPOSITION )
            set_scrollbar_pos();
        fill_listbox();
        break;

    }

}

/***********************************************************************
*                           GET_CURRENTLINE                            *
***********************************************************************/
static void get_currentline( void )
{
int inserted_index;
int i;
LISTBOX_CLASS lb;

/*
---------------------------------------------
If I am sitting on an inserted line, the file
doesn't know anything about the line I am on.
--------------------------------------------- */
if ( IsInsert )
    inserted_index = CurrentLine - TopLine;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
i = lb.selected_index();

if ( i != NO_INDEX )
    {
    CurrentLine = TopLine + i;
    if ( IsInsert && i >= inserted_index )
        CurrentLine--;
    }
}

/***********************************************************************
*                              DO_SELECT                               *
***********************************************************************/
static void do_select()
{
LISTBOX_CLASS  lb;
EDITDOWN_CLASS e;

/*
-----------------------------------------------------------
If I was inserting and the operator didn't save the changes
reload the listbox before doing anything else.
----------------------------------------------------------- */
if ( IsInsert )
    fill_listbox();

lb.init( EditDownWindow, DOWNTIME_LISTBOX );

e.read( lb.selected_text() );

ProgramIsUpdating = TRUE;

e.fill_editboxes();

ProgramIsUpdating = FALSE;

IsInsert   = FALSE;
HaveChange = FALSE;
}

/***********************************************************************
*                         DO_EDITBOX_KEYPRESS                          *
*                                                                      *
*               -1 tells listbox to handle the keypress.               *
*               -2 tells listbox I handled the keypress.               *
*                                                                      *
***********************************************************************/
static int do_editbox_keypress( int id, int current_index )
{
int i;

if ( HaveChange )
    {
    fill_listbox();
    HaveChange = FALSE;
    IsInsert   = FALSE;
    }

switch ( id )
    {
    case VK_DOWN:
        if ( current_index == (LinesPerPage - 1) )
            {
            i = TopLine + current_index;
            if ( i < (NofLines - 1) )
                {
                CurrentLine = i + 1;
                scroll_down_one_line();
                do_select();
                return -2;
                }
            }
        break;

    case VK_UP:
        if ( current_index == 0 )
            {
            if ( TopLine > 0 )
                {
                CurrentLine = TopLine - 1;
                scroll_up_one_line();
                do_select();
                return -2;
                }
            }
        break;

    case VK_PRIOR:
        if ( current_index == 0 )
            {
            if ( TopLine > 0 )
                {
                CurrentLine = TopLine - LinesPerPage;
                maxint( CurrentLine, 0 );
                scroll_up_one_page();
                do_select();
                return -2;
                }
            }

        break;

    case VK_NEXT:
        if ( current_index == (LinesPerPage - 1) )
            {
            i = TopLine + current_index;
            if ( i < (NofLines - 1) )
                {
                CurrentLine = i + LinesPerPage;
                minint( CurrentLine, (NofLines - 1) );
                scroll_down_one_page();
                do_select();
                return -2;
                }
            }
        break;

    }

return -1;
}

/***********************************************************************
*                            PURGE_DOWNTIME                            *
***********************************************************************/
static BOOLEAN purge_downtimes( void )
{
TCHAR * filename;
int32   n;
BOOLEAN status;

status = FALSE;

if ( T.open_status() )
    T.close();

filename = downtime_dbname( MyComputer, MyMachine );

if ( file_exists(filename) )
    {
    if ( T.open( filename, DOWNTIME_RECLEN, FL) )
        {
        n = T.nof_recs();
        if ( n > 0 )
            {
            T.goto_record_number( n-1 );
            T.get_current_record( NO_LOCK );
            }
        T.empty();
        if ( n > 0 )
            T.rec_append();
        T.close();
        status = TRUE;
        }
    }

return status;
}

/***********************************************************************
*                             INIT_GLOBALS                             *
***********************************************************************/
static BOOLEAN init_globals( void )
{
LISTBOX_CLASS lb;
TCHAR       * filename;

set_text( EditDownWindow, MACHINE_NAME_TEXTBOX, MyMachine );

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
LinesPerPage = lb.visible_row_count();

if ( T.open_status() )
    T.close();

filename = downtime_dbname( MyComputer, MyMachine );
if ( !file_exists(filename) )
    T.create( filename );

if ( T.open(filename, DOWNTIME_RECLEN, PFL) )
    {
    NofLines = T.nof_recs();
    TopLine = NofLines - LinesPerPage;
    maxint( TopLine, 0 );
    set_scrollbar();
    CurrentLine = NO_INDEX;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        FILL_GLOBAL_LISTBOXES                         *
***********************************************************************/
static void fill_global_listboxes( void )
{
LISTBOX_CLASS lb;
DB_TABLE t;
TCHAR part[PART_NAME_LEN+1];
TCHAR worker[OPERATOR_NUMBER_LEN+1];

lb.init( EditDownWindow, PART_COMBOBOX );
lb.empty();

t.open( parts_dbname(MyComputer, MyMachine), PARTS_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( part, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
    lb.add( part );
    }
t.close();

lb.init( EditDownWindow, OPERATOR_COMBOBOX );
lb.empty();

t.open( operator_dbname(), OPERATOR_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( worker, OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
    lb.add( worker );
    }
t.close();

lb.init( EditDownWindow, CAT_COMBOBOX );
Category.rewind();
while ( Category.next() )
    lb.add( Category.name() );

}

/***********************************************************************
*                           SELECT_LAST_LINE                           *
***********************************************************************/
static void select_last_line()
{
LISTBOX_CLASS  lb;
HWND w;
int  i;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
i = lb.count();
if ( i > 0 )
    {
    i--;
    lb.setcursel( i );
    get_currentline();
    do_select();
    w = GetDlgItem( EditDownWindow, CODE_EDITBOX );
    SetFocus( w );
    eb_select_all( w );
    }

}

/***********************************************************************
*                        REFRESH_LISTBOX_ENTRY                         *
***********************************************************************/
static void refresh_listbox_entry()
{
EDITDOWN_CLASS e;
LISTBOX_CLASS  lb;

if ( ProgramIsUpdating )
    return;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
e.read_editboxes();
lb.replace( e.lbline() );

}

/***********************************************************************
*                           REFRESH_SUBCAT                             *
***********************************************************************/
static void refresh_subcat( void )
{
LISTBOX_CLASS  lb;
TCHAR mycode[CODE_LEN+1];
TCHAR cat[DOWNCAT_NUMBER_LEN+1];
TCHAR subcat[DOWNCAT_NUMBER_LEN+1];

lb.init( EditDownWindow, SUB_CAT_COMBOBOX );

get_text( mycode, EditDownWindow, CODE_EDITBOX, CODE_LEN );
if ( lstrlen(mycode) < DOWNCAT_NUMBER_LEN )
    return;

extcode( cat, subcat, mycode );

if ( SubCategory.findname( cat, lb.selected_text()) )
    {
    lstrcpy( subcat, SubCategory.subcat() );
    set_text( EditDownWindow, CODE_EDITBOX, code(cat, subcat) );
    }

}

/***********************************************************************
*                             REFRESH_CAT                              *
***********************************************************************/
static void refresh_cat( void )
{
LISTBOX_CLASS  lb;
TCHAR mycode[CODE_LEN+1];
TCHAR category[DOWNCAT_NUMBER_LEN+1];
TCHAR subcategory[DOWNCAT_NUMBER_LEN+1];

lb.init( EditDownWindow, CAT_COMBOBOX );

get_text( mycode, EditDownWindow, CODE_EDITBOX, CODE_LEN );
if ( lstrlen(mycode) < DOWNCAT_NUMBER_LEN )
    return;

extcode( category, subcategory, mycode );

if ( Category.findname(lb.selected_text()) )
    {
    lstrcpy( category, Category.cat() );
    set_text( EditDownWindow, CODE_EDITBOX, code(category, subcategory) );
    }

}

/***********************************************************************
*                              ADD_RECORD                              *
***********************************************************************/
static void add_record( void )
{
LISTBOX_CLASS  lb;
EDITDOWN_CLASS e;
int  i;
int  midline;
int  n;
HWND w;
SYSTEMTIME t;
TIME_CLASS oldtime;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
n = lb.count();

/*
-----------------------------------------------------
Don't do anything if the selected line is not showing
----------------------------------------------------- */
if ( n > 0 )
    if ( CurrentLine < TopLine || CurrentLine >= (TopLine+LinesPerPage) )
        return;

if ( n > 0 )
    i = lb.current_index();
else
    i = 0;

if ( n > 0 )
    e.read( lb.selected_text() );

/*
--------------------------------------------------------------
If this is the last record then I am probably adding something
for the current time.
-------------------------------------------------------------- */
if ( n == 0 || CurrentLine == (NofLines - 1) )
    {
    GetLocalTime( &t );
    e.set( t );
    }

if ( n == LinesPerPage )
    {
    /*
    -------------------------------------------
    The listbox is full so I have to delete one
    ------------------------------------------- */
    midline = LinesPerPage / 2;
    if ( i < midline )
        lb.remove( n-1 );
    else
        {
        TopLine++;
        lb.remove( 0 );
        i--;
        }
    }

if ( n > 0 )
    i++;
lb.insert( i, e.lbline() );
lb.setcursel( i );
get_currentline();

oldtime = OriginalTime;
do_select();

/*
-----------------------------------------------------------------
Restore the old time so save_changes will know this is a new time
----------------------------------------------------------------- */
OriginalTime = oldtime;

IsInsert   = TRUE;
HaveChange = TRUE;

w = GetDlgItem( EditDownWindow, CODE_EDITBOX );
SetFocus( w );
eb_select_all( w );
}

/***********************************************************************
*                              SUB_PROC                                *
* Subclass the four edit controls so if the operator presses an up     *
* or down arrow, the current selection in the listbox changes.         *
***********************************************************************/
LRESULT CALLBACK sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  i;
LISTBOX_CLASS lb;

if ( msg == WM_KEYDOWN )
    {
    if ( wParam == VK_UP || wParam == VK_DOWN )
        {
        lb.init( EditDownWindow, DOWNTIME_LISTBOX );
        i = lb.current_index();
        if ( do_editbox_keypress(wParam, i) == -1 )
            {
            if ( wParam == VK_UP && i > 0 )
                i--;
            else if ( wParam == VK_DOWN && i < (lb.count() - 1) )
                i++;
            else
                return 0;

            lb.setcursel( i );
            }

        get_currentline();
        do_select();
        SetFocus( w );
        eb_select_all( w );
        return 0;
        }

    }

i = GetWindowLong( w, GWL_ID ) - DATE_EDITBOX;
return CallWindowProc( OldEditProc[i], w, msg, wParam, lParam );
}

/***********************************************************************
*                          SUB_CLASS_CONTROLS                          *
***********************************************************************/
static void sub_class_controls( HWND parent )
{
int i;
HWND w;

for ( i=0; i<NOF_EDITBOXES; i++ )
    {
    w = GetDlgItem( parent, DATE_EDITBOX+i );
    OldEditProc[i] = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) sub_proc );
    }
}

/***********************************************************************
*                               RECTIME                                *
***********************************************************************/
SYSTEMTIME & rectime( int32 record_number )
{
static SYSTEMTIME st;

init_systemtime_struct( st );

T.goto_record_number( record_number );
T.get_current_record( NO_LOCK );
T.get_date( st, DOWNTIME_DATE_OFFSET );
T.get_time( st, DOWNTIME_TIME_OFFSET );

return st;
}

/***********************************************************************
*                               RESTART                                *
***********************************************************************/
void restart( void )
{

EDITDOWN_CLASS e;
LISTBOX_CLASS  lb;
BOOLEAN        have_current_line;
int            row_in_listbox;

have_current_line = FALSE;

/*
-----------------------------------------------------
Don't do anything if the selected line is not showing
----------------------------------------------------- */
if ( CurrentLine >= TopLine && CurrentLine < (TopLine+LinesPerPage) )
    {
    lb.init( EditDownWindow, DOWNTIME_LISTBOX );
    row_in_listbox = lb.current_index();
    e.read( lb.selected_text() );
    have_current_line = TRUE;
    }

init_globals();

if ( have_current_line )
    {
    T.reset_search_mode();
    e.put_record();
    if ( T.get_next_key_match(2, NO_LOCK) )
        {
        CurrentLine = T.current_record_number();
        if ( CurrentLine >= row_in_listbox )
            TopLine = CurrentLine - row_in_listbox;
        else
            TopLine = 0;
        }
    }

set_scrollbar_pos();
fill_listbox();
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes( void )
{
int32   firstrec;
int32   lastrec;
int32   rn;
int32   new_rec_number;
TCHAR * filename;

EDITDOWN_CLASS e;
TIME_CLASS     tc;
BOOLEAN        need_insert;
LISTBOX_CLASS  lb;
HCURSOR        old_cursor;
DSTAT_CLASS    d;

if ( !HaveChange )
    {
    resource_message_box( MainInstance, NO_CHANGES_MADE_STRING, NOTHING_TO_SAVE_STRING );
    return;
    }

new_rec_number = NO_INDEX;
lb.init( EditDownWindow, DOWNTIME_LISTBOX );

/*
----------------------------------------------------
Don't do anything if the current line is not showing
---------------------------------------------------- */
if ( CurrentLine < TopLine || CurrentLine >= (TopLine+LinesPerPage) )
    return;

e.read( lb.selected_text() );

if ( IsInsert && e == OriginalTime )
    {
    if ( resource_message_box(MainInstance, WILL_REPLACE_ORIGINAL_STRING, NO_DATE_CHANGE_STRING, MB_YESNO) == IDNO )
    return;
    }

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

if ( !waitfor_shotsave_semaphor(MyComputer, MyMachine) )
    {
    resource_message_box( MainInstance, UNABLE_TO_COMPLY_STRING, SEMAPHOR_FAIL_STRING );
    SetCursor( old_cursor );
    return;
    }

/*
---------------------------
Reopen table with full lock
--------------------------- */
T.close();

filename = downtime_dbname( MyComputer, MyMachine );

if ( T.open(filename, DOWNTIME_RECLEN, FL) )
    {

    /*
    -------------------------------------
    Get the first and last record numbers
    ------------------------------------- */
    firstrec = 0;
    lastrec  = T.nof_recs();
    need_insert = FALSE;

    if ( !lastrec )
        {
        new_rec_number = 0;
        need_insert = TRUE;
        }

    if ( lastrec > 0 )
        lastrec--;

    if ( new_rec_number == NO_INDEX )
        {
        tc.set( rectime(firstrec) );
        if ( e <= tc )
            {
            new_rec_number = 0;
            if ( e < tc )
                need_insert = TRUE;
            }
        }

    if ( new_rec_number == NO_INDEX )
        {
        /*
        ---------------------
        Check the last record
        --------------------- */
        tc.set( rectime(lastrec) );
        if ( e > tc )
            {
            new_rec_number = lastrec+1;
            need_insert = TRUE;
            }
        else if ( e == tc )
            new_rec_number = lastrec;

        }

    if ( new_rec_number == NO_INDEX )
        {
        while ( firstrec < lastrec-1 )
            {
            rn = (firstrec + lastrec)/2;
            tc.set( rectime(rn) );

            if ( e == tc )
                {
                new_rec_number = rn;
                break;
                }

            if ( e > tc )
                firstrec = rn;
            else
                lastrec = rn;
            }

        if ( new_rec_number == NO_INDEX )
            {
            new_rec_number = lastrec;
            need_insert = TRUE;
            }
        }


    if ( new_rec_number != NO_INDEX )
        {
        if ( e.put_record() )
            {
            if ( new_rec_number >= (int32) T.nof_recs() )
                T.rec_append();
            else
                {
                T.goto_record_number( new_rec_number );
                if ( need_insert )
                    T.rec_insert();
                else
                    T.rec_update();
                }

            /*
            -------------------------------------------------
            If I made a new record and this was not an insert
            then I need to delete the original.
            ------------------------------------------------- */
            if ( need_insert && !IsInsert )
                {
                e = OriginalTime;
                e.put_record();
                T.reset_search_mode();
                if ( T.get_next_key_match(2, NO_LOCK) )
                    T.rec_delete();
                }

            /*
            ----------------------------------
            Nothing will happen if this is old
            ---------------------------------- */
            e.fill_dstat( d );
            d.put( MyComputer, MyMachine );
            }
        }

    HaveChange = FALSE;
    IsInsert   = FALSE;
    poke_data( DDE_MONITOR_TOPIC, ItemList[DOWN_DATA_INDEX].name, e.ddeline() );
    }
else
    {
    resource_message_box( MainInstance, CANNOT_UPDATE_STRING, UNABLE_TO_COMPLY_STRING );
    }

free_shotsave_semaphore();

SetCursor( old_cursor );

restart();
}

/***********************************************************************
*                           GOTO_CURRENTLINE                           *
***********************************************************************/
static void goto_currentline( void )
{
LISTBOX_CLASS lb;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
lb.setcursel( CurrentLine - TopLine );
}

/***********************************************************************
*                           POSITION_MYSELF                            *
***********************************************************************/
static void position_myself( void )
{
RECT mr;
RECT dr;
long delta;

HWND desktop_window;

GetWindowRect( MainWindow, &mr );
dr = mr;

/*
------------------------------------
Get the bounds of the desktop window
------------------------------------ */
desktop_window = GetDesktopWindow();
if ( desktop_window )
    {
    GetClientRect( desktop_window, &dr );
    }

delta = (dr.right - dr.left) - (mr.right - mr.left);
delta /= 2;

mr.right += delta;
mr.left  += delta;

delta = (dr.bottom - dr.top) - (mr.bottom - mr.top);
delta /= 2;

mr.bottom += delta;
mr.top   += delta;

MoveWindow( MainWindow, mr.left, mr.top, mr.right-mr.left, mr.bottom-mr.top, TRUE );
}

/***********************************************************************
*                            EDITDOWN_PROC                             *
***********************************************************************/
BOOL CALLBACK editdown_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        EditDownWindow = hWnd;
        sub_class_controls( hWnd );
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        position_myself();
        set_tab_stops();
        if ( init_globals() )
            {
            fill_global_listboxes();
            fill_listbox();
            select_last_line();
            }
        SetFocus( GetDlgItem(hWnd,ADD_BUTTON) );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, EDIT_DOWNTIMES_SCREEN_HELP );
        return TRUE;

    case WM_VSCROLL:
        do_scroll( wParam );
        return TRUE;

    case WM_VKEYTOITEM:
        return do_editbox_keypress( id, cmd );

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case SAVE_CHANGES_BUTTON:
                save_changes();
                if ( id == SAVE_CHANGES_BUTTON )
                    return TRUE;
            case IDCANCEL:
                PostMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;

            case PURGE_BUTTON:
                if ( resource_message_box(MainInstance, CONTINUE_STRING, PURGE_ALL_DOWNTIMES_STRING, MB_YESNO) == IDYES )
                    {
                    if ( purge_downtimes() )
                        {
                        init_globals();
                        fill_listbox();
                        select_last_line();
                        }
                    }
                return TRUE;

            case F9_KEY:
            case INSERT_KEY:
            case ADD_BUTTON:
                add_record();
                return TRUE;

            case DOWNTIME_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    if ( IsInsert && resource_message_box(MainInstance, RETURN_TO_NEW_STRING, NEW_RECORD_NOT_SAVED_STRING, MB_YESNO) == IDYES )
                        {
                        goto_currentline();
                        }
                    else
                        {
                        get_currentline();
                        do_select();
                        }
                    }
                return TRUE;

            case DATE_EDITBOX:
            case TIME_EDITBOX:
            case CODE_EDITBOX:
                if ( cmd == EN_CHANGE )
                    {
                    HaveChange = TRUE;
                    refresh_listbox_entry();
                    if ( id == CODE_EDITBOX )
                        fill_catboxes();
                    }
                return TRUE;

            case CAT_COMBOBOX:
                if ( cmd == CBN_SELCHANGE )
                    {
                    HaveChange = TRUE;
                    refresh_cat();
                    }
                return TRUE;

            case SUB_CAT_COMBOBOX:
                if ( cmd == CBN_SELCHANGE )
                    {
                    HaveChange = TRUE;
                    refresh_subcat();
                    }
                return TRUE;

            case PART_COMBOBOX:
            case OPERATOR_COMBOBOX:
                if ( cmd == CBN_SELCHANGE )
                    {
                    HaveChange = TRUE;
                    refresh_listbox_entry();
                    }
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_COMMAND:
        switch (id)
            {
            case IDCANCEL:
                PostMessage( hWnd, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_SETFOCUS:
        if ( EditDownWindow )
            SetFocus( GetDlgItem(EditDownWindow, DOWNTIME_LISTBOX) );
        return 0;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        register_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX, password_level_callback );
        get_current_password_level();
        return 0;

    case WM_EV_SHUTDOWN:
        PostMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                       GET_COMPUTER_AND_MACHINE                       *
***********************************************************************/
static void get_computer_and_machine( void )
{
TCHAR * cp;
TCHAR machine[MACHINE_NAME_LEN+1];
TCHAR part[PART_NAME_LEN+1];

if ( get_startup_part(machine, part) )
    {
    cp = machine_computer_name( machine );
    if ( cp )
        {
        lstrcpyn( MyComputer, cp,      COMPUTER_NAME_LEN+1 );
        lstrcpyn( MyMachine,  machine, MACHINE_NAME_LEN+1  );
        }
    }

}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( int cmd_show )
{

WNDCLASS wc;
COMPUTER_CLASS c;

EditDownWindow    = 0;
names_startup();
c.startup();

get_computer_and_machine();

Category.read();
SubCategory.read();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MAKEINTRESOURCE(EDITDOWN_ICON) );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    0, 0,                             // X,y
    225, 225,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

EditDownWindow  = CreateDialog(
    MainInstance,
    TEXT("EDITDOWN_DIALOG"),
    MainWindow,
    (DLGPROC) editdown_proc );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( void )
{
if ( T.open_status() )
    T.close();

unregister_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX );
client_dde_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
BOOL status;
MSG   msg;

MainInstance = this_instance;

MyWindowTitle = resource_string( MAIN_WINDOW_TITLE_STRING );
if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return 0;

NotFoundError = maketext( resource_string(NOT_FOUND_ERROR_STRING) );

init( cmd_show );

AccelHandle = LoadAccelerators( this_instance, AccelName );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( EditDownWindow )
        status = TranslateAccelerator( EditDownWindow, AccelHandle, &msg );

    if ( !status && EditDownWindow )
        status = IsDialogMessage( EditDownWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return msg.wParam;
}
