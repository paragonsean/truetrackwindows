#define NOMINMAX
#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\array.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\genlist.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\param.h"
#include "..\include\rectclas.h"
#include "..\include\subs.h"
#include "..\include\structs.h"
#include "..\include\textlen.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"

#include "ctrlimit.h"
#include "xaxis.h"
#include "extern.h"
#include "fixparam.h"
#include "tcconfig.h"
#include "gridclas.h"
#include "mplot.h"
#include "resource.h"

extern HDC     MyDc;   /* Current device context, set in profile */
extern RECT    PlotWindowRect;
extern BOOLEAN Printing;

extern GRID_CLASS Grid;
extern BOOLEAN    GridIsVisible;
extern int NofColumns;
extern STRING_CLASS ArialFontName;

#define STANDARD_SCREEN_TYPE  1

TCHAR * resource_string( UINT resource_id );

static PART_NAME_ENTRY Current;

INT32_ARRAY_CLASS  Parameters;
TREND_CONFIG_CLASS TcConfig;
X_AXIS_CLASS       XAxis;
MULTI_PLOT_CLASS   Plots;

static int CursorStartY;
static int CursorEndY;
static int CursorLeft;
static int CursorX;
static int32 NofPointsRead = 0;
static int32 XAxisIndex = 0;
static int32 FirstIndex = NO_INDEX;
static BOOLEAN HaveCursorLine      = FALSE;
static BOOLEAN ReadyForCursor      = FALSE;
static BOOLEAN Resetting           = FALSE;

static int32 CellPlaces = 2;

static const TCHAR SpaceString[] = TEXT( " " );
static       TCHAR EmptyString[] = TEXT( "" );

void toggle_realtime( void );
void update_screen_type( short new_screen_type );
void set_realtime_menu_string( void );

class STAT_ENTRY
    {
    private:
    static int32 n_per_sample;

    public:
    int32 sample_n;
    double sample_sum;

    double sum;
    double sumb;
    double sum2;
    int32 k;
    int32 n;
    float  upper_limit;
    float  lower_limit;
    int32 n_out;
    float  maximum;
    float  minimum;

    STAT_ENTRY();
    ~STAT_ENTRY() {}
    void add( float x );
    void set_sample_size( int32 new_size );
    void set_limits( float low, float high ) { lower_limit = low; upper_limit = high; }
    int32 count( void ) { return n; }
    float avg( void );
    float stdev( void );
    float percent_out( void );
    float range( void ) { return maximum - minimum; }
    float max( void ) { return maximum; }
    float min( void ) { return minimum; }
    float z( float control_limit );
    };

int32 STAT_ENTRY::n_per_sample = 1;

void text_out( int32 x, int32 y, TCHAR * sorc );

/***********************************************************************
*                            DECIMAL_PLACES                            *
***********************************************************************/
static int32 decimal_places( float x )
{
int32 places;

if ( x < 0.0 )
    x = -x;

places = 4;

if ( x > 1.0 && places > 3 )
    places = 3;

if ( x > 10.0 && places > 2 )
    places = 2;

if ( x > 100.0 && places > 1 )
    places = 1;

if ( x > 1000.0 && places > 0 )
    places = 0;

return places;
}

/***********************************************************************
*                             STAT_ENTRY                               *
***********************************************************************/
STAT_ENTRY::STAT_ENTRY()
{
k            = 0;
n            = 0;
n_out        = 0;
sample_n     = 0;
sample_sum   = 0.0;
sum          = 0.0;
sumb         = 0.0;
sum2         = 0.0;
maximum      = 0.0;
minimum      = 0.0;
lower_limit  = 0.0;
upper_limit  = 0.0;
}

/***********************************************************************
*                             STAT_ENTRY                               *
*                             PERCENT_OUT                              *
***********************************************************************/
float STAT_ENTRY::percent_out( void )
{
float percent;

percent = 1.0;
if ( n > 0 )
    percent = float( n_out ) / float(n);

percent *= 100.0;

return percent;
}

/***********************************************************************
*                             STAT_ENTRY                               *
*                           SET_SAMPLE_SIZE                            *
***********************************************************************/
void STAT_ENTRY::set_sample_size( int32 new_size )
{
if ( new_size > 0 )
    n_per_sample = new_size;
}

/***********************************************************************
*                             STAT_ENTRY                               *
*                                 Z                                    *
***********************************************************************/
float STAT_ENTRY::z( float x )
{
float sd;
float a;

a  = avg();
a -= x;
if ( a < 0.0 )
    a = -a;

sd = stdev();
if ( not_float_zero(sd) )
    a /= sd;

return a;
}

/***********************************************************************
*                             STAT_ENTRY                               *
*                                ADD                                   *
***********************************************************************/
void STAT_ENTRY::add( float x )
{
if ( n == 0 )
    {
    minimum = x;
    maximum = x;
    }
else
    {
    if ( x < minimum )
        minimum = x;
    if ( x > maximum )
        maximum = x;
    }

if ( upper_limit != lower_limit )
    {
    if ( x > upper_limit || x < lower_limit )
        n_out++;
    }

sample_sum += x;
sample_n++;
if ( sample_n == n_per_sample )
    {
    k++;
    sumb += (sample_sum * sample_sum) / (float) sample_n;
    sample_sum = 0.0;
    sample_n   = 0;
    }

sum  += x;
sum2 += x*x;
n++;
}

