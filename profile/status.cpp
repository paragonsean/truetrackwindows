#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\alarmsum.h"
#include "..\include\computer.h"
#include "..\include\dstat.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\ft.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\param.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"

#include "dbarcfg.h"
#include "downclas.h"
#include "rtmlist.h"
#include "resource.h"
#include "extern.h"

static COLORREF Green_Color             = RGB( 0x00, 0xFF, 0x00 );
static COLORREF Status_Background_Color = RGB( 0xFF, 0xFF, 0xCC );
static BOOLEAN  HaveNewFont             = FALSE;

/*
---------------------------------------------------------------
Timeout used for the wm_timer messages that update the downtime
--------------------------------------------------------------- */
static const UINT TIMEOUT_MS = 61000L;

const int CYCLE_START_TYPE             =  0;
const int MACHINE_NAME_TYPE            =  1;
const int PART_NAME_TYPE               =  2;
const int SHOT_NUMBER_TYPE             =  3;
const int SHOT_TIME_TYPE               =  4;
const int ALARM_STATE_TYPE             =  5;
const int TOTAL_SHOTS_TYPE             =  6;
const int GOOD_SHOTS_TYPE              =  7;
const int BAD_SHOTS_TYPE               =  8;
const int DOWNTIME_STATE_TYPE          =  9;
const int TOTAL_MINUTES_TYPE           = 10;
const int DOWN_MINUTES_TYPE            = 11;
const int DOWN_PERCENT_TYPE            = 12;
const int SYSTEM_DOWN_MINUTES_TYPE     = 13;
const int NON_SYSTEM_DOWN_MINUTES_TYPE = 14;
const int UP_MINUTES_TYPE              = 15;
const int UP_PERCENT_TYPE              = 16;
const int PARAMETER_VALUE_TYPE         =100;  /* 100 = parameter 0, 101 = parameter 1, etc. */

struct DOWNTIME_STRING_ID_ENTRY
    {
    int32 id;
    COLORREF * color;
    };

static DOWNTIME_STRING_ID_ENTRY De[] =
    {
    { NO_DOWN_STRING,     &AlarmColor   },
    { HUMAN_DOWN_STRING,  &AlarmColor   },
    { AUTO_DOWN_STRING,   &AlarmColor   },
    { PROG_DOWN_STRING,   &AlarmColor   },
    { PROG_UP_STRING,     &WarningColor },
    { MACH_UP_STRING,     &NoAlarmColor },
    { NO_MONITOR_STRING,  CurrentColor+BACKGROUND_COLOR }
    };

const int32 NofDownStates    = sizeof(De)/sizeof(DOWNTIME_STRING_ID_ENTRY);

const int SYSTEM_SECONDS_INDEX = 0;
const int DOWN_SECONDS_INDEX   = 1;
const int UP_SECONDS_INDEX     = 2;
const int NOF_SECONDS_INDEXES  = 3;

/*
-----------------------
STATUS_SCREEN_ROW_ENTRY
----------------------- */
class STATUS_SCREEN_ROW_ENTRY
{
public:
PART_NAME_ENTRY     pn;
STRING_CLASS        shotnumber;
TIME_CLASS          shottime;
ALARM_SUMMARY_CLASS as;
int                 alarm_state;
BOARD_DATA          board_status;
STRING_CLASS        cat;
STRING_CLASS        subcat;
DSTAT_CLASS         d;
int                 down_seconds[NOF_SECONDS_INDEXES];
TIME_CLASS          now;  /* Time of last downtime update */
PARAMETER_CLASS     param;
int                 nof_parms;
float               pvalue[MAX_PARMS];

STATUS_SCREEN_ROW_ENTRY();
void init();
void get_alarm_summary_info();
void get_downtime_status();
bool get_shot_info();
void read_alarm_info();
void add_down_seconds( DOWNTIME_RECORD_CLASS & this_rec, DOWNTIME_RECORD_CLASS & next_rec );
void read_downtimes();
void read_parameters();
};

static STATUS_SCREEN_ROW_ENTRY * Row = 0;
static NofRows = 0;

/*
--------------------------
STATUS_SCREEN_COLUMN_ENTRY
-------------------------- */
class STATUS_SCREEN_COLUMN_ENTRY
{
public:
STRING_CLASS name;
int          width;
int          type;
STATUS_SCREEN_COLUMN_ENTRY() { width=0; type=0; }
~STATUS_SCREEN_COLUMN_ENTRY() {}
BOOLEAN get( const TCHAR * sorc );
TCHAR * put();
};

static STATUS_SCREEN_COLUMN_ENTRY * Column = 0;
static NofColumns = 0;

static BOOLEAN NeedAlarms   = FALSE;
static BOOLEAN NeedDowntime = FALSE;

const TCHAR NullChar    = TEXT('\0');
const TCHAR CommaChar   = TEXT(',');
const TCHAR EmptyString[] = TEXT("");
extern BOOLEAN Editing_Status_Screen;

HWND  MyHeader = 0;
int   HeaderHeight = 0;
int   DeltaY       = 0;

static FONT_LIST_CLASS FontList;
static HFONT FontHandle = INVALID_FONT_HANDLE;

void set_realtime_menu_string( void );
TCHAR * resource_string( UINT resource_id );
void update_screen_type( short new_screen_type );
void start_alarm_summary_dialog();

/***********************************************************************
*                        STATUS_SCREEN_ROW_ENTRY                       *
***********************************************************************/
STATUS_SCREEN_ROW_ENTRY::STATUS_SCREEN_ROW_ENTRY()
{
init();
}

