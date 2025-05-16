#include <windows.h>
#include <ddeml.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\genlist.h"
#include "..\include\events.h"
#include "..\include\ft.h"
#include "..\include\structs.h"
#include "..\include\machine.h"
#include "..\include\part.h"
#include "..\include\param.h"
#include "..\include\ftanalog.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\alarmsum.h"
#include "..\include\verrors.h"

#include "boards.h"
#include "extern.h"
#include "shotsave.h"

#define ABOVE_TARGET      1
#define BELOW_TARGET      2
#define TARGET_TRACK_INIT 0

static const BOARD_DATA BAD_VELOCITY = 0xFFFF;

static short NofPoints;
static short LastPositionBasedPoint;
static short IndexOfP1 = 0;
static short IndexOfP2 = 0;

static BOOLEAN ClearAlarmsEachShot = FALSE;
static float IParam[MAX_INTERNAL_PARMS];
static float Parameter[MAX_PARMS];

PROFILE_HEADER Ph;
BOARD_DATA     Raw[MAX_POINTS];
float          Points[MAX_CURVES][MAX_POINTS];

short DistanceUnits;
short VelocityUnits;

struct MONITOR_ENTRY
     {
     MACHINE_CLASS       machine;
     PART_CLASS          part;
     PARAMETER_CLASS     param;
     FTANALOG_CLASS      ftanalog;
     };

GENERIC_LIST_CLASS MachineList;
MONITOR_ENTRY    * CurrentMonitor;

static HARDWARE_BOARD_ENTRY * Be = 0;

/*
--------------------------------
Wire bitsets for use with alarms
-------------------------------- */
BITSETYPE ClearBits;
BITSETYPE SetBits;

/*
------------------------------------------------------------------------
This will be set by check_alarms if any values exceed their alarm limits
------------------------------------------------------------------------ */
static BOOLEAN HasAlarm = FALSE;

/*
------------------------
Position Based Constants
------------------------ */
static float CountsPerSec   = 3000000.0;
static float X1PosDivisor   = 1.0;
static float X4PosDivisor   = 1.0;
static float VelocityFactor = 1.0;

/*
--------------------
Time Based Constants
-------------------- */
static float MsPerSample      = 1.0;

static TCHAR AChar = TEXT( 'A' );
static TCHAR PChar = TEXT( 'P' );
static TCHAR YChar = TEXT( 'Y' );

static TCHAR ConfigSection[]      = TEXT( "Config" );
static TCHAR MonallIniFile[]      = TEXT( "monall.ini" );
static TCHAR ClearAlarmsEachShotKey[] = TEXT( "ClearAlarmsEachShot" );
static TCHAR VisiTrakIniFile[] = TEXT( "visitrak.ini" );
static TCHAR WorkSection[]     = TEXT( "Work" );
static TCHAR WorkStartKey[]    = TEXT( "WorkStart" );
static TCHAR WorkEndKey[]      = TEXT( "WorkEnd" );

BOOLEAN append_downtime_record( TCHAR * computer, TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat );
void issue_downtime_event( TCHAR * computer, TCHAR * machine, TCHAR * part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat );

/***********************************************************************
*                              DUPS_TO_VU                              *
*                                                                      *
*          Convert distance units per second to velocity units         *
***********************************************************************/
float dups_to_vu()
{
return (float) dups_to_vu( VelocityUnits, DistanceUnits );
}

/***********************************************************************
*                          INDEX_FROM_TIME                             *
*                      Post-impact times only (MS).                    *
***********************************************************************/
inline short index_from_time( short ms )
{
short i;
i = LastPositionBasedPoint + ms / short(MsPerSample);
if ( i >= NofPoints )
    i = NofPoints - 1;
return i;
}

/***********************************************************************
*                          INDEX_FROM_TIME                             *
*                      Post-impact times only (MS).                    *
***********************************************************************/
inline short index_from_time( float ms )
{
short i;
i = LastPositionBasedPoint + short( ms / MsPerSample );
if ( i >= NofPoints )
    i = NofPoints - 1;
return i;
}

/***********************************************************************
*                          INDEX_FROM_POSITION                         *
*                      Pre-impact positions only.                      *
***********************************************************************/
inline short index_from_position( float position )
{
short x;

position *= X1PosDivisor;

x = short( position );

if ( (position - float(x)) > (float(x+1) - position) )
    x++;

maxshort( x, 0 );
minshort( x, LastPositionBasedPoint );

return x;
}

/***********************************************************************
*                      VELOCITY_BETWEEN_POSITIONS                      *
*                                                                      *
*                      Pre-impact positions only.                      *
***********************************************************************/
float velocity_between_positions( short i1, short i2 )
{

float v;
float dt;

if ( i1 == i2 )
    return Points[VELOCITY_CURVE_INDEX][i1];

v  = Points[POSITION_CURVE_INDEX][i2] - Points[POSITION_CURVE_INDEX][i1];
dt = Points[TIME_CURVE_INDEX][i2]     - Points[TIME_CURVE_INDEX][i1];
if ( not_float_zero(dt) )
    v /= dt;

v *= dups_to_vu();

return v;
}

/***********************************************************************
*                      VELOCITY_BETWEEN_POSITIONS                      *
*                                                                      *
*                      Pre-impact positions only;                      *
***********************************************************************/
float velocity_between_positions( float pos1, float pos2 )
{

short i1;
short i2;

i1 = index_from_position( pos1 );
i2 = index_from_position( pos2 );

return velocity_between_positions( i1, i2 );
}

/***********************************************************************
*                        VELOCITY_AT_POSITION                          *
*                                                                      *
*                      Pre-impact positions only;                      *
***********************************************************************/
float velocity_at_position( float pos )
{
short i;

i = index_from_position( pos );

return Points[VELOCITY_CURVE_INDEX][i];
}

/***********************************************************************
*                         VELOCITY_FROM_COUNT                          *
*                            Position Based                            *
***********************************************************************/
inline float velocity_from_count( BOARD_DATA count )
{
if ( count == 0 )
    count = 1;

return VelocityFactor / float(count);
}

/***********************************************************************
*                      SECONDS_BETWEEN_POSITIONS                       *
***********************************************************************/
float seconds_between_positions( short i1, short i2 )
{
if ( i1 == i2 )
    return Points[TIME_CURVE_INDEX][i1];

return Points[TIME_CURVE_INDEX][i2] - Points[TIME_CURVE_INDEX][i1];
}

/***********************************************************************
*                      SECONDS_BETWEEN_POSITIONS                       *
***********************************************************************/
float seconds_between_positions( float pos1, float pos2 )
{
short i1;
short i2;

i1 = index_from_position( pos1 );
i2 = index_from_position( pos2 );

return seconds_between_positions( i1, i2 );
}

/***********************************************************************
*                           TIME_FROM_COUNTS                           *
*                            Position Based                            *
***********************************************************************/
inline float time_from_counts( long counts )
{
return float(counts) / CountsPerSec;
}


/***********************************************************************
*                            POS_FROM_INDEX                            *
*                            Position Based                            *
***********************************************************************/
inline float pos_from_index( short i )
{
return float(i) / X1PosDivisor;
}

/***********************************************************************
*                            POS_FROM_COUNT                            *
*                              Time Based                              *
***********************************************************************/
inline float pos_from_count( short i )
{
return float(i) / X4PosDivisor;
}