/***********************************************************************
*                             STAT_ENTRY                               *
*                               STDEV                                  *
***********************************************************************/
float STAT_ENTRY::stdev( void )
{
double sst;
double ssb;
double c;
int32 myk;

double sd;

sd = 0.0;

if ( n > 1 )
    {
    c = sum * sum;
    c /= (float) n;
    sst = sum2 - c;

    if ( k == n || k < 1 )
        {
        sd = sst / float(n-1);
        }
    else
        {
        ssb = sumb;
        myk = k;
        if ( sample_n > 0 )
            {
            ssb += (sample_sum * sample_sum) / sample_n;
            myk++;
            }

        ssb -= c;
        sd = sst - ssb;
        sd /= float(n-myk);
        }

    if ( sd > 0.01 )
        sd = sqrt(sd);
    }

return (float) sd;
}

/***********************************************************************
*                             STAT_ENTRY                               *
*                                AVG                                   *
***********************************************************************/
float STAT_ENTRY::avg( void )
{
if ( n > 0 )
    return (float) sum / double(n);

return sum;
}


/***********************************************************************
*                                 LINE                                 *
***********************************************************************/
static void line( HDC dc )
{
int   orimode;
orimode = SetROP2( dc, R2_NOT );

MoveToEx( dc, CursorX, CursorStartY, 0 );
LineTo(   dc, CursorX, CursorEndY      );

SetROP2( dc, orimode );
}

/***********************************************************************
*                           DRAW_CURSOR_LINE                           *
***********************************************************************/
static void draw_cursor_line( HDC dc )
{
int32 i;
BOOLEAN need_bitmap;

line( dc );

if ( HaveCursorLine )
    need_bitmap = NO_BITMAP;
else
    need_bitmap = WITH_BITMAP;

for ( i=0; i<Plots.count(); i++ )
    Plots[i].show_info_box( dc, CursorX, XAxisIndex, need_bitmap );

XAxis.show_info_box( dc, CursorX, XAxisIndex, need_bitmap );
}

/***********************************************************************
*                           DRAW_CURSOR_LINE                           *
***********************************************************************/
static void draw_cursor_line( void )
{
HDC   dc;

dc = GetDC( MainPlotWindow );

if ( dc )
    {
    draw_cursor_line( dc );
    ReleaseDC( MainPlotWindow, dc );
    }
}

/***********************************************************************
*                          ERASE_CURSOR_LINE                           *
***********************************************************************/
static void erase_cursor_line( void )
{
int32 i;
HDC   dc;

dc = GetDC( MainPlotWindow );

if ( HaveCursorLine )
    {
    line( dc );

    for ( i=0; i<Plots.count(); i++ )
        Plots[i].restore_info_box( dc );

    XAxis.restore_info_box( dc );

    ReleaseDC( MainPlotWindow, dc );

    HaveCursorLine = FALSE;
    XAxisIndex     = NO_INDEX;
    }

}

/***********************************************************************
*                        HIDE_TREND_CURSOR_LINE                        *
***********************************************************************/
void erase_trend_cursor_line( void )
{
erase_cursor_line();
}

/***********************************************************************
*                             MOUSE_TO_TREND                           *
***********************************************************************/
void mouse_to_trend( int32 x, int32 y )
{
BOOLEAN mouse_is_in_window;
int32 j;

if ( !ReadyForCursor )
    return;

if ( DialogIsActive )
    return;

j = XAxis.x_index( x );

mouse_is_in_window = FALSE;
if ( y >= CursorStartY && y <= CursorEndY && x >= CursorLeft )
    mouse_is_in_window = TRUE;

if ( HaveCursorLine )
    {
    if ( !mouse_is_in_window || j != XAxisIndex )
        erase_cursor_line();
    }

if ( mouse_is_in_window && j != XAxisIndex )
    {
    XAxisIndex = j;
    CursorX    = XAxis.x_pos_from_index( j );
    draw_cursor_line();
    HaveCursorLine = TRUE;
    }
}

/***********************************************************************
*                     GET_NOF_POINTS_FROM_SHOT_RANGE                   *
***********************************************************************/
int32 get_nof_points_from_shot_range( int32 & first_record_to_read )
{
NAME_CLASS s;
DB_TABLE   t;
int32      n;
int32      n_to_read;
int32      r1;
int32      r2;
int32      x;

r1 = 0;
s = shotparm_dbname( ComputerName, MachineName, PartName );
if ( s.file_exists() )
    {
    if ( t.open(s.text(), SHOTPARM_RECLEN, PFL) )
        {
        n = t.nof_recs();

        if ( TcConfig.time_period_type == LAST_N_TIME_PERIOD_TYPE )
            {
            n_to_read = TcConfig.nof_points;
            if ( TcConfig.chart_type == XBAR_CHART_TYPE || TcConfig.chart_type == RANGE_CHART_TYPE )
                {
                if ( TcConfig.nto_average > 1 )
                    n_to_read *= TcConfig.nto_average;
                }
            r1 = n - n_to_read;
            if ( r1 < 0 )
                r1 = 0;
            n = TcConfig.nof_points;
            }
        else if ( n > 0 )
            {
            t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, TcConfig.first_shot, SHOT_LEN );
            if ( t.goto_first_equal_or_greater_record(1) )
                {
                r1 = t.current_record_number();
                t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, TcConfig.last_shot, SHOT_LEN );
                if ( t.goto_first_equal_or_greater_record(1) )
                    {
                    r2 = t.current_record_number();
                    t.get_current_record( NO_LOCK );
                    x = t.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );
                    if ( x > TcConfig.last_shot )
                        r2--;
                    n = r2 - r1 + 1;
                    }
                else
                    {
                    n -= r1;
                    }
                }

            }
        t.close();
        }
    }

first_record_to_read = r1;
return n;
}

