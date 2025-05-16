#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\part.h"
#include "..\include\subs.h"
#include "..\include\array.h"
#include "..\include\fileclas.h"
#include "..\include\stringcl.h"
#include "..\include\v5help.h"

#include "resource.h"
#include "resrc1.h"

static HWND CalcWindow = 0;
static HWND DensityDialogWindow = 0;
static bool ProgramIsUpdating   = false;
static bool DensityIsUpdating   = false;

struct BUTTON_TYPE {
    UINT  control_id;
    short unit_id;
    };

static BOOLEAN FirstTime = TRUE;
static BOOLEAN SyncPlungerDiameter = FALSE;
static double LbsPerTon = 2000.0;

static DOUBLE_ARRAY_CLASS Density;
static INT32_ARRAY_CLASS  DensityUnitsId;
static int32              CurrentDensityIndex = 0;

static short CurrentPressureUnit = PSI_UNITS;
static short CurrentDistanceUnit;
static short CurrentAreaUnit;
static short CurrentVolumeUnit;
static short CurrentVelocityUnit;
static short CurrentWeightUnit;

/*
---------------
Pressure labels
--------------- */
static const UINT PressureLabel[] =
    {
    INTENS_PRESSURE_UNITS_TBOX,
    METAL_PRESSURE_UNITS_TBOX
    };

static const int32 NofPressureLabels = sizeof(PressureLabel)/sizeof(UINT);

/*
---------------
Distance labels
--------------- */
static const UINT DistanceLabel[] =
    {
    PLUNGER_DIAMETER_UNITS_TBOX,
    DRY_SHOT_LENGTH_UNITS_TBOX,
    P1_POSITION_UNITS_TBOX,
    BISCUIT_LEN_UNITS_TBOX,
    DIST_TO_FILL_RUNNER_UNITS_TBOX,
    METAL_AT_GATE_UNITS_TBOX,
    CYLINDER_DIAMETER_UNITS_TBOX,
    SWEEP_TO_FILL_CAVITY_UNITS_TBOX
    };

static const int32 NofDistanceLabels = sizeof(DistanceLabel)/sizeof(UINT);

/*
---------------
Distance values
--------------- */
static const UINT DistanceValue[] =
    {
    PLUNGER_DIAMETER_EBOX,
    DRY_SHOT_LENGTH_EBOX,
    BISCUIT_LEN_EBOX,
    CYLINDER_DIAMETER_EBOX
    };

static const int32 NofDistanceValues = sizeof(DistanceValue)/sizeof(UINT);

/*
-----------
Area labels
----------- */
static const UINT AreaLabel[] = {
    PLUNGER_AREA_UNITS_TBOX,
    GATE_AREA_UNITS_TBOX,
    CYLINDER_AREA_UNITS_TBOX,
    AREA_CAST_OVRFLW_UNITS_TBOX
    };

static const int32 NofAreaLabels = sizeof(AreaLabel)/sizeof(UINT);

/*
-----------
Area values
----------- */
static const UINT AreaValue[] = {
    GATE_AREA_EBOX,
    CYLINDER_AREA_TBOX,
    AREA_CAST_OVRFLW_EBOX
    };

static const int32 NofAreaValues = sizeof(AreaValue)/sizeof(UINT);

/*
-------------
Volume Labels
------------- */
static const UINT VolumeLabel[] = {
    SHOT_SLEEVE_VOL_UNITS_TBOX,
    VOLUME_OF_SHOT_UNITS_TBOX,
    BISCUIT_VOLUME_UNITS_TBOX,
    RUNNER_VOLUME_UNITS_TBOX,
    CAVITY_VOLUME_UNITS_TBOX
    };

static const int32 NofVolumeLabels = sizeof(VolumeLabel)/sizeof(UINT);

/*
---------------
Velocity labels
--------------- */
static const UINT VelocityLabel[] = {
    VEL_DURING_FILL_UNITS_TBOX,
    GATE_VELOCITY_UNITS_TBOX
    };

static const int32 NofVelocityLabels = sizeof(VelocityLabel)/sizeof(UINT);

/*
---------------
Velocity values
--------------- */
static const UINT VelocityValue[] = {
    VEL_DURING_FILL_EBOX
    };

static const int32 NofVelocityValues = sizeof(VelocityValue)/sizeof(UINT);

/*
-------------
Weight labels
------------- */
static const UINT WeightLabel[] = {
    CASTING_WEIGHT_UNITS_TBOX,
    OVERFLOW_WEIGHT_UNITS_TBOX,
    CAVITY_WEIGHT_UNITS_TBOX,
    WEIGHT_R_AND_B_UNITS_TBOX,
    TOTAL_SHOT_WEIGHT_UNITS_TBOX,
    BISCUIT_WEIGHT_UNITS_TBOX,
    RUNNER_WEIGHT_UNITS_TBOX
    };

static const int32 NofWeightLabels = sizeof(WeightLabel)/sizeof(UINT);

/*
-------------
Weight Values
------------- */
static const UINT WeightValue[] = {
    CASTING_WEIGHT_EBOX,
    OVERFLOW_WEIGHT_EBOX,
    WEIGHT_R_AND_B_EBOX
    };

static const int32 NofWeightValues = sizeof(WeightValue)/sizeof(UINT);

static const UINT SaveBoxes[] = {
    PLUNGER_DIAMETER_EBOX,
    DRY_SHOT_LENGTH_EBOX,
    CASTING_WEIGHT_EBOX,
    OVERFLOW_WEIGHT_EBOX,
    WEIGHT_R_AND_B_EBOX,
    BISCUIT_LEN_EBOX,
    VEL_DURING_FILL_EBOX,
    GATE_AREA_EBOX,
    FILLTIME_EBOX,
    INTENS_PRESSURE_EBOX,
    CYLINDER_DIAMETER_EBOX,
    AREA_CAST_OVRFLW_EBOX
    };

const int32 MAX_DENSITY_LB_LINE = DENSITY_NAME_LEN + 2 * MAX_DOUBLE_LEN + 2;

