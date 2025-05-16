#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\array.h"
#include "..\include\catclass.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\listbox.h"
#include "..\include\param.h"
#include "..\include\listbox.h"
#include "..\include\stringcl.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"
#include "..\include\workclas.h"
#include "..\include\v5help.h"

#include "resource.h"
#include "extern.h"
#include "dbarcfg.h"
#include "vlbclass.h"

static TCHAR DBarConfigIniFile[] = TEXT( "dbarcfg.ini" );

static TCHAR RangeTypeKey[]      = TEXT( "RangeType" );
static TCHAR NOfDaysKey[]        = TEXT( "NOfDays" );
static TCHAR StartTimeKey[]      = TEXT( "StartTime" );
static TCHAR EndTimeKey[]        = TEXT( "EndTime" );
static TCHAR SortSubsByCatKey[]  = TEXT( "SortSubsByCat" );
static TCHAR ShowAllSubsKey[]    = TEXT( "ShowAllSubs" );

static TCHAR CrLfStr[] = TEXT( "\r\n" );
static TCHAR TabStr[]  = TEXT("\t");
static const TCHAR NullChar  = TEXT('\0');
static const TCHAR SpaceChar = TEXT( ' ' );
static const TCHAR TabChar   = TEXT('\t');

VIRTUAL_LISTBOX_CLASS Vlb;

#ifdef UNICODE
static UINT ClipFormat = CF_UNICODETEXT;
#else
static UINT ClipFormat = CF_TEXT;
#endif

void gethelp( UINT helptype, DWORD context );

class DOWNBAR_SCREEN_CONFIG_CLASS : public DOWNBAR_CONFIG_CLASS
{
public:

void put_to_screen( HWND w );
void get_from_screen( HWND w );
};

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                            BOOLEAN_TO_INI                            *
***********************************************************************/
static void boolean_to_ini( const TCHAR * key, BOOLEAN b )
{
bool b1;

b1 = false;
if ( b )
    b1 = true;

bool_to_ini( DBarConfigIniFile, ConfigSection, key, b1 );
}

/***********************************************************************
*                            BOOLEAN_FROM_INI                          *
***********************************************************************/
static BOOLEAN boolean_from_ini( const TCHAR * key )
{
return boolean_from_ini( DBarConfigIniFile, ConfigSection, key );
}

/***********************************************************************
*                             TEXT_TO_INI                              *
***********************************************************************/
static void text_to_ini( TCHAR * key, TCHAR * s )
{
put_ini_string( DBarConfigIniFile, ConfigSection, key, s );
}

/***********************************************************************
*                            TEXT_FROM_INI                             *
***********************************************************************/
static TCHAR * text_from_ini( TCHAR * key )
{
return get_ini_string( DBarConfigIniFile, ConfigSection, key );
}

/***********************************************************************
*                            INT32_TO_INI                              *
***********************************************************************/
static void int32_to_ini( TCHAR * key, int32 i )
{
int32_to_ini( DBarConfigIniFile, ConfigSection, key, i );
}

/***********************************************************************
*                            INT32_FROM_INI                            *
***********************************************************************/
static int32 int32_from_ini( TCHAR * key )
{
return int32_from_ini( DBarConfigIniFile, ConfigSection, key );
}

/***********************************************************************
*                               UL_FROM_INI                            *
***********************************************************************/
static unsigned long ul_from_ini( TCHAR * key )
{
return ul_from_ini( DBarConfigIniFile, ConfigSection, key );
}

/***********************************************************************
*                                 UL_TO_INI                            *
***********************************************************************/
static void ul_to_ini( TCHAR * key, unsigned long i )
{
ul_to_ini( DBarConfigIniFile, ConfigSection, key, i );
}

/***********************************************************************
*                          DOWNBAR_CONFIG_CLASS                        *
***********************************************************************/
DOWNBAR_CONFIG_CLASS::DOWNBAR_CONFIG_CLASS()
{
range_type = DTR_ALL_RANGE;
nof_days   = 1;
show_all_subs = FALSE;
sort_subs_by_cat = FALSE;
}