/***********************************************************************
*                          SET_COLUMN_WIDTHS                           *
***********************************************************************/
static void set_column_widths( void )
{
int32   i;
int32   n;
int32   width;
HFONT   f;
HFONT   old_font;
RECT    r;
TEXT_LEN_CLASS t;
HDC     dc;

GetClientRect( MainPlotWindow, &r );
dc = GetDC( MainPlotWindow );

f = create_font( dc, 0, 13, ArialFontName.text() );

if ( f )
    old_font = (HFONT) SelectObject( dc, f );

/*
---------------------------------
Get the width of the widest label
--------------------------------- */
t.init( dc );

n = CurrentParam.count();
for ( i=0; i<n; i++ )
    t.check( CurrentParam.parameter[i].name );

width = t.width();
if ( (r.right - r.left) < 1000 )
    width += width/10;
Grid.SetSH_Width( width );

/*
----------------------------------------------------------
Get the column width that would completely fill the screen
---------------------------------------------------------- */
i = r.right - r.left - width;
if ( NofColumns > 0 )
    i /= NofColumns;

/*
-------------------------------------------------------
Get the width of the largest number I am going to print
This doesn't work correctly for 640x480. I don't know
why so I kludged the following.
------------------------------------------------------- */
t.init( dc );
t.check( TEXT("00000. ") );
if ( (r.right - r.left) < 1000 )
    t.check( TEXT("00000000. ") );
width = t.width();

/*
------------------------------------
Allow for separators between columns
------------------------------------ */
i -= 2;

if ( i > width )
    width = i;

if ( old_font )
    SelectObject( dc, old_font );

ReleaseDC( MainPlotWindow, dc );

if ( f )
    DeleteObject( f );

for ( i=0; i<NofColumns; i++ )
    Grid.SetColWidth( i, width );
}

/***********************************************************************
*                             SET_CELLTEXT                             *
***********************************************************************/
static void set_celltext( int col, long row, float sorc )
{
Grid.QuickSetText( col, row, fixed_string(sorc, 5-CellPlaces, CellPlaces) );

}

/***********************************************************************
*                          CHECK_GRID_HEIGHT                           *
***********************************************************************/
static void check_grid_height( int nof_rows )
{
int dy;
int halfway;
int bottom_of_last_row;
RECTANGLE_CLASS r;
WINDOW_CLASS    w;

/*
-----------------------------------------------------------------------
Get the midway point of the plot screen. This is the highest I want the
stat screen to start.
----------------------------------------------------------------------- */
GetClientRect( MainPlotWindow, &r );
r.top += r.bottom;
halfway = r.top / 2;

/*
-----------------------------------------------------------
The bottom of the corner button is the top of the first row
----------------------------------------------------------- */
w = Grid.m_CUGCnrBtn->m_cnrBtnWnd;           //pointer to corner button window
w.getrect( r );

/*
---------------------------------
All the rows have the same height
--------------------------------- */
bottom_of_last_row = r.bottom + nof_rows * Grid.GetRowHeight(0);

/*
----------------------------------------------
The m_ctrlWnd is the handle of the entire grid
---------------------------------------------- */
w = Grid.m_ctrlWnd;
w.getrect( r );

if ( r.bottom != bottom_of_last_row )
    {
    dy = r.bottom - bottom_of_last_row;
    w.get_move_rect( r );
    r.top += dy;

    if ( r.top < halfway )
        r.top = halfway;

    MoveWindow( Grid.m_ctrlWnd, r.left, r.top, r.width(), r.height(), FALSE );
    }
}