class DENSITY_CLASS
    {
    private:
    TCHAR s[MAX_DENSITY_LB_LINE+1];

    public:
    TCHAR name[DENSITY_NAME_LEN+1];
    TCHAR liquid_density[MAX_DOUBLE_LEN+1];
    TCHAR solid_density[MAX_DOUBLE_LEN+1];

    DENSITY_CLASS( void );
    BOOLEAN read_editboxes( void );
    BOOLEAN read( TCHAR * lb_source_line );
    BOOLEAN read( DB_TABLE & t );
    BOOLEAN put_record(  DB_TABLE & t, int density_number );
    BOOLEAN fill_editboxes( void );
    TCHAR  * lbline( void );
    };

const int32 NofSaveBoxes = sizeof(SaveBoxes)/sizeof(UINT);

const static TCHAR LbsPerTonKey[] = TEXT( "LbsPerTon" );
const static TCHAR SyncPlungerDiameterKey[] = TEXT( "SyncPlungerDiameter" );
const static TCHAR NullChar       = TEXT( '\0' );
const static TCHAR TabChar        = TEXT( '\t' );

extern PART_CLASS CurrentPart;
extern HINSTANCE  MainInstance;
extern HWND       MainDialogWindow;
extern TCHAR      EditPartIniFile[];
extern TCHAR      ConfigSection[];

void gethelp( UINT helptype, DWORD context );
TCHAR * resource_string( UINT resource_id );


/***********************************************************************
*                             DENSITY_CLASS                            *
*                             DENSITY_CLASS                            *
***********************************************************************/
DENSITY_CLASS::DENSITY_CLASS( void )
{
*s              = NullChar;
*name           = NullChar;
*liquid_density = NullChar;
*solid_density  = NullChar;
}

/***********************************************************************
*                             DENSITY_CLASS                            *
*                             READ_EDITBOXES                           *
***********************************************************************/
BOOLEAN DENSITY_CLASS::read_editboxes( void )
{
get_text( name, DensityDialogWindow, METAL_NAME_EBOX, DENSITY_NAME_LEN );
strip( name );

get_text( liquid_density, DensityDialogWindow, LIQUID_DENSITY_EBOX, MAX_DOUBLE_LEN );
strip( liquid_density );

get_text( solid_density,  DensityDialogWindow, SOLID_DENSITY_EBOX,  MAX_DOUBLE_LEN );
strip( solid_density );

return TRUE;
}

/***********************************************************************
*                             DENSITY_CLASS                            *
*                                 LBLINE                               *
***********************************************************************/
TCHAR * DENSITY_CLASS::lbline( void )
{
TCHAR * cp;
cp = copy_w_char( s, name, TabChar );
cp = copy_w_char( cp, liquid_density, TabChar );
lstrcpy( cp, solid_density );
return s;
}

/***********************************************************************
*                            COPY_FROM_LB                              *
***********************************************************************/
static void copy_from_lb( TCHAR * dest, TCHAR * sorc )
{

if ( lstrlen(sorc) > 0 )
    lstrcpy( dest, sorc );
else
    *dest = NullChar;
}

/***********************************************************************
*                                 READ                                 *
***********************************************************************/
BOOLEAN DENSITY_CLASS::read( TCHAR * lb_source_line )
{
TCHAR * cp;

cp = lb_source_line;
replace_tab_with_null( cp );
copy_from_lb( name, cp );

cp = nextstring( cp );
replace_tab_with_null( cp );
copy_from_lb( liquid_density, cp );

cp = nextstring( cp );
copy_from_lb( solid_density, cp );

return TRUE;
}

/***********************************************************************
*                                 READ                                 *
***********************************************************************/
BOOLEAN DENSITY_CLASS::read( DB_TABLE & t )
{
t.get_alpha( name, DENSITY_NAME_OFFSET, DENSITY_NAME_LEN );
lstrcpy( liquid_density, ascii_double(t.get_double(DENSITY_LIQUID_OFFSET)) );
lstrcpy( solid_density,  ascii_double(t.get_double(DENSITY_SOLID_OFFSET))  );
return TRUE;
}

/***********************************************************************
*                              PUT_RECORD                              *
***********************************************************************/
BOOLEAN DENSITY_CLASS::put_record(  DB_TABLE & t, int density_number )
{
t.put_long(   DENSITY_NUMBER_OFFSET, density_number,            DENSITY_NUMBER_LEN );
t.put_alpha(  DENSITY_NAME_OFFSET,   name,                      DENSITY_NAME_LEN   );
t.put_long(   DENSITY_UNITS_OFFSET,  LBS_PER_IN_CUBED_UNITS,    UNITS_ID_LEN       );
t.put_double( DENSITY_SOLID_OFFSET,  extdouble(solid_density),  DENSITY_FLOAT_LEN  );
t.put_double( DENSITY_LIQUID_OFFSET, extdouble(liquid_density), DENSITY_FLOAT_LEN  );
return TRUE;
}

/***********************************************************************
*                            FILL_EDITBOXES                            *
***********************************************************************/
BOOLEAN DENSITY_CLASS::fill_editboxes( void )
{
set_text( DensityDialogWindow, METAL_NAME_EBOX,     name );
set_text( DensityDialogWindow, LIQUID_DENSITY_EBOX, liquid_density );
set_text( DensityDialogWindow, SOLID_DENSITY_EBOX,  solid_density  );
return TRUE;
}

/***********************************************************************
*                             SET_TAB_STOPS                            *
***********************************************************************/
static void set_tab_stops( void )
{
static int id[] = {
 LIQUID_DENSITY_STATIC,
 SOLID_DENSITY_STATIC
 };

const int n = sizeof(id)/sizeof(int);

static int tabs[n];

int        i;
int        pixels_per_dialog_unit;
int        left;
HWND       w;
RECT       r;

if ( !DensityDialogWindow )
    return;

r.bottom = 1;
r.left   = 1;
r.right  = 4;
r.top    = 1;
MapDialogRect( DensityDialogWindow, &r );
pixels_per_dialog_unit = r.right;

w = GetDlgItem(DensityDialogWindow, DENSITY_LISTBOX );
GetWindowRect( w, &r );
left = r.left;

for ( i=0; i<n; i++ )
    {
    w = GetDlgItem(DensityDialogWindow, id[i] );
    GetWindowRect( w, &r );
    tabs[i] = r.left - left;
    tabs[i] *= 4;
    tabs[i] /= pixels_per_dialog_unit;
    }

SendDlgItemMessage( DensityDialogWindow, DENSITY_LISTBOX, LB_SETTABSTOPS, (WPARAM) n, (LPARAM) tabs );
}

