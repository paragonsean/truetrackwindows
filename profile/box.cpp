#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dstat.h"
#include "..\include\dbclass.h"
#include "..\include\ft.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\param.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"

#include "rtmlist.h"
#include "resource.h"
#include "extern.h"

const static int32 NOT_MONITORED_DOWN_STATE = 6;
const static int32 TOOLTIPS_PER_BOX         = 6;

const static UINT  MACHINE_TT_ID_OFFSET     = 1;
const static UINT  PART_TT_ID_OFFSET        = 2;
const static UINT  SHOT_NUMBER_TT_ID_OFFSET = 3;
const static UINT  SHOT_DATE_TT_ID_OFFSET   = 4;
const static UINT  DOWNTIME_TT_ID_OFFSET    = 5;
const static UINT  ALARM_TT_ID_OFFSET       = 6;

class STATUSBOX_ENTRY
    {
    public:
    RECT            border_rect;
    PART_NAME_ENTRY current;
    int32           shot_number;
    TCHAR           date_string[ALPHATIME_LEN+2+ALPHADATE_LEN+1];
    TCHAR           downcat[DOWNCAT_NUMBER_LEN+1];
    TCHAR           downsubcat[DOWNCAT_NUMBER_LEN+1];
    BOOLEAN         have_part;
    BOOLEAN         is_ftii;
    BOARD_DATA      status;
    int32           down_state;
    int32           alarm_state;
    STATUSBOX_ENTRY( void );
    ~STATUSBOX_ENTRY( void );
    void show_border( HDC dc );
    void show_text( HDC dc );
    void    get_alarm_info();
    void    get_shot_info( void );
    BOOLEAN get_part_info( void );
    void    get_downtime_status( void );
    void    read_alarm_info( void );
    void    refresh( void );
    RECT  * bounds( void );
    RECT  * machine_rect( void );
    RECT  * part_rect( void );
    RECT  * shot_rect( void );
    RECT  * date_rect( void );
    RECT  * down_rect( void );
    RECT  * alarm_rect( void );
    };

static int32             NofStatusBoxes = 0;
static STATUSBOX_ENTRY * StatusBoxes    = 0;

static int BoxCharHeight;     /* Height of character */
static int BoxNumberHeight;   /* Height of number */
static int BoxBaseHeight;     /* Height from top of char to baseline */
static int BoxCharWidth;

static int BoxLineWidth  = 1;
static int BoxLineHeight = 1;

static int HGap;
static int VGap;

static int VMachineOffset;
static int VPartOffset;
static int VShotOffset;
static int VDateOffset;
static int VAlarmOffset;
static int VDownOffset;

static int BorderHeight;
static int BorderWidth;

static BOOLEAN HaveTooltips = FALSE;

const static TCHAR NullChar    = TEXT( '\0' );
const static TCHAR TwoSpaces[] = TEXT( "  " );

TCHAR * resource_string( UINT resource_id );

BOOLEAN is_alarm( short parameter_index, float value );
BOOLEAN is_warning( short parameter_index, float value );
void toggle_realtime( void );
void update_screen_type( short new_screen_type );
void set_realtime_menu_string( void );

/***********************************************************************
*                           STATUSBOX_ENTRY                            *
***********************************************************************/
STATUSBOX_ENTRY::STATUSBOX_ENTRY( void )
{
lstrcpy( current.computer, NO_COMPUTER );
lstrcpy( current.machine,  NO_MACHINE );
lstrcpy( current.part,     NO_PART );
*date_string = NullChar;
*downcat     = NullChar;
*downsubcat  = NullChar;
border_rect.top    = 0;
border_rect.left   = 0;
border_rect.bottom = 0;
border_rect.right  = 0;
shot_number        = NO_SHOT_NUMBER;
have_part          = FALSE;
is_ftii            = FALSE;
status             = 0;
down_state         = NO_DOWN_STATE;
alarm_state        = NO_ALARM_STATE;

}

/***********************************************************************
*                          ~STATUSBOX_ENTRY                            *
***********************************************************************/
STATUSBOX_ENTRY::~STATUSBOX_ENTRY( void )
{
}