/***********************************************************************
*                              LOAD_DATA                               *
***********************************************************************/
static void load_data( void )
{
const FIELDOFFSET float_len = SHOTPARM_FLOAT_LEN + 1;
int32    col;
int32    i;
int32    r1;
int32    nof_points;
int32    nto_average;
int32    n;
int32    pn;
long     row;
float    lcl;
float    ucl;
float    xbar;
float    y;
float    zmin;
float    z;
MINMAX   fixed;

CONTROL_LIMIT_CLASS c;
STAT_ENTRY stat[MAX_PARMS];
FIELDOFFSET offset;
SYSTEMTIME st;
TCHAR buf[PARAMETER_NAME_LEN+1];
NAME_CLASS s;
CUGCell cell;
FIXED_PARAM_CLASS fpc;

DB_TABLE t;

Current.set( ComputerName, MachineName, PartName );
c.read( ComputerName, MachineName, PartName );

Plots.init( Parameters.count() );

nof_points = get_nof_points_from_shot_range(r1);
if ( nof_points <= 0 )
    return;

set_column_widths();

n = TcConfig.n_visible();
Grid.GetCell( 0,0,&cell );
Grid.SetNumberRows( n, FALSE );
row = 0;
for ( i=0; i<CurrentParam.count(); i++ )
    {
    if ( TcConfig.visible[i] )
        {
        cell.SetText( CurrentParam.parameter[i].name );
        Grid.SetCell( -1, row, &cell );
        cell.SetText( TEXT("0.0") );
        for ( col=0; col<NofColumns; col++ )
            Grid.SetCell( col, row, &cell );
        row++;
        }
    }

check_grid_height( n );

if ( RealTime && ((TcConfig.time_period_type == SHOT_RANGE_TIME_PERIOD_TYPE)
    || ( TcConfig.chart_type != TREND_CHART_TYPE ) || GridIsVisible) )
    {
    SingleMachineOnly = FALSE;
    toggle_realtime();
    }

XAxis.init( nof_points );

Parameters.rewind();
for ( i=0; i<Parameters.count(); i++ )
    {
    Plots[i].init( nof_points );
    Parameters.next();
    pn = Parameters.value();
    lstrcpy( buf, CurrentParam.parameter[pn].name );
    fpc.read( buf );
    upper_case( buf );
    Plots[i].set_title( buf );
    Plots[i].set_units( units_name(CurrentParam.parameter[pn].units) );

    if ( TcConfig.need_warning_limits )
        Plots[i].set_warning_limits( CurrentParam.parameter[pn].limits[WARNING_MIN].value, CurrentParam.parameter[pn].limits[WARNING_MAX].value );

    if ( TcConfig.need_alarm_limits )
        Plots[i].set_alarm_limits( CurrentParam.parameter[pn].limits[ALARM_MIN].value, CurrentParam.parameter[pn].limits[ALARM_MAX].value );

    if ( fpc.is_fixed )
        {
        fixed.min = extfloat( fpc.min_value );
        fixed.max = extfloat( fpc.max_value );
        }
    else
        {
        fixed.min = 0.0;
        fixed.max = fixed.min;
        }
    Plots[i].set_fixed_limits( fixed.min, fixed.max );
    }

nto_average = 1;
if ( TcConfig.chart_type == XBAR_CHART_TYPE || TcConfig.chart_type == RANGE_CHART_TYPE )
    {
    if ( TcConfig.nto_average > 1 )
        nto_average = TcConfig.nto_average;
    }

stat[0].set_sample_size( nto_average );

hourglass_cursor();

for ( i=0; i<CurrentParam.count(); i++ )
    stat[i].set_limits( CurrentParam.parameter[i].limits[ALARM_MIN].value, CurrentParam.parameter[i].limits[ALARM_MAX].value );

s = shotparm_dbname( ComputerName, MachineName, PartName );
if ( s.file_exists() )
    {
    if (  t.open(s.text(), SHOTPARM_RECLEN, PFL) )
        {
        if ( t.nof_recs() > 0 )
            {
            if ( t.goto_record_number(r1) )
                {
                t.get_current_record( NO_LOCK );
                n = 0;
                while ( TRUE )
                    {
                    if ( n == 0 )
                        {
                        if ( !XAxis.next() )
                            break;
                        i = t.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );

                        t.get_date( st, SHOTPARM_DATE_OFFSET );
                        t.get_time( st, SHOTPARM_TIME_OFFSET );

                        if ( TcConfig.time_period_type == SHOT_RANGE_TIME_PERIOD_TYPE )
                            if ( i > TcConfig.last_shot )
                                break;
                        XAxis.set( i, st );
                        }

                    offset = SHOTPARM_PARAM_1_OFFSET;
                    for ( i=0; i<CurrentParam.count(); i++ )
                        {
                        stat[i].add( t.get_float(offset) );
                        offset += float_len;
                        }

                    Parameters.rewind();
                    for ( i=0; i<Plots.count(); i++ )
                        {
                        Parameters.next();
                        offset = SHOTPARM_PARAM_1_OFFSET + Parameters.value() * float_len;
                        y = t.get_float( offset );
                        if ( n == 0 )
                            Plots[i].next();
                        Plots[i].add( y );
                        }
                    if ( !t.get_next_record(NO_LOCK) )
                        break;
                    n++;
                    if ( n >= nto_average )
                        n = 0;
                    }

                NofPointsRead = stat[0].n;
                s = resource_string( STAT_N_EQUALS_STRING );
                s += int32toasc( NofPointsRead );
                s += resource_string( N_PER_GROUP_STRING );
                s += int32toasc( nto_average );

                cell.SetText( s.text() );
                Grid.SetCell( -1, -1, &cell );

                row = 0;
                for ( i=0; i<CurrentParam.count(); i++ )
                    {
                    if ( TcConfig.visible[i] )
                        {
                        xbar   = stat[i].avg();
                        y      = stat[i].stdev();

                        if ( is_float_zero(xbar) && is_float_zero(stat[i].max()-stat[i].min()) )
                            CellPlaces = 2;
                        else
                            CellPlaces = decimal_places( xbar  );

                        set_celltext( 0, row, stat[i].max()   );
                        set_celltext( 1, row, stat[i].min()   );
                        set_celltext( 2, row, xbar            );
                        set_celltext( 3, row, stat[i].range() );
                        set_celltext( 4, row, y               );

                        lcl = CurrentParam.parameter[i].limits[ALARM_MIN].value;
                        set_celltext( LOW_ALARM_COLUMN_NUMBER, row, lcl );

                        ucl = CurrentParam.parameter[i].limits[ALARM_MAX].value;
                        set_celltext( HIGH_ALARM_COLUMN_NUMBER, row, ucl );

                        if ( c.find(i+1) )
                            {
                            lcl = c.low_limit();
                            ucl = c.high_limit();
                            }

                        set_celltext( LCL_COLUMN_NUMBER, row, lcl );
                        set_celltext( UCL_COLUMN_NUMBER, row, ucl );

                        CellPlaces = 2;
                        if ( not_float_zero(ucl-lcl) )
                            {
                            if ( not_float_zero(y) )
                                {
                                zmin = stat[i].z(lcl);
                                set_celltext( 9, row, zmin );

                                z = stat[i].z(ucl);
                                set_celltext( 10, row, z );

                                if ( z < zmin )
                                    zmin = z;

                                set_celltext( 11, row, zmin );

                                zmin /= 3.0;
                                set_celltext( 12, row, zmin );
                                }

                            set_celltext( 13, row, stat[i].percent_out() );
                            }
                        else
                            {
                            for ( col=9; col<14; col++ )
                                set_celltext( col, row, 0.0 );
                            }
                        row++;
                        }
                    }
                }
            }

        t.close();
        }
    }

restore_cursor();
}

/***********************************************************************
*                           RELOAD_TC_DATA                             *
***********************************************************************/
void reload_tc_data( void )
{
if ( HaveCursorLine )
    erase_cursor_line();

ReadyForCursor = FALSE;
FirstIndex     = NO_INDEX;

load_data();

InvalidateRect( MainPlotWindow, 0, TRUE );
}