/***********************************************************************
*                          END_DENSITY_DIALOG                          *
***********************************************************************/
static void end_density_dialog( void )
{
HWND w;

if ( DensityDialogWindow )
    {
    w = DensityDialogWindow;
    DensityDialogWindow = 0;
    EndDialog( w, 0 );
    }
}

/***********************************************************************
*                            SAVE_DENSITIES                            *
***********************************************************************/
BOOLEAN save_densities( void )
{
LISTBOX_CLASS lb;
int32         i;
int32         n;
TCHAR       * cp;
DENSITY_CLASS density;
DB_TABLE      t;

lb.init( DensityDialogWindow, DENSITY_LISTBOX );
n = lb.count();

if ( t.open( density_dbname(), DENSITY_RECLEN, FL ) )
    {
    t.empty();
    for ( i=0; i<n; i++ )
        {
        cp = lb.item_text( i );
        if ( cp )
            {
            density.read( cp );
            density.put_record( t, i+1 );
            t.rec_append();
            }
        }
    t.close();
    }

return TRUE;
}

/***********************************************************************
*                    GET_CURRENTLY_SELECTED_DENSITY                    *
***********************************************************************/
static void get_currently_selected_density( void )
{
LISTBOX_CLASS lb;
TCHAR   buf[MAX_DENSITY_LB_LINE+1];
TCHAR * cp;
DENSITY_CLASS density;

lb.init( DensityDialogWindow, DENSITY_LISTBOX );
cp = lb.selected_text();
if ( !cp )
    return;

lstrcpy( buf, cp );
density.read( buf );

DensityIsUpdating = true;

density.fill_editboxes();

DensityIsUpdating = false;
}

/***********************************************************************
*                         INIT_DENSITY_DIALOG                          *
***********************************************************************/
static void init_density_dialog( void )
{
LISTBOX_CLASS lb;
DB_TABLE t;
DENSITY_CLASS density;

if ( !DensityDialogWindow )
    return;

lb.init( DensityDialogWindow, DENSITY_LISTBOX );
lb.empty();

if ( t.open( density_dbname(), DENSITY_RECLEN, PFL ) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        density.read( t );
        lb.add( density.lbline() );
        }
    t.close();
    }

lb.setcursel( 0 );
get_currently_selected_density();
}

/***********************************************************************
*                         ADD_DENSITY_ENTRY                            *
***********************************************************************/
static void add_density_entry( void )
{
LISTBOX_CLASS lb;
DENSITY_CLASS density;
int           i;
HWND          w;

if ( !DensityDialogWindow )
    return;

lb.init( DensityDialogWindow, DENSITY_LISTBOX );
density.read_editboxes();
lstrcpy( density.name, resource_string(NEW_DENSITY_NAME_STRING) );
i = lb.add( density.lbline() );
lb.setcursel( i );
DensityIsUpdating = true;

density.fill_editboxes();

DensityIsUpdating = false;

w = GetDlgItem( DensityDialogWindow, METAL_NAME_EBOX );
SetFocus( w );
eb_select_all( w );
}

/***********************************************************************
*                      DELETE_DENSITY_ENTRY                            *
***********************************************************************/
static void delete_density_entry( void )
{
LISTBOX_CLASS lb;
DENSITY_CLASS density;
int           i;
int           n;

if ( !DensityDialogWindow )
    return;

lb.init( DensityDialogWindow, DENSITY_LISTBOX );
n = lb.count();
if ( n < 2 )
    return;

i = lb.selected_index();
if ( i >= 0 )
    {
    lb.remove( i );
    n = lb.count();
    if ( i >= n )
        i = n-1;
    if ( i < 0 )
        i = 0;
    lb.setcursel( i );
    get_currently_selected_density();
    }
}

/***********************************************************************
*                    REFRESH_DENSITY_LISTBOX_ENTRY                     *
***********************************************************************/
static void refresh_density_listbox_entry( void )
{
LISTBOX_CLASS lb;
DENSITY_CLASS density;

if ( !DensityDialogWindow )
    return;

lb.init( DensityDialogWindow, DENSITY_LISTBOX );

density.read_editboxes();
lb.replace( density.lbline() );
}