/***********************************************************************
*                    FIRST_DIFFERENTIAL_CURVE_POINT                    *
* The first differential curve point is normally the first point       *
* after impact but if pre-impact work is being calculated then         *
* it is 0;                                                             *
***********************************************************************/
short first_differential_curve_point( void )
{

short p;
short work_parameter;
MONITOR_ENTRY * m;

work_parameter = PRE_IMPACT_WORK + 1;
m = CurrentMonitor;

if ( m )
    {
    if ( m->machine.diff_curve_number >= LOW_METAL_PRESSURE_DIFF_CURVE )
        return 0;

    for ( p=0; p<MAX_PARMS; p++ )
        {
        if ( m->param.parameter[p].input.type == INTERNAL_PARAMETER )
            {
            if ( m->param.parameter[p].input.number == work_parameter )
                return 0;
            }
        }
    }

return LastPositionBasedPoint+1;
}

/***********************************************************************
*                             PLUNGER_AREA                             *
***********************************************************************/
float plunger_area( void )
{
float d2;

d2 = CurrentMonitor->part.plunger_diameter;
if ( not_float_zero(d2) )
    {
    d2 *= d2;
    d2 *= .78540;
    }
else
    d2 = 1.0;

return d2;
}


/***********************************************************************
*                       FILL_DIFFERENTIAL_CURVE                        *
***********************************************************************/
static void fill_differential_curve( void )
{
short i;
float ch;
float cr;
float area;
float * hp;
float * rp;
float * dp;

Ph.curves[DIFFERENTIAL_CURVE_INDEX].type  = NO_CURVE_TYPE;
dp = Points[DIFFERENTIAL_CURVE_INDEX];

/*
----------------------------------------------------
Point hp at the beginning of the head pressure curve
---------------------------------------------------- */
hp = 0;
i = CurrentMonitor->part.head_pressure_channel;
if ( i != NO_FT_CHANNEL )
    hp = Points[CHAN_1_ANALOG_CURVE_INDEX+i-1];

/*
---------------------------------------------------
Point Rp at the beginning of the rod pressure curve
--------------------------------------------------- */
rp = 0;
i = CurrentMonitor->part.rod_pressure_channel;
if ( i != NO_FT_CHANNEL )
    {
    /*
    ------------------------------------------
    Make i the index of the rod pressure curve
    ------------------------------------------ */
    i--;
    i += CHAN_1_ANALOG_CURVE_INDEX;

    rp = Points[i];

    /*
    ----------------------------------------------
    Copy the units to the differential curve
    (I assume they are the same for rod and head).
    ---------------------------------------------- */
    Ph.curves[DIFFERENTIAL_CURVE_INDEX].units = Ph.curves[i].units;
    }

/*
-------------------------
Fill the array with zeros
------------------------- */
for ( i=0; i<NofPoints; i++ )
    *dp++ = 0.0;

/*
--------------------------------------------
Do nothing if there is no differential curve
-------------------------------------------- */
if ( !hp || !rp || CurrentMonitor->machine.diff_curve_number == NO_DIFF_CURVE_NUMBER )
    return;

Ph.curves[DIFFERENTIAL_CURVE_INDEX].type  = DIFF_PRESSURE_CURVE;

/*
-----------------------------------------------
Get the coefficients for the differential curve
----------------------------------------------- */
get_dcurve_coefficients( ch, cr, CurrentMonitor->machine.diff_curve_number );

/*
-------------------------------------------
Divide the coefficients by the plunger area
------------------------------------------- */
area = plunger_area();

ch /= area;
cr /= area;

/*
------------------------------------------------------------------
The differential curve is only defined after impact. Start at that
point and calculate the differential curve values;
i  = LastPositionBasedPoint+1;
The above has now been changed. I define the differential curve
to start at 0 if work is being calculated.
------------------------------------------------------------------ */
i = first_differential_curve_point();
dp = Points[DIFFERENTIAL_CURVE_INDEX];
dp += i;
hp += i;
rp += i;

/*
--------------------------------------
Set the min and max to the first point
-------------------------------------- */
Ph.curves[DIFFERENTIAL_CURVE_INDEX].max = *dp;
Ph.curves[DIFFERENTIAL_CURVE_INDEX].min = *dp;

while ( i<NofPoints )
    {
    *dp = (*hp) * ch + (*rp) * cr;

    maxfloat( Ph.curves[DIFFERENTIAL_CURVE_INDEX].max, *dp );
    minfloat( Ph.curves[DIFFERENTIAL_CURVE_INDEX].min, *dp );

    dp++;
    hp++;
    rp++;
    i++;
    }
}