/***********************************************************************
*                          ~DOWNBAR_CONFIG_CLASS                       *
***********************************************************************/
DOWNBAR_CONFIG_CLASS::~DOWNBAR_CONFIG_CLASS()
{
}

/***********************************************************************
*                        DOWNBAR_CONFIG_CLASS                          *
*                                   =                                  *
***********************************************************************/
void DOWNBAR_CONFIG_CLASS::operator=( const DOWNBAR_CONFIG_CLASS & sorc )
{
range_type = sorc.range_type;
nof_days   = sorc.nof_days;
start      = sorc.start;
end        = sorc.end;
show_all_subs = sorc.show_all_subs;
sort_subs_by_cat = sorc.sort_subs_by_cat;
}

/***********************************************************************
*                           DOWNBAR_CONFIG_CLASS                       *
*                                  ==                                  *
***********************************************************************/
BOOLEAN DOWNBAR_CONFIG_CLASS::operator==(const DOWNBAR_CONFIG_CLASS & sorc )
{
if ( range_type != sorc.range_type )
    return FALSE;

if ( range_type & DTR_LAST_N_RANGE )
    if ( nof_days != sorc.nof_days )
        return FALSE;

if ( range_type & DTR_START_DATE_RANGE )
    if ( start != sorc.start )
        return FALSE;

if ( range_type & DTR_END_DATE_RANGE )
    if ( end != sorc.end )
        return FALSE;

return TRUE;
}

/***********************************************************************
*                           DOWNBAR_CONFIG_CLASS                       *
*                                  !=                                  *
***********************************************************************/
BOOLEAN DOWNBAR_CONFIG_CLASS::operator!=(const DOWNBAR_CONFIG_CLASS & sorc )
{
if ( operator==(sorc) )
    return FALSE;

return TRUE;
}
/***********************************************************************
*                           DOWNBAR_CONFIG_CLASS                       *
*                                 READ                                 *
***********************************************************************/
void DOWNBAR_CONFIG_CLASS::read( void )
{
range_type = ul_from_ini(    RangeTypeKey );
nof_days   = int32_from_ini( NOfDaysKey  );
start      = text_from_ini(  StartTimeKey );
end        = text_from_ini(  EndTimeKey  );
sort_subs_by_cat = boolean_from_ini( SortSubsByCatKey );
show_all_subs    = boolean_from_ini( ShowAllSubsKey   );
}

/***********************************************************************
*                          DOWNBAR_CONFIG_CLASS                        *
*                               WRITE                                  *
***********************************************************************/
void DOWNBAR_CONFIG_CLASS::write( void )
{
ul_to_ini(    RangeTypeKey, range_type   );
int32_to_ini( NOfDaysKey,   nof_days     );
text_to_ini(  StartTimeKey, start.text() );
text_to_ini(  EndTimeKey,   end.text()   );
boolean_to_ini(  SortSubsByCatKey, sort_subs_by_cat );
boolean_to_ini(  ShowAllSubsKey,   show_all_subs    );
}

/***********************************************************************
*                            TIME_TO_SCREEN                            *
***********************************************************************/
static void time_to_screen( HWND w, int box_id, SYSTEMTIME & st )
{
const TCHAR ZeroChar = TEXT( '0' );
TCHAR buf[3];

set_text( w, box_id, alphadate(st) );

box_id++;
insalph( buf, st.wHour, 2, ZeroChar, DECIMAL_RADIX );
set_text( w, box_id, buf );

box_id++;
insalph( buf, st.wMinute, 2, ZeroChar, DECIMAL_RADIX );
set_text( w, box_id, buf );
}