/***********************************************************************
*                          DENSITY_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK density_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        DensityDialogWindow = hWnd;
        set_tab_stops();
        limit_text( DensityDialogWindow, METAL_NAME_EBOX,     DENSITY_NAME_LEN  );
        limit_text( DensityDialogWindow, LIQUID_DENSITY_EBOX, DENSITY_FLOAT_LEN );
        limit_text( DensityDialogWindow, SOLID_DENSITY_EBOX,  DENSITY_FLOAT_LEN );
        init_density_dialog();
        break;

    case WM_COMMAND:

        if ( cmd == LBN_SELCHANGE && id == DENSITY_LISTBOX )
            {
            get_currently_selected_density();
            return TRUE;
            }

        if ( id == NEW_DENSITY_BUTTON )
            {
            add_density_entry();
            return TRUE;
            }

        if ( !DensityIsUpdating )
            {
            if ( id == METAL_NAME_EBOX || id == LIQUID_DENSITY_EBOX || id == SOLID_DENSITY_EBOX )
                {
                if ( cmd == EN_CHANGE )
                    {
                    refresh_density_listbox_entry();
                    return TRUE;
                    }
                }
            }

        if ( id == SAVE_DENSITY_CHANGES_BUTTON )
            {
            save_densities();
            PostMessage( CalcWindow, WM_NEW_DATA, 0, 0L );
            end_density_dialog();
            return TRUE;
            }

        if ( id == DELETE_DENSITY_BUTTON )
            {
            delete_density_entry();
            return TRUE;
            }

        if ( id == IDCANCEL )
            {
            end_density_dialog();
            return TRUE;
            }

        break;
    }

return FALSE;
}

/***********************************************************************
*                     UPDATE_WEIGHT_UNIT_LABELS                        *
***********************************************************************/
static void update_weight_unit_labels( void )
{

TCHAR * cp;
TCHAR  buf[MAX_DOUBLE_LEN+1];

int32   i;
UINT    id;
double   y;
short   new_weight_unit_id;
bool    was_updating;

new_weight_unit_id = CurrentWeightUnit;
was_updating       = ProgramIsUpdating;
ProgramIsUpdating  = true;

/*
----------------------------
Check for new weight units
---------------------------- */
for ( id=KG_UNITS_RADIO; id<= LBS_UNITS_RADIO; id++ )
    {
    if ( is_checked(CalcWindow, id) )
        {
        switch ( id )
            {
            case LBS_UNITS_RADIO:
                new_weight_unit_id = LBS_UNITS;
                break;

            case KG_UNITS_RADIO:
                new_weight_unit_id = KG_UNITS;
                break;
            }

        break;
        }

    }

if ( new_weight_unit_id != CurrentWeightUnit )
    {
    cp = units_name( new_weight_unit_id );

    /*
    ------------------------------
    Change all the weight labels
    ------------------------------ */
    for ( i=0; i<NofWeightLabels; i++ )
        set_text( CalcWindow, WeightLabel[i], cp );

    /*
    ------------------------------
    Change all the weight values
    ------------------------------ */
    for ( i=0; i<NofWeightValues; i++ )
        {
        if ( get_text(buf, CalcWindow, WeightValue[i], MAX_DOUBLE_LEN) )
            {
            y = extdouble( buf );
            y = adjust_for_units( new_weight_unit_id, y, CurrentWeightUnit  );
            set_text( CalcWindow, WeightValue[i], ascii_double(y) );
            }
        }

    CurrentWeightUnit = new_weight_unit_id;
    }

ProgramIsUpdating = was_updating;
}

/***********************************************************************
*                     UPDATE_VELOCITY_UNIT_LABELS                      *
***********************************************************************/
static void update_velocity_unit_labels( void )
{

TCHAR * cp;
TCHAR  buf[MAX_DOUBLE_LEN+1];

int32   i;
UINT    id;
double  y;
short   new_velocity_unit_id;

bool    was_updating;

was_updating       = ProgramIsUpdating;
ProgramIsUpdating  = true;

new_velocity_unit_id = CurrentVelocityUnit;

/*
----------------------------
Check for new velocity units
---------------------------- */
for ( id=IPS_UNITS_RADIO; id<= MPS_UNITS_RADIO; id++ )
    {
    if ( is_checked(CalcWindow, id) )
        {
        switch ( id )
            {
            case IPS_UNITS_RADIO:
                new_velocity_unit_id = IPS_UNITS;
                break;
            case FPM_UNITS_RADIO:
                new_velocity_unit_id = FPM_UNITS;
                break;

            case CMPS_UNITS_RADIO:
                new_velocity_unit_id = CMPS_UNITS;
                break;

            case MPS_UNITS_RADIO:
                new_velocity_unit_id = MPS_UNITS;
                break;
            }

        break;
        }

    }

if ( new_velocity_unit_id != CurrentVelocityUnit )
    {
    cp = units_name( new_velocity_unit_id );

    /*
    ------------------------------
    Change all the velocity labels
    ------------------------------ */
    for ( i=0; i<NofVelocityLabels; i++ )
        set_text( CalcWindow, VelocityLabel[i], cp );

    /*
    ------------------------------
    Change all the velocity values
    ------------------------------ */
    for ( i=0; i<NofVelocityValues; i++ )
        {
        if ( get_text(buf, CalcWindow, VelocityValue[i], MAX_DOUBLE_LEN) )
            {
            y = extdouble( buf );
            y = adjust_for_units( new_velocity_unit_id, y, CurrentVelocityUnit  );
            set_text( CalcWindow, VelocityValue[i], ascii_double(y) );
            }
        }

    CurrentVelocityUnit = new_velocity_unit_id;
    }

ProgramIsUpdating = was_updating;
}

/***********************************************************************
*                     UPDATE_DISTANCE_UNIT_LABELS                      *
***********************************************************************/
static void update_distance_unit_labels( void )
{

TCHAR * cp;

int32   i;
UINT    id;
double  y;
short   new_distance_unit_id;
short   new_area_unit_id;
short   new_volume_unit_id;
bool    was_updating;

was_updating       = ProgramIsUpdating;
ProgramIsUpdating  = true;

new_distance_unit_id = CurrentDistanceUnit;
new_area_unit_id     = CurrentAreaUnit;
new_volume_unit_id   = CurrentVolumeUnit;

/*
----------------------------
Check for new distance units
---------------------------- */
for ( id=IN_UNITS_RADIO; id<= MM_UNITS_RADIO; id++ )
    {
    if ( is_checked(CalcWindow, id) )
        {
        switch ( id )
            {
            case IN_UNITS_RADIO:
                new_distance_unit_id = INCH_UNITS;
                new_area_unit_id     = IN_SQUARED_UNITS;
                new_volume_unit_id   = IN_CUBED_UNITS;
                break;

            case CM_UNITS_RADIO:
                new_distance_unit_id = CM_UNITS;
                new_area_unit_id     = CM_SQUARED_UNITS;
                new_volume_unit_id   = CM_CUBED_UNITS;
                break;

            case MM_UNITS_RADIO:
                new_distance_unit_id = MM_UNITS;
                new_area_unit_id     = MM_SQUARED_UNITS;
                new_volume_unit_id   = MM_CUBED_UNITS;
                break;
            }

        break;
        }

    }

if ( new_distance_unit_id != CurrentDistanceUnit )
    {
    cp = units_name( new_distance_unit_id );

    /*
    ------------------------------
    Change all the distance labels
    ------------------------------ */
    for ( i=0; i<NofDistanceLabels; i++ )
        set_text( CalcWindow, DistanceLabel[i], cp );

    /*
    ------------------------------
    Change all the distance values
    ------------------------------ */
    for ( i=0; i<NofDistanceValues; i++ )
        {
        y = get_double_from_ebox( CalcWindow, DistanceValue[i] );
        y = adjust_for_units( new_distance_unit_id, y, CurrentDistanceUnit  );
        set_text( CalcWindow, DistanceValue[i], ascii_double(y) );
        }

    CurrentDistanceUnit = new_distance_unit_id;

    /*
    --------------------------
    Change all the area labels
    -------------------------- */
    cp = units_name( new_area_unit_id );
    for ( i=0; i<NofAreaLabels; i++ )
        set_text( CalcWindow, AreaLabel[i], cp );

    /*
    --------------------------
    Change all the area values
    -------------------------- */
    for ( i=0; i<NofAreaValues; i++ )
        {
        y = get_double_from_ebox( CalcWindow, AreaValue[i] );
        y = adjust_for_units( new_area_unit_id, y, CurrentAreaUnit  );
        set_text( CalcWindow, AreaValue[i], ascii_double(y) );
        }


    CurrentAreaUnit = new_area_unit_id;

    /*
    ----------------------------
    Change all the volume labels
    ---------------------------- */
    cp = units_name( new_volume_unit_id );
    for ( i=0; i<NofVolumeLabels; i++ )
        set_text( CalcWindow, VolumeLabel[i], cp );

    CurrentVolumeUnit = new_volume_unit_id;
    }

ProgramIsUpdating = was_updating;
}