/***********************************************************************
*                              SHOW_BORDER                             *
***********************************************************************/
void STATUSBOX_ENTRY::show_border( HDC dc )
{
COLORREF color;
HPEN     oldpen;
HPEN     borderpen;

if ( status & VIS_AUTO_SHOT )
    color = AlarmColor;
else
    color = CurrentColor[BORDER_COLOR];
borderpen = CreatePen( PS_SOLID, BoxLineWidth, color );
if ( borderpen )
    {
    oldpen = (HPEN) SelectObject( dc, borderpen );
    MoveToEx( dc, border_rect.left, border_rect.top, 0 );
    LineTo( dc, border_rect.right, border_rect.top );
    LineTo( dc, border_rect.right, border_rect.bottom );
    LineTo( dc, border_rect.left, border_rect.bottom );
    LineTo( dc, border_rect.left, border_rect.top );

    SelectObject( dc, oldpen );
    DeleteObject( borderpen );
    }
}

/***********************************************************************
*                              SHOW_TEXT                               *
***********************************************************************/
void STATUSBOX_ENTRY::show_text( HDC dc )
{
int32   i;
int32   slen;
int32   x;
int32   y;
TCHAR * cp;
INT      orbkmode;

struct DOWN_ENTRY
    {
    int32 id;
    COLORREF * color;
    };

const int32 NofDownStates    = 7;

static DOWN_ENTRY De[NofDownStates] =
    {
    { NO_DOWN_STRING,     &AlarmColor   },
    { HUMAN_DOWN_STRING,  &AlarmColor   },
    { AUTO_DOWN_STRING,   &AlarmColor   },
    { PROG_DOWN_STRING,   &AlarmColor   },
    { PROG_UP_STRING,     &WarningColor },
    { MACH_UP_STRING,     &NoAlarmColor },
    { NO_MONITOR_STRING,  CurrentColor+BACKGROUND_COLOR }
    };

const int32 NofAlarmStates = 2;
static DOWN_ENTRY Ae[NofAlarmStates] =
    {
    { ALARM_STRING,       &AlarmColor   },
    { WARNING_STRING,     &WarningColor }
    };

/*
-------
Machine
------- */
orbkmode = GetBkMode( dc );
SetBkMode( dc, TRANSPARENT );
SetTextAlign( dc, TA_BASELINE | TA_LEFT );
SetTextColor( dc, CurrentColor[TEXT_COLOR]       );

x = border_rect.left + HGap;
y = border_rect.top  + VMachineOffset;

TextOut( dc, x, y, current.machine, lstrlen(current.machine) );

if ( have_part )
    {
    /*
    ----
    Part
    ---- */
    SetTextAlign( dc, TA_BASELINE | TA_RIGHT );
    x = border_rect.right - HGap;
    TextOut( dc, x, y, current.part, lstrlen(current.part) );


    /*
    -----------
    Shot Number
    ----------- */
    cp = int32toasc( shot_number );

    SetTextAlign( dc, TA_BASELINE | TA_CENTER );
    x = (border_rect.left + border_rect.right) / 2;
    y = border_rect.top  + VShotOffset;
    TextOut( dc, x, y, cp, lstrlen(cp) );

    /*
    ---------
    Shot Date
    --------- */
    slen = lstrlen( date_string );
    if ( slen )
        {
        y = border_rect.top  + VDateOffset;
        TextOut( dc, x, y, date_string, slen );
        }

    /*
    -----------
    Down status
    ----------- */
    i = down_state;
    if ( i < 0 || i >= NofDownStates )
        i = NO_DOWN_STATE;

    SetTextAlign( dc, TA_BASELINE | TA_LEFT );

    if ( De[i].id != NO_MONITOR_STRING )
        {
        SetBkMode( dc, orbkmode );
        SetBkColor( dc, *De[i].color );
        }

    x = border_rect.left + HGap;
    y = border_rect.top  + VDownOffset;

    cp = resource_string( De[i].id );
    slen = lstrlen( cp );
    TextOut( dc, x, y, cp, slen );

    /*
    ------------------------------------------------
                    Alarm status
    I don't show anything if there is no alarm state
    so decrement the state value for the lookup.
    ------------------------------------------------ */
    i = alarm_state - 1;
    if ( i >= 0 && i < NofAlarmStates )
        {
        SetBkMode( dc, orbkmode );
        SetBkColor( dc, *Ae[i].color );

        SetTextAlign( dc, TA_BASELINE | TA_RIGHT );
        x = border_rect.right - HGap;
        y = border_rect.top  + VAlarmOffset;
        cp = resource_string( Ae[i].id );
        slen = lstrlen( cp );
        TextOut( dc, x, y, cp, slen );
        }
    }
else
    {
    SetTextAlign( dc, TA_BASELINE | TA_RIGHT );
    x = border_rect.right - HGap;
    cp = resource_string( OFF_LINE_STRING );
    slen = lstrlen( cp );
    TextOut( dc, x, y, cp, slen );
    }

SetBkMode( dc, orbkmode );
}

