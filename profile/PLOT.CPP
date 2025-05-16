#include <windows.h>
#include <cmath>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\ftanalog.h"
#include "..\include\names.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\part_analog_sensor.h"
#include "..\include\stringcl.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

#include "extern.h"

const int TIME_ONLY_DISPLAY = 3;    /* Defined in profile.cpp */
extern int CurrentDisplay;          /* Defined in profile.cpp */

static PART_ANALOG_SENSOR AnalogSensor; /* Used only by the ftii routines */

static const float MaxFloatShort = float( 0xFFFF );
void pressure_display_check();
double * calculate_ftii_parameters( FTII_PROFILE & f, MACHINE_CLASS & machine, PART_CLASS & part, PARAMETER_CLASS & param, PART_ANALOG_SENSOR & asensor, FTANALOG_CLASS & ftanalog );
float running_average( float * raw, int32 true_index, int32 final_index );

/***********************************************************************
*                              DAC_VOLTS                               *
***********************************************************************/
static double dac_volts( unsigned x )
{
double d;

d = double( x );
d /= 4095.0;
d *= 20.0;
d -= 10.0;
return round( d, 0.001 );
}

/***********************************************************************
*                           MAKE_DATE_STRING                           *
***********************************************************************/
static void make_date_string( void )
{
SYSTEMTIME st;

FileTimeToSystemTime( &Ph.time_of_shot, &st );

getday( DateString, st.wDayOfWeek );

copychars( DateString+DATESTRING_DATE_OFFSET, alphadate(st), ALPHADATE_LEN );
copychars( DateString+DATESTRING_TIME_OFFSET, alphatime(st), ALPHATIME_LEN );
}

/***********************************************************************
*                       FILL_DIFFERENTIAL_CURVE                        *
***********************************************************************/
BOOLEAN fill_differential_curve( MACHINE_CLASS machine, PART_CLASS & part, PROFILE_HEADER & ph, float points[][MAX_FT2_POINTS] )
{
int32 i;
int32 n;
float ch;
float cr;
float area;
float * hp;
float * rp;
float * dp;

ph.curves[DIFFERENTIAL_CURVE_INDEX].type = NO_CURVE_TYPE;

if ( part.head_pressure_channel == NO_FT_CHANNEL || part.rod_pressure_channel == NO_FT_CHANNEL )
    return FALSE;

/*
--------------------------------------------
Do nothing if there is no differential curve
-------------------------------------------- */
if ( machine.diff_curve_number == NO_DIFF_CURVE_NUMBER )
    return FALSE;

dp = points[DIFFERENTIAL_CURVE_INDEX];

/*
----------------------------------------------------
Point hp at the beginning of the head pressure curve
---------------------------------------------------- */
i  = CHAN_1_ANALOG_CURVE_INDEX + part.head_pressure_channel - 1;
hp = points[i];

/*
---------------------------------------------------
Point Rp at the beginning of the rod pressure curve
--------------------------------------------------- */
i  = CHAN_1_ANALOG_CURVE_INDEX + part.rod_pressure_channel - 1;
rp = points[i];

/*
----------------------------------------------
Copy the units to the differential curve
(I assume they are the same for rod and head).
---------------------------------------------- */
ph.curves[DIFFERENTIAL_CURVE_INDEX].units = Ph.curves[i].units;

/*
-------------------------
Fill the array with zeros
------------------------- */
n = ph.n;
while ( n )
    {
    *dp++ = 0.0;
    n--;
    }

ph.curves[DIFFERENTIAL_CURVE_INDEX].type  = DIFF_PRESSURE_CURVE;

/*
-----------------------------------------------
Get the coefficients for the differential curve
----------------------------------------------- */
get_dcurve_coefficients( ch, cr, machine.diff_curve_number );

/*
-------------------------------------------
Divide the coefficients by the plunger area
------------------------------------------- */
area = float( part.plunger_area() );
ch /= area;
cr /= area;

dp = points[DIFFERENTIAL_CURVE_INDEX];

/*
--------------------------------------
Set the min and max to the first point
-------------------------------------- */
*dp = (*hp) * ch + (*rp) * cr;
Ph.curves[DIFFERENTIAL_CURVE_INDEX].max = *dp;
Ph.curves[DIFFERENTIAL_CURVE_INDEX].min = *dp;

n = ph.n;
while ( n )
    {
    *dp = (*hp) * ch + (*rp) * cr;

    maxfloat( Ph.curves[DIFFERENTIAL_CURVE_INDEX].max, *dp );
    minfloat( Ph.curves[DIFFERENTIAL_CURVE_INDEX].min, *dp );

    dp++;
    hp++;
    rp++;
    n--;
    }

return TRUE;
}