/***********************************************************************
*                         UPDATE_CALCULATIONS                          *
***********************************************************************/
static void update_calculations( void )
{
const double pi   = 3.14159;
const double four = 4.0;

double gate_area;
double p1;
double plunger_area;
double metal_density;
double biscuit_volume;
double x;
double y;

ProgramIsUpdating = true;

/*
------------
Plunger area
------------ */
x = get_double_from_ebox( CalcWindow, PLUNGER_DIAMETER_EBOX );
x *= x;
x *= pi;
x /= four;
set_text( CalcWindow, PLUNGER_AREA_TBOX, ascii_double(x) );

plunger_area = x;
if ( plunger_area < 0.01 )
    plunger_area = 1.0;

/*
---------------------------------------------------
Shot sleeve volume = plunger area * dry shot length
--------------------------------------------------- */
x *= get_double_from_ebox( CalcWindow, DRY_SHOT_LENGTH_EBOX );
set_text( CalcWindow, SHOT_SLEEVE_VOL_TBOX, ascii_double(x) );

/*
------------------------------------------------
Cavity weight = casting weight + overflow weight
------------------------------------------------ */
x = get_double_from_ebox( CalcWindow, CASTING_WEIGHT_EBOX );
x += get_double_from_ebox( CalcWindow, OVERFLOW_WEIGHT_EBOX );
set_text( CalcWindow, CAVITY_WEIGHT_TBOX, ascii_double(x) );

/*
-----------------------------------------
Total weight = cavity weight + R&B weight
----------------------------------------- */
x += get_double_from_ebox( CalcWindow, WEIGHT_R_AND_B_EBOX );
set_text( CalcWindow, TOTAL_SHOT_WEIGHT_TBOX, ascii_double(x) );

/*
-------------
Metal Density
------------- */
metal_density = Density[CurrentDensityIndex];

if ( DensityUnitsId[CurrentDensityIndex] == LBS_PER_IN_CUBED_UNITS )
    {
    if ( CurrentWeightUnit != LBS_UNITS )
        metal_density = adjust_for_units( CurrentWeightUnit, metal_density, LBS_UNITS );

    if ( CurrentDistanceUnit != INCH_UNITS )
        {
        y = 1.0;
        y = adjust_for_units( CurrentDistanceUnit, y, INCH_UNITS );
        y *= y * y;
        if ( y > .0001 )
            metal_density /= y;
        }
    }
else  /* Must be g/cm3 */
    {
    if ( CurrentWeightUnit == LBS_UNITS )
        metal_density = adjust_for_units( CurrentWeightUnit, metal_density, KG_UNITS );
    metal_density /= 1000.0;

    if ( CurrentDistanceUnit != CM_UNITS )
        {
        y = 1.0;
        y = adjust_for_units( CurrentDistanceUnit, y, CM_UNITS );
        y *= y;
        y *= y;
        if ( y > .0001 )
            metal_density /= y;
        }
    }

if ( metal_density == 0.0 )
    metal_density = 1.0;

/*
-----------
Shot volume
----------- */
x /= metal_density;
set_text( CalcWindow, VOLUME_OF_SHOT_TBOX, ascii_double(x) );

/*
------------------
% Shot sleeve full
------------------ */
y = get_double_from_ebox( CalcWindow, SHOT_SLEEVE_VOL_TBOX );
if ( y > .01 )
    {
    x /= y;
    }

/*
-----------
P1 position
----------- */
p1 = get_double_from_ebox( CalcWindow, DRY_SHOT_LENGTH_EBOX );
p1 *= (1.0-x);
set_text( CalcWindow, P1_POSITION_TBOX, ascii_double(p1) );


x *= 100.0;
set_text( CalcWindow, PERCENT_SLEEVE_FULL_TBOX, ascii_double(x) );

/*
--------------
Biscuit volume
-------------- */
biscuit_volume = plunger_area * get_double_from_ebox( CalcWindow, BISCUIT_LEN_EBOX );
set_text( CalcWindow, BISCUIT_VOLUME_TBOX, ascii_double(biscuit_volume) );

/*
--------------
Biscuit weight
-------------- */
x = biscuit_volume * metal_density;
set_text( CalcWindow, BISCUIT_WEIGHT_TBOX, ascii_double(x) );

/*
-------------
Runner weight
------------- */
y = get_double_from_ebox( CalcWindow, WEIGHT_R_AND_B_EBOX );
y -= x;
set_text( CalcWindow, RUNNER_WEIGHT_TBOX, ascii_double(y) );

/*
-------------
Runner volume
------------- */
y /= metal_density;
set_text( CalcWindow, RUNNER_VOLUME_TBOX, ascii_double(y) );

/*
-----------------------
Distance to fill runner
----------------------- */
y /= plunger_area;
set_text( CalcWindow, DIST_TO_FILL_RUNNER_TBOX, ascii_double(y) );

/*
-------------
Metal at gate
------------- */
y += p1;
set_text( CalcWindow, METAL_AT_GATE_TBOX, ascii_double(y) );

/*
-------------
Gate velocity
------------- */
gate_area = get_double_from_ebox( CalcWindow, GATE_AREA_EBOX );
if ( gate_area < .001 )
    gate_area = 1.0;
y = get_double_from_ebox( CalcWindow, VEL_DURING_FILL_EBOX );
y *= plunger_area;
y /= gate_area;
set_text( CalcWindow, GATE_VELOCITY_EBOX, ascii_double(y) );

/*
-------------
Cavity volume
------------- */
x = get_double_from_ebox( CalcWindow, CAVITY_WEIGHT_TBOX );
x /= metal_density;
set_text( CalcWindow, CAVITY_VOLUME_TBOX, ascii_double(x) );

/*
--------------------
Sweep to fill cavity
-------------------- */
x /= plunger_area;
set_text( CalcWindow, SWEEP_TO_FILL_CAVITY_TBOX, ascii_double(x) );

/*
---------
Fill time
--------- */
y = get_double_from_ebox( CalcWindow, VEL_DURING_FILL_EBOX );

/*
-------------------------
Convert to distance units
------------------------- */
y /= dups_to_vu( CurrentVelocityUnit, CurrentDistanceUnit );

if ( y < .01 )
    y = 1.0;

/*
-------------
Convert to ms
------------- */
x *= 1000.0;

x /= y;
set_text( CalcWindow, FILLTIME_EBOX, ascii_double(x) );

/*
------------------------
Plunger Area / Gate area
------------------------ */
x = plunger_area / gate_area;
set_text( CalcWindow, PLUNGER_GATE_RATIO_TBOX, ascii_double(x) );

/*
-----------------------
Hydraulic cylinder area
----------------------- */
x = get_double_from_ebox( CalcWindow, CYLINDER_DIAMETER_EBOX );
x *= x;
x *= pi;
x /= four;
set_text( CalcWindow, CYLINDER_AREA_TBOX, ascii_double(x) );

/*
--------------
Metal pressure
-------------- */
x /= plunger_area;
x *= get_double_from_ebox( CalcWindow, INTENS_PRESSURE_EBOX );
set_text( CalcWindow, METAL_PRESSURE_TBOX, ascii_double(x) );

/*
-------------
Opening force
------------- */
short desired_pressure_unit;
short desired_area_unit;

if ( CurrentWeightUnit == LBS_UNITS )
    {
    desired_pressure_unit = PSI_UNITS;
    desired_area_unit     = IN_SQUARED_UNITS;
    }
else
    {
    desired_pressure_unit = KG_PER_CM2_UNITS;
    desired_area_unit     = CM_SQUARED_UNITS;
    }

if ( CurrentPressureUnit != desired_pressure_unit )
    x = adjust_for_units( desired_pressure_unit, x, CurrentPressureUnit );

y = get_double_from_ebox( CalcWindow, AREA_CAST_OVRFLW_EBOX );
if ( CurrentAreaUnit != desired_area_unit )
    y = adjust_for_units( desired_area_unit, y, CurrentAreaUnit );

x *= y;

if ( CurrentWeightUnit == LBS_UNITS )
    x /= LbsPerTon;
else
    x /= 1000.0;
set_text( CalcWindow, OPENING_FORCE_TBOX, ascii_double(x) );

ProgramIsUpdating = false;
}