/***********************************************************************
*                          DOWNBAR_CONFIG_CLASS                        *
*                             PUT_TO_SCREEN                            *
***********************************************************************/
void DOWNBAR_SCREEN_CONFIG_CLASS::put_to_screen( HWND w )
{
int i;
SYSTEMTIME st;

i = 0;
if ( range_type & DTR_LAST_N_RANGE )
    i = 1;
else if ( range_type & DTR_START_DATE_RANGE )
    i = 2;

CheckRadioButton( w, DTR_ALL_RADIO, DTR_FROM_RADIO, DTR_ALL_RADIO + i );

i = 1;
if ( range_type & DTR_END_DATE_RANGE )
    i = 0;
CheckRadioButton( w, DTR_TO_DATE_RADIO, DTR_TO_PRESENT_RADIO, DTR_TO_DATE_RADIO + i );

set_text( w, DTR_LAST_N_EBOX, int32toasc(nof_days) );

st = start.system_time();
time_to_screen( w, DTR_FROM_DATE_EBOX, st );

st = end.system_time();
time_to_screen( w, DTR_TO_DATE_EBOX, st );

set_checkbox( w, SORT_SUBS_BY_CAT_XBOX, sort_subs_by_cat );
set_checkbox( w, SHOW_ALL_SUBS_XBOX,    show_all_subs );
}

/***********************************************************************
*                               GET_DATE                               *
***********************************************************************/
static SYSTEMTIME & get_date( HWND w, UINT date_ebox, UINT hour_ebox, UINT minute_ebox )
{
static SYSTEMTIME st;
TCHAR buf[ALPHADATE_LEN + 1];

init_systemtime_struct( st );

get_text( buf, w, date_ebox, ALPHADATE_LEN+1 );
extmdy( st, buf );

get_text( buf, w, hour_ebox, 3 );
st.wHour   = asctoint32( buf );

get_text( buf, w, minute_ebox, 3 );
st.wMinute = asctoint32( buf );

st.wSecond       = 0;
st.wMilliseconds = 0;

return st;
}

/***********************************************************************
*                           GET_FROM_SCREEN                            *
***********************************************************************/
void DOWNBAR_SCREEN_CONFIG_CLASS::get_from_screen( HWND w )
{
TCHAR buf[MAX_INTEGER_LEN+1];

range_type = DTR_ALL_RANGE;

if ( is_checked(w, DTR_LAST_N_RADIO) )
    range_type = DTR_LAST_N_RANGE;
else if ( is_checked(w, DTR_FROM_RADIO) )
    {
    range_type = DTR_START_DATE_RANGE;
    if ( is_checked(w, DTR_TO_DATE_RADIO) )
        range_type |= DTR_END_DATE_RANGE;
    }

get_text( buf, w, DTR_LAST_N_EBOX, MAX_INTEGER_LEN );
nof_days = asctoint32( buf );

start.set( get_date(w, DTR_FROM_DATE_EBOX, DTR_FROM_HOUR_EBOX, DTR_FROM_MINUTE_EBOX) );
end.set( get_date(w, DTR_TO_DATE_EBOX, DTR_TO_HOUR_EBOX, DTR_TO_MINUTE_EBOX)  );

sort_subs_by_cat = is_checked( w, SORT_SUBS_BY_CAT_XBOX );
show_all_subs    = is_checked( w, SHOW_ALL_SUBS_XBOX    );
}

