#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\timeclas.h"

#include "..\include\array.h"
#include "..\include\catclass.h"
#include "..\include\colorcl.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\downtime.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\rectclas.h"
#include "..\include\textlen.h"
#include "..\include\textlist.h"
#include "..\include\shiftcl.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MainDialog;
WINDOW_CLASS ReportPlot;

RECTANGLE_CLASS MainRect;
RECTANGLE_CLASS PrinterMargin;

static COLOR_CLASS Black(       0,   0,   0 );
static COLOR_CLASS Gray(      160, 160, 160 );
static COLOR_CLASS LightGray( 240, 240, 240 );
static COLOR_CLASS Red(       255,   0,   0 );
static COLOR_CLASS Shadow(      0,   0,   0 );
static COLOR_CLASS White(     255, 255, 255 );

static BOOLEAN LegendIsVisible  = FALSE;
static FONT_LIST_CLASS FontList;

static HFONT FontHandle             = INVALID_FONT_HANDLE;
static HFONT TitleFontHandle        = INVALID_FONT_HANDLE;
static HFONT PrinterFontHandle      = INVALID_FONT_HANDLE;
static HFONT PrinterTitleFontHandle = INVALID_FONT_HANDLE;

static TCHAR MyIniFile[]         = TEXT( "shiftrpt.ini" );
static TCHAR PrinterSection[]    = TEXT( "Printer" );
static TCHAR ScreenSection[]     = TEXT( "Screen" );
static TCHAR GrayColorKey[]      = TEXT( "GrayColor" );
static TCHAR LightGrayColorKey[] = TEXT( "LightGrayColor" );
static TCHAR ShadowColorKey[]    = TEXT( "ShadowColor" );

static TCHAR ConfigFilePrefix[]   = TEXT( "shiftrpt-" );
static int   ConfigFilePrefixLen  = 9;
static TCHAR ConfigFileSuffix[]   = TEXT( ".dat" );
static TCHAR DefaultConfig[]      = TEXT( "default" );
static TCHAR DefaultCsvFileName[] = TEXT( "shift_report.csv" );
static STRING_CLASS CurrentConfig;

static TCHAR MyClassName[] = TEXT( "ShiftReport" );
static TCHAR ReportPlotClassName[] = TEXT( "ReportPlot" );

static TCHAR CommaString[]      = TEXT( "," );
static TCHAR SpaceString[]      = TEXT( " " );
static TCHAR StarDotStar[]      = TEXT( "*.*" );
static TCHAR ShowLegendPrefix[] = TEXT( "ShowLegend=" );
static TCHAR NString[]          = TEXT( "N" );
static TCHAR YString[]          = TEXT( "Y" );

static TCHAR CommaChar          = TEXT( ',' );
static TCHAR EqualsChar         = TEXT( '=' );
static TCHAR NullChar           = TEXT( '\0' );
static TCHAR PeriodChar         = TEXT( '.' );
static TCHAR SpaceChar          = TEXT( ' ' );

static MACHINE_NAME_LIST_CLASS Machine;
static TEXT_LIST_CLASS SelectedMachines;

DOWNCAT_NAME_CLASS  Cat;
DOWNSCAT_NAME_CLASS SubCat;
INT_ARRAY_CLASS     SubCatCount;

/*
-------------------
Time range settings
------------------- */
const int MONTHS_INDEX = 0;
const int WEEKS_INDEX  = 1;
const int DAYS_INDEX   = 2;
const int SHIFTS_INDEX = 3;
const int NOF_RANGE_SETTINGS = 4;
static STRING_CLASS StartRange[NOF_RANGE_SETTINGS];
static STRING_CLASS EndRange[NOF_RANGE_SETTINGS];

/*
-----------------------------------------------
These are used to save the start and end ranges
----------------------------------------------- */
static TCHAR * StartString[NOF_RANGE_SETTINGS] =
    {
    { TEXT( "StartMonths=" ) },
    { TEXT( "StartWeeks="  ) },
    { TEXT( "StartDays="   ) },
    { TEXT( "StartShifts=" ) }
    };

static TCHAR * EndString[NOF_RANGE_SETTINGS] =
    {
    { TEXT( "EndMonths="   ) },
    { TEXT( "EndWeeks="    ) },
    { TEXT( "EndDays="     ) },
    { TEXT( "EndShifts="   ) }
    };

/*
-------------------
Date interval types
------------------- */
const int SHIFTS_TYPE     = 1;
const int DAYS_TYPE       = 2;
const int WEEKS_TYPE      = 3;
const int MONTHS_TYPE     = 4;
const int ALL_DATES_TYPE  = 5;

static int DateIntervalType = SHIFTS_TYPE;
static TIME_CLASS StartTime;
static TIME_CLASS EndTime;

/*
-----------------------------------------------------------------------
This is used to tell the save_as_dialog whether he is loading or saving
----------------------------------------------------------------------- */
static bool SavingReport = true;

/*
-----------
Zoom states
----------- */
static bool Zooming = false;
static int  LeftCursorX  = 0;
static int  RightCursorX = 0;

/*
---------------------------
Global for get/release_mydc
--------------------------- */
static int NofDcOwners = 0;

static STRING_CLASS CommandLine;
static STRING_CLASS PlotTitle;
static STRING_CLASS CsvFileName;

/*
---------
Plot Data
--------- */
bool            PrintOnly = false;
bool            CsvOnly   = false;
bool            Printing  = false;
RECTANGLE_CLASS PrintRect;

HDC             MyDc = NULL;
RECTANGLE_CLASS PlotRect;
unsigned long   PlotPixelWidth   = 1;    /* right - left */
unsigned long   PlotSecondsWidth = 1;    /* EndTime - StartTime */

static int      ShadowHeight;
static int      ShadowWidth;

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                             CHOOSE_FONT                              *
***********************************************************************/
static bool choose_font( HFONT & font_handle, HFONT & title_font_handle )
{
FONT_CLASS lf;

if ( font_handle != INVALID_FONT_HANDLE )
    FontList.get_font( lf, font_handle );

if ( lf.choose(ReportPlot.handle()) )
    {
    if ( font_handle != INVALID_FONT_HANDLE )
        FontList.free_handle( font_handle );
    font_handle = FontList.get_handle( lf );

    if ( lf.lfWeight != FW_BOLD )
        {
        if ( title_font_handle != INVALID_FONT_HANDLE )
            FontList.free_handle( title_font_handle );

        lf.lfWeight = FW_BOLD;
        title_font_handle = FontList.get_handle( lf );
        }

    return true;
    }
return false;
}

/***********************************************************************
*                        GET_BACKGROUND_COLORS                         *
***********************************************************************/
static void get_background_colors()
{
INI_CLASS    ini;

ini.set_file( ini_file_name(MyIniFile) );

if ( Printing )
    ini.set_section( PrinterSection );
else
    ini.set_section( ScreenSection );

if ( ini.exists() )
    {
    if ( ini.find(GrayColorKey) )
        Gray.get( ini.get_string() );

    if ( ini.find(LightGrayColorKey) )
        LightGray.get( ini.get_string() );

    if ( ini.find(ShadowColorKey) )
        Shadow.get( ini.get_string() );
    }
}

/***********************************************************************
*                            GET_ALL_LIMITS                            *
***********************************************************************/
static void get_all_limits()
{
SYSTEMTIME      st;
DB_TABLE        t;
TCHAR         * filename;
TCHAR         * machine_name;
DOWNTIME_ENTRY  d;
int32           n;
short           shift;
bool            new_start;
bool            new_end;

new_start = false;
new_end   = false;

SelectedMachines.rewind();
while ( SelectedMachines.next() )
    {
    machine_name = SelectedMachines.text();
    if ( !Machine.find(machine_name) )
        break;

    filename = downtime_dbname(Machine.computer_name(), machine_name );
    if ( file_exists(filename) )
        {
        t.open( filename, DOWNTIME_RECLEN, PFL );
        if ( t.get_next_record(NO_LOCK) )
            {
            d.get( t );
            if ( d.time < StartTime )
                {
                StartTime = d.time;
                new_start = true;
                }
            n = t.nof_recs();
            n--;
            if ( n > 0 )
                {
                t.goto_record_number( n );
                t.get_current_record( NO_LOCK );
                d.get( t );
                if ( d.time > EndTime )
                    {
                    EndTime = d.time;
                    new_end = true;
                    }
                }
            }
        t.close();
        }
    }

/*
--------------------------------------------------------------------
If the start has changed I want to back up to the start of the shift
-------------------------------------------------------------------- */
if ( new_start )
    {
    st = StartTime.system_time();
    StartTime.set( start_of_shift(st) );
    }

/*
----------------------------------------------------------------------------
If the end has changed I want to go forward to the end of the current shift;
---------------------------------------------------------------------------- */
if ( new_end )
    {
    st = EndTime.system_time();
    shift = shift_number( 0, st  );

    /*
    -----------------------------------------------
    Set the end time to the start of the next shift
    ----------------------------------------------- */
    while ( true )
        {
        EndTime += SECONDS_PER_HOUR;
        st = EndTime.system_time();
        if ( shift != shift_number(0, st) )
            {
            EndTime.set( start_of_shift(st) );
            break;
            }
        }
    }

MainDialog.refresh();
}