/***********************************************************************
*                             READ_COUNTS                              *
***********************************************************************/
BOOLEAN read_counts( MUX_ENTRY * me )
{

short i;
short j;
short n;
short channel;
short npre;
short npost;
short pi;
short si;
BITSETYPE curve_type;

int   status;
long  count;

BOARD_DATA xmin;
BOARD_DATA xmax;
BOARD_DATA x;
BOARD_DATA * p;

BOARD_DATA last_position;
short      index_of_last_position;
float      last_velocity;

float * vp;
float * tp;
float * pp;
float velocity_multiplier;
float dt;
float dv;

/*
---------------------------------------
Get the number of position based points
--------------------------------------- */
status= lrd_eos_pos( &x );
if ( status != FT_OK )
    {
    error_message(  READ_COUNTS, FT_BOARD_ERROR, status );
    return FALSE;
    }

if ( x > BOARD_DATA(MAX_POSITION_SAMPLES) )
    x = BOARD_DATA(MAX_POSITION_SAMPLES);

npre = x;

/*
-----------------------------------
Get the number of time based points
----------------------------------- */
status = rd_delay( &x );
if ( status != FT_OK )
    {
    error_message(  READ_COUNTS, FT_BOARD_ERROR, status );
    return FALSE;
    }

if ( x > BOARD_DATA(MAX_TIME_SAMPLES) )
    x = BOARD_DATA(MAX_TIME_SAMPLES);

npost = x;

/*
-----------------------------------------------------------
Add one to the total so I can add a zero point at the start
----------------------------------------------------------- */
NofPoints               = npre + npost + 1;
LastPositionBasedPoint  = npre;
Ph.last_pos_based_point = npre;
Ph.n                    = NofPoints;

/*
-----------------------------------
Read the position based time counts
----------------------------------- */
status = lfill_vel_array( Raw, &xmin, &xmax, npre );
if ( status != FT_OK )
    {
    error_message(  READ_COUNTS, FT_BOARD_ERROR, status );
    return FALSE;
    }

/*
-----------------------------------
Read the time based position counts
----------------------------------- */
status = lfill_pos_array( Raw+npre, &xmin, &xmax, npost );
if ( status != FT_OK )
    {
    error_message(  READ_COUNTS, FT_BOARD_ERROR, status );
    return FALSE;
    }

count = 0L;
p  = Raw;
vp = Points[VELOCITY_CURVE_INDEX];
tp = Points[TIME_CURVE_INDEX];
pp = Points[POSITION_CURVE_INDEX];

/*
-------------------------
Initialize the zero point
------------------------- */
*vp = 0.0;
*tp = 0.0;
*pp = 0.0;

/*
-------------------------------------------------------------------------
The velocity multiplier is for time based data. It includes the
units conversion as well as multiplying by 1000 for ms to sec conversion.
------------------------------------------------------------------------- */
velocity_multiplier = dups_to_vu();
velocity_multiplier *= float(MS_PER_SECOND);
velocity_multiplier /= MsPerSample;

dt = MsPerSample / float(MS_PER_SECOND);

/*
----------------------
Convert the count data
---------------------- */
Ph.curves[VELOCITY_CURVE_INDEX].type  = VELOCITY_CURVE;
Ph.curves[VELOCITY_CURVE_INDEX].units = me->vel_units;

Ph.curves[TIME_CURVE_INDEX].type      = TIME_CURVE;
Ph.curves[TIME_CURVE_INDEX].units     = SECOND_UNITS;

Ph.curves[POSITION_CURVE_INDEX].type  = POSITION_CURVE;
Ph.curves[POSITION_CURVE_INDEX].units = me->dist_units;

last_position = npre * 4;
index_of_last_position = npre;

for ( i=1; i<NofPoints; i++ )
    {
    vp++;
    tp++;
    pp++;
    if ( i <= npre )
        {
        count += long( *p );
        *vp = velocity_from_count( *p );
        *tp = time_from_counts( count );
        *pp = pos_from_index( i );
        }
    else
        {
        *pp = pos_from_count( *p );
        if ( *p != last_position && index_of_last_position < i )
            {
            n = i - index_of_last_position;
            *vp = pos_from_count( *p - last_position );
            *vp /= float(n) * dt;
            *vp *= dups_to_vu();

            dv = *vp;
            vp -= n;
            last_velocity = *vp;
            vp++;

            dv -= last_velocity;

            for ( j=1; j<n; j++ )
                {
                *vp = last_velocity + float(j) * dv / float(n);
                vp++;
                }

            last_position = *p;
            index_of_last_position = i;
            }
        else
            {
            *vp = 0;
            }
        *tp = *(tp-1) + dt;
        }

    if ( i== 1 )
        {
        Ph.curves[VELOCITY_CURVE_INDEX].max  = *vp;
        Ph.curves[VELOCITY_CURVE_INDEX].min  = 0.0;

        Ph.curves[TIME_CURVE_INDEX].max      = *tp;
        Ph.curves[TIME_CURVE_INDEX].min      = 0.0;

        Ph.curves[POSITION_CURVE_INDEX].max  = *pp;
        Ph.curves[POSITION_CURVE_INDEX].min  = 0.0;
        }
    else
        {
        maxfloat( Ph.curves[VELOCITY_CURVE_INDEX].max, *vp );
        minfloat( Ph.curves[VELOCITY_CURVE_INDEX].min, *vp );

        maxfloat( Ph.curves[TIME_CURVE_INDEX].max,     *tp );
        minfloat( Ph.curves[TIME_CURVE_INDEX].min,     *tp );

        maxfloat( Ph.curves[POSITION_CURVE_INDEX].max, *pp );
        minfloat( Ph.curves[POSITION_CURVE_INDEX].min, *pp );
        }

    p++;
    }

/*
-----------------------------------
Read the continuous analog channels
----------------------------------- */
pi = CHAN_1_ANALOG_CURVE_INDEX;
curve_type = CHAN_1_ANALOG_CURVE;
for ( channel=1; channel<=MAX_CONTINUOUS_CHANNELS; channel++ )
    {
    Ph.curves[pi].type = curve_type;

    if ( CurrentMonitor->part.head_pressure_channel == channel )
        Ph.curves[pi].type |= HEAD_PRESSURE_CURVE;

    if ( CurrentMonitor->part.rod_pressure_channel == channel )
        Ph.curves[pi].type |= ROD_PRESSURE_CURVE;

    /*
    -------------------------------------------
    Each consecutive curve type is the next bit
    ------------------------------------------- */
    curve_type <<= 1;

    status = lfill_analog_vs_pos_array( Raw, &xmin, &xmax, channel, npre );
    if ( status != FT_OK )
        {
        error_message(  READ_COUNTS, FT_BOARD_ERROR, status );
        return FALSE;
        }

    status = lfill_analog_vs_time_array( Raw+npre, &xmin, &xmax, channel, npost );
    if ( status != FT_OK )
        {
        error_message(  READ_COUNTS, FT_BOARD_ERROR, status );
        return FALSE;
        }

    si = channel - 1;
    Ph.curves[pi].units = me->asensor[si]->units;
    p  = Raw;
    pp = Points[CHAN_1_ANALOG_CURVE_INDEX+si];
    for ( i=1; i<NofPoints; i++ )
        {
        pp++;
        *pp = convert_analog( *p, me->asensor[si]);
        if ( i == 1 )
            {
            Ph.curves[pi].max = *pp;
            Ph.curves[pi].min = *pp;
            }
        else
            {
            maxfloat( Ph.curves[pi].max, *pp );
            minfloat( Ph.curves[pi].min, *pp );
            }

        p++;
        }

    /*
    --------------------
    Copy the first point
    -------------------- */
    pp  = Points[CHAN_1_ANALOG_CURVE_INDEX+si];
    *pp = *(pp+1);
    pi++;
    }

fill_differential_curve();

return TRUE;
}

/***********************************************************************
*                     FIND_EXISTING_MONITOR_ENTRY                      *
***********************************************************************/
static MONITOR_ENTRY * find_existing_monitor_entry( TCHAR * machine_to_find )
{
MONITOR_ENTRY * m;

MachineList.rewind();
while ( TRUE )
    {
    m = (MONITOR_ENTRY *) MachineList.next();
    if ( !m )
        break;

    if ( compare(m->machine.name, machine_to_find, MACHINE_NAME_LEN) == 0 )
        break;
    }

return m;
}