/***********************************************************************
*                            GRAY_CONTROLS                             *
***********************************************************************/
static void gray_controls( HWND w )
{
const int32 LAST_N_INDEX           = 0;
const int32 FROM_DATE_INDEX        = 1;
const int32 FROM_HOUR_INDEX        = 2;
const int32 FROM_MINUTE_INDEX      = 3;
const int32 TO_DATE_INDEX          = 4;
const int32 TO_HOUR_INDEX          = 5;
const int32 TO_MINUTE_INDEX        = 6;
const int32 TO_DATE_RADIO_INDEX    = 7;
const int32 TO_PRESENT_RADIO_INDEX = 8;
const int32 n                      = 9;

BOOLEAN last_n;
BOOLEAN from_and_present;
BOOLEAN from_and_to;
int32   i;

struct ENABLE_ENTRY
    {
    UINT id;
    BOOL enabled;
    };

ENABLE_ENTRY control[n] = {
    { DTR_LAST_N_EBOX,      FALSE },
    { DTR_FROM_DATE_EBOX,   FALSE },
    { DTR_FROM_HOUR_EBOX,   FALSE },
    { DTR_FROM_MINUTE_EBOX, FALSE },

    { DTR_TO_DATE_EBOX,     FALSE },
    { DTR_TO_HOUR_EBOX,     FALSE },
    { DTR_TO_MINUTE_EBOX,   FALSE },
    { DTR_TO_DATE_RADIO,    FALSE },
    { DTR_TO_PRESENT_RADIO, FALSE }
    };

last_n           = FALSE;
from_and_present = FALSE;
from_and_to      = FALSE;

if ( !is_checked(w, DTR_ALL_RADIO) )
    {
    last_n = is_checked( w, DTR_LAST_N_RADIO );
    if ( ! last_n )
        {
        if ( is_checked(w, DTR_TO_DATE_RADIO) )
            from_and_to = TRUE;
        else
            from_and_present = TRUE;
        }
    }

if ( last_n )
    control[LAST_N_INDEX].enabled = TRUE;

if ( from_and_present || from_and_to )
    {
    control[FROM_DATE_INDEX].enabled        = TRUE;
    control[FROM_HOUR_INDEX].enabled        = TRUE;
    control[FROM_MINUTE_INDEX].enabled      = TRUE;
    control[TO_DATE_RADIO_INDEX].enabled    = TRUE;
    control[TO_PRESENT_RADIO_INDEX].enabled = TRUE;
    if ( from_and_to )
        {
        control[TO_DATE_INDEX].enabled   = TRUE;
        control[TO_HOUR_INDEX].enabled   = TRUE;
        control[TO_MINUTE_INDEX].enabled = TRUE;
        }
    }


for ( i=0; i<n; i++ )
    EnableWindow( GetDlgItem(w,control[i].id), control[i].enabled );

}

/***********************************************************************
*                             LISTBOX_LINE                             *
***********************************************************************/
TCHAR * listbox_line( DB_TABLE & t )
{
const int LB_LINE_LEN = ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 + 2*(DOWNCAT_NUMBER_LEN + 1) + 2*(DOWNCAT_NAME_LEN+1) + PART_NAME_LEN + 1 + OPERATOR_NUMBER_LEN+1;

static TCHAR buf[LB_LINE_LEN+1];

SYSTEMTIME st;

TCHAR   category[DOWNCAT_NUMBER_LEN+1];
TCHAR   subcategory[DOWNCAT_NUMBER_LEN+1];
TCHAR   part[PART_NAME_LEN+1];
TCHAR   worker[OPERATOR_NUMBER_LEN+1];
TCHAR * cp;

init_systemtime_struct( st );

t.get_date( st, DOWNTIME_DATE_OFFSET );
t.get_time( st, DOWNTIME_TIME_OFFSET );

t.get_alpha( category,    DOWNTIME_CAT_OFFSET,       DOWNCAT_NUMBER_LEN );
t.get_alpha( subcategory, DOWNTIME_SUBCAT_OFFSET,    DOWNCAT_NUMBER_LEN );
t.get_alpha( part,        DOWNTIME_PART_NAME_OFFSET, PART_NAME_LEN );
t.get_alpha( worker,      DOWNTIME_OPERATOR_OFFSET,  OPERATOR_NUMBER_LEN );

cp = buf;
cp = copy_w_char( cp, alphadate(st), TabChar );
cp = copy_w_char( cp, alphatime(st), TabChar );

cp = copy_w_char( cp, Cats.name(category), TabChar );
cp = copy_w_char( cp, SubCats.name(category, subcategory), TabChar );

cp = copy_w_char( cp, part, TabChar );
lstrcpy( cp, worker );

return buf;
}