/***********************************************************************
*                          DO_FILLTIME_CHANGE                          *
*                                                                      *
* Normally filltime is calculated but if you change it then I will     *
* modify the gate velocity accordingly.                                *
*                                                                      *
***********************************************************************/
static void do_filltime_change( void )
{
double x;
double v;
double t;

ProgramIsUpdating = true;
x = get_double_from_ebox( CalcWindow, SWEEP_TO_FILL_CAVITY_TBOX );
t = get_double_from_ebox( CalcWindow, FILLTIME_EBOX );
if ( t < .001 )
    t = 1.0;

v = 1000.0 * x / t;

set_text( CalcWindow, VEL_DURING_FILL_EBOX, ascii_double(v) );

v *= get_double_from_ebox( CalcWindow, PLUNGER_GATE_RATIO_TBOX );
set_text( CalcWindow, GATE_VELOCITY_EBOX, ascii_double(v) );

ProgramIsUpdating = false;
}

/***********************************************************************
*                          DO_GATE_VELOCITY_CHANGE                     *
*                                                                      *
* Normally gate vel is calculated but if you change it then I will     *
* modify the filltime and vel during fill.                             *
*                                                                      *
***********************************************************************/
static void do_gate_velocity_change( void )
{
double a;
double x;
double v;
double t;

ProgramIsUpdating = true;
v = get_double_from_ebox( CalcWindow, GATE_VELOCITY_EBOX );

v *= get_double_from_ebox( CalcWindow, GATE_AREA_EBOX );
a = get_double_from_ebox( CalcWindow, PLUNGER_AREA_TBOX );
if ( a > .001 )
    v /= a;

set_text( CalcWindow, VEL_DURING_FILL_EBOX, ascii_double(v) );

x = get_double_from_ebox( CalcWindow, SWEEP_TO_FILL_CAVITY_TBOX );
t = x * 1000.0;
if ( v > .001 )
    t /= v;

set_text( CalcWindow, FILLTIME_EBOX, ascii_double(t) );

ProgramIsUpdating = false;
}