/***********************************************************************
*                        STATUS_SCREEN_ROW_ENTRY                       *
*                                 INIT                                 *
***********************************************************************/
void STATUS_SCREEN_ROW_ENTRY::init()
{
int i;

alarm_state=NO_ALARM_STATE;
board_status=0;
for ( i=0; i<NOF_SECONDS_INDEXES; i++ )
    down_seconds[i] = 0;
nof_parms = 0;

for ( i=0; i<MAX_PARMS; i++ )
    pvalue[i] = 0.0;

*pn.computer = NullChar;
*pn.machine  = NullChar;
*pn.part     = NullChar;
}

/***********************************************************************
*                        STATUS_SCREEN_ROW_ENTRY                       *
*                            GET_SHOT_INFO                             *
***********************************************************************/
bool STATUS_SCREEN_ROW_ENTRY::get_shot_info()
{
COMPUTER_CLASS c;
NAME_CLASS     s;
DB_TABLE       t;

if ( !c.online_status(pn.computer) )
    return false;

s = plookup_dbname( pn.computer, pn.machine );
if ( !s.file_exists() )
    return false;

if ( not_a_part(pn.part) )
    copystring( pn.part, current_part_name(pn.computer, pn.machine) );

if ( t.open(s.text(), PLOOKUP_RECLEN, PFL) )
    {
    t.put_alpha( PLOOKUP_PART_NAME_OFFSET, pn.part, PART_NAME_LEN );
    t.reset_search_mode();
    if ( t.get_next_key_match(1, FALSE) )
        {
        shotnumber = int32toasc( t.get_long(PLOOKUP_LAST_SHOT_OFFSET) );
        get_time_class_from_db( shottime, t, PLOOKUP_DATE_OFFSET );
        }

    t.close();
    return true;
    }

return false;
}

/***********************************************************************
*                        STATUS_SCREEN_ROW_ENTRY                       *
*                        get_alarm_summary_info                        *
***********************************************************************/
void STATUS_SCREEN_ROW_ENTRY::get_alarm_summary_info()
{
short shift_number;
SYSTEMTIME shift_date;
SYSTEMTIME now;

GetLocalTime( &now );
get_shift_number_and_date( &shift_number, &shift_date, now );

as.set_part( pn );
as.get_counts( shift_date, (int32) shift_number );
}

/***********************************************************************
*                        STATUS_SCREEN_ROW_ENTRY                       *
*                          get_downtime_status                         *
***********************************************************************/
void STATUS_SCREEN_ROW_ENTRY::get_downtime_status()
{
if ( d.get(pn.computer, pn.machine) )
    {
    cat    = category_name( d.category() );
    subcat = subcategory_name( d.category(), d.subcategory() );
    }
}

/***********************************************************************
*                        STATUS_SCREEN_ROW_ENTRY                       *
*                            read_parameters                           *
***********************************************************************/
void STATUS_SCREEN_ROW_ENTRY::read_parameters()
{
param.find( pn.computer, pn.machine, pn.part );
nof_parms = read_parameters_from_parmlist( pvalue, profile_name(pn.computer, pn.machine, pn.part, shotnumber.int_value()) );
}

/***********************************************************************
*                        STATUS_SCREEN_ROW_ENTRY                       *
*                            read_alarm_info                           *
***********************************************************************/
void STATUS_SCREEN_ROW_ENTRY::read_alarm_info()
{
PARAMETER_ENTRY * p;
int32             i;
float             high;
float             low;
float             v;

alarm_state = NO_ALARM_STATE;

read_parameters();
for ( i=0; i<nof_parms; i++ )
    {
    p = &param.parameter[i];
    if ( p->input.type != NO_PARAMETER_TYPE )
        {
        v = pvalue[i];

        high = p->limits[ALARM_MAX].value;
        low  = p->limits[ALARM_MIN].value;
        if ( not_float_zero(high - low) )
            {
            if ( not_float_zero(v-low) && not_float_zero(high-v) )
                {
                if ( v < low || v > high )
                    {
                    alarm_state = ALARM_STATE;
                    return;
                    }
                }
            }

        high = p->limits[WARNING_MAX].value;
        low  = p->limits[WARNING_MIN].value;
        if ( not_float_zero(high - low) )
            {
            if ( not_float_zero(v-low) && not_float_zero(high-v) )
                {
                if ( v < low || v > high )
                    {
                    alarm_state = WARNING_STATE;
                    }
                }
            }
        }
    }
}

/***********************************************************************
*                        STATUS_SCREEN_ROW_ENTRY                       *
*                           add_down_seconds                           *
***********************************************************************/
void STATUS_SCREEN_ROW_ENTRY::add_down_seconds( DOWNTIME_RECORD_CLASS & this_rec, DOWNTIME_RECORD_CLASS & next_rec )
{
int seconds;

seconds = next_rec.tm - this_rec.tm;
if ( seconds > 0 )
    {
    if ( strings_are_equal(this_rec.cat, DOWNCAT_SYSTEM_CAT) && strings_are_equal(this_rec.subcat, DOWNCAT_UP_SUBCAT) )
        down_seconds[UP_SECONDS_INDEX] += seconds;
    else if ( strings_are_equal(this_rec.cat, DOWNCAT_SYSTEM_CAT) && !strings_are_equal(this_rec.subcat, DOWNCAT_UNSP_DOWN_SUBCAT) )
        down_seconds[SYSTEM_SECONDS_INDEX] += seconds;
    else
        down_seconds[DOWN_SECONDS_INDEX] += seconds;
    }
}