/***********************************************************************
*                          PREVIOUS_INTERVAL                           *
***********************************************************************/
static void previous_interval()
{
SYSTEMTIME st;
WORD month;
WORD year;
unsigned long days;

if ( DateIntervalType == ALL_DATES_TYPE )
    return;

if ( !is_checked(MainDialog.control(ADD_NEW_DATA_XBOX)) )
    EndTime = StartTime;

switch ( DateIntervalType )
    {
    case SHIFTS_TYPE:
        StartTime -= SECONDS_PER_HOUR;
        break;

    case DAYS_TYPE:
        StartTime -= SECONDS_PER_DAY;
        break;

    case WEEKS_TYPE:
        StartTime -= 7 * SECONDS_PER_DAY;
        break;

    case MONTHS_TYPE:
        st = StartTime.system_time();
        month = st.wMonth;
        year  = st.wYear;
        month--;
        if ( month == 0 )
            {
            month = 12;
            year--;
            }
        days = days_in_month( month, year );
        if ( days < st.wDay )
            days = st.wDay;
        StartTime -= SECONDS_PER_DAY * days;
        break;
    }

st = StartTime.system_time();
StartTime.set( start_of_shift(st) );
ReportPlot.refresh();
}

/***********************************************************************
*                             NEXT_INTERVAL                            *
***********************************************************************/
static bool next_interval()
{
SYSTEMTIME st;
TIME_CLASS now;
short      shift;

if ( DateIntervalType == ALL_DATES_TYPE )
    return false;

now.get_local_time();

if ( !is_checked( MainDialog.control(ADD_NEW_DATA_XBOX)) )
    StartTime = EndTime;

switch ( DateIntervalType )
    {
    case SHIFTS_TYPE:
        EndTime += SECONDS_PER_HOUR;
        st = EndTime.system_time();
        shift = shift_number( 0, st  );

        /*
        -----------------------------------------------
        Set the end time to the start of the next shift
        ----------------------------------------------- */
        while ( true )
            {
            EndTime += SECONDS_PER_HOUR;
            st = EndTime.system_time();
            if ( shift != shift_number(0, st) )
                {
                EndTime.set( start_of_shift(st) );
                break;
                }
            }
        break;

    case DAYS_TYPE:
        EndTime += SECONDS_PER_DAY;
        break;

    case WEEKS_TYPE:
        EndTime += 7 * SECONDS_PER_DAY;
        break;

    case MONTHS_TYPE:
        st  = EndTime.system_time();
        EndTime += SECONDS_PER_DAY * days_in_month(st);
        break;
    }

ReportPlot.refresh();
return true;
}

/***********************************************************************
*                          RESIZE_REPORT_PLOT                          *
***********************************************************************/
static void resize_report_plot()
{
RECTANGLE_CLASS r;
WINDOW_CLASS    w;
int             h;

MainWindow.get_client_rect( r );
MainDialog.move( r );

w = MainDialog.control( SHIFTS_RADIO );
w.getrect( r );
h = r.height();
MainDialog.get_client_rect( r );
r.top = h;
ReportPlot.move( r );
}

/***********************************************************************
*                              DRAW_LINE                               *
***********************************************************************/
static void draw_line( int x1, int y1, int x2, int y2 )
{
MoveToEx( MyDc, x1, y1, 0 );
LineTo( MyDc, x2, y2 );
}

/***********************************************************************
*                              DRAW_TEXT                               *
***********************************************************************/
static void draw_text( int x, int y, TCHAR * s )
{
int slen;

slen = lstrlen( s );

TextOut( MyDc, x, y, s, slen );
}

/***********************************************************************
*                                RELOAD                                *
***********************************************************************/
static void reload()
{
int n;

Cat.read();
SubCat.read();
n = SubCat.count();
while ( n > 0 )
    {
    n--;
    SubCatCount[n] = 0;
    }

get_background_colors();
}

/***********************************************************************
*                               X_PIXEL                                *
* This only works if you have set the PlotTimeWidth since you          *
* changed the start or end time and the PlotPixelWidth;                *
***********************************************************************/
static int x_pixel( TIME_CLASS & t )
{
__int64 x;

x = t - StartTime;
x *= PlotPixelWidth;
x /= PlotSecondsWidth;
x += PlotRect.left;

return (int) x;
}

/***********************************************************************
*                               X_PIXEL                                *
* This only works if you have set the PlotTimeWidth since you          *
* changed the start or end time and the PlotPixelWidth;                *
***********************************************************************/
static BOOLEAN time_from_x_pixel( TIME_CLASS & dest, int pixel )
{
__int64 x;

if ( pixel <= PlotRect.left )
    {
    dest = StartTime;
    return TRUE;
    }
else if ( pixel >= PlotRect.right )
    {
    dest = EndTime;
    return TRUE;
    }

if ( PlotPixelWidth < 1 )
    return FALSE;

x = pixel - PlotRect.left;
x *= PlotSecondsWidth;
x /= PlotPixelWidth;

dest = StartTime;
dest += (unsigned long) x;

return TRUE;
}

/***********************************************************************
*                             PLOT_MACHINE                             *
***********************************************************************/
void plot_machine( TCHAR * machine_name, int ytop, int ybottom )
{
TCHAR         * filename;
SYSTEMTIME      st;
DB_TABLE        t;
int             i;
int             n;
RECTANGLE_CLASS border;
RECTANGLE_CLASS r;
DOWNTIME_ENTRY  this_dte;
DOWNTIME_ENTRY  next_dte;
TIME_CLASS      now;

now.get_local_time();

/*
-------------------------------------------------------------------------
If the start time is greater than the current time there is nothing to do
------------------------------------------------------------------------- */
if ( StartTime > now )
    return;

r.top    = ytop + 2;
r.bottom = ybottom - 2;

border.top    = r.top;
border.bottom = r.bottom;
border.left   = PlotRect.right;
border.right  = PlotRect.left;

st = StartTime.system_time();

if ( !Machine.find(machine_name) )
    return;

filename = downtime_dbname(Machine.computer_name(), machine_name );
if ( !file_exists(filename) )
    return;

t.open( filename, DOWNTIME_RECLEN, PFL );
t.put_date( DOWNTIME_DATE_OFFSET, st );
t.put_time( DOWNTIME_TIME_OFFSET, st );
if ( t.goto_first_equal_or_greater_record(2) )
    {
    n = t.current_record_number();
    t.get_current_record( NO_LOCK );
    this_dte.get( t );
    next_dte = this_dte;
    if ( next_dte.time > EndTime )
        next_dte.time = EndTime;

    if ( next_dte.time > now )
        next_dte.time = now;


    /*
    ------------------------------------------------------------------------------
    This is the first record of the shift. This is unique because I probably have
    to go back one record and get the seconds before this record that are still in
    the shift.
    ------------------------------------------------------------------------------ */
    if ( this_dte.time > StartTime )
        {
        if ( n == 0 )
            {
            /*
            -----------------------------------------------------------
            This is the 0th record, meaning that there is no way for me
            to know what happened since the start of the shift;
            ----------------------------------------------------------- */
            copystring( this_dte.cat,    DOWNCAT_SYSTEM_CAT );
            copystring( this_dte.subcat, DOWNCAT_UNSP_DOWN_SUBCAT );
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
            this_dte.get( t );

            n++;
            t.goto_record_number( n );
            }

        r.left  = x_pixel( StartTime );
        r.right = x_pixel( next_dte.time );
        i = SubCat.index( this_dte.cat, this_dte.subcat );
        SubCatCount[i]++;
        r.fill( MyDc, SubCat.color(this_dte.cat, this_dte.subcat) );
        if ( r.left < border.left )
            border.left = r.left;
        if ( r.right > border.right )
            border.right = r.right;

        this_dte = next_dte;
        }

    while ( true )
        {
        if ( t.get_next_record(NO_LOCK) )
            {
            next_dte.get( t );
            if ( next_dte.time > EndTime )
                next_dte.time = EndTime;
            }
        else
            {
            next_dte.time = EndTime;

            if ( next_dte.time > now )
                next_dte.time = now;
            }

        r.left  = x_pixel( this_dte.time );
        r.right = x_pixel( next_dte.time );
        i = SubCat.index( this_dte.cat, this_dte.subcat );
        SubCatCount[i]++;
        r.fill( MyDc, SubCat.color(this_dte.cat, this_dte.subcat) );
        if ( r.left < border.left )
            border.left = r.left;
        if ( r.right > border.right )
            border.right = r.right;

        if ( next_dte.time == EndTime || next_dte.time == now )
            break;

        this_dte = next_dte;
        }
    }
else
    {
    if ( StartTime < now )
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
            this_dte.get( t );
            }
        else
            {
            copystring( this_dte.cat,    DOWNCAT_SYSTEM_CAT );
            copystring( this_dte.subcat, DOWNCAT_UNSP_DOWN_SUBCAT );
            }

        r.left  = x_pixel( StartTime );
        if ( EndTime > now )
            r.right = x_pixel( now );
        else
            r.right = x_pixel( EndTime );
        i = SubCat.index( this_dte.cat, this_dte.subcat );
        SubCatCount[i]++;
        r.fill( MyDc, SubCat.color(this_dte.cat, this_dte.subcat) );
        if ( r.left < border.left )
            border.left = r.left;
        if ( r.right > border.right )
            border.right = r.right;
        }
    }
t.close();

if ( border.right > border.left )
    border.draw( MyDc, Black.color );
}