/***********************************************************************
*                          SAVE_CALC_CHANGES                           *
***********************************************************************/
void save_calc_changes( void )
{
int32         i;
TCHAR         s[MAX_DOUBLE_LEN+1];
FILE_CLASS    f;
LISTBOX_CLASS lb;
double        x;

lb.init( CalcWindow, DENSITY_CBOX );

f.open_for_write( calc_file_name(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) );

f.writeline( lb.selected_text() );
f.writeline( int32toasc(CurrentDistanceUnit) );
f.writeline( int32toasc(CurrentVelocityUnit) );
f.writeline( int32toasc(CurrentWeightUnit) );

for ( i=0; i<NofSaveBoxes; i++ )
    {
    get_text(s, CalcWindow, SaveBoxes[i], MAX_DOUBLE_LEN );
    f.writeline( s );
    }

f.close();

if ( is_checked(CalcWindow, COPY_P1_P2_CHECKBOX) )
    {
    x = get_double_from_ebox( CalcWindow, SWEEP_TO_FILL_CAVITY_TBOX );
    CurrentPart.runner_fill_distance = (float) x;

    x += get_double_from_ebox( CalcWindow, DIST_TO_FILL_RUNNER_TBOX );
    CurrentPart.sleeve_fill_distance = (float) x;
    }

if ( is_checked(CalcWindow, SYNC_PLUNGER_DIAMETER_XBOX) )
    {
    x = get_double_from_ebox( CalcWindow, PLUNGER_DIAMETER_EBOX );
    CurrentPart.plunger_diameter = (float) x;
    }

boolean_to_ini( EditPartIniFile, ConfigSection, SyncPlungerDiameterKey, SyncPlungerDiameter );
}

/***********************************************************************
*                            UPDATE_DENSITY                            *
***********************************************************************/
static void update_density( void )
{
int32 i;
LISTBOX_CLASS lb;

lb.init( CalcWindow, DENSITY_CBOX );
i = lb.selected_index();
if ( i >= 0 && i< DensityUnitsId.count() )
    {
    CurrentDensityIndex = i;
    set_text( CalcWindow, DENSITY_TBOX, ascii_double(Density[i]) );

    /*
    -----
    Units
    ----- */
    set_text( CalcWindow, DENSITY_UNITS_TBOX, units_name(DensityUnitsId[i]) );
    }
}

/***********************************************************************
*                      CHECK_SAVE_PART_SETUP_TBOX                      *
***********************************************************************/
static void check_save_part_setup_tbox()
{
BOOLEAN need_save_message;
BOOLEAN tbox_is_visible;
HWND    w;

w = GetDlgItem( CalcWindow, SAVE_PART_SETUP_TBOX );

tbox_is_visible = FALSE;
if ( IsWindowVisible(w) )
    tbox_is_visible = TRUE;

need_save_message = FALSE;
if ( SyncPlungerDiameter || is_checked(CalcWindow, COPY_P1_P2_CHECKBOX) )
    need_save_message = TRUE;

if ( tbox_is_visible != need_save_message )
    show_window( w, need_save_message );
}

/***********************************************************************
*                             FILL_CONTROLS                            *
***********************************************************************/
static void fill_controls()
{
DB_TABLE t;
UINT     id;
int32    i;
int32    n;
LISTBOX_CLASS lb;
TCHAR current_density_name[DENSITY_NAME_LEN+1];
TCHAR name[DENSITY_NAME_LEN+1];

FILE_CLASS f;
TCHAR    * cp;

ProgramIsUpdating = true;

SyncPlungerDiameter = boolean_from_ini( EditPartIniFile, ConfigSection, SyncPlungerDiameterKey );
set_checkbox( CalcWindow, SYNC_PLUNGER_DIAMETER_XBOX, SyncPlungerDiameter );
check_save_part_setup_tbox();

f.open_for_read( calc_file_name(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) );
id = IN_UNITS_RADIO;
CurrentDistanceUnit = INCH_UNITS;
CurrentAreaUnit     = IN_SQUARED_UNITS;
CurrentVolumeUnit   = IN_CUBED_UNITS;

cp = f.readline();
if ( cp )
    lstrcpy( current_density_name, cp );
else
    *current_density_name = NullChar;

cp = f.readline();
if ( cp )
    {
    n = asctoint32( cp );
    switch ( n )
        {
        case CM_UNITS:
            id = CM_UNITS_RADIO;
            break;

        case MM_UNITS:
            id = MM_UNITS_RADIO;
            break;
        }
    }

CheckRadioButton( CalcWindow, IN_UNITS_RADIO, MM_UNITS_RADIO, id );
update_distance_unit_labels();

/*
---------------------------------------------------------
Fill the pressure labels. This doesn't change for a part,
but I do it after getting the distance units.
--------------------------------------------------------- */
CurrentPressureUnit = CurrentPart.pressure_units;
if ( CurrentPressureUnit != PSI_UNITS && CurrentPressureUnit != BAR_UNITS && CurrentPressureUnit != KG_PER_CM2_UNITS )
    {
    if ( CurrentDistanceUnit == INCH_UNITS )
        CurrentPressureUnit = PSI_UNITS;
    else
        CurrentPressureUnit = BAR_UNITS;
    }

cp = units_name( CurrentPressureUnit );
for ( i=0; i<NofPressureLabels; i++ )
    set_text( CalcWindow, PressureLabel[i], cp );

id                  = IPS_UNITS_RADIO;
CurrentVelocityUnit = IPS_UNITS;
cp = f.readline();
if ( cp )
    {
    n = asctoint32( cp );
    switch ( n )
        {
        case FPM_UNITS:
            id = FPM_UNITS_RADIO;
            break;

        case CMPS_UNITS:
            id = CMPS_UNITS_RADIO;
            break;

        case MPS_UNITS:
            id = MPS_UNITS_RADIO;
            break;
        }
    }

CheckRadioButton( CalcWindow, IPS_UNITS_RADIO, MPS_UNITS_RADIO, id );
update_velocity_unit_labels();

id = LBS_UNITS_RADIO;
CurrentWeightUnit   = LBS_UNITS;

cp = f.readline();
if ( cp )
    {
    n = asctoint32( cp );
    if ( n == KG_UNITS )
        id = KG_UNITS_RADIO;
    }

CheckRadioButton( CalcWindow, KG_UNITS_RADIO, LBS_UNITS_RADIO, id );
update_weight_unit_labels();

for ( n=0; n<NofSaveBoxes; n++ )
    {
    cp = f.readline();
    if ( !cp )
        break;
    set_text( CalcWindow, SaveBoxes[n], cp );
    }

f.close();

if ( SyncPlungerDiameter )
    set_text( CalcWindow, PLUNGER_DIAMETER_EBOX, ascii_float(CurrentPart.plunger_diameter) );

lb.init( CalcWindow, DENSITY_CBOX );
lb.empty();

if ( t.open( density_dbname(), DENSITY_RECLEN, PFL ) )
    {
    n = t.nof_recs();
    if ( n > 0 )
        {
        Density.init( n );
        DensityUnitsId.init( n );
        }
    n = 0;
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( name, DENSITY_NAME_OFFSET, DENSITY_NAME_LEN );
        if ( *current_density_name == NullChar )
            lstrcpy( current_density_name, name );
        if ( strings_are_equal(current_density_name, name) )
            CurrentDensityIndex = n;
        lb.add( name );
        Density[n]        = t.get_double( DENSITY_LIQUID_OFFSET );
        DensityUnitsId[n] = t.get_long( DENSITY_UNITS_OFFSET );
        n++;
        }
    t.close();
    }