/***********************************************************************
*                           LOAD_MONITOR_ENTRY                         *
***********************************************************************/
static BOOLEAN load_monitor_entry( MONITOR_ENTRY * m, TCHAR * machine_to_find, TCHAR * part_to_find )
{

if ( !m->machine.find(ComputerName, machine_to_find) )
    return FALSE;

if ( !m->part.find(ComputerName, machine_to_find, part_to_find) )
    return FALSE;

if ( !m->param.find(ComputerName, machine_to_find, part_to_find) )
    return FALSE;

if ( !m->ftanalog.load(ComputerName, machine_to_find, part_to_find) )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                         FIND_CURRENT_MONITOR                         *
***********************************************************************/
BOOLEAN find_current_monitor( TCHAR * machine_to_find, TCHAR * part_to_find )
{

MONITOR_ENTRY * m;

m = find_existing_monitor_entry( machine_to_find );
if ( m )
    {
    if ( lstrcmp(m->part.name, part_to_find) != 0 )
        load_monitor_entry(m, machine_to_find, part_to_find );
    CurrentMonitor = m;
    return TRUE;
    }

m = new MONITOR_ENTRY;
if ( !m )
    {
    error_message( FIND_CURRENT_MACHINE, NO_MEM_ALLOC_ERROR );
    return FALSE;
    }

if ( load_monitor_entry(m, machine_to_find, part_to_find) )
    {
    MachineList.append( m );
    CurrentMonitor = m;
    return TRUE;
    }

delete m;
return FALSE;
}

/***********************************************************************
*                  GET_INTENSIFICATION_RESPONSE_TIME                   *
*                                (MS)                                  *
***********************************************************************/
void get_intensification_response_time( float target_pressure, short curve_index )
{
short   i;
short   last_point_side;
float   difference;
float   min_difference;
float * p;

i = LastPositionBasedPoint + 1;
p = Points[curve_index]+LastPositionBasedPoint + 1;
last_point_side = TARGET_TRACK_INIT;
min_difference  = 1000.0; /* Seconds */

while ( i < NofPoints )
    {

    if ( *p > target_pressure )
        {
        if ( last_point_side == BELOW_TARGET )
            break;
        difference = *p - target_pressure;
        last_point_side = ABOVE_TARGET;
        }
    else
        {
        if ( last_point_side == ABOVE_TARGET )
            break;
        difference = target_pressure - *p;
        last_point_side = BELOW_TARGET;
        }

    if ( difference < min_difference )
        {
        min_difference = difference;
        if ( is_float_zero(min_difference) )
            break;
        }

    i++;
    p++;
    }

IParam[INTENS_RESPONSE_TIME] = 0.0;

if ( i < NofPoints )
    IParam[INTENS_RESPONSE_TIME] = (i - LastPositionBasedPoint) * MsPerSample;
}

/***********************************************************************
*                            READ_PARAMETER                            *
***********************************************************************/
static float read_parameter( BOARD_DATA ft_param_cmd )
{
int status;
BOARD_DATA x;
float      y;

y = 0.0;

status = ft_read_parameter( ft_param_cmd, &x );
if ( status == FT_OK )
    y = float( x );
else
    error_message( READ_PARAMETER_SUB, FT_BOARD_ERROR, status );

return y;
}

/***********************************************************************
*                           PRE_IMPACT_WORK                            *
*                                                                      *
*             I assume the P3 point has already been set.              *
***********************************************************************/
static float pre_impact_work( void )
{
int     first_point;
int     last_point;
int     i;
float * dp;
float   sum;
float   x;
TCHAR * cp;

/*
----------------------------------------------
Start at the beginning if no workstart setting
---------------------------------------------- */
x = 0.0;
cp = get_ini_string( VisiTrakIniFile, WorkSection, WorkStartKey );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    {
    x = IParam[P3_POS_PARM];
    x -= extfloat( cp, lstrlen(cp) );
    }

first_point = index_from_position( x );
if ( first_point >= LastPositionBasedPoint )
    first_point = LastPositionBasedPoint - 1;

x = IParam[P3_POS_PARM];
cp = get_ini_string( VisiTrakIniFile, WorkSection, WorkEndKey );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    x -= extfloat( cp, lstrlen(cp) );
last_point = index_from_position( x );
if ( last_point < first_point )
    last_point = first_point + 1;

i   = first_point;
dp  = Points[DIFFERENTIAL_CURVE_INDEX] + first_point;

/*
-------------------------------------------------------------------
P0 + P1   P1 + P2         Pn-1 + Pn    P0                        Pn
------- + ------- + ... + ---------  = -- + P1 + P2 + ... Pn-1 + --
   2         2                2        2                          2
------------------------------------------------------------------- */
sum = *dp/2.0;
dp++;
while ( i<last_point )
    {
    sum += *dp;
    dp++;
    i++;
    }

sum += *dp/2.0;

/*
----------------------------------------------------------------
There are X1PosDivisor points per unit measure so divide by this
---------------------------------------------------------------- */
sum /= X1PosDivisor;

/*
---------------------------------------------------------------------
These are pressures. To get the force I need to multiply by the area.
--------------------------------------------------------------------- */
sum *= plunger_area();

return sum;
}

/***********************************************************************
*                          TRUE_CSFS_POSITION                          *
***********************************************************************/
static double true_csfs_position( int csfs_index )
{
int    i1;
int    i2;

double v;
double v1;
double v2;
double p;
double p1;
double p2;

i2 = csfs_index;
i1 = i2 - 1;
if ( i1 < 0 )
    i1 = 0;

v  = (double) CurrentMonitor->part.csfs_min_velocity;

p1 = Points[POSITION_CURVE_INDEX][i1];
v1 = (double) Points[VELOCITY_CURVE_INDEX][i1];

v2 = (double) Points[VELOCITY_CURVE_INDEX][i2];
p2 = Points[POSITION_CURVE_INDEX][i2];

p  = p2;

if ( v>=v1 && v<=v2 && p2>p1 && v2!=v1 )
    p = p1 + (p2 - p1) * (v-v1)/(v2-v1);

return p;
}

/***********************************************************************
*                          TRUE_EOS_POSITION                           *
***********************************************************************/
static double true_eos_position( void )
{
int    i1;
int    i2;
double v;
double v1;
double v2;
double p;
double p1;
double p2;

i2 = LastPositionBasedPoint;
i1 = i2 - 1;

v  = (double) CurrentMonitor->part.eos_velocity;

p1 = Points[POSITION_CURVE_INDEX][i1];
v1 = (double) Points[VELOCITY_CURVE_INDEX][i1];

v2 = (double) Points[VELOCITY_CURVE_INDEX][i2];
p2 = Points[POSITION_CURVE_INDEX][i2];

p  = p2;

if ( v1>=v && v>=v2 && p2>p1 && v2!=v1 )
    p = p1 + (p2 - p1) * (v1-v)/(v1-v2);

return p;
}

/***********************************************************************
*                       READ_INTERNAL_PARAMETERS                       *
***********************************************************************/
static void read_internal_parameters( void )
{

int       status;
short     channel;
short     csfs_index;
short     curve_index;
short     i;
short     n;
BITSETYPE curve_type;
float   * pp;
float   * xp;
float     max_pos;
float     max_psi;
BOARD_DATA x;
MONITOR_ENTRY * m;

m = CurrentMonitor;

/*
----------------------------------------------------------------------
Position P3 (IN,MM)  Cavity Full Position.
The plunger position when the die cavity and overflows are full,
coincides with the position at which the End of Shot Velocity is
measured.
---------------------------------------------------------------------- */
//IParam[P3_POS_PARM] = Points[POSITION_CURVE_INDEX][LastPositionBasedPoint];

IParam[P3_POS_PARM] = (float) true_eos_position();

/*
-----------------------------------------------------------------------
Position P2 (IN,MM) Metal At Gate Position
(also known as the Runner Full position). The plunger position when the
liquid metal front is at the gate. Calculated by measuring back from P3
the distance from P2 to P3 entered in Part Setup.
----------------------------------------------------------------------- */
IParam[P2_POS_PARM] = IParam[P3_POS_PARM] - m->part.runner_fill_distance;
IndexOfP2 = index_from_position( IParam[P2_POS_PARM] );

/*
----------------------------------------------------------------------
Position P1 (IN,MM) Shot Sleeve Full Position.
This is the plunger position where the volume of metal fills the shot
sleeve. Calculated by measuring back from P3 the distance from P1 to
P3 entered in Part Setup.
---------------------------------------------------------------------- */
IParam[P1_POS_PARM] = IParam[P3_POS_PARM] - m->part.sleeve_fill_distance;
IndexOfP1 = index_from_position( IParam[P1_POS_PARM] );

/*
----------------------------------------------------------------------
Calculated Start Of Fast Shot (CSFS)
The position when the user set Minimum Velocity for CSFS has been
equalled or exceeded and the user set Minimum Position for CSFS been
exceeded. Set in Part Setup. It's position can be marked on the trace.
---------------------------------------------------------------------- */
csfs_index = 0;
status = ft_read_parameter( POS_CSFS, &x );
if ( status == FT_OK )
    {
    csfs_index = short(x);
    //IParam[CSFS] = Points[POSITION_CURVE_INDEX][csfs_index];
    IParam[CSFS] = (float) true_csfs_position( (int) csfs_index );
    }

/*
----------------------------------------------------------------------
Fast Shot Distance. (IN,MM)
The distance between Calculated Start of Fast Shot position to the End
of Shot Velocity position.
---------------------------------------------------------------------- */
IParam[FAST_SHOT_DISTANCE] = IParam[P3_POS_PARM] - IParam[CSFS];

/*
----------------------------------------------------------------------
Fast Shot Time(MS)
The time from the Calculated Start of Fast Shot (CSFS) position to the
End of Shot Velocity (EOSV) position.
---------------------------------------------------------------------- */
IParam[FAST_SHOT_TIME] = seconds_between_positions( IParam[CSFS], IParam[P3_POS_PARM] );
IParam[FAST_SHOT_TIME] *= float( MS_PER_SECOND );

/*
----------------------------------------------------------------------
Slow Shot Time (MS)
The time from the start of shot until the position for Calculated Start
Of Fast Shot (CSFS) is detected.
---------------------------------------------------------------------- */
IParam[SLOW_SHOT_TIME] = Points[TIME_CURVE_INDEX][csfs_index];
IParam[SLOW_SHOT_TIME] *= float( MS_PER_SECOND );

/*
----------------------------------------------------------------------
Total Shot Time (MS)
The time from the start of the shot until the End Of shot Velocity
(EOSV) is detected.
---------------------------------------------------------------------- */
IParam[TOTAL_SHOT_TIME] = Points[TIME_CURVE_INDEX][LastPositionBasedPoint];
IParam[TOTAL_SHOT_TIME] *= float( MS_PER_SECOND );

/*
----------------------------------------------------------------------
Filltime (MS)
The time taken for the metal to fill the diecavity and the overflow
wells. Calculated by measuring the time taken for the plunger to move
from the metal at gate position (P2) to the cavity full position (P3).
---------------------------------------------------------------------- */
IParam[FILL_TIME] = seconds_between_positions( IParam[P2_POS_PARM], IParam[P3_POS_PARM] );
IParam[FILL_TIME] *= float( MS_PER_SECOND );

/*
---------------------------------------------------------------------
Fast Shot Rise Time (MS)
The time interval between the Calculated Start of Fast Shot (CSFS) to
the time when the Peak Fast Shot Velocity is reached. This is read
directly from the FasTrak board.
--------------------------------------------------------------------- */
IParam[RISE_TIME] = read_parameter(RT_MILLI) + read_parameter(RT_MICRO) / 1000.0;

/*
---------------------------------------------------------
Cycle Time (SEC)
The time interval between subsequent cycle start signals.
--------------------------------------------------------- */
rd_cycle_tim( IParam+CYCLE_TIME_PARM );

/*
-----------------------------------------------------------------------
User Time Interval One to Five (SEC)
Time interval measured between an I/O device switching on/off. The
outputs are received from a programmable controller, for example a pump
switching in vacuum diecasting. This function is not possible with
2020S, 2020AT or multiplexed systems.
----------------------------------------------------------------------- */
for ( i=0; i<USER_TIME_INT_COUNT; i++ )
    rd_time_interval( IParam+USER_TIME_INT_ONE+i, i );

/*
------------------------------------------------------------------
Biscuit Size (MM)
The measured difference between the final plunger position and the
Total Stroke Length.
------------------------------------------------------------------ */
rd_bisc_size( IParam+BISCUIT_SIZE );

/*
-------------------------------------------------------------------
Intens Pressure (PSI,BAR)
The intensified pressure recorded when the user set Time for Intens
Pressure has elapsed.
------------------------------------------------------------------- */
curve_type  = m->machine.impact_pres_type[POST_IMPACT_INDEX];
if ( curve_type & DIFF_PRESSURE_CURVE )
    {
    curve_index = DIFFERENTIAL_CURVE_INDEX;
    }
else
    {
    if ( curve_type & ROD_PRESSURE_CURVE )
        channel = m->part.rod_pressure_channel;
    else if ( curve_type & HEAD_PRESSURE_CURVE )
        channel = m->part.head_pressure_channel;
    else
        channel = 0;

    if ( channel > 0 )
        channel--;

    curve_index = CHAN_1_ANALOG_CURVE_INDEX + channel;
    }

i = short( m->part.time_for_intens_pres/ int32(MsPerSample) );
i += LastPositionBasedPoint;
minshort( i, NofPoints-1 );

IParam[INTENS_PRESSURE] = Points[curve_index][i];

/*
--------------------------------------------------------------------
Intens Response Time (MS)
Time between the intensification start and the pressure set in Press
for Response Time has been reached. The pressure can be increasing
or decreasing toward this point.
-------------------------------------------------------------------- */
get_intensification_response_time( m->part.pres_for_response_time, curve_index );

/*
-----------------------------------------------------------------------------
Effective Peak Intens (PSI,BAR)
The pressure recorded at the maximum position reached during intensification.
Skip EpiSkipCount points before starting to look so I don't get confused
by overshoot on the ram.
----------------------------------------------------------------------------- */
xp = Points[POSITION_CURVE_INDEX] + LastPositionBasedPoint;
pp = Points[curve_index] + LastPositionBasedPoint;
i  = LastPositionBasedPoint+1;
if ( (NofPoints-i) > EpiSkipCount )
    i += EpiSkipCount;

IParam[EFF_PEAK_INTENS] = *pp;
max_pos   = *xp;
while ( TRUE  )
    {
    i++;
    if ( i >= NofPoints )
        break;

    xp++;
    pp++;

    if ( *xp > max_pos )
        {
        max_pos = *xp;
        IParam[EFF_PEAK_INTENS] = *pp;
        }
    }

/*
----------------------------------------------------------------------
Impact Pressure to Peak  (PSI,BAR)
The pressure recorded when the End of Shot Velocity (EOSV) has been
detected and the monitoring changes from position based to time based.
---------------------------------------------------------------------- */
/*
-------------------------
Start at the impact point
------------------------- */
pp = Points[curve_index] + LastPositionBasedPoint;
max_psi = *pp;

/*
--------------------------------
Check the n-1 points before this
-------------------------------- */
n  = m->machine.impact_points[PRE_IMPACT_INDEX];
minshort( n, LastPositionBasedPoint );
n--;
while ( n )
    {
    n--;
    pp--;
    if ( *pp > max_psi )
        max_psi = *pp;
    }

/*
------------------------------
Check the n post impact points
------------------------------ */
n  = m->machine.impact_points[POST_IMPACT_INDEX];
minshort( n, NofPoints-LastPositionBasedPoint-1 );
pp = Points[curve_index] + LastPositionBasedPoint;
while ( n )
    {
    if ( *pp > max_psi )
        max_psi = *pp;
    n--;
    pp++;
    }

IParam[IMPACT_PRES_TO_PEAK] = max_psi;

/*
-------------------------------------------------------------------
Intens Squeeze Distance (IN,MM)
The distance travelled by the plunger from the EOSV position to the
final position recorded.
------------------------------------------------------------------- */
IParam[INTENS_SQUEEZE_DIST] = max_pos - IParam[P3_POS_PARM];

/*
-------------------------------------------------------
Sleeve Full Velocity (IPS,FPM,CMPS,MPS)
The plunger velocity measured at P1 (Shot Sleeve Full).
------------------------------------------------------- */
IParam[SLEEVE_FULL_VELOCITY] = Points[VELOCITY_CURVE_INDEX][IndexOfP1];

/*
------------------------------------------------------------
Runner Full Velocity (IPS,FPM,CMPS,MPS)
The plunger velocity when the metal front is at the gate P2.
------------------------------------------------------------ */
IParam[RUNNER_FULL_VELOCITY] = Points[VELOCITY_CURVE_INDEX][IndexOfP2];

/*
------------------------------------------------------------------------
Avg Fill Velocity (IPS,FPM,CMPS,MPS)
The average plunger velocity measured between position P2 and the End of
Shot Velocity (EOSV) position P3.
------------------------------------------------------------------------ */
// IParam[AVG_FILL_VELOCITY] = velocity_between_positions( IndexOfP2, LastPositionBasedPoint );
IParam[AVG_FILL_VELOCITY] = velocity_between_positions( IParam[P2_POS_PARM], IParam[P3_POS_PARM] );

/*
--------------------------------------------------------------
User Velocity Positions One - Four (IPS,FPM,CMPS,MPS)
The velocity(ies) measured at the Positions at Which to Record
Velocities set in Part Setup.
-------------------------------------------------------------- */
for ( i=0; i<USER_VEL_COUNT; i++ )
    IParam[USER_VEL_1+i] = velocity_at_position( m->part.user_velocity_position[i] );

/*
-----------------------------------------------------------------------------
User Avg Vel Range One to Four (IPS,FPM,CMPS,MPS)
The average plunger velocity recorded between 2 setpoints entered by the user
in Position Ranges Over Which to Record Average Velocities in Part Setup.
----------------------------------------------------------------------------- */
for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    IParam[USER_AVG_VEL_1+i] = 0.0;
    if ( m->part.user_avg_velocity_start[i] != m->part.user_avg_velocity_end[i] )
        IParam[USER_AVG_VEL_1+i] = velocity_between_positions( (float) m->part.user_avg_velocity_start[i], (float) m->part.user_avg_velocity_end[i] );
    }

/*
-----------------------------------------------------------------
Avg Slow Shot Velocity (IPS,FPM,CMPS,MPS)
The average velocity recorded between the initial movement of the
plunger and the Calculated Start of Fast Shot.
----------------------------------------------------------------- */
//IParam[AVG_SLOW_SHOT_VELOCITY] = velocity_between_positions( 0, csfs_index );
IParam[AVG_SLOW_SHOT_VELOCITY] = velocity_between_positions( (float) 0.0, IParam[CSFS] );

/*
-----------------------------------------------------------------------
Avg Fast Shot Velocity (IPS,FPM,CMPS,MPS)
The average velocity recorded between the Calculated Start of Fast Shot
position to the End of Shot Velocity position.
----------------------------------------------------------------------- */
// IParam[AVG_FAST_SHOT_VELOCITY] = velocity_between_positions( csfs_index, LastPositionBasedPoint );
IParam[AVG_FAST_SHOT_VELOCITY] = velocity_between_positions( IParam[CSFS], IParam[P3_POS_PARM] );

/*
----------------------------------------------------------------------
Avg Gate Velocity
The average velocity of metal thru the gate(s). This is equal to the
plunger velocity times the ratio of plunger area over total gate area.
---------------------------------------------------------------------- */
IParam[AVG_GATE_VELOCITY] = velocity_between_positions( IndexOfP2, LastPositionBasedPoint );

/*
----------------------------------------------------------------
Pre-Impact Work
This uses the differential pressure calculated during pre-impact
(a special case). The work is the sum of force * distance for
each point.
---------------------------------------------------------------- */
IParam[PRE_IMPACT_WORK] = pre_impact_work();
}