/***********************************************************************
*                      DRAW_RECTANGLE_BACKGROUND                       *
***********************************************************************/
static void draw_rectangle_background( RECTANGLE_CLASS & sorc )
{
RECTANGLE_CLASS r;

/*
---------------
Draw the border
--------------- */
r = sorc;
r.draw( MyDc, Black.color );

/*
-------------------
Fill the background
------------------- */
r.top++;
//r.bottom--;
r.left++;
//r.right--;
r.fill( MyDc, LightGray.color );

/*
---------------------
Draw the right shadow
--------------------- */
r = sorc;
r.top    += ShadowHeight;
r.bottom += ShadowHeight;
r.left = sorc.right + 1;
r.right = r.left + ShadowWidth;
r.fill( MyDc, Shadow.color );

/*
----------------------
Draw the bottom shadow
---------------------- */
r = sorc;
r.top     = sorc.bottom + 1;
r.bottom  = r.top + ShadowHeight;
r.left   += 2*ShadowWidth;
r.right  += ShadowWidth;
r.fill( MyDc, Shadow.color );
}

/***********************************************************************
*                          PAINT_REPORT_PLOT                           *
***********************************************************************/
static void paint_report_plot()
{
static TCHAR    SampleXTick[] = TEXT( "00 " );

RECTANGLE_CLASS background;
RECTANGLE_CLASS plot_border;
RECTANGLE_CLASS legend_border;
RECTANGLE_CLASS legend;
RECTANGLE_CLASS r;

TEXT_LEN_CLASS  t;
TEXT_LEN_CLASS  t2;
HFONT           oldfont;
HFONT           myfont;
STRING_CLASS    s;
TIME_CLASS      mytime;
TIME_CLASS      plotstart;
SYSTEMTIME      st;
unsigned long   interval;
int             ch;
int             colorsum;
int             cw;
int             greenpart;
int             tick_height;
int             tick_width;
int             nof_tick_labels;
int             n;
int             plot_base_height;
int             x;
int             y;
HPEN            mypen;
HPEN            oldpen;
COLOR_CLASS     mycolor;
COLOR_CLASS     textcolor;

PlotSecondsWidth  = (int) (EndTime-StartTime);

mypen = CreatePen( PS_SOLID, 1, Black.color );

if ( mypen )
    oldpen = (HPEN) SelectObject( MyDc, mypen );

if ( Printing )
    background = PrintRect;
else
    ReportPlot.get_client_rect( background );

if ( !Printing )
    background.fill( MyDc, White.color );

if ( Printing && (PrinterFontHandle != INVALID_FONT_HANDLE) )
    oldfont = (HFONT) SelectObject( MyDc, PrinterFontHandle );
else if ( FontHandle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( MyDc, FontHandle );
else
    oldfont = INVALID_FONT_HANDLE;

cw = average_character_width( MyDc );
ch = character_height( MyDc );

/*
---------------------------------------
Calculate the height and width of ticks
--------------------------------------- */
tick_width = 5*cw/4;
tick_height = 2*ch/3;

/*
-----------------------------------------
Calculate the height and width of shadows
----------------------------------------- */
ShadowWidth  = cw;
ShadowHeight = ShadowWidth;

/*
--------------------------------------
Draw a gray rectangle on the left side
-------------------------------------- */
r = background;
r.right = r.left + r.width()/6;
r.fill( MyDc, Gray.color );

/*
-----
Title
----- */
myfont = INVALID_FONT_HANDLE;
if ( Printing )
    {
    if ( PrinterTitleFontHandle != INVALID_FONT_HANDLE )
        myfont = (HFONT) SelectObject( MyDc, PrinterTitleFontHandle );
    }
else if ( TitleFontHandle != INVALID_FONT_HANDLE )
    {
    myfont = (HFONT) SelectObject( MyDc, TitleFontHandle );
    }

SetTextAlign( MyDc, TA_TOP | TA_LEFT );
SetTextColor( MyDc, Black.color );
SetBkMode( MyDc, TRANSPARENT );

x = r.right + cw;
y = ch;

TextOut( MyDc, x, y, PlotTitle.text(), PlotTitle.len() );

if ( myfont != INVALID_FONT_HANDLE )
    SelectObject( MyDc, myfont );

/*
----------
Time Range
---------- */
y += ch;

s = StartTime.mmddyy();
s += SpaceString;
s += StartTime.hhmm();
s += TEXT( "   --   " );
s += EndTime.mmddyy();
s += SpaceString;
s += EndTime.hhmm();

TextOut( MyDc, x, y, s.text(), s.len() );

/*
-------------------------------
Draw a red line under the title
------------------------------- */
y += 3 * ch / 2;

r = background;
r.top = y;
y += 2;

r.bottom = y;
r.left = x;
r.fill( MyDc, Red.color );

//y += ch;

plot_border = background;
plot_border.top = y;
plot_border.right -= ShadowWidth;
plot_border.left += background.width()/30;
plot_border.bottom -= ShadowHeight;

/*
-----------------------------------
Get the height needed for the plot;
----------------------------------- */
interval = 2 * ch;
plot_base_height = tick_height + 5*ch/2;   /* tick plus units, name, and one space */

y = SelectedMachines.count() * interval;
y += plot_base_height;
y += ch;

n = plot_border.height();
if ( n > y )
    {
    n -= y;
    n /= 2;
    if ( n > 0 )
        {
        plot_border.top += n;
        plot_border.bottom -= n;
        }
    }


if ( is_checked( MainDialog.control(SHOW_LEGEND_XBOX) ) )
    {
    legend_border = background;
    legend_border.right -= ShadowWidth;

    t.init( MyDc );
    t2.init( MyDc );

    n = Cat.count();
    while ( n > 0 )
        {
        n--;
        t.check( Cat[n].name );
        }

    n = SubCat.count();
    while ( n > 0 )
        {
        n--;
        t2.check(SubCat[n].name );
        SubCatCount[n] = 0;
        }

    legend_border.left = legend_border.right - t.width() - t2.width() - 5*cw;    /* 2 spaces on each side + space in the middle */
    plot_border.right  = legend_border.left - cw - ShadowWidth;
    }

draw_rectangle_background( plot_border );

/*
--------------
Plot rectangle
-------------- */
t.init( MyDc );
SelectedMachines.rewind();
while ( SelectedMachines.next() )
    t.check( SelectedMachines.text() );

PlotRect = plot_border;
PlotRect.left += t.width() + cw;
PlotRect.left += tick_width;
PlotRect.right -= cw;
PlotRect.bottom -= plot_base_height;

/*
-------------------------------------------------------
Set the global that is used to calculate the x position
------------------------------------------------------- */
PlotPixelWidth = (int) PlotRect.width();

x = PlotRect.left;
x -= tick_width/2;
y = PlotRect.bottom + tick_height;

draw_line( x, PlotRect.top+ch, x,              PlotRect.bottom );
draw_line( x, PlotRect.bottom, PlotRect.right, PlotRect.bottom );

/*
-----------------------------------------------------
X axis ticks: each label is 2 chars wide plus a space
----------------------------------------------------- */
nof_tick_labels = PlotRect.right - PlotRect.left + 2*cw;
nof_tick_labels /= pixel_width( MyDc, SampleXTick );

n = (EndTime - StartTime ) / SECONDS_PER_HOUR;
if ( n <= nof_tick_labels )
    {

    /*
    -----------
    Major Ticks
    ----------- */
    mytime = StartTime;
    while ( mytime.system_time().wMinute != 0 )
        mytime += (unsigned long) 1;

    plotstart = mytime;

    SetTextAlign( MyDc, TA_TOP | TA_CENTER );

    y = PlotRect.bottom + tick_height;
    while ( mytime < EndTime )
        {
        x = x_pixel( mytime );
        draw_line( x, PlotRect.bottom, x,  y );
        draw_text( x, y, mytime.hh() );
        mytime += SECONDS_PER_HOUR;
        }

    /*
    ------------
    Hours string
    ------------ */
    y += ch;
    x = plot_border.left + plot_border.right;
    x /= 2;
    draw_text( x, y, resource_string(HOURS_STRING) );

    /*
    -----------
    Minor Ticks
    ----------- */
    mytime = plotstart;

    y = PlotRect.bottom + tick_height/2;
    interval = SECONDS_PER_HOUR/4;

    while ( mytime > StartTime )
        mytime -= interval;

    mytime += interval;

    while ( mytime < EndTime )
        {
        x = x_pixel( mytime );
        draw_line( x, PlotRect.bottom, x,  y );
        mytime += interval;
        }
    }
else
    {
    n = (EndTime - StartTime ) / SECONDS_PER_DAY;
    if ( n <= nof_tick_labels )
        {
        /*
        -----------
        Major Ticks
        ----------- */
        mytime = StartTime;
        while ( mytime.system_time().wHour != 0 )
            mytime += (unsigned long) SECONDS_PER_MINUTE;

        /*
        ------------------------------------------------------------------
        Back up past the first second of the day and then increment by one
        ------------------------------------------------------------------ */
        while ( mytime.system_time().wHour == 0 )
            mytime -= (unsigned long) 1;

        mytime += (unsigned long) 1;

        /*
        ---------------------------------------------
        Save this so I can use it for the minor ticks
        --------------------------------------------- */
        plotstart = mytime;

        SetTextAlign( MyDc, TA_TOP | TA_CENTER );

        while ( mytime < EndTime )
            {
            x = x_pixel( mytime );
            draw_line( x, PlotRect.bottom, x,  y );
            draw_text( x, y, mytime.dd() );
            mytime += SECONDS_PER_DAY;
            }

        /*
        -----------
        Minor Ticks
        ----------- */
        mytime = plotstart;

        y = PlotRect.bottom + tick_height;
        interval = SECONDS_PER_HOUR*6;

        while ( mytime > StartTime )
            mytime -= interval;

        mytime += interval;

        while ( mytime < EndTime )
            {
            x = x_pixel( mytime );
            draw_line( x, PlotRect.bottom, x,  y );
            mytime += interval;
            }

        /*
        -----------
        Days string
        ----------- */
        y += ch;
        x = plot_border.left + plot_border.right;
        x /= 2;
        draw_text( x, y, resource_string(DAYS_STRING) );
        }
    else
        {
        /*
        --------------
        Label by month
        -------------- */
        SetTextAlign( MyDc, TA_TOP | TA_CENTER );

        mytime = StartTime;
        st = mytime.system_time();
        st.wHour         = 0;
        st.wMinute       = 0;
        st.wSecond       = 0;
        st.wMilliseconds = 0;
        mytime.set( st );

        while ( true )
            {
            mytime += (unsigned long) SECONDS_PER_DAY;
            if ( mytime > EndTime )
                break;

            st = mytime.system_time();
            if ( st.wDay == 1 )
                {
                x = x_pixel( mytime );
                y = PlotRect.bottom + tick_height;
                draw_line( x, PlotRect.bottom, x,  y );
                draw_text( x, y, mytime.mm() );
                }
            }

        /*
        -------------
        Months string
        ------------- */
        y += ch;
        x = plot_border.left + plot_border.right;
        x /= 2;
        draw_text( x, y, resource_string(MONTHS_STRING) );
        }
    }

x = PlotRect.left - tick_width;
interval = ch * 2;
y = PlotRect.bottom - (2*ch);

SetTextAlign( MyDc, TA_TOP | TA_RIGHT );
SelectedMachines.rewind();
while ( SelectedMachines.next() )
    {
    draw_text( x, y+ch/2, SelectedMachines.text() );
    plot_machine( SelectedMachines.text(), y, y+interval );
    y -= interval;
    }

/*
---------------
Draw the Legend
--------------- */
if ( is_checked( MainDialog.control(SHOW_LEGEND_XBOX) ) )
    {
    interval = ch + 2;
    legend = legend_border;
    legend.left += cw;
    legend.right -= cw;

    /*
    -----------------------------------------------------------------
    See if the legend will fit in the same vertical space as the plot
    ----------------------------------------------------------------- */
    y = 0;
    n = 0;
    for ( n=0; n<SubCat.count(); n++ )
        {
        if ( SubCatCount[n] > 0 )
            y += interval;
        }

    n = legend.height();
    if ( n > y )
        {
        /*
        ----------------------------------------------------
        See if there is room for a gap at the top and bottom
        ---------------------------------------------------- */
        n -= y;
        if ( n >= (2*ch) )
            {
            n -= 2*ch;
            legend.top += ch;
            legend.bottom -= ch;
            }

        n /= 2;
        if ( n > 0 )
            {
            legend.top    += n;
            legend.bottom -= n;
            legend_border.top += n;
            legend_border.bottom -= n;
            }
        }

    draw_rectangle_background( legend_border );

    SetTextAlign( MyDc, TA_TOP | TA_LEFT );
    x = legend.left + cw;
    y = legend.top + 1;
    n = 0;

    r = legend;

    while ( n < SubCat.count() )
        {
        if ( SubCatCount[n] > 0 )
            {
            mycolor = SubCat[n].color;
            greenpart = mycolor.green_component();
            colorsum = greenpart + mycolor.blue_component() + mycolor.red_component();

            r.top = y;
            r.bottom = y + interval;
            r.fill( MyDc, mycolor.color );

            textcolor = White;
            if ( colorsum > 550 || (greenpart > 200 && colorsum<400) )
                textcolor = Black;

            if ( greenpart > 230 && mycolor.red_component() > 230 )
                textcolor = Black;

            SetTextColor( MyDc, textcolor.color );

            s = Cat.name(SubCat[n].catnumber);
            s += TEXT(" ");
            s += SubCat[n].name;
            draw_text( x, y, s.text() );
            y += interval;
            }
        n++;
        }
    }

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( MyDc, oldfont );

if ( mypen )
    {
    SelectObject( MyDc, oldpen );
    DeleteObject( mypen );
    }
}

/***********************************************************************
*                            GET_PRINTER_DC                            *
***********************************************************************/
void get_printer_dc()
{
static PRINTDLG   pd;
DEVMODE  * dm;

pd.lStructSize = sizeof(PRINTDLG);
pd.hwndOwner   = MainDialog.handle();
pd.hDevMode    = 0;
pd.hDevNames   = 0;
pd.hDC         = 0;
pd.Flags       = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_HIDEPRINTTOFILE | PD_NOSELECTION | PD_NOPAGENUMS;
pd.nFromPage   = 1;
pd.nToPage     = 1;
pd.nMinPage    = 1;
pd.nMaxPage    = 1;
pd.nCopies     = 1;
pd.hInstance   = MainInstance;
pd.lCustData            = 0;
pd.lpfnPrintHook        = 0;
pd.lpfnSetupHook        = 0;
pd.lpPrintTemplateName  = 0;
pd.lpSetupTemplateName  = 0;
pd.hPrintTemplate       = 0;
pd.hSetupTemplate       = 0;

if ( PrintDlg(&pd) )
    {
    MyDc = pd.hDC;
    if ( pd.hDevMode )
        {
        dm = (DEVMODE *) GlobalLock( pd.hDevMode );
        if ( dm->dmOrientation == DMORIENT_PORTRAIT )
            {
            PrinterMargin.right  =   0;    /* mils */
            PrinterMargin.bottom = 500;
            }
        else
            {
            PrinterMargin.right  = 500;  /* mils */
            PrinterMargin.bottom =   0;
            }

        GlobalUnlock( pd.hDevMode );
        GlobalFree( pd.hDevMode );
        }

    if ( pd.hDevNames )
        GlobalFree( pd.hDevNames );
    }
}

/***********************************************************************
*                          GET_DEFAULT_PRINTER_DC                      *
***********************************************************************/
void get_default_printer_dc()
{
PRINTER_INFO_2 * p;
DWORD     n;
DWORD     bytes_needed;
DWORD     slen;

DEVMODE * dm;
TCHAR   * cp;
TCHAR     s[MAX_PATH+1];
HANDLE    ph;

GetProfileString( TEXT("windows"), TEXT("device"), TEXT("..."), s, MAX_PATH );
cp = findchar( CommaChar, s );
if ( cp )
    *cp = NullChar;

if ( OpenPrinter(s, &ph, 0) )
    {
    bytes_needed = 0;

    if ( !GetPrinter(ph, 2, 0, 0, &bytes_needed) )
        {
        if ( bytes_needed > 0 )
            {
            slen = sizeof( PRINTER_INFO_2 );
            n = bytes_needed / slen + 1;
            slen *= n;

            p = new PRINTER_INFO_2[n];
            if ( p )
                {
                if ( GetPrinter(ph, 2, (BYTE *) p, slen, &bytes_needed) )
                    {
                    dm = p->pDevMode;
                    if ( dm )
                        {
                        if ( dm->dmOrientation == DMORIENT_PORTRAIT )
                            {
                            PrinterMargin.right  =   0;    /* mils */
                            PrinterMargin.bottom = 500;
                            }
                        else
                            {
                            PrinterMargin.right  = 500;  /* mils */
                            PrinterMargin.bottom =   0;
                            }
                        MyDc = CreateDC( 0, s, 0, 0 );
                        }

                    }
                delete[] p;
                }
            else
                {
                resource_message_box( MainInstance, PRINTER_LIST_STRING, NO_MEMORY_STRING );
                }
            }
        }

    ClosePrinter( ph );
    }
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static void paint_me()
{
PAINTSTRUCT     ps;

MyDc = BeginPaint( ReportPlot.handle(), &ps );
if ( !PrintOnly && !CsvOnly )
    paint_report_plot();
MyDc = NULL;
EndPaint( ReportPlot.handle(), &ps );
}

/***********************************************************************
*                               PRINT_ME                               *
***********************************************************************/
static void print_me( bool need_to_ask_which_printer )
{
static DOCINFO di = { sizeof(DOCINFO), TEXT("TrueTrak Shift Report"), 0 };
POINT           p;
int             x;

if ( need_to_ask_which_printer )
    get_printer_dc();
else
    get_default_printer_dc();

if ( MyDc )
    {
    Printing = true;
    get_background_colors();
    if ( StartDoc(MyDc, &di) > 0 )
        {
        if ( StartPage(MyDc) > 0 )
            {
            p.x = GetDeviceCaps( MyDc, PHYSICALOFFSETX );
            p.y = GetDeviceCaps( MyDc, PHYSICALOFFSETY );


            PrintRect.left   = p.x;
            PrintRect.top    = p.y;
            PrintRect.right  = GetDeviceCaps( MyDc, HORZRES ) - p.x;
            PrintRect.bottom = GetDeviceCaps( MyDc, VERTRES ) - p.y;

            x = GetDeviceCaps( MyDc, LOGPIXELSX );

            if ( PrinterMargin.left > 0 )
                PrintRect.left  += (x * PrinterMargin.left) / 1000;

            if ( PrinterMargin.right > 0 )
                PrintRect.right -= (x * PrinterMargin.right) / 1000;

            x = GetDeviceCaps( MyDc, LOGPIXELSY );

            if ( PrinterMargin.top > 0 )
                PrintRect.top += (x * PrinterMargin.top) / 1000;

            if ( PrinterMargin.bottom > 0 )
                PrintRect.bottom -= (x * PrinterMargin.bottom) / 1000;

            paint_report_plot();

            if ( EndPage(MyDc) > 0 )
                EndDoc( MyDc );
            }
        }
    DeleteDC( MyDc );
    MyDc = 0;
    Printing = false;
    get_background_colors();
    }

MyDc = 0;
}

/***********************************************************************
*                               GET_MYDC                               *
***********************************************************************/
static void get_mydc( void )
{

if ( !MyDc )
    {
    MyDc = ReportPlot.get_dc();
    }

NofDcOwners++;
}

/***********************************************************************
*                             RELEASE_MYDC                             *
***********************************************************************/
static void release_mydc( void )
{

if ( !MyDc )
    return;

if ( NofDcOwners )
    NofDcOwners--;

if ( NofDcOwners == 0 )
    {
    ReportPlot.release_dc( MyDc );
    MyDc = NULL;
    }

}

/***********************************************************************
*                           MAKE_CSV_STRING                            *
***********************************************************************/
static void make_csv_string( STRING_CLASS & dest, TIME_CLASS & t1, TIME_CLASS & t2, TCHAR * category, TCHAR * subcategory )
{
COLOR_CLASS c;

dest = Machine.name();
dest += CommaString;

dest += t1.text();
dest += CommaString;

dest += t2.text();
dest += CommaString;

dest += category;
dest += CommaString;

dest += subcategory;
dest += CommaString;

dest += Cat.name( category );
dest += CommaString;

dest += SubCat.name( category, subcategory );
dest += CommaString;

c = SubCat.color( category, subcategory );

dest += c.put();
}

/***********************************************************************
*                             WRITE_CSV_FILE                           *
***********************************************************************/
static void write_csv_file()
{
static TCHAR title[] = TEXT( "Machine, Start Time, End Time, Category, Sub-Category, Category Name, Sub-Category Name, Red, Green, Blue" );
TCHAR         * filename;
SYSTEMTIME      st;
DB_TABLE        t;
int             n;
DOWNTIME_ENTRY  this_dte;
DOWNTIME_ENTRY  next_dte;
TIME_CLASS      now;
FILE_CLASS      f;
STRING_CLASS    s;

if ( CsvFileName.isempty() )
    return;

if ( !f.open_for_write( CsvFileName.text() ) )
    return;

f.writeline( title );

s.upsize( 150 );
now.get_local_time();

/*
-------------------------------------------------------------------------
If the start time is greater than the current time there is nothing to do
------------------------------------------------------------------------- */
if ( StartTime > now )
    return;

SelectedMachines.rewind();
while ( SelectedMachines.next() )
    {
    st = StartTime.system_time();

    if ( !Machine.find(SelectedMachines.text()) )
        continue;

    filename = downtime_dbname( Machine.computer_name(), Machine.name() );
    if ( !file_exists(filename) )
        return;

    t.open( filename, DOWNTIME_RECLEN, PFL );
    t.put_date( DOWNTIME_DATE_OFFSET, st );
    t.put_time( DOWNTIME_TIME_OFFSET, st );
    if ( t.goto_first_equal_or_greater_record(2) )
        {
        n = t.current_record_number();
        t.get_current_record( NO_LOCK );
        this_dte.get( t );
        next_dte = this_dte;
        if ( next_dte.time > EndTime )
            next_dte.time = EndTime;

        if ( next_dte.time > now )
            next_dte.time = now;

        /*
        ------------------------------------------------------------------------------
        This is the first record of the shift. This is unique because I probably have
        to go back one record and get the seconds before this record that are still in
        the shift.
        ------------------------------------------------------------------------------ */
        if ( this_dte.time > StartTime )
            {
            if ( n == 0 )
                {
                /*
                -----------------------------------------------------------
                This is the 0th record, meaning that there is no way for me
                to know what happened since the start of the shift;
                ----------------------------------------------------------- */
                copystring( this_dte.cat,    DOWNCAT_SYSTEM_CAT );
                copystring( this_dte.subcat, DOWNCAT_UNSP_DOWN_SUBCAT );
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
                this_dte.get( t );

                n++;
                t.goto_record_number( n );
                }

            make_csv_string( s, StartTime, next_dte.time, this_dte.cat, this_dte.subcat );
            f.writeline( s.text() );

            this_dte = next_dte;
            }

        while ( true )
            {
            if ( t.get_next_record(NO_LOCK) )
                {
                next_dte.get( t );
                if ( next_dte.time > EndTime )
                    next_dte.time = EndTime;
                }
            else
                {
                next_dte.time = EndTime;

                if ( next_dte.time > now )
                    next_dte.time = now;
                }

            make_csv_string( s, this_dte.time, next_dte.time, this_dte.cat, this_dte.subcat );
            f.writeline( s.text() );

            if ( next_dte.time == EndTime || next_dte.time == now )
                break;

            this_dte = next_dte;
            }
        }
    else
        {
        if ( StartTime < now )
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
                this_dte.get( t );
                }
            else
                {
                copystring( this_dte.cat,    DOWNCAT_SYSTEM_CAT );
                copystring( this_dte.subcat, DOWNCAT_UNSP_DOWN_SUBCAT );
                }

            if ( EndTime > now )
                make_csv_string( s, StartTime, now, this_dte.cat, this_dte.subcat );
            else
                make_csv_string( s, StartTime, EndTime, this_dte.cat, this_dte.subcat );
            f.writeline( s.text() );
            }
        }
    t.close();
    }

f.close();
}