/***********************************************************************
*                            GET_DOWNTIME_STATUS                       *
***********************************************************************/
void STATUSBOX_ENTRY::get_downtime_status( void )
{
COMPUTER_CLASS c;
DSTAT_CLASS d;

if ( !have_part )
    return;

if ( is_empty(current.computer) || is_empty(current.machine) )
    return;

if ( !c.online_status(current.computer) )
    return;

if ( d.get(current.computer, current.machine) )
    {
    lstrcpy( downcat,    d.category() );
    lstrcpy( downsubcat, d.subcategory() );
    down_state = d.down_state();
    }
}

/***********************************************************************
*                        STATUSBOX_ENTRY::BOUNDS                       *
*                                                                      *
*This returns the border rectange with the right and bottom            *
*incremented (because a point is not in a rectangle if it is on the    *
*right or bottom line.                                                 *
***********************************************************************/
RECT * STATUSBOX_ENTRY::bounds( void )
{
static RECT r;

r = border_rect;
r.right++;
r.bottom++;

return &r;
}

/***********************************************************************
*                           MACHINE_RECT                               *
***********************************************************************/
RECT * STATUSBOX_ENTRY::machine_rect( void )
{
static RECT r;

r.top    = border_rect.top  + VMachineOffset - BoxBaseHeight;
r.left   = border_rect.left + HGap;
r.right  = border_rect.left + (border_rect.right - border_rect.left) / 3;
r.bottom = r.top + BoxCharHeight;

return &r;
}

/***********************************************************************
*                             PART_RECT                                *
***********************************************************************/
RECT * STATUSBOX_ENTRY::part_rect( void )
{
static RECT r;

r.top    = border_rect.top  + VPartOffset - BoxBaseHeight;
r.left   = border_rect.left + (border_rect.right - border_rect.left) / 3;
r.right  = border_rect.right - HGap;
r.bottom = r.top + BoxCharHeight;

return &r;
}

/***********************************************************************
*                             SHOT_RECT                                *
***********************************************************************/
RECT * STATUSBOX_ENTRY::shot_rect( void )
{
static RECT r;

r.top    = border_rect.top  + VShotOffset - BoxBaseHeight;
r.left   = border_rect.left + HGap;
r.right  = border_rect.right - HGap;
r.bottom = r.top + BoxNumberHeight;

return &r;
}

/***********************************************************************
*                             DATE_RECT                                *
***********************************************************************/
RECT * STATUSBOX_ENTRY::date_rect( void )
{
static RECT r;

r.top    = border_rect.top   + VDateOffset - BoxBaseHeight;
r.left   = border_rect.left  + HGap;
r.right  = border_rect.right - HGap;
r.bottom = r.top + BoxNumberHeight;

return &r;
}

/***********************************************************************
*                             DOWN_RECT                                *
***********************************************************************/
RECT * STATUSBOX_ENTRY::down_rect( void )
{
static RECT r;

r.top    = border_rect.top  + VDownOffset - BoxBaseHeight;
r.left   = border_rect.left  + HGap;
r.right  = (border_rect.left + border_rect.right ) / 2;
r.bottom = r.top + BoxCharHeight;

return &r;
}

/***********************************************************************
*                            ALARM_RECT                                *
***********************************************************************/
RECT * STATUSBOX_ENTRY::alarm_rect( void )
{
static RECT r;

r.top    = border_rect.top  + VDownOffset - BoxBaseHeight;
r.left  = (border_rect.left + border_rect.right ) / 2;
r.right  = border_rect.right - HGap;
r.bottom = r.top + BoxCharHeight;

return &r;
}

/***********************************************************************
*                            REFRESH                                   *
***********************************************************************/
void STATUSBOX_ENTRY::refresh( void )
{
RECT  r;

r = border_rect;

if ( r.top > 0 )
    r.top--;

if ( r.left > 0 )
    r.left--;

r.right++;
r.bottom++;

InvalidateRect( MainPlotWindow, &r, TRUE );
}