/***********************************************************************
*                       FAST_SHOT_TARGET_POSITION                      *
*                                                                      *
* Position after min stroke len where analog input crosses the         *
* target value.                                                        *
*                                                                      *
***********************************************************************/
float fast_shot_target_position( float target, BITSETYPE vartype, short curve_index )
{
short   i;
float * p;

MONITOR_ENTRY * m;

m = CurrentMonitor;
i = index_from_position( m->part.min_stroke_length );
if ( i <=0 || i > LastPositionBasedPoint )
    i = LastPositionBasedPoint / 2;

p = Points[curve_index] + i;

while ( i <= LastPositionBasedPoint )
    {
    if ( vartype & TRIGGER_WHEN_SMALLER )
        {
        if ( *p <= target )
            break;
        }
    else
        {
        if ( *p >= target )
            break;
        }
    i++;
    p++;
    }

if ( i > LastPositionBasedPoint )
    i = LastPositionBasedPoint;

return pos_from_index(i);
}

/***********************************************************************
*                          READ_FTANALOG_PARAMETERS                    *
***********************************************************************/
static void read_ftanalog_parameters( void )
{
short      i;
short      j;
short      n;
short      curve_index;
short      p;
float      max_value;
float      min_value;
float      sum;
float      x;
float      y;
TCHAR    * cp;
BITSETYPE  result_type;
BITSETYPE  variable_type;
MONITOR_ENTRY * m;

m = CurrentMonitor;

for ( p=0; p<MAX_PARMS; p++ )
    {
    if ( m->param.parameter[p].input.type == FT_ANALOG_INPUT || m->param.parameter[p].input.type == FT_TARGET_INPUT )
        {
        curve_index = m->ftanalog.array[p].channel - 1;

        /*
        ---------------------------------------------------------------------------------
        The  channel is set to MAX_FT_CHANNELS if the differential channel is to be used.
        --------------------------------------------------------------------------------- */
        if ( curve_index >= MAX_FT_CHANNELS )
            curve_index = DIFFERENTIAL_CURVE_INDEX;
        else if ( curve_index >= MAX_CONTINUOUS_CHANNELS )
            curve_index = CHAN_1_ANALOG_CURVE_INDEX;
        else
            curve_index += CHAN_1_ANALOG_CURVE_INDEX;

        x = 0.0;
        result_type   = m->ftanalog.array[p].result_type;
        variable_type = m->ftanalog.array[p].ind_var_type;
        cp            = m->ftanalog.array[p].ind_value.text();

        if ( *cp == PChar )
            {
            /*
            -----------------------------------------------------------
            I want to use a parameter, which must be already calculated
            ----------------------------------------------------------- */
            cp++;
            i = (short) asctoint32( cp );
            i--;
            if ( i>=0 && i<p )
                x = Parameter[i];
            }
        else
            {
            x = m->ftanalog.array[p].ind_value.float_value();
            }

        y = 0.0;
        cp = m->ftanalog.array[p].end_value.text();

        if ( *cp == PChar )
            {
            /*
            -----------------------------------------------------------
            I want to use a parameter, which must be already calculated
            ----------------------------------------------------------- */
            cp++;
            i = (short) asctoint32( cp );
            i--;
            if ( i>=0 && i<p )
                y = Parameter[i];
            }
        else if ( *cp == AChar )
            {
            y = 10000.0;
            }
        else
            {
            y = m->ftanalog.array[p].end_value.float_value();
            }

        if ( m->param.parameter[p].input.type == FT_ANALOG_INPUT )
            {
            i = -1;
            if ( variable_type & POSITION_VAR )
                {
                i = index_from_position( x );
                j = i;
                if ( m->ftanalog.array[p].result_type != SINGLE_POINT_RESULT_TYPE )
                    j = index_from_position( y );
                }
            else if ( variable_type & TIME_VAR )
                {
                i = index_from_time( x );
                j = i;
                if ( m->ftanalog.array[p].result_type != SINGLE_POINT_RESULT_TYPE )
                    j = index_from_time( y );
                }

            if ( j < i )
                {
                n = i;
                i = j;
                j = n;
                }

            n = 1 + j - i;
            max_value = Points[curve_index][i];
            min_value = max_value;
            sum = 0.0;
            while ( i <= j )
                {
                x = Points[curve_index][i];
                sum += x;
                minfloat( min_value, x );
                maxfloat( max_value, x );
                i++;
                }

            if ( result_type & MAX_RESULT_TYPE )
                Parameter[p] = max_value;
            else if ( result_type & MIN_RESULT_TYPE )
                Parameter[p] = min_value;
            else
                Parameter[p] = sum / float(n);
            }
        else
            {
            Parameter[p] = fast_shot_target_position( m->ftanalog.array[p].ind_value.float_value(), m->ftanalog.array[p].ind_var_type, curve_index );
            }
        }
    }
}