/***********************************************************************
*                           VOID CANCEL_ZOOM                           *
***********************************************************************/
static void cancel_zoom()
{
Zooming = 0;
LeftCursorX = 0;
RightCursorX = 0;
MainDialog.refresh();
}

/***********************************************************************
*                               DO_ZOOM                                *
***********************************************************************/
static void do_zoom()
{
TIME_CLASS st;
TIME_CLASS et;
SYSTEMTIME s;
int        x;
short      shift;

bool new_start;
bool new_end;

new_start = false;
new_end   = false;

st = StartTime;
et = EndTime;

if ( LeftCursorX > RightCursorX )
    {
    x = LeftCursorX;
    LeftCursorX  = RightCursorX;
    RightCursorX = x;
    }

if ( time_from_x_pixel(st, LeftCursorX) )
    {
    if ( time_from_x_pixel(et, RightCursorX) )
        {
        if ( st != StartTime )
            {
            StartTime = st;
            new_start = true;
            }

        if ( et != EndTime )
            {
            EndTime = et;
            new_end = true;
            }

        }
    }

/*
--------------------------------------------------------------------
If the start has changed I want to back up to the start of the shift
-------------------------------------------------------------------- */
if ( new_start )
    {
    s = StartTime.system_time();
    StartTime.set( start_of_shift(s) );
    }

/*
----------------------------------------------------------------------------
If the end has changed I want to go forward to the end of the current shift;
---------------------------------------------------------------------------- */
if ( new_end )
    {
    s = EndTime.system_time();
    shift = shift_number( 0, s  );

    /*
    -----------------------------------------------
    Set the end time to the start of the next shift
    ----------------------------------------------- */
    while ( true )
        {
        EndTime += SECONDS_PER_HOUR;
        s = EndTime.system_time();
        if ( shift != shift_number(0, s) )
            {
            EndTime.set( start_of_shift(s) );
            break;
            }
        }
    }



Zooming = 0;
LeftCursorX = 0;
RightCursorX = 0;
MainDialog.refresh();
}