/***********************************************************************
*                              FTII_VOLTS                              *
***********************************************************************/
static double ftii_volts( unsigned int count )
{
double x;

x = double( count );
x *= 5.0;
x /= 4095.0;

return x;
}

/***********************************************************************
*                           SMOOTH_ONE_CURVE                           *
***********************************************************************/
static void smooth_one_curve( float * yp, int n )
{
static float * buf = 0;
int32 last_index;
int32 i;

if ( !buf )
    buf = new float[MAX_FT2_POINTS];

last_index = n-1;

for ( i=0; i<n; i++ )
    buf[i] = running_average( yp+i, i, last_index );

for ( i=0; i<n; i++ )
    yp[i] = buf[i];
}

/***********************************************************************
*                            READ_FTII_PROFILE                         *
***********************************************************************/
static BOOL read_ftii_profile( TCHAR * fname )
{
static BOOLEAN firstime = TRUE;
static BITSETYPE ft_analog_curve_type[NOF_FTII_ANALOGS] =
    {
    CHAN_1_ANALOG_CURVE, CHAN_2_ANALOG_CURVE, CHAN_3_ANALOG_CURVE, CHAN_4_ANALOG_CURVE,
    CHAN_5_ANALOG_CURVE, CHAN_6_ANALOG_CURVE, CHAN_7_ANALOG_CURVE, CHAN_8_ANALOG_CURVE
    };

STRING_CLASS s;
FTII_PROFILE fp;
float      * yp;
double     * dp;
double       x;
double       real_timer_frequency;
unsigned     * ap;
unsigned     umax;
int          i;
int          imax;
int          imin;
int          curve_index;
int          ftii_curve_index;
int          analog_index;
int          analog_sensor_index;
BITSETYPE    my_type;
BITSETYPE    current_plot_types;
bool         is_dac;

/*
-----------------------------------------------------
Get the current types of curves plotted on the screen
----------------------------------------------------- */
current_plot_types = PreImpactPlotCurveTypes;
current_plot_types |= PostImpactPlotCurveTypes;

/*
----------------------------------------------------
Load the analog sensors if I haven't done so already
---------------------------------------------------- */
if ( firstime )
    {
    AnalogSensor.get();
    firstime = FALSE;
    }

s = fname;
s.replace( TEXT(".pro"), TEXT(".pr2") );

if ( !fp.get(s) )
    return FALSE;

FileTimeToLocalFileTime( &fp.shot_time, &Ph.time_of_shot );

Ph.n                    = (short) fp.n;
Ph.last_pos_based_point = (short) fp.last_position_based_point;

NofPoints = Ph.n;

/*
--------------
Position Curve
-------------- */
yp = Points[POSITION_CURVE_INDEX];
for (i=0; i<fp.n; i++ )
    {
    x = CurrentPart.dist_from_x4( (double) fp.position[i] );
    yp[i] = (float) x;
    }
i = fp.curve_index( FTII_POSITION_TYPE );
Ph.curves[POSITION_CURVE_INDEX].type  = POSITION_CURVE;
Ph.curves[POSITION_CURVE_INDEX].units = CurrentPart.distance_units;
x = (double) fp.curvedata[i].imax;
x = CurrentPart.dist_from_x4( x );
Ph.curves[POSITION_CURVE_INDEX].max   = float( x );
Ph.curves[POSITION_CURVE_INDEX].min   = 0.0;
PosCurve = POSITION_CURVE_INDEX;

/*
----------
Time Curve
---------- */
real_timer_frequency = (double) fp.timer_frequency;
umax = fp.timer[i];
yp = Points[TIME_CURVE_INDEX];
for (i=0; i<fp.n; i++ )
    {
    maxunsigned( umax, fp.timer[i] );
    x = (double) fp.timer[i];
    x /= real_timer_frequency;
    yp[i] = (float) x;
    }

i = fp.curve_index( FTII_TIMER_COUNT_TYPE );
Ph.curves[TIME_CURVE_INDEX].type      = TIME_CURVE;
Ph.curves[TIME_CURVE_INDEX].units     = SECOND_UNITS;
x = umax;
x /= real_timer_frequency;
Ph.curves[TIME_CURVE_INDEX].max       = float( x );
Ph.curves[TIME_CURVE_INDEX].min       = 0.0;
TimeCurve = TIME_CURVE_INDEX;

/*
--------------
Velocity Curve
-------------- */
imax = fp.velocity[0];
imin = imax;

yp = Points[VELOCITY_CURVE_INDEX];
for ( i=0; i<fp.n; i++ )
    {
    maxint( imax, fp.velocity[i] );
    minint( imin, fp.velocity[i] );
    x = (double) fp.velocity[i];
    x = CurrentPart.velocity_from_x4( x );
    yp[i] = (float) x;
    }

if ( Smoothing && SmoothingVelocity )
    smooth_one_curve( yp, fp.n );

Ph.curves[VELOCITY_CURVE_INDEX].type  = VELOCITY_CURVE;
Ph.curves[VELOCITY_CURVE_INDEX].units = CurrentPart.velocity_units;

x = (double) imax;
x = CurrentPart.velocity_from_x4( x );
Ph.curves[VELOCITY_CURVE_INDEX].max = float( x );

x = (double) imin;
x = CurrentPart.velocity_from_x4( x );
Ph.curves[VELOCITY_CURVE_INDEX].min = float( x );

/*
-----------------------------------------------------------------------------
                               Analog Channels
The analog_index is the channel number - 1
I only have room for 4 curves so I load the four that are currently displayed
----------------------------------------------------------------------------- */
analog_index     = 0;
curve_index      = CHAN_1_ANALOG_CURVE_INDEX;
ftii_curve_index = fp.curve_index( FTII_ANALOG1_TYPE );

while ( curve_index <= CHAN_4_ANALOG_CURVE_INDEX )
    {
    if ( analog_index >= fp.nof_analogs )
        break;

    /*
    -------------------------------------------------
    Convert the ftii curve type to an ft channel type
    ------------------------------------------------- */
    my_type = ft_analog_curve_type[analog_index];

    i = analog_index + 1;
    if ( i == CurrentPart.head_pressure_channel )
        my_type |= HEAD_PRESSURE_CURVE;
    else if ( i == CurrentPart.rod_pressure_channel )
        my_type |= ROD_PRESSURE_CURVE;

    if ( my_type & current_plot_types )
        {
        Ph.curves[curve_index].type = my_type;

        /*
        --------------------------------------
        The actual type may be channels 17-20.
        -------------------------------------- */
        if ( fp.curvedata[ftii_curve_index].curve_type >= FTII_ANALOG17_TYPE )
            is_dac = true;
        else
            is_dac = false;

        if ( analog_index < MAX_FT_CHANNELS )
            analog_sensor_index = CurrentPart.analog_sensor[analog_index];
        else
            analog_sensor_index = DEFAULT_ANALOG_SENSOR;

        if ( is_dac )
            {
            Ph.curves[curve_index].units = VOLTS_UNITS;
            Ph.curves[curve_index].max   = dac_volts( fp.curvedata[ftii_curve_index].umax );
            Ph.curves[curve_index].min   = dac_volts( fp.curvedata[ftii_curve_index].umin );
            }
        else
            {
            Ph.curves[curve_index].units = AnalogSensor[analog_sensor_index].units_index();
            Ph.curves[curve_index].max   = AnalogSensor[analog_sensor_index].converted_value( fp.curvedata[ftii_curve_index].umax );
            Ph.curves[curve_index].min   = AnalogSensor[analog_sensor_index].converted_value( fp.curvedata[ftii_curve_index].umin );
            }

        yp = Points[curve_index];
        ap = fp.analog[analog_index];
        for (i=0; i<fp.n; i++ )
            {
            if ( is_dac )
                yp[i] = (float) dac_volts( ap[i] );
            else
                yp[i] = (float) AnalogSensor[analog_sensor_index].converted_value( ap[i] );
            }

        if ( analog_index >=0 && analog_index < MAX_FT_CHANNELS )
            {
            if ( Smoothing && SmoothingChannel[analog_index] )
                smooth_one_curve( yp, fp.n );
            }
        curve_index++;
        }

    ftii_curve_index++;
    analog_index++;
    }

while ( curve_index < MAX_CURVES )
    {
    Ph.curves[curve_index].type = NO_CURVE_TYPE;
    curve_index++;
    }

/*
----------
Parameters
---------- */
NofParms = CurrentParam.count();
dp = calculate_ftii_parameters( fp, CurrentMachine, CurrentPart, CurrentParam, AnalogSensor, CurrentFtAnalog );
if ( dp )
    {
    for ( i=0; i<MAX_PARMS; i++ )
        Parms[i] = dp[i];
    delete[] dp;
    dp = 0;
    }
else
    {
    for ( i=0; i<MAX_PARMS; i++ )
        Parms[i] = 0.0;
    }

return TRUE;
}