/***********************************************************************
*                       READ_FTONOFF_PARAMETERS                        *
***********************************************************************/
static void read_ftonoff_parameters( void )
{
short fi;
short p;
MONITOR_ENTRY * m;

m = CurrentMonitor;

for ( p=0; p<MAX_PARMS; p++ )
    {
    if ( m->param.parameter[p].input.type == FT_ON_OFF_INPUT )
        {
        fi = m->param.parameter[p].input.number;
        if ( fi <= 0 || fi > (2*FT_ONOFF_COUNT) )
            return;
        rd_on_off_input( Parameter+p, fi );
        }
    }
}

/***********************************************************************
*                    READ_DIGITAL_INPUT_PARAMETERS                     *
***********************************************************************/
static void read_digital_input_parameters()
{
BOOLEAN firstime;
short wire_number;
short p;
BOARD_DATA x;
BITSETYPE  mask;
MONITOR_ENTRY * m;

firstime = TRUE;

x = 0;
m = CurrentMonitor;

for ( p=0; p<MAX_PARMS; p++ )
    {
    if ( m->param.parameter[p].input.type == FT_DIGITAL_INPUT )
        {
        if ( firstime )
            {
            ft_read_parameter( RD_CUR_INPUT_WORD, &x );
            firstime = FALSE;
            }

        wire_number = m->param.parameter[p].input.number;
        if ( wire_number >= LOW_FT_INPUT_WIRE && wire_number <= HIGH_FT_INPUT_WIRE )
            {
            mask = 1;
            wire_number -= LOW_FT_INPUT_WIRE;
            mask <<= wire_number;

            if ( x & mask )
                Parameter[p] = 1.0;
            else
                Parameter[p] = 0.0;
            }
        }
    }
}