/***********************************************************************
*                            DO_NEW_TC_CONFIG                          *
*            if force_change != 0 then force a new config              *
***********************************************************************/
void do_new_tc_config( WPARAM force_change )
{
RECTANGLE_CLASS r;
NAME_CLASS      s;

TREND_CONFIG_CLASS tc;
INT32_ARRAY_CLASS  ia;
bool               new_config;

new_config = false;
if ( force_change != 0 )
    new_config = true;

if ( !Current.is_equal(ComputerName, MachineName, PartName) )
    new_config = true;

if ( PartBasedDisplay )
    {
    s = tc_parameter_list_filename(CurrentPart.computer, CurrentPart.machine, CurrentPart.name);
    if ( !s.file_exists() )
        s.null();
    }

if ( s.isempty() )
    s = tc_parameter_list_filename();

ia.read( s.text() );

if ( ia != Parameters )
    {
    Parameters = ia;
    new_config = true;
    }

tc.read();
if ( tc != TcConfig )
    {
    TcConfig = tc;
    new_config = true;
    }

if ( new_config )
    reload_tc_data();
}

/***********************************************************************
*                          SHOW_ONE_PARAMETER                          *
***********************************************************************/
void show_one_parameter( int32 parameter_index )
{
INT32_ARRAY_CLASS ia;
TCHAR * fname;

if ( ia.init(1) )
    {
    ia.next();
    ia.set( parameter_index );

    if ( PartBasedDisplay )
        fname = tc_parameter_list_filename(CurrentPart.computer, CurrentPart.machine, CurrentPart.name);
    else
        fname = tc_parameter_list_filename();

    ia.write( tc_parameter_list_filename() );
    }
}

/***********************************************************************
*                               CELLTEXT                               *
***********************************************************************/
TCHAR * celltext( int32 col, int32 row )
{
int32    slen;
CUGCell  cell;
const   TCHAR * cp;
static  TCHAR  * s = 0;

if ( s )
    {
    delete[] s;
    s = 0;
    }

if ( Grid.GetCell(col, row, &cell) == UG_SUCCESS )
    {
    cp = cell.GetText();
    slen = lstrlen( cp );
    if ( slen > 0 )
        {
        s = maketext( slen );
        if ( s )
            {
            lstrcpy( s, cp );
            return s;
            }
        }
    }

return EmptyString;
}

/***********************************************************************
*                              CELLVALUE                               *
***********************************************************************/
static float cellvalue( int32 col, int32 row )
{
return extfloat( celltext(col, row) );
}

/***********************************************************************
*                        PRINT_STATISTICAL_SUMMARY                     *
*                                                                      *
*                              M01 VISITRAK                            *
*                                N = 1200                              *
*                                                                      *
*                               Begin       End                        *
*                             ---------  ---------                     *
*                    Shot             1       1203                     *
*                    Date     4/20/1998  5/27/1998                     *
*                    Time      12:34:34   04:25:13                     *
*                                                                      *
*                                                  Alarm Limits        *
*            Max   Min     Average Range  Std Dev  Low     High        *
*          ------- ------- ------- ------- ------- ------ ------ ------*
*Fill Time  123.22  100.45  111.00   23.12                             *
*                                                                      *
***********************************************************************/
void print_statistical_summary( const RECT & r )
{
int32          dx;
int32          dy;
int32          n;
int32          nof_rows;
int32          col;
int32          row;
int32          date_width;
int32          space_width;
int32          label_width;
int32          x0;
int32          x;
int32          y;
int32        * places;
int32          w;
float          z;
TIME_CLASS     t;
TEXT_LEN_CLASS tmax;
STRING_CLASS   s;

dy  = character_height( MyDc );
y   = dy;

x   = (r.left + r.right) / 2;

SetTextAlign( MyDc, TA_TOP | TA_CENTER );

/*
----------------
Machine and part
---------------- */
s = MachineName;
s += SpaceString;
s += PartName;

text_out( x, y, s.text() );

y += dy;

/*
s = resource_string( STAT_N_EQUALS_STRING );
s += int32toasc( NofPointsRead );
text_out( x, y, s.text() );
*/

text_out( x, y, celltext(-1,-1) );

y += dy;

tmax.init( MyDc );
tmax.check( resource_string(ALARM_SHOT_LABEL_STRING) );
tmax.check( resource_string(ALARM_DATE_LABEL_STRING) );
tmax.check( resource_string(ALARM_TIME_LABEL_STRING) );
label_width = tmax.width();

w = label_width;

tmax.init( MyDc );
tmax.check( TEXT("A") );
space_width = tmax.width() * 3;

w += 2*space_width;

tmax.init( MyDc );
tmax.check( t.mmddyyyy() );
date_width = tmax.width();

w += 2*date_width;

x0 = r.right + r.left - w;
x0 /= 2;

x = x0;
n = XAxis.count();
if ( n > 0 )
    n--;
dx = space_width + date_width;

SetTextAlign( MyDc, TA_TOP | TA_LEFT );
text_out( x, y, resource_string(ALARM_SHOT_LABEL_STRING) );

x += label_width + dx;
SetTextAlign( MyDc, TA_TOP | TA_RIGHT );
text_out( x, y, int32toasc(XAxis[0].shot_number) );

x += dx;
text_out( x, y, int32toasc(XAxis[n].shot_number) );

y += dy;

x = x0;
SetTextAlign( MyDc, TA_TOP | TA_LEFT );
text_out( x, y, resource_string(ALARM_DATE_LABEL_STRING) );

x += label_width + dx;
SetTextAlign( MyDc, TA_TOP | TA_RIGHT );
text_out( x, y, XAxis[0].shot_time.mmddyyyy() );

x += dx;
text_out( x, y, XAxis[n].shot_time.mmddyyyy() );

y += dy;
x = x0;
SetTextAlign( MyDc, TA_TOP | TA_LEFT );
text_out( x, y, resource_string(ALARM_TIME_LABEL_STRING) );

x += label_width + dx;
SetTextAlign( MyDc, TA_TOP | TA_RIGHT );
text_out( x, y, XAxis[0].shot_time.hhmmss() );

x += dx;
text_out( x, y, XAxis[n].shot_time.hhmmss() );

/*
----------------------
Get the number of rows
---------------------- */
nof_rows = Grid.GetNumberRows();

/*
----------------------------------
Get the width of the longest label
---------------------------------- */
tmax.init( MyDc );
for ( row=0; row<nof_rows; row++ )
    tmax.check( celltext(-1, row) );

label_width = tmax.width();

dx = r.right - r.left - label_width;
dx--;
dx /= NofColumns;

y += dy;

/*
-------------------------
Print the column headings
------------------------- */
SetTextAlign( MyDc, TA_TOP | TA_CENTER );
x = r.left + label_width + HIGH_ALARM_COLUMN_NUMBER*dx;
text_out( x, y, resource_string(ALARM_LIMITS_STRING) );
y += dy;

x = r.left + label_width + dx/2;
for ( col=0; col<NofColumns; col++ )
    {
    text_out( x, y, celltext(col, -1) );
    x += dx;
    }

/*
----------------------------------------
Get the number of places for each column
---------------------------------------- */
places = new int32[nof_rows];
if ( places )
    {
    for ( row=0; row<nof_rows; row++ )
        places[row] = 4;

    for ( row=0; row<nof_rows; row++ )
        {
        z = extfloat( celltext(AVERAGE_COLUMN_NUMBER, row) );
        if ( z < 0.0 )
            z = -z;

        if ( z > 1.0 && places[row] > 3 )
            places[row] = 3;

        if ( z > 10.0 && places[row] > 2 )
            places[row] = 2;

        if ( z > 100.0 && places[row] > 1 )
            places[row] = 1;

        if ( z > 1000.0 && places[row] > 0 )
            places[row] = 0;
        }
    }

/*
--------------
Print each row
-------------- */
for ( row=0; row<nof_rows; row++ )
    {
    SetTextAlign( MyDc, TA_TOP | TA_LEFT );
    y += dy;
    x = r.left;
    text_out( x, y, celltext(-1,row) );

    SetTextAlign( MyDc, TA_TOP | TA_RIGHT );
    x += label_width + dx;

    if ( places )
        n = places[row];
    else
        n = 2;

    for ( col=0; col<NofColumns; col++ )
        {
        z = extfloat( celltext(col, row) );
        text_out( x, y, fixed_string( z, 5-n, n) );

        /*
        -----------------------------------------
        All z's, cpk's, and percents use 2 places
        ----------------------------------------- */
        if ( col == UCL_COLUMN_NUMBER )
            n = 2;

        x += dx;
        }
    }

if ( places )
    delete[] places;
}