/***********************************************************************
*                             SET_TAB_STOPS                            *
***********************************************************************/
static void set_tab_stops( HWND w )
{
const  int NOF_TABS  = 5;
static int tabs[NOF_TABS];
int        x;

x = LOWORD( GetDialogBaseUnits() );
x /= 2;

tabs[0] = (ALPHADATE_LEN + 1) * x;
tabs[1] = tabs[0] + (ALPHATIME_LEN + 1) * x;
tabs[2] = tabs[1] + (DOWNCAT_NAME_LEN + 4) * x;
tabs[3] = tabs[2] + (DOWNCAT_NAME_LEN + 4) * x;
tabs[4] = tabs[3] + (PART_NAME_LEN + 2) * x;

SendDlgItemMessage( w, DOWNTIME_LISTBOX, LB_SETTABSTOPS, (WPARAM) NOF_TABS, (LPARAM) tabs );
}

/***********************************************************************
*                            INIT_CONTROLS                             *
***********************************************************************/
static void init_controls( HWND w )
{
DOWNBAR_SCREEN_CONFIG_CLASS dsc;

set_tab_stops( w );

dsc.read();
dsc.put_to_screen( w );
gray_controls(w);

Vlb.startup( downtime_dbname(DowntimeComputerName, DowntimeMachineName),  DOWNTIME_RECLEN, w, DOWNTIME_LISTBOX, DOWNTIME_SCROLLBAR, listbox_line );
Vlb.goto_end();
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes( HWND w )
{
DOWNBAR_SCREEN_CONFIG_CLASS dsc;

dsc.get_from_screen( w );
dsc.write();
SendMessage( MainWindow, WM_NEW_DTR_CONFIG, 0, 0L );
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( HWND w )
{
Vlb.shutdown();
DialogIsActive = FALSE;
EndDialog( w, 0 );
}

/***********************************************************************
*                            GET_NEW_START_TIME                        *
***********************************************************************/
void get_new_start_time( HWND w, int box_id )
{
LISTBOX_CLASS lb;
TIME_CLASS     t;
lb.init( w, DOWNTIME_LISTBOX );

t = lb.selected_text();
time_to_screen( w, box_id, t.system_time() );
}

/***********************************************************************
*                           GET_RECORD_RANGE                           *
***********************************************************************/
static void get_record_range( int32 & firstrec, int32 & lastrec, DB_TABLE & t, HWND w )
{
int32 n;
DOWNBAR_SCREEN_CONFIG_CLASS dsc;
SYSTEMTIME st;

dsc.get_from_screen( w );

n = t.nof_recs();

if ( dsc.range_type & DTR_ALL_RANGE )
    {
    firstrec = 0;
    lastrec  = n-1;
    }
else if ( dsc.range_type & DTR_LAST_N_RANGE )
    {
    dsc.start.get_local_time();
    dsc.start.set_time( TEXT("00:00:00") );
    dsc.start -= (dsc.nof_days - 1) * SECONDS_PER_DAY;
    lastrec = n-1;
    }
if ( firstrec == NO_INDEX )
    {
    t.put_date( DOWNTIME_DATE_OFFSET, dsc.start.system_time() );
    t.put_time( DOWNTIME_TIME_OFFSET, dsc.start.system_time() );
    if ( t.goto_first_equal_or_greater_record(2) )
        firstrec = t.current_record_number();
    }

if ( lastrec == NO_INDEX )
    {
    if ( (firstrec < (n-1)) && (dsc.range_type & DTR_END_DATE_RANGE) )
        {
        st = dsc.end.system_time();
        st.wSecond = 59;
        dsc.end.set( st );
        t.put_date( DOWNTIME_DATE_OFFSET, dsc.end.system_time() );
        t.put_time( DOWNTIME_TIME_OFFSET, dsc.end.system_time() );
        if ( t.goto_first_greater_than_record(2) )
            lastrec = t.current_record_number() - 1;
        else
            lastrec = n-1;
        }
    else
        {
        lastrec = n-1;
        }

    }
}

/***********************************************************************
*                                EXPORT                                *
***********************************************************************/
static void export( HWND w )
{

#define DOWNTIME_DATE_OFFSET                 0
#define DOWNTIME_TIME_OFFSET                 11
#define DOWNTIME_CAT_OFFSET                  20
#define DOWNTIME_SUBCAT_OFFSET               25
#define DOWNTIME_OPERATOR_OFFSET             30
#define DOWNTIME_PART_NAME_OFFSET            41

const LINE_LEN = ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 +
                 DOWNCAT_NAME_LEN + 1 + DOWNCAT_NAME_LEN +
                 OPERATOR_FIRST_NAME_LEN + 1 + OPERATOR_LAST_NAME_LEN +
                 PART_NAME_LEN + 3;

DB_TABLE t;
int32 firstrec;
int32 lastrec;
int32 n;
HGLOBAL   mymem;
HGLOBAL   newmem;
DWORD     slen;
DWORD     nof_bytes;
TCHAR  * buf;
TCHAR  * cp;
TCHAR    cat[DOWNCAT_NUMBER_LEN+1];
TCHAR    subcat[DOWNCAT_NUMBER_LEN+1];
TCHAR    part_name[PART_NAME_LEN+1];
TCHAR    worker_number[OPERATOR_NUMBER_LEN+1];
TIME_CLASS tc;
WORKER_LIST_CLASS worker;
STRING_CLASS title;
STRING_CLASS heading;

if ( Vlb.count() < 1 )
    return;

title = resource_string( MainInstance, MACHINE_INTRO_STRING );
title += DowntimeMachineName;
title += CrLfStr;

heading = resource_string( MainInstance, DBARCFG_HEADING_STRING );

worker.load();

firstrec = NO_INDEX;
lastrec  = NO_INDEX;

if ( t.open(downtime_dbname(DowntimeComputerName, DowntimeMachineName), DOWNTIME_RECLEN, PFL) )
    {
    get_record_range( firstrec, lastrec, t, w );

    if ( (lastrec >= firstrec) && (firstrec >= 0) )
        {
        n = lastrec - firstrec;
        n++;
        slen = n * LINE_LEN + 1;
        slen += title.len();
        slen += heading.len();
        slen += 4;
        slen *= sizeof( TCHAR );

        mymem = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, slen );
        if ( !mymem )
            {
            resource_message_box( MainInstance, NO_MEMORY_STRING, CANNOT_COMPLY_STRING );
            return;
            }

        buf = (TCHAR *) GlobalLock(mymem);
        if ( !buf )
            {
            GlobalFree( mymem );
            return;
            }

        cp = buf;
        cp = copystring( cp, title.text() );
        cp = copystring( cp, heading.text() );

        t.goto_record_number( firstrec );
        t.get_current_record(NO_LOCK);
        while ( t.current_record_number() <= lastrec )
            {
            get_time_class_from_db( tc, t, DOWNTIME_DATE_OFFSET );
            cp = copy_w_char( cp, tc.text(), TabChar );

            t.get_alpha( cat,    DOWNTIME_CAT_OFFSET,       DOWNCAT_NUMBER_LEN );
            cp = copy_w_char( cp, Cats.name(cat), TabChar );

            t.get_alpha( subcat, DOWNTIME_SUBCAT_OFFSET,    DOWNCAT_NUMBER_LEN );
            cp = copy_w_char( cp, SubCats.name(cat, subcat), TabChar );

            t.get_alpha( worker_number, DOWNTIME_OPERATOR_OFFSET, OPERATOR_NUMBER_LEN );
            cp = copy_w_char( cp, worker.full_name(worker_number), TabChar );

            t.get_alpha( part_name, DOWNTIME_PART_NAME_OFFSET, PART_NAME_LEN );
            cp = copystring( cp, part_name );
            cp = copystring( cp, CrLfStr );

            if ( !t.get_next_record(NO_LOCK) )
                break;
            }

        *cp = NullChar;
        cp++;

        GlobalUnlock(mymem);

        if ( DWORD(cp) > DWORD(buf) )
            {
            nof_bytes = DWORD(cp) - DWORD(buf);
            if ( nof_bytes < slen )
                {
                newmem = GlobalReAlloc( mymem, nof_bytes, 0 );
                if ( !newmem )
                    {
                    GlobalFree( mymem );
                    mymem = 0;
                    }
                else
                    {
                    mymem = newmem;
                    }
                }

            if ( OpenClipboard(w) )
                {
                EmptyClipboard();
                if ( SetClipboardData(ClipFormat, mymem) )
                    mymem = 0;
                CloseClipboard();
                }
            }

        if ( mymem )
            GlobalFree( mymem );
        }
    else
        {
        resource_message_box( NOTHING_TO_EXPORT_STRING, CANNOT_COMPLY_STRING );
        }
    t.close();
    }

}

/***********************************************************************
*                                PURGE                                 *
***********************************************************************/
static void purge( HWND w )
{
DB_TABLE t;
int32 firstrec;
int32 lastrec;
int32 n_to_kill;

if ( Vlb.count() < 1 )
    return;

Vlb.shutdown();

firstrec = NO_INDEX;
lastrec  = NO_INDEX;

if ( t.open(downtime_dbname(DowntimeComputerName, DowntimeMachineName), DOWNTIME_RECLEN, FL) )
    {
    get_record_range( firstrec, lastrec, t, w );
    if ( (lastrec >= firstrec) && (firstrec >= 0) )
        {
        n_to_kill = lastrec - firstrec;
        n_to_kill++;
        t.goto_record_number( firstrec );
        t.rec_delete( n_to_kill );
        }
    }

t.close();

Vlb.startup( downtime_dbname(DowntimeComputerName, DowntimeMachineName),  DOWNTIME_RECLEN, w, DOWNTIME_LISTBOX, DOWNTIME_SCROLLBAR, listbox_line );
Vlb.goto_end();
}

/***********************************************************************
*                       DOWNTIME_REPORT_SETUP_PROC                     *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK downtime_report_setup_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;
int     cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        init_controls( w );
        return TRUE;

    case WM_CLOSE:
        shutdown( w );
        return TRUE;

    case WM_VSCROLL:
        Vlb.do_vscroll( wParam );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, DOWNTIME_REPORT_SETUPS_HELP );
        return TRUE;

    case WM_VKEYTOITEM:
        return Vlb.do_keypress( id, cmd );

    case WM_COMMAND:
        switch ( id )
            {

            case DOWNTIME_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    Vlb.do_select();
                return TRUE;

            case EXPORT_DOWNTIMES_BUTTON:
                export( w );
                return TRUE;

            case PURGE_SELECTION_BUTTON:
                purge( w );
                return TRUE;

            case IDOK:
                save_changes( w );

            case IDCANCEL:
                shutdown( w );
                return TRUE;

            case DTR_RANGE_START_BUTTON:
                get_new_start_time( w, DTR_FROM_DATE_EBOX );
                return TRUE;

            case DTR_RANGE_END_BUTTON:
                get_new_start_time( w, DTR_TO_DATE_EBOX );
                return TRUE;

            case DTR_ALL_RADIO:
            case DTR_LAST_N_RADIO:
            case DTR_FROM_RADIO:
            case DTR_TO_DATE_RADIO:
            case DTR_TO_PRESENT_RADIO:
                gray_controls(w);
                return TRUE;
            }
        break;
    }

return FALSE;
}