/***********************************************************************
*                         READ_ALL_PARAMETERS                          *
***********************************************************************/
void read_all_parameters( void )
{
short i;
short p;
MONITOR_ENTRY * m;

m = CurrentMonitor;

if ( !m )
    return;

for ( p=0; p<MAX_PARMS; p++ )
    Parameter[p] = 0.0;

read_internal_parameters();

for ( p=0; p<MAX_PARMS; p++ )
    {
    if ( m->param.parameter[p].input.type == INTERNAL_PARAMETER )
        {
        i = m->param.parameter[p].input.number - 1;
        if ( i >= 0 && i < MAX_INTERNAL_PARMS )
            Parameter[p] = IParam[i];
        }

    }

read_ftanalog_parameters();
read_ftonoff_parameters();
read_digital_input_parameters();
}

/***********************************************************************
*                           CHECK_ONE_ALARM                            *
***********************************************************************/
void check_one_alarm( float value, float low, short low_wire, float high, float high_wire )
{
BITSETYPE this_bit;
bool is_low;
bool is_high;

/*
----------------------------------------------------
If high and low are the same I should not check this
---------------------------------------------------- */
if ( not_float_zero(high-low) )
    {
    is_low  = false;
    is_high = false;

    if ( value < low )
        {
        if ( not_float_zero(low-value) )
            is_low = true;
        }
    else if ( value > high )
        {
        if ( not_float_zero(value-high) )
            is_high = true;
        }

    if ( is_low || is_high )
        HasAlarm = TRUE;

    if ( low_wire != NO_WIRE )
        {
        this_bit = wirebit(low_wire);
        ClearBits |= this_bit;
        if ( is_low )
            SetBits |= this_bit;
        }

    if ( high_wire != NO_WIRE )
        {
        this_bit = wirebit(high_wire);
        ClearBits |= this_bit;
        if ( is_high )
            SetBits |= this_bit;
        }
    }

}

/***********************************************************************
*                             CHECK_ALARMS                             *
***********************************************************************/
static void check_alarms( const SYSTEMTIME & st )
{
short      i;
short      shift_number;
SYSTEMTIME shift_date;
BITSETYPE  this_bit;

ALARM_SUMMARY_CLASS as;
PARAMETER_CLASS   * p;
PARAMETER_ENTRY   * pe;

HasAlarm = FALSE;

/*
--------------------------------------------
Do nothing if alarms are not being monitored
-------------------------------------------- */
if ( !(CurrentMonitor->machine.monitor_flags & MA_ALARM_MONITORING_ON) )
    return;

p = &CurrentMonitor->param;

if ( !p )
    return;

get_shift_number_and_date( &shift_number, &shift_date, st );

as.set_part( CurrentMonitor->part.computer, CurrentMonitor->part.machine, CurrentMonitor->part.name );
as.set_counts( Parameter, p );
as.save( shift_date, (int32) shift_number );

for ( i=0; i<MAX_PARMS; i++ )
    {
    pe = p->parameter + i;
    if ( pe->input.type != NO_PARAMETER_TYPE )
        {
        check_one_alarm( Parameter[i], pe->limits[ALARM_MIN].value, pe->limits[ALARM_MIN].wire_number, pe->limits[ALARM_MAX].value, pe->limits[ALARM_MAX].wire_number );
        check_one_alarm( Parameter[i], pe->limits[WARNING_MIN].value, pe->limits[WARNING_MIN].wire_number, pe->limits[WARNING_MAX].value, pe->limits[WARNING_MAX].wire_number );
        }
    }

/*
---------------------------------------------------
I want to set all of the wires in SetBits and clear
any in ClearBits that are NOT in SetBits
--------------------------------------------------- */
for ( i=LOW_WIRE; i<=HIGH_WIRE; i++ )
    {
    this_bit = wirebit( i );
    if ( SetBits & this_bit )
        Be->setwire( i );
    else if ( ClearBits & this_bit )
        Be->clearwire( i );
    }

}