/***********************************************************************
*                          PAINT_TREND_DISPLAY                         *
***********************************************************************/
void paint_trend_display( const RECT & ur )
{
static int32 nof_plots = 0;

int32        i;
int32        gap;
int32        plot_height;
RECTANGLE_CLASS gr;
RECT         r;
BOOLEAN      is_full_update;
BOOLEAN      had_cursor_line;
BOOLEAN      grid_show_state;
STRING_CLASS s;

if ( Printing && GridIsVisible )
    {
    print_statistical_summary( ur );
    return;
    }

is_full_update = TRUE;
ReadyForCursor = FALSE;

grid_show_state = IsWindowVisible( Grid.m_ctrlWnd );
if ( GridIsVisible )
    {
    if ( !grid_show_state )
        ShowWindow( Grid.m_ctrlWnd, SW_SHOW );

    /*
    ----------------------------------------------------
    The realtime should not be on if the grid is visible
    ---------------------------------------------------- */
    if ( RealTime )
        {
        SingleMachineOnly = FALSE;
        toggle_realtime();
        }
    }
else if ( grid_show_state )
    ShowWindow( Grid.m_ctrlWnd, SW_HIDE );

if ( XAxis.set_count() <= 0 )
    return;

if ( Printing )
    {
    r = PlotWindowRect;
    }
else
    {
    GetClientRect( MainPlotWindow, &r );
    if ( !EqualRect(&r, &ur) )
        is_full_update = FALSE;
    }

if ( GridIsVisible )
    {
    GetWindowRect( Grid.m_ctrlWnd, &gr );
    r.bottom -= gr.height() + 1;
    }

r.left += 12 * MyCharWidth;

XAxis.set_axis_color( CurrentColor[BORDER_COLOR] );
XAxis.set_label_color( CurrentColor[TEXT_COLOR] );
XAxis.set_type( INTEGER_AXIS_TYPE );
XAxis.set_title( resource_string(SHOT_NUMBER_STRING) );
XAxis.set_rect( r );

XAxis.paint( MyDc );

r.bottom = XAxis.y_intercept();

/*
-----------------------------------------------
Leave room at the top for the current shot info
----------------------------------------------- */
gap = MyCharHeight/2;
r.top += gap;

s = MachineName;
s += SpaceString;
s += PartName;

if ( TcConfig.chart_type == XBAR_CHART_TYPE && TcConfig.nto_average > 1 )
    {
    s += resource_string( N_EQUALS_STRING );
    s += int32toasc( TcConfig.nto_average );
    s += resource_string( END_PARENS_STRING );
    }

SetTextAlign( MyDc, TA_TOP | TA_CENTER );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR] );
SetBkMode(    MyDc, TRANSPARENT );