/***********************************************************************
*                           DRAW_CURSOR_LINE                           *
***********************************************************************/
static void draw_cursor_line( int x )
{
int     orimode;

get_mydc();

if ( MyDc )
    {
    orimode = SetROP2( MyDc, R2_NOT );

    MoveToEx( MyDc, x, PlotRect.top, 0 );
    LineTo(   MyDc, x, PlotRect.bottom );

    SetROP2( MyDc, orimode );
    }

release_mydc();
}

/***********************************************************************
*                            X_IN_PLOTRECT                             *
* The x in the PlotRect is never zero. I see if the mouse is in the    *
* plot rect and, if so, return the valid value. Otherwise I return 0   *
***********************************************************************/
static int x_in_plotrect( LPARAM lParam )
{
POINT p;

p.x = (LONG) ( LOWORD(lParam) );
p.y = (LONG) ( HIWORD(lParam) );

if ( PlotRect.contains(p) )
    return (int) p.x;

return 0;
}

/***********************************************************************
*                             DO_MOUSEMOVE                             *
***********************************************************************/
static void do_mousemove( LPARAM lParam )
{
int x;

if ( !Zooming )
    return;

x = x_in_plotrect( lParam );
if ( !x )
    return;

if ( RightCursorX )
    {
    if ( x == RightCursorX )
        return;
    draw_cursor_line( RightCursorX );
    RightCursorX  = x;
    draw_cursor_line( RightCursorX );
    }
else
    {
    if ( x == LeftCursorX )
        return;
    draw_cursor_line( LeftCursorX );
    LeftCursorX  = x;
    draw_cursor_line( LeftCursorX );
    }
}