/***********************************************************************
*                             LOAD_PROFILE                             *
***********************************************************************/
void load_profile( TCHAR * fname )
{
static int MySemaphor = 0;
static BITSETYPE MyAnalogCurveType[8] = {
    CHAN_1_ANALOG_CURVE, CHAN_2_ANALOG_CURVE, CHAN_3_ANALOG_CURVE,
    CHAN_4_ANALOG_CURVE, CHAN_5_ANALOG_CURVE, CHAN_6_ANALOG_CURVE,
    CHAN_7_ANALOG_CURVE, CHAN_8_ANALOG_CURVE
    };

HANDLE  fh;
DWORD   access_mode;
DWORD   share_mode;
DWORD   bytes_read;
DWORD   bytes_to_read;
BOOL    status;
int     ai;
int     i;
int     p;
TCHAR * cp;
short   nmax;
unsigned short * raw;
unsigned short * rp;
float            b;
float          * yp;
float            ymin;

if ( MySemaphor )
    return;

MySemaphor++;

TimeCurve = 0;
PosCurve  = 0;

raw         = 0;
status      = FALSE;
access_mode = GENERIC_READ;
share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;

/*
---------------------------------------------------------------
If this is a ftii file the last character of the file name is 2
--------------------------------------------------------------- */
cp = fname + lstrlen(fname) - 1;
if ( *cp == TEXT('2')  )
    fh = INVALID_HANDLE_VALUE;
else
    fh = CreateFile( fname, access_mode, share_mode, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );

if ( fh != INVALID_HANDLE_VALUE )
    {
    status = ReadFile( fh, &Ph, sizeof(Ph), &bytes_read, 0 );
    if ( status )
        status = ReadFile( fh, &NofParms, sizeof(short), &bytes_read, 0 );

    if ( status && NofParms > 0 )
        status = ReadFile( fh, Parms, NofParms*sizeof(float), &bytes_read, 0 );

    if ( status )
        {
        status = FALSE;
        nmax = 0;
        raw = 0;
        if ( Ph.n > nmax )
            nmax = Ph.n;

        if ( nmax )
            {
            raw = new unsigned short[nmax];
            if ( raw )
                status = TRUE;
            }
        }

    if ( status )
        {
        if ( Ph.n > 0 )
            {
            bytes_to_read = Ph.n * sizeof( unsigned short );
            for ( i=0; i<MAX_CURVES; i++ )
                {
                if ( Ph.curves[i].type == NO_CURVE_TYPE )
                    break;
                else if ( Ph.curves[i].type & TIME_CURVE )
                    TimeCurve = i;
                else if ( Ph.curves[i].type & POSITION_CURVE )
                    PosCurve = i;

                status = ReadFile( fh, raw, bytes_to_read, &bytes_read, 0 );
                if ( !status )
                    break;

                ymin = Ph.curves[i].min;
                b    = Ph.curves[i].max - ymin;
                b    /= MaxFloatShort;

                rp = raw;
                yp = Points[i];
                p  = Ph.n;
                while ( p-- )
                    {
                    *yp = float( *rp );
                    *yp *= b;
                    *yp += ymin;
                    yp++;
                    rp++;
                    }

                if ( Smoothing )
                    {
                    if ( (Ph.curves[i].type & VELOCITY_CURVE) && SmoothingVelocity )
                        smooth_one_curve( Points[i], Ph.n );
                    else
                        {
                        for ( ai=0; ai<8; ai++ )
                            {
                            if ( (Ph.curves[i].type & MyAnalogCurveType[ai]) && SmoothingChannel[ai] )
                                {
                                smooth_one_curve( Points[i], Ph.n );
                                break;
                                }
                            }
                        }
                    }
                }
            NofPoints = Ph.n;
            }
        delete[] raw;
        }
    CloseHandle( fh );
    }
else
    {
    status = read_ftii_profile( fname );
    }

if ( status )
    {
    HaveHeader = TRUE;
    fill_differential_curve( CurrentMachine, CurrentPart, Ph, Points );
    make_date_string();
    pressure_display_check();
    }

MySemaphor--;
}