lb.setcursel( CurrentDensityIndex );
update_density();

ProgramIsUpdating = false;
}

/***********************************************************************
*                          GET_TON_CONVERSION                          *
***********************************************************************/
static void get_ton_conversion( void )
{
STRING_CLASS s;

s = get_ini_string( EditPartIniFile, ConfigSection, LbsPerTonKey );
if ( !unknown(s.text()) )
    LbsPerTon = extdouble( s.text() );
}

/***********************************************************************
*                   CHECK_SYNC_PLUNGER_DIAMETER_XBOX                   *
***********************************************************************/
static void check_sync_plunger_diameter_xbox()
{
STRING_CLASS s;

SyncPlungerDiameter = is_checked( CalcWindow, SYNC_PLUNGER_DIAMETER_XBOX );
if ( SyncPlungerDiameter )
    {
    s = double( CurrentPart.plunger_diameter );
    if ( s.isempty() )
        s = TEXT("0.0" );
    s.set_text( CalcWindow, PLUNGER_DIAMETER_EBOX );
    update_calculations();
    }

check_save_part_setup_tbox();
}

/***********************************************************************
*                           SHUTDOWN_CALCULATOR                        *
***********************************************************************/
static void shutdown_calculator( void )
{
HWND w;

if ( CalcWindow )
    {
    w = CalcWindow;
    CalcWindow = 0;
    EndDialog( w, 0 );
    }
}

/***********************************************************************
*                           CALCULATOR_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK calculator_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CalcWindow = hWnd;
        if ( FirstTime )
            {
            get_ton_conversion();
            FirstTime = FALSE;
            }
        fill_controls();
        update_density();
        update_calculations();
        break;

    case WM_NEW_DATA:
        fill_controls();
        update_density();
        update_calculations();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, CALCULATOR_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IN_UNITS_RADIO:
            case CM_UNITS_RADIO:
            case MM_UNITS_RADIO:
                update_distance_unit_labels();
                update_calculations();
                return TRUE;

            case IPS_UNITS_RADIO:
            case FPM_UNITS_RADIO:
            case CMPS_UNITS_RADIO:
            case MPS_UNITS_RADIO:
                update_velocity_unit_labels();
                update_calculations();
                return TRUE;

            case LBS_UNITS_RADIO:
            case KG_UNITS_RADIO:
                update_weight_unit_labels();
                update_calculations();
                return TRUE;

            case DENSITY_CBOX:
                if ( cmd == CBN_SELCHANGE && !ProgramIsUpdating )
                    {
                    update_density();
                    update_calculations();
                    }
                return TRUE;

            case PLUNGER_DIAMETER_EBOX:
            case DRY_SHOT_LENGTH_EBOX:
            case BISCUIT_LEN_EBOX:
            case CYLINDER_DIAMETER_EBOX:
            case CASTING_WEIGHT_EBOX:
            case OVERFLOW_WEIGHT_EBOX:
            case WEIGHT_R_AND_B_EBOX:
            case VEL_DURING_FILL_EBOX:
            case GATE_AREA_EBOX:
            case INTENS_PRESSURE_EBOX:
            case AREA_CAST_OVRFLW_EBOX:
                if ( cmd == EN_CHANGE && !ProgramIsUpdating )
                    {
                    update_calculations();
                    }
                return TRUE;

            case FILLTIME_EBOX:
                if ( cmd == EN_CHANGE && !ProgramIsUpdating )
                    do_filltime_change();
                return TRUE;

            case SYNC_PLUNGER_DIAMETER_XBOX:
                check_sync_plunger_diameter_xbox();
                if ( is_checked(CalcWindow, SYNC_PLUNGER_DIAMETER_XBOX) )
                    {
                    }
                return TRUE;

            case GATE_VELOCITY_EBOX:
                if ( cmd == EN_CHANGE && !ProgramIsUpdating )
                    do_gate_velocity_change();
                return TRUE;

            case EDIT_DENSITIES_BUTTON:
                if ( !DensityDialogWindow )
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("DENSITY_DIALOG"),
                        hWnd,
                        (DLGPROC) density_dialog_proc );
                    }
                return TRUE;

            case COPY_P1_P2_CHECKBOX:
                check_save_part_setup_tbox();
                return TRUE;

            case IDOK:
                update_calculations();
                return TRUE;

            case CALC_SAVE_CHANGES_BUTTON:
                save_calc_changes();
                PostMessage( MainDialogWindow, WM_NEW_DATA, 0, 0L );
            case IDCANCEL:
                shutdown_calculator();
                return TRUE;
            }
        break;

    case WM_CLOSE:
        shutdown_calculator();
        return TRUE;

    }

return FALSE;
}