/***********************************************************************
*                        STATUS_SCREEN_ROW_ENTRY                       *
*                            read_downtimes                            *
***********************************************************************/
void STATUS_SCREEN_ROW_ENTRY::read_downtimes()
{
COMPUTER_CLASS c;
int            i;
int            n;
NAME_CLASS     s;
DB_TABLE       t;
TIME_CLASS     bofshift;
SYSTEMTIME     st;
DOWNTIME_RECORD_CLASS next_rec;
DOWNTIME_RECORD_CLASS this_rec;

if ( !c.online_status(pn.computer) )
    return;

for ( i=0; i<NOF_SECONDS_INDEXES; i++ )
    down_seconds[i] = 0;

now.get_local_time();
bofshift.set( start_of_shift(now.system_time()) );
st = bofshift.system_time();
s = downtime_dbname( pn.computer, pn.machine );
if ( s.file_exists() )
    {
    if ( t.open(s.text(), DOWNTIME_RECLEN, PFL) )
        {
        t.put_date( DOWNTIME_DATE_OFFSET, st );
        t.put_time( DOWNTIME_TIME_OFFSET, st );
        if ( t.goto_first_equal_or_greater_record(2) )
            {
            n = t.current_record_number();
            t.get_current_record( NO_LOCK );
            this_rec.get( t );
            next_rec = this_rec;

            /*
            ------------------------------------------------------------------------------
            This is the first record of the shift. This is unique because I probably have
            to go back one record and get the seconds before this record that are still in
            the shift.
            ------------------------------------------------------------------------------ */
            if ( this_rec.tm > bofshift )
                {
                if ( n == 0 )
                    {
                    /*
                    -----------------------------------------------------------
                    This is the 0th record, meaning that there is no way for me
                    to know what happened since the start of the shift;
                    ----------------------------------------------------------- */
                    copystring( this_rec.cat,    DOWNCAT_SYSTEM_CAT );
                    copystring( this_rec.subcat, DOWNCAT_UNSP_DOWN_SUBCAT );
                    }
                else
                    {
                    /*
                    ------------------------------------------------------------------
                    Get the previous record and set his time to the start of the shift
                    ------------------------------------------------------------------ */
                    n--;
                    t.goto_record_number( n );
                    t.get_current_record( NO_LOCK );
                    this_rec.get( t );

                    n++;
                    t.goto_record_number( n );
                    }
                this_rec.tm = bofshift;
                add_down_seconds( this_rec, next_rec );
                this_rec = next_rec;
                }

            while ( true )
                {
                if ( t.get_next_record(NO_LOCK) )
                    {
                    next_rec.get( t );
                    if ( next_rec.tm > now )
                        next_rec.tm = now;
                    }
                else
                    next_rec.tm = now;

                add_down_seconds( this_rec, next_rec );
                if ( next_rec.tm == now )
                    break;

                this_rec = next_rec;
                }
            }
        else
            {
            /*
            ---------------------------------------------------------------------
            There is no record for this shift. I need to read the last record and
            use that state for all the minutes.
            --------------------------------------------------------------------- */
            n = t.nof_recs();
            if ( n > 0 )
                {
                n--;
                t.goto_record_number( n );
                t.get_current_record( NO_LOCK );
                this_rec.get( t );
                }
            else
                {
                copystring( this_rec.cat,    DOWNCAT_SYSTEM_CAT );
                copystring( this_rec.subcat, DOWNCAT_UNSP_DOWN_SUBCAT );
                }
            this_rec.tm = bofshift;
            next_rec = this_rec;
            next_rec.tm = now;
            add_down_seconds( this_rec, next_rec );
            }
        t.close();
        }
    }
}

/***********************************************************************
*                    STATUS_SCREEN_COLUMN_ENTRY                        *
*                              get                                     *
***********************************************************************/
BOOLEAN STATUS_SCREEN_COLUMN_ENTRY::get( const TCHAR * sorc )
{
STRING_CLASS s;
TCHAR      * cp;
BOOLEAN      status;

status = FALSE;
s = sorc;
if ( !s.isempty() )
    {
    cp = s.text();
    if ( replace_char_with_null(cp, CommaChar) )
        {
        name = cp;
        cp = nextstring( cp );
        if (cp)
            {
            if ( replace_char_with_null(cp, CommaChar) )
                {
                width = asctoint32( cp );
                cp = nextstring( cp );
                if (cp)
                    {
                    type = asctoint32( cp );
                    status = TRUE;
                    }
                }
            }
        }
    }

return status;
}

/***********************************************************************
*                    STATUS_SCREEN_COLUMN_ENTRY                        *
*                              put                                     *
***********************************************************************/
TCHAR * STATUS_SCREEN_COLUMN_ENTRY::put()
{
static STRING_CLASS s;

s = EmptyString;
s.cat_w_char( name, CommaChar );
s.cat_w_char( int32toasc(width), CommaChar );
s += int32toasc( type );

return s.text();
}