TextOut( MyDc, (r.left+r.right)/2, r.top, s.text(), s.len() );

r.top += MyCharHeight + gap;

if ( Plots.count() <= 0 )
    return;

CursorStartY    = r.top;
CursorEndY      = r.bottom;
CursorLeft      = r.left;
FirstIndex      = NO_INDEX;

plot_height = r.bottom - r.top;
plot_height -= gap * ( Plots.count() - 1 );
plot_height /= Plots.count();

/*
-------------------------------
Give up at some reasonable size
------------------------------- */
if ( plot_height < MyCharHeight )
    {
    /*
    ---------------------------------
    Only show this error message once
    --------------------------------- */
    if ( nof_plots != Plots.count() )
        {
        resource_message_box( MainInstance, NOTHING_TO_DISPLAY_STRING, TOO_MANY_GRAPHS_STRING );
        nof_plots = Plots.count();
        }

    return;
    }

/*
-------------------------------
Put the last plot at the bottom
------------------------------- */
r.top = r.bottom - plot_height;
i = Plots.count();
while ( i > 0 )
    {
    i--;
    Plots[i].set_rect( r );
    Plots[i].paint( MyDc );
    r.bottom = r.top - gap;
    r.top = r.bottom - plot_height;
    }

had_cursor_line = FALSE;
if ( is_full_update )
    {
    if ( HaveCursorLine )
        {
        /*
        ---------------------------------------
        The entire cursor line has been erased,
        I need to pretend I erased it.
        --------------------------------------- */
        had_cursor_line = TRUE;
        XAxisIndex = XAxis.x_index( CursorX );
        HaveCursorLine = FALSE;
        }
    }

if ( HaveCursorLine || had_cursor_line )
    {
    draw_cursor_line( MyDc );
    HaveCursorLine = TRUE;
    }

ReadyForCursor  = TRUE;
}

/***********************************************************************
*                           BEGIN_TREND_DISPLAY                        *
***********************************************************************/
BOOLEAN begin_trend_display( void )
{
Parameters.cleanup();
do_new_tc_config( 1 );

CursorStartY   = 0;
CursorEndY     = 0;
CursorX        = 0;
CursorLeft     = 0;
HaveCursorLine = FALSE;
ReadyForCursor = FALSE;
FirstIndex     = NO_INDEX;

if ( RealTime && !SingleMachineOnly )
    {
    SingleMachineOnly = TRUE;
    set_realtime_menu_string();
    }

return TRUE;
}

/***********************************************************************
*                         SAVE_CONTROL_LIMITS                          *
***********************************************************************/
void save_control_limits( void )
{
CONTROL_LIMIT_CLASS c;
int32 row;
int32 n;
float low_alarm;
float low_control;
float high_alarm;
float high_control;

n = Grid.GetNumberRows();
if ( n <= 0 )
    return;

for ( row=0; row<n; row++ )
    {
    low_alarm    = cellvalue( LOW_ALARM_COLUMN_NUMBER,  row );
    high_alarm   = cellvalue( HIGH_ALARM_COLUMN_NUMBER, row );
    low_control  = cellvalue( LCL_COLUMN_NUMBER,        row );
    high_control = cellvalue( UCL_COLUMN_NUMBER,        row );
    if ( not_float_zero(low_control - low_alarm) || not_float_zero(high_control - high_alarm) )
        c.add( row+1, low_control, high_control );

    }

c.write( ComputerName, MachineName, PartName );
}

/***********************************************************************
*                             END_TREND_DISPLAY                        *
***********************************************************************/
void end_trend_display( void )
{
if ( !Resetting && GridIsVisible )
    {
    GridIsVisible = FALSE;
    ShowWindow( Grid.m_ctrlWnd, SW_HIDE );
    }

erase_cursor_line();
ReadyForCursor = FALSE;
Parameters.cleanup();
XAxis.cleanup();
Plots.cleanup();
}

/***********************************************************************
*                          DO_NEW_CONTROL_LIMITS                       *
***********************************************************************/
void do_new_control_limits( void )
{
save_control_limits();
reload_tc_data();
}

/***********************************************************************
*                          TREND_SCREEN_RESET                          *
***********************************************************************/
void trend_screen_reset( void )
{
Resetting = TRUE;
end_trend_display();
begin_trend_display();
Resetting = FALSE;
}

/***********************************************************************
*                           TREND_SCREEN_NEW_SHOT                      *
***********************************************************************/
void trend_screen_new_shot( void )
{
const FIELDOFFSET float_len = SHOTPARM_FLOAT_LEN + 1;

FIELDOFFSET offset;
int32       shot_number;
int32       i;
SYSTEMTIME  st;
DB_TABLE    t;
float       y;
NAME_CLASS  s;

if ( !Current.is_equal(ComputerName, MachineName, PartName) )
    return;

if ( TcConfig.time_period_type != LAST_N_TIME_PERIOD_TYPE )
    return;

if ( TcConfig.chart_type != TREND_CHART_TYPE )
    return;

/*
-------------------------
See if this is a new shot
------------------------- */
i = XAxis.set_count() - 1;
shot_number = asctoint32( ShotName );
if ( shot_number > XAxis[i].shot_number )
    {
    s = shotparm_dbname( ComputerName, MachineName, PartName );
    if ( s.file_exists() )
        {
        if ( t.open(shotparm_dbname(ComputerName, MachineName, PartName), SHOTPARM_RECLEN, PFL) )
            {
            t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, shot_number, SHOT_LEN );
            if ( t.get_next_key_match(1, FALSE) )
                {
                t.get_date( st, SHOTPARM_DATE_OFFSET );
                t.get_time( st, SHOTPARM_TIME_OFFSET );

                XAxis.left_shift( shot_number, st );
                Parameters.rewind();
                for ( i=0; i<Plots.count(); i++ )
                    {
                    Parameters.next();
                    offset = SHOTPARM_PARAM_1_OFFSET + Parameters.value() * float_len;
                    y = t.get_float( offset );
                    Plots[i].left_shift( y );
                    }
                }
            t.close();
            }
        }
    }