/***********************************************************************
*                             CLEAR_ALARMS                             *
* This can be called only when the board is in command mode. It        *
* immediately clears all the alarm bits. This is called only if        *
* ClearAlarmsEachShot=Y in monall.ini.                                 *
***********************************************************************/
static void clear_alarms()
{
PARAMETER_CLASS   * p;
PARAMETER_ENTRY   * pe;
short               my_wire_number;
int                 i;
int                 j;
BITSETYPE           alarm_bits;

p = &CurrentMonitor->param;
if ( !p )
    return;

alarm_bits = 0;
for ( i=0; i<MAX_PARMS; i++ )
    {
    pe = p->parameter + i;
    if ( pe->input.type != NO_PARAMETER_TYPE )
        {
        for ( j=0; j<NOF_ALARM_LIMIT_TYPES; j++ )
            {
            my_wire_number = pe->limits[j].wire_number;
            if ( my_wire_number != NO_WIRE )
                alarm_bits |= wirebit( my_wire_number );
            }
        }
    }

alarm_bits &= Be->dout_bits;
if ( alarm_bits )
    {
    Be->dout_bits &= ~alarm_bits;
    if ( wr_opto(Be->dout_bits) == FT_OK )
        Be->need_dout_update = FALSE;
    else
        Be->need_dout_update = TRUE;
    }
}

/***********************************************************************
*                       RESET_MACHINE_FOR_GETDATA                      *
***********************************************************************/
void reset_machine_for_getdata( TCHAR * machine_to_find )
{
MONITOR_ENTRY * m;
TCHAR part_to_find[PART_NAME_LEN+1];

m = find_existing_monitor_entry( machine_to_find );

/*
--------------------------------------------------------------
It is only necessary to reset the machine if I already have it
-------------------------------------------------------------- */
if ( !m )
    return;

lstrcpy( part_to_find, m->part.name );
load_monitor_entry(m, machine_to_find, part_to_find );
}

/***********************************************************************
*                            GET_SHOT_DATA                             *
***********************************************************************/
void get_shot_data( HARDWARE_BOARD_ENTRY * b )
{
static BOOLEAN firstime = TRUE;

short       c;
short       i;
TCHAR     * cp;
MUX_ENTRY * me;
SYSTEMTIME  st;
DSTAT_CLASS d;

if ( !b )
    return;

if ( firstime )
    {
    cp = get_ini_string( MonallIniFile, ConfigSection, ClearAlarmsEachShotKey );
    if ( *cp == YChar )
        ClearAlarmsEachShot = TRUE;
    firstime = FALSE;
    }

/*
---------------------------------------------------------------------
Save the pointer to the board entry so I don't have to pass it around
--------------------------------------------------------------------- */
Be = b;

c = b->current_mux;
c--;
if ( c >= 0 && c < MAX_MUX_CHANNELS )
    {
    me = &b->mux[c];

    if ( !me->psensor )
        return;

    if ( !me->asensor )
        return;

    if ( !find_current_monitor(me->machine, me->part) )
        return;

    if ( ClearAlarmsEachShot )
        clear_alarms();

    /*
    -----------------------------
    Initialize the profile header
    ----------------------------- */
    lstrcpy( Ph.machine_name, me->machine );
    lstrcpy( Ph.part_name, me->part );

    /*
    --------------------------------------------
    The getsystemtimeasfiletime only works on NT
    -------------------------------------------- */
    GetLocalTime(&st);
    SystemTimeToFileTime(&st,&Ph.time_of_shot );
    me->time_of_last_shot = sec_since1990( st );
    if ( me->down_state != MACH_UP_STATE )
        {
        d.set_cat( DOWNCAT_SYSTEM_CAT );
        d.set_subcat( DOWNCAT_UP_SUBCAT );
        d.set_time( st );
        d.put( ComputerName, me->machine );
        me->down_state = d.down_state();

        /*
        -----------------------------------------------------------
        Only write to the downtime file if the cycle timeout is set
        ----------------------------------------------------------- */
        if ( me->cycle_timeout_seconds > 0 )
            append_downtime_record( ComputerName, me->machine, CurrentMonitor->part.name, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT );
        else
            issue_downtime_event( ComputerName, me->machine, CurrentMonitor->part.name, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT );
        }

    Ph.shot_number    = 0;
    Ph.n              = 0;
    Ph.last_pos_based_point = 0;
    for ( i=0; i<MAX_CURVES; i++ )
        {
        Ph.curves[i].type  = NO_CURVE_TYPE;
        Ph.curves[i].max   = 0;
        Ph.curves[i].min   = 0;
        Ph.curves[i].units = NO_UNITS;
        }

    CountsPerSec = me->psensor->crystal_freq * 1000000.0;

    if ( b->cycles_per_time_count )
        CountsPerSec /= float(b->cycles_per_time_count);

    if ( is_float_zero(CountsPerSec) )
        CountsPerSec = 1.0;

    X1PosDivisor   = me->psensor->x1_pos_divisor;
    if ( is_float_zero(X1PosDivisor) )
        X1PosDivisor = 1.0;

    X4PosDivisor   = me->psensor->x4_pos_divisor;

    VelocityFactor = me->psensor->vel_dividend;
    if ( is_float_zero(VelocityFactor) )
        VelocityFactor = 1.0;

    MsPerSample   = me->ms_per_sample;
    if ( is_float_zero(MsPerSample) )
        MsPerSample = 1.0;

    DistanceUnits = me->dist_units;
    VelocityUnits = me->vel_units;

    if ( !read_counts(me) )
        return;

    read_all_parameters();

    /*
    -------------------------------------------------------------------------------------
    Clear the global bitsets. This used to be done in check_alarms but I moved it here so
    I could  add bits for the reflected alarms. I no longer reflect alarms this way but
    I left the code here.
    -------------------------------------------------------------------------------------- */
    SetBits   = 0;
    ClearBits = 0;

    /*
    ---------------------------------------------------------------------------
    This will set the HasAlarm global if any values exceeded their alarm values
    --------------------------------------------------------------------------- */
    check_alarms( st );

    /*
    -------------------------------------------------
    This was a good shot, turn on the monitoring wire
    (Nothing is done if it is already on).
    ------------------------------------------------- */
    if ( MonitorWire != NO_WIRE && ClearMonitorWireOnDown )
        b->setwire( MonitorWire );

    if ( DownTimeAlarmWire != NO_WIRE )
        b->clearwire( DownTimeAlarmWire );

    /*
    --------------------------------------------------------------------------------
    Set the differential curve to not_used, as I now do the calculations on the fly.
    -------------------------------------------------------------------------------- */
    Ph.curves[DIFFERENTIAL_CURVE_INDEX].type  = NO_CURVE_TYPE;

    add_to_shotsave( Ph, Points, Parameter, CurrentMonitor->param.count(), HasAlarm );
    SetEvent( ShotSaveEvent );
    }

}