/***********************************************************************
*                            DO_LBUTTONDOWN                            *
***********************************************************************/
static void do_lbuttondown( LPARAM lParam )
{
int x;

x = x_in_plotrect( lParam );
if ( !x )
    {
    if ( !Zooming )
        return;

    if ( !RightCursorX )
        return;

    x = (int) ( LOWORD(lParam) );
    if ( x < PlotRect.left )
        x = PlotRect.left;
    else if ( x >= PlotRect.right )
        x = PlotRect.right -1;
    }

if ( x )
    {
    if ( !Zooming )
        {
        Zooming = true;
        LeftCursorX = x;
        draw_cursor_line( LeftCursorX );
        }
    else if ( RightCursorX )
        {
        do_zoom();
        }
    else
        {
        RightCursorX = x;
        draw_cursor_line( RightCursorX );
        }
    }
}

/***********************************************************************
*                            DO_POPUP_MENU                             *
***********************************************************************/
void do_popup_menu( LPARAM lParam )
{
HMENU m;
POINT p;

m = CreatePopupMenu();
if ( !m )
    return;

p.x = LOWORD( lParam );
p.y = HIWORD( lParam );

ClientToScreen( ReportPlot.handle(), &p );

if ( !Zooming )
    AppendMenu( m, MF_STRING, ZOOM_POPUP, resource_string(ZOOM_POPUP_STRING) );
AppendMenu( m, MF_STRING, FONT_POPUP, resource_string(FONT_POPUP_STRING) );
AppendMenu( m, MF_STRING, PRINTER_FONT_POPUP, resource_string(PRINTER_FONT_POPUP_STRING) );

TrackPopupMenu( m, TPM_LEFTALIGN | TPM_LEFTBUTTON, p.x, p.y, 0, ReportPlot.handle(), 0 );

DestroyMenu( m );
}

/***********************************************************************
*                            DO_ZOOM_POPUP                             *
***********************************************************************/
void do_zoom_popup()
{

LPARAM lParam;
POINT  p;

GetCursorPos( &p );

ScreenToClient( ReportPlot.handle(), &p );

lParam = MAKELPARAM( (WORD) p.x, (WORD) p.y );

do_lbuttondown( lParam );
}

/***********************************************************************
*                          REPORT_PLOT_PROC                            *
***********************************************************************/
LRESULT CALLBACK report_plot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

static int id;
static int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        ReportPlot = hWnd;
        return 0;

    case WM_DESTROY:
        return 0;


    case WM_MOUSEMOVE:
        do_mousemove( lParam );
        return 0;

    case WM_LBUTTONDOWN:
        do_lbuttondown( lParam );
        return 0;

    case WM_RBUTTONDOWN:
        do_popup_menu( lParam );
        return 0;

    case WM_SIZE:
        resize_report_plot();
        break;

    case WM_PAINT:
        paint_me();
        return 0;

    case WM_COMMAND:
        switch ( id )
            {
            case ZOOM_POPUP:
                do_zoom_popup();
                return 0;

            case FONT_POPUP:
                if ( choose_font(FontHandle, TitleFontHandle) )
                    InvalidateRect( hWnd, 0, TRUE );
                return 0;

            case PRINTER_FONT_POPUP:
                choose_font( PrinterFontHandle, PrinterTitleFontHandle );
                return 0;
            }
        break;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                          CREATE_REPORT_PLOT                          *
***********************************************************************/
static void create_report_plot( HWND parent )
{
RECTANGLE_CLASS r;
WNDCLASS        wc;
WINDOW_CLASS    w;
int             h;

w = GetDlgItem( parent, SHIFTS_RADIO );
w.getrect( r );
h = r.height();
w = parent;
w.get_client_rect( r );
r.top = h;
h = r.height();

wc.lpszClassName = ReportPlotClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) report_plot_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0; //         TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

w = CreateWindow(
    ReportPlotClassName,
    TEXT("ReportPlot"),
    WS_CHILD,
    0, r.top,                         // X,y
    r.width(), h,                     // W,h
    parent,
    NULL,
    MainInstance,
    NULL
    );

w.show( SW_SHOW );
UpdateWindow( w.handle() );
}

/***********************************************************************
*                   POSITION_SELECT_MACHINES_DIALOG                    *
***********************************************************************/
static void position_select_machines_dialog( HWND select_machines_handle )
{
WINDOW_CLASS w;
RECTANGLE_CLASS pr;
RECTANGLE_CLASS r;

w = MainDialog.control( SELECT_MACHINES_PB );
w.getrect( pr );

w = select_machines_handle;
w.getrect( r );

r.moveto( pr.left, pr.bottom );
w.move( r );
}

/***********************************************************************
*                          LOAD_REPORT_LIST                            *
***********************************************************************/
void load_report_list( HWND listbox_handle )
{
TCHAR         * cp;
STRING_CLASS    s;
TEXT_LIST_CLASS t;
LISTBOX_CLASS   lb;

lb.init( listbox_handle );

s = ConfigFilePrefix;
s += StarDotStar;

t.get_file_list( exe_directory(), s.text() );
t.rewind();
while ( t.next() )
    {
    cp = t.text();
    if ( lstrlen(cp) > ConfigFilePrefixLen )
        {
        cp += ConfigFilePrefixLen;
        s = cp;
        cp = findchar( PeriodChar, s.text() );
        if ( cp )
            {
            *cp = NullChar;
            lb.add( s.text() );
            }

        }
    }
}

/***********************************************************************
*                       POSITION_SAVE_AS_DIALOG                        *
***********************************************************************/
static void position_save_as_dialog( HWND save_as_dialog_handle )
{
WINDOW_CLASS w;
RECTANGLE_CLASS pr;
RECTANGLE_CLASS r;

w = MainDialog.control(SAVE_CONFIG_PB);
w.getrect( pr );

w = save_as_dialog_handle;
w.getrect( r );

r.moveto( pr.left, pr.bottom );
w.move( r );
}

/***********************************************************************
*                              PUT_CONFIG                              *
***********************************************************************/
static void put_config( TCHAR * name )
{
STRING_CLASS s;
FILE_CLASS   f;
FONT_CLASS   lf;
int          n;
TIME_CLASS   now;
RECTANGLE_CLASS r;

s = exe_directory();

if ( CsvFileName.isempty() )
    {
    CsvFileName = s;
    CsvFileName.cat_path( DefaultCsvFileName );
    }

s += ConfigFilePrefix;
if ( is_empty(name) )
    s += CurrentConfig;
else
    s += name;
s += ConfigFileSuffix;

n = SelectedMachines.count();
f.open_for_write( s.text() );
f.writeline( int32toasc(n) );
if ( n > 0 )
    {
    SelectedMachines.rewind();
    while ( SelectedMachines.next() )
        f.writeline( SelectedMachines.text() );
    }

n = 0;
if ( FontHandle != INVALID_FONT_HANDLE )
    n++;
if ( PrinterFontHandle != INVALID_FONT_HANDLE )
    n++;
f.writeline( int32toasc(n) );

if ( FontHandle != INVALID_FONT_HANDLE )
    {
    FontList.get_font( lf, FontHandle );
    f.writeline( lf.put() );
    }

if ( PrinterFontHandle != INVALID_FONT_HANDLE )
    {
    FontList.get_font( lf, PrinterFontHandle );
    f.writeline( lf.put() );
    }

s = ShowLegendPrefix;

if ( LegendIsVisible )
    {
    s += YString;
    }
else
    {
    s += NString;
    }
f.writeline( s.text() );

MainWindow.getrect( r );
f.writeline( r.put() );

for ( n=0; n<NOF_RANGE_SETTINGS; n++ )
    {
    s = StartString[n];
    s += StartRange[n];
    f.writeline( s.text() );
    }

for ( n=0; n<NOF_RANGE_SETTINGS; n++ )
    {
    s = EndString[n];
    s += EndRange[n];
    f.writeline( s.text() );
    }

f.writeline( PlotTitle.text() );
f.writeline( CsvFileName.text() );
f.close();
}