InvalidateRect( MainPlotWindow, 0, TRUE );
}

/***********************************************************************
*                              DOUBLE_CLICK                            *
***********************************************************************/
static void double_click( int32 shot_number )
{
DB_TABLE t;
int32    n;
NAME_CLASS s;

shot_number = NO_SHOT_NUMBER;

s.get_graphlst_dbname( ComputerName, MachineName, PartName );
if ( s.file_exists() )
    {
    if ( t.open(s.text(), GRAPHLST_RECLEN, PFL) )
        {
        t.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, shot_number, SHOT_LEN );
        if ( t.goto_first_equal_or_greater_record(1) )
            {
            t.get_rec();
            shot_number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
            }
        else
            {
            n = t.nof_recs();
            if ( n > 0 )
                {
                n--;
                t.goto_record_number(n);
                t.get_rec();
                shot_number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
                }
            else
                {
                resource_message_box( MainInstance, NOTHING_TO_DISPLAY_STRING, NO_GRAPHS_FOUND_STRING );
                }
            }
        t.close();
        }
    }

if ( shot_number != NO_SHOT_NUMBER )
    {
    if ( RealTime )
        {
        SingleMachineOnly = FALSE;
        toggle_realtime();
        }

    update_screen_type( PROFILE_SCREEN_TYPE );

    lstrcpy( ShotName, int32toasc(shot_number) );
    fix_shotname( ShotName );
    PostMessage( MainWindow, WM_NEWSHOT, 0, 0L );
    }
}

/***********************************************************************
*                           TREND_LEFT_BUTTON_UP                       *
***********************************************************************/
void trend_escape( void )
{
if ( FirstIndex != NO_INDEX )
    {
    FirstIndex = NO_INDEX;
    InvalidateRect( MainPlotWindow, 0, TRUE );
    }
}

/***********************************************************************
*                           TREND_LEFT_BUTTON_UP                       *
***********************************************************************/
void trend_left_button_up( POINT & p )
{
HDC   dc;
int   orimode;
int32 i1;
int32 i2;

if ( p.y < CursorStartY || p.y > CursorEndY || p.x < CursorLeft )
    {
    trend_escape();
    return;
    }

if ( FirstIndex == NO_INDEX )
    {
    FirstIndex = XAxisIndex;

    dc = GetDC( MainPlotWindow );

    if ( dc )
        {
        orimode = SetROP2( dc, R2_NOT );
        MoveToEx( dc, CursorX, CursorStartY, 0 );
        LineTo(   dc, CursorX, CursorEndY      );
        SetROP2( dc, orimode );
        }

    ReleaseDC( MainPlotWindow, dc );
    }
else
    {
    if ( FirstIndex > XAxisIndex )
        {
        i1 = XAxisIndex;
        i2 = FirstIndex;
        }
    else if ( FirstIndex < XAxisIndex )
        {
        i1 = FirstIndex;
        i2 = XAxisIndex;
        }
    else
        {
        double_click( XAxis[XAxisIndex].shot_number );
        return;
        }

    TcConfig.time_period_type = SHOT_RANGE_TIME_PERIOD_TYPE;
    TcConfig.first_shot       = XAxis[i1].shot_number;
    TcConfig.last_shot        = XAxis[i2].shot_number;
    TcConfig.write();
    trend_screen_reset();
    }
}

/***********************************************************************
*                            TREND_REALTIME                            *
***********************************************************************/
void trend_realtime( void )
{
if ( TcConfig.chart_type != TREND_CHART_TYPE || TcConfig.time_period_type != LAST_N_TIME_PERIOD_TYPE )
    {
    TcConfig.chart_type       = TREND_CHART_TYPE;
    TcConfig.time_period_type = LAST_N_TIME_PERIOD_TYPE;
    TcConfig.write();
    trend_screen_reset();
    }

RealTime          = TRUE;
SingleMachineOnly = TRUE;
set_realtime_menu_string();
}

/***********************************************************************
*                            TREND_ALLSHOTS                            *
***********************************************************************/
void trend_allshots( void )
{
NAME_CLASS s;
DB_TABLE   t;
int32      n;
int32      first_shot;
int32      last_shot;

n                 = 0;
RealTime          = FALSE;
SingleMachineOnly = FALSE;
set_realtime_menu_string();

s = shotparm_dbname( ComputerName, MachineName, PartName );
if ( s.file_exists() )
    {
    if ( t.open(shotparm_dbname(ComputerName, MachineName, PartName), SHOTPARM_RECLEN, PFL) )
        {
        n = t.nof_recs();

        if ( n > 0 )
            {
            if ( t.get_next_record(FALSE) )
                {
                first_shot = t.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );
                if ( t.goto_record_number(n-1) )
                    {
                    t.get_current_record( NO_LOCK );
                    last_shot = t.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );
                    }
                else
                    {
                    n = 0;
                    }
                }
            }

        t.close();
        }
    }

if ( n > 0 )
    {
    TcConfig.time_period_type = SHOT_RANGE_TIME_PERIOD_TYPE;
    TcConfig.first_shot       = first_shot;
    TcConfig.last_shot        = last_shot;
    TcConfig.write();
    trend_screen_reset();
    }
else
    resource_message_box( MainInstance, NOTHING_TO_DISPLAY_STRING, CANNOT_COMPLY_STRING );
}