/***********************************************************************
*                            READ_ALARM_INFO                           *
***********************************************************************/
void STATUSBOX_ENTRY::read_alarm_info( void )
{
COMPUTER_CLASS c;
PARAMETER_ENTRY * p;
PARAMETER_CLASS   param;
int32             i;
int32             n;
float             high;
float             low;
float             v;
float             value[MAX_PARMS];

alarm_state = NO_ALARM_STATE;

if ( is_empty(current.computer) || is_empty(current.machine) || !have_part )
    return;

if ( !c.online_status(current.computer) )
    return;

if ( param.find(current.computer, current.machine, current.part) )
    {
    if ( is_ftii )
        n = read_parameters_from_parmlist( value, profile_name(current.computer, current.machine, current.part, shot_number) );
    else
        n = read_parameters_from_profile( value, profile_name(current.computer, current.machine, current.part, shot_number) );
    for ( i=0; i<n; i++ )
        {
        p = &param.parameter[i];
        if ( p->input.type != NO_PARAMETER_TYPE )
            {
            v = value[i];

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
}

/***********************************************************************
*                           GET_ALARM_INFO                             *
*                                                                      *
* This assumes that I have just received a new shot event, meaning     *
* that the main program has filled the CurrentParam class and the      *
* Parms array.                                                         *
***********************************************************************/
void STATUSBOX_ENTRY::get_alarm_info( void )
{
int32 i;
int32 n;

alarm_state = NO_ALARM_STATE;

n = CurrentParam.count();
for ( i=0; i<n; i++ )
    {
    if ( is_alarm(i, Parms[i]) )
        {
        alarm_state = ALARM_STATE;
        break;
        }

    if ( alarm_state != WARNING_STATE )
        if ( is_warning(i, Parms[i]) )
            alarm_state = WARNING_STATE;
    }
}

/***********************************************************************
*                            GET_SHOT_INFO                             *
***********************************************************************/
void STATUSBOX_ENTRY::get_shot_info( void )
{
COMPUTER_CLASS c;
TCHAR     * cp;
SYSTEMTIME  st;
DB_TABLE    t;
NAME_CLASS  s;

if ( !have_part )
    return;

if ( is_empty(current.computer) || is_empty(current.machine) )
    return;

if ( !c.online_status(current.computer) )
    return;

init_systemtime_struct( st );

s = plookup_dbname( current.computer,current.machine );
if ( !s.file_exists() )
    return;

t.open( s.text(), PLOOKUP_RECLEN, PFL );
t.put_alpha(PLOOKUP_PART_NAME_OFFSET, current.part, PART_NAME_LEN);
t.reset_search_mode();
if ( t.get_next_key_match(1, FALSE) )
    {
    shot_number = t.get_long( PLOOKUP_LAST_SHOT_OFFSET );

    t.get_date( st, PLOOKUP_DATE_OFFSET );
    t.get_time( st, PLOOKUP_TIME_OFFSET );

    cp = date_string;
    copychars( cp, alphatime(st), ALPHATIME_LEN );
    cp += ALPHATIME_LEN;

    lstrcpy( cp, TwoSpaces );
    cp+= 2;

    copychars( cp, alphadate(st), ALPHADATE_LEN );
    cp += ALPHADATE_LEN;

    *cp = NullChar;
    }
t.close();
}

/***********************************************************************
*                            GET_PART_INFO                             *
***********************************************************************/
BOOLEAN STATUSBOX_ENTRY::get_part_info( void )
{
COMPUTER_CLASS c;
SYSTEMTIME  st;
MACHINE_CLASS m;

if ( is_empty(current.computer) || is_empty(current.machine) )
    return FALSE;

if ( !c.online_status(current.computer) )
    return FALSE;

init_systemtime_struct( st );

have_part = FALSE;
down_state = NO_DOWN_STATE;
shot_number = NO_SHOT_NUMBER;
*date_string = NullChar;

/*
------------------------------
See if this computer is online
------------------------------ */
if ( c.find(current.computer) )
    {
    if ( c.is_present() )
        {
        if ( m.find(current.computer, current.machine) )
            {
            have_part = TRUE;
            lstrcpy( current.part, m.current_part );
            if ( !(m.monitor_flags & MA_MONITORING_ON) )
                down_state = NOT_MONITORED_DOWN_STATE;
            else
                down_state = NO_DOWN_STATE;
            is_ftii = m.is_ftii;
            }

        shot_number  = NO_SHOT_NUMBER;
        *date_string = NullChar;

        if ( have_part )
            {
            get_shot_info();
            if ( down_state != NOT_MONITORED_DOWN_STATE )
                get_downtime_status();
            }
        }
    }

return have_part;
}

/***********************************************************************
*                             BOX_DISPLAY_STARTUP                      *
***********************************************************************/
BOOLEAN box_display_startup( void )
{
HDC        dc;
TEXTMETRIC tm;

dc = GetDC( MainPlotWindow );

GetTextMetrics( dc, &tm );

BoxCharHeight   = tm.tmHeight - tm.tmInternalLeading;
BoxNumberHeight = BoxCharHeight - tm.tmDescent;
BoxBaseHeight   = BoxNumberHeight;
BoxCharWidth    = tm.tmAveCharWidth;

HGap = BoxCharWidth;
VGap = BoxCharHeight/2;

BorderHeight = 5*VGap + 2*BoxLineHeight + 2*BoxCharHeight + 2*BoxNumberHeight;
BorderWidth  = 5*HGap + 16*BoxCharWidth;

VMachineOffset = BoxLineHeight + VGap + BoxBaseHeight;
VPartOffset    = VMachineOffset;
VShotOffset    = BoxLineHeight + 2*VGap + BoxCharHeight + BoxBaseHeight;
VDateOffset    = BoxLineHeight + 3*VGap + BoxCharHeight + BoxNumberHeight   + BoxBaseHeight;
VAlarmOffset   = BoxLineHeight + 4*VGap + BoxCharHeight + 2*BoxNumberHeight + BoxBaseHeight;
VDownOffset    = VAlarmOffset;

ReleaseDC( MainPlotWindow, dc );

StatusBoxes = 0;
return TRUE;
}

/***********************************************************************
*                        DELETE_TOOLTIPS                               *
***********************************************************************/
void delete_tooltips( void )
{
int32 i;
int32 j;
TOOLINFO ti;

if ( !ToolTipWindow )
    return;

if ( !HaveTooltips )
    return;

if ( !StatusBoxes )
    return;

ti.cbSize = sizeof(TOOLINFO);
ti.hwnd   = MainPlotWindow;

for ( i=0; i<NofStatusBoxes; i++ )
    {
    if ( StatusBoxes[i].have_part )
        {
        ti.uId = i * 10 + DOWNTIME_TT_ID_OFFSET;
        for ( j=0; j<TOOLTIPS_PER_BOX; j++ )
            {
            SendMessage( ToolTipWindow, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );
            ti.uId++;
            }
        }
    }

HaveTooltips = FALSE;
}

/***********************************************************************
*                          EMPTY_STATUS_BOXES                          *
***********************************************************************/
void empty_status_boxes( void )
{
delete_tooltips();

if ( StatusBoxes )
    {
    delete[] StatusBoxes;
    StatusBoxes    = 0;
    NofStatusBoxes = 0;
    }
}

/***********************************************************************
*                          MAKE_STATUS_BOXES                           *
***********************************************************************/
BOOLEAN make_status_boxes( void )
{
int32          n;

empty_status_boxes();

n = RealtimeMachineList.count();

if ( n < 1 )
    return FALSE;

StatusBoxes = new STATUSBOX_ENTRY[n];
if ( StatusBoxes )
    {
    NofStatusBoxes = n;
    return TRUE;
    }

NofStatusBoxes = 0;
return FALSE;
}

/***********************************************************************
*                          STATUS_BOX_INDEX                            *
***********************************************************************/
int32 status_box_index( TCHAR * machine_name )
{
int32 i;

if ( StatusBoxes )
    {
    for ( i=0; i<NofStatusBoxes; i++ )
        {
        if ( strings_are_equal(machine_name, StatusBoxes[i].current.machine ) )
            return i;
        }
    }

return NO_INDEX;
}

/***********************************************************************
*                            FIND_STATUS_BOX                           *
***********************************************************************/
STATUSBOX_ENTRY * find_status_box( TCHAR * machine_name )
{
int32 i;

i = status_box_index( machine_name );

if ( i != NO_INDEX )
    return StatusBoxes+i;

return 0;
}

/***********************************************************************
*                              TIPTEXT                                 *
***********************************************************************/
TCHAR * tiptext( int32 state, TCHAR * cat, TCHAR * subcat )
{
const int32 TIPTEXT_STRING_LEN = (2*DOWNCAT_NAME_LEN) + 2;
static TCHAR buf[TIPTEXT_STRING_LEN+1];

if ( state == HUMAN_DOWN_STATE )
    {
    lstrcpy( buf, category_name(cat) );
    lstrcat( buf, TwoSpaces );
    lstrcat( buf, subcategory_name(cat, subcat) );
    return buf;
    }

return resource_string( NO_DOWN_TT_STRING + state );
}

/***********************************************************************
*                     BOX_SCREEN_DOWN_DATA_CHANGE                      *
***********************************************************************/
void box_screen_down_data_change( TCHAR * machine_name, TCHAR * cat, TCHAR * subcat )
{
int32 i;
DSTAT_CLASS d;

STATUSBOX_ENTRY * se;
TOOLINFO ti;

i = status_box_index( machine_name );
if ( i != NO_INDEX )
    {
    se = StatusBoxes + i;
    lstrcpy( se->downcat, cat );
    lstrcpy( se->downsubcat, subcat );
    d.set_cat( cat );
    d.set_subcat( subcat );
    se->down_state = d.down_state();
    se->refresh();

    ti.cbSize   = sizeof(TOOLINFO);
    ti.hwnd     = MainPlotWindow;
    ti.hinst    = MainInstance;
    ti.uId      = i * 10 + DOWNTIME_TT_ID_OFFSET;
    ti.lpszText = tiptext( se->down_state, se->downcat, se->downsubcat );
    SendMessage( ToolTipWindow, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti );
    }
}

/***********************************************************************
*                    BOX_SCREEN_NEW_MACHINE_STATUS                     *
***********************************************************************/
void box_screen_new_machine_status( TCHAR * machine_name, BOARD_DATA new_status )
{
COMPUTER_CLASS    c;
STATUSBOX_ENTRY * se;

se = find_status_box( machine_name );
if ( se )
    {
    c.set_online_status( se->current.computer, TRUE );
    se->status = new_status;
    se->refresh();
    }

}

/***********************************************************************
*                    BOX_SCREEN_NEW_SHOT                               *
***********************************************************************/
void box_screen_new_shot( TCHAR * machine_name )
{
COMPUTER_CLASS c;
int               i;
STATUSBOX_ENTRY * se;
bool              was_offline;

was_offline = false;

se = find_status_box( machine_name );
if ( se )
    {
    c.set_online_status( se->current.computer, TRUE );
    if ( !se->have_part )
        {
        se->get_part_info();
        se->read_alarm_info();
        was_offline = true;
        }
    se->get_shot_info();
    se->get_alarm_info();
    se->refresh();
    if ( was_offline )
        {
        if ( c.find(se->current.computer) )
            {
            for ( i=0; i<NofStatusBoxes; i++ )
                {
                se = StatusBoxes+i;
                if ( strings_are_equal(se->current.computer, c.name()) && !se->have_part )
                    {
                    se->get_part_info();
                    se->read_alarm_info();
                    se->get_downtime_status();
                    se->refresh();
                    }
                }
            }
        }
    }
}

/***********************************************************************
*                    BOX_SCREEN_NEW_MONITOR_SETUP                      *
***********************************************************************/
void box_screen_new_monitor_setup( TCHAR * machine_name )
{
COMPUTER_CLASS    c;
STATUSBOX_ENTRY * se;

se = find_status_box( machine_name );
if ( se )
    {
    c.set_online_status( se->current.computer, TRUE );
    se->get_part_info();
    se->read_alarm_info();
    se->refresh();
    }
}

/***********************************************************************
*                      BOX_SCREEN_LEFT_BUTTON_UP                       *
*      P is the position of the cursor in the client window.           *
***********************************************************************/
void box_screen_left_button_up( POINT & p )
{
COMPUTER_CLASS c;
int32 i;

if ( !StatusBoxes )
    return;

for ( i=0; i<NofStatusBoxes; i++ )
    {
    /*
    ---------------------------------------
    Only paint if I am in the update region
    --------------------------------------- */
    if ( PtInRect(StatusBoxes[i].bounds(), p) )
        {
        if ( !c.online_status(StatusBoxes[i].current.computer) )
            return;
        lstrcpy( ComputerName, StatusBoxes[i].current.computer );
        lstrcpy( MachineName,  StatusBoxes[i].current.machine );
        lstrcpy( PartName,     StatusBoxes[i].current.part );
        lstrcpy( ShotName,     int32toasc(StatusBoxes[i].shot_number) );
        fix_shotname( ShotName );
        RealTime          = TRUE;
        SingleMachineOnly = TRUE;
        set_realtime_menu_string();
        update_screen_type( PROFILE_SCREEN_TYPE );
        SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
        }
    }
}

/***********************************************************************
*                          PAINT_BOX_DISPLAY                           *
***********************************************************************/
void paint_box_display( PAINTSTRUCT & ps )
{
int32 i;
RECT  r;
HBRUSH oldbrush;
HBRUSH mybrush;

if ( !StatusBoxes )
    return;

mybrush  = (HBRUSH) GetStockObject( DKGRAY_BRUSH );
oldbrush = (HBRUSH) SelectObject( ps.hdc, mybrush );

for ( i=0; i<NofStatusBoxes; i++ )
    {
    /*
    ---------------------------------------
    Only paint if I am in the update region
    --------------------------------------- */
    if (IntersectRect(&r, &ps.rcPaint, &StatusBoxes[i].border_rect) )
        {
        FillRect( ps.hdc, &StatusBoxes[i].border_rect, mybrush );
        StatusBoxes[i].show_border( ps.hdc );
        StatusBoxes[i].show_text( ps.hdc );
        }
    }

SelectObject( ps.hdc, oldbrush );
DeleteObject( mybrush );
}

/***********************************************************************
*                            BEGIN_BOX_DISPLAY                         *
***********************************************************************/
BOOLEAN begin_box_display( void )
{
int32   i;
int32   x;
int32   y;
RECT    r;
TCHAR * cp;
STATUSBOX_ENTRY * se;
TOOLINFO ti;

x       = 0;
y       = 0;

RealTime          = TRUE;
SingleMachineOnly = FALSE;
set_realtime_menu_string();

if ( make_status_boxes() )
    {
    GetClientRect( MainPlotWindow, &r );

    RealtimeMachineList.rewind();
    hourglass_cursor();
    for ( i=0; i<NofStatusBoxes; i++ )
        {
        if ( !RealtimeMachineList.next() )
            break;

        cp = RealtimeMachineList.text();
        se = StatusBoxes + i;

        if ( MachList.find(cp) )
            {
            lstrcpy( se->current.computer, MachList.computer_name() );
            lstrcpy( se->current.machine,  MachList.name() );
            se->get_part_info();
            se->read_alarm_info();
            se->get_downtime_status();
            se->border_rect.top    = y;
            se->border_rect.left   = x;
            se->border_rect.bottom = y + BorderHeight;
            se->border_rect.right  = x + BorderWidth;
            se->status      = 0;
            x += BorderWidth + BoxLineWidth + 1;
            if ( (x + BorderWidth) > r.right )
                {
                x = 0;
                y += BorderHeight + BoxLineWidth + 1;
                }

            if ( se->have_part && ToolTipWindow )
                {
                ti.cbSize = sizeof(TOOLINFO);
                ti.uFlags = 0;
                ti.hwnd   = MainPlotWindow;
                ti.hinst  = MainInstance;
                ti.uId    = i * 10 + MACHINE_TT_ID_OFFSET;
                ti.lpszText = resource_string( MACHINE_TT_STRING );
                ti.rect     = *se->machine_rect();
                SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );

                ti.uId++;
                ti.lpszText = resource_string( PART_TT_STRING );
                ti.rect     = *se->part_rect();
                SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );

                ti.uId++;
                ti.lpszText = resource_string( SHOT_TT_STRING );
                ti.rect     = *se->shot_rect();
                SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );

                ti.uId++;
                ti.lpszText = resource_string( DATE_TT_STRING );
                ti.rect     = *se->date_rect();
                SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );

                ti.uId++;
                ti.lpszText = tiptext( se->down_state, se->downcat, se->downsubcat );
                ti.rect     = *se->down_rect();
                SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );

                ti.uId++;
                ti.lpszText = resource_string( ALARM_TT_STRING );
                ti.rect     = *se->alarm_rect();
                SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );
                }
            }
        HaveTooltips = TRUE;
        SendMessage( ToolTipWindow, TTM_ACTIVATE, (WPARAM) TRUE, 0 );
        }
    restore_cursor();
    }

return TRUE;
}

/***********************************************************************
*                             END_BOX_DISPLAY                          *
***********************************************************************/
void end_box_display( void )
{
SendMessage( ToolTipWindow, TTM_ACTIVATE, (WPARAM) FALSE, 0 );
delete_tooltips();
}