/***********************************************************************
*                          ADJUST_TIME_RANGE                           *
*   sorc is either StartRange or EndRange. t is StartTime or EndTime   *
* This routine starts at the current time and adjusts is by the        *
* settings in the sorc strings for delta months, weeks, days, shifts   *
***********************************************************************/
static void adjust_time_range( TIME_CLASS & t, STRING_CLASS * sorc )
{
unsigned long dt;
bool          is_negative;
int           n;
WORD          current_unit;
SYSTEMTIME    st;
short         new_shift;
short         old_shift;

t.get_local_time();

if ( !sorc[MONTHS_INDEX].isempty() )
    {
    n = sorc[MONTHS_INDEX].int_value();
    is_negative = false;
    if ( n < 0 )
        {
        is_negative = true;
        n = -n;
        }

    dt = SECONDS_PER_DAY;
    current_unit = StartTime.system_time().wMonth;
    while ( n > 0 )
        {
        if ( is_negative )
            t -= dt;
        else
            t += dt;

        st = t.system_time();
        if ( current_unit != st.wMonth )
            {
            current_unit = st.wMonth;
            n--;
            }
        }

    if ( (sorc == StartRange ) )
        {
        st = t.system_time();
        st.wDay          = 1;
        st.wHour         = 0;
        st.wMinute       = 0;
        st.wSecond       = 0;
        st.wMilliseconds = 0;
        }
    else
        {
        /*
        --------------------
        Go to the next month
        -------------------- */
        while ( current_unit == st.wMonth )
            {
            t += dt;
            st = t.system_time();
            }
        /*
        --------------------
        Then back up one day
        -------------------- */
        t -= dt;
        st = t.system_time();
        st.wHour         = 23;
        st.wMinute       = 59;
        st.wSecond       = 30;
        st.wMilliseconds = 0;
        }

    t.set( st );
    }

if ( !sorc[WEEKS_INDEX].isempty() )
    {
    n = sorc[WEEKS_INDEX].int_value();
    is_negative = false;
    if ( n < 0 )
        {
        is_negative = true;
        n = -n;
        }

    dt = SECONDS_PER_DAY;
    current_unit = StartTime.system_time().wDayOfWeek;
    while ( n > 0 )
        {
        if ( is_negative )
            {
            t -= dt;
            if ( current_unit == 0 )
                {
                current_unit = 6;
                n--;
                }
            else
                {
                current_unit--;
                }
            }
        else
            {
            t += dt;
            if ( current_unit == 6 )
                {
                current_unit = 0;
                n--;
                }
            else
                {
                current_unit++;
                }
            }
        }

    if ( sorc == StartRange )
        {
        while ( current_unit > 0 )
            {
            t -= dt;
            current_unit--;
            }
        st = t.system_time();
        st.wHour         = 0;
        st.wMinute       = 0;
        st.wSecond       = 0;
        st.wMilliseconds = 0;
        }
    else
        {
        while ( current_unit < 6 )
            {
            t += dt;
            current_unit++;
            }
        st = t.system_time();
        st.wHour         = 23;
        st.wMinute       = 59;
        st.wSecond       = 30;
        st.wMilliseconds = 0;
        }

    t.set( st );
    }

if ( !sorc[DAYS_INDEX].isempty() )
    {
    dt = SECONDS_PER_DAY;
    n = sorc[DAYS_INDEX].int_value();
    is_negative = false;
    if ( n < 0 )
        {
        is_negative = true;
        n = -n;
        }

    while ( n > 0 )
        {
        if ( is_negative )
            t -= dt;
        else
            t += dt;
        n--;
        }

    if ( sorc == StartRange )
        {
        st = t.system_time();
        st.wHour         = 0;
        st.wMinute       = 0;
        st.wSecond       = 0;
        st.wMilliseconds = 0;
        }
    else
        {
        st.wHour         = 23;
        st.wMinute       = 59;
        st.wSecond       = 30;
        st.wMilliseconds = 0;
        }

    t.set( st );
    }

/*
------------------------
Always adjust the shifts
------------------------ */
dt = SECONDS_PER_HOUR;
st = t.system_time();
old_shift = shift_number( 0, st  );
n = 0;

if ( !sorc[SHIFTS_INDEX].isempty() )
    {
    n = sorc[SHIFTS_INDEX].int_value();
    is_negative = false;
    if ( n < 0 )
        {
        is_negative = true;
        n = -n;
        }
    }

while ( n > 0 )
    {
    if ( is_negative )
        t -= dt;
    else
        t += dt;

    st = t.system_time();
    new_shift = shift_number( 0, st  );

    if ( new_shift != old_shift )
        {
        n--;
        old_shift = new_shift;
        }
    }

if ( sorc == StartRange )
    {
    t.set( start_of_shift(st) );
    }
else
    {
    while ( true )
        {
        t += dt;
        st = t.system_time();
        new_shift = shift_number( 0, st  );
        if ( new_shift != old_shift )
            {
            t.set( start_of_shift(st) );
            break;
            }
        }
    }
}

/***********************************************************************
*                              GET_CONFIG                              *
***********************************************************************/
static void get_config( TCHAR * name )
{
TCHAR       * cp;
STRING_CLASS  s;
FILE_CLASS    f;
FONT_CLASS    lf;
int           i;
int           n;
TIME_CLASS    now;

SelectedMachines.empty();
s = exe_directory();

s += ConfigFilePrefix;
if ( is_empty(name) )
    CurrentConfig = DefaultConfig;
else
    CurrentConfig = name;

s += CurrentConfig;
s += ConfigFileSuffix;

if ( file_exists(s.text()) )
    {
    if ( f.open_for_read(s.text()) )
        {
        n = asctoint32( f.readline() );
        while ( n )
            {
            SelectedMachines.append( f.readline() );
            n--;
            }

        n = asctoint32( f.readline() );
        if ( n > 0 )
            {
            cp = f.readline();
            if ( !is_empty(cp) )
                {
                lf.get( cp );
                FontHandle = FontList.get_handle( lf );
                if ( lf.lfWeight != FW_BOLD )
                    {
                    lf.lfWeight = FW_BOLD;
                    TitleFontHandle = FontList.get_handle( lf );
                    }
                }
            }

        if ( n > 1 )
            {
            cp = f.readline();
            if ( !is_empty(cp) )
                {
                lf.get( cp );
                PrinterFontHandle = FontList.get_handle( lf );
                if ( lf.lfWeight != FW_BOLD )
                    {
                    lf.lfWeight = FW_BOLD;
                    PrinterTitleFontHandle = FontList.get_handle( lf );
                    }
                }
            }

        cp = f.readline();
        if ( !is_empty(cp) )
            {
            if ( findstring(ShowLegendPrefix, cp) )
                {
                cp = findchar( EqualsChar, cp );
                if ( cp )
                    {
                    cp++;
                    if ( *cp == YString[0] )
                        {
                        if ( MainDialog.handle() != 0 )
                            set_checkbox( MainDialog.control(SHOW_LEGEND_XBOX), TRUE );
                        LegendIsVisible = TRUE;
                        }
                    else
                        {
                        if ( MainDialog.handle() != 0 )
                            set_checkbox( MainDialog.control(SHOW_LEGEND_XBOX), FALSE );
                        LegendIsVisible = FALSE;
                        }
                    }
                }
            }

        /*
        ------------------------
        Get the main window size
        ------------------------ */
        cp = f.readline();
        if ( !is_empty(cp) )
            MainRect.get( cp );


        /*
        ---------------------------
        Read the time range settins
        --------------------------- */
        for ( i=0; i<NOF_RANGE_SETTINGS; i++ )
            {
            cp = f.readline();
            if (!cp )
                break;
            n = lstrlen( StartString[i] );
            if ( strings_are_equal(StartString[i], cp, n) )
                {
                cp += n;
                StartRange[i] = cp;
                }
            }


        for ( i=0; i<NOF_RANGE_SETTINGS; i++ )
            {
            cp = f.readline();
            if ( !cp )
                break;
            n = lstrlen( EndString[i] );
            if ( strings_are_equal(EndString[i], cp, n) )
                {
                cp += n;
                EndRange[i] = cp;
                }
            }

        PlotTitle = f.readline();
        CsvFileName = f.readline();
        f.close();
        }
    }

adjust_time_range( StartTime, StartRange );
adjust_time_range( EndTime, EndRange );
}

/***********************************************************************
*                       UPDATE_SELECTED_MACHINES                       *
***********************************************************************/
static void update_selected_machines( HWND w )
{
LISTBOX_CLASS           lb;
int   i;
int   n;
INT * x;

lb.init( w, MACHINE_LB );

SelectedMachines.empty();
n = lb.get_select_list( &x );
if ( n > 0 )
    {
    for ( i=0; i<n; i++ )
        SelectedMachines.append( lb.item_text(x[i]) );
    delete[] x;
    }
}

/***********************************************************************
*                              LOAD_MACHINES                           *
***********************************************************************/
static void load_machines( HWND w )
{
COMPUTER_CLASS          c;
LISTBOX_CLASS           lb;
int i;

Machine.empty();
lb.init( w, MACHINE_LB );

/*
----------------------------
Make a list of the computers
---------------------------- */
c.rewind();
while ( c.next() )
    {
    if ( c.is_present() )
        Machine.add_computer( c.name() );
    }

Machine.rewind();
while ( Machine.next() )
    {
    i = lb.add( Machine.name() );
    if ( SelectedMachines.find( Machine.name() ) )
        lb.setcursel( i );
    }
}

/***********************************************************************
*                     SELECT_MACHINES_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK select_machines_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static LISTBOX_CLASS lb;
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        position_select_machines_dialog( hWnd );
        load_machines( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case ALL_MACHINES_PB:
                lb.init( hWnd, MACHINE_LB );
                lb.select_all();
                return TRUE;

            case NO_MACHINES_PB:
                lb.init( hWnd, MACHINE_LB );
                lb.unselect_all();
                return TRUE;

            case IDOK:
                update_selected_machines( hWnd );
                MainDialog.refresh();
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           SAVE_AS_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK save_as_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static HWND MyComboBox = 0;
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        if ( SavingReport )
            {
            set_text( hWnd, resource_string(SAVE_REPORT_STRING) );
            set_text( hWnd, IDOK,  resource_string(SAVE_STRING) );
            }
        else
            {
            set_text( hWnd, resource_string(LOAD_REPORT_STRING) );
            set_text( hWnd, IDOK,  resource_string(LOAD_STRING) );
            }

        position_save_as_dialog( hWnd );
        MyComboBox = GetDlgItem( hWnd, SAVE_AS_CBOX );
        load_report_list( MyComboBox );
        set_text( MyComboBox, CurrentConfig.text() );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                CurrentConfig.get_text( MyComboBox );
                if ( CurrentConfig.isempty() )
                    CurrentConfig = DefaultConfig;
                if ( SavingReport )
                    {
                    put_config( CurrentConfig.text() );
                    }
                else
                    {
                    get_config( CurrentConfig.text() );
                    MainWindow.move( MainRect );
                    }
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                       POSITION_WRITE_CSV_DIALOG                      *
***********************************************************************/
static void position_write_csv_dialog( HWND write_csv_handle )
{
WINDOW_CLASS w;
RECTANGLE_CLASS pr;
RECTANGLE_CLASS r;

w = MainDialog.control( WRITE_CSV_PB );
w.getrect( pr );

w = write_csv_handle;
w.getrect( r );

r.moveto( pr.left-(r.width()/2), pr.bottom );
w.move( r );
}