/***********************************************************************
*                             LOAD_PROFILE                             *
***********************************************************************/
void load_profile( void )
{
STRING_CLASS s;
s = profile_name( ComputerName, MachineName, PartName, ShotName );
load_profile( s.text() );
}

/***********************************************************************
*                             LOAD_PROFILE                             *
***********************************************************************/
void load_profile( PROFILE_NAME_ENTRY * p )
{
STRING_CLASS s;
s = profile_name(p->computer, p->machine, p->part, p->shot );
load_profile( s.text() );
}

/***********************************************************************
*                           CAN_SHOW_MASTER                            *
* This checks the file date to see if I can display this. I do it      *
* this way so I don't load the file and change the menu settings.      *
* If the file is too old and this is not time based mode, I do         *
* nothing.                                                             *
***********************************************************************/
static bool can_show_master( TCHAR * fname )
{
HANDLE  fh;
bool    is_ok;
TCHAR * cp;

const DWORD access_mode = GENERIC_READ;
const DWORD share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
DWORD   bytes_read;
BOOL    status;
PROFILE_HEADER myph;

if ( !file_exists(fname) )
    return false;

if ( CurrentDisplay == TIME_ONLY_DISPLAY )
    return true;

status = FALSE;
is_ok  = false;

fh = CreateFile( fname, access_mode, share_mode, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh != INVALID_HANDLE_VALUE )
    {
    /*
    -------------------------------------------------------------------------
    If the last character of the file name is a '2' then this is an ftii file
    ------------------------------------------------------------------------- */
    cp = fname + lstrlen(fname) - 1;
    if ( *cp == TEXT('2') )
        {
        if ( GetFileTime(fh, 0, 0, &myph.time_of_shot) )
            status = TRUE;
        }
    else
        {
        status = ReadFile( fh, &myph, sizeof(myph), &bytes_read, 0 );
        }

    if ( status )
        {
        if ( CompareFileTime(&myph.time_of_shot, &DateICanShowPressure) >= 0 )
            is_ok = true;
        }

    CloseHandle( fh );
    }

return is_ok;
}