/***********************************************************************
*                              HAVE_TYPE                               *
*           See if one of the columns displays this type               *
***********************************************************************/
static BOOLEAN have_type( int type_to_check_for )
{
int i;
for ( i=0; i<NofColumns; i++ )
    {
    if ( Column[i].type == type_to_check_for )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               FIND_ROW                               *
***********************************************************************/
static int find_row( TCHAR * machine_name )
{
int i;

for ( i=0; i<NofRows; i++ )
    {
    if ( strings_are_equal(Row[i].pn.machine, machine_name) )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                          SHOW_ONE_ROW                                *
***********************************************************************/
void show_one_row( HDC dc, int r )
{
int   d;
int   pi;
int   i;
int   x;
int   y;
int   sum;
int   total;
float high;
float low;
float v;

COLORREF        color;
COMPUTER_CLASS  c;
TCHAR         * cp;
BOOLEAN         i_set_the_color;
BOOLEAN         is_present;
RECTANGLE_CLASS rc;
STRING_CLASS    s;

y = HeaderHeight+2;
y += r*DeltaY;
x = 6;
is_present = c.online_status( Row[r].pn.computer );
if ( is_present )
    {
    if ( not_a_part(Row[r].pn.part) )
        is_present = FALSE;
    }

for ( i=0; i<NofColumns; i++ )
    {
    if ( Column[i].type == CYCLE_START_TYPE && (Row[r].board_status & VIS_AUTO_SHOT) )
        color = Green_Color;
    else
        color = Status_Background_Color;
    rc.left  = x - 5;
    rc.right = rc.left + Column[i].width -1;
    rc.top = y - 1;
    rc.bottom = rc.top + DeltaY - 1;
    rc.fill( dc, color );
    if ( Column[i].type == MACHINE_NAME_TYPE )
        {
        s = Row[r].pn.machine;
        TextOut( dc, x, y, s.text(), s.len() );
        }
    else if ( Column[i].type == PART_NAME_TYPE )
        {
        s = Row[r].pn.part;
        if ( !is_present )
            s = resource_string( OFF_LINE_STRING );
        TextOut( dc, x, y, s.text(), s.len() );
        }

    else if ( Column[i].type == SHOT_NUMBER_TYPE )
        {
        if ( is_present )
            TextOut( dc, x, y, Row[r].shotnumber.text(), Row[r].shotnumber.len() );
        }
    else if ( Column[i].type == SHOT_TIME_TYPE )
        {
        if ( is_present )
            {
            cp = Row[r].shottime.text();
            if ( is_present )
                TextOut( dc, x, y, cp, lstrlen(cp) );
            }
        }
    else if ( Column[i].type == TOTAL_SHOTS_TYPE )
        {
        s = int32toasc( Row[r].as.total_shot_count() );
        if ( is_present )
            TextOut( dc, x, y, s.text(), s.len() );
        }
    else if ( Column[i].type == GOOD_SHOTS_TYPE )
        {
        s = int32toasc( Row[r].as.total_shot_count() - Row[r].as.total_alarm_count() );
        if ( is_present )
            TextOut( dc, x, y, s.text(), s.len() );
        }
    else if ( Column[i].type == BAD_SHOTS_TYPE )
        {
        s = int32toasc( Row[r].as.total_alarm_count() );
        if ( is_present )
            TextOut( dc, x, y, s.text(), s.len() );
        }
    else if ( Column[i].type == ALARM_STATE_TYPE )
        {
        if ( Row[r].alarm_state == ALARM_STATE )
            {
            color = SetTextColor( dc, AlarmColor );
            s = TEXT("ALARM" );
            if ( is_present )
                TextOut( dc, x, y, s.text(), s.len() );
            SetTextColor( dc, color );
            }
        else
            {
            s = TEXT("Ok" );
            if ( is_present )
                TextOut( dc, x, y, s.text(), s.len() );
            }

        }
    else if ( Column[i].type == DOWNTIME_STATE_TYPE )
        {
        d = Row[r].d.down_state();
        if ( d < 0 || d >= NofDownStates )
            d = NO_DOWN_STATE;

        if ( d == HUMAN_DOWN_STATE )
            {
            s = Row[r].cat;
            s += TEXT( "  " );
            s += Row[r].subcat;
            }
        else
            {
            s = resource_string( De[d].id );
            }
        if ( d != MACH_UP_STATE )
            color = SetTextColor( dc, AlarmColor );
        if ( is_present )
            TextOut( dc, x, y, s.text(), s.len() );
        if ( d != MACH_UP_STATE )
            SetTextColor( dc, color );
        }
    else if ( Column[i].type > DOWNTIME_STATE_TYPE && Column[i].type < PARAMETER_VALUE_TYPE )
        {
        if ( Column[i].type == TOTAL_MINUTES_TYPE )
            {
            sum =  Row[r].down_seconds[SYSTEM_SECONDS_INDEX];
            sum += Row[r].down_seconds[DOWN_SECONDS_INDEX];
            sum += Row[r].down_seconds[UP_SECONDS_INDEX];
            sum /= 60;
            }
        else if ( Column[i].type == DOWN_MINUTES_TYPE )
            {
            sum =  Row[r].down_seconds[SYSTEM_SECONDS_INDEX];
            sum += Row[r].down_seconds[DOWN_SECONDS_INDEX];
            sum /= 60;
            }
        else if ( Column[i].type == NON_SYSTEM_DOWN_MINUTES_TYPE )
            {
            sum = Row[r].down_seconds[DOWN_SECONDS_INDEX];
            sum /= 60;
            }
        else if ( Column[i].type == UP_MINUTES_TYPE )
            {
            sum = Row[r].down_seconds[UP_SECONDS_INDEX];
            sum /= 60;
            }
        else if ( Column[i].type == UP_PERCENT_TYPE )
            {
            sum = Row[r].down_seconds[UP_SECONDS_INDEX];
            total = sum + Row[r].down_seconds[SYSTEM_SECONDS_INDEX];
            total += Row[r].down_seconds[DOWN_SECONDS_INDEX];
            sum *= 100;
            if ( total > 0 )
                sum /= total;
            }
        else if ( Column[i].type == DOWN_PERCENT_TYPE )
            {
            sum =  Row[r].down_seconds[SYSTEM_SECONDS_INDEX];
            sum += Row[r].down_seconds[DOWN_SECONDS_INDEX];
            total = sum + Row[r].down_seconds[UP_SECONDS_INDEX];
            sum *= 100;
            if ( total > 0 )
                sum /= total;
            }
        s = int32toasc( sum );
        if ( is_present )
            TextOut( dc, x, y, s.text(), s.len() );
        }
    else if ( Column[i].type >= PARAMETER_VALUE_TYPE )
        {
        if ( s.upsize(MAX_DOUBLE_LEN) )
            {
            i_set_the_color = FALSE;
            pi = Column[i].type - PARAMETER_VALUE_TYPE;
            v = Row[r].pvalue[pi];
            if ( pi >= 0 && pi < Row[r].nof_parms )
                {
                high = Row[r].param.parameter[i].limits[ALARM_MAX].value;
                low  = Row[r].param.parameter[i].limits[ALARM_MIN].value;
                if ( not_float_zero(high - low) )
                    {
                    if ( not_float_zero(v-low) && not_float_zero(high-v) )
                        {
                        if ( v < low || v > high )
                            {
                            color = SetTextColor( dc, AlarmColor );
                            i_set_the_color = TRUE;
                            }
                        }
                    }


                if ( !i_set_the_color )
                    {
                    high = Row[r].param.parameter[i].limits[WARNING_MAX].value;
                    low  = Row[r].param.parameter[i].limits[WARNING_MIN].value;
                    if ( not_float_zero(high - low) )
                        {
                        if ( not_float_zero(v-low) && not_float_zero(high-v) )
                            {
                            if ( v < low || v > high )
                                {
                                color = SetTextColor( dc, WarningColor );
                                i_set_the_color = TRUE;
                                }
                            }
                        }
                    }

                rounded_double_to_tchar( s.text(), (double) Row[r].pvalue[pi] );
                if ( is_present )
                    TextOut( dc, x, y, s.text(), s.len() );

                if ( i_set_the_color )
                    SetTextColor( dc, color );
                }
            }
        }

    x += Column[i].width;
    }
}

/***********************************************************************
*                          SHOW_ONE_ROW_ONLY                           *
*                                                                      *
* Call this when there is only one row to show. This sets up the       *
* dc, etc.                                                             *
***********************************************************************/
static void show_one_row_only( int rownum )
{
INT     orbkmode;
HDC     dc;
HFONT   oldfont;

dc = GetDC( MainPlotWindow );

if ( FontHandle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( dc, FontHandle );
else
    oldfont = INVALID_FONT_HANDLE;

orbkmode = GetBkMode( dc );
SetBkMode( dc, TRANSPARENT );
SetTextAlign( dc, TA_TOP | TA_LEFT );
SetTextColor( dc, 0 );

show_one_row( dc, rownum );

SetBkMode( dc, orbkmode );

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( dc, oldfont );

ReleaseDC( MainPlotWindow, dc );
}

/***********************************************************************
*                   GET_STATUS_SCREEN_COLUMN_WIDTHS                    *
***********************************************************************/
static void get_status_screen_column_widths()
{
int    i;
int    n;
HDITEM hdi;

if ( !MyHeader )
    return;

n = SendMessage( MyHeader, HDM_GETITEMCOUNT, 0, 0L );
if ( n == NofColumns )
    {
    hdi.mask = HDI_WIDTH;
    for ( i=0; i<n; i++ )
        {
        if (SendMessage(MyHeader, HDM_GETITEM, (WPARAM) i, (LPARAM) &hdi) )
            Column[i].width = hdi.cxy;
        }
    }
}

/***********************************************************************
*                          SAVE_STATUS_CONFIG                          *
***********************************************************************/
void save_status_config()
{
FILE_CLASS      f;
FONT_CLASS      lf;
RECTANGLE_CLASS r;
int             i;
int             n;
STRING_CLASS    s;

temp_message( resource_string(SAVING_STRING) );

if ( f.open_for_write(status_screen_config_filename()) )
    {
    s.cat_w_char( int32toasc((int32) GetRValue(Status_Background_Color)), CommaChar );
    s.cat_w_char( int32toasc((int32) GetGValue(Status_Background_Color)), CommaChar );
    s += int32toasc( (int32) GetBValue(Status_Background_Color) );
    f.writeline( s.text() );

    GetWindowRect( MyHeader, &r );
    f.writeline( int32toasc(r.height()) );

    if ( FontList.get_font(lf, FontHandle) )
        f.writeline( lf.put() );

    get_status_screen_column_widths();

    n = SendMessage( MyHeader, HDM_GETITEMCOUNT, 0, 0L );
    if ( n == NofColumns )
        {
        for ( i=0; i<n; i++ )
            f.writeline( Column[i].put() );
        }

    f.close();
    }

}

/***********************************************************************
*                           SHOW_CYCLE_START                           *
***********************************************************************/
static void show_cycle_start( int rownum )
{
int i;
int y;
int x;
HDC dc;
RECTANGLE_CLASS rc;
COLORREF color;

dc = GetDC( MainPlotWindow );

y = HeaderHeight+2;
y += rownum*DeltaY;
x = 6;

for ( i=0; i<NofColumns; i++ )
    {
    if ( Column[i].type == CYCLE_START_TYPE )
        {

        if ( Row[rownum].board_status & VIS_AUTO_SHOT )
            color = Green_Color;
        else
            color = Status_Background_Color;
        rc.left  = x - 5;
        rc.right = rc.left + Column[i].width -1;
        rc.top = y - 1;
        rc.bottom = rc.top + DeltaY - 1;

        rc.fill( dc, color );
        break;
        }
    x += Column[i].width;
    }

ReleaseDC( MainPlotWindow, dc );
}

/***********************************************************************
*                  STATUS_SCREEN_NEW_MACHINE_STATUS                    *
***********************************************************************/
void status_screen_new_machine_status( TCHAR * machine_name, BOARD_DATA new_status )
{
int i;

for ( i=0; i<NofRows; i++ )
    {
    if ( strings_are_equal(Row[i].pn.machine, machine_name) )
        {
        if ( Row[i].board_status != new_status )
            {
            Row[i].board_status = new_status;
            show_cycle_start( i );
            break;
            }
        }
    }
}

/***********************************************************************
*                 STATUS_SCREEN_NEW_SHOT                               *
***********************************************************************/
void status_screen_new_shot( TCHAR * machine_name )
{
int     i;

i = find_row( machine_name );
if ( i == NO_INDEX )
    return;

if ( Row[i].get_shot_info() )
    {
    if ( NeedAlarms )
        Row[i].get_alarm_summary_info();
    Row[i].read_alarm_info();

    show_one_row_only( i );
    }
}

/***********************************************************************
*                      STATUS_SCREEN_TIMER_UPDATE                      *
***********************************************************************/
void status_screen_timer_update()
{
/*
-----------------------------------------------------------------
The following static entrys allow me to determine whether a shift
change has taken place. If so I need to reset everyone.
----------------------------------------------------------------- */
static short last_shift = 0;
static short last_dow   = 0;

short current_shift;
short current_dow;
int r;
int seconds;
int state;
TIME_CLASS t;
BOOLEAN this_is_a_new_shift = FALSE;
BOOLEAN have_data_to_display;

t.get_local_time();

current_shift = shift_number( &current_dow, t.system_time() );
if ( current_shift != last_shift || current_dow != last_dow )
    {
    this_is_a_new_shift = TRUE;
    last_shift = current_shift;
    last_dow   = current_dow;
    }

for ( r=0; r<NofRows; r++ )
    {
    have_data_to_display = FALSE;
    if ( this_is_a_new_shift )
        {
        if ( NeedAlarms )
            Row[r].get_alarm_summary_info();
        /*
        -----------------------------------------------------------------------
        Since I only get timer messages if there is downtime info, I don't have
        to check NeedDowntimes here.
        ----------------------------------------------------------------------- */
        Row[r].get_downtime_status();
        Row[r].read_downtimes();
        have_data_to_display = TRUE;
        }
    else
        {
        seconds = t - Row[r].now;
        if ( seconds > 60 )
            {
            state = Row[r].d.down_state();
            if ( state == MACH_UP_STATE )
                Row[r].down_seconds[UP_SECONDS_INDEX] += seconds;
            else if ( state == HUMAN_DOWN_STATE || state == AUTO_DOWN_STATE )
                Row[r].down_seconds[DOWN_SECONDS_INDEX] += seconds;
            else
                Row[r].down_seconds[SYSTEM_SECONDS_INDEX] += seconds;
            Row[r].now = t;
            have_data_to_display = TRUE;
            }
        }
    if ( have_data_to_display )
        show_one_row_only( r );
    }

StatusScreenTimerId = SetTimer( MainPlotWindow, 0, TIMEOUT_MS, NULL );
}

/***********************************************************************
*                  STATUS_SCREEN_DOWN_DATA_CHANGE                      *
***********************************************************************/
void status_screen_down_data_change( TCHAR * machine_name, TCHAR * cat, TCHAR * subcat )
{
int32 r;

r = find_row( machine_name );
if ( r == NO_INDEX )
    return;

Row[r].cat    = category_name( cat );
Row[r].subcat = subcategory_name( cat, subcat );
Row[r].d.set_cat( cat );
Row[r].d.set_subcat( subcat );
Row[r].read_downtimes();

show_one_row_only( r );
}

/***********************************************************************
*                           CREATE_HEADER_CONTROL                      *
***********************************************************************/
static void create_header_control()
{
COMPUTER_CLASS c;
RECTANGLE_CLASS r;
HDITEM hdi;
int    n;
int    i;
TCHAR * cp;
FONT_CLASS lf;
STRING_CLASS s;
FILE_CLASS f;
HDC        dc;
HFONT      oldfont;
BYTE       rcolor, gcolor, bcolor;
BOOLEAN    i_loaded_the_font;

s = status_screen_config_filename();

/*
-------------------------------------------------------------------
If I am reloading the header then the font is already loaded and
I want to see if the DeltaY needs to be increased so I will know to
make HeaderHeight bigger.
------------------------------------------------------------------- */
if ( FontHandle != INVALID_FONT_HANDLE )
    {
    dc = GetDC( MainPlotWindow );
    oldfont = (HFONT) SelectObject( dc, FontHandle );
    DeltaY = character_height( dc );
    DeltaY++;
    SelectObject( dc, oldfont );
    ReleaseDC( MainPlotWindow, dc );
    }

if ( f.open_for_read(s.text()) )
    {
    if ( Column )
        {
        delete[] Column;
        Column     = 0;
        NofColumns = 0;
        }

    /*
    --------------------------------------------------------------------------------------
    See how many columns there are. The first three lines are for color, height, and font.
    -------------------------------------------------------------------------------------- */
    n = f.nof_lines();
    n -= 3;
    if ( n > 0 )
        {
        Column = new STATUS_SCREEN_COLUMN_ENTRY[n];
        if ( Column )
            NofColumns = n;
        }

    GetClientRect( MainPlotWindow, &r );

    s = f.readline();
    cp = s.text();
    if ( replace_char_with_null(cp, CommaChar) )
        {
        rcolor = (BYTE) asctoint32( cp );
        cp = nextstring( cp );
        if (cp)
            {
            if ( replace_char_with_null(cp, CommaChar) )
                {
                gcolor = (BYTE) asctoint32( cp );
                cp = nextstring( cp );
                if (cp)
                    {
                    bcolor = (BYTE) asctoint32( cp );
                    Status_Background_Color = RGB( rcolor, gcolor, bcolor );
                    }
                }
            }
        }

    s = f.readline();
    HeaderHeight = s.int_value();
    if ( HaveNewFont )
        {
        HeaderHeight = DeltaY + 2;
        HaveNewFont = FALSE;
        }

    MyHeader = CreateWindowEx(
        0,
        WC_HEADER,
        0,
        WS_CHILD | WS_VISIBLE | WS_BORDER | HDS_BUTTONS | HDS_HORZ,
        0, 0,                         // X, y
        r.width(), HeaderHeight,        // W, h
        MainPlotWindow,
        (HMENU) STATUS_HEADER,
        MainInstance,
        0 );

    s = f.readline();

    i_loaded_the_font = FALSE;
    if ( FontHandle == INVALID_FONT_HANDLE )
        {
        lf.get( s.text() );
        FontHandle = FontList.get_handle( lf );
        i_loaded_the_font = TRUE;
        }

    if ( FontHandle != INVALID_FONT_HANDLE )
        {
        SendMessage(MyHeader, WM_SETFONT, (WPARAM) FontHandle, 0 );

        if ( i_loaded_the_font )
            {
            /*
            -------------------------
            Get the size of each line
            ------------------------- */
            dc = GetDC( MainPlotWindow );
            oldfont = (HFONT) SelectObject( dc, FontHandle );
            DeltaY = character_height( dc );
            DeltaY++;
            SelectObject( dc, oldfont );
            ReleaseDC( MainPlotWindow, dc );
            }
        }

    hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
    hdi.fmt  = HDF_CENTER | HDF_STRING;
    for ( i=0; i<NofColumns; i++ )
        {
        if ( Column[i].get(f.readline()) )
            {
            hdi.pszText    = Column[i].name.text();
            hdi.cchTextMax = Column[i].name.len();
            hdi.cxy        = Column[i].width;
            SendMessage(MyHeader, HDM_INSERTITEM, (WPARAM) i, (LPARAM) &hdi);
            }
        }

    f.close();


    for ( i=0; i<NofColumns; i++ )
        {
        if ( Column[i].type >= TOTAL_SHOTS_TYPE && Column[i].type <= BAD_SHOTS_TYPE )
            NeedAlarms = TRUE;

        if ( Column[i].type >= DOWNTIME_STATE_TYPE )
            NeedDowntime = TRUE;
        }

    if ( Row )
        {
        delete[] Row;
        Row = 0;
        NofRows = 0;
        }

    n = RealtimeMachineList.count();
    if ( n > 0 )
        {
        Row = new STATUS_SCREEN_ROW_ENTRY[n];
        if ( !Row )
            n = 0;
        }

    RealtimeMachineList.rewind();
    hourglass_cursor();
    NofRows = 0;
    while ( NofRows<n )
        {
        if ( !RealtimeMachineList.next() )
            break;
        cp = RealtimeMachineList.text();
        if ( MachList.find(cp) )
            {
            if ( strings_are_equal( MachList.computer_name(), NO_COMPUTER) )
                {
                continue;
                }
            else
                {
                copystring( Row[NofRows].pn.machine, cp );
                copystring( Row[NofRows].pn.computer, MachList.computer_name() );
                if ( c.online_status(Row[NofRows].pn.computer) )
                    {
                    copystring( Row[NofRows].pn.part, current_part_name(Row[NofRows].pn.computer, cp) );
                    if ( Row[NofRows].get_shot_info() )
                        {
                        if ( NeedAlarms )
                            Row[NofRows].get_alarm_summary_info();
                        Row[NofRows].read_alarm_info();
                        if ( NeedDowntime )
                            {
                            Row[NofRows].get_downtime_status();
                            Row[NofRows].read_downtimes();
                            }
                        }
                    }
                else
                    {
                    copystring( Row[NofRows].pn.part, NoPart );
                    }
                NofRows++;
                }
            }
        }
    restore_cursor();
    }

if ( NeedDowntime )
    StatusScreenTimerId = SetTimer( MainPlotWindow, 0, TIMEOUT_MS, NULL );
}

/***********************************************************************
*                      CHOOSE_STATUS_SCREEN_FONT                       *
*   This is called by profile in response to popup menu choice         *
***********************************************************************/
void choose_status_screen_font()
{
FONT_CLASS lf;

if ( !MyHeader )
    {
    resource_message_box( MainInstance, CANNOT_COMPLY_STRING, STATUS_SCREEN_MENU_STRING );
    return;
    }

if ( FontHandle != INVALID_FONT_HANDLE )
    FontList.get_font( lf, FontHandle );

if ( lf.choose(MyHeader) )
    {
    DestroyWindow( MyHeader );
    MyHeader = 0;
    if ( FontHandle != INVALID_FONT_HANDLE )
        FontList.free_handle( FontHandle );
    FontHandle = FontList.get_handle( lf );
    HaveNewFont = TRUE;
    create_header_control();
    InvalidateRect( MainPlotWindow, 0, TRUE );
    }
}

/***********************************************************************
*                         BEGIN_STATUS_DISPLAY                         *
***********************************************************************/
BOOLEAN begin_status_display()
{
RealTime          = TRUE;
SingleMachineOnly = FALSE;

set_realtime_menu_string();
create_header_control();
return TRUE;
}

/***********************************************************************
*                          END_STATUS_DISPLAY                          *
***********************************************************************/
void end_status_display()
{

/*
-----------------------------------------------------------
If I have a timer qued up, kill it.
Note: profile zeroes this when he gets the wm_timer message.
------------------------------------------------------------ */
if ( StatusScreenTimerId != 0 )
    {
    KillTimer( MainPlotWindow, StatusScreenTimerId );
    StatusScreenTimerId = 0;
    }

if ( MyHeader )
    {
    DestroyWindow( MyHeader );
    MyHeader = 0;
    }

if ( Column )
    {
    delete[] Column;
    Column     = 0;
    NofColumns = 0;
    }

if ( Row )
    {
    delete[] Row;
    Row = 0;
    NofRows = 0;
    }

if ( FontHandle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( FontHandle );
    FontHandle = INVALID_FONT_HANDLE;
    }

Editing_Status_Screen = FALSE;
}

/***********************************************************************
*                         PAINT_STATUS_DISPLAY                         *
***********************************************************************/
void paint_status_display( PAINTSTRUCT & ps )
{
int r;
INT     orbkmode;
HFONT   oldfont;

if ( NeedNewStatusScreenColumnWidths )
    {
    NeedNewStatusScreenColumnWidths = FALSE;
    get_status_screen_column_widths();
    }

oldfont = INVALID_FONT_HANDLE;
if ( FontHandle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( ps.hdc, FontHandle );

orbkmode = GetBkMode( ps.hdc );
SetBkMode( ps.hdc, TRANSPARENT );
SetTextAlign( ps.hdc, TA_TOP | TA_LEFT );
SetTextColor( ps.hdc, 0 );

for ( r=0; r<NofRows; r++ )
    show_one_row( ps.hdc, r );

SetBkMode( ps.hdc, orbkmode );

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( ps.hdc, oldfont );
}

/***********************************************************************
*                    STATUS_SCREEN_LEFT_BUTTON_UP                      *
*      P is the position of the cursor in the client window.           *
***********************************************************************/
void status_screen_left_button_up( POINT & p )
{
COMPUTER_CLASS c;
int i;
int col;
RECTANGLE_CLASS r;
DOWNBAR_CONFIG_CLASS d;

d.range_type = DTR_START_DATE_RANGE;
d.nof_days   = 1;
d.end.get_local_time();
d.start.set( start_of_shift(d.end.system_time()) );

r.bottom = HeaderHeight+2;

for ( i=0; i<NofRows; i++ )
    {
    r.top = r.bottom;
    r.bottom += DeltaY;
    if ( p.y >= r.top && p.y <= r.bottom )
        {
        if ( !c.online_status(Row[i].pn.computer) )
            break;
        r.right = 1;
        for ( col=0; col<NofColumns; col++ )
            {
            r.left = r.right;
            r.right += Column[col].width;
            if ( p.x >= r.left && p.x <= r.right )
                {
                if ( Column[col].type >= DOWNTIME_STATE_TYPE )
                    {
                    d.write();
                    copystring( DowntimeComputerName, Row[i].pn.computer );
                    copystring( DowntimeMachineName,  Row[i].pn.machine  );
                    update_screen_type( DOWNBAR_SCREEN_TYPE );
                    return;
                    }
                else if ( Column[col].type <= ALARM_STATE_TYPE )
                    {
                    lstrcpy( ComputerName, Row[i].pn.computer );
                    lstrcpy( MachineName,  Row[i].pn.machine );
                    lstrcpy( PartName,     Row[i].pn.part );
                    lstrcpy( ShotName,     Row[i].shotnumber.text() );
                    fix_shotname( ShotName );
                    RealTime          = TRUE;
                    SingleMachineOnly = TRUE;
                    set_realtime_menu_string();
                    update_screen_type( PROFILE_SCREEN_TYPE );
                    SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
                    return;
                    }
                else if ( Column[col].type >= TOTAL_SHOTS_TYPE && Column[col].type <= BAD_SHOTS_TYPE )
                    {
                    lstrcpy( ComputerName, Row[i].pn.computer );
                    lstrcpy( MachineName,  Row[i].pn.machine );
                    lstrcpy( PartName,     Row[i].pn.part );
                    lstrcpy( ShotName,     Row[i].shotnumber.text() );
                    fix_shotname( ShotName );
                    start_alarm_summary_dialog();
                    }
                }
            }
        }
    }
}