/***********************************************************************
*                         WRITE_CSV_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK write_csv_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        position_write_csv_dialog( hWnd );
        set_text( hWnd, CSV_FILE_NAME_EBOX, CsvFileName.text() );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                CsvFileName.get_text( hWnd, CSV_FILE_NAME_EBOX );
                write_csv_file();
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        GET_RANGES_FROM_EBOXES                        *
***********************************************************************/
static void get_ranges_from_eboxes( HWND w )
{
int id;
int i;

id = START_MONTHS_EBOX;

for ( i=0; i<NOF_RANGE_SETTINGS; i++ )
    {
    StartRange[i].get_text( w, id );
    id++;
    }


id = END_MONTHS_EBOX;
for ( i=0; i<NOF_RANGE_SETTINGS; i++ )
    {
    EndRange[i].get_text( w, id );
    id++;
    }
}

/***********************************************************************
*                        SET_EBOXES_FROM_RANGES                        *
***********************************************************************/
static void set_eboxes_from_ranges( HWND w )
{
int id;
int i;

id = START_MONTHS_EBOX;

for ( i=0; i<NOF_RANGE_SETTINGS; i++ )
    {
    StartRange[i].set_text( w, id );
    id++;
    }


id = END_MONTHS_EBOX;
for ( i=0; i<NOF_RANGE_SETTINGS; i++ )
    {
    EndRange[i].set_text( w, id );
    id++;
    }
}

/***********************************************************************
*                  POSITION_REPORT_TIME_RANGE_DIALOG                   *
***********************************************************************/
static void position_report_time_range_dialog( HWND hWnd )
{
WINDOW_CLASS w;
RECTANGLE_CLASS pr;
RECTANGLE_CLASS r;

w = MainDialog.control( EDIT_RANGE_PB );
w.getrect( pr );

w = hWnd;
w.getrect( r );

r.moveto( pr.right-r.width(), pr.bottom );
w.move( r );
}

/***********************************************************************
*                           REPORT_TIME_RANGE_DIALOG_PROC              *
***********************************************************************/
BOOL CALLBACK report_time_range_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        position_report_time_range_dialog( hWnd );
        PlotTitle.set_text( hWnd, PLOT_TITLE_EBOX );
        set_eboxes_from_ranges( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                PlotTitle.get_text( hWnd, PLOT_TITLE_EBOX );
                get_ranges_from_eboxes( hWnd );
                adjust_time_range( StartTime, StartRange );
                adjust_time_range( EndTime, EndRange );
                put_config( CurrentConfig.text() );
                ReportPlot.refresh();
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          RESIZE_MAIN_DIALOG                          *
***********************************************************************/
static void resize_main_dialog()
{
RECTANGLE_CLASS r;

MainWindow.getrect( MainRect );
MainWindow.get_client_rect( r );
MainDialog.move( r );
resize_report_plot();
}

/***********************************************************************
*                             MAIN_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MainDialog = hWnd;
        CheckRadioButton( MainDialog.handle(), SHIFTS_RADIO, ALL_DATES_RADIO, SHIFTS_RADIO );
        load_machines( hWnd );
        create_report_plot( hWnd );
        if ( LegendIsVisible )
            set_checkbox( MainDialog.control(SHOW_LEGEND_XBOX), TRUE );
        if ( PrintOnly || CsvOnly )
            MainDialog.post( WM_DBINIT );
        return TRUE;

    case WM_DBINIT:
        if ( PrintOnly || CsvOnly )
            {
            if ( PrintOnly )
                print_me( false );

            if ( CsvOnly )
                write_csv_file();

            MainWindow.close();
            }
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case EDIT_RANGE_PB:
                DialogBox( MainInstance, TEXT("REPORT_TIME_RANGE_DIALOG"), hWnd, report_time_range_dialog_proc );
                return TRUE;

            case SELECT_MACHINES_PB:
                DialogBox( MainInstance, TEXT("SELECT_MACHINES_DIALOG"), hWnd, select_machines_dialog_proc );
                return TRUE;

            case WRITE_CSV_PB:
                DialogBox( MainInstance, TEXT("WRITE_CSV_DIALOG"), hWnd, write_csv_dialog_proc );
                return TRUE;

            case SHOW_LEGEND_XBOX:
                LegendIsVisible = is_checked( MainDialog.control(SHOW_LEGEND_XBOX)  );
                MainDialog.refresh();
                return TRUE;

            case SHIFTS_RADIO:
                DateIntervalType = SHIFTS_TYPE;
                return TRUE;

            case DAYS_RADIO:
                DateIntervalType = DAYS_TYPE;
                return TRUE;

            case WEEKS_RADIO:
                DateIntervalType = WEEKS_TYPE;
                return TRUE;

            case MONTHS_RADIO:
                DateIntervalType = MONTHS_TYPE;
                return TRUE;

            case ALL_DATES_RADIO:
                DateIntervalType = ALL_DATES_TYPE;
                get_all_limits();
                return TRUE;

            case RELOAD_PB:
                reload();
                get_config( CurrentConfig.text()  );
                MainDialog.refresh();
                return TRUE;

            case LOAD_CONFIG_PB:
            case SAVE_CONFIG_PB:
                if ( id == LOAD_CONFIG_PB )
                    SavingReport = false;
                else
                    SavingReport = true;
                DialogBox( MainInstance, TEXT("SAVE_AS_DIALOG"), hWnd, save_as_dialog_proc );
                MainDialog.refresh();
                return TRUE;

            case PRINT_PB:
                print_me( true );
                return TRUE;

            case PREV_PB:
                previous_interval();
                return TRUE;

            case NEXT_PB:
                next_interval();
                return TRUE;

            case IDCANCEL:
                if ( Zooming )
                    {
                    cancel_zoom();
                    return TRUE;
                    }
            case IDOK:
                MainWindow.close();
                return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                               GET_HELP                               *
***********************************************************************/
void get_help()
{
static char s[] = "name=shiftrpt.swf";
const int SLEN = sizeof( s );

TCHAR   path[MAX_PATH+1];
char    cpath[MAX_PATH+1];
FILE_CLASS f;

if ( get_exe_directory(path) )
    {
    lstrcat( path, TEXT("ft2help.txt") );
    f.open_for_write( path );
    f.writebinary( s, SLEN );
    f.close();

    get_exe_directory(path);
    lstrcat( path, TEXT("ft2help.exe") );
    unicode_to_char( cpath, path );
    WinExec( cpath, SW_SHOWNORMAL );
    }
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
    case WM_CREATE:
        MainWindow = hWnd;
        break;

    case WM_SIZE:
        resize_main_dialog();
        break;

    case WM_HELP:
        get_help();
        return 0;

    case WM_SETFOCUS:
        MainDialog.set_focus();
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case EXIT_MENU_CHOICE:
                MainWindow.close();
                return 0;
            }

        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
COMPUTER_CLASS c;
STRING_CLASS title;
WNDCLASS wc;
TCHAR    * cp;
int        width;
int        height;

names_startup();
c.startup();
shifts_startup();

reload();

cp = findstring( TEXT("-p"), CommandLine.text() );
if ( cp )
    {
    PrintOnly = true;
    cmd_show = SW_SHOWMINIMIZED;
    }


cp = findstring( TEXT("-csv"), CommandLine.text() );
if ( cp )
    {
    CsvOnly = true;
    cmd_show = SW_SHOWMINIMIZED;
    }

title = DefaultConfig;
cp = findstring( TEXT("-f"), CommandLine.text() );
if ( cp )
    {
    cp += 2;
    if ( *cp != NullChar )
        {
        title = cp;
        cp = findchar( SpaceChar, title.text() );
        if ( cp )
            *cp = NullChar;
        }
    }

get_config( title.text() );

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0; //         TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

width  = MainRect.width();
height = MainRect.height();

if ( width <= 10 || height <= 0 )
    MainRect.set( 0, 0, 750, 550 );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    MainRect.left, MainRect.top,      // X,y
    width, height,                      // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT( "MAIN_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) main_dialog_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
shifts_shutdown();

if ( FontHandle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( FontHandle );
    FontHandle = INVALID_FONT_HANDLE;
    }

if ( TitleFontHandle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( TitleFontHandle );
    TitleFontHandle = INVALID_FONT_HANDLE;
    }

if ( PrinterFontHandle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( PrinterFontHandle );
    PrinterFontHandle = INVALID_FONT_HANDLE;
    }

if ( PrinterTitleFontHandle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( PrinterTitleFontHandle );
    PrinterTitleFontHandle = INVALID_FONT_HANDLE;
    }
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

InitCommonControls();

MainInstance = this_instance;

CommandLine = GetCommandLine();

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialog.handle() )
        status = IsDialogMessage( MainDialog.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}