/***********************************************************************
*                      GET_REFERENCE_TRACE_NAME                        *
* Bupres backed up current.ma2 as current.mas for a long time. If it   *
* is an ma2 file the first two bytes are int 2 or 3 whereas if it is   *
* really an mas the first byte (or int if unicode) is a character      *
* >= '0'. Pass me the name of the ma2 file.                            *
***********************************************************************/
BOOLEAN get_reference_trace_name( STRING_CLASS & dest )
{
STRING_CLASS sorc;
FILE_CLASS f;
int        i;
TCHAR    * cp;

if ( file_exists(dest.text()) )
    return TRUE;

sorc = dest;
cp = sorc.text();
cp += sorc.len();
cp--;
*cp = TEXT( 'S' );

if ( file_exists(sorc.text()) )
    {
    if ( f.open_for_read(sorc.text())  )
        {
        f.readbinary( &i, sizeof(i) );
        f.close();

        if ( i==2 || i==3 )
            MoveFile( sorc.text(), dest.text() );
        else
            dest = sorc;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                         LOAD_REFERENCE_TRACE                         *
***********************************************************************/
BOOLEAN load_reference_trace()
{
STRING_CLASS fname;

fname = master_ii_name( ComputerName, MachineName, PartName, CURRENT_MASTER_TRACE_NAME );

if ( !get_reference_trace_name(fname) )
    return FALSE;

if ( !can_show_master(fname.text()) )
    return FALSE;

load_profile( fname.text() );
return TRUE;
}
